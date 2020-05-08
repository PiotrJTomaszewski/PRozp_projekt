#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

bool stack_init(stack_t *stack, int max_size) {
    stack->head = 0;
    stack->data = malloc(max_size * sizeof stack->data);
    return (stack == NULL);
}

void stack_destroy(stack_t *stack) {
    free(stack->data);
}

void stack_push(stack_t *stack, int value) {
    stack->data[(stack->head)++] = value;
}

int stack_pop(stack_t *stack) {
    return stack->data[--(stack->head)];
}

void stack_clear(stack_t *stack) {
    stack->head = 0;
}

int stack_get_size(stack_t *stack) {
    return stack->head;
}

int stack_get_value_at(stack_t *stack, int id) {
    return stack->data[id];
}

void stack_remove_from_beginning(stack_t *stack, int count) {
    stack->head -= count;
    memmove(stack->data, stack->data+count, stack->head);
}
