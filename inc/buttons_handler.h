#ifndef BUTTONS_HANDLER_H
#define BUTTONS_HANDLER_H

#include "pico/stdlib.h"

#define START_PAUSE_BUTTON 26
#define INCREA_BUTTON 21
#define DECREA_BUTTON 20
#define SETT_BUTTON 19

void init_buttons();

int get_button_index(uint8_t button);

bool check_button_press(uint8_t button);

#endif // BUTTONS_HANDLER_H