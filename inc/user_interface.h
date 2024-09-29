// user_interface.h

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "st7735.h"
#include "fonts.h"
#include "buttons_handler.h"
#include "main.h"

// Screen dimensions and layout constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

// Menu items with associated Y positions
typedef enum menu_items
{
    SESSIONS_Y = 40,
    WORKDURATION_Y = 60,
    SHORTDURATION_Y = 80,
    LONGBREAK_Y = 100
} menu_items;

/**
 * @brief Initialize the display hardware and SPI communication.
 */
void init_display(void);

/**
 * @brief Draw the initial view of the timer interface.
 *
 * @param time TO ADD !!!!!
 */

void home_view(time_s *time);

/**
 * @brief Update the pause/resume button display on the screen.
 *
 * @param is_timer_paused Boolean flag indicating whether the timer is paused
 */
void update_pause_resume_display(bool *startWork);

/**
 * @brief Update the time display on the screen.
 *
 * @param minute current minute
 * @param second Current second
 */
bool update_time(time_s *time);
void update_minutes(uint8_t minute);
void update_seconds(uint8_t second);

/**
 * @brief Update the progress bar on the screen.
 *
 * @param completion_percentage Progress percentage (0-100)
 */
void update_progress_bar(int completion_percentage);

/*########################### Settings Interface ###########################*/

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
uint8_t settings_up();
uint8_t settings_down();
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

void sessions_view(time_s *time);
void sessions_up();
void sessions_down();
void update_sessions(time_s *time);

void work_duration_view(time_s *time);
void work_duration_up();
void work_duration_down();
void update_work_duration(time_s *time);

void short_break_view(time_s *time);
void short_break_up();
void short_break_down();
void short_break_duration(time_s *time);

void long_break_view(time_s *time);
void long_break_up();
void long_break_down();
void long_break_duration(time_s *time);

void tasks_done_view();
#endif // USER_INTERFACE_H