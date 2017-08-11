#ifndef __STACK_H
#define __STACK_H

typedef struct
{
    int maxsize;
    void* data;
    size_t typesize;
    int top;
} Stack;

Stack* stack_alloc(int maxsize, size_t typesize);
void stack_free(Stack* stack);

bool stack_isempty(Stack *stack);
bool stack_isfull(Stack *stack);
void* stack_peek(Stack *stack);
void* stack_pop(Stack *stack);
bool stack_push(Stack *stack, void* elem);

#endif
