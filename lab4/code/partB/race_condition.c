#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 10
#define NUM_ITERATIONS 10000

long count = 0;

void *increment_count(void *arg)
{
    for (long i = 0; i < NUM_ITERATIONS; i++)
    {
        count++;
    }
    pthread_exit(0);
}

void *decrement_count(void *arg)
{
    for (long i = 0; i < NUM_ITERATIONS; i++)
    {
        count--;
    }
    pthread_exit(0);
}

int main()
{
    pthread_t threads[NUM_THREADS];

    for (long i = 0; i < NUM_THREADS / 2; i++)
    {
        pthread_create(&threads[i], NULL, increment_count, NULL);
    }
    for (long i = NUM_THREADS / 2; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, decrement_count, NULL);
    }
    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Expected count: 0 (because half increment and half decrement)\n");
    printf("Actual count: %ld\n", count);
    pthread_exit(0);
}
