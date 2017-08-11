#include "semaphores.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "logger.h"
#include "utils.h"

mutex_t mutex_create(const char* name, bool create_new, int init_val)
{
    if(init_val != 0 && init_val != 1) {
        log_debug("Mutex has invalid initial value: %d", init_val);
        return NULL;
    }
    mutex_t mutex = malloc(sizeof(struct mutex));
    if(!mutex) {
        perror("malloc");
        return NULL;
    }
    mutex->name = strdup(name);
    mutex->created = create_new;
    int flags = 0;
    if(create_new) {
        flags |= O_CREAT | O_EXCL;
    }
    sem_t* sem = sem_open(name, flags, S_IRWXU | S_IRWXG | S_IRWXO, init_val);
    if(sem == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    mutex->sem = sem;

    return mutex;
}

void mutex_delete(mutex_t mutex)
{
    if(!mutex) {
        return;
    }
    sem_close(mutex->sem);
    if(mutex->created) {
        sem_unlink(mutex->name);
    }
    free(mutex->name);
    free(mutex);
}

void mutex_wait(mutex_t mutex)
{
    if(sem_wait(mutex->sem) == -1) {
        perror("sem_wait");
    }
}

void mutex_signal(mutex_t mutex)
{
    int sem_getvalue(sem_t *sem, int *valp);
    int val = 0;
    if(sem_getvalue(mutex->sem, &val) == -1) {
        perror("sem_getvalue");
    }
    if(val > 1) {
        log_debug("Mutex has value of %d!", val);
        return;
    }
    if(sem_post(mutex->sem) == -1) {
        perror("sem_post");
    }
}

semaphore_t semap_create(const char* name, bool create_new, int init_val)
{
    semaphore_t semap = malloc(sizeof(struct semaphore));
    if(!semap) {
        perror("malloc");
        return NULL;
    }
    int flags = 0;
    if(create_new) {
        flags |= O_CREAT | O_EXCL;
    }
    sem_t* sem = sem_open(name, flags, S_IRWXU | S_IRWXG | S_IRWXO, init_val);
    if(sem == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    semap->sem = sem;
    semap->name = strdup(name);
    semap->created = create_new;

    return semap;
}

void semap_delete(semaphore_t semap)
{
    if(!semap) {
        return;
    }
    sem_close(semap->sem);
    if(semap->created) {
        sem_unlink(semap->name);
    }
    free(semap->name);
    free(semap);
}

void semap_wait(semaphore_t semap)
{
    if(sem_wait(semap->sem) == -1) {
        perror("sem_wait");
    }
}

void semap_signal(semaphore_t semap)
{
    if(sem_post(semap->sem) == -1) {
        perror("sem_post");
    }
}

int semap_get_state(semaphore_t semap)
{
    int val = -1;
    if(sem_getvalue(semap->sem, &val) == -1) {
        perror("sem_getvalue");
        return -1;
    }
    return val;
}
