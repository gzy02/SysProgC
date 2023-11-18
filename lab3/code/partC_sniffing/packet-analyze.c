#include "packet-analyze.h"
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>

struct eth_count eth_count;
struct ip_count ip_count;

void print_ether_addr(unsigned char *src, unsigned char *dst)
{
    if (*src == 0)
        return;
    printf("ether source-dest addr are:\n\t");
    for (int i = 0; i < ETH_ALEN; i++)
    {
        printf("%x:", src[i]);
    }
    printf("\n\t");
    for (int i = 0; i < ETH_ALEN; i++)
    {
        printf("%x:", dst[i]);
    }
    printf("\n");
}

int analyze_eth(char *buf)
{
    struct ether_header *ethhdr = (struct ether_header *)buf;

    unsigned char *src = ethhdr->ether_shost;
    unsigned char *dst = ethhdr->ether_dhost;
    int proto = ntohs(ethhdr->ether_type);
    print_ether_addr(src, dst);

    // printf("packet type: 0x%04x\n", proto);

    switch (proto)
    {
    case ETHERTYPE_ARP:
        eth_count.arps++;
        break;
    case ETHERTYPE_IP:
        eth_count.ips++;
        break;
    case ETHERTYPE_IPV6:
        eth_count.ipv6s++;
        break;
    default:
        fprintf(stderr, "ether, to do: %04x\n", proto);
        break;
    }
    return proto;
}

int analyze_ip(char *buf)
{
    struct iphdr *ip_addr = (struct iphdr *)buf;
    int proto = ip_addr->protocol;
    switch (proto)
    {
    case IPPROTO_TCP:
        ip_count.tcps++;
        break;
    case IPPROTO_UDP:
        ip_count.udps++;
        break;
    default:
        fprintf(stderr, "ip proto to do: %04x\n", proto);
        break;
    }
    return proto;
}

int analyze_tcp(char *addr)
{
    struct tcphdr *tcp_addr = (struct tcphdr *)addr;
    unsigned short src_port = ntohs(tcp_addr->th_sport);
    unsigned short dst_port = ntohs(tcp_addr->th_dport);
    printf("a tcp packet from: %d --> %d\n", src_port, dst_port);
    // intended for debugging purpose
    if (dst_port == 12345)
    {
        fprintf(stderr, "a connection to host: %d\n", 12345);
        unsigned char data_offset = (tcp_addr->th_off);
        char *payload = addr + sizeof(int) * data_offset;
        // randomly print the payload, should use a cleverer algorithm
        for (int i = 0; i < 10; i++)
            fprintf(stderr, "%c", payload[i]);
        fprintf(stderr, "\n");
    }
    return 0;
}

void packet_analyze(char *buf, ssize_t n)
{
    // first analyze the ether packer, get its inner protocol
    int inner_ether_proto = analyze_eth(buf);
    switch (inner_ether_proto)
    {
    case ETHERTYPE_IP:
    {
        char *ip_addr = buf + sizeof(struct ether_header);
        int inner_ip_proto = analyze_ip(ip_addr);
        switch (inner_ip_proto)
        {
        case IPPROTO_TCP:
        {
            char *tcp_addr = ip_addr + sizeof(struct iphdr);
            analyze_tcp(tcp_addr);
            break;
        }
        case IPPROTO_UDP:
            fprintf(stderr, "to do udp: %04x\n", inner_ip_proto);
            break;
        default:
            fprintf(stderr, "to do ip proto: %04x\n", inner_ip_proto);
            break;
        }
        break;
    }
    case ETHERTYPE_ARP:
        fprintf(stderr, "todo %d\n", inner_ether_proto);
        break;
    default:
    {
        fprintf(stderr, "todo %d\n", inner_ether_proto);
        break;
    }
    }
}
