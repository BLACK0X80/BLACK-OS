#include <arch/x86/idt.h>
#include <kernel/kernel.h>
#include <drivers/serial.h>
#include <string.h>

typedef struct {
    uint16_t black_base_low;
    uint16_t black_sel;
    uint8_t  black_zero;
    uint8_t  black_flags;
    uint16_t black_base_high;
} __attribute__((packed)) black_idt_entry_t;

typedef struct {
    uint16_t black_limit;
    uint32_t black_base;
} __attribute__((packed)) black_idt_ptr_t;

static black_idt_entry_t black_idt_entries[256];
static black_idt_ptr_t black_idtr;
static black_isr_handler_t black_handlers[256];
static int black_apic_mode = 0;

extern void black_idt_flush(uint32_t);

extern void black_isr0(void);  extern void black_isr1(void);
extern void black_isr2(void);  extern void black_isr3(void);
extern void black_isr4(void);  extern void black_isr5(void);
extern void black_isr6(void);  extern void black_isr7(void);
extern void black_isr8(void);  extern void black_isr9(void);
extern void black_isr10(void); extern void black_isr11(void);
extern void black_isr12(void); extern void black_isr13(void);
extern void black_isr14(void); extern void black_isr15(void);
extern void black_isr16(void); extern void black_isr17(void);
extern void black_isr18(void); extern void black_isr19(void);

extern void black_irq0(void);  extern void black_irq1(void);
extern void black_irq2(void);  extern void black_irq3(void);
extern void black_irq4(void);  extern void black_irq5(void);
extern void black_irq6(void);  extern void black_irq7(void);
extern void black_irq8(void);  extern void black_irq9(void);
extern void black_irq10(void); extern void black_irq11(void);
extern void black_irq12(void); extern void black_irq13(void);
extern void black_irq14(void); extern void black_irq15(void);

static void black_idt_set_gate(uint8_t black_num, uint32_t black_base,
                                uint16_t black_sel, uint8_t black_flags)
{
    black_idt_entries[black_num].black_base_low  = black_base & 0xFFFF;
    black_idt_entries[black_num].black_base_high = (black_base >> 16) & 0xFFFF;
    black_idt_entries[black_num].black_sel       = black_sel;
    black_idt_entries[black_num].black_zero      = 0;
    black_idt_entries[black_num].black_flags     = black_flags;
}

static void black_pic_remap(void)
{
    black_outb(0x20, 0x11); black_io_wait();
    black_outb(0xA0, 0x11); black_io_wait();
    black_outb(0x21, 0x20); black_io_wait();
    black_outb(0xA1, 0x28); black_io_wait();
    black_outb(0x21, 0x04); black_io_wait();
    black_outb(0xA1, 0x02); black_io_wait();
    black_outb(0x21, 0x01); black_io_wait();
    black_outb(0xA1, 0x01); black_io_wait();
    black_outb(0x21, 0x00); black_io_wait();
    black_outb(0xA1, 0x00); black_io_wait();
}

void black_pic_disable(void)
{
    black_outb(0xA1, 0xFF);
    black_outb(0x21, 0xFF);
}

void black_idt_init(void)
{
    memset(black_idt_entries, 0, sizeof(black_idt_entries));
    memset(black_handlers, 0, sizeof(black_handlers));

    black_pic_remap();

    black_idt_set_gate(0,  (uint32_t)black_isr0,  0x08, 0x8E);
    black_idt_set_gate(1,  (uint32_t)black_isr1,  0x08, 0x8E);
    black_idt_set_gate(2,  (uint32_t)black_isr2,  0x08, 0x8E);
    black_idt_set_gate(3,  (uint32_t)black_isr3,  0x08, 0x8E);
    black_idt_set_gate(4,  (uint32_t)black_isr4,  0x08, 0x8E);
    black_idt_set_gate(5,  (uint32_t)black_isr5,  0x08, 0x8E);
    black_idt_set_gate(6,  (uint32_t)black_isr6,  0x08, 0x8E);
    black_idt_set_gate(7,  (uint32_t)black_isr7,  0x08, 0x8E);
    black_idt_set_gate(8,  (uint32_t)black_isr8,  0x08, 0x8E);
    black_idt_set_gate(9,  (uint32_t)black_isr9,  0x08, 0x8E);
    black_idt_set_gate(10, (uint32_t)black_isr10, 0x08, 0x8E);
    black_idt_set_gate(11, (uint32_t)black_isr11, 0x08, 0x8E);
    black_idt_set_gate(12, (uint32_t)black_isr12, 0x08, 0x8E);
    black_idt_set_gate(13, (uint32_t)black_isr13, 0x08, 0x8E);
    black_idt_set_gate(14, (uint32_t)black_isr14, 0x08, 0x8E);
    black_idt_set_gate(15, (uint32_t)black_isr15, 0x08, 0x8E);
    black_idt_set_gate(16, (uint32_t)black_isr16, 0x08, 0x8E);
    black_idt_set_gate(17, (uint32_t)black_isr17, 0x08, 0x8E);
    black_idt_set_gate(18, (uint32_t)black_isr18, 0x08, 0x8E);
    black_idt_set_gate(19, (uint32_t)black_isr19, 0x08, 0x8E);

    black_idt_set_gate(32, (uint32_t)black_irq0,  0x08, 0x8E);
    black_idt_set_gate(33, (uint32_t)black_irq1,  0x08, 0x8E);
    black_idt_set_gate(34, (uint32_t)black_irq2,  0x08, 0x8E);
    black_idt_set_gate(35, (uint32_t)black_irq3,  0x08, 0x8E);
    black_idt_set_gate(36, (uint32_t)black_irq4,  0x08, 0x8E);
    black_idt_set_gate(37, (uint32_t)black_irq5,  0x08, 0x8E);
    black_idt_set_gate(38, (uint32_t)black_irq6,  0x08, 0x8E);
    black_idt_set_gate(39, (uint32_t)black_irq7,  0x08, 0x8E);
    black_idt_set_gate(40, (uint32_t)black_irq8,  0x08, 0x8E);
    black_idt_set_gate(41, (uint32_t)black_irq9,  0x08, 0x8E);
    black_idt_set_gate(42, (uint32_t)black_irq10, 0x08, 0x8E);
    black_idt_set_gate(43, (uint32_t)black_irq11, 0x08, 0x8E);
    black_idt_set_gate(44, (uint32_t)black_irq12, 0x08, 0x8E);
    black_idt_set_gate(45, (uint32_t)black_irq13, 0x08, 0x8E);
    black_idt_set_gate(46, (uint32_t)black_irq14, 0x08, 0x8E);
    black_idt_set_gate(47, (uint32_t)black_irq15, 0x08, 0x8E);

    black_idt_set_gate(0x80, (uint32_t)black_isr19, 0x08, 0xEE);

    black_idtr.black_limit = sizeof(black_idt_entries) - 1;
    black_idtr.black_base  = (uint32_t)&black_idt_entries;
    black_idt_flush((uint32_t)&black_idtr);
}

void black_idt_set_handler(uint8_t black_num, black_isr_handler_t black_handler)
{
    black_handlers[black_num] = black_handler;
}

void black_idt_set_apic_mode(int black_mode) { black_apic_mode = black_mode; }
int  black_idt_is_apic_mode(void) { return black_apic_mode; }

void black_isr_dispatch(black_registers_t *black_regs)
{
    if (black_handlers[black_regs->black_int_no]) {
        black_handlers[black_regs->black_int_no](black_regs);
    }

    if (black_regs->black_int_no >= 32 && black_regs->black_int_no < 48 && !black_apic_mode) {
        if (black_regs->black_int_no >= 40) {
            black_outb(0xA0, 0x20);
        }
        black_outb(0x20, 0x20);
    }
}
