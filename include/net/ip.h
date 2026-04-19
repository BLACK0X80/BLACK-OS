#ifndef _BLACK_IP_H
#define _BLACK_IP_H
#include <stdint.h>
void black_ip_init(void);
int black_ip_send(uint32_t black_dst, uint8_t black_proto, const void *black_data, uint16_t black_len);
void black_ip_receive(void *black_data, uint16_t black_len);
#endif
