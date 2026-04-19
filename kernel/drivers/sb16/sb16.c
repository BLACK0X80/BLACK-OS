#include <kernel/kernel.h>

#define BLACK_SB16_DSP_RESET 0x226
#define BLACK_SB16_DSP_READ  0x22A
#define BLACK_SB16_DSP_WRITE 0x22C
#define BLACK_SB16_DSP_STATUS 0x22E

static int black_sb16_ready = 0;

static void black_sb16_write(uint8_t black_val) {
    while (black_inb(BLACK_SB16_DSP_WRITE) & 0x80) __asm__ volatile("pause");
    black_outb(BLACK_SB16_DSP_WRITE, black_val);
}

static uint8_t black_sb16_read(void) {
    while (!(black_inb(BLACK_SB16_DSP_STATUS) & 0x80)) __asm__ volatile("pause");
    return black_inb(BLACK_SB16_DSP_READ);
}

void black_sb16_init(void) {
    black_outb(BLACK_SB16_DSP_RESET, 1);
    for (int black_i = 0; black_i < 1000; black_i++) __asm__ volatile("pause");
    black_outb(BLACK_SB16_DSP_RESET, 0);
    
    int black_timeout = 10000;
    while (black_timeout--) {
        if ((black_inb(BLACK_SB16_DSP_STATUS) & 0x80) && black_sb16_read() == 0xAA) {
            black_sb16_ready = 1;
            break;
        }
    }
}

void black_sb16_play(void *black_data, uint32_t black_len) {
    if (!black_sb16_ready) return;
    (void)black_data; (void)black_len; (void)black_sb16_write;
}
