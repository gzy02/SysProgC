#include "packet-process.h"
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <string.h>

struct sockaddr_in source, dest;
int total = 0, tcp = 0, udp = 0, icmp = 0, igmp = 0, other = 0, arp_packet = 0;
int ipheader_len;

// print format of etherNet-header
void mac_header(unsigned char *buffer)
{
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    printf("\nEthernet Header\n");
    printf("\t|-Source Address      : %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n",
           eth->h_source[0], eth->h_source[1], eth->h_source[2],
           eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    printf("\t|-Destination Address : %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n",
           eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
           eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    printf("\t|-Protocol            : %d\n", ntohs(eth->h_proto));
}

// print format of ip-header
void ip_header(unsigned char *buffer)
{
    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ipheader_len = ip->ihl * 4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;

    printf("\nIP Header\n");
    printf("\t|-Version                 : %d\n", (unsigned int)ip->version);
    printf("\t|-Internet Header Length  : %d DWORDS or %d Bytes\n",
           (unsigned int)ip->ihl, ((unsigned int)(ip->ihl)) * 4);
    printf("\t|-Type of Service         : %d\n", (unsigned int)ip->tos);
    printf("\t|-Total length            : %d Bytes\n", ntohs(ip->tot_len));
    printf("\t|-Identification          : %d\n", ntohs(ip->id));
    printf("\t|-TTL                     : %d\n", (unsigned int)ip->ttl);
    printf("\t|-Protocol                : %d\n", (unsigned int)ip->protocol);
    printf("\t|-Header Checksum         : %d\n", ntohs(ip->check));
    printf("\t|-Source IP               : %s\n", inet_ntoa(source.sin_addr));
    printf("\t|-Destination IP          : %s\n", inet_ntoa(dest.sin_addr));
}

// print format of arp-header
void arp_header(unsigned char *buffer)
{
    struct arphdr *arp = (struct arphdr *)(buffer + sizeof(struct ethhdr));
    printf("\nARP Header\n");
    printf("\t|-Format of Hardware Address      : %d\n", (unsigned int)arp->ar_hrd);
    printf("\t|-Format of Protocol Address      : %.4x\n", (unsigned int)arp->ar_pro);
    printf("\t|-Length of Hardware Address      : %c\n", arp->ar_hln);
    printf("\t|-Length of Protocol Address      : %c\n", arp->ar_pln);
    printf("\t|-ARP opcode:                     : %d\n", (unsigned int)arp->ar_op);
}

// print format of tcp-header
void tcp_header(unsigned char *buffer)
{
    struct tcphdr *tcp = (struct tcphdr *)(buffer + ipheader_len + sizeof(struct ethhdr));
    printf("\nTCP Header\n");
    printf("\t|-Source Port             : %u\n", ntohs(tcp->source));
    printf("\t|-Destination Port        : %u\n", ntohs(tcp->dest));
    printf("\t|-Sequence Number         : %u\n", ntohl(tcp->seq));
    printf("\t|-Acknowledge Number      : %u\n", ntohl(tcp->ack_seq));
    printf("\t|-Header Length           : %d DWORDS or %d BYTES\n",
           (unsigned int)tcp->doff, (unsigned int)tcp->doff * 4);
    printf("\t|----------Flags-----------\n");
    printf("\t\t|-Urgent Flag          : %d\n", (unsigned int)tcp->urg);
    printf("\t\t|-Acknowledgement Flag : %d\n", (unsigned int)tcp->ack);
    printf("\t\t|-Push Flag            : %d\n", (unsigned int)tcp->psh);
    printf("\t\t|-Reset Flag           : %d\n", (unsigned int)tcp->rst);
    printf("\t\t|-Synchronise Flag     : %d\n", (unsigned int)tcp->syn);
    printf("\t\t|-Finish Flag          : %d\n", (unsigned int)tcp->fin);
    printf("\t|-Window size             : %d\n", ntohs(tcp->window));
    printf("\t|-Checksum                : %d\n", ntohs(tcp->check));
    printf("\t|-Urgent Pointer          : %d\n", tcp->urg_ptr);
}

// print format of udp-header
void udp_header(unsigned char *buffer)
{
    struct udphdr *udp = (struct udphdr *)(buffer + ipheader_len + sizeof(struct ethhdr));
    printf("\nUDP Header\n");
    printf("\t|-Source Port    	      : %d\n", ntohs(udp->source));
    printf("\t|-Destination Port	      : %d\n", ntohs(udp->dest));
    printf("\t|-UDP Length      	      : %d\n", ntohs(udp->len));
    printf("\t|-UDP Checksum   	      : %d\n", ntohs(udp->check));
}

// print format of icmp-header
void icmp_header(unsigned char *buffer)
{
    struct icmp *icmp_packet = (struct icmp *)(buffer + ipheader_len + sizeof(struct ethhdr));
    printf("\nICMP Header\n");
    printf("\t|-TYPE                    : %d\n", icmp_packet->icmp_type);
    printf("\t|-CODE                    : %d\n", icmp_packet->icmp_code);
    printf("\t|-CHECKSUM                : %d\n", ntohs(icmp_packet->icmp_cksum));
}

// print format of payload
void payload(unsigned char *buffer, int buffer_len)
{
    printf("\nData\n");
    for (int i = 0; i < buffer_len; i++)
    {
        if (i != 0 && i % 16 == 0)
            printf("\n");
        printf(" %.2X ", buffer[i]);
    }
    printf("\n");
}

// print ICMP packet
void ICMP_printer(unsigned char *buffer, int buffer_len)
{
    printf("\n---------------------------ICMP Packet---------------------------");
    // print header
    mac_header(buffer);
    ip_header(buffer);
    icmp_header(buffer);
    // print packet payload
    unsigned char *data = (buffer + ipheader_len + sizeof(struct ethhdr) + sizeof(struct icmphdr));
    int data_len = buffer_len - (ipheader_len + sizeof(struct ethhdr) + sizeof(struct icmphdr));
    payload(data, data_len);
    printf("--------------------------------------------------------------\n\n\n");
}

// print ARP packet
void ARP_printer(unsigned char *buffer)
{
    printf("\n---------------------------ARP Packet---------------------------");
    // Exercise 2: complete the analysis function of the remaining three packets:
    // Add your code here:
    // print header
    mac_header(buffer);
    arp_header(buffer);
    printf("--------------------------------------------------------------\n\n\n");
}

// print TCP packet
void TCP_printer(unsigned char *buffer, int buffer_len)
{
    printf("\n---------------------------TCP Packet---------------------------");
    // Exercise 2: complete the analysis function of the remaining three packets:
    // Add your code here:
    // print header
    mac_header(buffer);
    ip_header(buffer);
    tcp_header(buffer);
    // print packet payload
    unsigned char *data = (buffer + ipheader_len + sizeof(struct ethhdr) + sizeof(struct tcphdr));
    int data_len = buffer_len - (ipheader_len + sizeof(struct ethhdr) + sizeof(struct tcphdr));
    payload(data, data_len);
    printf("--------------------------------------------------------------\n\n\n");
}

// print UDP packet
void UDP_printer(unsigned char *buffer, int buffer_len)
{
    printf("\n---------------------------UDP Packet---------------------------");
    // Exercise 2: complete the analysis function of the remaining three packets:
    // Add your code here:
    // print header
    mac_header(buffer);
    ip_header(buffer);
    udp_header(buffer);
    // print packet payload
    unsigned char *data = (buffer + ipheader_len + sizeof(struct ethhdr) + sizeof(struct udphdr));
    int data_len = buffer_len - (ipheader_len + sizeof(struct ethhdr) + sizeof(struct udphdr));
    payload(data, data_len);
    printf("--------------------------------------------------------------\n\n\n");
}

// packet process func
void packet_process(unsigned char *buffer, int buffer_len)
{
    ++total;
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    switch ((int)ntohs(eth->h_proto))
    {
    case ETHERTYPE_IP:
    {
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        switch (ip->protocol)
        {
        case IPPROTO_ICMP:
            ++icmp;
            ICMP_printer(buffer, buffer_len);
            break;
        case IPPROTO_IGMP:
            ++igmp;
            break;
        case IPPROTO_TCP:
            // Exercise 2: complete the analysis function of the remaining three packets:
            // Add your code here:
            ++tcp;
            TCP_printer(buffer, buffer_len);
            break;
        case IPPROTO_UDP:
            // Exercise 2: complete the analysis function of the remaining three packets:
            // Add your code here:
            ++udp;
            UDP_printer(buffer, buffer_len);
            break;
        default:
            ++other;
        }
        break;
    }
    case ETHERTYPE_ARP:
        // Exercise 2: complete the analysis function of the remaining three packets:
        // Add your code here:
        ++arp_packet;
        ARP_printer(buffer);
        break;

    default:
        ++other;
    }
    printf("Result: [TCP : %d], [UDP : %d], [ARP : %d], [ICMP : %d], [IGMP : %d], [OTHER : %d], [TOTAL : %d]\n",
           tcp, udp, arp_packet, icmp, igmp, other, total);
}