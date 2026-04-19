#include <kernel/kernel.h>
#include <drivers/pit.h>

void black_speaker_play(uint32_t black_freq) {
    if (black_freq == 0) {
        black_outb(0x61, black_inb(0x61) & 0xFC);
        return;
    }
    uint32_t black_div = 1193180 / black_freq;
    black_outb(0x43, 0xB6);
    black_outb(0x42, (uint8_t)(black_div & 0xFF));
    black_outb(0x42, (uint8_t)(black_div >> 8));
    uint8_t black_tmp = black_inb(0x61);
    if (black_tmp != (black_tmp | 3)) {
        black_outb(0x61, black_tmp | 3);
    }
}

void black_speaker_stop(void) {
    black_outb(0x61, black_inb(0x61) & 0xFC);
}

void black_speaker_beep(void) {
    black_speaker_play(1000);
    uint64_t black_start = black_pit_get_uptime_ms();
    while (black_pit_get_uptime_ms() - black_start < 100) __asm__ volatile("pause");
    black_speaker_stop();
}
