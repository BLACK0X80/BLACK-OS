#include <drivers/e1000.h>
#include <kernel/kernel.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/heap.h>
#include <string.h>

#define BLACK_E1000_NUM_RX_DESC 32
#define BLACK_E1000_NUM_TX_DESC 8

typedef struct {
    volatile uint64_t black_addr;
    volatile uint16_t black_length;
    volatile uint16_t black_checksum;
    volatile uint8_t  black_status;
    volatile uint8_t  black_errors;
    volatile uint16_t black_special;
} __attribute__((packed)) black_e1000_rx_desc_t;

typedef struct {
    volatile uint64_t black_addr;
    volatile uint16_t black_length;
    volatile uint8_t  black_cso;
    volatile uint8_t  black_cmd;
    volatile uint8_t  black_status;
    volatile uint8_t  black_css;
    volatile uint16_t black_special;
} __attribute__((packed)) black_e1000_tx_desc_t;

static uint32_t black_e1000_mem_base;
static uint16_t black_e1000_io_base;
static int black_e1000_has_eeprom;

static black_e1000_rx_desc_t *black_rx_descs;
static black_e1000_tx_desc_t *black_tx_descs;

static uint8_t black_rx_bufs[BLACK_E1000_NUM_RX_DESC][8192];
static uint8_t black_tx_bufs[BLACK_E1000_NUM_TX_DESC][8192];

static uint32_t black_rx_cur = 0;
static uint32_t black_tx_cur = 0;

static void black_e1000_write_reg(uint32_t black_addr, uint32_t black_val) {
    if (black_e1000_mem_base) *(volatile uint32_t *)(black_e1000_mem_base + black_addr) = black_val;
    else { black_outl(black_e1000_io_base, black_addr); black_outl(black_e1000_io_base + 4, black_val); }
}

static uint32_t black_e1000_read_reg(uint32_t black_addr) {
    if (black_e1000_mem_base) return *(volatile uint32_t *)(black_e1000_mem_base + black_addr);
    black_outl(black_e1000_io_base, black_addr);
    return black_inl(black_e1000_io_base + 4);
}

void black_e1000_init(black_pci_device_t *black_dev) {
    if (!black_dev) return;
    black_e1000_mem_base = black_dev->black_bar[0] & ~0xF;
    black_e1000_io_base  = black_dev->black_bar[1] & ~0x3;
    black_vmm_map_page(black_e1000_mem_base, black_e1000_mem_base, PAGE_PRESENT | PAGE_WRITE | PAGE_NOCACHE);

    black_e1000_write_reg(0x0020, 0x10);
    uint32_t black_status = black_e1000_read_reg(0x0008);
    black_e1000_has_eeprom = (black_status & 0x10) ? 1 : 0;

    black_rx_descs = (black_e1000_rx_desc_t *)kcalloc(1, sizeof(black_e1000_rx_desc_t) * BLACK_E1000_NUM_RX_DESC + 16);
    for (int black_i = 0; black_i < BLACK_E1000_NUM_RX_DESC; black_i++) {
        black_rx_descs[black_i].black_addr = (uint64_t)(uint32_t)black_rx_bufs[black_i];
        black_rx_descs[black_i].black_status = 0;
    }
    
    black_e1000_write_reg(0x2800, (uint32_t)black_rx_descs);
    black_e1000_write_reg(0x2804, 0);
    black_e1000_write_reg(0x2808, BLACK_E1000_NUM_RX_DESC * 16);
    black_e1000_write_reg(0x2810, 0);
    black_e1000_write_reg(0x2818, BLACK_E1000_NUM_RX_DESC - 1);
    black_e1000_write_reg(0x0100, 0x02 | 0x04 | 0x08 | (1 << 15));

    black_tx_descs = (black_e1000_tx_desc_t *)kcalloc(1, sizeof(black_e1000_tx_desc_t) * BLACK_E1000_NUM_TX_DESC + 16);
    for (int black_i = 0; black_i < BLACK_E1000_NUM_TX_DESC; black_i++) {
        black_tx_descs[black_i].black_addr = 0;
        black_tx_descs[black_i].black_cmd = 0;
    }
    
    black_e1000_write_reg(0x3800, (uint32_t)black_tx_descs);
    black_e1000_write_reg(0x3804, 0);
    black_e1000_write_reg(0x3808, BLACK_E1000_NUM_TX_DESC * 16);
    black_e1000_write_reg(0x3810, 0);
    black_e1000_write_reg(0x3818, 0);
    black_e1000_write_reg(0x0400, 0x02 | 0x08 | 0x01 | 0x10);

    black_e1000_write_reg(0x00D0, 0x1F6DC);
    black_e1000_write_reg(0x00D8, 0x1F6DC);
    black_e1000_read_reg(0x00C0);
}

int black_e1000_send(const void *black_data, uint16_t black_len) {
    if (!black_e1000_mem_base) return -1;
    black_e1000_tx_desc_t *black_td = &black_tx_descs[black_tx_cur];
    memcpy(black_tx_bufs[black_tx_cur], black_data, black_len);
    black_td->black_addr = (uint64_t)(uint32_t)black_tx_bufs[black_tx_cur];
    black_td->black_length = black_len;
    black_td->black_cmd = 0x08 | 0x02 | 0x01;
    black_td->black_status = 0;
    black_tx_cur = (black_tx_cur + 1) % BLACK_E1000_NUM_TX_DESC;
    black_e1000_write_reg(0x3818, black_tx_cur);
    return 0;
}

int black_e1000_receive(void *black_buf, uint16_t black_max_len) {
    if (!black_e1000_mem_base) return 0;
    black_e1000_rx_desc_t *black_rd = &black_rx_descs[black_rx_cur];
    if (!(black_rd->black_status & 0x01)) return 0;
    uint16_t black_len = black_rd->black_length;
    if (black_len > black_max_len) black_len = black_max_len;
    memcpy(black_buf, black_rx_bufs[black_rx_cur], black_len);
    black_rd->black_status = 0;
    black_e1000_write_reg(0x2818, black_rx_cur);
    black_rx_cur = (black_rx_cur + 1) % BLACK_E1000_NUM_RX_DESC;
    return black_len;
}
