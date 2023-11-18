#ifndef MASTER_CLIENT_H
#define MASTER_CLIENT_H
#include "../util_c/configuration.h"
#include "master-rpcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void put(char *key, char *value);
char *get(char *key);
void delete(char *key);
void connect_master();
#endif