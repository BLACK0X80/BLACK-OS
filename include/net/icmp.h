#ifndef _BLACK_ICMP_H
#define _BLACK_ICMP_H
#include <stdint.h>
void black_icmp_init(void);
int black_icmp_send_echo(uint32_t black_dst_ip, uint16_t black_id, uint16_t black_seq);
void black_icmp_receive(void *black_data, uint16_t black_len);
#endif
