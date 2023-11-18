#ifndef USTCSSE_PACKET_ANALYZE_H
#define USTCSSE_PACKET_ANALYZE_H

#define TODO()                                                                \
    do                                                                        \
    {                                                                         \
        printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__); \
    } while (0)

#include <sys/types.h>

struct eth_count
{
    int arps;
    int ips;
    int ipv6s;
};

struct ip_count
{
    int tcps;
    int udps;
};

extern struct eth_count eth_count;
extern struct ip_count ip_count;

void packet_analyze(char *buf, ssize_t n);

#endif // USTCSSE_PACKET_ANALYZE_H
