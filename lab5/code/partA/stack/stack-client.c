#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "stack-rpcs.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define TESTSIZE 1000

int main(){
    int raw_size = Stack_size();
    for(int i=0; i<TESTSIZE; i++)
        Stack_push(i);
    assert(Stack_size() == raw_size + TESTSIZE);
    for(int i=TESTSIZE-1; i>=0; i--){
        int value = Stack_pop(i);
        assert(value == i);
    }
    assert(Stack_size() == raw_size);
    printf("PASS!\n");
}