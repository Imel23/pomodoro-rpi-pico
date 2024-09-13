#ifndef BUTTONS_HANDLER_H
#define BUTTONS_HANDLER_H

#include "pico/stdlib.h"

#define START_PAUSE_BUTTON 26
#define INCREA_BUTTON 21
#define DECREA_BUTTON 20
#define SETT_BUTTON 19

void init_buttons();

bool check_button_press(uint8_t button, bool *button_was_pressed, uint64_t *last_debounce_time, const uint64_t debounce_delay);

#endif // BUTTONS_HANDLER_H