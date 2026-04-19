#include <drivers/mouse.h>
#include <arch/x86/idt.h>
#include <kernel/kernel.h>

static black_mouse_state_t black_mouse = { 0, 0, 0, 0, 0 };
static int32_t black_mouse_max_x = 1024;
static int32_t black_mouse_max_y = 768;
static uint8_t black_mouse_cycle = 0;
static int8_t  black_mouse_bytes[3];

static void black_mouse_wait_write(void) { while (black_inb(0x64) & 2); }
static void black_mouse_wait_read(void) { while (!(black_inb(0x64) & 1)); }
static void black_mouse_write(uint8_t black_val) { black_mouse_wait_write(); black_outb(0x64, 0xD4); black_mouse_wait_write(); black_outb(0x60, black_val); }

static void black_mouse_irq(black_registers_t *black_r)
{
    (void)black_r;
    uint8_t black_data = black_inb(0x60);
    switch (black_mouse_cycle) {
        case 0:
            black_mouse_bytes[0] = (int8_t)black_data;
            if (black_data & 0x08) black_mouse_cycle = 1;
            break;
        case 1:
            black_mouse_bytes[1] = (int8_t)black_data;
            black_mouse_cycle = 2;
            break;
        case 2:
            black_mouse_bytes[2] = (int8_t)black_data;
            black_mouse_cycle = 0;
            black_mouse.black_buttons = black_mouse_bytes[0] & 0x07;
            black_mouse.black_dx = black_mouse_bytes[1];
            black_mouse.black_dy = -black_mouse_bytes[2];
            black_mouse.black_x += black_mouse.black_dx;
            black_mouse.black_y += black_mouse.black_dy;
            if (black_mouse.black_x < 0) black_mouse.black_x = 0;
            if (black_mouse.black_y < 0) black_mouse.black_y = 0;
            if (black_mouse.black_x >= black_mouse_max_x) black_mouse.black_x = black_mouse_max_x - 1;
            if (black_mouse.black_y >= black_mouse_max_y) black_mouse.black_y = black_mouse_max_y - 1;
            break;
    }
}

void black_mouse_init(void)
{
    black_mouse_wait_write(); black_outb(0x64, 0xA8);
    black_mouse_wait_write(); black_outb(0x64, 0x20);
    black_mouse_wait_read();
    uint8_t black_status = black_inb(0x60) | 2;
    black_mouse_wait_write(); black_outb(0x64, 0x60);
    black_mouse_wait_write(); black_outb(0x60, black_status);
    black_mouse_write(0xF6); black_mouse_wait_read(); black_inb(0x60);
    black_mouse_write(0xF4); black_mouse_wait_read(); black_inb(0x60);
    black_idt_set_handler(44, black_mouse_irq);
}

void black_mouse_get_state(black_mouse_state_t *black_s) { *black_s = black_mouse; }
void black_mouse_set_bounds(int32_t black_w, int32_t black_h) { black_mouse_max_x = black_w; black_mouse_max_y = black_h; }
