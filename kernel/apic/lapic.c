#include <kernel/kernel.h>
#include <mm/vmm.h>

#define BLACK_LAPIC_ID          0x0020
#define BLACK_LAPIC_VER         0x0030
#define BLACK_LAPIC_TPR         0x0080
#define BLACK_LAPIC_EOI         0x00B0
#define BLACK_LAPIC_SVR         0x00F0
#define BLACK_LAPIC_ESR         0x0280
#define BLACK_LAPIC_ICRLO       0x0300
#define BLACK_LAPIC_ICRHI       0x0310
#define BLACK_LAPIC_TIMER       0x0320
#define BLACK_LAPIC_PCINT       0x0340
#define BLACK_LAPIC_LINT0       0x0350
#define BLACK_LAPIC_LINT1       0x0360
#define BLACK_LAPIC_ERROR       0x0370
#define BLACK_LAPIC_TICR        0x0380
#define BLACK_LAPIC_TCCR        0x0390
#define BLACK_LAPIC_TDCR        0x03E0

static uint32_t black_lapic_phys = 0xFEE00000;
static uint32_t *black_lapic_base = (uint32_t *)0xFEE00000;

static void black_lapic_write(uint32_t black_reg, uint32_t black_val) { black_lapic_base[black_reg / 4] = black_val; }
static uint32_t black_lapic_read(uint32_t black_reg) { return black_lapic_base[black_reg / 4]; }

void black_lapic_init(void)
{
    black_vmm_map_page((uint32_t)black_lapic_base, black_lapic_phys, PAGE_PRESENT | PAGE_WRITE | PAGE_NOCACHE);
    black_lapic_write(BLACK_LAPIC_SVR, 0x100 | 0xFF);
    black_lapic_write(BLACK_LAPIC_TDCR, 0x03);
    black_lapic_write(BLACK_LAPIC_TIMER, 32 | 0x20000);
    black_lapic_write(BLACK_LAPIC_TICR, 10000000);
    black_lapic_write(BLACK_LAPIC_LINT0, 0x10000);
    black_lapic_write(BLACK_LAPIC_LINT1, 0x10000);
    if (((black_lapic_read(BLACK_LAPIC_VER) >> 16) & 0xFF) >= 4)
        black_lapic_write(BLACK_LAPIC_PCINT, 0x10000);
    black_lapic_write(BLACK_LAPIC_ERROR, 0x10000);
    black_lapic_write(BLACK_LAPIC_ESR, 0);
    black_lapic_write(BLACK_LAPIC_ESR, 0);
    black_lapic_write(BLACK_LAPIC_EOI, 0);
    black_lapic_write(BLACK_LAPIC_ICRHI, 0);
    black_lapic_write(BLACK_LAPIC_ICRLO, 0x000C4500);
    black_lapic_write(BLACK_LAPIC_TPR, 0);
}

void black_lapic_eoi(void) { black_lapic_write(BLACK_LAPIC_EOI, 0); }
uint32_t black_lapic_get_id(void) { return black_lapic_read(BLACK_LAPIC_ID) >> 24; }

void black_lapic_send_ipi(uint8_t black_apic_id, uint8_t black_vector)
{
    black_lapic_write(BLACK_LAPIC_ICRHI, black_apic_id << 24);
    black_lapic_write(BLACK_LAPIC_ICRLO, black_vector | 0x4000);
    while (black_lapic_read(BLACK_LAPIC_ICRLO) & 0x1000) __asm__ volatile("pause");
}
