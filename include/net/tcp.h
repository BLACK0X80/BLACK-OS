#ifndef _BLACK_TCP_H
#define _BLACK_TCP_H
#include <stdint.h>
void black_tcp_init(void);
int black_tcp_connect(uint32_t black_dst, uint16_t black_port);
int black_tcp_send(int black_fd, const void *black_data, uint16_t black_len);
int black_tcp_receive(int black_fd, void *black_buf, uint16_t black_max);
void black_tcp_close(int black_fd);
#endif
