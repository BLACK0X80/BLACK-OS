#ifndef _BLACK_SYMBOLS_H
#define _BLACK_SYMBOLS_H
#include <stdint.h>
void black_symbols_init(void);
void black_symbols_add(uint32_t black_addr, const char *black_name);
const char *black_symbols_lookup(uint32_t black_addr);
#endif
