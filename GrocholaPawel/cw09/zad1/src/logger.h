#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <stdio.h>

enum log_level
{
    LOG_DEBUG = 0,
    LOG_DETAIL,
    LOG_INFO,
    LOG_ERROR,
    LOG_DUMMY //used as array size
};
void log_init();
void log_enable(enum log_level level);
void log_disable(enum log_level level);

void log_level(enum log_level level, const char* format, va_list args);
void log_debug(const char* format, ...);
void log_detail(const char* format, ...);
void log_info(const char* format, ...);
void log_error(const char* format, ...);

void log_set_stream(enum log_level level, FILE* stream);
FILE* log_get_stream(enum log_level level);
void log_set_header(void (*header_f)(enum log_level level, FILE* stream));
#endif
