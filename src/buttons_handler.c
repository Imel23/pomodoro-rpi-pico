#include "buttons_handler.h"
#include <stdio.h>
#define DEBOUNCE_DELAY 50 // Debounce delay in milliseconds

// Array of your button GPIO pins
const uint8_t button_pins[] = {
    START_PAUSE_BUTTON,
    INCREA_BUTTON,
    DECREA_BUTTON,
    SETT_BUTTON};

#define NUM_BUTTONS 4

volatile bool is_start_pause_pressed = false;
volatile bool is_settings_pressed = false;
volatile bool is_increase_pressed = false;
volatile bool is_decrease_pressed = false;

void init_buttons()
{
    uint32_t event_mask = GPIO_IRQ_EDGE_FALL;

    // Initialize buttons
    uint8_t buttons[] = {START_PAUSE_BUTTON, INCREA_BUTTON, DECREA_BUTTON, SETT_BUTTON};
    for (int i = 0; i < 4; i++)
    {
        gpio_init(buttons[i]);
        gpio_set_dir(buttons[i], GPIO_IN);
        gpio_pull_up(buttons[i]);
    }

    // Register the global IRQ handler and enable interrupts for all buttons
    gpio_set_irq_enabled_with_callback(START_PAUSE_BUTTON, event_mask, true, &buttons_callback);
    for (int i = 1; i < 4; i++)
    {
        gpio_set_irq_enabled(buttons[i], event_mask, true);
    }
}

void buttons_callback(uint gpio, uint32_t events)
{
    if (gpio == START_PAUSE_BUTTON)
    {
        is_start_pause_pressed = true;
    }
    else if (gpio == INCREA_BUTTON)
    {
        is_increase_pressed = true;
    }
    else if (gpio == DECREA_BUTTON)
    {
        is_decrease_pressed = true;
    }
    else if (gpio == SETT_BUTTON)
    {
        is_settings_pressed = true;
    }
}
/*
bool check_button_press(uint8_t button)
{
    static bool button_was_pressed[NUM_BUTTONS] = {false};
    static uint64_t last_debounce_time[NUM_BUTTONS] = {0};

    int index = get_button_index(button);
    if (index == -1)
    {
        // Handle error: button not recognized
        return false;
    }

    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    bool button_state = gpio_get(button);

    if (button_state == 0 && !button_was_pressed[index] &&
        (current_time - last_debounce_time[index]) > DEBOUNCE_DELAY)
    {
        button_was_pressed[index] = true;
        last_debounce_time[index] = current_time;
        return true;
    }

    if (button_state == 1)
    {
        button_was_pressed[index] = false;
    }

    return false;
}
*/