#include <kernel/kernel.h>
#include <drivers/speaker.h>
#include <drivers/pit.h>

typedef struct {
    uint32_t black_freq;
    uint32_t black_duration;
} black_note_t;

static const black_note_t black_matrix_theme[] = {
    {330, 200}, {392, 200}, {440, 400}, {392, 200}, {330, 600}, {0, 0}
};

void black_music_play_theme(void) {
    for (int black_i = 0; black_matrix_theme[black_i].black_freq != 0 || black_matrix_theme[black_i].black_duration != 0; black_i++) {
        if (black_matrix_theme[black_i].black_freq > 0) {
            black_speaker_play(black_matrix_theme[black_i].black_freq);
        } else {
            black_speaker_stop();
        }
        
        uint64_t black_start = black_pit_get_uptime_ms();
        while (black_pit_get_uptime_ms() - black_start < black_matrix_theme[black_i].black_duration) {
            __asm__ volatile("pause");
        }
        black_speaker_stop();
        
        black_start = black_pit_get_uptime_ms();
        while (black_pit_get_uptime_ms() - black_start < 50) {
            __asm__ volatile("pause");
        }
    }
}
