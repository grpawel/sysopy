#include "locks.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static struct flock* flock_alloc(__off_t start, __off_t len, short whence, short type)
{
    struct flock* fl = malloc(sizeof(struct flock));
    fl->l_start = start;
    fl->l_len = len;
    fl->l_whence = whence;
    fl->l_type = type;
    return fl;
}
static short flock_type(LOCK_TYPE type)
{
    switch(type) {
        case LOCK_READ: {
            return F_RDLCK;
        }
        case LOCK_WRITE: {
            return F_WRLCK;
        }
    }
    return 0; //should not happen
}
int byte_setlock(int pfd, int offset, LOCK_TYPE type, bool is_blocking)
{
    struct flock* fl = flock_alloc(
        offset, 1, SEEK_SET, flock_type(type));
    int err;
    if(!is_blocking) {
        err = fcntl(pfd, F_SETLK, fl);
    }
    else {
        err = fcntl(pfd, F_SETLKW, fl);
    }
    free(fl);
    return err;
}

void byte_freelock(int pfd, int offset)
{
    struct flock* fl = flock_alloc(offset, 1, SEEK_SET, F_UNLCK);
    fcntl(pfd, F_SETLK, fl);
    free(fl);
}
Lock* byte_getlock(int pfd, int offset, LOCK_TYPE type)
{
    struct flock* fl = flock_alloc(offset, 1, SEEK_SET, flock_type(type));
    fcntl(pfd, F_GETLK, fl);
    Lock* lock = NULL;
    if(fl->l_type != F_UNLCK) {
        lock = malloc(sizeof(Lock));
        lock->offset = offset;
        lock->pid = fl->l_pid;
        lock->type = type;
    }
    free(fl);
    return lock;
}

void* lock_print(Lock* lock, void* ignored)
{
    char typestr[5];
    strcpy(typestr, lock->type == LOCK_READ ? "write" : "read ");
    printf("%s: pid: %d (byte 0x%x ('%c'), offset %d)", typestr, lock->pid, lock->byte, lock->byte, lock->offset);
    return ignored;
}

void* file_locks_traverse(int pfd, void*(*func)(Lock*, void*), void* acc)
{
    off_t size = lseek(pfd, 0, SEEK_END);
    off_t current = 0;
    while(lseek(pfd, current, SEEK_SET) != size) {
        Lock* lock = byte_getlock(pfd, current, LOCK_READ);
        if(lock != NULL) {
            acc = func(lock, acc);
            free(lock);
        }
        lock = byte_getlock(pfd, current, LOCK_WRITE);
        if(lock != NULL) {
            acc = func(lock, acc);
            free(lock);
        }
        current++;
    }
    return acc;
}
void byte_write(int pfd, int offset, uint8_t byte)
{
    pwrite(pfd, &byte, 1, offset);
}

uint8_t byte_read(int pfd, int offset)
{
    uint8_t byte;
    pread(pfd, &byte, 1, offset);
    return byte;
}
