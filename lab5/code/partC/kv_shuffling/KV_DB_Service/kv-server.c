#include "../util_c/hash-table.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MapNode *hashTable[TABLE_SIZE];
pthread_rwlock_t rwlock[TABLE_SIZE];

#define BUFFER_SIZE 1024

// 将hash值在[l,r]范围的键值对序列化返回并清空
char *delete_kv_range(int l, int r)
{
    char *res = malloc(sizeof(char) * BUFFER_SIZE * TABLE_SIZE);
    *res = '\0';
    char *tmp = malloc(sizeof(char) * BUFFER_SIZE);
    for (int i = l; i <= r; i++)
    {
        pthread_rwlock_wrlock(&rwlock[i]);
        MapNode *current = hashTable[i];
        while (current)
        {
            sprintf(tmp, "%s\t%s\n", current->key, current->value);
            strcat(res, tmp);
            MapNode *prev = current;
            current = current->next;
            free(prev);
        }
        hashTable[i] = NULL;
        pthread_rwlock_unlock(&rwlock[i]);
    }
    free(tmp);
    return res;
}

// 将所有键值对序列化返回
char *get_all_kv()
{
    char *res = malloc(sizeof(char) * BUFFER_SIZE * TABLE_SIZE);
    *res = '\0';
    char *tmp = malloc(sizeof(char) * BUFFER_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        pthread_rwlock_rdlock(&rwlock[i]);
        MapNode *current = hashTable[i];
        while (current)
        {
            sprintf(tmp, "%s\t%s\n", current->key, current->value);
            strcat(res, tmp);
            current = current->next;
        }
        pthread_rwlock_unlock(&rwlock[i]);
    }
    free(tmp);
    return res;
}

void put(char *key, char *value)
{
    unsigned int index = djb_hash(key);
    MapNode *newNode = malloc(sizeof(MapNode));
    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->next = NULL;

    pthread_rwlock_wrlock(&rwlock[index]);

    if (!hashTable[index])
    {
        hashTable[index] = newNode;
    }
    else
    {
        MapNode *current = hashTable[index];
        while (current)
        {
            if (strcmp(current->key, key) == 0)
            {
                free(current->value);
                current->value = strdup(value);
                free(newNode->key);
                free(newNode->value);
                free(newNode);
                pthread_rwlock_unlock(&rwlock[index]);
                return;
            }
            current = current->next;
        }
        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }

    pthread_rwlock_unlock(&rwlock[index]);
}

char *get(char *key)
{
    unsigned int index = djb_hash(key);
    MapNode *current;

    pthread_rwlock_rdlock(&rwlock[index]);

    current = hashTable[index];
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            pthread_rwlock_unlock(&rwlock[index]);
            return current->value;
        }
        current = current->next;
    }

    pthread_rwlock_unlock(&rwlock[index]);
    return NULL;
}

void delete(char *key)
{
    unsigned int index = djb_hash(key);
    MapNode *current = hashTable[index];
    MapNode *prev = NULL;

    pthread_rwlock_wrlock(&rwlock[index]);

    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                hashTable[index] = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            pthread_rwlock_unlock(&rwlock[index]);
            return;
        }
        prev = current;
        current = current->next;
    }

    pthread_rwlock_unlock(&rwlock[index]);
}
