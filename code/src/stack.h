#ifndef __STACK_H__
#define __STACK_H__

typedef struct Stack {
    int *data;
    int head; // The next index
} stack_t;

bool stack_init(stack_t *stack, int max_size);

void stack_destroy(stack_t *stack);

void stack_push(stack_t *stack, int value);

int stack_pop(stack_t *stack);

void stack_clear(stack_t *stack);

int stack_get_size(stack_t *stack);

int stack_get_value_at(stack_t *stack, int id);

void stack_remove_from_beginning(stack_t *stack, int count);

#endif
