#ifndef __TIMER_H__
#define __TIMER_H__


#include <stdint.h>

// Alarm interrupt handler
volatile bool alarm_fired;

void alarm_in_us(uint32_t delay_us);

#endif //__TIMER_H__