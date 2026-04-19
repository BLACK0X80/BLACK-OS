#ifndef _BLACK_ARP_H
#define _BLACK_ARP_H
#include <stdint.h>
void black_arp_init(void);
int black_arp_resolve(uint32_t black_ip, uint8_t *black_mac);
void black_arp_receive(void *black_data, uint16_t black_len);
#endif
