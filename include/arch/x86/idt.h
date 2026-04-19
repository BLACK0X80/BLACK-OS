#ifndef _BLACK_IDT_H
#define _BLACK_IDT_H

#include <stdint.h>

typedef struct {
    uint32_t black_ds;
    uint32_t black_edi, black_esi, black_ebp, black_esp;
    uint32_t black_ebx, black_edx, black_ecx, black_eax;
    uint32_t black_int_no, black_err_code;
    uint32_t black_eip, black_cs, black_eflags, black_useresp, black_ss;
} __attribute__((packed)) black_registers_t;

typedef void (*black_isr_handler_t)(black_registers_t *);

void black_idt_init(void);
void black_idt_set_handler(uint8_t black_num, black_isr_handler_t black_handler);
void black_idt_set_apic_mode(int black_mode);
int  black_idt_is_apic_mode(void);
void black_pic_disable(void);

#define idt_init          black_idt_init
#define idt_set_handler   black_idt_set_handler
#define idt_set_apic_mode black_idt_set_apic_mode
#define idt_is_apic_mode  black_idt_is_apic_mode
#define pic_disable       black_pic_disable

#endif
