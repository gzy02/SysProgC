#ifndef LF_STACK_H
#define LF_STACK_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include <stdatomic.h>

typedef struct Node_t{
    int value;
    struct Node_t *next;
} Node_t;

typedef _Atomic(Node_t *) atomic_node;

typedef struct Lf_stack_t{
    atomic_node top;
} lf_stack_t;

void lf_stack_init(lf_stack_t *stack);
void lf_stack_push(lf_stack_t *stack, int value);
int lf_stack_pop (lf_stack_t *stack);
int lf_stack_size(lf_stack_t *stack);


#endif