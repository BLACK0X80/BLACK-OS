#include <kernel/kernel.h>
#include <string.h>

#define BLACK_ETH_MTU       1500
#define BLACK_ETH_HLEN      14
#define BLACK_ETH_ALEN      6

#define BLACK_ETH_TYPE_IP   0x0800
#define BLACK_ETH_TYPE_ARP  0x0806

#define BLACK_IP_PROTO_ICMP 1
#define BLACK_IP_PROTO_TCP  6
#define BLACK_IP_PROTO_UDP  17

#define BLACK_ARP_REQUEST   1
#define BLACK_ARP_REPLY     2

#define BLACK_MAX_ARP_CACHE 32
#define BLACK_MAX_SOCKETS   32

typedef struct { uint8_t black_bytes[4]; } black_ipv4_addr_t;
typedef struct { uint8_t black_bytes[6]; } black_mac_addr_t;

typedef struct {
    black_mac_addr_t black_dst;
    black_mac_addr_t black_src;
    uint16_t black_type;
} __attribute__((packed)) black_eth_header_t;

typedef struct {
    uint8_t  black_ihl_ver;
    uint8_t  black_tos;
    uint16_t black_total_len;
    uint16_t black_id;
    uint16_t black_frag;
    uint8_t  black_ttl;
    uint8_t  black_protocol;
    uint16_t black_checksum;
    black_ipv4_addr_t black_src;
    black_ipv4_addr_t black_dst;
} __attribute__((packed)) black_ip_header_t;

typedef struct {
    uint16_t black_src_port;
    uint16_t black_dst_port;
    uint32_t black_seq;
    uint32_t black_ack;
    uint8_t  black_data_offset;
    uint8_t  black_flags;
    uint16_t black_window;
    uint16_t black_checksum;
    uint16_t black_urgent;
} __attribute__((packed)) black_tcp_header_t;

typedef struct {
    uint16_t black_src_port;
    uint16_t black_dst_port;
    uint16_t black_length;
    uint16_t black_checksum;
} __attribute__((packed)) black_udp_header_t;

typedef struct {
    uint8_t  black_type;
    uint8_t  black_code;
    uint16_t black_checksum;
    uint16_t black_id;
    uint16_t black_seq;
} __attribute__((packed)) black_icmp_header_t;

typedef struct {
    black_ipv4_addr_t black_ip;
    black_mac_addr_t black_mac;
    uint32_t black_timestamp;
    int black_valid;
} black_arp_entry_t;

typedef struct {
    int black_type;
    int black_protocol;
    black_ipv4_addr_t black_local_addr;
    uint16_t black_local_port;
    black_ipv4_addr_t black_remote_addr;
    uint16_t black_remote_port;
    int black_state;
    int black_active;
    uint8_t black_recv_buf[2048];
    uint32_t black_recv_len;
    uint32_t black_tcp_seq;
    uint32_t black_tcp_ack;
} black_socket_t;

static black_ipv4_addr_t black_local_ip = {{ 10, 0, 2, 15 }};
static black_mac_addr_t  black_local_mac = {{ 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 }};
static black_ipv4_addr_t black_gateway   = {{ 10, 0, 2, 2 }};
static black_ipv4_addr_t black_dns       = {{ 8, 8, 8, 8 }};
static black_ipv4_addr_t black_subnet    = {{ 255, 255, 255, 0 }};
static black_arp_entry_t black_arp_cache[BLACK_MAX_ARP_CACHE];
static black_socket_t    black_sockets[BLACK_MAX_SOCKETS];

static uint16_t black_htons(uint16_t black_v) { return (black_v >> 8) | (black_v << 8); }
static uint16_t black_ntohs(uint16_t black_v) { return black_htons(black_v); }

static uint16_t black_checksum(void *black_data, int black_len)
{
    uint32_t black_sum = 0;
    uint16_t *black_p = (uint16_t *)black_data;
    while (black_len > 1) { black_sum += *black_p++; black_len -= 2; }
    if (black_len == 1) black_sum += *(uint8_t *)black_p;
    while (black_sum >> 16) black_sum = (black_sum & 0xFFFF) + (black_sum >> 16);
    return ~(uint16_t)black_sum;
}

void black_net_init(void)
{
    memset(black_arp_cache, 0, sizeof(black_arp_cache));
    memset(black_sockets, 0, sizeof(black_sockets));
    (void)black_checksum; (void)black_ntohs;
    (void)black_subnet; (void)black_dns;
}

void black_arp_add(black_ipv4_addr_t *black_ip, black_mac_addr_t *black_mac)
{
    for (int black_i = 0; black_i < BLACK_MAX_ARP_CACHE; black_i++) {
        if (!black_arp_cache[black_i].black_valid) {
            memcpy(&black_arp_cache[black_i].black_ip, black_ip, 4);
            memcpy(&black_arp_cache[black_i].black_mac, black_mac, 6);
            black_arp_cache[black_i].black_valid = 1;
            return;
        }
    }
}

black_mac_addr_t *black_arp_lookup(black_ipv4_addr_t *black_ip)
{
    for (int black_i = 0; black_i < BLACK_MAX_ARP_CACHE; black_i++) {
        if (black_arp_cache[black_i].black_valid && memcmp(&black_arp_cache[black_i].black_ip, black_ip, 4) == 0)
            return &black_arp_cache[black_i].black_mac;
    }
    return NULL;
}



void black_net_get_ip(black_ipv4_addr_t *black_out) { memcpy(black_out, &black_local_ip, 4); }
void black_net_get_mac(black_mac_addr_t *black_out) { memcpy(black_out, &black_local_mac, 6); }
void black_net_get_gateway(black_ipv4_addr_t *black_out) { memcpy(black_out, &black_gateway, 4); }
void black_net_set_ip(black_ipv4_addr_t *black_ip) { memcpy(&black_local_ip, black_ip, 4); }
