#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "key-value.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)





kv_t kv_new(char *key, char *value) {
    kv_t kv = calloc(1, sizeof(*kv));
    assert(kv);
    kv->key = strdup(key);
    assert(kv->key);
    kv->value = strdup(value);
    assert(kv->value);
    return kv;
}

void kv_free(kv_t kv) {
    free(kv->key);
    free(kv->value);
    free(kv);
}


void kv_print(kv_t kv){
    printf("{%s: %s}\n", kv->key, kv->value);
}



