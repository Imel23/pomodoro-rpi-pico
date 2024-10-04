#include <stdio.h>
#include "pico/stdlib.h"
#include "state_machine.h"
#include "user_interface.h"
#include "simon_game.h"

#define rot 3

void __init();

int main()
{
    __init();

    ST7735_SetRotation(rot);

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
        case GAME:
            simon_game_state();
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