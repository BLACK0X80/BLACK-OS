#include <drivers/pit.h>
#include <arch/x86/idt.h>
#include <kernel/kernel.h>

static volatile uint64_t black_pit_ticks = 0;
static uint32_t black_pit_frequency = 100;
static volatile int black_use_lapic = 0;

static void black_pit_irq_handler(black_registers_t *black_regs)
{
    (void)black_regs;
    black_pit_ticks++;
}

void black_pit_init(uint32_t black_freq)
{
    black_pit_frequency = black_freq;
    uint32_t black_divisor = 1193180 / black_freq;

    black_outb(0x43, 0x36);
    black_outb(0x40, (uint8_t)(black_divisor & 0xFF));
    black_outb(0x40, (uint8_t)((black_divisor >> 8) & 0xFF));

    black_idt_set_handler(32, black_pit_irq_handler);
}

uint64_t black_pit_get_ticks(void) { return black_pit_ticks; }

uint64_t black_pit_get_uptime_ms(void)
{
    return (black_pit_ticks * 1000) / black_pit_frequency;
}

void black_pit_sleep(uint32_t black_ms)
{
    uint64_t black_target = black_pit_get_uptime_ms() + black_ms;
    while (black_pit_get_uptime_ms() < black_target) {
        black_hlt();
    }
}

void black_pit_use_lapic_timer(void) { black_use_lapic = 1; }
