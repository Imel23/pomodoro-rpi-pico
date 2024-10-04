#include <stdio.h>
#include <stdlib.h>
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "buttons_handler.h"
#include "user_interface.h"
#include "simon_game.h"

#define MAX_LEVEL 20

typedef enum
{
    START_BTN_NOT_PRESSED,
    START_BTN_PRESSED,
    RIGHTSEQ,
    WRONGSEQ
} event_e;

void idle_sequence();
event_e check_start_btn();
void generate_sequence();
void play_sequence();
void play_wrong_sequence();
void get_sequence();
void play_right_sequence();
void toggle_seq_led(uint seq, bool state);

uint diff = 1000;
uint level = 1;
uint seq[MAX_LEVEL], playerseq[MAX_LEVEL];

state_e simon_state = IDLE_S;
event_e event = START_BTN_NOT_PRESSED;

void simon_game()
{

    switch (simon_state)
    {
    case IDLE_S:
        idle_sequence();
        if (START_BTN_PRESSED == check_start_btn())
        {
            simon_state = PLAYSEQ_S;
            generate_sequence();
        }
        break;

    case PLAYSEQ_S:
        play_sequence();
        get_sequence();
        break;

    case RIGHTSEQ_S:
        play_right_sequence();
        if (level < MAX_LEVEL)
            level++;
        diff -= 100;
        simon_state = PLAYSEQ_S;
        break;

    case WRONGSEQ_S:
        play_wrong_sequence();
        level = 1;
        diff = 1000;
        simon_state = IDLE_S;
        break;

    default:
        break;
    }
}

uint8_t idle_cpt = 0;

void idle_sequence()
{
    // Flash the LEDs in sequence
    switch (idle_cpt)
    {
    case 0:
        simon_game_yellow();
        simon_game_green_off();
        break;
    case 10:
        simon_game_yellow_off();
        simon_game_blue();
        break;
    case 20:
        simon_game_blue_off();
        simon_game_red();
        break;
    case 30:
        simon_game_red_off();
        simon_game_green();
        break;
    default:
        break;
    }
    idle_cpt++;
    if (idle_cpt > 40)
        idle_cpt = 0;
}

event_e check_start_btn()
{
    if (is_start_pause_pressed)
    {
        is_start_pause_pressed = false;
        return START_BTN_PRESSED;
    }
    return START_BTN_NOT_PRESSED;
}

void play_right_sequence()
{
    simon_game_yellow_off();
    simon_game_blue_off();
    simon_game_red_off();
    simon_game_green_off();
    sleep_ms(500);
    simon_game_yellow();
    simon_game_blue();
    simon_game_red();
    simon_game_green();
    sleep_ms(500);
    simon_game_yellow_off();
    simon_game_blue_off();
    simon_game_red_off();
    simon_game_green_off();
    sleep_ms(500);
}
void generate_sequence()
{
    uint i;
    // Use time since boot (in microseconds) as a seed
    srand(to_us_since_boot(get_absolute_time()));
    for (i = 0; i < MAX_LEVEL; i++)
    {
        seq[i] = (rand() % 4);
    }

    simon_game_yellow_off();
    simon_game_blue_off();
    simon_game_red_off();
    simon_game_green_off();
    sleep_ms(300);
}

void toggle_seq_led(uint seq, bool simon_state)
{
    switch (seq)
    {
    case 0:
        if (simon_state)
            simon_game_yellow();
        else
            simon_game_yellow_off();
        break;
    case 1:
        if (simon_state)
            simon_game_blue();
        else
            simon_game_blue_off();
        break;
    case 2:
        if (simon_state)
            simon_game_red();
        else
            simon_game_red_off();
        break;
    case 3:
        if (simon_state)
            simon_game_green();
        else
            simon_game_green_off();
        break;
    default:
        break;
    }
}
void play_sequence()
{
    uint i;
    simon_game_yellow_off();
    simon_game_blue_off();
    simon_game_red_off();
    simon_game_green_off();

    for (i = 0; i < level; i++)
    {
        toggle_seq_led(seq[i], true);
        sleep_ms(diff);
        toggle_seq_led(seq[i], false);
        sleep_ms(200);
    }
}

void play_wrong_sequence()
{
    uint i;
    for (i = 0; i < 3; i++)
    {
        simon_game_yellow();
        simon_game_blue();
        simon_game_red();
        simon_game_green();
        sleep_ms(500);
        simon_game_yellow_off();
        simon_game_blue_off();
        simon_game_red_off();
        simon_game_green_off();
        sleep_ms(500);
    }
}

void get_sequence()
{
    uint i, flag = 0;

    for (i = 0; i < level; i++)
    {
        flag = 0;
        while (!flag)
        {
            if (is_start_pause_pressed)
            {
                is_start_pause_pressed = false;
                simon_game_yellow();
                sleep_ms(500);
                simon_game_yellow_off();
                sleep_ms(200);
                flag = 1;
                playerseq[i] = 0;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    simon_state = WRONGSEQ_S;
                    return;
                }
            }
            else if (is_decrease_pressed)
            {
                is_decrease_pressed = false;
                simon_game_blue();
                sleep_ms(500);
                simon_game_blue_off();
                sleep_ms(200);
                flag = 1;
                playerseq[i] = 1;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    simon_state = WRONGSEQ_S;
                    return;
                }
            }
            else if (is_increase_pressed)
            {
                is_increase_pressed = false;
                simon_game_red();
                sleep_ms(500);
                simon_game_red_off();
                sleep_ms(200);
                flag = 1;
                playerseq[i] = 2;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    simon_state = WRONGSEQ_S;
                    return;
                }
            }
            else if (is_settings_pressed)
            {
                is_settings_pressed = false;
                simon_game_green();
                sleep_ms(500);
                simon_game_green_off();
                sleep_ms(200);
                flag = 1;
                playerseq[i] = 3;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    simon_state = WRONGSEQ_S;
                    return;
                }
            }
        }
    }
    event = RIGHTSEQ;
    simon_state = RIGHTSEQ_S;
}