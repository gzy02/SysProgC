#ifndef CONCUR_HASH_H
#define CONCUR_HASH_H

#include <pthread.h>

typedef struct Node_t
{
    char *key;
    char *value;
    struct Node_t *next;
} node_t;

typedef struct Hash_t
{
    int num_buckets;
    // 以下成员都是长为num_buckets的数组指针
    node_t **buckets;
    pthread_rwlock_t *mu;
} hash_t;

void hash_init(hash_t *hash, int num_buckets);
void hash_insert(hash_t *ht, char *key, char *value);
char *hash_lookup(hash_t *ht, char *key);
int hash_delete(hash_t *ht, char *key);
int hash_size(hash_t *ht);

#endif
