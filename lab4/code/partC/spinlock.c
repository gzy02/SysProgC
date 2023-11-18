#include "spinlock.h"
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
void spinlock_init(spinlock_t *lock)
{
    atomic_init(&lock->n, 0);
}

void spinlock_lock(spinlock_t *lock)
{
    // 原子替换obj指向的值desired并返回obj之前保存的值。
    while (atomic_exchange(&lock->n, 1))
    {
        // Spin until the lock is acquired
    }
}

void spinlock_unlock(spinlock_t *lock)
{
    atomic_store(&lock->n, 0);
}

typedef struct
{
    spinlock_t lock;
    int num;
} counter_t;

counter_t counter;

void *start()
{
    for (int i = 0; i < 10000; i++)
    {
        spinlock_lock(&counter.lock);
        counter.num++;
        spinlock_unlock(&counter.lock);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t pids[10];

    spinlock_init(&counter.lock);

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
    printf("Testing spinlock: PASS\n");
    return 0;
}