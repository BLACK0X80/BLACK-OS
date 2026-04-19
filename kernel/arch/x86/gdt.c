#include <arch/x86/gdt.h>
#include <kernel/kernel.h>
#include <string.h>

typedef struct {
    uint16_t black_limit_low;
    uint16_t black_base_low;
    uint8_t  black_base_mid;
    uint8_t  black_access;
    uint8_t  black_granularity;
    uint8_t  black_base_high;
} __attribute__((packed)) black_gdt_entry_t;

typedef struct {
    uint32_t black_prev_tss;
    uint32_t black_esp0;
    uint32_t black_ss0;
    uint32_t black_esp1, black_ss1;
    uint32_t black_esp2, black_ss2;
    uint32_t black_cr3, black_eip, black_eflags;
    uint32_t black_eax, black_ecx, black_edx, black_ebx;
    uint32_t black_esp, black_ebp, black_esi, black_edi;
    uint32_t black_es, black_cs, black_ss, black_ds, black_fs, black_gs;
    uint32_t black_ldt;
    uint16_t black_trap;
    uint16_t black_iomap_base;
} __attribute__((packed)) black_tss_t;

typedef struct {
    uint16_t black_limit;
    uint32_t black_base;
} __attribute__((packed)) black_gdt_ptr_t;

static black_gdt_entry_t black_gdt[6];
static black_tss_t black_tss;
static black_gdt_ptr_t black_gdtr;

extern void black_gdt_flush(uint32_t);
extern void black_tss_flush(void);

static void black_gdt_set_gate(int black_num, uint32_t black_base, uint32_t black_limit,
                                uint8_t black_access, uint8_t black_gran)
{
    black_gdt[black_num].black_base_low    = (black_base & 0xFFFF);
    black_gdt[black_num].black_base_mid    = (black_base >> 16) & 0xFF;
    black_gdt[black_num].black_base_high   = (black_base >> 24) & 0xFF;
    black_gdt[black_num].black_limit_low   = (black_limit & 0xFFFF);
    black_gdt[black_num].black_granularity = ((black_limit >> 16) & 0x0F) | (black_gran & 0xF0);
    black_gdt[black_num].black_access      = black_access;
}

void black_gdt_init(void)
{
    black_gdtr.black_limit = sizeof(black_gdt) - 1;
    black_gdtr.black_base  = (uint32_t)&black_gdt;

    black_gdt_set_gate(0, 0, 0, 0, 0);
    black_gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    black_gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    black_gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    black_gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    memset(&black_tss, 0, sizeof(black_tss));
    black_tss.black_ss0 = 0x10;
    black_tss.black_iomap_base = sizeof(black_tss);

    uint32_t black_tss_base = (uint32_t)&black_tss;
    uint32_t black_tss_limit = sizeof(black_tss) - 1;
    black_gdt_set_gate(5, black_tss_base, black_tss_limit, 0xE9, 0x00);

    black_gdt_flush((uint32_t)&black_gdtr);
    black_tss_flush();
}

void black_gdt_set_kernel_stack(uint32_t black_stack)
{
    black_tss.black_esp0 = black_stack;
}

void black_gdt_set_kernel_stack_cpu(uint32_t black_cpu_id, uint32_t black_stack)
{
    (void)black_cpu_id;
    black_tss.black_esp0 = black_stack;
}
