#include <arpa/inet.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <malloc.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
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

int get_data()
{
    sendbuff[total_len++] = 0x68;
    sendbuff[total_len++] = 0x65;
    sendbuff[total_len++] = 0x6C;
    sendbuff[total_len++] = 0x6C;
    sendbuff[total_len++] = 0x6F;
    return 5;
}

struct dummy_tcphdr
{
    in_addr_t saddr;
    in_addr_t daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t tcplen;
};
// 12字节的TCP伪首部
struct dummy_tcphdr *get_dummy_tcp(struct iphdr *iph)
{
    struct dummy_tcphdr *dummyhdr = (struct dummy_tcphdr *)malloc(sizeof(struct dummy_tcphdr));
    dummyhdr->saddr = iph->saddr;
    dummyhdr->daddr = iph->daddr;
    dummyhdr->zero = 0;
    dummyhdr->protocol = iph->protocol;
}
unsigned short tcp_checksum(unsigned short *ptr, int nbytes, unsigned short *dummyhdr)
{
    long sum;
    int dummyhdr_size = sizeof(struct dummy_tcphdr);
    while (dummyhdr_size != 0)
    {
        sum += *dummyhdr++;
        dummyhdr_size -= 2;
    }

    unsigned short oddbyte;
    unsigned short answer;
    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
    {
        oddbyte = 0;
        *((uint8_t *)&oddbyte) = *(uint8_t *)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}
void get_tcp(struct dummy_tcphdr *dummyhdr)
{
    struct tcphdr *th = (struct tcphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));
    th->source = htons(SRC_PORT);
    th->dest = htons(DST_PORT);
    th->seq = htonl(114514);
    th->ack_seq = 0;
    // 数据偏移
    th->doff = sizeof(struct tcphdr) / 4;
    th->fin = 0;
    th->syn = 1;
    th->rst = 0;
    th->psh = 0;
    th->ack = 0;
    th->urg = 0;
    th->window = htons(42);
    th->check = 0;
    th->urg_ptr = 0;
    th->res1 = th->res2 = 0;
    total_len += sizeof(struct tcphdr);
    int data_len = get_data();
    dummyhdr->tcplen = htons(sizeof(struct tcphdr) + data_len);
    th->check = htons(tcp_checksum((unsigned short *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr)), sizeof(struct tcphdr) + data_len, (unsigned short *)dummyhdr));
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
    iph->protocol = IPPROTO_TCP;
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
    iph->daddr = inet_addr(destination_ip); // put destination IP address
    printf("destIP:%.2X\n", iph->daddr);
    total_len += sizeof(struct iphdr);
    get_tcp(get_dummy_tcp(iph));
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