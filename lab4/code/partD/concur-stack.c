#include "concur-stack.h"
#include <stdio.h>
#include <stdlib.h>

void concur_stack_init(concur_stack_t *stack)
{
    pthread_mutex_init(&stack->mu, NULL);
    return;
}

void concur_stack_push(concur_stack_t *stack, int value)
{
    pthread_mutex_lock(&stack->mu);
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = stack->top;
    stack->top = new_node;
    pthread_mutex_unlock(&stack->mu);
}

int concur_stack_pop(concur_stack_t *stack)
{
    int value = -1;

    if (stack->top)
    {
        pthread_mutex_lock(&stack->mu);
        node_t *head = stack->top;
        value = head->value;
        stack->top = head->next;
        free(head);
        pthread_mutex_unlock(&stack->mu);
    }

    return value;
}

int concur_stack_size(concur_stack_t *stack)
{
    int size = 0;
    pthread_mutex_lock(&stack->mu);
    node_t *cur = stack->top;
    while (cur)
    {
        ++size;
        cur = cur->next;
    }
    pthread_mutex_unlock(&stack->mu);
    return size;
}
