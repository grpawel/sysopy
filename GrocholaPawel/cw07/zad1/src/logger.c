#include "logger.h"

#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#include "utils.h"

static FILE* streams[LOG_DUMMY] = {0};
static bool debug_enabled = false;

void log_init()
{
    streams[LOG_DEBUG] = stdout;
    streams[LOG_INFO] = stdout;
    streams[LOG_ERROR] = stderr;
}

void log_debug_enable()
{
    debug_enabled = true;
}

void log_debug_disable()
{
    debug_enabled = false;
}

static void _print_header(enum log_level level, FILE* stream)
{
    fprint_time(stream);
    switch(level) {
        case LOG_DEBUG: fprintf(stream, "DEBUG | "); break;
        case LOG_INFO:  fprintf(stream, "INFO  | "); break;
        case LOG_ERROR: fprintf(stream, "ERROR | "); break;
        default:        fprintf(stream, "      | "); break;
    }

}
static void (*print_header)(enum log_level, FILE*) = _print_header;

void log_level(enum log_level level, const char* format, va_list args)
{
    if(level == LOG_DEBUG && !debug_enabled) {
        return;
    }
    FILE* stream = streams[level];
    print_header(level, stream);
    vfprintf(stream, format, args);
    fprintf(stream, "\n");
}

void log_debug(const char* format, ...)
{
    if(!debug_enabled) {
        return;
    }
    va_list args;
    va_start(args, format);
    log_level(LOG_DEBUG, format, args);
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
    print_header = header_f;
}
