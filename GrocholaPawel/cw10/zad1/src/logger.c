#include "logger.h"

#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "utils.h"

static FILE* streams[LOG_DUMMY];
static bool enabled[LOG_DUMMY];

pthread_mutex_t writing_mutexes[LOG_DUMMY];

void log_init()
{
    streams[LOG_DEBUG] = stdout;
    streams[LOG_DETAIL] = stdout;
    streams[LOG_INFO] = stdout;
    streams[LOG_ERROR] = stderr;

    enabled[LOG_DEBUG] = false;
    enabled[LOG_DETAIL] = false;
    enabled[LOG_INFO] = true;
    enabled[LOG_ERROR] = true;

    for(int i = 0; i < LOG_DUMMY; i++) {
        pthread_mutex_init(writing_mutexes + i, NULL);
    }
}

void log_enable(enum log_level level)
{
    enabled[level] = true;
}

void log_disable(enum log_level level)
{
    enabled[level] = false;
}

static void _print_header(enum log_level level, FILE* stream)
{
    fprint_time(stream);
    switch(level) {
        case LOG_DEBUG: fprintf(stream, "DEBUG | "); break;
        case LOG_DETAIL: fprintf(stream, "DET   | "); break;
        case LOG_INFO:  fprintf(stream, "INFO  | "); break;
        case LOG_ERROR: fprintf(stream, "ERROR | "); break;
        default:        fprintf(stream, "      | "); break;
    }
}

static void (*header_printer)(enum log_level, FILE*) = _print_header;

void log_level(enum log_level level, const char* format, va_list args)
{
    if(!enabled[level]) {
        return;
    }
    FILE* stream = streams[level];
    pthread_mutex_lock(writing_mutexes + level);
    header_printer(level, stream);
    vfprintf(stream, format, args);
    fprintf(stream, "\n");
    pthread_mutex_unlock(writing_mutexes + level);
}

void log_debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_level(LOG_DEBUG, format, args);
    va_end(args);
}

void log_detail(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_level(LOG_DETAIL, format, args);
    va_end(args);
}

void log_info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_level(LOG_INFO, format, args);
    va_end(args);
}

void log_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_level(LOG_ERROR, format, args);
    va_end(args);
}

void log_set_stream(enum log_level level, FILE* stream)
{
    streams[level] = stream;
}

FILE* log_get_stream(enum log_level level)
{
    return streams[level];
}

void log_set_header(void (*header_f)(enum log_level level, FILE* stream))
{
    header_printer = header_f;
}
