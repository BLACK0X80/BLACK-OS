#ifndef _BLACK_SOCKET_H
#define _BLACK_SOCKET_H
#include <stdint.h>
void black_socket_init(void);
int black_socket_create(int black_type, int black_proto);
int black_socket_bind(int black_fd, uint32_t black_ip, uint16_t black_port);
int black_socket_close(int black_fd);
#endif
