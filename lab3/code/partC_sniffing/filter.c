#include "filter.h"
#include <arpa/inet.h>
#include <memory.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * name: filterByMacAddress
 * aug1: pointer of buffer.
 * aug2: pointer of mac address string. and input format is like : AC-23-74-EF-7D-9F
 * aug3: type, 0 -> source address, 1 -> destination address, filter by source/destination mac address.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByMacAddress(unsigned char *buffer, char *mac_addr, int type)
{
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    int mac_len = 17;

    int l_temp = 0, r_temp = 0, value = 0, j = 0;
    int resultArray[6];

    for (int i = 0; i < mac_len - 1; i = i + 3)
    {
        // left -> mac_addr[i]
        if ('0' <= mac_addr[i] && mac_addr[i] <= '9')
        {
            l_temp = mac_addr[i] - '0';
        }
        else if ('A' <= mac_addr[i] && mac_addr[i] <= 'F')
        {
            l_temp = mac_addr[i] - 'A' + 10;
        }
        // right -> mac_addr[i+1]
        if ('0' <= mac_addr[i + 1] && mac_addr[i + 1] <= '9')
        {
            r_temp = mac_addr[i + 1] - '0';
        }
        else if ('A' <= mac_addr[i + 1] && mac_addr[i + 1] <= 'F')
        {
            r_temp = mac_addr[i + 1] - 'A' + 10;
        }
        value = l_temp * 16 + r_temp;
        resultArray[j] = value; // store in result array
        j++;
    }
    if (type == 0)
    {
        // compare with source addr
        for (int i = 0; i < 6; i++)
        {
            if (resultArray[i] != (int)eth->h_source[i])
                return false;
        }
        return true;
    }
    else
    {
        // compare with destination addr
        for (int i = 0; i < 6; i++)
        {
            if (resultArray[i] != (int)eth->h_dest[i])
            {
                return false;
            }
        }
        return true;
    }
}

/*
 * name: filterByProtocol
 * aug1: pointer of buffer.
 * aug: type of packet you want to filter, 1->ARP; 2->ICMP; 3->TCP, 4->UDP.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByProtocol(unsigned char *buffer, int type)
{
    // Exercise 3: complete filter by MAC address, protocol type, and port number:
    // Add your code here:
    if (type < 1 || type > 4)
        return false;
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    int eth_protocol = (int)ntohs(eth->h_proto), type2eth_protocol[5] = {0, ETHERTYPE_ARP, ETHERTYPE_IP, ETHERTYPE_IP, ETHERTYPE_IP};
    // 判断ARP和其他三个（均使用IP协议）
    if (eth_protocol != type2eth_protocol[type])
        return false;
    else
    {
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        // int8不用转网络字节序
        int ip_protocol = ip->protocol, type2ip_protocol[5] = {0, 0, IPPROTO_ICMP, IPPROTO_TCP, IPPROTO_UDP};
        // 判断ICMP/TCP/UDP
        if (ip_protocol != type2ip_protocol[type])
            return false;
        else
            return true;
    }
}

/*
 * name: filterByIpAddress
 * aug1: pointer of buffer.
 * aug2: IP address.
 * aug3: type, 0->source IP, 1->destination IP.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByIpAddress(unsigned char *buffer, char *ip_addr, int type)
{
    struct sockaddr_in source, dest;
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    // if the type of packet is not IP.
    if ((int)ntohs(eth->h_proto) != ETHERTYPE_IP)
    {
        return false;
    }

    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;

    char *source_ip;
    char *dest_ip;
    char temp[20];
    source_ip = inet_ntoa(source.sin_addr);
    strcpy(temp, source_ip);
    dest_ip = inet_ntoa(dest.sin_addr);
    if (type == 0)
    { // filter by source address
        if (!strcmp(temp, ip_addr))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    { // filter by distination address
        if (!strcmp(dest_ip, ip_addr))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

/*
 * name: filterByPort
 * aug1: pointer of buffer.
 * aug2: TCP/UDP port value.
 * aug3: type, 0->source port, 1->destination port.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByPort(unsigned char *buffer, unsigned short port, int type)
{
    // Exercise 3: complete filter by MAC address, protocol type, and port number:
    // Add your code here:
    if (type != 0 || type != 1)
        return false;
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    // if the type of packet is not IP.
    if ((int)ntohs(eth->h_proto) != ETHERTYPE_IP)
    {
        return false;
    }
    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    if (ip->protocol == IPPROTO_TCP)
    {
        struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
        if (type == 0)
        {
            if (ntohs(tcp->source) == port)
                return true;
            else
                return false;
        }
        else
        {
            if (ntohs(tcp->dest) == port)
                return true;
            else
                return false;
        }
    }
    else if (ip->protocol == IPPROTO_UDP)
    {
        struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
        if (type == 0)
        {
            if (ntohs(udp->source) == port)
                return true;
            else
                return false;
        }
        else
        {
            if (ntohs(udp->dest) == port)
                return true;
            else
                return false;
        }
    }
    else
        return false;
}
