#include "configuration.h"
#include "master-rpcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    char *master_ip = "localhost:1234";
    Config_IP(master_ip);

    char *config_str, *res;
    config_str = GetConfig();
    printf("GetConfig:\n%s\n", config_str);
    deserialize_config(config_str);
    free(config_str);

    res = AddServer("localhost:2345");
    config_str = GetConfig();
    printf("Result:%s\nConfig:\n%s\n", res, config_str);
    deserialize_config(config_str);
    free(config_str);
    free(res);

    res = AddServer("localhost:2345");
    config_str = GetConfig();
    printf("Result:%s\nConfig:\n%s\n", res, config_str);
    deserialize_config(config_str);
    free(config_str);
    free(res);

    res = AddServer("localhost:5678");
    config_str = GetConfig();
    printf("Result:%s\nConfig:\n%s\n", res, config_str);
    deserialize_config(config_str);
    free(config_str);
    free(res);

    res = RemoveServer("localhost:2345");
    config_str = GetConfig();
    printf("Result:%s\nConfig:\n%s\n", res, config_str);
    deserialize_config(config_str);
    free(config_str);
    free(res);

    res = RemoveServer("localhost:2345");
    config_str = GetConfig();
    printf("Result:%s\nConfig:\n%s\n", res, config_str);
    deserialize_config(config_str);
    free(config_str);
    free(res);

    return 0;
}
