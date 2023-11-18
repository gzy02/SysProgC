#include <arpa/inet.h>
#include <errno.h>
#include <linux/icmp.h>
#include <linux/if_packet.h>
#include <malloc.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
struct ifreq ifreq_c, ifreq_i, ifreq_arp; /// for each ioctl keep diffrent ifreq structure otherwise error may come in sending(sendto )
int sock_raw;
unsigned char *sendbuff;

#define DESTMAC0 0xff
#define DESTMAC1 0xff
#define DESTMAC2 0xff
#define DESTMAC3 0xff
#define DESTMAC4 0xff
#define DESTMAC5 0xff

#define TPA0 172
#define TPA1 23
#define TPA2 112
#define TPA3 1
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
    printf("Mac= %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
           (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]),
           (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]),
           (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]),
           (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]),
           (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]),
           (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]));
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
    eth->h_proto = htons(ETH_P_ARP);
    printf("ethernet packaging done.\n");
    total_len += sizeof(struct ethhdr);
}

void get_arp()
{
    struct ether_arp *arp = (struct ether_arp *)(sendbuff + sizeof(struct ethhdr));
    arp->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    arp->ea_hdr.ar_pro = htons(ETH_P_IP);
    arp->ea_hdr.ar_hln = 6;
    arp->ea_hdr.ar_pln = 4;
    arp->ea_hdr.ar_op = htons(ARPOP_REQUEST); // ARP request

    memset(&ifreq_c, 0, sizeof(ifreq_c));
    // Replace eth0 with your network interface
    strncpy(ifreq_c.ifr_name, "eth0", IFNAMSIZ - 1);
    if ((ioctl(sock_raw, SIOCGIFHWADDR, &ifreq_c)) < 0)
        printf("error in SIOCGIFHWADDR ioctl reading");

    /* sender hardware address */
    memcpy(arp->arp_sha, &ifreq_c.ifr_hwaddr.sa_data, sizeof(arp->arp_sha));

    /* sender protocol address */
    in_addr_t spa = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_arp.ifr_addr))->sin_addr)));
    memcpy(arp->arp_spa, &spa, sizeof(arp->arp_spa));

    /* target hardware address */
    // ARP request报文中，目的MAC地址的值为0
    memset(arp->arp_tha, 0, sizeof(arp->arp_tha));

    /* target protocol address */
    // 不妨让目的IP地址为 172.23.112.1，网络中以大端序存储
    // 因此高位字节存储在低地址，低位字节存储在高地址
    // 所以第一个字节为172，第二个字节为23，第三个字节为112，第四个字节为1
    arp->arp_tpa[0] = TPA0;
    arp->arp_tpa[1] = TPA1;
    arp->arp_tpa[2] = TPA2;
    arp->arp_tpa[3] = TPA3;
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
    get_arp();
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
