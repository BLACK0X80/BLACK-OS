#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

void black_gui_draw_rect(int black_x, int black_y, int black_w, int black_h, uint32_t black_color) {
    for (int black_j = black_y; black_j < black_y + black_h; black_j++) {
        for (int black_i = black_x; black_i < black_x + black_w; black_i++) {
            black_fb_putpixel(black_i, black_j, black_color);
        }
    }
}

void black_gui_draw_line(int black_x0, int black_y0, int black_x1, int black_y1, uint32_t black_color) {
    int black_dx = black_x1 - black_x0;
    int black_dy = black_y1 - black_y0;
    int black_steps = (black_dx > black_dy) ? black_dx : black_dy;
    if (black_steps < 0) black_steps = -black_steps;
    if (black_steps == 0) return;
    float black_xi = (float)black_dx / black_steps;
    float black_yi = (float)black_dy / black_steps;
    float black_cx = black_x0;
    float black_cy = black_y0;
    for (int black_i = 0; black_i <= black_steps; black_i++) {
        black_fb_putpixel((int)black_cx, (int)black_cy, black_color);
        black_cx += black_xi;
        black_cy += black_yi;
    }
}

void black_gui_draw_border(int black_x, int black_y, int black_w, int black_h, uint32_t black_color) {
    black_gui_draw_line(black_x, black_y, black_x + black_w, black_y, black_color);
    black_gui_draw_line(black_x, black_y + black_h, black_x + black_w, black_y + black_h, black_color);
    black_gui_draw_line(black_x, black_y, black_x, black_y + black_h, black_color);
    black_gui_draw_line(black_x + black_w, black_y, black_x + black_w, black_y + black_h, black_color);
}
