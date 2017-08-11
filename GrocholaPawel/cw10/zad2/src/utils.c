#include "utils.h"

#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void fprint_time(FILE* stream)
{
    struct timespec ts;
    if(clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
    }
    time_t time = ts.tv_sec;
    struct tm *tm;
    tm = localtime(&time);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    fprintf(stream, "[%s.%06ld] ", buf, ts.tv_nsec / 1000);
}

char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}

int random_incl(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

char* random_name(size_t len)
{
    char* str = malloc(len + 1);
    if(!str) {
        perror("malloc");
        return NULL;
    }
    str[0] = '/';
    for(size_t i=1; i < len; i++) {
        char c = (char) random_incl('a', 'z');
        str[i] = c;
    }
    str[len] = '\0';
    return str;
}

struct timespec ms_to_timespec(long milisec)
{
    int sec = milisec / 1000;
    int nanosec = (milisec % 1000) * 1e6;
    struct timespec ts = {.tv_sec = sec, .tv_nsec = nanosec};
    return ts;
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
