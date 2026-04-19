#ifndef _BLACK_SERIAL_H
#define _BLACK_SERIAL_H

#include <stdint.h>

#define BLACK_COM1  0x3F8

void black_serial_init(void);
void black_serial_putchar(char black_c);
void black_serial_puts(const char *black_s);
void black_serial_put_hex(uint32_t black_val);
void black_serial_put_dec(uint32_t black_val);
int  black_serial_printf(const char *black_fmt, ...);

#define serial_init      black_serial_init
#define serial_puts      black_serial_puts
#define serial_putchar   black_serial_putchar
#define serial_printf    black_serial_printf

#endif
