#include "user_interface.h"
#include <stdio.h>
#include <string.h>

void init_display()
{
    // Initialize the SPI0 of Raspberry Pi
    // This example will use SPI0 at 4MHz.
    spi_init(SPI_PORT, 4000 * 1000);

    // Configure GPIO pins for SPI communication
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);

    // Initialize control pins
    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_init(LCD_DC); // Data/Command selection pin
    gpio_set_dir(LCD_DC, GPIO_OUT);

    // Initialize the LCD
    ST7735_Init();
}

/*########################### Home Interface ###########################*/

static bool work = true;
static uint8_t progress_bar_time;
// Draw the progress bar outline only once
static bool is_outline_drawn = false;

void home_view(time_s *time)
{
    // Clear the screen and draw the initial view
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(8, 8, "WORK", Font_11x18, ST7735_WHITE, ST7735_BLACK);

    // Draw session indicator
    char session_indicator[5];
    sprintf(session_indicator, "%02d/%02d", time->currentSession, time->sessions);
    ST7735_DrawString(3, BUTTONS_Y, session_indicator, Font_7x10, ST7735_WHITE, ST7735_BLACK);

    // Draw menu label
    ST7735_DrawString(95, BUTTONS_Y, "MENU", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    // Initially, draw "START" at the beginning
    ST7735_DrawString(47, BUTTONS_Y, "START", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    ST7735_DrawRect(PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, ST7735_WHITE);

    update_minutes(time->currentMinute);
    update_seconds(time->currentSecond);
    progress_bar_time = time->workDuration;
    update_progress_bar((int)(((progress_bar_time * 60) - ((time->currentMinute * 60) + time->currentSecond)) * 100 / (progress_bar_time * 60)));
}

void update_pause_resume_display(bool *startWork)
{
    *startWork = !*startWork;
    const char *button_text = *startWork ? "PAUSE" : "RESUME";
    // Clear the previous text by overwriting it with the background color (black)
    ST7735_FillRectangle(47, BUTTONS_Y, 48, 10, ST7735_BLACK);
    ST7735_DrawString(47, BUTTONS_Y, button_text, Font_7x10, ST7735_WHITE, ST7735_BLACK);
}

bool update_time(time_s *time)
{
    if ((time->currentSecond == 0) && (time->currentMinute != 0))
    {
        time->currentSecond = 59;
        time->currentMinute--;
        update_minutes(time->currentMinute);
    }
    else if (((time->currentSecond == 0) && (time->currentMinute == 0)))
    {
        if (time->currentSession >= time->sessions)
        {
            work = true;
            time->currentMinute = time->workDuration;
            time->currentSecond = 0;
            time->currentSession = 1;
            return true;
        }

        work = !work;
        is_outline_drawn = false;
        // erase the progress bar
        ST7735_FillRectangle(PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, ST7735_BLACK);

        if (work)
        {
            progress_bar_time = time->workDuration;

            time->currentMinute = time->workDuration;
            time->currentSecond = 0;

            ST7735_FillRectangle(8, 8, 58, 20, ST7735_BLACK);
            ST7735_DrawString(8, 8, "WORK", Font_11x18, ST7735_WHITE, ST7735_BLACK);

            time->currentSession++;
            char session_indicator[2];
            sprintf(session_indicator, "%02d", time->currentSession);
            ST7735_FillRectangle(3, BUTTONS_Y, 10, 10, ST7735_BLACK);
            ST7735_DrawString(3, BUTTONS_Y, session_indicator, Font_7x10, ST7735_WHITE, ST7735_BLACK);
        }
        else
        {
            ST7735_FillRectangle(8, 8, 58, 20, ST7735_BLACK);
            ST7735_DrawString(8, 8, "BREAK", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            if (time->currentSession % 4 == 0)
            {
                progress_bar_time = time->longBreak;
                time->currentMinute = time->longBreak;
                time->currentSecond = 0;
            }
            else
            {
                progress_bar_time = time->shortBreak;
                time->currentMinute = time->shortBreak;
                time->currentSecond = 0;
            }
        }
    }
    else
    {
        time->currentSecond--;
    }
    // Update the time display if changed
    update_seconds(time->currentSecond);

    // Update the progress bar
    update_progress_bar((int)(((progress_bar_time * 60) - ((time->currentMinute * 60) + time->currentSecond)) * 100 / (progress_bar_time * 60)));
    return false;
}

void update_minutes(uint8_t minute)
{
    char timeString[3];

    sprintf(timeString, "%02d:", minute);

    // Erase the old time by drawing it in black
    ST7735_FillRectangle(TIME_X, TIME_Y, 33, 10, ST7735_BLACK);
    // Draw the new time in white
    ST7735_DrawString(TIME_X, TIME_Y, timeString, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void update_seconds(uint8_t second)
{
    char timeString[2];

    sprintf(timeString, "%02d", second);

    // Erase the old time by drawing it in black
    ST7735_FillRectangle(TIME_X + 35, TIME_Y, 48, 10, ST7735_BLACK);
    // Draw the new time in white
    ST7735_DrawString(TIME_X + 35, TIME_Y, timeString, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void update_progress_bar(int completion_percentage)
{
    // Ensure percentage is within valid range
    if (completion_percentage < 0)
        completion_percentage = 0;
    if (completion_percentage > 100)
        completion_percentage = 100;

    // Draw the progress bar outline only once
    if (!is_outline_drawn)
    {
        ST7735_DrawRect(PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, ST7735_WHITE);
        is_outline_drawn = true;
    }

    // Calculate the width of the filled portion of the progress bar
    int filled_width = (int)((float)completion_percentage * PROGRESS_BAR_WIDTH / 100.0f);

    // Fill the progress bar
    ST7735_FillRectangle(PROGRESS_BAR_X, PROGRESS_BAR_Y, filled_width, PROGRESS_BAR_HEIGHT, ST7735_WHITE);
}

/*########################### Settings Interface ###########################*/

// Array defining the menu order
menu_items menu_items_array[] = {SESSIONS_Y, WORKDURATION_Y, SHORTDURATION_Y, LONGBREAK_Y};
#define MENU_ITEM_COUNT (sizeof(menu_items_array) / sizeof(menu_items_array[0]))

uint8_t previous_selection_index = 0; // Keep track of the previous selection
uint8_t current_selection_index = 0;

// Function to display the settings view
void settings_view()
{

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(7, 16, "Settings", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawLineThick(6, 35, 117, 35, ST7735_WHITE, 2);
    ST7735_DrawString(12, 46, "Sessions", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 76, "Work Duration", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 106, "Short Break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 136, "Long Break", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    selection_arrow(menu_items_array[previous_selection_index], menu_items_array[current_selection_index]);
}

uint8_t settings_up()
{
    previous_selection_index = current_selection_index;
    // Move to the previous menu item
    if (current_selection_index == 0)
    {
        current_selection_index = MENU_ITEM_COUNT - 1; // Wrap around to the last item
    }
    else
    {
        current_selection_index--;
    }
    // Update the selection arrow
    selection_arrow(menu_items_array[previous_selection_index], menu_items_array[current_selection_index]);
    return current_selection_index;
}

uint8_t settings_down()
{
    previous_selection_index = current_selection_index;
    // Move to the next menu item
    current_selection_index = (current_selection_index + 1) % MENU_ITEM_COUNT;

    // Update the selection arrow
    selection_arrow(menu_items_array[previous_selection_index], menu_items_array[current_selection_index]);

    return current_selection_index;
}

void selection_arrow(menu_items previous_delta, menu_items current_delta)
{
    const int x1 = 6;
    const int x2 = 10;
    const int x3 = 6;
    const int initial_y1 = 38;
    const int initial_y2 = 40;
    const int initial_y3 = 43;

    // Erase the arrow at the previous position by drawing over it with the background color
    ST7735_FillTriangle(x1, initial_y1 + previous_delta, x2, initial_y2 + previous_delta, x3, initial_y3 + previous_delta, ST7735_BLACK);

    // Draw the arrow at the new position
    ST7735_FillTriangle(x1, initial_y1 + current_delta, x2, initial_y2 + current_delta, x3, initial_y3 + current_delta, ST7735_WHITE);
}

/*########################### Sessions Interface ###########################*/

uint8_t session;

// Function to display the Sessions view
void sessions_view(time_s *time)
{
    char sessionNumber[2];
    sprintf(sessionNumber, "%02d", time->sessions);

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(20, 16, "Sessions", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawLineThick(6, 35, 117, 35, ST7735_WHITE, 2);
    ST7735_DrawString(12, 140, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(92, 140, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(50, 80, sessionNumber, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    session = time->sessions;
}

void sessions_up()
{

    if (session >= 99)
        return;

    session++;

    char sessionNumber[2];
    sprintf(sessionNumber, "%02d", session);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(50, 80, sessionNumber, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void sessions_down()
{

    if (session <= 0)
        return;

    session--;

    char sessionNumber[2];
    sprintf(sessionNumber, "%02d", session);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(50, 80, sessionNumber, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void update_sessions(time_s *time)
{
    time->sessions = session;
    time->currentMinute = time->workDuration;
    time->currentSecond = 0;
    time->currentSession = 1;
    work = true;
}

/*########################### Work Duration Interface ###########################*/

uint8_t work_duration;

// Function to display the Work Duration view
void work_duration_view(time_s *time)
{
    char work_duration_str[6];
    sprintf(work_duration_str, "%02d min", time->workDuration);

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(20, 16, "Work duration", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawLineThick(6, 35, 117, 35, ST7735_WHITE, 2);
    ST7735_DrawString(12, 140, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(92, 140, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(30, 80, work_duration_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    work_duration = time->workDuration;
}

void work_duration_up()
{

    if (work_duration >= 60)
        return;

    work_duration++;

    char work_duration_str[6];
    sprintf(work_duration_str, "%02d min", work_duration);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(30, 80, work_duration_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void work_duration_down()
{

    if (work_duration <= 0)
        return;

    work_duration--;

    char work_duration_str[6];
    sprintf(work_duration_str, "%02d min", work_duration);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(30, 80, work_duration_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void update_work_duration(time_s *time)
{
    time->workDuration = work_duration;
    time->currentMinute = work_duration;
    time->currentSecond = 0;
    time->currentSession = 1;
    work = true;
}

/*########################### short break Interface ###########################*/

uint8_t short_break;

// Function to display the short break view
void short_break_view(time_s *time)
{
    char short_break_str[6];
    sprintf(short_break_str, "%02d min", time->shortBreak);

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(20, 16, "Short break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawLineThick(6, 35, 117, 35, ST7735_WHITE, 2);
    ST7735_DrawString(12, 140, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(92, 140, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(30, 80, short_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    short_break = time->shortBreak;
}

void short_break_up()
{

    if (short_break >= 60)
        return;

    short_break++;

    char short_break_str[6];
    sprintf(short_break_str, "%02d min", short_break);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(30, 80, short_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void short_break_down()
{
    if (short_break <= 0)
        return;

    short_break--;

    char short_break_str[6];
    sprintf(short_break_str, "%02d min", short_break);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(30, 80, short_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void short_break_duration(time_s *time)
{
    time->shortBreak = short_break;
    time->currentMinute = time->workDuration;
    time->currentSecond = 0;
    time->currentSession = 1;
    work = true;
}

/*########################### long break Interface ###########################*/

uint8_t long_break;

// Function to display the long break view
void long_break_view(time_s *time)
{
    char long_break_str[6];
    sprintf(long_break_str, "%02d min", time->longBreak);

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(20, 16, "long break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawLineThick(6, 35, 117, 35, ST7735_WHITE, 2);
    ST7735_DrawString(12, 140, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(92, 140, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(30, 80, long_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    long_break = time->longBreak;
}

void long_break_up()
{

    if (long_break >= 60)
        return;

    long_break++;

    char long_break_str[6];
    sprintf(long_break_str, "%02d min", long_break);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(30, 80, long_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void long_break_down()
{
    if (long_break <= 0)
        return;

    long_break--;

    char long_break_str[6];
    sprintf(long_break_str, "%02d min", long_break);

    ST7735_FillRectangle(50, 80, 48, 10, ST7735_BLACK);
    ST7735_DrawString(30, 80, long_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void long_break_duration(time_s *time)
{
    time->longBreak = long_break;
    time->currentMinute = time->workDuration;
    time->currentSecond = 0;
    time->currentSession = 1;
    work = true;
}

/*########################### TASKS DONE Interface ###########################*/

// Function to display the TASKS DONE view
void tasks_done_view()
{

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(30, 80, "Congrats", Font_7x10, ST7735_WHITE, ST7735_BLACK);
}