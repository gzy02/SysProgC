#include "configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// config序列化生成char*
char *serialize_config(Configuration *config)
{
    pthread_rwlock_rdlock(&config->rwlock);
    char *result = malloc(config->count * (sizeof(ShardInfo)));
    result[0] = '\0';
    for (int i = 0; i < config->count; i++)
    {
        static char buffer[sizeof(ShardInfo)];
        sprintf(buffer, "%d %d %s\n", config->shards[i].l, config->shards[i].r, config->shards[i].addr);
        strcat(result, buffer);
    }
    pthread_rwlock_unlock(&config->rwlock);
    return result;
}

// char* 反序列化生成config
Configuration *deserialize_config(char *str)
{
    Configuration *config = malloc(sizeof(Configuration));
    config->shards = malloc(sizeof(ShardInfo));
    config->count = 0;
    char *saveptr, *line = strtok_r(str, "\n", &saveptr);
    while (line != NULL)
    {
        config->shards = realloc(config->shards, (config->count + 1) * sizeof(ShardInfo));
        sscanf(line, "%d %d %s", &config->shards[config->count].l, &config->shards[config->count].r, config->shards[config->count].addr);
        config->count++;
        line = strtok_r(NULL, "\n", &saveptr);
    }
    pthread_rwlock_init(&config->rwlock, NULL);
    return config;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

char *getServerAddress(Configuration *config, char *key)
{
    unsigned int hashValue = djb_hash(key) % CONFIG_RANGE;
    int index = hashValue / (CONFIG_RANGE / config->count);
    return strdup(config->shards[min(index, config->count - 1)].addr);
}