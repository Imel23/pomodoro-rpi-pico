// main.h

#ifndef MAIN_H
#define MAIN_H

#include "timer_.h"

typedef struct
{
    uint8_t sessions;
    uint8_t workDuration;
    uint8_t shortBreak;
    uint8_t longBreak;
    uint8_t currentMinute;
    uint8_t currentSecond;
    uint8_t currentSession;
} time_s;

typedef enum
{
    HOME,
    SETTINGS,
    SESSIONS,
    WORKDURATION,
    SHORTBREAK,
    LONGBREAK
} pomodoro_e;

#define rot 2

void __init();

void home_state();

void settings_state();

void sessions_state();
#endif