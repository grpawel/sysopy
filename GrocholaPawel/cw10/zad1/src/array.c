#include "array.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


bool array_create(array_t* array, size_t elem_size, int len_initial)
{
    array_t new_array = malloc(sizeof(struct array));
    if(!array) {
        return false;
    }
    new_array->len_max = len_initial;
    new_array->len_cur = 0;
    new_array->elem_size = elem_size;
    new_array->buf = malloc(elem_size * len_initial);
    if(!new_array->buf) {
        free(new_array);
        return false;
    }
    *array = new_array;
    return true;
}

static void* array_getbufpos(array_t array, int i)
{
    return ((uint8_t*)array->buf) + array->elem_size * i;
}

static void array_grow(array_t array)
{
    int new_len = array->len_max * 1.5;
    size_t new_size = new_len * array->elem_size;
    array->buf = realloc(array->buf, new_size);
    array->len_max = new_len;
}

void array_destroy(array_t array)
{
    free(array->buf);
    free(array);
}

bool array_insert_last(array_t array, void* elem)
{
    if(array->len_cur == array->len_max) {
        array_grow(array);
    }
    void* buf_pos = array_getbufpos(array, array->len_cur);
    memcpy(buf_pos, elem, array->elem_size);
    array->len_cur++;
    return true;
}

bool array_remove(array_t array, int i)
{
    int last = array->len_cur - 1;
    array->len_cur--;
    if(i >= last) {
        return false;
    }
    void* buf_i = array_getbufpos(array, i);
    void* buf_next = array_getbufpos(array, i+1);
    size_t buf_len = (last - i) * array->elem_size;
    memmove(buf_i, buf_next, buf_len);
    //memcpy(buf_i, buf_last, array->elem_size);
    return true;
}

bool array_isempty(array_t array)
{
    return array->len_cur == 0;
}

int array_len(array_t array)
{
    return array->len_cur;
}

void* array_get(array_t array, int i)
{
    void* buf_pos = array_getbufpos(array, i);
    return buf_pos;
}

void* array_raw(array_t array)
{
    return array->buf;
}
