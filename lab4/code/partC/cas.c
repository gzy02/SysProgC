#include "cas.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




unsigned long
atomic_cmp_set(volatile long *ptr, unsigned long old,
               unsigned long set){
    unsigned long prev;
    __asm__ volatile(
    "LOCK\n"
    "cmpxchgq %2,%1"
    : "=a"(prev), "+m"(*(volatile long *)(ptr))
    : "r"(set), "0"(old)
    : "memory");
    //printf("prev=%ld\n", prev);
    return prev;
}
