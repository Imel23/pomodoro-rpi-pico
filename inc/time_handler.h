// time_handler.h

#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include "pico/stdlib.h"
#include "timer_.h"

typedef struct
{
    uint8_t workDuration;
    uint8_t pauseDuration;
    uint8_t currentMinute;
    uint8_t currentSecond;
} time_s;

#endif // TIME_HANDLER_H