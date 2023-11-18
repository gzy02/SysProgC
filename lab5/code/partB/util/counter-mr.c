#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "key-value.h"
#include "../lib/map-reduce.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




list_t mr_map(char *s){
    list_t kvs = list_new();
    char *delim = " ";
    char *saveptr;
    char *token = strtok_r(s, delim, &saveptr);
    while(token){
        kv_t kv = kv_new(token, "1");
        list_append(kvs, kv);
        token = strtok_r(0, delim, &saveptr);
    }
    return kvs;
}

// take as input a key, and list of all associated values
// return an aggregation
char *mr_reduce(char *key, list_t values){
    char *buf = calloc(1024, sizeof(*buf));
    sprintf(buf, "%d", values->last);
    return buf;
}



