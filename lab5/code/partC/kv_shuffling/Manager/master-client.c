#include "master-client.h"
#define BUFFER_SIZE 1024

void update_config(Configuration *past_config, Configuration *new_config, char *res)
{
    for (int i = 0; i < past_config->count; ++i)
    {
        for (int j = 0; j < new_config->count; ++j)
        {
            if (strcmp(past_config->shards[i].addr, new_config->shards[j].addr) == 0)
            {
                Config_IP(past_config->shards[i].addr);

                int pl = past_config->shards[i].l, pr = past_config->shards[i].r, nl = new_config->shards[j].l, nr = new_config->shards[j].r;
                // 度量两个区间的相对关系，将失效的区间的key-value删除
                // 在[pl,pr]而不在[nl,nr]的就是失效的区间，需要将这些区间的key-value删除，并加入res中

                // [pl,pr]和[nl,nr]没有交集
                if (pl > nr || pr < nl)
                {
                    strcat(res, Delete_kv_range(pl, pr));
                }
                // [pl,pr]包含[nl,nr]
                else if (pl <= nl && pr >= nr)
                {
                    strcat(res, Delete_kv_range(pl, nl - 1));
                    strcat(res, Delete_kv_range(nr + 1, pr));
                }
                // [pl,pr]和[nl,nr]有交集，但是不包含
                else if (pl <= nl && pr >= nl)
                {
                    strcat(res, Delete_kv_range(pl, nl - 1));
                }
                else if (pl <= nr && pr >= nr)
                {
                    strcat(res, Delete_kv_range(nr + 1, pr));
                }
            }
        }
    }

    // res的结构为 key \t value \n key \t value \n ...
    // 一一读取，并put到新的区间中
    char *save_ptr, *p = strtok_r(res, "\n", &save_ptr);
    while (p != NULL)
    {
        char *key = strtok(p, "\t");
        char *value = strtok(NULL, "\t");
        put(key, value);
        p = strtok_r(NULL, "\n", &save_ptr);
    }
}
int main()
{
    char command[BUFFER_SIZE], addr[BUFFER_SIZE], key[BUFFER_SIZE], value[BUFFER_SIZE];
    char *res, *config_str;

    printf("Enter command (ADD/REMOVE/PUT/GET/DELETE/SHOWALL) \n");
    while (1)
    {
        printf("# ");
        scanf("%s", command);

        if (strcmp(command, "ADD") == 0)
        {
            scanf("%s", addr);
            connect_master();
            config_str = GetConfig();
            Configuration *past_config = deserialize_config(config_str);
            free(config_str);

            res = AddServer(addr);
            if (strcmp(res, "EXISTED") == 0)
            {
                printf("\tServer already exist\n");
                free(res);
                continue;
            }
            free(res);

            config_str = GetConfig();
            printf("\tConfig Begin\n%s\tConfig End\n", config_str);
            Configuration *new_config = deserialize_config(config_str);
            free(config_str);

            // KV shuffling
            res = malloc(BUFFER_SIZE * TABLE_SIZE);
            *res = '\0';
            update_config(past_config, new_config, res);

            free(past_config);
            free(new_config);
            free(res);
        }
        else if (strcmp(command, "REMOVE") == 0)
        {
            scanf("%s", addr);
            connect_master();
            config_str = GetConfig();
            Configuration *past_config = deserialize_config(config_str);
            free(config_str);

            res = RemoveServer(addr);
            if (strcmp(res, "NOT EXIST") == 0)
            {
                printf("\tServer not exist\n");
                free(res);
                continue;
            }
            free(res);

            config_str = GetConfig();
            printf("\tConfig Begin\n%s\tConfig End\n", config_str);
            Configuration *new_config = deserialize_config(config_str);
            free(config_str);

            // KV shuffling
            res = malloc(BUFFER_SIZE * TABLE_SIZE);
            *res = '\0';
            for (int i = 0; i < past_config->count; ++i)
            {
                if (strcmp(past_config->shards[i].addr, addr) == 0)
                {
                    Config_IP(addr);
                    strcat(res, Delete_kv_range(past_config->shards[i].l, past_config->shards[i].r)); // addr的区间全部删除
                    continue;
                }
            }
            update_config(past_config, new_config, res);

            free(past_config);
            free(new_config);
            free(res);
        }
        else if (strcmp(command, "SHOWALL") == 0)
        {
            connect_master();
            config_str = GetConfig();
            Configuration *config = deserialize_config(config_str);
            free(config_str);
            for (int i = 0; i < config->count; ++i)
            {
                Config_IP(config->shards[i].addr);
                res = Get_all_kv();
                printf("\t%s Begin\n%s\t%s End\n", config->shards[i].addr, res, config->shards[i].addr);
                free(res);
            }
        }
        else if (strcmp(command, "GET") == 0)
        {
            scanf("%s", key);
            res = get(key);
            printf("\t%s: %s\n", key, res);
            free(res);
        }
        else if (strcmp(command, "PUT") == 0)
        {
            scanf("%s", key);
            scanf("%s", value);
            put(key, value);
            printf("\tPUT OK\n");
        }
        else if (strcmp(command, "DELETE") == 0)
        {
            scanf("%s", key);
            delete (key);
            printf("\tDELETE OK\n");
        }
        else if (strcmp(command, "EXIT") == 0)
        {
            break;
        }
        else
        {
            printf("\tInvalid command\n");
            continue;
        }
    }

    return 0;
}
