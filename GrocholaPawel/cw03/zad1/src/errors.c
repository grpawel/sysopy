#include "errors.h"

char* err_to_string(err_t err)
{
    switch(err) {
        case ERR_SUCCESS: return "No error.";
        case ERR_INVALID_VAR_DECL: return "Invalid variable declaration.";
        case ERR_CANNOT_CREATE_CHILD_PROCESS: return "Cannot create child process.";
        case ERR_COMMAND_CANNOT_RUN: return "Cannot execute command.";
        case ERR_CANNOT_SET_CPU_LIMIT: return "Cannot set rlimit for CPU usage.";
        case ERR_CANNOT_SET_AS_LIMIT: return "Cannot set rlimit for address space usage.";
        case ERR_CANNOT_OPEN_FILE: return "Cannot open file.";
        default: return "Unknown error";
    }
}

bool err_is_system(err_t err)
{
    switch(err) {
        case ERR_CANNOT_CREATE_CHILD_PROCESS:
        case ERR_COMMAND_CANNOT_RUN:
        case ERR_CANNOT_SET_CPU_LIMIT:
        case ERR_CANNOT_SET_AS_LIMIT:
        case ERR_CANNOT_OPEN_FILE:
            return true;
        case ERR_SUCCESS:
        case ERR_INVALID_VAR_DECL:
	default:
            return false;
    }
}
