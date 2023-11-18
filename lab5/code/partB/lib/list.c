#include "list.h"
#include <assert.h>
#include <stdlib.h>

#define TODO()                                                                \
    do                                                                        \
    {                                                                         \
        extern int printf(char *, ...);                                       \
        printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__); \
    } while (0)

#define INIT_SIZE 8
#define SCALE_FACTOR 2

list_t list_new()
{
    list_t l = calloc(1, sizeof(*l));
    l->containter = calloc(INIT_SIZE, sizeof(*(l->containter)));
    l->size = INIT_SIZE;
    l->last = 0;
    return l;
}

void list_append(list_t list, void *data)
{
    if (list->last >= list->size)
    {
        list->containter = realloc(list->containter,
                                   (SCALE_FACTOR * list->size) * sizeof(*(list->containter)));
        assert(list->containter);
        list->size *= 2;
    }
    list->containter[list->last++] = data;
    return;
}

void *list_nth(list_t list, int n)
{
    return list->containter[n];
}

void list_print(list_t list, void (*f)(void *))
{
    for (int i = 0; i < list->last; i++)
        f(list->containter[i]);
}

void list_destroy(list_t list)
{
    free(list->containter);
    free(list);
}
