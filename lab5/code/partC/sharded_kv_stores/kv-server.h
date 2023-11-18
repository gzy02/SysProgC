// file-struct.h
#ifndef KV_SERVER_H
#define KV_SERVER_H

void put(char *key, char *value);
char *get(char *key);
void delete(char *key);
void append(char *key, char *value);

#endif