#include "master-server.h"
#include "../util_c/configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Configuration *config;

void config_init()
{
    config = (Configuration *)malloc(sizeof(Configuration));
    config->count = 0;
    config->shards = NULL;
    pthread_rwlock_init(&config->rwlock, NULL);
}

int find_server(char *server)
{
    for (int i = 0; i < config->count; ++i)
    {
        if (strcmp(config->shards[i].addr, server) == 0)
        {
            return i;
        }
    }
    return -1;
}

void update_index()
{
    int span = CONFIG_RANGE / config->count, l = 0, r = span - 1;
    for (int i = 0; i < config->count; ++i)
    {
        config->shards[i].l = l;
        config->shards[i].r = r;
        l = r + 1;
        r += span;
    }
    config->shards[config->count - 1].r = CONFIG_RANGE - 1;
}

char *add_server(char *server)
{
    {
        pthread_rwlock_rdlock(&config->rwlock);
        if (find_server(server) != -1)
        {
            pthread_rwlock_unlock(&config->rwlock);
            return "EXISTED";
        }
        pthread_rwlock_unlock(&config->rwlock);
    }

    pthread_rwlock_wrlock(&config->rwlock);
    config->shards = realloc(config->shards, (config->count + 1) * sizeof(ShardInfo));
    strcpy(config->shards[config->count].addr, server);
    ++config->count;
    update_index();
    pthread_rwlock_unlock(&config->rwlock);

    return "ADD";
}

char *remove_server(char *server)
{
    pthread_rwlock_wrlock(&config->rwlock);
    int id = find_server(server);
    if (id == -1)
        return "NOT EXIST";
    --config->count;
    for (int i = id; i < config->count; ++i)
    {
        strcpy(config->shards[i].addr, config->shards[i + 1].addr);
    }
    update_index();
    pthread_rwlock_unlock(&config->rwlock);
    return "REMOVE";
}

char *getconfig()
{
    return serialize_config(config);
}
