// user_interface.h

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "st7735.h"
#include "fonts.h"
#include "buttons_handler.h"
#include "time_handler.h"

// Screen dimensions and layout constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160
#define TIME_X 70
#define TIME_Y 8
#define PROGRESS_BAR_X 3
#define PROGRESS_BAR_Y 80
#define PROGRESS_BAR_WIDTH 122
#define PROGRESS_BAR_HEIGHT 18
#define BUTTONS_Y 147

/**
 * @brief Initialize the display hardware and SPI communication.
 */
void init_display(void);

/**
 * @brief Draw the initial view of the timer interface.
 *
 * @param timer_state Current state of the timer (e.g., "WORK", "REST")
 * @param minutes TO ADD !!!!!
 * @param current_session Current session number
 * @param total_sessions Total number of sessions
 */
void draw_initial_view(char *timer_state, uint8_t minutes, int current_session, int total_sessions);

/**
 * @brief Update the pause/resume button display on the screen.
 *
 * @param is_timer_paused Boolean flag indicating whether the timer is paused
 */
bool update_pause_resume_display(bool is_timer_paused);

/**
 * @brief Update the time display on the screen.
 *
 * @param minute current minute
 * @param second Current second
 */
void update_time(time_s *time);
void update_minutes(uint8_t minute);
void update_seconds(uint8_t second);

/**
 * @brief Update the progress bar on the screen.
 *
 * @param completion_percentage Progress percentage (0-100)
 */
void update_progress_bar(int completion_percentage);

/*########################### Settings Interface ###########################*/

// Menu items with associated Y positions
typedef enum menu_items
{
    sessions = 38,
    work_duration = 68,
    short_duration = 98,
    long_break = 128
} menu_items;

/**
 * @brief Display and navigate the settings menu.
 *
 * This function initializes the settings menu view and handles the
 * navigation between the menu items using up and down buttons.
 *
 */
void settings_view();

/**
 * @brief Handle the navigation of menu items using buttons.
 *
 * This function updates the current menu selection index based on the
 * button presses (up and down) and wraps around the menu items if the
 * beginning or end is reached. It also updates the selection arrow
 * on the screen.
 *
 */
void handle_menu_navigation();

/**
 * @brief Update the position of the selection arrow on the screen.
 *
 * This function erases the arrow from its previous position and draws it
 * at the new position to indicate the current menu selection.
 *
 * @param previous_delta The Y position of the previous menu item.
 * @param current_delta The Y position of the current menu item.
 */
void selection_arrow(menu_items previous_delta, menu_items current_delta);

#endif // USER_INTERFACE_H