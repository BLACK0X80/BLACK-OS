#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

#define BLACK_TERM_COLS 80
#define BLACK_TERM_ROWS 25

static char black_term_buf[BLACK_TERM_ROWS][BLACK_TERM_COLS];
static int black_term_cx = 0;
static int black_term_cy = 0;

void black_gui_terminal_init(void) {
    for (int black_r = 0; black_r < BLACK_TERM_ROWS; black_r++) {
        for (int black_c = 0; black_c < BLACK_TERM_COLS; black_c++) {
            black_term_buf[black_r][black_c] = ' ';
        }
    }
    black_term_cx = 0;
    black_term_cy = 0;
}

void black_gui_terminal_putchar(char black_ch) {
    if (black_ch == '\n') {
        black_term_cx = 0;
        black_term_cy++;
    } else {
        if (black_term_cx < BLACK_TERM_COLS) {
            black_term_buf[black_term_cy][black_term_cx] = black_ch;
            black_term_cx++;
        }
    }
    if (black_term_cy >= BLACK_TERM_ROWS) {
        for (int black_r = 1; black_r < BLACK_TERM_ROWS; black_r++) {
            for (int black_c = 0; black_c < BLACK_TERM_COLS; black_c++) {
                black_term_buf[black_r - 1][black_c] = black_term_buf[black_r][black_c];
            }
        }
        for (int black_c = 0; black_c < BLACK_TERM_COLS; black_c++) {
            black_term_buf[BLACK_TERM_ROWS - 1][black_c] = ' ';
        }
        black_term_cy = BLACK_TERM_ROWS - 1;
    }
}

void black_gui_terminal_render(int black_ox, int black_oy) {
    for (int black_r = 0; black_r < BLACK_TERM_ROWS; black_r++) {
        for (int black_c = 0; black_c < BLACK_TERM_COLS; black_c++) {
            char black_s[2] = { black_term_buf[black_r][black_c], '\0' };
            black_fb_drawstring(black_ox + black_c * 8, black_oy + black_r * 16, black_s, 0x00FF00, 0x000000);
        }
    }
}
