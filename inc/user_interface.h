// user_interface.h

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "st7735.h"
#include "fonts.h"

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
 * @brief Display and update the main timer view on the screen.
 *
 * @param is_button_clicked Boolean flag indicating if the start/pause/resume button was clicked
 * @param is_timer_paused Boolean flag indicating whether the button start/pause/resume was clicked to pause the timer
 * @param timer_state Current state of the timer (e.g., "WORK", "REST")
 * @param current_minutes Current minutes of the timer
 * @param current_seconds Current seconds of the timer
 * @param previous_minutes Pointer to previous minutes (for updating display efficiently)
 * @param previous_seconds Pointer to previous seconds (for updating display efficiently)
 * @param current_session Current session number
 * @param total_sessions Total number of sessions
 * @param progress_percentage Progress percentage of the current session (0-100)
 */
void home_view(bool is_button_clicked, bool is_timer_paused, char *timer_state,
               int current_minutes, int current_seconds,
               int *previous_minutes, int *previous_seconds,
               int current_session, int total_sessions, int progress_percentage);

/**
 * @brief Draw the initial view of the timer interface.
 *
 * @param timer_state Current state of the timer (e.g., "WORK", "REST")
 * @param current_session Current session number
 * @param total_sessions Total number of sessions
 */
void draw_initial_view(char *timer_state, int current_session, int total_sessions);

/**
 * @brief Update the pause/resume button display on the screen.
 *
 * @param is_timer_paused Boolean flag indicating whether the timer is paused
 */
void update_pause_resume_display(bool is_timer_paused);

/**
 * @brief Update the time display on the screen.
 *
 * @param previous_minutes Previous minutes
 * @param previous_seconds Previous seconds
 * @param current_minutes New minutes to display
 * @param current_seconds New seconds to display
 */
void update_time_display(int previous_minutes, int previous_seconds, int current_minutes, int current_seconds);

/**
 * @brief Update the progress bar on the screen.
 *
 * @param completion_percentage Progress percentage (0-100)
 */
void update_progress_bar(int completion_percentage);

#endif // USER_INTERFACE_H