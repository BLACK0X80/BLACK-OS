#include <kernel/kernel.h>
#include <acpi/acpi.h>
#include <mm/vmm.h>
#include <string.h>

static black_rsdp_t *black_rsdp = NULL;
static black_acpi_header_t *black_rsdt = NULL;
static black_acpi_madt_t *black_madt = NULL;
static uint32_t black_lapic_addr = 0;
static uint32_t black_ioapic_addr = 0;
static uint8_t  black_ioapic_id = 0;

#define BLACK_ACPI_MAX_CPUS 16

typedef struct {
    uint8_t black_apic_id;
    uint8_t black_processor_id;
    uint8_t black_enabled;
} black_acpi_cpu_entry_t;

static black_acpi_cpu_entry_t black_acpi_cpus[BLACK_ACPI_MAX_CPUS];
static uint32_t black_acpi_cpu_count = 0;

static int black_acpi_checksum(void *black_ptr, int black_len)
{
    uint8_t black_sum = 0;
    for (int black_i = 0; black_i < black_len; black_i++)
        black_sum += ((uint8_t *)black_ptr)[black_i];
    return black_sum == 0;
}

static void black_acpi_ensure_mapped(uint32_t black_phys, uint32_t black_size)
{
    uint32_t black_start = black_phys & 0xFFFFF000;
    uint32_t black_end = (black_phys + black_size + 0xFFF) & 0xFFFFF000;
    for (uint32_t black_page = black_start; black_page < black_end; black_page += 0x1000) {
        uint32_t black_virt = black_page + KERNEL_VMA;
        black_vmm_map_page(black_virt, black_page, PAGE_PRESENT | PAGE_WRITE);
    }
}

static void *black_acpi_find_rsdp(void)
{
    uint16_t black_ebda_seg = *(uint16_t *)(0x40E + KERNEL_VMA);
    uint32_t black_ebda_addr = (uint32_t)black_ebda_seg << 4;

    if (black_ebda_addr) {
        for (uint32_t black_a = black_ebda_addr; black_a < black_ebda_addr + 1024; black_a += 16) {
            uint8_t *black_p = (uint8_t *)(black_a + KERNEL_VMA);
            if (black_p[0] == 'R' && black_p[1] == 'S' && black_p[2] == 'D' && black_p[3] == ' ' &&
                black_p[4] == 'P' && black_p[5] == 'T' && black_p[6] == 'R' && black_p[7] == ' ') {
                if (black_acpi_checksum(black_p, 20))
                    return (void *)black_p;
            }
        }
    }

    for (uint32_t black_a = 0xE0000; black_a < 0x100000; black_a += 16) {
        uint8_t *black_p = (uint8_t *)(black_a + KERNEL_VMA);
        if (black_p[0] == 'R' && black_p[1] == 'S' && black_p[2] == 'D' && black_p[3] == ' ' &&
            black_p[4] == 'P' && black_p[5] == 'T' && black_p[6] == 'R' && black_p[7] == ' ') {
            if (black_acpi_checksum(black_p, 20))
                return (void *)black_p;
        }
    }

    return NULL;
}

static void black_acpi_parse_madt(black_acpi_madt_t *black_mt)
{
    if (!black_mt) return;
    black_madt = black_mt;
    black_lapic_addr = black_madt->black_lapic_addr;

    uint8_t *black_ptr = (uint8_t *)black_madt + sizeof(black_acpi_madt_t);
    uint8_t *black_end = (uint8_t *)black_madt + black_madt->black_header.black_length;

    while (black_ptr < black_end) {
        uint8_t black_type = black_ptr[0];
        uint8_t black_len = black_ptr[1];

        if (black_type == 0 && black_acpi_cpu_count < BLACK_ACPI_MAX_CPUS) {
            uint8_t black_pid = black_ptr[2];
            uint8_t black_aid = black_ptr[3];
            uint32_t black_flags = *(uint32_t *)(black_ptr + 4);
            if (black_flags & 1) {
                black_acpi_cpus[black_acpi_cpu_count].black_apic_id = black_aid;
                black_acpi_cpus[black_acpi_cpu_count].black_processor_id = black_pid;
                black_acpi_cpus[black_acpi_cpu_count].black_enabled = 1;
                black_acpi_cpu_count++;
            }
        } else if (black_type == 1 && black_ioapic_addr == 0) {
            black_ioapic_id = black_ptr[2];
            black_ioapic_addr = *(uint32_t *)(black_ptr + 4);
        }

        black_ptr += black_len;
    }
}

int black_acpi_init(void)
{
    black_rsdp = black_acpi_find_rsdp();
    if (!black_rsdp) return -1;

    uint32_t black_rsdt_phys = black_rsdp->black_rsdt_address;
    if (black_rsdt_phys >= 0x40000000) return -2;

    black_acpi_ensure_mapped(black_rsdt_phys, 0x1000);
    black_rsdt = (black_acpi_header_t *)(black_rsdt_phys + KERNEL_VMA);

    if (!black_acpi_checksum(black_rsdt, black_rsdt->black_length)) return -3;

    black_acpi_ensure_mapped(black_rsdt_phys, black_rsdt->black_length);

    int black_entries = (black_rsdt->black_length - sizeof(black_acpi_header_t)) / 4;
    uint32_t *black_ptrs = (uint32_t *)((uint32_t)black_rsdt + sizeof(black_acpi_header_t));

    for (int black_i = 0; black_i < black_entries; black_i++) {
        uint32_t black_tphys = black_ptrs[black_i];
        if (black_tphys >= 0x40000000) continue;
        black_acpi_ensure_mapped(black_tphys, sizeof(black_acpi_header_t));
        black_acpi_header_t *black_h = (black_acpi_header_t *)(black_tphys + KERNEL_VMA);
        if (black_h->black_signature[0] == 'A' && black_h->black_signature[1] == 'P' &&
            black_h->black_signature[2] == 'I' && black_h->black_signature[3] == 'C') {
            black_acpi_ensure_mapped(black_tphys, black_h->black_length);
            if (black_acpi_checksum(black_h, black_h->black_length))
                black_acpi_parse_madt((black_acpi_madt_t *)black_h);
        }
    }

    return 0;
}

void *black_acpi_find_table(const char *black_sig)
{
    if (!black_rsdt) return NULL;
    int black_entries = (black_rsdt->black_length - sizeof(black_acpi_header_t)) / 4;
    uint32_t *black_ptrs = (uint32_t *)((uint32_t)black_rsdt + sizeof(black_acpi_header_t));
    for (int black_i = 0; black_i < black_entries; black_i++) {
        uint32_t black_tphys = black_ptrs[black_i];
        if (black_tphys >= 0x40000000) continue;
        black_acpi_ensure_mapped(black_tphys, sizeof(black_acpi_header_t));
        black_acpi_header_t *black_h = (black_acpi_header_t *)(black_tphys + KERNEL_VMA);
        if (memcmp(black_h->black_signature, black_sig, 4) == 0 && black_acpi_checksum(black_h, black_h->black_length))
            return black_h;
    }
    return NULL;
}

uint32_t black_acpi_get_lapic_addr(void) { return black_lapic_addr; }
uint32_t black_acpi_get_ioapic_addr(void) { return black_ioapic_addr; }
uint8_t  black_acpi_get_ioapic_id(void) { return black_ioapic_id; }
int black_acpi_is_available(void) { return black_rsdp != NULL; }
uint32_t black_acpi_get_cpu_count(void) { return black_acpi_cpu_count; }

int black_acpi_get_cpu_apic_id(uint32_t black_index, uint8_t *black_apic_id)
{
    if (black_index >= black_acpi_cpu_count) return -1;
    *black_apic_id = black_acpi_cpus[black_index].black_apic_id;
    return 0;
}
