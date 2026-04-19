#include <kernel/kernel.h>
#include <net/net.h>

int black_loopback_send(const void *black_data, uint16_t black_len) {
    (void)black_data; (void)black_len;
    return 0;
}

int black_loopback_receive(void *black_buf, uint16_t black_max_len) {
    (void)black_buf; (void)black_max_len;
    return 0;
}
