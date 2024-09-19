#include <stdio.h>
#include "pico/stdlib.h"
#include "user_interface.h"
#include "main.h"
#include "buttons_handler.h"
#include "time_handler.h"

time_s time = {5, 2, 0, 0};

int main()
{
    __init();

    // ST7735_SetRotation(rot);

    bool pause_clicked = false;

    uint64_t last_timer_tick = 0;
    const uint64_t timer_interval = 1000;

    time.currentSecond = 0;
    time.currentMinute = time.workDuration;

    while (true)
    {
        settings_view();
        sleep_ms(50);
    }
    draw_initial_view("WORK", time.workDuration, 1, 4);

    while (true)
    {
        if (is_start_pause_pressed)
        {
            is_start_pause_pressed = false;
            update_pause_resume_display(pause_clicked);

            pause_clicked = !pause_clicked;
            alarm_fired = true;
        }

        if (pause_clicked)
        {
            if (alarm_fired)
            {
                // printf("fire !!!!! \n");
                alarm_fired = false;
                alarm_in_us(1000000 * 1);
                update_time(&time);
            }
        }

        sleep_ms(10);
    }

    return 0;
}

void __init()
{
    stdio_init_all();
    init_display();
    init_buttons();
}