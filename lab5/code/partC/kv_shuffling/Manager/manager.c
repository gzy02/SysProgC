#include "../util_c/configuration.h"
#include "master-client.h"
char *get_address_for_key(char *key)
{
    char *config_str = GetConfig();
    Configuration *config = deserialize_config(config_str);
    char *ret = getServerAddress(config, key);
    free(config);
    free(config_str);
    return ret;
}
void connect_master()
{
    static char *master_ip = "localhost:1234";
    Config_IP(master_ip);
}
void connect_kv_db(char *key)
{
    connect_master();
    char *addr = get_address_for_key(key);
    Config_IP(addr);
    free(addr);
}

void put(char *key, char *value)
{
    connect_kv_db(key);
    Put(key, value);
}
char *get(char *key)
{
    connect_kv_db(key);
    return Get(key);
}
void delete(char *key)
{
    connect_kv_db(key);
    Delete(key);
}
