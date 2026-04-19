#include <kernel/kernel.h>
#include <drivers/serial.h>

void black_assert_failed(const char *black_expr, const char *black_file, int black_line) {
    black_serial_puts("[ASSERT FAIL] ");
    black_serial_puts(black_expr);
    black_serial_puts(" at ");
    black_serial_puts(black_file);
    black_serial_putchar('\n');
    (void)black_line;
    for (;;) black_hlt();
}
