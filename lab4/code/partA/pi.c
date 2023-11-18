#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct arg_t
{
    float x;
    float y;
};

void *start(void *__arg)
{
    // Exercise 2: Implement your code here
    // Add your code here:
    struct arg_t *arg = (struct arg_t *)__arg;
    float x = arg->x, y = arg->y;
    long ret = (x * x + y * y <= 1);
    //((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5) < 0.25);
    pthread_exit((void *)ret);
}

int main(int argc, char **argv)
{
    long N1 = atoll(argv[1]);
    pthread_t tids[N1];

    // pi(concurrency)
    for (long i = 0; i < N1; ++i)
    {
        struct arg_t *arg = malloc(sizeof(*arg));
        int x = rand();
        int y = rand();
        arg->x = 1.0 * x / RAND_MAX;
        arg->y = 1.0 * y / RAND_MAX;
        pthread_create(&tids[i], 0, start, arg);
    }

    void *res;
    long M = 0;
    for (long i = 0; i < N1; ++i)
    {
        pthread_join(tids[i], &res);
        M += (long)res;
    }
    printf("pai = %f\n", 4.0 * M / N1);

    assert(4.0 * M / N1 > 3.1 && 4.0 * M / N1 < 3.2);

    pthread_exit(0);
}
