#ifndef _BLACK_FRAMEBUFFER_H
#define _BLACK_FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint32_t *black_addr;
    uint32_t *black_backbuffer;
    uint32_t black_width;
    uint32_t black_height;
    uint32_t black_pitch;
    uint8_t  black_bpp;
} black_framebuffer_t;

void black_fb_init(uint32_t black_addr, uint32_t black_w, uint32_t black_h, uint32_t black_pitch, uint8_t black_bpp);
void black_fb_putpixel(int black_x, int black_y, uint32_t black_color);
void black_fb_fillrect(int black_x, int black_y, int black_w, int black_h, uint32_t black_color);
void black_fb_drawchar(int black_x, int black_y, char black_c, uint32_t black_fg, uint32_t black_bg);
void black_fb_drawstring(int black_x, int black_y, const char *black_s, uint32_t black_fg, uint32_t black_bg);
void black_fb_blit(int black_dx, int black_dy, int black_w, int black_h, uint32_t *black_src);
void black_fb_scroll_up(int black_lines, uint32_t black_bg);
void black_fb_swap(void);
void black_fb_clear(uint32_t black_color);
black_framebuffer_t *black_fb_get(void);
uint32_t black_fb_rgb(uint8_t black_r, uint8_t black_g, uint8_t black_b);

#define fb_init       black_fb_init
#define fb_putpixel   black_fb_putpixel
#define fb_fillrect   black_fb_fillrect
#define fb_drawchar   black_fb_drawchar
#define fb_drawstring black_fb_drawstring
#define fb_swap       black_fb_swap
#define fb_clear      black_fb_clear
#define fb_get        black_fb_get
#define fb_rgb        black_fb_rgb

#endif
