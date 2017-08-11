#include "shared_mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

memblock_t mem_open(bool create_new, size_t size, const char* ftok_path, int ftok_id)
{
    memblock_t mem = malloc(sizeof(struct mem_block));
    if(!mem) {
        perror("malloc");
        goto cleanup;
    }
    key_t key = ftok(ftok_path, ftok_id);
    if(key == -1) {
        perror("ftok");
        goto cleanup;
    }
    int flags = 0;
    if(create_new) {
        flags |= IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO;
    }
    mem->id = shmget(key, size, flags);
    if(mem->id == -1) {
        perror("shmget");
        goto cleanup;
    }
    mem->created = create_new;
    mem->size = size;

    mem->block = shmat(mem->id, NULL, 0);
    if(mem->block == (void*) -1) {
        perror("shmat");
        goto cleanup;
    }
    return mem;
    cleanup:
    if(mem) free(mem);
    return NULL;
}


void mem_close(memblock_t mem)
{
    if(!mem) {
        return;
    }
    if(shmdt(mem->block) == -1) {
        perror("shmdt");
    }
    if(mem->created) {
        if(shmctl(mem->id, IPC_RMID, NULL) == -1) {
            perror("shmctl");
        }
    }
    free(mem);
}
