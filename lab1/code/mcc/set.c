#include <string.h>
#include "set.h"

#define MAX_VAR 256

struct set_t{
    char *set_data[MAX_VAR];
    int set_count;
    int set_cursor;
};

struct set_t set;

int set_exists(char *var) {
    for (int i = 0; i < set.set_count; i++) {
        if (strcmp(var, set.set_data[i]) == 0)
            return 1;
    }
    return 0;
}

void set_add(char *var) {
    if (!set_exists(var)) {
        set.set_data[set.set_count++] = var;
    }
}

char *set_next(){
    if(set.set_cursor >= set.set_count)
        return 0;
    return set.set_data[set.set_cursor++];
}

void set_reset(){
    set.set_count = 0;
    set.set_cursor = 0;
}
