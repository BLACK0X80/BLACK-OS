#include <drivers/pci.h>
#include <kernel/kernel.h>
#include <string.h>

#define BLACK_PCI_MAX_DEVICES 64
#define BLACK_PCI_CONFIG_ADDR 0xCF8
#define BLACK_PCI_CONFIG_DATA 0xCFC

static black_pci_device_t black_pci_devices[BLACK_PCI_MAX_DEVICES];
static uint32_t black_pci_device_count = 0;

uint32_t black_pci_read(uint8_t black_bus, uint8_t black_slot, uint8_t black_func, uint8_t black_off)
{
    uint32_t black_addr = (uint32_t)((black_bus << 16) | (black_slot << 11) | (black_func << 8) | (black_off & 0xFC) | 0x80000000);
    black_outl(BLACK_PCI_CONFIG_ADDR, black_addr);
    return black_inl(BLACK_PCI_CONFIG_DATA);
}

void black_pci_write(uint8_t black_bus, uint8_t black_slot, uint8_t black_func, uint8_t black_off, uint32_t black_val)
{
    uint32_t black_addr = (uint32_t)((black_bus << 16) | (black_slot << 11) | (black_func << 8) | (black_off & 0xFC) | 0x80000000);
    black_outl(BLACK_PCI_CONFIG_ADDR, black_addr);
    black_outl(BLACK_PCI_CONFIG_DATA, black_val);
}

static void black_pci_scan_device(uint8_t black_bus, uint8_t black_slot, uint8_t black_func)
{
    uint32_t black_reg0 = black_pci_read(black_bus, black_slot, black_func, 0);
    uint16_t black_vendor = black_reg0 & 0xFFFF;
    uint16_t black_device = (black_reg0 >> 16) & 0xFFFF;
    if (black_vendor == 0xFFFF) return;
    if (black_pci_device_count >= BLACK_PCI_MAX_DEVICES) return;

    uint32_t black_reg2 = black_pci_read(black_bus, black_slot, black_func, 0x08);
    uint32_t black_reg3 = black_pci_read(black_bus, black_slot, black_func, 0x0C);
    uint32_t black_reg15 = black_pci_read(black_bus, black_slot, black_func, 0x3C);

    black_pci_device_t *black_dev = &black_pci_devices[black_pci_device_count++];
    black_dev->black_bus = black_bus;
    black_dev->black_slot = black_slot;
    black_dev->black_func = black_func;
    black_dev->black_vendor_id = black_vendor;
    black_dev->black_device_id = black_device;
    black_dev->black_class_code = (black_reg2 >> 24) & 0xFF;
    black_dev->black_subclass = (black_reg2 >> 16) & 0xFF;
    black_dev->black_prog_if = (black_reg2 >> 8) & 0xFF;
    black_dev->black_header_type = (black_reg3 >> 16) & 0xFF;
    black_dev->black_irq_line = black_reg15 & 0xFF;

    for (int black_i = 0; black_i < 6; black_i++) {
        black_dev->black_bar[black_i] = black_pci_read(black_bus, black_slot, black_func, 0x10 + black_i * 4);
    }

    if ((black_dev->black_header_type & 0x80) && black_func == 0) {
        for (uint8_t black_f = 1; black_f < 8; black_f++) {
            black_pci_scan_device(black_bus, black_slot, black_f);
        }
    }
}

void black_pci_init(void)
{
    black_pci_device_count = 0;
    memset(black_pci_devices, 0, sizeof(black_pci_devices));
    for (uint16_t black_bus = 0; black_bus < 256; black_bus++) {
        for (uint8_t black_slot = 0; black_slot < 32; black_slot++) {
            black_pci_scan_device((uint8_t)black_bus, black_slot, 0);
        }
    }
}

uint32_t black_pci_get_device_count(void) { return black_pci_device_count; }
black_pci_device_t *black_pci_get_device(uint32_t black_idx) { return black_idx < black_pci_device_count ? &black_pci_devices[black_idx] : NULL; }
