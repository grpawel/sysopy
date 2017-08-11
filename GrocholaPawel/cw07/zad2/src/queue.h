#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct queue
{
    int len_max;
    int len_cur;
    int elem_next;
    size_t elem_size;
    uint8_t array_start;
}* queue_t;

//size of memory block pointed by mem must be at least len_max * elem_size + sizeof(struct queue)
queue_t queue_create(int len_max, size_t elem_size, void* mem);

//returns memory size for queue
size_t queue_memsize(int len_max, int elem_size);

bool queue_insert(queue_t queue, void* elem);

bool queue_remove(queue_t queue, void* buf);

bool queue_peek(queue_t queue, void* buf);

int queue_free_slots(queue_t queue);

bool queue_isempty(queue_t queue);

int queue_len(queue_t queue);

void queue_metaprint(queue_t queue);

void* queue_foreach(queue_t queue, void* (*f)(void* elem, void* f_ret), void* f_init);

#endif
