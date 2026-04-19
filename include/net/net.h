#ifndef _BLACK_NET_H
#define _BLACK_NET_H

#include <stdint.h>

void black_net_init(void);
int black_net_send(const void *black_data, uint16_t black_len);
int black_net_receive(void *black_buf, uint16_t black_max_len);

#endif
