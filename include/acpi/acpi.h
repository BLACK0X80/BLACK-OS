#ifndef _BLACK_ACPI_H
#define _BLACK_ACPI_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    char black_signature[8];
    uint8_t black_checksum;
    char black_oem_id[6];
    uint8_t black_revision;
    uint32_t black_rsdt_address;
    uint32_t black_length;
    uint64_t black_xsdt_address;
    uint8_t black_ext_checksum;
    uint8_t black_reserved[3];
} __attribute__((packed)) black_rsdp_t;

typedef struct {
    char black_signature[4];
    uint32_t black_length;
    uint8_t black_revision;
    uint8_t black_checksum;
    char black_oem_id[6];
    char black_oem_table_id[8];
    uint32_t black_oem_revision;
    uint32_t black_creator_id;
    uint32_t black_creator_revision;
} __attribute__((packed)) black_acpi_header_t;

typedef struct {
    black_acpi_header_t black_header;
    uint32_t black_lapic_addr;
    uint32_t black_flags;
} __attribute__((packed)) black_acpi_madt_t;

int black_acpi_init(void);
void *black_acpi_find_table(const char *black_sig);
uint32_t black_acpi_get_lapic_addr(void);
uint32_t black_acpi_get_ioapic_addr(void);
uint8_t  black_acpi_get_ioapic_id(void);
int black_acpi_is_available(void);
uint32_t black_acpi_get_cpu_count(void);
int black_acpi_get_cpu_apic_id(uint32_t black_index, uint8_t *black_apic_id);

#endif
