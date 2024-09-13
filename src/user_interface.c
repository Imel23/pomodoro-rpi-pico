#include "user_interface.h"
#include <stdio.h>

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

void home_view(bool is_button_clicked, bool is_timer_paused, char *timer_state,
               int current_minutes, int current_seconds,
               int *previous_minutes, int *previous_seconds,
               int current_session, int total_sessions, int progress_percentage)
{

    static bool is_initial_state_drawn = false;

    if (!is_initial_state_drawn)
    {
        draw_initial_view(timer_state, current_session, total_sessions);
        is_initial_state_drawn = true; // Mark initial state as drawn
    }
    else if (is_button_clicked)
    {
        update_pause_resume_display(is_timer_paused);
    }

    // Update the time display if changed
    update_time_display(*previous_minutes, *previous_seconds, current_minutes, current_seconds);

    // Store the current time for the next update
    *previous_minutes = current_minutes;
    *previous_seconds = current_seconds;

    // Update the progress bar
    update_progress_bar(progress_percentage);
}

void draw_initial_view(char *timer_state, int current_session, int total_sessions)
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
}

void update_pause_resume_display(bool is_timer_paused)
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
}

void update_time_display(int previous_minutes, int previous_seconds, int current_minutes, int current_seconds)
{
    char previous_time_string[6], current_time_string[6];

    sprintf(previous_time_string, "%02d:%02d", previous_minutes, previous_seconds);
    sprintf(current_time_string, "%02d:%02d", current_minutes, current_seconds);

    // Only update the display if the time has changed
    if (previous_minutes != current_minutes || previous_seconds != current_seconds)
    {
        // Erase the old time by drawing it in black
        ST7735_DrawString(TIME_X, TIME_Y, previous_time_string, Font_11x18, ST7735_BLACK, ST7735_BLACK);
        // Draw the new time in white
        ST7735_DrawString(TIME_X, TIME_Y, current_time_string, Font_11x18, ST7735_WHITE, ST7735_BLACK);
    }
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

void settings_view()
{
}