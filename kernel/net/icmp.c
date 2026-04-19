#include <kernel/kernel.h>
#include <net/net.h>

void black_icmp_init(void) {
}

int black_icmp_send_echo(uint32_t black_dst_ip, uint16_t black_id, uint16_t black_seq) {
    (void)black_dst_ip; (void)black_id; (void)black_seq;
    return 0;
}

void black_icmp_receive(void *black_data, uint16_t black_len) {
    (void)black_data; (void)black_len;
}
