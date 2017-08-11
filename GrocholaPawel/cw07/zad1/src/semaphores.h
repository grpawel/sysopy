#ifndef __SEMAPHORES_H_
#define __SEMAPHORES_H_

#include <stdbool.h>

typedef struct _semmut_data
{
    int semset_id;
    int next_num;
    int count;
    bool created;
}* semmut_data;

semmut_data semmut_init(bool create_new, int count, const char* ftok_path, int ftok_id);
semmut_data semmut_init_private(int count);
void semmut_deinit(semmut_data data);

typedef struct mutex
{
    int num;
    int semset_id;
}* mutex_t;

mutex_t mutex_create(semmut_data data);
void mutex_delete(mutex_t mut);
void mutex_wait(mutex_t mutex);
void mutex_signal(mutex_t mutex);
void mutex_set_state(mutex_t mutex, int state);

typedef struct semaphore
{
    int num;
    int semset_id;
}* semaphore_t;


semaphore_t semap_create(semmut_data data);
void semap_delete(semaphore_t semap);
void semap_wait(semaphore_t semap);
void semap_signal(semaphore_t semap);
int semap_get_state(semaphore_t semap);
void semap_set_state(semaphore_t semap, int state);

#endif

