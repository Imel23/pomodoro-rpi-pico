#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

// Alarm interrupt handler
extern volatile bool alarm_fired;

extern void alarm_in_us(uint32_t delay_us);

#endif //__TIMER_H__