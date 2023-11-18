
#define TODO()                                                                \
    do                                                                        \
    {                                                                         \
        extern int printf(char *, ...);                                       \
        printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__); \
    } while (0)

#include <pthread.h>
pthread_mutex_t mu;

int count = 0;

int increment()
{
    pthread_mutex_lock(&mu);
    count++;
    pthread_mutex_unlock(&mu);
    return count;
}

int decrement()
{
    pthread_mutex_lock(&mu);
    count--;
    pthread_mutex_unlock(&mu);
    return count;
}

int get_count()
{
    return count;
}