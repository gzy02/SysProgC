#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "string-rpcs.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




void test() {
    char *str1 = "hello world";
    int value1 = CountString(str1);
    assert(value1 == 2);

    char *str2 = "The quick brown fox jumps over the lazy dog";
    int value2 = CountString(str2);
    assert(value2 == 9);

    char *str3 = "A";
    int value3 = CountString(str3);
    assert(value3 == 1);

    char *str4 = "This is a test string";
    int value4 = CountString(str4);
    assert(value4 == 5);

    char *str5 = "";
    int value5 = CountString(str5);
    assert(value5 == 0);
}

int main() {
    test();
    printf("All tests passed!\n");
    return 0;
}