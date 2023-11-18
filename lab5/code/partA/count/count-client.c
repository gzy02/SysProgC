#include "count-rpcs.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 20

#define TODO()                                                                \
    do                                                                        \
    {                                                                         \
        extern int printf(char *, ...);                                       \
        printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__); \
    } while (0)

void *increment_thread(void *arg)
{
    for (int i = 0; i < 1000; ++i)
    {
        increment();
    }
    pthread_exit(0);
}

void *decrement_thread(void *arg)
{
    for (int i = 0; i < 1000; ++i)
    {
        decrement();
    }
    pthread_exit(0);
}

int main()
{
    pthread_t threads[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS / 2; i++)
    {
        pthread_create(&threads[i], NULL, increment_thread, NULL);
    }
    for (long i = NUM_THREADS / 2; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, decrement_thread, NULL);
    }
    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Expected count: 0 (because half increment and half decrement)\n");
    assert(get_count() == 0);
    printf("Actual count: %lld\n", get_count());

    return 0;
}
