#include "lf-stack.h"
#include <stdio.h>
#include <stdlib.h>

void lf_stack_init(lf_stack_t *stack)
{
    atomic_store(&stack->top, 0);
    return;
}
// The key idea to build a lock-free stack is to compare and swap the stack top atomically.
void lf_stack_push(lf_stack_t *stack, int value)
{
    Node_t *new_node = (Node_t *)malloc(sizeof(Node_t));
    new_node->value = value;
    new_node->next = stack->top;

    // 当前值(stack->top)与期望值(expect)相等时，修改当前值为设定值(desired)，返回true，跳出循环
    // 当前值与期望值不等时，将期望值修改为当前值，返回false，继续循环
    while (!atomic_compare_exchange_weak(&stack->top, &new_node->next, new_node))
        ;
}

int lf_stack_pop(lf_stack_t *stack)
{
    Node_t *pop_node = stack->top;
    while (pop_node && !atomic_compare_exchange_weak(&stack->top, &pop_node, pop_node->next))
        ;
    if (!pop_node) // stack is empty
        return 0;
    int value = pop_node->value;
    free(pop_node);
    return value;
}

int lf_stack_size(lf_stack_t *stack)
{
    int count = 0;
    Node_t *node = stack->top;
    while (node)
    {
        count++;
        node = node->next;
    }
    return count;
}
