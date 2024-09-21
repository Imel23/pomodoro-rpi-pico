#include "buttons_handler.h"
#include <stdio.h>

#define DEBOUNCE_DELAY 300 // Debounce delay in milliseconds
#define NUM_BUTTONS 4

// Array of your button GPIO pins
const uint8_t button_pins[] = {
    START_PAUSE_BUTTON,
    INCREA_BUTTON,
    DECREA_BUTTON,
    SETT_BUTTON};

volatile bool is_start_pause_pressed = false;
volatile bool is_settings_pressed = false;
volatile bool is_increase_pressed = false;
volatile bool is_decrease_pressed = false;

void init_buttons()
{
    uint32_t event_mask = GPIO_IRQ_EDGE_RISE;

    // Initialize buttons
    uint8_t buttons[] = {START_PAUSE_BUTTON, INCREA_BUTTON, DECREA_BUTTON, SETT_BUTTON};
    for (int i = 0; i < 4; i++)
    {
        gpio_init(buttons[i]);
        gpio_set_dir(buttons[i], GPIO_IN);
        gpio_pull_up(buttons[i]);
    }

    // Register the global IRQ handler and enable interrupts for all buttons
    // gpio_set_irq_enabled_with_callback(START_PAUSE_BUTTON, event_mask, true, &buttons_callback);
    gpio_set_irq_callback(&buttons_callback);
    for (int i = 0; i < 4; i++)
    {
        gpio_set_irq_enabled(buttons[i], event_mask, true);
    }
    irq_set_enabled(IO_IRQ_BANK0, true);
}

static uint64_t last_debounce_time[NUM_BUTTONS] = {0};

bool debounce(uint gpio)
{
    uint64_t current_time = to_ms_since_boot(get_absolute_time());

    int button_index = -1;
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        if (gpio == button_pins[i])
        {
            button_index = i;
            break;
        }
    }

    if (button_index == -1)
    {
        // Button not recognized
        return false;
    }

    // Check if the button press happened within the debounce delay
    if ((current_time - last_debounce_time[button_index]) > DEBOUNCE_DELAY)
    {
        // Update the last debounce time
        last_debounce_time[button_index] = current_time;
        return true;
    }

    return false;
}

void buttons_callback(uint gpio, uint32_t events)
{
    if (!debounce(gpio))
    {
        // If debounce fails, return early
        return;
    }

    if (gpio == START_PAUSE_BUTTON)
    {
        printf("button1 clicked\n");
        is_start_pause_pressed = true;
    }
    else if (gpio == INCREA_BUTTON)
    {
        printf("button2 clicked\n");
        is_increase_pressed = true;
    }
    else if (gpio == DECREA_BUTTON)
    {
        printf("button3 clicked\n");
        is_decrease_pressed = true;
    }
    else if (gpio == SETT_BUTTON)
    {
        printf("button4 clicked\n");
        is_settings_pressed = true;
    }
    // sleep_ms(250);
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