#ifndef _BLACK_MULTIBOOT_H
#define _BLACK_MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_BOOTLOADER_MAGIC  0x2BADB002

#define MULTIBOOT_INFO_MEMORY       0x00000001
#define MULTIBOOT_INFO_BOOTDEV      0x00000002
#define MULTIBOOT_INFO_CMDLINE      0x00000004
#define MULTIBOOT_INFO_MODS         0x00000008
#define MULTIBOOT_INFO_MEM_MAP      0x00000040
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

typedef struct {
    uint32_t black_flags;
    uint32_t black_mem_lower;
    uint32_t black_mem_upper;
    uint32_t black_boot_device;
    uint32_t black_cmdline;
    uint32_t black_mods_count;
    uint32_t black_mods_addr;
    uint32_t black_syms[4];
    uint32_t black_mmap_length;
    uint32_t black_mmap_addr;
    uint32_t black_drives_length;
    uint32_t black_drives_addr;
    uint32_t black_config_table;
    uint32_t black_boot_loader_name;
    uint32_t black_apm_table;
    uint32_t black_vbe_control_info;
    uint32_t black_vbe_mode_info;
    uint16_t black_vbe_mode;
    uint16_t black_vbe_interface_seg;
    uint16_t black_vbe_interface_off;
    uint16_t black_vbe_interface_len;
    uint64_t black_framebuffer_addr;
    uint32_t black_framebuffer_pitch;
    uint32_t black_framebuffer_width;
    uint32_t black_framebuffer_height;
    uint8_t  black_framebuffer_bpp;
    uint8_t  black_framebuffer_type;
    uint8_t  black_color_info[6];
} __attribute__((packed)) black_multiboot_info_t;

typedef struct {
    uint32_t black_size;
    uint64_t black_addr;
    uint64_t black_len;
    uint32_t black_type;
} __attribute__((packed)) black_mmap_entry_t;

#define BLACK_MMAP_AVAILABLE  1
#define BLACK_MMAP_RESERVED   2
#define BLACK_MMAP_ACPI       3
#define BLACK_MMAP_NVS        4

#endif
