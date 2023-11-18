#ifndef PACKET_PROCESS_H
#define PACKET_PROCESS_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




// print format of etherNet-header
void mac_header(unsigned char *buffer);

// print format of ip-header
void ip_header(unsigned char *buffer);

// printf format of arp-header
void arp_header(unsigned char *buffer);

// print format of tcp-header
void tcp_header(unsigned char *buffer);

// print format of udp-header
void udp_header(unsigned char *buffer);

// print format of icmp-header
void icmp_header(unsigned char *buffer);

// print format of payload
void payload(unsigned char *buffer,int buffer_len);

// print ICMP packet
void ICMP_printer(unsigned char *buffer, int buffer_len);

// print ARP packet
void ARP_printer(unsigned char *buffer);

// print TCP packet
void TCP_printer(unsigned char *buffer, int buffer_len);

// print UDP packet
void UDP_printer(unsigned char *buffer, int buffer_len);

// packet process func
void packet_process(unsigned char *buffer, int buffer_len);

#endif


