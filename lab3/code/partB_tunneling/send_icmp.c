#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "send_icmp.h"

extern unsigned char sendbuff[BUFFSIZE];
extern unsigned char recvbuff[BUFFSIZE];

extern tunnel *tnel;
extern int socket_fd;
extern struct sockaddr_in dest_addr;
extern unsigned short total_len;

static unsigned short icmp_checksum(
    unsigned short *ptr, int nbytes)
{
    long sum;
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

void init_icmp_packet(const char *myname,
                      const char *whoname, const char *dst_ip)
{
    tnel = (tunnel *)malloc(sizeof(tunnel));
    memcpy(tnel->sname, myname, strlen(myname));
    memcpy(tnel->dname, whoname, strlen(whoname));
    struct icmphdr *icmph = (struct icmphdr *)(sendbuff);

    icmph->code = 0;
    icmph->un.echo.id = htons(1);
    icmph->un.echo.sequence = htons(1);
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dst_ip, &dest_addr.sin_addr);
}

int senddata(char *data)
{
    // Exercise 2.
    // Add your code here:
    strcpy(tnel->data, data);

    // ICMP头、dname、sname必须全发完，data不必全发完
    total_len = sizeof(struct icmphdr) + sizeof(tnel->dname) + sizeof(tnel->sname) + strlen(data); // data发到'\0'即可

    // 装载ICMP头
    struct icmphdr *icmph = (struct icmphdr *)(sendbuff);
    // checksum置位0
    icmph->checksum = 0;
    // echo 请求包 type 字段为 8, code 字段为 0
    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    // id, 可随机，范围: 0 ~ 65535
    // sequence, 可随机，范围: 0 ~ 65535
    icmph->un.echo.id = 0;
    icmph->un.echo.sequence = 0;
    icmph->checksum = icmp_checksum((unsigned short *)icmph, total_len);

    // 装载ICMP tunnel数据体
    memcpy(sendbuff + sizeof(struct icmphdr), tnel, total_len - sizeof(struct icmphdr));

    // 加密
    encrypt();

    // 发送ICMP数据包，系统会自动填充IP头
    return sendto(socket_fd, sendbuff, total_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

int filter(struct icmphdr *icmph)
{
    tunnel *recv = (tunnel *)(recvbuff + sizeof(struct iphdr) + sizeof(struct icmphdr));
    if (icmph->type != ICMP_ECHO ||
        (strcmp(recv->dname, tnel->sname) != 0))
        return 0;
    return 1;
}

#define KEY "secretkey"
void encrypt()
{
    const int key_len = strlen(KEY);
    for (int i = 0; i < total_len; ++i)
    {
        sendbuff[i] = sendbuff[i] ^ KEY[i % key_len];
    }
}

void decrypt()
{
    struct iphdr *iph = (struct iphdr *)(recvbuff);
    const int key_len = strlen(KEY), ipheader_len = sizeof(struct iphdr), recv_len = ntohs(iph->tot_len);
    // printf("%d %d %d\n", recv_len, ntohs(iph->tot_len), ipheader_len);
    // printf("Let's decrypt! Received data: ");
    for (int i = ipheader_len; i < recv_len; ++i)
    {
        // 打印接收到的数据(密文)
        // if (recvbuff[i] >= 0x20 && recvbuff[i] <= 0x7E)
        //    printf("%c", recvbuff[i]);
        // else
        //    printf("X");

        recvbuff[i] = recvbuff[i] ^ KEY[(i - ipheader_len) % key_len];
    }
    // printf("\n");
}