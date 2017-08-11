#include "queue.h"

#include <stdio.h>
#include <string.h>

#include "logger.h"
//size of memory block pointed by mem must be at least len_max * elem_size + sizeof(struct queue)
queue_t queue_create(int len_max, size_t elem_size, void* mem)
{
    uint8_t* byte_cur = mem;
    queue_t queue = (struct queue*) byte_cur;
    byte_cur += sizeof(struct queue);
    queue->len_max = len_max;
    queue->len_cur = 0;
    queue->elem_next = 0;
    queue->elem_size = elem_size;

    return queue;
}

bool queue_insert(queue_t queue, void* elem)
{
    uint8_t* array = &(queue->array_start);
    if(queue->len_cur >= queue->len_max) {
        return false;
    }
    memcpy(array + queue->elem_next * queue->elem_size, elem, queue->elem_size);
    queue->len_cur++;
    queue->elem_next++;
    if(queue->elem_next == queue->len_max) {
        queue->elem_next = 0;
    }
    return true;
}

bool queue_remove(queue_t queue, void* buf)
{
    uint8_t* array = &(queue->array_start);
    if(queue->len_cur == 0) {
        return false;
    }
    int first_elem = queue->elem_next - queue->len_cur;
    if(first_elem < 0) {
        first_elem += queue->len_max;
    }
    memcpy(buf, array + first_elem * queue->elem_size, queue->elem_size);
    queue->len_cur--;
    return true;
}

bool queue_peek(queue_t queue, void* buf)
{
    uint8_t* array = &(queue->array_start);
    if(queue->len_cur == 0) {
        return false;
    }
    int first_elem = queue->elem_next - queue->len_cur;
    if(first_elem < 0) {
        first_elem += queue->len_max;
    }
    memcpy(buf, array + first_elem * queue->elem_size, queue->elem_size);
    return true;
}

int queue_freespaces(queue_t queue)
{
    return queue->len_max - queue->len_cur;
}

bool queue_isempty(queue_t queue)
{
    return queue->len_cur == queue->len_max;
}

int queue_len(queue_t queue)
{
    return queue->len_cur;
}

void queue_metaprint(queue_t queue)
{
    printf("len_max %d, len_cur %d, elem_next %d, elem_size %ld\n", queue->len_max, queue->len_cur, queue->elem_next, queue->elem_size);
}

size_t queue_memsize(int len_max, int elem_size)
{
    //-1 because struct and following array overlap
    return sizeof(struct queue) - 1 + len_max * elem_size;
}
/*
void* queue_foreach(queue_t queue, void* (*f)(void* elem, void* f_ret), void* f_init)
{
    int cur = queue->elem_next - queue->len_cur;
    if(cur < 0) {
        cur += queue->len_max;
    }
    void* f_ret = f_init;
    for(int i = 0; i < queue->len_cur; i++) {
        f_ret = f(queue->array[cur], f_ret);
        cur++;
        if(cur >= queue->len_max) {
            cur = 0;
        }
    }
    return f_ret;
}
*/
