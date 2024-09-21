#include <stdio.h>
#include "pico/stdlib.h"
#include "user_interface.h"
#include "main.h"
#include "buttons_handler.h"

time_s time = {8, 25, 5, 10, 25, 0, 1};
pomodoro_e state = HOME;

bool startWork = false;
bool intializeView = true;
uint8_t idx = 0;

int main()
{
    __init();

    // ST7735_SetRotation(rot);

    while (true)
    {
        switch (state)
        {
        case HOME:
            home_state();
            break;
        case SETTINGS:
            settings_state();
            break;
        case SESSIONS:
            sessions_state();
            break;
        case WORKDURATION:
            break;
        case SHORTBREAK:
            break;
        case LONGBREAK:
            break;
        default:
            break;
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

void home_state()
{
    if (intializeView)
    {
        home_view(&time);
        intializeView = false;
        startWork = false;
    }
    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        alarm_fired = true;
        update_pause_resume_display(&startWork);
    }

    if (startWork)
    {
        if (alarm_fired)
        {
            alarm_fired = false;
            alarm_in_us(1000000 * 1);
            update_time(&time);
        }
    }

    if (is_settings_pressed)
    {
        is_settings_pressed = false;
        state = SETTINGS;
        intializeView = true;
    }

    if (is_increase_pressed)
    {
        is_increase_pressed = false;
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
    }
}

void settings_state()
{
    if (intializeView)
    {
        settings_view();
        intializeView = false;
    }

    if (is_settings_pressed)
    {
        is_settings_pressed = false;
        state = HOME;
        intializeView = true;
    }

    if (is_increase_pressed)
    {
        is_increase_pressed = false;
        idx = settings_up();
    }

    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        idx = settings_down();
    }

    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        switch (idx)
        {
        case 0:
            state = SESSIONS;
            intializeView = true;
            break;
        case 1:
            state = WORKDURATION;
            intializeView = true;
            break;
        case 2:
            state = SHORTBREAK;
            intializeView = true;
            break;
        case 3:
            state = LONGBREAK;
            intializeView = true;
            break;
        default:
            break;
        }
    }
}

void sessions_state()
{
    if (intializeView)
    {
        sessions_view(&time);
        intializeView = false;
    }
    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        state = HOME;
        intializeView = true;
    }

    if (is_settings_pressed)
    {
        is_settings_pressed = false;
        state = HOME;
        intializeView = true;
    }

    if (is_increase_pressed)
    {
        is_increase_pressed = false;
        state = HOME;
        intializeView = true;
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        state = HOME;
        intializeView = true;
    }
}