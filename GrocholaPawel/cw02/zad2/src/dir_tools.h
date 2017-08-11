#ifndef _DIR_TOOLS_H
#define _DIR_TOOLS_H
#include <sys/types.h>

char* mode_to_str(mode_t mode);
char* path_add(const char* path_base, const char* name);
#endif
