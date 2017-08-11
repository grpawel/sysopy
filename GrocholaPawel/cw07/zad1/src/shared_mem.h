#ifndef __SHARED_MEM_H_
#define __SHARED_MEM_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct mem_block
{
    int id;
    size_t size;
    void* block;
    bool created;
}* memblock_t;

memblock_t mem_open(bool create, size_t size, const char* ftok_path, int ftok_id);
void mem_close(memblock_t mem);
#endif
