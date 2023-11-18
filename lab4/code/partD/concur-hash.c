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
    ht->mu = calloc(num_buckets, sizeof(*ht->mu));
    ht->buckets = calloc(num_buckets, sizeof(*ht->buckets));
    ht->num_buckets = num_buckets;
}

void hash_insert(hash_t *ht, char *key, char *value)
{
    int bucket_id = djb_hash(key) % ht->num_buckets;
    node_t *new_node = malloc(sizeof(node_t));
    new_node->key = strdup(key);
    new_node->value = strdup(value);

    pthread_rwlock_wrlock(&ht->mu[bucket_id]);
    new_node->next = ht->buckets[bucket_id];
    ht->buckets[bucket_id] = new_node;
    pthread_rwlock_unlock(&ht->mu[bucket_id]);
}

char *hash_lookup(hash_t *ht, char *key)
{
    int bucket_id = djb_hash(key) % ht->num_buckets;
    pthread_rwlock_rdlock(&ht->mu[bucket_id]);
    node_t *node = ht->buckets[bucket_id];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            pthread_rwlock_unlock(&ht->mu[bucket_id]);
            return node->value;
        }
        node = node->next;
    }
    pthread_rwlock_unlock(&ht->mu[bucket_id]);
    return NULL;
}

// return 0 for success; 1 for non-exiting "key"
int hash_delete(hash_t *ht, char *key)
{
    int bucket_id = djb_hash(key) % ht->num_buckets;
    node_t *prev = NULL;
    pthread_rwlock_wrlock(&ht->mu[bucket_id]);
    node_t *cur = ht->buckets[bucket_id];
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
            pthread_rwlock_unlock(&ht->mu[bucket_id]);
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    pthread_rwlock_unlock(&ht->mu[bucket_id]);
    return 1;
}

int hash_size(hash_t *ht)
{
    int size = 0;
    for (int i = 0; i < ht->num_buckets; i++)
    {
        pthread_rwlock_rdlock(&ht->mu[i]);
        node_t *node = ht->buckets[i];
        while (node != NULL)
        {
            size++;
            node = node->next;
        }
        pthread_rwlock_unlock(&ht->mu[i]);
    }
    return size;
}
