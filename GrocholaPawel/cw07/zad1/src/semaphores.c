#include "semaphores.h"

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "logger.h"


bool created = false;

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

semmut_data semmut_init(bool create_new, int count, const char* ftok_path, int ftok_id)
{
    semmut_data data = malloc(sizeof(struct _semmut_data));
    if(!data) {
        perror("malloc");
        return NULL;
    }
    data->count = count;
    data->next_num = 0;
    data->created = create_new;
    key_t key = ftok(ftok_path, ftok_id);
    if(key == -1) {
        perror("ftok");
        return NULL;
    }
    int flags = 0;
    if(create_new) {
        flags |= IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO;
    }
    data->semset_id = semget(key, data->count, flags);
    if(data->semset_id == -1) {
        perror("semget");
        return NULL;
    }
    return data;
}

semmut_data semmut_init_private(int count)
{
    semmut_data data = malloc(sizeof(struct _semmut_data));
    if(!data) {
        perror("malloc");
        return NULL;
    }
    data->count = count;
    data->next_num = 0;
    data->created = true;
    int flags = IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO;
    data->semset_id = semget(IPC_PRIVATE, data->count, flags);
    if(data->semset_id == -1) {
        perror("semget");
        return NULL;
    }
    return data;
}

void semmut_deinit(semmut_data data)
{
    if(data->created) {
        if(semctl(data->semset_id, 0, IPC_RMID) == -1) {
            perror("semctl");
        }
    }
    free(data);
}

mutex_t mutex_create(semmut_data data)
{
    if(data->next_num == data->count) {
        return NULL;
    }
    mutex_t mut = malloc(sizeof(struct mutex));
    if(!mut) {
        perror("malloc");
        return NULL;
    }
    mut->semset_id = data->semset_id;
    mut->num = data->next_num++;

    return mut;
}

void mutex_delete(mutex_t mut)
{
    free(mut);
}

void mutex_wait(mutex_t mut)
{
    struct sembuf ops;
    ops.sem_num = mut->num;
    ops.sem_flg = 0;
    ops.sem_op = -1;
    if(semop(mut->semset_id, &ops, 1) == -1) {
        perror("semop");
    }
}
void mutex_signal(mutex_t mut)
{
    int val = semctl(mut->semset_id, mut->num, GETVAL);
    if(val == -1) {
        perror("semctl");
    }
    if(val > 1) {
        log_debug("Mutex has value of 2!");
        return;
    }
    struct sembuf ops;
    ops.sem_num = mut->num;
    ops.sem_flg = 0;
    ops.sem_op = 1;
    if(semop(mut->semset_id, &ops, 1) == -1) {
        perror("semop");
    }
}

void mutex_set_state(mutex_t mut, int state)
{
    if(state != 0 && state != 1) {
        log_debug("Invalid state set for mutex: %d", state);
    }
    union semun arg = {.val = state};
    if(semctl(mut->semset_id, mut->num, SETVAL, arg) == -1) {
        perror("semctl");
    }
}

semaphore_t semap_create(semmut_data data)
{
    if(data->next_num == data->count) {
        return NULL;
    }
    semaphore_t semap = malloc(sizeof(struct semaphore));
    if(!semap) {
        perror("malloc");
        return NULL;
    }
    semap->semset_id = data->semset_id;
    semap->num = data->next_num++;

    return semap;
}

void semap_delete(semaphore_t semap)
{
    free(semap);
}

void semap_wait(semaphore_t semap)
{
    struct sembuf ops;
    ops.sem_num = semap->num;
    ops.sem_flg = 0;
    ops.sem_op = -1;
    if(semop(semap->semset_id, &ops, 1) == -1) {
        perror("semop");
    }
}
void semap_signal(semaphore_t semap)
{
    struct sembuf ops;
    ops.sem_num = semap->num;
    ops.sem_flg = 0;
    ops.sem_op = 1;
    if(semop(semap->semset_id, &ops, 1) == -1) {
        perror("semop");
    }
}

int semap_get_state(semaphore_t semap)
{
    int val = semctl(semap->semset_id, semap->num, GETVAL);
    if(val == -1) {
        perror("semctl");
    }
    return val;
}

void semap_set_state(semaphore_t semap, int state)
{
    if(state < 0) {
        log_debug("Invalid state set for mutex: %d", state);
    }
    union semun arg = {.val = state};
    if(semctl(semap->semset_id, semap->num, SETVAL, arg) == -1) {
        perror("semctl");
    }
}
