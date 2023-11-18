#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "hash-table.h"
#include <pthread.h>
#define ADDR_LENGTH 100
#define CONFIG_RANGE TABLE_SIZE

typedef struct
{
    int l, r;
    char addr[ADDR_LENGTH];
} ShardInfo;

typedef struct
{
    ShardInfo *shards;
    int count;
    pthread_rwlock_t rwlock;
} Configuration;

char *serialize_config(Configuration *config);
Configuration *deserialize_config(char *str);
char *getServerAddress(Configuration *config, char *key);

#endif