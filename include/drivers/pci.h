#ifndef _BLACK_PCI_H
#define _BLACK_PCI_H

#include <stdint.h>

typedef struct {
    uint8_t  black_bus;
    uint8_t  black_slot;
    uint8_t  black_func;
    uint16_t black_vendor_id;
    uint16_t black_device_id;
    uint8_t  black_class_code;
    uint8_t  black_subclass;
    uint8_t  black_prog_if;
    uint8_t  black_header_type;
    uint32_t black_bar[6];
    uint8_t  black_irq_line;
} black_pci_device_t;

typedef black_pci_device_t pci_device_t;

void black_pci_init(void);
uint32_t black_pci_get_device_count(void);
black_pci_device_t *black_pci_get_device(uint32_t black_idx);
uint32_t black_pci_read(uint8_t black_bus, uint8_t black_slot, uint8_t black_func, uint8_t black_off);
void black_pci_write(uint8_t black_bus, uint8_t black_slot, uint8_t black_func, uint8_t black_off, uint32_t black_val);

#define pci_init             black_pci_init
#define pci_get_device_count black_pci_get_device_count
#define pci_get_device       black_pci_get_device

#endif
