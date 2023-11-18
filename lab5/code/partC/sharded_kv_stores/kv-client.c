#include "kv-client.h"
#include "kv-rpcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// hash
/* D. J. Bernstein hash function */
static unsigned int djb_hash(const char *cp)
{
    unsigned hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char)*cp++;
    return hash;
}

char *getServerAddress(Configuration *config, char *key)
{
    unsigned int hashValue = djb_hash(key) % 1000;
    for (int i = 0; i < config->count; ++i)
    {
        if (hashValue >= config->shards[i].l && hashValue <= config->shards[i].r)
        {
            return config->shards[i].addr;
        }
    }
    return NULL;
}

void initialize(Configuration *config)
{
    config->count = 3;
    config->shards = malloc(config->count * sizeof(ShardInfo));

    strcpy(config->shards[0].addr, "localhost:1234");
    config->shards[0].l = 0;
    config->shards[0].r = 332;

    strcpy(config->shards[1].addr, "localhost:1234");
    config->shards[1].l = 333;
    config->shards[1].r = 666;

    strcpy(config->shards[2].addr, "localhost:1234");
    config->shards[2].l = 667;
    config->shards[2].r = 999;
}

char *get_address_for_key(char *key)
{
    static Configuration config;
    static int initialized = 0;
    if (!initialized)
    {
        initialize(&config);
        initialized = 1;
    }
    return getServerAddress(&config, key);
}

int main()
{
    char key[] = "majingnan";
    char *address = get_address_for_key(key);
    if (address)
    {
        printf("键为 %s 的数据位于服务器: %s\n", key, address);
    }
    else
    {
        printf("没有找到键为 %s 的服务器地址\n", key);
    }

    Config_IP(address);

    Put("majingnan", "localhost:1234");
    Put("xuhao", "localhost:4321");
    Put("zhaojiacheng", "localhost:3456");
    Append("majingnan", "localhost:2345");
    Append("majingnan", "localhost:3456");
    Append("majingnan", "localhost:4567");
    printf("Ma: %s\n", Get("majingnan"));
    printf("Xu: %s\n", Get("xuhao"));
    Put("majingnan", "localhost:1234");
    printf("Ma: %s\n", Get("majingnan"));
    printf("Zhao: %s\n", Get("zhaojiacheng"));
    Delete("xuhao");
    printf("Xu: %s\n", Get("xuhao"));

    return 0;
}
