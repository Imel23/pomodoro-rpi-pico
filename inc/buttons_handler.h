#ifndef BUTTONS_HANDLER_H
#define BUTTONS_HANDLER_H

#include "pico/stdlib.h"

#define START_PAUSE_BUTTON 0
#define INCREA_BUTTON 1
#define DECREA_BUTTON 2
#define SETT_BUTTON 3

#define debounce_delay 300

extern volatile bool is_start_pause_pressed;
extern volatile bool is_settings_pressed;
extern volatile bool is_increase_pressed;
extern volatile bool is_decrease_pressed;

void init_buttons();

void buttons_callback(uint gpio, uint32_t events);

#endif // BUTTONS_HANDLER_H