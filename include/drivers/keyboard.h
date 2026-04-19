#ifndef _BLACK_KEYBOARD_H
#define _BLACK_KEYBOARD_H

#include <stdint.h>

#define KEY_MOD_SHIFT  0x01
#define KEY_MOD_CTRL   0x02
#define KEY_MOD_ALT    0x04

typedef struct {
    uint8_t  black_scancode;
    char     black_ascii;
    uint8_t  black_modifiers;
    uint8_t  black_pressed;
    uint8_t  black_extended;
} black_key_event_t;

typedef black_key_event_t key_event_t;

void black_keyboard_init(void);
void black_keyboard_set_callback(void (*black_cb)(char));
int  black_keyboard_get_event(black_key_event_t *black_ev);
char black_keyboard_getchar(void);

#define keyboard_init         black_keyboard_init
#define keyboard_set_callback black_keyboard_set_callback
#define keyboard_get_event    black_keyboard_get_event

#endif
