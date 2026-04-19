#include <kernel/kernel.h>
#include <net/net.h>

void black_udp_init(void) {
}

int black_udp_send(uint32_t black_dst, uint16_t black_sport, uint16_t black_dport, const void *black_data, uint16_t black_len) {
    (void)black_dst; (void)black_sport; (void)black_dport; (void)black_data; (void)black_len;
    return 0;
}

int black_udp_receive(uint16_t black_port, void *black_buf, uint16_t black_max) {
    (void)black_port; (void)black_buf; (void)black_max;
    return 0;
}
