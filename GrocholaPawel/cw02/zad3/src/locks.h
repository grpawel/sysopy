#ifndef _LOCKS_H
#define _LOCKS_H
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    LOCK_READ,
    LOCK_WRITE
} LOCK_TYPE;

typedef struct _Lock
{
    uint8_t byte;
    int offset;
    int pid;
    LOCK_TYPE type;
} Lock;

int byte_setlock(int pfd, int offset, LOCK_TYPE type, bool is_blocking);
Lock* byte_getlock(int pfd, int offset, LOCK_TYPE type);

void byte_freelock(int pfd, int offset);

void* lock_print(Lock* lock, void* ignored);

void* file_locks_traverse(int pfd, void*(*func)(Lock*, void*), void* acc);

void byte_write(int pfd, int offset, uint8_t byte);
uint8_t byte_read(int pfd, int offset);

#endif
