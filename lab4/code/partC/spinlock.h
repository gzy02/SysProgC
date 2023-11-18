#ifndef SPINLOCK_H
#define SPINLOCK_H

#define TODO()                                                                \
    do                                                                        \
    {                                                                         \
        extern int printf(char *, ...);                                       \
        printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__); \
    } while (0)

#include <stdatomic.h>

typedef struct Spinlock_t
{
    atomic_int n;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

#endif
