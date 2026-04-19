#ifndef _BLACK_MOUSE_H
#define _BLACK_MOUSE_H

#include <stdint.h>

typedef struct {
    int32_t black_x;
    int32_t black_y;
    int32_t black_dx;
    int32_t black_dy;
    uint8_t black_buttons;
} black_mouse_state_t;

typedef black_mouse_state_t mouse_state_t;

void black_mouse_init(void);
void black_mouse_get_state(black_mouse_state_t *black_state);
void black_mouse_set_bounds(int32_t black_w, int32_t black_h);

#define mouse_init      black_mouse_init
#define mouse_get_state black_mouse_get_state
#define mouse_set_bounds black_mouse_set_bounds

#endif
