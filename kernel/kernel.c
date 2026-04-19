#include <kernel/kernel.h>
#include <kernel/multiboot.h>
#include <drivers/vga.h>
#include <drivers/serial.h>
#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <drivers/pit.h>
#include <kernel/shell.h>
#include <kernel/scheduler.h>
#include <kernel/smp.h>
#include <security/security.h>
#include <drivers/pci.h>
#include <drivers/e1000.h>
#include <gui/desktop.h>
#include <acpi/acpi.h>
#include <apic/lapic.h>
#include <apic/ioapic.h>
#include <isolation/isolation.h>
#include <fs/vfs.h>
#include <fs/ramfs.h>
#include <fs/devfs.h>
#include <fs/procfs.h>
#include <syscall/syscall.h>
#include <string.h>

#define BLACK_HEAP_START  0xD0000000
#define BLACK_HEAP_SIZE   0x02000000

static void black_boot_delay(void)
{
    
    for (volatile int black_i = 0; black_i < 10000; black_i++);
}

void black_kernel_main(black_multiboot_info_t *black_mboot_info, uint32_t black_magic)
{
    black_vga_init();
    black_vga_clear();
    black_serial_init();

    black_serial_puts("\n[BLACK] BlackOS PHANTOM starting...\n");

    if (black_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        black_vga_puts("  FATAL: Invalid multiboot magic!\n");
        goto black_halt;
    }

    black_multiboot_info_t *black_mboot_virt =
        (black_multiboot_info_t *)((uint32_t)black_mboot_info + KERNEL_VMA);

    
    black_gdt_init();
    black_idt_init();
    black_pmm_init(black_mboot_virt);
    black_vmm_init();
    black_heap_init(BLACK_HEAP_START, BLACK_HEAP_SIZE);
    
    char *black_test = kmalloc(128); 
    if (black_test) kfree(black_test);

    
    black_pit_init(100);

    
    black_gui_init(black_mboot_virt);

    black_gui_update_boot_progress(10, "Core Memory Subsystems initialized...");
    black_boot_delay();

    black_gui_update_boot_progress(30, "Scanning hardware via ACPI and PCI...");
    int black_acpi_result = black_acpi_init();
    (void)black_acpi_result;
    black_pci_init();
    black_boot_delay();

    black_gui_update_boot_progress(45, "Enabling User Input (Keyboard & Mouse)...");
    black_keyboard_init();
    black_mouse_init();
    black_boot_delay();

    black_gui_update_boot_progress(60, "Loading Virtual File System and RamFS...");
    black_vfs_init();
    black_vfs_set_root(black_ramfs_init());
    black_devfs_init(black_vfs_get_root());
    black_procfs_init(black_vfs_get_root());
    
    black_vfs_mkdir("/System");
    black_vfs_mkdir("/System/Config");
    black_vfs_mkdir("/Users");
    black_vfs_mkdir("/Logs");
    black_boot_delay();

    black_gui_update_boot_progress(75, "Standing up Syscall Dispatcher and Scheduler...");
    black_syscall_init();
    black_smp_init();
    black_scheduler_init();
    black_boot_delay();

    black_gui_update_boot_progress(90, "Initializing Shell and Core Security...");
    black_security_init();
    black_isolation_init();
    black_sti();
    black_shell_init();
    black_boot_delay();

    black_gui_update_boot_progress(100, "Boot Complete. Starting BlackOS Workspace...");
    
    
    uint64_t black_start_ms = black_pit_get_uptime_ms();
    while (black_pit_get_uptime_ms() - black_start_ms < 4000) {
        __asm__ volatile ("hlt");
    }

    black_gui_create_window("Terminal", 100, 100, 600, 400);
    black_scheduler_enable();
    black_shell_run();

black_halt:
    black_vga_puts("\n  System halted.\n");
    black_serial_puts("[BLACK] Halted\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
