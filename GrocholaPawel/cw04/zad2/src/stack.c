#include "stack.h"


Stack* stack_alloc(int maxsize, size_t typesize)
{
    Stack *stack = malloc(sizeof(Stack));
    stack->data = malloc(size * typesize);
    stack->top = -1;
    stack->size = size;
    stack->typesize = typesize;
    return stack;
}

void stack_free(Stack* stack)
{
    free(stack->data);
    free(stack);
}

bool stack_isempty(Stack *stack)
{
    return stack->top == -1;
}

bool stack_isfull(Stack *stack)
{
    return stack->top == stack->maxsize;
}

void* stack_peek(Stack *stack)
{
    if(stack_isempty(stack)) return NULL;
    return stack->data[top];
}

void* stack_pop(Stack *stack) {
    if(stack_isempty(stack)) return NULL;
    void* data = stack->data[stack->top];
    stack->top--;
    return data;
}

bool stack_push(Stack *stack, void* elem) {
    if(stack_isfull(stack)) return false;
    stack->top++;
    stack->data[top] = elem;
    return true;
}
