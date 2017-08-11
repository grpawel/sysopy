#ifndef __UTILS_H
#define __UTILS_H
int random_incl(int min, int max);

struct timespec ms_to_timespec(long milisec);
long timespec_to_ms(struct timespec ts);

void sleep_ms(long milisec);
long current_ms(void);
#endif
