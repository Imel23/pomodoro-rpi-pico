#include <stdio.h>
#include "pico/stdlib.h"
#include "user_interface.h"
#include "main.h"
#include "buttons_handler.h"
#include "testimg.h"

time_s time = {5, 2, 1, 2, 2, 0, 1};
pomodoro_e state = HOME;

bool startWork = false;
bool intializeView = true;
uint8_t idx = 0;
uint8_t blink_fps = 0;
uint8_t sad_fps = 0;
uint8_t happy_fps = 0;
uint32_t percentage = 0;
bool work = true;

int main()
{
    __init();

    ST7735_SetRotation(rot);

    // ST7735_FillScreen(ST7735_BLACK);
    // while (true)
    // {
    //     eyes_normal_blink();
    //     sleep_ms(33);
    // }

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
            work_duration_state();
            break;
        case SHORTBREAK:
            short_break_state();
            break;
        case LONGBREAK:
            long_break_state();
            break;
        case TASKS_DONE:
            tasks_done_state();
            break;
        default:
            break;
        }

        sleep_ms(33);
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
        state = SETTINGS;
        update_sessions(&time);
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

void eyes_normal_blink()
{
    switch (blink_fps)
    {
    case 0:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[0]);
        break;
    case 41:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[1]);
        break;
    case 42:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[2]);
        break;
    case 44:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[3]);
        break;
    case 45:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[4]);
        break;
    case 46:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[5]);
        break;
    case 47:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[4]);
        break;
    case 48:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[3]);
        break;
    case 49:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[2]);
        break;
    case 50:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[1]);
        break;
    default:
        break;
    }
    blink_fps++;
    if (blink_fps > 50)
        blink_fps = 0;
}

void eyes_sad_blink()
{
    switch (sad_fps)
    {
    case 0:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[0]);
        break;
    case 41:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[6]);
        break;
    case 42:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[7]);
        break;
    case 44:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[8]);
        break;
    case 45:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[9]);
        break;
    case 46:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[10]);
        break;
    case 87:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[9]);
        break;
    case 88:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[8]);
        break;
    case 89:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[7]);
        break;
    case 90:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[6]);
        break;
    default:
        break;
    }
    sad_fps++;
    if (sad_fps > 90)
        sad_fps = 0;
}

void eyes_happy_blink()
{
    switch (happy_fps)
    {
    case 0:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[0]);
        break;
    case 41:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[11]);
        break;
    case 42:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[12]);
        break;
    case 44:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[13]);
        break;
    case 45:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[14]);
        break;
    case 46:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[15]);
        break;
    case 87:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[14]);
        break;
    case 88:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[13]);
        break;
    case 89:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[12]);
        break;
    case 90:
        ST7735_DrawImage(45, 30, 72, 44, (uint16_t *)image[11]);
        break;
    default:
        break;
    }
    happy_fps++;
    if (happy_fps > 90)
        happy_fps = 0;
}