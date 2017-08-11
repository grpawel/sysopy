#include "shared_mem.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

memblock_t mem_open(size_t size, const char* name, bool create_new)
{
    memblock_t mem = calloc(1, sizeof(struct mem_block));
    if(!mem) {
        perror("calloc");
        goto cleanup;
    }
    int flags = O_RDWR;
    if(create_new) {
        flags |= O_CREAT | O_EXCL;
    }
    mem->created = create_new;
    mem->size = size;
    mem->name = strdup(name);
    int fd = shm_open(name, flags, S_IRWXU | S_IRWXG | S_IRWXO);
    if(fd == -1) {
        perror("shm_open");
        goto cleanup;
    }
    if(create_new) {
        if(ftruncate(fd, size) == -1) {
            perror("ftruncate");
            goto cleanup;
        }
    }
    mem->block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(mem->block == MAP_FAILED) {
        perror("mmap");
        goto cleanup;
    }
    int *p = mem->block;
    *p = 1;
    return mem;
    cleanup:
    if(!mem) return NULL;
    if(mem->name) {
        free(mem->name);
        mem->name = NULL;
    }
    if(mem->block != MAP_FAILED) mem_close(mem);
    return NULL;
}


void mem_close(memblock_t mem)
{
    if(!mem) {
        return;
    }
    if(munmap(mem->block, mem->size) == -1) {
        perror("munmap");
    }
    if(mem->created) {
        if(shm_unlink(mem->name) == -1) {
            perror("shm_unlink");
        }
    }
    if(mem->name) free(mem->name);
    free(mem);
}
