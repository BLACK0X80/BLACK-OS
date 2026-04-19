#include <kernel/kernel.h>
#include <mm/vmm.h>

#define BLACK_IOAPIC_REGSEL 0x00
#define BLACK_IOAPIC_WIN    0x10

static uint32_t black_ioapic_phys = 0xFEC00000;
static uint32_t *black_ioapic_base = (uint32_t *)0xFEC00000;

static void black_ioapic_write(uint8_t black_off, uint32_t black_val)
{
    black_ioapic_base[BLACK_IOAPIC_REGSEL / 4] = black_off;
    black_ioapic_base[BLACK_IOAPIC_WIN / 4] = black_val;
}

static uint32_t black_ioapic_read(uint8_t black_off)
{
    black_ioapic_base[BLACK_IOAPIC_REGSEL / 4] = black_off;
    return black_ioapic_base[BLACK_IOAPIC_WIN / 4];
}

void black_ioapic_init(void)
{
    black_vmm_map_page((uint32_t)black_ioapic_base, black_ioapic_phys, PAGE_PRESENT | PAGE_WRITE | PAGE_NOCACHE);
    uint32_t black_max_intr = (black_ioapic_read(0x01) >> 16) & 0xFF;
    for (uint32_t black_i = 0; black_i <= black_max_intr; black_i++) {
        black_ioapic_write(0x10 + black_i * 2, 0x10000 | (32 + black_i));
        black_ioapic_write(0x10 + black_i * 2 + 1, 0);
    }
}
