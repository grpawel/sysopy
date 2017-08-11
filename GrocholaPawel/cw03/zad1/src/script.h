#ifndef _SCRIPT_H
#define _SCRIPT_H
#include <stdio.h>

#ifdef ZADANIE_2
#include <sys/resource.h>
extern rlim_t cpu_limit;
extern rlim_t as_limit;
#endif
void run_script(FILE* file);

#endif
