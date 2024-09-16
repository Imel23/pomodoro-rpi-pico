#include "buttons_handler.h"

#define DEBOUNCE_DELAY 50 // Debounce delay in milliseconds

// Array of your button GPIO pins
const uint8_t button_pins[] = {
    START_PAUSE_BUTTON,
    INCREA_BUTTON,
    DECREA_BUTTON,
    SETT_BUTTON};

#define NUM_BUTTONS 4

void init_buttons()
{
    gpio_init(START_PAUSE_BUTTON);
    gpio_set_dir(START_PAUSE_BUTTON, GPIO_IN);
    gpio_pull_up(START_PAUSE_BUTTON);

    gpio_init(INCREA_BUTTON);
    gpio_set_dir(INCREA_BUTTON, GPIO_IN);
    gpio_pull_up(INCREA_BUTTON);

    gpio_init(DECREA_BUTTON);
    gpio_set_dir(DECREA_BUTTON, GPIO_IN);
    gpio_pull_up(DECREA_BUTTON);

    gpio_init(SETT_BUTTON);
    gpio_set_dir(SETT_BUTTON, GPIO_IN);
    gpio_pull_up(SETT_BUTTON);
}

// Function to map a button GPIO pin to an index
int get_button_index(uint8_t button)
{
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        if (button_pins[i] == button)
        {
            return i;
        }
    }
    // Return -1 if button not found
    return -1;
}

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