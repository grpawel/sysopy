#ifndef __UTILS_H_
#define __UTILS_H_

#define OUT

extern const int INTSIZE;
//must be freed
char* int_to_chara(int n);
int chara_to_int(char* a);
int random_incl(int min, int max);
char* strdup(const char* p);
#endif
