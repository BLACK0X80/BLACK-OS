#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

static uint32_t black_cursor_x = 0;
static uint32_t black_cursor_y = 0;

void black_gui_cursor_init(void) {
    black_cursor_x = 400;
    black_cursor_y = 300;
}

void black_gui_cursor_move(int black_dx, int black_dy) {
    black_cursor_x += black_dx;
    black_cursor_y += black_dy;
    if (black_cursor_x > 1024) black_cursor_x = 1024;
    if (black_cursor_y > 768) black_cursor_y = 768;
}

void black_gui_cursor_draw(void) {
    for (int black_i = 0; black_i < 12; black_i++) {
        black_fb_putpixel(black_cursor_x, black_cursor_y + black_i, 0x00FF00);
        black_fb_putpixel(black_cursor_x + black_i, black_cursor_y, 0x00FF00);
    }
}

uint32_t black_gui_cursor_get_x(void) { return black_cursor_x; }
uint32_t black_gui_cursor_get_y(void) { return black_cursor_y; }
