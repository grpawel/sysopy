#ifndef __SHARED_MEM_H_
#define __SHARED_MEM_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct mem_block
{
    size_t size;
    void* block;
    bool created;
    char* name;
}* memblock_t;

memblock_t mem_open(size_t size, const char* name, bool create_new);
void mem_close(memblock_t mem);
#endif
