#include <kernel/kernel.h>

int32_t black_syscall_gui_create_window(const char *black_title, int black_x, int black_y, int black_w, int black_h) {
    (void)black_title; (void)black_x; (void)black_y; (void)black_w; (void)black_h;
    return 0;
}

int32_t black_syscall_gui_draw(int black_wid, int black_x, int black_y, uint32_t black_color) {
    (void)black_wid; (void)black_x; (void)black_y; (void)black_color;
    return 0;
}
