#include <kernel/kernel.h>
#include <drivers/serial.h>

void black_klog(int black_level, const char *black_subsys, const char *black_fmt, ...)
{
    static const char *black_level_names[] = { "DBG", "INF", "WRN", "ERR", "FTL" };

    if (black_level < 0 || black_level > 4) black_level = 1;

    black_serial_putchar('[');
    black_serial_puts(black_level_names[black_level]);
    black_serial_putchar(']');
    black_serial_putchar('[');
    black_serial_puts(black_subsys);
    black_serial_putchar(']');
    black_serial_putchar(' ');

    __builtin_va_list black_args;
    __builtin_va_start(black_args, black_fmt);

    while (*black_fmt) {
        if (*black_fmt == '%') {
            black_fmt++;
            switch (*black_fmt) {
                case 's': {
                    const char *black_s = __builtin_va_arg(black_args, const char *);
                    black_serial_puts(black_s ? black_s : "(null)");
                    break;
                }
                case 'd': {
                    int black_v = __builtin_va_arg(black_args, int);
                    if (black_v < 0) { black_serial_putchar('-'); black_v = -black_v; }
                    black_serial_put_dec((uint32_t)black_v);
                    break;
                }
                case 'u':
                    black_serial_put_dec(__builtin_va_arg(black_args, uint32_t));
                    break;
                case 'x':
                    black_serial_put_hex(__builtin_va_arg(black_args, uint32_t));
                    break;
                default:
                    black_serial_putchar(*black_fmt);
            }
        } else {
            black_serial_putchar(*black_fmt);
        }
        black_fmt++;
    }

    black_serial_putchar('\n');
    __builtin_va_end(black_args);
}
