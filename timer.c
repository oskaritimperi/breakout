#include "timer.h"

#include <time.h>

#if defined(_WIN32)
#include <windows.h>
int64_t frequency;
#endif

void TimerInit()
{
#if defined(_WIN32)
    QueryPerformanceFrequency((LARGE_INTEGER *) &frequency);
#endif
}

int64_t TimerGetTimeMicro()
{
#if defined(_WIN32)
    int64_t value;
    QueryPerformanceCounter((LARGE_INTEGER *) &value);
    value = value * 1000LL*1000LL;
    return value / frequency;
#else
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp))
    {
        return 0;
    }
    return (((int64_t) tp.tv_sec) * 1000LL*1000LL*1000LL + ((int64_t) tp.tv_nsec)) / 1000LL;
#endif
}

void TimerStart(Timer *timer)
{
    *timer = TimerGetTimeMicro();
}

int64_t TimerElapsedMicro(Timer *timer)
{
    int64_t now = TimerGetTimeMicro(timer);
    return now - *timer;
}

int64_t TimerElapsedMilli(Timer *timer)
{
    return TimerElapsedMicro(timer) / 1000LL;
}
