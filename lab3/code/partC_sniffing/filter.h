#ifndef MINIWIRESHARK_FILTER_H
#define MINIWIRESHARK_FILTER_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include <stdbool.h>

/*
 * name: filterByMacAddress
 * aug1: pointer of buffer.
 * aug2: pointer of mac address string. and input format is like : AC-23-74-EF-7D-9F
 * aug3: type, 0 -> source address, 1 -> destination address, filter by source/destination mac address.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByMacAddress(unsigned char *buffer, char *mac_addr, int type);

/*
 * name: filterByIpAddress
 * aug1: pointer of buffer.
 * aug2: IP address.
 * aug3: type, 0->source IP, 1->destination IP.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByIpAddress(unsigned char *buffer, char *ip_addr, int type);

/*
 * name: filterByPort
 * aug1: pointer of buffer.
 * aug2: TCP/UDP port value.
 * aug3: type, 0->source port, 1->destination port.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByPort(unsigned char *buffer, unsigned short port, int type);

/*
 * name: filterByProtocol
 * aug1: pointer of buffer.
 * aug: type of packet you want to filter, 1->ARP; 2->ICMP; 3->TCP, 4->UDP.
 * return: bool, if the filtering succeed, return true; else return false.
 */
bool filterByProtocol(unsigned char * buffer, int type);

#endif //MINIWIRESHARK_FILTER_H
