#include "concur-hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// hash
/* this is the D. J. Bernstein hash function */
static unsigned int djb_hash(const char *cp)
{
    unsigned hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char)*cp++;
    return hash;
}

void hash_init(hash_t *ht, int num_buckets)
{
    pthread_mutex_init(&ht->mu, 0);
    ht->buckets = calloc(num_buckets, sizeof(*ht->buckets));
    // calloc() 函数将分配的内存全部初始化为零
    ht->num_buckets = num_buckets;
}

void hash_insert(hash_t *ht, char *key, char *value)
{
    pthread_mutex_lock(&ht->mu);
    int bucket_id = djb_hash(key) % ht->num_buckets;
    node_t *new_node = malloc(sizeof(node_t));
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    // 头插法
    new_node->next = ht->buckets[bucket_id];
    ht->buckets[bucket_id] = new_node;
    pthread_mutex_unlock(&ht->mu);
}

char *hash_lookup(hash_t *ht, char *key)
{
    pthread_mutex_lock(&ht->mu);
    int bucket_id = djb_hash(key) % ht->num_buckets;
    node_t *node = ht->buckets[bucket_id];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            pthread_mutex_unlock(&ht->mu);
            return node->value;
        }
        node = node->next;
    }
    pthread_mutex_unlock(&ht->mu);
    return NULL;
}

// return 0 for success; 1 for non-exiting "key"
int hash_delete(hash_t *ht, char *key)
{
    pthread_mutex_lock(&ht->mu);
    int bucket_id = djb_hash(key) % ht->num_buckets;
    node_t *cur = ht->buckets[bucket_id];
    node_t *prev = NULL;
    while (cur != NULL)
    {
        if (strcmp(cur->key, key) == 0)
        {
            if (prev == NULL)
            {
                ht->buckets[bucket_id] = cur->next;
            }
            else
            {
                prev->next = cur->next;
            }
            free(cur->key);
            free(cur->value);
            free(cur);
            pthread_mutex_unlock(&ht->mu);
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return 1;
}

int hash_size(hash_t *ht)
{
    int size = 0;
    pthread_mutex_lock(&ht->mu);
    for (int i = 0; i < ht->num_buckets; i++)
    {
        node_t *node = ht->buckets[i];
        while (node != NULL)
        {
            size++;
            node = node->next;
        }
    }
    pthread_mutex_unlock(&ht->mu);
    return size;
}
