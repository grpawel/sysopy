#ifndef __ARRAY_H_
#define __ARRAY_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct array
{
    int len_max;
    int len_cur;
    size_t elem_size;
    void* buf;
}* array_t;

bool array_create(array_t* array, size_t elem_size, int len_initial);

void array_destroy(array_t array);

bool array_insert_last(array_t array, void* elem);

bool array_remove(array_t array, int i);

bool array_isempty(array_t array);

int array_len(array_t array);

void* array_get(array_t array, int i);

void* array_raw(array_t array);
//void* array_foreach(array_t array, void* (*f)(void* elem, void* f_ret), void* f_init);

#endif
