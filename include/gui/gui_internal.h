#ifndef _BLACK_GUI_INTERNAL_H
#define _BLACK_GUI_INTERNAL_H
void black_gui_draw_rect(int black_x, int black_y, int black_w, int black_h, uint32_t black_color);
void black_gui_draw_line(int black_x0, int black_y0, int black_x1, int black_y1, uint32_t black_color);
void black_gui_draw_border(int black_x, int black_y, int black_w, int black_h, uint32_t black_color);
void black_gui_draw_icon_folder(int black_x, int black_y, uint32_t black_color);
void black_gui_draw_icon_file(int black_x, int black_y, uint32_t black_color);
void black_gui_terminal_init(void);
void black_gui_terminal_putchar(char black_ch);
void black_gui_terminal_render(int black_ox, int black_oy);
void black_gui_editor_init(void);
void black_gui_filemanager_init(void);
#endif
