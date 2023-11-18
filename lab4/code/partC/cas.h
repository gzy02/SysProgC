#ifndef CAS_H
#define CAS_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




//static __inline__
unsigned long
atomic_cmp_set(volatile long *ptr, unsigned long old, unsigned long set);


#endif