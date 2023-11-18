#include <assert.h>
#include <pthread.h>
#include <stdio.h>

void *start(void *a)
{
    long n = (long)a;
    pthread_exit((void *)(n * n));
}

int main()
{
    long N = 10000;
    pthread_t tids[N + 1];
    long sum = 0;
    // Exercise 1: Implement your code here
    // Add your code here:
    for (long i = 0; i <= N; ++i)
    {
        pthread_create(&tids[i], 0, start, (void *)i);
    }
    for (long i = 0; i <= N; ++i)
    {
        void *res;
        pthread_join(tids[i], &res);
        sum += (long)res;
    }

    printf("sum: %ld\n", sum);
    // 添加断言
    long expected_sum = 0;
    for (long i = 0; i <= N; ++i)
    {
        expected_sum += i * i;
    }
    assert(sum == expected_sum); // 断言sum的值等于预期值
    pthread_exit(0);
}
