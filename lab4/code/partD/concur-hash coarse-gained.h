#ifndef CONCUR_HASH_H
#define CONCUR_HASH_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include <pthread.h>

typedef struct Node_t {
    char *key;
    char *value;
    struct Node_t *next;
} node_t;

typedef struct Hash_t {
    pthread_mutex_t mu;
    node_t **buckets;
    int num_buckets;
} hash_t;

void hash_init(hash_t *hash, int num_buckets);
void hash_insert(hash_t *ht, char *key, char *value);
char *hash_lookup(hash_t *ht, char *key);
int hash_delete(hash_t *ht, char *key);
int hash_size(hash_t *ht);

#endif
