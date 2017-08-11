#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int INTSIZE = sizeof(int);
char* int_to_chara(int n)
{
    char *a = malloc(sizeof(char) * (2 * INTSIZE + 4));
    if(a == NULL) {
        perror("malloc");
        return NULL;
    }

    snprintf(a, 2*INTSIZE + 4, "0x%08x", n);
    return a;
}
int chara_to_int(char* a)
{
    unsigned n;
    sscanf(a, "%x", &n);
    return (int) n;
}

int random_incl(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}
