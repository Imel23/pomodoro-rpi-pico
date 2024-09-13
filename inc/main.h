// main.h

#ifndef MAIN_H
#define MAIN_H

#define BUTTON1 16
#define BUTTON2 17
#define BUTTON3 18
#define BUTTON4 19

extern volatile bool alarm_fired;
extern void alarm_in_us(uint32_t delay_us);

#endif