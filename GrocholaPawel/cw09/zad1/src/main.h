#ifndef __MAIN_H_
#define __MAIN_H_

#include <pthread.h>

extern int* numbers;
extern int numbers_size;
extern int number_min;
extern int number_max;

extern int writers_count;
extern int readers_count;

extern int reads_to_do;
extern int writes_to_do;

extern pthread_mutex_t threads_creating_mutex;
extern pthread_cond_t threads_creating_cond;
extern bool threads_created;

#endif
