#include <kernel/kernel.h>
#include <net/net.h>

void black_tcp_init(void) {
}

int black_tcp_connect(uint32_t black_dst, uint16_t black_port) {
    (void)black_dst; (void)black_port;
    return 0;
}

int black_tcp_send(int black_fd, const void *black_data, uint16_t black_len) {
    (void)black_fd; (void)black_data; (void)black_len;
    return 0;
}

int black_tcp_receive(int black_fd, void *black_buf, uint16_t black_max) {
    (void)black_fd; (void)black_buf; (void)black_max;
    return 0;
}

void black_tcp_close(int black_fd) {
    (void)black_fd;
}
