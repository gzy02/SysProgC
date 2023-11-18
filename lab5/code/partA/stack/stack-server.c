#include "stack-server.h"
#include <stdio.h>
#include <stdlib.h>

stack_t stack;

void stack_init()
{
    // Exercise:
    // Add your code here:
    atomic_init(&stack.top, NULL);
    return;
}

void stack_push(int value)
{
    // Exercise:
    // Add your code here:
    node_t *new_node = malloc(sizeof(node_t));
    new_node->value = value;

    new_node->next = atomic_load(&stack.top);
    while (!atomic_compare_exchange_weak(&stack.top, &new_node->next, new_node))
        ;
}

int stack_pop()
{
    int value = -1;
    // Exercise:
    // Add your code here:
    node_t *del = atomic_load(&stack.top);
    while (del && !atomic_compare_exchange_weak(&stack.top, &del, del->next))
        ;
    if (del)
    {
        value = del->value;
        free(del);
    }
    return value;
}

int stack_size()
{
    int size = 0;
    // Exercise:
    // Add your code here:
    node_t *cur = stack.top;
    while (cur)
    {
        cur = cur->next;
        ++size;
    }
    return size;
}
