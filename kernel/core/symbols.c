#include <kernel/kernel.h>

typedef struct {
    uint32_t black_addr;
    char black_name[64];
} black_symbol_entry_t;

static black_symbol_entry_t black_symbols[256];
static uint32_t black_symbol_count = 0;

void black_symbols_init(void) {
    black_symbol_count = 0;
}

void black_symbols_add(uint32_t black_addr, const char *black_name) {
    if (black_symbol_count >= 256) return;
    black_symbols[black_symbol_count].black_addr = black_addr;
    int black_i = 0;
    while (black_name[black_i] && black_i < 63) {
        black_symbols[black_symbol_count].black_name[black_i] = black_name[black_i];
        black_i++;
    }
    black_symbols[black_symbol_count].black_name[black_i] = '\0';
    black_symbol_count++;
}

const char *black_symbols_lookup(uint32_t black_addr) {
    for (uint32_t black_i = 0; black_i < black_symbol_count; black_i++) {
        if (black_symbols[black_i].black_addr == black_addr) {
            return black_symbols[black_i].black_name;
        }
    }
    return "<unknown>";
}
