#ifndef _BLACK_LOOPBACK_H
#define _BLACK_LOOPBACK_H
#include <stdint.h>
int black_loopback_send(const void *black_data, uint16_t black_len);
int black_loopback_receive(void *black_buf, uint16_t black_max_len);
#endif
