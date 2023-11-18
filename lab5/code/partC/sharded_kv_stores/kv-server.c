#include "kv-server.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024
#define TABLE_SIZE 1000

const char *DELIM = "; ";
typedef struct MapNode
{
    char *key;
    list_t values;
    struct MapNode *next;
} MapNode;

MapNode *hashTable[TABLE_SIZE];

// hash
/* D. J. Bernstein hash function */
static unsigned int djb_hash(const char *cp)
{
    unsigned hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char)*cp++;
    return hash % TABLE_SIZE;
}
MapNode *get_new_node(char *key, char *value)
{
    MapNode *newNode = malloc(sizeof(MapNode));
    newNode->key = strdup(key);
    newNode->values = list_new();
    list_append(newNode->values, strdup(value));
    newNode->next = NULL;
    return newNode;
}

void put(char *key, char *value)
{
    unsigned int index = djb_hash(key);
    MapNode *newNode = get_new_node(key, value);
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
                list_destroy(current->values);
                current->values = newNode->values;
                free(newNode->key);
                free(newNode);
                return;
            }
            current = current->next;
        }
        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }
}

void append(char *key, char *value)
{
    unsigned int index = djb_hash(key);
    if (!hashTable[index])
    {
        hashTable[index] = get_new_node(key, value);
    }
    else
    {
        MapNode *current = hashTable[index];
        while (current)
        {
            if (strcmp(current->key, key) == 0)
            {
                list_append(current->values, strdup(value));
                return;
            }
            current = current->next;
        }
        MapNode *newNode = get_new_node(key, value);
        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }
}

char *get(char *key)
{
    unsigned int index = djb_hash(key);
    MapNode *current = hashTable[index];
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            static char buffer[BUFFER_SIZE];
            memset(buffer, 0, sizeof(buffer));
            list_t values = current->values;
            for (int i = 0; i < values->last; ++i)
            {
                char *value = values->containter[i];
                strcat(buffer, value);
                strcat(buffer, DELIM);
            }
            return strdup(buffer);
        }
        current = current->next;
    }
    return NULL;
}

void delete(char *key)
{
    unsigned int index = djb_hash(key);
    MapNode *current = hashTable[index];
    MapNode *prev = NULL;

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
            list_destroy(current->values);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

/*
int main()
{
    put("majingnan", "localhost:1234");
    put("xuhao", "localhost:4321");
    put("zhaojiacheng", "localhost:3456");
    append("majingnan", "localhost:2345");
    append("majingnan", "localhost:3456");
    append("majingnan", "localhost:4567");
    printf("Ma: %s\n", get("majingnan"));
    printf("Xu: %s\n", get("xuhao"));
    put("majingnan", "localhost:1234");
    printf("Ma: %s\n", get("majingnan"));
    printf("Zhao: %s\n", get("zhaojiacheng"));
    delete ("xuhao");
    printf("Xu: %s\n", get("xuhao"));
    return 0;
}
*/
