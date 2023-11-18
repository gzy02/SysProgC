#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#define TABLE_SIZE 1000

typedef struct MapNode
{
    char *key;
    char *value;
    struct MapNode *next;
} MapNode;
unsigned int djb_hash(const char *cp);

#endif