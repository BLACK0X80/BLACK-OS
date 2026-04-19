#ifndef _BLACK_VGA_H
#define _BLACK_VGA_H

#include <stdint.h>

#define BLACK_VGA_WIDTH    80
#define BLACK_VGA_HEIGHT   25
#define BLACK_VGA_MEMORY   0xC00B8000

#define BLACK_VGA_BLACK        0x0
#define BLACK_VGA_GREEN        0xA
#define BLACK_VGA_DARK_GREEN   0x2
#define BLACK_VGA_MATRIX_ATTR  ((BLACK_VGA_BLACK << 4) | BLACK_VGA_GREEN)
#define BLACK_VGA_OK_ATTR      ((BLACK_VGA_BLACK << 4) | BLACK_VGA_GREEN)
#define BLACK_VGA_ERR_ATTR     ((BLACK_VGA_BLACK << 4) | 0x4)

void black_vga_init(void);
void black_vga_clear(void);
void black_vga_putchar(char black_c);
void black_vga_puts(const char *black_s);
void black_vga_put_dec(uint32_t black_val);
void black_vga_put_hex(uint32_t black_val);
void black_vga_set_color(uint8_t black_fg, uint8_t black_bg);
void black_vga_puts_ok(void);
void black_vga_puts_fail(void);
void black_vga_puts_colored(const char *black_s, uint8_t black_attr);
void black_vga_scroll(void);
void black_vga_set_cursor(int black_x, int black_y);

#define vga_init       black_vga_init
#define vga_clear      black_vga_clear
#define vga_putchar    black_vga_putchar
#define vga_puts       black_vga_puts
#define vga_put_dec    black_vga_put_dec
#define vga_put_hex    black_vga_put_hex
#define vga_puts_ok    black_vga_puts_ok
#define vga_puts_fail  black_vga_puts_fail

#endif
