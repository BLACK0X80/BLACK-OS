#include <kernel/kernel.h>

int32_t black_syscall_net_socket(int black_type, int black_proto) {
    (void)black_type; (void)black_proto;
    return -1;
}

int32_t black_syscall_net_send(int black_fd, const void *black_buf, uint32_t black_len) {
    (void)black_fd; (void)black_buf; (void)black_len;
    return -1;
}

int32_t black_syscall_net_recv(int black_fd, void *black_buf, uint32_t black_len) {
    (void)black_fd; (void)black_buf; (void)black_len;
    return -1;
}
