#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "concur-hash.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define TEST_SIZE 10000
#define KEY_SIZE 10

static struct Hash_t hash;
static char *testkey[TEST_SIZE];

void *work_hash_insert(void *arg){
    char *fmt = (char *)arg;
    for(int i=0; i<TEST_SIZE; i++) {
        char buf[1024];
        sprintf(buf, fmt, i);
        hash_insert(&hash, strdup(buf), fmt);
    }
    pthread_exit(0);
}

void *work_hash_lookup(void *arg){
    char *fmt = (char *)arg;
    for(int i=0; i<TEST_SIZE; i++) {
        char buf[1024];
        sprintf(buf, fmt, i);
        assert(strcmp(hash_lookup(&hash, strdup(buf)), fmt) == 0);
    }
    pthread_exit(0);
}

void *work_hash_delete(void *arg){
    char *fmt = (char *)arg;
    for(int i=0; i<TEST_SIZE; i++) {
        char buf[1024];
        sprintf(buf, fmt, i);
        hash_delete(&hash, strdup(buf));
    }
    pthread_exit(0);
}

int main(){
    hash_init(&hash, 128);

    pthread_t threads[2];

    pthread_create(&threads[0], NULL, work_hash_insert, (void*)"1_%d");
    pthread_create(&threads[1], NULL, work_hash_insert, (void*)"2_%d");
    for(int i=0; i<2; i++)
        pthread_join(threads[i], NULL);
    assert(hash_size(&hash) == 2*TEST_SIZE);

    pthread_create(&threads[0], NULL, work_hash_lookup, (void*)"1_%d");
    pthread_create(&threads[1], NULL, work_hash_lookup, (void*)"2_%d");
    for(int i=0; i<2; i++)
        pthread_join(threads[i], NULL);
    assert(hash_size(&hash) == 2*TEST_SIZE);

    pthread_create(&threads[0], NULL, work_hash_delete, (void*)"1_%d");
    pthread_create(&threads[1], NULL, work_hash_delete, (void*)"2_%d");
    for(int i=0; i<2; i++)
        pthread_join(threads[i], NULL);
    assert(hash_size(&hash) == 0);

    printf("PASS!\n");
    return 0;
}