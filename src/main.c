#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "state_machine.h"
#include "user_interface.h"
#include "simon_game.h"
#include "buzzer.h"

#define rot 1

volatile uint8_t interrupt_flag = 0;

// Core 1 function to handle an event (e.g., button press)
void core1_entry()
{
    while (true)
    {
        if (interrupt_flag)
        {
            tone();
        }
        // Core 1 can also check for other interrupts/events here
        sleep_ms(10); // Small sleep to yield CPU time
    }
}

void __init();

int main()
{
    __init();

    multicore_launch_core1(core1_entry);

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
            games_state();
            break;
        case SIMON:
            simon_game_state();
            break;
        case MASTERMIND:
            mastermind_game_state();
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