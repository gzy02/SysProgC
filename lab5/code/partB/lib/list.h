#ifndef LIST_H
#define LIST_H

typedef struct List_t *list_t;
struct List_t
{
    void **containter;
    int size; // maximum size
    int last; // always point to position to hold the next item
};

list_t list_new();
void list_append(list_t list, void *data);
void *list_nth(list_t list, int n);
void list_print(list_t, void (*f)(void *));
void list_destroy(list_t list);
#endif
