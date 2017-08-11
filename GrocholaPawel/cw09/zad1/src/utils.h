#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdio.h>

void fprint_time(FILE* stream);
char* strdup(const char* p);
int random_incl(int min, int max);
char* random_name(size_t len);
#endif
