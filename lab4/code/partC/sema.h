#ifndef SEMA_H
#define SEMA_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include <stdatomic.h>

typedef struct {
    atomic_int value;
} sema_t;

void sema_init(sema_t *sem, int v);
void sema_wait(sema_t *sem);
void sema_post(sema_t *sem);



#endif

