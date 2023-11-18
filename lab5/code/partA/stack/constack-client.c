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


void *work_stack_push(void *arg){
    for(int i=0; i<TESTSIZE; i++)
        Stack_push(i);
}

void *work_stack_pop(void *arg){
    for(int i=0; i<TESTSIZE; i++)
        Stack_pop();
}


int main(){
    int raw_size = Stack_size();
    for(int i=0; i<TESTSIZE * 2; i++)
        Stack_push(1);
        
    pthread_t threads[4];
    for(int i=0; i<2; i++)
        pthread_create(&threads[i], NULL, work_stack_push, (void*)0);
    
    for(int i=2; i<4; i++)
        pthread_create(&threads[i], NULL, work_stack_pop, (void*)0);

    for(int i=0; i<4; i++)
        pthread_join(threads[i], NULL);
    
    ;
    assert(Stack_size() == raw_size + TESTSIZE * 2);
    printf("Pass!\n");

    return 0;
}