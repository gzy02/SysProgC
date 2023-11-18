#ifndef MAP_REDUCE_H
#define MAP_REDUCE_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include "list.h"

// take as input a single string, and return a list of
// (key, value), both key and value are strings.
// the return type: list_t<kv_t>
// list_t mr_map(char *s);
// take as input a key, and list of all associated values (a list of strings)
// return an aggregation (a single string)
// the input type: list_t<char *>
// char *mr_reduce(char *key, list_t values);

// for debugging
void list_print(list_t, void(*f)(void *));

#endif


