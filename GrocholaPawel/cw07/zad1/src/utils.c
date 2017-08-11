#include "utils.h"

#include <signal.h>
#include <stdarg.h>
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
