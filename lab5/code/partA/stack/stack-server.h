#ifndef STACK_H
#define STACK_H

#include <pthread.h>
#include <stdatomic.h>

typedef struct Node_t
{
    int value;
    struct Node_t *next;
} node_t;
typedef _Atomic(node_t *) atomic_node;
typedef struct Stack_t
{
    // Exercise: your code here
    // Add your code here:
    atomic_node top;
} stack_t;

void stack_init();
void stack_push(int value);
int stack_pop();
int stack_size();

#endif
