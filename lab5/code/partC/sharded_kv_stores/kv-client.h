#ifndef KV_CLIENT_H
#define KV_CLIENT_H

#define ADDR_LENGTH 100

typedef struct
{
    int l, r;
    char addr[ADDR_LENGTH];
} ShardInfo;

typedef struct
{
    ShardInfo *shards;
    int count;
} Configuration;
#endif