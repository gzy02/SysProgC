#ifndef COUNT_SERVER_H
#define COUNT_SERVER_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




int increment();
int decrement();
int get_count();

#endif