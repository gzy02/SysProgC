#include "mini-lib.h"

// see:
// https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

// syscall number for x86-64, in rax
#define SYS_WRITE 1
#define SYS_SLEEP 35
#define SYS_ALARM 37
#define SYS_EXIT 60

// arguments: rdi, rsi, rdx, r10, r8, r9

#define CALL2(n) "movq $" #n ", %%rax\n"
#define CALL(n) CALL2(n)

int strlen(const char *s)
{
    int n = 0;
    while (*s++)
        n++;
    return n;
}

int puts(const char *s)
{
    long n = strlen(s);
    long r;
    asm(CALL(SYS_WRITE) "movq $1, %%rdi\n"
                        "movq %1, %%rsi\n"
                        "movq %2, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
        : "=r"(r)
        : "r"(s), "r"(n)
        : "%rax", "%rdi", "%rsi", "%rdx");
    return (int)r;
}

void exit(int status)
{
    // Your code here:
    // TODO();
    long r = status;
    asm(CALL(SYS_EXIT) "movq %0, %%rdi\n"
                       "syscall\n"
        :
        : "r"(r)
        : "%rax", "%rdi");
}

int alarm(unsigned int seconds)
{
    // Your code here:
    // TODO();
    long r;
    long sec = seconds;
    asm(CALL(SYS_ALARM) "movq %1, %%rdi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
        : "=r"(r)
        : "r"(sec)
        : "%rax", "%rdi");
    return (int)r;
}

int sleep(struct timespec *ts, struct timespec *rem)
{
    // Your code here:
    // TODO();
    long r;
    asm(CALL(SYS_SLEEP) "movq %1, %%rdi\n"
                        "movq %2, %%rsi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
        : "=r"(r)
        : "r"(ts), "r"(rem)
        : "%rax", "%rdi", "%rsi");
    return (int)r;
}
