#ifndef MASTER_SERVER_H
#define MASTER_SERVER_H

char *message(char *key);
char *add_server(char *key);
char *remove_server(char *key);
char *getconfig();
void config_init();
#endif