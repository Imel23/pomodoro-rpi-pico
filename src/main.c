#include <stdio.h>
#include "pico/stdlib.h"
#include "user_interface.h"
#include "main.h"
#include "buttons_handler.h"

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
    init_buttons();

    int minutes = 5;
    int seconds = 59;
    int old_minutes = -1;
    int old_seconds = -1;

    bool timer_running = false;
    bool pause_clicked = false;
    bool button_clicked_once = false;

    uint64_t last_timer_tick = 0;
    const uint64_t timer_interval = 1000;

    home_view(button_clicked_once, pause_clicked, "WORK", minutes, seconds, &old_minutes, &old_seconds, 1, 4, (int)((5 - minutes) * 100 / 5));

    while (true)
    {
        uint64_t current_time = to_ms_since_boot(get_absolute_time());

        if (is_start_pause_pressed)
        {
            printf("start pause button was pressed!\n");
            is_start_pause_pressed = false;
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