//Pawel Grochola
#ifndef TOOLS_H
#define TOOLS_H
#include <string.h>
char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}
#endif