#ifndef _BLACK_UDP_H
#define _BLACK_UDP_H
#include <stdint.h>
void black_udp_init(void);
int black_udp_send(uint32_t black_dst, uint16_t black_sport, uint16_t black_dport, const void *black_data, uint16_t black_len);
int black_udp_receive(uint16_t black_port, void *black_buf, uint16_t black_max);
#endif
