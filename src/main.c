#include <stdio.h>
#include "pico/stdlib.h"
#include "user_interface.h"
#include "main.h"

void init_gpio()
{
    gpio_init(BUTTON1);
    gpio_set_dir(BUTTON1, GPIO_IN);
    gpio_pull_up(BUTTON1);
}

bool check_button_press(bool *button_was_pressed, uint64_t *last_debounce_time, const uint64_t debounce_delay)
{
    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    bool button_state = gpio_get(BUTTON1);

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

void update_timer(int *minutes, int *seconds, bool *timer_running)
{
    if (*seconds == 0)
    {
        if (*minutes > 0)
        {
            *seconds = 59;
            --(*minutes);
        }
        else
        {
            *timer_running = false;
        }
    }
    else
    {
        --(*seconds);
    }
}

int main()
{
    stdio_init_all();
    init_display();
    init_gpio();

    int minutes = 5;
    int seconds = 59;
    int old_minutes = -1;
    int old_seconds = -1;

    bool timer_running = false;
    bool pause_clicked = false;
    bool button_clicked_once = false;
    bool button_was_pressed = false;
    uint64_t last_debounce_time = 0;
    const uint64_t debounce_delay = 300;

    uint64_t last_timer_tick = 0;
    const uint64_t timer_interval = 1000;

    home_view(button_clicked_once, pause_clicked, "WORK", minutes, seconds, &old_minutes, &old_seconds, 1, 4, (int)((5 - minutes) * 100 / 5));

    while (true)
    {
        uint64_t current_time = to_ms_since_boot(get_absolute_time());

        if (check_button_press(&button_was_pressed, &last_debounce_time, debounce_delay))
        {
            if (button_clicked_once)
            {
                pause_clicked = !pause_clicked;
            }
            timer_running = !timer_running;
            button_clicked_once = true;
        }

        if (timer_running && (current_time - last_timer_tick) >= timer_interval)
        {
            last_timer_tick = current_time;

            update_timer(&minutes, &seconds, &timer_running);
        }
        home_view(button_clicked_once, pause_clicked, "WORK", minutes, seconds, &old_minutes, &old_seconds, 1, 4, (int)((5 - minutes) * 100 / 5));

        sleep_ms(10);
    }

    return 0;
}