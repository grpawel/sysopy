#ifndef __SEMAPHORES_H_
#define __SEMAPHORES_H_

#include <semaphore.h>
#include <stdbool.h>

typedef struct mutex
{
    char* name;
    sem_t* sem;
    bool created;
}* mutex_t;

mutex_t mutex_create(const char* name, bool create_new, int init_val);
void mutex_delete(mutex_t mutex);
void mutex_wait(mutex_t mutex);
void mutex_signal(mutex_t mutex);

typedef struct semaphore
{
    char* name;
    sem_t* sem;
    bool created;
}* semaphore_t;


semaphore_t semap_create(const char* name, bool create_new, int init_val);
void semap_delete(semaphore_t semap);
void semap_wait(semaphore_t semap);
void semap_signal(semaphore_t semap);
int semap_get_state(semaphore_t semap);

#endif

