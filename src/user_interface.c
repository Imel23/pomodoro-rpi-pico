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

void draw_initial_view(char *timer_state, uint8_t minutes, int current_session, int total_sessions)
{
    // Clear the screen and draw the initial view
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(8, 8, timer_state, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    // Draw session indicator
    char session_indicator[5];
    sprintf(session_indicator, "%02d/%02d", current_session, total_sessions);
    ST7735_DrawString(3, BUTTONS_Y, session_indicator, Font_7x10, ST7735_WHITE, ST7735_BLACK);

    // Draw menu label
    ST7735_DrawString(95, BUTTONS_Y, "CONF", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    // Initially, draw "START" at the beginning
    ST7735_DrawString(47, BUTTONS_Y, "START", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    ST7735_DrawRect(PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, ST7735_WHITE);

    update_minutes(minutes);

    update_seconds(0);
}

bool update_pause_resume_display(bool is_timer_paused)
{
    static bool previous_pause_state = false;
    static bool firstTime = true;

    if (firstTime)
    {
        ST7735_FillRectangle(47, BUTTONS_Y, 48, 10, ST7735_BLACK);
        ST7735_DrawString(47, BUTTONS_Y, "PAUSE", Font_7x10, ST7735_WHITE, ST7735_BLACK);
        firstTime = false;
    }
    // Update "PAUSE"/"RESUME" label based on pause state
    else if (is_timer_paused != previous_pause_state)
    {
        const char *button_text = is_timer_paused ? "RESUME" : "PAUSE";
        // Clear the previous text by overwriting it with the background color (black)
        ST7735_FillRectangle(47, BUTTONS_Y, 48, 10, ST7735_BLACK);
        ST7735_DrawString(47, BUTTONS_Y, button_text, Font_7x10, ST7735_WHITE, ST7735_BLACK);
    }
    previous_pause_state = is_timer_paused;

    return is_timer_paused;
}

void update_time(time_s *time)
{

    if ((time->currentSecond == 0) && (time->currentMinute != 0))
    {
        time->currentSecond = 59;
        time->currentMinute--;
        update_minutes(time->currentMinute);
    }
    else if (((time->currentSecond == 0) && (time->currentMinute == 0)))
    {
        // To DO !!!!
        time->currentMinute = time->workDuration;
    }
    else
    {
        time->currentSecond--;
    }
    // Update the time display if changed
    update_seconds(time->currentSecond);

    // Update the progress bar
    update_progress_bar((int)(((time->workDuration * 60) - ((time->currentMinute * 60) + time->currentSecond)) * 100 / (time->workDuration * 60)));
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
    static bool is_outline_drawn = false;
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
menu_items menu_items_array[] = {sessions, work_duration, short_duration, long_break};
#define MENU_ITEM_COUNT (sizeof(menu_items_array) / sizeof(menu_items_array[0]))
volatile int current_selection_index = 0;

// Function to display the settings view
void settings_view()
{
    static bool is_initial_state_drawn = false;

    if (!is_initial_state_drawn)
    {
        // Draw static elements
        ST7735_FillScreen(ST7735_BLACK);
        ST7735_DrawString(7, 16, "Settings", Font_11x18, ST7735_WHITE, ST7735_BLACK);
        ST7735_DrawLineThick(6, 35, 117, 35, ST7735_WHITE, 2);
        ST7735_DrawString(12, 46, "Sessions", Font_7x10, ST7735_WHITE, ST7735_BLACK);
        ST7735_DrawString(12, 76, "Work Duration", Font_7x10, ST7735_WHITE, ST7735_BLACK);
        ST7735_DrawString(12, 106, "Short Break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
        ST7735_DrawString(12, 136, "Long Break", Font_7x10, ST7735_WHITE, ST7735_BLACK);
        is_initial_state_drawn = true; // Mark initial state as drawn
    }
    handle_menu_navigation();
}

void handle_menu_navigation()
{
    static int previous_selection_index = 0; // Keep track of the previous selection

    // Check for button presses
    if (is_increase_pressed)
    {
        is_increase_pressed = false;
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
    }
    else if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        previous_selection_index = current_selection_index;
        // Move to the next menu item
        current_selection_index = (current_selection_index + 1) % MENU_ITEM_COUNT;
    }
    else
    {
        // No button press; previous and current selection remain the same
        previous_selection_index = current_selection_index;
    }
    printf("current selction index: %d\n", current_selection_index);
    // Update the selection arrow
    selection_arrow(menu_items_array[previous_selection_index], menu_items_array[current_selection_index]);
}

void selection_arrow(menu_items previous_delta, menu_items current_delta)
{
    const int x1 = 6;
    const int x2 = 9;
    const int x3 = 6;
    const int initial_y1 = 38;
    const int initial_y2 = 40;
    const int initial_y3 = 43;

    // Erase the arrow at the previous position by drawing over it with the background color
    ST7735_FillTriangle(x1, initial_y1 + previous_delta, x2, initial_y2 + previous_delta, x3, initial_y3 + previous_delta, ST7735_BLACK);

    // Draw the arrow at the new position
    ST7735_FillTriangle(x1, initial_y1 + current_delta, x2, initial_y2 + current_delta, x3, initial_y3 + current_delta, ST7735_WHITE);
}
