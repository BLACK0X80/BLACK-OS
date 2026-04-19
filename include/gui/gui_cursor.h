#ifndef _BLACK_GUI_CURSOR_H
#define _BLACK_GUI_CURSOR_H
#include <stdint.h>
void black_gui_cursor_init(void);
void black_gui_cursor_move(int black_dx, int black_dy);
void black_gui_cursor_draw(void);
uint32_t black_gui_cursor_get_x(void);
uint32_t black_gui_cursor_get_y(void);
#endif
