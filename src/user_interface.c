#include "user_interface.h"
#include <stdio.h>
#include <string.h>
#include "testimg.h"

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
    if (work)
        ST7735_DrawString(115, 110, "WORK", Font_7x10, ST7735_CYAN, ST7735_BLACK);
    else
        ST7735_DrawString(115, 110, "BREAK", Font_7x10, ST7735_YELLOW, ST7735_BLACK);

    // Draw session indicator
    char session_indicator[5];
    sprintf(session_indicator, "%02d/%02d", time->currentSession, time->sessions);
    ST7735_DrawString(60, 110, session_indicator, Font_7x10, ST7735_CYAN, ST7735_BLACK);

    // Initially, draw "START" at the beginning
    ST7735_DrawString(8, 110, "START", Font_7x10, ST7735_CYAN, ST7735_BLACK);

    update_minutes(time->currentMinute);
    update_seconds(time->currentSecond);
    progress_bar_time = time->workDuration;
}

void update_pause_resume_display(bool *startWork)
{
    *startWork = !*startWork;
    const char *button_text = *startWork ? "PAUSE" : "RESUME";
    // Clear the previous text by overwriting it with the background color (black)
    ST7735_FillRectangle(8, 110, 48, 10, ST7735_BLACK);
    ST7735_DrawString(8, 110, button_text, Font_7x10, ST7735_CYAN, ST7735_BLACK);
}

bool update_time(time_s *time, uint32_t *percentage, bool *work_ptr)
{
    if ((time->currentSecond == 0) && (time->currentMinute != 0))
    {
        time->currentSecond = 59;
        time->currentMinute--;
        update_minutes(time->currentMinute);
    }
    else if (((time->currentSecond == 0) && (time->currentMinute == 0)))
    {
        interrupt_flag = 0;
        if (time->currentSession >= time->sessions)
        {
            work = true;
            time->currentMinute = time->workDuration;
            time->currentSecond = 0;
            time->currentSession = 1;
            return true;
        }

        work = !work;
        *work_ptr = work;
        is_outline_drawn = false;

        if (work)
        {
            progress_bar_time = time->workDuration;

            time->currentMinute = time->workDuration;
            time->currentSecond = 0;

            ST7735_FillRectangle(115, 110, 40, 20, ST7735_BLACK);
            ST7735_DrawString(115, 110, "WORK", Font_7x10, ST7735_CYAN, ST7735_BLACK);

            time->currentSession++;
            char session_indicator[2];
            sprintf(session_indicator, "%02d", time->currentSession);
            ST7735_FillRectangle(60, 110, 10, 10, ST7735_BLACK);
            ST7735_DrawString(60, 110, session_indicator, Font_7x10, ST7735_CYAN, ST7735_BLACK);
        }
        else
        {
            ST7735_FillRectangle(115, 110, 40, 20, ST7735_BLACK);
            ST7735_DrawString(115, 110, "BREAK", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
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
        if (((time->currentSecond == 10) && (time->currentMinute == 0)))
            interrupt_flag = 3;
        time->currentSecond--;
    }
    // Update the time display if changed
    update_seconds(time->currentSecond);

    *percentage = (int)(((progress_bar_time * 60) - ((time->currentMinute * 60) + time->currentSecond)) * 100 / (progress_bar_time * 60));
    return false;
}

void update_minutes(uint8_t minute)
{
    char timeString[3];

    sprintf(timeString, "%02d:", minute);

    // Erase the old time by drawing it in black
    ST7735_FillRectangle(40, 70, 33, 10, ST7735_BLACK);
    // Draw the new time in white
    ST7735_DrawString(40, 70, timeString, Font_16x26, ST7735_WHITE, ST7735_BLACK);
}

void update_seconds(uint8_t second)
{
    char timeString[2];

    sprintf(timeString, "%02d", second);

    // Erase the old time by drawing it in black
    ST7735_FillRectangle(40 + 50, 70, 48, 10, ST7735_BLACK);
    // Draw the new time in white
    ST7735_DrawString(40 + 50, 70, timeString, Font_16x26, ST7735_WHITE, ST7735_BLACK);
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
        ST7735_DrawRect(20, 100, 120, 20, ST7735_WHITE);
        is_outline_drawn = true;
    }

    // Calculate the width of the filled portion of the progress bar
    int filled_width = (int)((float)completion_percentage * 120 / 100.0f);

    // Fill the progress bar
    ST7735_FillRectangle(21, 100, filled_width, 19, ST7735_GREEN);
}

/*########################### Settings Interface ###########################*/

// Array defining the menu order
menu_items menu_items_array[] = {SESSIONS_Y, WORKDURATION_Y, SHORTDURATION_Y, LONGBREAK_Y, PLAY_Y};
#define MENU_ITEM_COUNT (sizeof(menu_items_array) / sizeof(menu_items_array[0]))

uint8_t previous_selection_index = 0; // Keep track of the previous selection
uint8_t current_selection_index = 0;

// Function to display the settings view
void settings_view()
{

    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(30, 8, "Settings", Font_11x18, ST7735_CYAN, ST7735_BLACK);
    ST7735_DrawLineThick(30, 30, 120, 30, ST7735_MAGENTA, 2);
    ST7735_DrawString(12, 50, "Tasks", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 65, "Work Duration", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 80, "Short Break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 95, "Long Break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(12, 110, "Play", Font_7x10, ST7735_WHITE, ST7735_BLACK);
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
    const int x1 = 5;
    const int x2 = 9;
    const int x3 = 5;
    const int initial_y1 = 1;
    const int initial_y2 = 3;
    const int initial_y3 = 5;

    // Erase the arrow at the previous position by drawing over it with the background color
    ST7735_FillTriangle(x1, initial_y1 + previous_delta, x2, initial_y2 + previous_delta, x3, initial_y3 + previous_delta, ST7735_BLACK);

    // Draw the arrow at the new position
    ST7735_FillTriangle(x1, initial_y1 + current_delta, x2, initial_y2 + current_delta, x3, initial_y3 + current_delta, ST7735_MAGENTA);
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
    ST7735_DrawString(50, 16, "Tasks", Font_11x18, ST7735_CYAN, ST7735_BLACK);
    ST7735_DrawLineThick(50, 35, 105, 35, ST7735_MAGENTA, 2);
    ST7735_DrawString(130, 110, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(10, 110, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(65, 60, sessionNumber, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    session = time->sessions;
}

void sessions_up()
{

    if (session >= 99)
        return;

    session++;

    char sessionNumber[2];
    sprintf(sessionNumber, "%02d", session);

    ST7735_FillRectangle(65, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(65, 60, sessionNumber, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void sessions_down()
{

    if (session <= 1)
        return;

    session--;

    char sessionNumber[2];
    sprintf(sessionNumber, "%02d", session);

    ST7735_FillRectangle(65, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(65, 60, sessionNumber, Font_11x18, ST7735_WHITE, ST7735_BLACK);
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
    ST7735_DrawString(10, 16, "Work duration", Font_11x18, ST7735_CYAN, ST7735_BLACK);
    ST7735_DrawLineThick(10, 35, 150, 35, ST7735_MAGENTA, 2);
    ST7735_DrawString(130, 110, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(10, 110, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(45, 60, work_duration_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    work_duration = time->workDuration;
}

void work_duration_up()
{

    if (work_duration >= 60)
        return;

    work_duration++;

    char work_duration_str[6];
    sprintf(work_duration_str, "%02d min", work_duration);

    ST7735_FillRectangle(45, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(45, 60, work_duration_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void work_duration_down()
{

    if (work_duration <= 1)
        return;

    work_duration--;

    char work_duration_str[6];
    sprintf(work_duration_str, "%02d min", work_duration);

    ST7735_FillRectangle(45, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(45, 60, work_duration_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
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
    ST7735_DrawString(15, 16, "Short break", Font_11x18, ST7735_CYAN, ST7735_BLACK);
    ST7735_DrawLineThick(15, 35, 135, 35, ST7735_MAGENTA, 2);
    ST7735_DrawString(130, 110, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(10, 110, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(45, 60, short_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    short_break = time->shortBreak;
}

void short_break_up()
{

    if (short_break >= 60)
        return;

    short_break++;

    char short_break_str[6];
    sprintf(short_break_str, "%02d min", short_break);

    ST7735_FillRectangle(45, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(45, 60, short_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void short_break_down()
{
    if (short_break <= 1)
        return;

    short_break--;

    char short_break_str[6];
    sprintf(short_break_str, "%02d min", short_break);

    ST7735_FillRectangle(45, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(45, 60, short_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
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
    ST7735_DrawString(20, 16, "long break", Font_11x18, ST7735_CYAN, ST7735_BLACK);
    ST7735_DrawLineThick(22, 35, 130, 35, ST7735_MAGENTA, 2);
    ST7735_DrawString(130, 110, "[+]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(10, 110, "[-]", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    ST7735_DrawString(45, 60, long_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    long_break = time->longBreak;
}

void long_break_up()
{

    if (long_break >= 60)
        return;

    long_break++;

    char long_break_str[6];
    sprintf(long_break_str, "%02d min", long_break);

    ST7735_FillRectangle(45, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(45, 60, long_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void long_break_down()
{
    if (long_break <= 1)
        return;

    long_break--;

    char long_break_str[6];
    sprintf(long_break_str, "%02d min", long_break);

    ST7735_FillRectangle(45, 60, 48, 10, ST7735_BLACK);
    ST7735_DrawString(45, 60, long_break_str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
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
    ST7735_DrawString(30, 60, "Congrats", Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

/*########################### EYES EXPRESSIONS #################################*/

uint8_t blink_fps = 0;
uint8_t sad_fps = 0;
uint8_t happy_fps = 0;
uint8_t angry_fps = 0;
uint8_t stars_fps1 = 0;
uint8_t stars_fps2 = 0;
faces_e face = EYES;

void eyes_normal_blink()
{
    if (face == ANGRY)
    {
        angry_face();
        return;
    }
    else if (face == STARS)
    {
        stars_face();
        return;
    }

    switch (blink_fps)
    {
    case 0:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[0]);
        break;
    case 41:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[1]);
        break;
    case 42:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[2]);
        break;
    case 44:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[3]);
        break;
    case 45:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[4]);
        break;
    case 46:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[5]);
        break;
    case 47:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[4]);
        break;
    case 48:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[3]);
        break;
    case 49:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[2]);
        break;
    case 50:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[1]);
        break;
    default:
        break;
    }
    blink_fps++;
    if (blink_fps > 50)
        blink_fps = 0;
}

void eyes_sad_blink()
{
    if (face == ANGRY)
    {
        angry_face();
        return;
    }
    else if (face == STARS)
    {
        stars_face();
        return;
    }

    switch (sad_fps)
    {
    case 0:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[0]);
        break;
    case 1:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[6]);
        break;
    case 2:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[7]);
        break;
    case 3:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[8]);
        break;
    case 4:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[9]);
        break;
    case 5:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[10]);
        break;
    case 48:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[11]);
        break;
    case 49:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[12]);
        break;
    case 50:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[11]);
        break;
    default:
        break;
    }
    sad_fps++;
    if (sad_fps > 50)
        sad_fps = 5;
}

void eyes_happy_blink()
{
    if (face == ANGRY)
    {
        angry_face();
        return;
    }
    else if (face == STARS)
    {
        stars_face();
        return;
    }

    switch (happy_fps)
    {
    case 0:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[0]);
        break;
    case 1:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[13]);
        break;
    case 2:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[14]);
        break;
    case 3:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[15]);
        break;
    case 4:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[16]);
        break;
    case 48:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[17]);
        break;
    case 49:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[18]);
        break;
    case 50:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)image[17]);
        break;
    default:
        break;
    }
    happy_fps++;
    if (happy_fps > 50)
        happy_fps = 4;
}

void angry_face()
{
    switch (angry_fps)
    {
    case 0:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)angry[0]);
        break;
    case 30:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)angry[1]);
        break;
    case 60:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)angry[2]);
        break;
    default:
        break;
    }
    angry_fps++;
    if (angry_fps > 90)
    {
        angry_fps = 0;
        face = EYES;
    }
}

void stars_face()
{
    switch (stars_fps1)
    {
    case 0:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[0]);
        break;
    case 1:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[1]);
        break;
    case 2:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[2]);
        break;
    case 3:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[3]);
        break;
    case 4:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[4]);
        break;
    case 5:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[5]);
        break;
    case 6:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[6]);
        break;
    case 7:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[7]);
        break;
    case 8:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[8]);
        break;
    case 9:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[9]);
        break;
    case 10:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[10]);
        break;
    case 11:
        ST7735_DrawImage(45, 10, 72, 44, (uint16_t *)stars[11]);
        break;

    default:
        break;
    }
    stars_fps1++;
    if (stars_fps1 > 11)
    {
        stars_fps1 = 0;
        stars_fps2++;
        if (stars_fps2 > 5)
        {
            stars_fps2 = 0;
            face = EYES;
        }
    }
}

/*########################### SIMON GAME #################################*/

void simon_game_view()
{
    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);

    const char *feedback_text = "Simon Game";
    int feedback_width = strlen(feedback_text) * 6; // Assuming Font_7x10
    int feedback_x = (160 - feedback_width) / 2;
    ST7735_DrawString(feedback_x, 15, feedback_text, Font_7x10, ST7735_BLACK, ST7735_CYAN);

    ST7735_DrawCircle(20, 85, 15, ST7735_CYAN);
    simon_game_yellow_off();

    ST7735_DrawCircle(60, 85, 15, ST7735_CYAN);
    simon_game_blue_off();

    ST7735_DrawCircle(100, 85, 15, ST7735_CYAN);
    simon_game_red_off();

    ST7735_DrawCircle(140, 85, 15, ST7735_CYAN);
    simon_game_green_off();
}

void simon_game_yellow_off()
{
    ST7735_FillCircle(20, 85, 14, ST7735_YELLOW_OFF);
}
void simon_game_blue_off()
{
    ST7735_FillCircle(60, 85, 14, ST7735_BLUE_OFF);
}
void simon_game_red_off()
{
    ST7735_FillCircle(100, 85, 14, ST7735_RED_OFF);
}
void simon_game_green_off()
{
    ST7735_FillCircle(140, 85, 14, ST7735_GREEN_OFF);
}
void simon_game_yellow()
{
    ST7735_FillCircle(20, 85, 14, ST7735_YELLOW);
}
void simon_game_blue()
{
    ST7735_FillCircle(60, 85, 14, ST7735_BLUE);
}
void simon_game_red()
{
    ST7735_FillCircle(100, 85, 14, ST7735_RED);
}
void simon_game_green()
{
    ST7735_FillCircle(140, 85, 14, ST7735_GREEN);
}

/*########################### GAME VIEW #################################*/

// Function to display the settings view
void games_view()
{
    // Draw static elements
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_FillRectangle(0, 0, 159, 64, ST7735_RED);
    ST7735_FillRectangle(0, 65, 159, 63, ST7735_BLUE);

    ST7735_DrawString(45, 32, "Simon Game", Font_7x10, ST7735_BLACK, ST7735_RED);
    ST7735_DrawString(30, 97, "Mastermind Game", Font_7x10, ST7735_BLACK, ST7735_BLUE);
}