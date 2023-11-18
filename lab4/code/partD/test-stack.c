#include "concur-stack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TESTSIZE 1000

struct Concur_stack_t con_stack;

void *work_stack_push(void *arg)
{
    for (int i = 0; i < TESTSIZE; i++)
        concur_stack_push(&con_stack, i);
}

void *work_stack_pop(void *arg)
{
    for (int i = 0; i < TESTSIZE; i++)
        concur_stack_pop(&con_stack);
}

int main()
{
    concur_stack_init(&con_stack);
    for (int i = 0; i < TESTSIZE * 2; i++)
        concur_stack_push(&con_stack, 1);

    pthread_t threads[4];
    for (int i = 0; i < 2; i++)
        pthread_create(&threads[i], NULL, work_stack_push, (void *)0);

    for (int i = 2; i < 4; i++)
        pthread_create(&threads[i], NULL, work_stack_pop, (void *)0);

    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    int count = concur_stack_size(&con_stack);
    assert(count == TESTSIZE * 2);

    printf("PASS!\n");
    return 0;
}
