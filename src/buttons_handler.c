#include "buttons_handler.h"

void init_buttons()
{
    gpio_init(START_PAUSE_BUTTON);
    gpio_set_dir(START_PAUSE_BUTTON, GPIO_IN);
    gpio_pull_up(START_PAUSE_BUTTON);

    gpio_init(START_PAUSE_BUTTON);
    gpio_set_dir(START_PAUSE_BUTTON, GPIO_IN);
    gpio_pull_up(START_PAUSE_BUTTON);

    gpio_init(START_PAUSE_BUTTON);
    gpio_set_dir(START_PAUSE_BUTTON, GPIO_IN);
    gpio_pull_up(START_PAUSE_BUTTON);

    gpio_init(START_PAUSE_BUTTON);
    gpio_set_dir(START_PAUSE_BUTTON, GPIO_IN);
    gpio_pull_up(START_PAUSE_BUTTON);
}

bool check_button_press(uint8_t button, bool *button_was_pressed, uint64_t *last_debounce_time, const uint64_t debounce_delay)
{
    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    bool button_state = gpio_get(button);

    if (button_state == 0 && !(*button_was_pressed) && (current_time - *last_debounce_time) > debounce_delay)
    {
        *button_was_pressed = true;
        *last_debounce_time = current_time;
        return true;
    }

    if (button_state == 1)
    {
        *button_was_pressed = false;
    }

    return false;
}