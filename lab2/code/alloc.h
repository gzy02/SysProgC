#ifndef ALLOC_H
#define ALLOC_H
#include <stdlib.h>
#define NEW(p) \
do{            \
p = malloc(sizeof(*p)); \
}while(0)

#endif
