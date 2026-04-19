#include <kernel/kernel.h>
#include <drivers/serial.h>
#include <drivers/vga.h>

void black_panic(const char *black_msg)
{
    black_cli();

    black_serial_puts("\n\n=== BLACK KERNEL PANIC ===\n");
    black_serial_puts(black_msg);
    black_serial_puts("\n");

    uint32_t black_eip, black_ebp, black_esp;
    __asm__ volatile("mov %%ebp, %0" : "=r"(black_ebp));
    __asm__ volatile("mov %%esp, %0" : "=r"(black_esp));
    __asm__ volatile("1: lea 1b, %0" : "=r"(black_eip));

    black_serial_puts("EIP: 0x");
    black_serial_put_hex(black_eip);
    black_serial_puts("\nEBP: 0x");
    black_serial_put_hex(black_ebp);
    black_serial_puts("\nESP: 0x");
    black_serial_put_hex(black_esp);
    black_serial_puts("\n\nStack trace:\n");

    uint32_t *black_frame = (uint32_t *)black_ebp;
    for (int black_i = 0; black_i < 16 && black_frame; black_i++) {
        uint32_t black_ret = black_frame[1];
        if (black_ret < 0xC0000000 || black_ret > 0xD0000000) break;
        black_serial_puts("  #");
        black_serial_put_dec(black_i);
        black_serial_puts(": 0x");
        black_serial_put_hex(black_ret);
        black_serial_putchar('\n');
        black_frame = (uint32_t *)black_frame[0];
    }

    black_serial_puts("\n=== SYSTEM HALTED ===\n");

    black_vga_set_color(0x4, 0x0);
    black_vga_puts("\n\n  *** BLACK KERNEL PANIC ***\n  ");
    black_vga_puts(black_msg);
    black_vga_puts("\n  System halted.\n");

    for (;;) black_hlt();
}
