#include <stdlib.h>
#include <time.h>

int main()
{
    int i = 0;
    // this clock will alarm after 1 sec
    struct timespec ts = {.tv_sec = 1, .tv_nsec = 0};
    for (;;)
    {
        nanosleep(&ts, &ts);
    }
    exit(0);
}
