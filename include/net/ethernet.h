#ifndef _BLACK_ETHERNET_H
#define _BLACK_ETHERNET_H
#include <stdint.h>
void black_ethernet_init(void);
int black_ethernet_send(const uint8_t *black_dst_mac, uint16_t black_type, const void *black_data, uint16_t black_len);
void black_ethernet_receive(void *black_frame, uint16_t black_len);
#endif
