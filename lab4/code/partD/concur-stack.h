#ifndef CONCUR_STACK_H
#define CONCUR_STACK_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include <pthread.h>

typedef struct Node_t{
    int value;
    struct Node_t *next;
} node_t;

typedef struct Concur_stack_t{
    pthread_mutex_t mu;
    node_t *top;
} concur_stack_t;

void concur_stack_init(concur_stack_t *stack);
void concur_stack_push(concur_stack_t *stack, int value);
int concur_stack_pop(concur_stack_t *stack);
int concur_stack_size(concur_stack_t *stack);


#endif
