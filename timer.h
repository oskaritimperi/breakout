#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef int64_t Timer;

void TimerInit();

void TimerStart(Timer *timer);

int64_t TimerElapsedMicro(Timer *timer);

int64_t TimerElapsedMilli(Timer *timer);

#endif
