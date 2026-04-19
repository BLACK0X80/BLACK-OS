#include <drivers/serial.h>
#include <kernel/kernel.h>
#include <string.h>
#include <stdint.h>

static int black_serial_ready = 0;

void black_serial_init(void)
{
    black_outb(BLACK_COM1 + 1, 0x00);
    black_outb(BLACK_COM1 + 3, 0x80);
    black_outb(BLACK_COM1 + 0, 0x03);
    black_outb(BLACK_COM1 + 1, 0x00);
    black_outb(BLACK_COM1 + 3, 0x03);
    black_outb(BLACK_COM1 + 2, 0xC7);
    black_outb(BLACK_COM1 + 4, 0x0B);
    black_serial_ready = 1;
}

void black_serial_putchar(char black_c)
{
    if (!black_serial_ready) return;
    while (!(black_inb(BLACK_COM1 + 5) & 0x20));
    black_outb(BLACK_COM1, black_c);
}

void black_serial_puts(const char *black_s)
{
    while (*black_s) {
        black_serial_putchar(*black_s++);
    }
}

void black_serial_put_hex(uint32_t black_val)
{
    char black_hex[9];
    for (int black_i = 7; black_i >= 0; black_i--) {
        int black_nibble = black_val & 0xF;
        black_hex[black_i] = black_nibble < 10 ? '0' + black_nibble : 'A' + black_nibble - 10;
        black_val >>= 4;
    }
    black_hex[8] = '\0';
    black_serial_puts(black_hex);
}

void black_serial_put_dec(uint32_t black_val)
{
    char black_buf[12];
    int black_i = 0;
    if (black_val == 0) { black_serial_putchar('0'); return; }
    while (black_val > 0) {
        black_buf[black_i++] = '0' + (black_val % 10);
        black_val /= 10;
    }
    while (black_i > 0) black_serial_putchar(black_buf[--black_i]);
}

int black_serial_printf(const char *black_fmt, ...)
{
    __builtin_va_list black_args;
    __builtin_va_start(black_args, black_fmt);
    int black_count = 0;

    while (*black_fmt) {
        if (*black_fmt == '%') {
            black_fmt++;
            switch (*black_fmt) {
                case 's': {
                    const char *black_s = __builtin_va_arg(black_args, const char *);
                    if (black_s) black_serial_puts(black_s);
                    else black_serial_puts("(null)");
                    break;
                }
                case 'd': {
                    int black_v = __builtin_va_arg(black_args, int);
                    if (black_v < 0) { black_serial_putchar('-'); black_v = -black_v; }
                    black_serial_put_dec((uint32_t)black_v);
                    break;
                }
                case 'u': {
                    uint32_t black_v = __builtin_va_arg(black_args, uint32_t);
                    black_serial_put_dec(black_v);
                    break;
                }
                case 'x': {
                    uint32_t black_v = __builtin_va_arg(black_args, uint32_t);
                    black_serial_put_hex(black_v);
                    break;
                }
                case 'c': {
                    char black_c = (char)__builtin_va_arg(black_args, int);
                    black_serial_putchar(black_c);
                    break;
                }
                case '%':
                    black_serial_putchar('%');
                    break;
            }
        } else {
            black_serial_putchar(*black_fmt);
        }
        black_fmt++;
        black_count++;
    }

    __builtin_va_end(black_args);
    return black_count;
}
