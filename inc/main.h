// main.h

#ifndef MAIN_H
#define MAIN_H

#include "timer_.h"

#define SECOND 1000000
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
    LONGBREAK,
    TASKS_DONE
} pomodoro_e;

#define rot 3

void __init();

void home_state();
void settings_state();
void sessions_state();
void work_duration_state();
void short_break_state();
void long_break_state();
void tasks_done_state();
void eyes_normal_blink();

#endif