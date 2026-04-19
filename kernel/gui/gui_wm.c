#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

#define BLACK_MAX_WINDOWS 16

typedef struct {
    int black_x, black_y, black_w, black_h;
    char black_title[64];
    int black_active;
    int black_focused;
} black_window_t;

static black_window_t black_windows[BLACK_MAX_WINDOWS];
static int black_window_count = 0;

void black_gui_wm_init(void) {
    black_window_count = 0;
    for (int black_i = 0; black_i < BLACK_MAX_WINDOWS; black_i++) {
        black_windows[black_i].black_active = 0;
    }
}

int black_gui_wm_create(const char *black_title, int black_x, int black_y, int black_w, int black_h) {
    if (black_window_count >= BLACK_MAX_WINDOWS) return -1;
    black_window_t *black_win = &black_windows[black_window_count];
    black_win->black_x = black_x;
    black_win->black_y = black_y;
    black_win->black_w = black_w;
    black_win->black_h = black_h;
    black_win->black_active = 1;
    black_win->black_focused = 1;
    int black_i = 0;
    while (black_title[black_i] && black_i < 63) {
        black_win->black_title[black_i] = black_title[black_i];
        black_i++;
    }
    black_win->black_title[black_i] = '\0';
    return black_window_count++;
}

void black_gui_wm_render(void) {
    for (int black_i = 0; black_i < black_window_count; black_i++) {
        if (!black_windows[black_i].black_active) continue;
        black_window_t *black_w = &black_windows[black_i];
        for (int black_y = black_w->black_y; black_y < black_w->black_y + 20; black_y++) {
            for (int black_x = black_w->black_x; black_x < black_w->black_x + black_w->black_w; black_x++) {
                black_fb_putpixel(black_x, black_y, 0x003300);
            }
        }
        black_fb_drawstring(black_w->black_x + 4, black_w->black_y + 4, black_w->black_title, 0x00FF00, 0x003300);
        for (int black_y = black_w->black_y + 20; black_y < black_w->black_y + black_w->black_h; black_y++) {
            for (int black_x = black_w->black_x; black_x < black_w->black_x + black_w->black_w; black_x++) {
                black_fb_putpixel(black_x, black_y, 0x001100);
            }
        }
    }
}

void black_gui_wm_close(int black_id) {
    if (black_id >= 0 && black_id < BLACK_MAX_WINDOWS) {
        black_windows[black_id].black_active = 0;
    }
}
