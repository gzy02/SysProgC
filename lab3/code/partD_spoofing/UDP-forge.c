#include <arpa/inet.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <malloc.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct ifreq ifreq_c, ifreq_i, ifreq_ip; /// for each ioctl keep diffrent ifreq structure otherwise error may come in sending(sendto )
int sock_raw;
unsigned char *sendbuff;

#define DESTMAC0 0x00
#define DESTMAC1 0x0c
#define DESTMAC2 0x29
#define DESTMAC3 0xe1
#define DESTMAC4 0x8c
#define DESTMAC5 0xaa

#define SRC_PORT 8080
#define DST_PORT 7777

#define destination_ip "172.23.112.1"

int total_len = 0, send_len;

void get_eth_index()
{
    memset(&ifreq_i, 0, sizeof(ifreq_i));
    // Replace eth0 with your network interface
    strncpy(ifreq_i.ifr_name, "eth0", IFNAMSIZ - 1);
    if ((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0)
        printf("error in index ioctl reading");
    printf("index=%d\n", ifreq_i.ifr_ifindex);
}

void get_mac()
{
    memset(&ifreq_c, 0, sizeof(ifreq_c));
    // Replace eth0 with your network interface
    strncpy(ifreq_c.ifr_name, "eth0", IFNAMSIZ - 1);
    if ((ioctl(sock_raw, SIOCGIFHWADDR, &ifreq_c)) < 0)
        printf("error in SIOCGIFHWADDR ioctl reading");
    printf("Mac= %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]));
    printf("ethernet packaging start ... \n");
    struct ethhdr *eth = (struct ethhdr *)(sendbuff);
    eth->h_source[0] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]);
    eth->h_source[1] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]);
    eth->h_source[2] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]);
    eth->h_source[3] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]);
    eth->h_source[4] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]);
    eth->h_source[5] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]);
    eth->h_dest[0] = DESTMAC0;
    eth->h_dest[1] = DESTMAC1;
    eth->h_dest[2] = DESTMAC2;
    eth->h_dest[3] = DESTMAC3;
    eth->h_dest[4] = DESTMAC4;
    eth->h_dest[5] = DESTMAC5;
    eth->h_proto = htons(ETH_P_IP); // 0x0800
    printf("ethernet packaging done.\n");
    total_len += sizeof(struct ethhdr);
}

void get_data()
{
    sendbuff[total_len++] = 0x68;
    sendbuff[total_len++] = 0x65;
    sendbuff[total_len++] = 0x6C;
    sendbuff[total_len++] = 0x6C;
    sendbuff[total_len++] = 0x6F;
}

void get_udp()
{
    struct udphdr *uh = (struct udphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));
    // Exercise 5: Write UDP_forge.c in your project to achieve the forgery of UDP protocol packets:
    // Add your code here:
    uh->source = htons(SRC_PORT);
    uh->dest = htons(DST_PORT);
    uh->len = htons(sizeof(struct udphdr));
    // 字段check是校验和，但在UDP中不是强制的（仅限IPv4），因此可以简单置为零
    uh->check = 0;
    total_len += sizeof(struct udphdr);
    get_data();
}

unsigned short checksum(unsigned char *buf, int size)
{
    unsigned int checkSum = 0;
    for (int i = 0; i < size; i += 2)
    {
        unsigned short first = (unsigned short)buf[i] << 8;
        unsigned short second = (unsigned short)buf[i + 1] & 0x00ff;
        checkSum += first + second;
    }
    while (1)
    {
        unsigned short c = (checkSum >> 16);
        if (c > 0)
        {
            checkSum = (checkSum << 16) >> 16;
            checkSum += c;
        }
        else
        {
            break;
        }
    }
    return ~checkSum;
}

void get_ip()
{
    memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    // Replace eth0 with your network interface
    strncpy(ifreq_ip.ifr_name, "eth0", IFNAMSIZ - 1);
    if (ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0)
    {
        printf("error in SIOCGIFADDR \n");
    }
    printf("%s\n", inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
    struct iphdr *iph = (struct iphdr *)(sendbuff + sizeof(struct ethhdr));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->id = htons(10201);
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
    iph->daddr = inet_addr("192.168.117.138"); // put destination IP address
    printf("destIP:%.2X\n", iph->daddr);
    total_len += sizeof(struct iphdr);
    get_udp();
    iph->tot_len = htons(total_len - sizeof(struct ethhdr));
    iph->check = htons(checksum((unsigned char *)(sendbuff + sizeof(struct ethhdr)), sizeof(struct iphdr)));
}

int main()
{
    sock_raw = socket(AF_PACKET, SOCK_RAW, ETH_P_IP);
    if (sock_raw == -1)
        printf("error in socket");
    sendbuff = (unsigned char *)malloc(64); // increase in case of large data.
    memset(sendbuff, 0, 64);
    get_eth_index(); // interface number
    get_mac();
    get_ip();
    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex;
    sadr_ll.sll_halen = ETH_ALEN;
    sadr_ll.sll_addr[0] = DESTMAC0;
    sadr_ll.sll_addr[1] = DESTMAC1;
    sadr_ll.sll_addr[2] = DESTMAC2;
    sadr_ll.sll_addr[3] = DESTMAC3;
    sadr_ll.sll_addr[4] = DESTMAC4;
    sadr_ll.sll_addr[5] = DESTMAC5;
    printf("sending...\n");
    while (1)
    {
        send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll));
        if (send_len < 0)
        {
            printf("error in sending....sendlen=%d....errno=%d\n", send_len, errno);
            return -1;
        }
        printf("Sent %d bytes\n", send_len);
        fflush(stdout);
        sleep(1);
    }
}
