#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "state_machine.h"
#include <stdio.h>

// Note definitions (unchanged)
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

#define BUZZER_PIN 29 // GPIO pin for the buzzer

// Mario main theme melody
int melody[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0, 0,
    NOTE_G6, 0, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0};
// Mario main them tempo
int tempo[] = {
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,

    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,

    9,
    9,
    9,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,

    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,

    9,
    9,
    9,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
};
// Underworld melody
int underworld_melody[] = {
    NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
    NOTE_AS3, NOTE_AS4, 0,
    0,
    NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
    NOTE_AS3, NOTE_AS4, 0,
    0,
    NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
    NOTE_DS3, NOTE_DS4, 0,
    0,
    NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
    NOTE_DS3, NOTE_DS4, 0,
    0, NOTE_DS4, NOTE_CS4, NOTE_D4,
    NOTE_CS4, NOTE_DS4,
    NOTE_DS4, NOTE_GS3,
    NOTE_G3, NOTE_CS4,
    NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
    NOTE_GS4, NOTE_DS4, NOTE_B3,
    NOTE_AS3, NOTE_A3, NOTE_GS3,
    0, 0, 0};
// Underwolrd tempo
int underworld_tempo[] = {
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    6, 18, 18, 18,
    6, 6,
    6, 6,
    6, 6,
    18, 18, 18, 18, 18, 18,
    10, 10, 10,
    10, 10, 10,
    3, 3, 3};

void buzz(uint pin, long frequency, long length);

// Note frequencies
#define NOTE_C4 262
#define NOTE_G3 196
#define NOTE_E3 165
#define NOTE_A3 220
#define NOTE_B3 247
#define NOTE_C3 131

// Mario game over melody
int mario_game_over_melody[] = {
    NOTE_C4, NOTE_G3, NOTE_E3,
    NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3,
    NOTE_E3, NOTE_G3, NOTE_C3, 0};

// Durations (in milliseconds)
int mario_game_over_durations[] = {
    250, 250, 250,
    250, 250, 250, 250,
    250, 250, 500, 0};

void buzz(uint pin, long frequency, long length)
{
    if (frequency == 0)
    {
        sleep_ms(length);
        return;
    }

    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_enabled(slice_num, false);
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint32_t clock = 125000000;
    uint32_t divider16 = clock / frequency / 4096 + (clock % (frequency * 4096) != 0);
    if (divider16 / 16 == 0)
        divider16 = 16;
    uint32_t wrap = clock * 16 / divider16 / frequency - 1;
    pwm_set_clkdiv_int_frac(slice_num, divider16 / 16, divider16 & 0xF);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(pin), wrap / 2);
    pwm_set_enabled(slice_num, true);
    sleep_ms(length);
    pwm_set_enabled(slice_num, false);
}

void play_mario_death_sound()
{

    int size = sizeof(mario_game_over_melody) / sizeof(int);

    for (int i = 0; i < size; i++)
    {
        if (interrupt_flag != 2)
            return;
        // play_tone(gpio, mario_death_melody[i], mario_death_durations[i]);
        buzz(BUZZER_PIN, mario_game_over_melody[i], mario_game_over_durations[i]);
        sleep_ms(10); // Small pause between notes
    }
}

void sing(int s)
{
    int thisNote, noteDuration, pauseBetweenNotes;
    int size;
    int *current_melody;
    int *current_tempo;

    if (s == 2)
    {
        printf("'Underworld Theme'\n");
        size = sizeof(underworld_melody) / sizeof(int);
        current_melody = underworld_melody;
        current_tempo = underworld_tempo;
    }
    else
    {
        printf("'Mario Theme'\n");
        size = sizeof(melody) / sizeof(int);
        current_melody = melody;
        current_tempo = tempo;
    }

    for (thisNote = 0; thisNote < size; thisNote++)
    {
        if (interrupt_flag != 1)
            return;
        noteDuration = 1000 / current_tempo[thisNote];
        buzz(BUZZER_PIN, current_melody[thisNote], noteDuration);
        pauseBetweenNotes = noteDuration * 1.30;
        sleep_ms(pauseBetweenNotes);
    }
}

void reminder_sound()
{

    for (int i = 0; i < 9; i++)
    {
        if (interrupt_flag != 3)
            return;

        // play_tone(gpio, mario_death_melody[i], mario_death_durations[i]);
        buzz(BUZZER_PIN, NOTE_C4, 250);
        sleep_ms(750); // Small pause between notes
    }
}

void tone()
{
    if (interrupt_flag == 1)
    {
        sing(1);
        sing(2);
    }
    else if (interrupt_flag == 2)
    {
        play_mario_death_sound();
    }
    else if (interrupt_flag == 3)
    {
        reminder_sound();
    }
}
