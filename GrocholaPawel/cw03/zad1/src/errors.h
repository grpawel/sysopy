#ifndef _ERRORS_H
#define _ERRORS_H
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t err_t;

#define ERROR_STARTING_NUM (51000)
typedef enum
{
    ERR_SUCCESS = 0,
    ERR_INVALID_VAR_DECL = ERROR_STARTING_NUM,
    ERR_CANNOT_CREATE_CHILD_PROCESS,
    ERR_COMMAND_CANNOT_RUN,
    ERR_CANNOT_SET_CPU_LIMIT,
    ERR_CANNOT_SET_AS_LIMIT,
    ERR_CANNOT_OPEN_FILE
} ERRORS;

char* err_to_string(err_t err);
bool err_is_system(err_t err);


#endif
