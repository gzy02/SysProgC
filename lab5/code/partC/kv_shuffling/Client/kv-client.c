#include "../util_c/configuration.h"
#include "kv-rpcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_address_for_key(char *key)
{
    static char *master_addr = ":1234";
    Config_IP(master_addr);
    char *config_str = GetConfig();
    Configuration *config = deserialize_config(config_str);
    char *ret = getServerAddress(config, key);
    free(config);
    free(config_str);
    return ret;
}
int main()
{
    {
        char key[1024] = "majingnan";
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

        Put(key, "Silly Boy");
        printf("%s: %s\n", key, Get(key));
    }
    {
        char key[1024] = "xuhao";
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

        Put(key, "ordinary");
        printf("%s: %s\n", key, Get(key));
    }

    {
        char key[1024] = "zhaojiacheng";
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

        printf("尝试在:2345处获取xuhao/majingnan的信息\n");
        printf("%s: %s\n", "xuhao", Get("xuhao"));
        printf("%s: %s\n", "majingnan", Get("majingnan"));

        Put(key, "talent");
        printf("%s: %s\n", key, Get(key));
        Delete(key);
        printf("%s: %s\n", key, Get(key));
    }

    return 0;
}
