#include "utils.h"
#include <time.h>
#include <stdlib.h>
int random_incl(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

struct timespec ms_to_timespec(long milisec)
{
    int sec = milisec / 1000;
    int nanosec = (milisec % 1000) * 1e6;
    struct timespec ts = {.tv_sec = sec, .tv_nsec = nanosec};
    return ts;
}

long timespec_to_ms(struct timespec ts)
{
    long milisec = 0;
    milisec += ts.tv_sec * 1000;
    milisec += ts.tv_nsec / 1e6;
    return milisec;
}

void sleep_ms(long milisec)
{
    struct timespec request = ms_to_timespec(milisec);
    struct timespec remaining;
    int ret = 0;
    do {
        ret = nanosleep(&request, &remaining);
        request = remaining;
    } while(ret == -1);
}

long current_ms(void)
{
    struct timespec ts = {0,0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return timespec_to_ms(ts);
}
