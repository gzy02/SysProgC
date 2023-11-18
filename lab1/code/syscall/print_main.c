#include "mini-lib.h"

int main()
{
    printf("str=%s,num=%d,hex=%x,char=%c,%%\n", "hello", (int)-2147483648, 0xfffffff1, 'a');
    return 0;
}
