#include "square-rpcs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define TODO()                                                                \
    do                                                                        \
    {                                                                         \
        extern int printf(char *, ...);                                       \
        printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__); \
    } while (0)

int main()
{
    int value = Square(3);
    assert(value == 9);
    printf("square: %d\n", value);
    return 0;
}
