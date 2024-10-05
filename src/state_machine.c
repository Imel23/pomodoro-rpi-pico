#include <stdio.h>
#include "pico/stdlib.h"
#include "user_interface.h"
#include "state_machine.h"
#include "buttons_handler.h"
#include "timer_.h"
#include "simon_game.h"
#include "flash.h"

#define SECOND 100000

time_s time;
pomodoro_e state = HOME;

bool startWork = false;
bool intializeView = true;
uint8_t idx = 0;
uint32_t percentage = 0;
bool work = true;
bool firstPowerOn = true;
void home_state()
{
    if (firstPowerOn)
    {
        flash_read(&time);
        time.currentMinute = time.workDuration;
        time.currentSecond = 0;
        time.currentSession = 1;
        firstPowerOn = false;
    }
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
            alarm_in_us(SECOND);
            if (update_time(&time, &percentage, &work))
            {
                state = TASKS_DONE;
                intializeView = true;
                return;
            }
        }
    }
    if (!work)
    {
        eyes_normal_blink();
    }
    else
    {
        if (percentage < 30)
            eyes_normal_blink();
        else if (percentage < 70)
            eyes_sad_blink();
        else
            eyes_happy_blink();
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
        face = ANGRY;
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        face = STARS;
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
        idx = settings_down();
    }

    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        idx = settings_up();
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
        case 4:
            state = GAME;
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
        state = SETTINGS;
        update_sessions(&time);
        flash_write(&time);
        intializeView = true;
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
        sessions_up();
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        sessions_down();
    }
}

void work_duration_state()
{
    if (intializeView)
    {
        work_duration_view(&time);
        intializeView = false;
    }
    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        state = SETTINGS;
        update_work_duration(&time);
        flash_write(&time);
        intializeView = true;
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
        work_duration_up();
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        work_duration_down();
    }
}

void short_break_state()
{
    if (intializeView)
    {
        short_break_view(&time);
        intializeView = false;
    }
    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        state = SETTINGS;
        short_break_duration(&time);
        flash_write(&time);
        intializeView = true;
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
        short_break_up();
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        short_break_down();
    }
}

void long_break_state()
{
    if (intializeView)
    {
        long_break_view(&time);
        intializeView = false;
    }
    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        state = SETTINGS;
        long_break_duration(&time);
        flash_write(&time);
        intializeView = true;
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
        long_break_up();
    }
    if (is_decrease_pressed)
    {
        is_decrease_pressed = false;
        long_break_down();
    }
}

void tasks_done_state()
{
    if (intializeView)
    {
        tasks_done_view();
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

void simon_game_state()
{
    if (intializeView)
    {
        simon_game_view();
        intializeView = false;
    }

    simon_game();

    if (simon_state == IDLE_S)
    {
        if (is_settings_pressed)
        {
            is_settings_pressed = false;
            state = HOME;
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
}