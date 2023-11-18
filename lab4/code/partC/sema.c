#include "sema.h"
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

void sema_init(sema_t *sem, int v)
{
    atomic_init(&sem->value, v);
}

void sema_wait(sema_t *sem)
{
    int expected, desired;
    do
    {
        while ((expected = atomic_load(&sem->value)) <= 0)
        {
            // busy-wait/spin until value is greater than 0
        }
        desired = expected - 1;
    } while (!atomic_compare_exchange_weak(&sem->value, &expected, desired));
    // 当前值与期望值(expect)相等时，修改当前值为设定值(desired)，返回true，跳出循环
    // 当前值与期望值不等时，将期望值修改为当前值，返回false，继续循环
}

void sema_post(sema_t *sem)
{
    atomic_fetch_add(&sem->value, 1);
}

typedef struct
{
    sema_t sem;
    int num;
} counter_t;

counter_t counter;

void *start(void *arg)
{
    for (int i = 0; i < 10000; i++)
    {
        sema_wait(&counter.sem);
        counter.num++;
        sema_post(&counter.sem);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t pids[10];
    sema_init(&counter.sem, 1);

    for (int i = 0; i < 10; i++)
    {
        pthread_create(&pids[i], NULL, start, NULL);
    }

    for (int i = 0; i < 10; i++)
    {
        pthread_join(pids[i], NULL);
    }

    printf("Counter: %d\n", counter.num);
    assert(counter.num == 100000);
    return 0;
}