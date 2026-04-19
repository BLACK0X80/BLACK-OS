#ifndef _BLACK_GDT_H
#define _BLACK_GDT_H

#include <stdint.h>

void black_gdt_init(void);
void black_gdt_set_kernel_stack(uint32_t black_stack);
void black_gdt_set_kernel_stack_cpu(uint32_t black_cpu_id, uint32_t black_stack);

#define gdt_init                 black_gdt_init
#define gdt_set_kernel_stack     black_gdt_set_kernel_stack
#define gdt_set_kernel_stack_cpu black_gdt_set_kernel_stack_cpu

#endif
