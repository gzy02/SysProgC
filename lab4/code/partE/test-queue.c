#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "lf-queue.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define TESTSIZE 1000

struct Lf_queue_t lf_queue;


void *work_queue_enqueue(void *arg){
    for(int i=0; i<TESTSIZE; i++)
        lf_queue_enqueue(&lf_queue, i);
}

void *work_queue_dequeue(void *arg){
    for(int i=0; i<TESTSIZE; i++)
        lf_queue_dequeue(&lf_queue);
}

int main(){
    lf_queue_init(&lf_queue);

     for(int i=0; i<TESTSIZE*2; i++)
        lf_queue_enqueue(&lf_queue, 1);
        
    pthread_t threads[4];
    for(int i=0; i<2; i++)
        pthread_create(&threads[i], NULL, work_queue_enqueue, (void*)0);
    
    for(int i=2; i<4; i++)
        pthread_create(&threads[i], NULL, work_queue_dequeue, (void*)0);

    for(int i=0; i<4; i++)
        pthread_join(threads[i], NULL);
    
    int count = lf_queue_size(&lf_queue);
    assert(count==TESTSIZE * 2);

    printf("PASS!\n");

    return 0;
}