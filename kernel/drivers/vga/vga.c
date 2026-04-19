#include <drivers/vga.h>
#include <kernel/kernel.h>
#include <string.h>

static uint16_t *black_vga_buffer = (uint16_t *)BLACK_VGA_MEMORY;
static int black_vga_x = 0;
static int black_vga_y = 0;
static uint8_t black_vga_attr = BLACK_VGA_MATRIX_ATTR;
static void (*black_vga_cb)(char) = NULL;

void black_vga_set_callback(void (*cb)(char))
{
    black_vga_cb = cb;
}

void black_vga_init(void)
{
    black_vga_buffer = (uint16_t *)BLACK_VGA_MEMORY;
    black_vga_x = 0;
    black_vga_y = 0;
    black_vga_attr = BLACK_VGA_MATRIX_ATTR;
}

void black_vga_clear(void)
{
    uint16_t black_blank = (uint16_t)(' ' | (black_vga_attr << 8));
    for (int black_i = 0; black_i < BLACK_VGA_WIDTH * BLACK_VGA_HEIGHT; black_i++) {
        black_vga_buffer[black_i] = black_blank;
    }
    black_vga_x = 0;
    black_vga_y = 0;
}

void black_vga_scroll(void)
{
    if (black_vga_y >= BLACK_VGA_HEIGHT) {
        for (int black_i = 0; black_i < BLACK_VGA_WIDTH * (BLACK_VGA_HEIGHT - 1); black_i++) {
            black_vga_buffer[black_i] = black_vga_buffer[black_i + BLACK_VGA_WIDTH];
        }
        uint16_t black_blank = (uint16_t)(' ' | (black_vga_attr << 8));
        for (int black_i = BLACK_VGA_WIDTH * (BLACK_VGA_HEIGHT - 1);
             black_i < BLACK_VGA_WIDTH * BLACK_VGA_HEIGHT; black_i++) {
            black_vga_buffer[black_i] = black_blank;
        }
        black_vga_y = BLACK_VGA_HEIGHT - 1;
    }
}

void black_vga_set_cursor(int black_x, int black_y)
{
    uint16_t black_pos = black_y * BLACK_VGA_WIDTH + black_x;
    black_outb(0x3D4, 0x0F);
    black_outb(0x3D5, (uint8_t)(black_pos & 0xFF));
    black_outb(0x3D4, 0x0E);
    black_outb(0x3D5, (uint8_t)((black_pos >> 8) & 0xFF));
}

void black_vga_putchar(char black_c)
{
    if (black_vga_cb) {
        black_vga_cb(black_c);
    }
    if (black_c == '\n') {
        black_vga_x = 0;
        black_vga_y++;
    } else if (black_c == '\r') {
        black_vga_x = 0;
    } else if (black_c == '\t') {
        black_vga_x = (black_vga_x + 4) & ~3;
    } else if (black_c == '\b') {
        if (black_vga_x > 0) {
            black_vga_x--;
            black_vga_buffer[black_vga_y * BLACK_VGA_WIDTH + black_vga_x] =
                (uint16_t)(' ' | (black_vga_attr << 8));
        }
    } else {
        black_vga_buffer[black_vga_y * BLACK_VGA_WIDTH + black_vga_x] =
            (uint16_t)(black_c | (black_vga_attr << 8));
        black_vga_x++;
    }

    if (black_vga_x >= BLACK_VGA_WIDTH) {
        black_vga_x = 0;
        black_vga_y++;
    }

    black_vga_scroll();
    black_vga_set_cursor(black_vga_x, black_vga_y);
}

void black_vga_puts(const char *black_s)
{
    while (*black_s) {
        black_vga_putchar(*black_s++);
    }
}

void black_vga_set_color(uint8_t black_fg, uint8_t black_bg)
{
    black_vga_attr = (black_bg << 4) | black_fg;
}

void black_vga_puts_ok(void)
{
    uint8_t black_old = black_vga_attr;
    black_vga_attr = BLACK_VGA_OK_ATTR;
    black_vga_puts("[  OK  ]");
    black_vga_attr = black_old;
}

void black_vga_puts_fail(void)
{
    uint8_t black_old = black_vga_attr;
    black_vga_attr = BLACK_VGA_ERR_ATTR;
    black_vga_puts("[ FAIL ]");
    black_vga_attr = black_old;
}

void black_vga_puts_colored(const char *black_s, uint8_t black_attr_val)
{
    uint8_t black_old = black_vga_attr;
    black_vga_attr = black_attr_val;
    black_vga_puts(black_s);
    black_vga_attr = black_old;
}

void black_vga_put_dec(uint32_t black_val)
{
    char black_buf[12];
    int black_i = 0;
    if (black_val == 0) { black_vga_putchar('0'); return; }
    while (black_val > 0) {
        black_buf[black_i++] = '0' + (black_val % 10);
        black_val /= 10;
    }
    while (black_i > 0) black_vga_putchar(black_buf[--black_i]);
}

void black_vga_put_hex(uint32_t black_val)
{
    black_vga_puts("0x");
    for (int black_i = 28; black_i >= 0; black_i -= 4) {
        int black_nibble = (black_val >> black_i) & 0xF;
        black_vga_putchar(black_nibble < 10 ? '0' + black_nibble : 'A' + black_nibble - 10);
    }
}
