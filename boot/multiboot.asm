bits 32

BLACK_MULTIBOOT_MAGIC     equ 0x1BADB002
BLACK_MULTIBOOT_ALIGN     equ 1 << 0
BLACK_MULTIBOOT_MEMINFO   equ 1 << 1
BLACK_MULTIBOOT_VIDEO     equ 1 << 2
BLACK_MULTIBOOT_FLAGS     equ BLACK_MULTIBOOT_ALIGN | BLACK_MULTIBOOT_MEMINFO | BLACK_MULTIBOOT_VIDEO
BLACK_MULTIBOOT_CHECKSUM  equ -(BLACK_MULTIBOOT_MAGIC + BLACK_MULTIBOOT_FLAGS)

BLACK_KERNEL_VMA          equ 0xC0000000
BLACK_KERNEL_PAGE_INDEX   equ (BLACK_KERNEL_VMA >> 22)

global black_boot_page_directory

section .multiboot
align 4
    dd BLACK_MULTIBOOT_MAGIC
    dd BLACK_MULTIBOOT_FLAGS
    dd BLACK_MULTIBOOT_CHECKSUM
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 1024
    dd 768
    dd 32

section .boot.bss nobits alloc noexec write
align 16
black_boot_stack_bottom:
    resb 32768
black_boot_stack_top:

section .boot.bss nobits alloc noexec write
align 4096

black_boot_page_directory:
    resb 4096

black_boot_page_table_identity:
    resb 4096

black_boot_page_table_kernel:
    resb 4096

black_boot_page_table_kernel2:
    resb 4096

section .boot
global _start

_start:
    cli

    mov [black_multiboot_info_ptr], ebx
    mov [black_multiboot_magic], eax
    mov esp, black_boot_stack_top

    mov ecx, 1024
    xor eax, eax
    mov edi, black_boot_page_directory
    rep stosd

    mov edi, black_boot_page_table_identity
    mov eax, 0x00000003
    mov ecx, 1024

.black_fill_identity:
    mov [edi], eax
    add eax, 4096
    add edi, 4
    loop .black_fill_identity

    mov edi, black_boot_page_table_kernel
    mov eax, 0x00000003
    mov ecx, 1024

.black_fill_kernel:
    mov [edi], eax
    add eax, 4096
    add edi, 4
    loop .black_fill_kernel

    mov edi, black_boot_page_table_kernel2
    mov eax, 0x00400003
    mov ecx, 1024

.black_fill_kernel2:
    mov [edi], eax
    add eax, 4096
    add edi, 4
    loop .black_fill_kernel2

    mov eax, black_boot_page_table_identity
    or eax, 0x03
    mov [black_boot_page_directory + 0*4], eax

    mov eax, black_boot_page_table_kernel
    or eax, 0x03
    mov [black_boot_page_directory + BLACK_KERNEL_PAGE_INDEX*4], eax

    mov eax, black_boot_page_table_kernel2
    or eax, 0x03
    mov [black_boot_page_directory + (BLACK_KERNEL_PAGE_INDEX+1)*4], eax

    mov eax, black_boot_page_directory
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    mov eax, black_higher_half
    jmp eax

section .text
black_higher_half:
    mov dword [black_boot_page_directory + BLACK_KERNEL_VMA], 0
    mov eax, cr3
    mov cr3, eax

    mov esp, black_kernel_stack_top

    push dword [black_multiboot_magic + BLACK_KERNEL_VMA]
    push dword [black_multiboot_info_ptr + BLACK_KERNEL_VMA]

    extern black_kernel_main
    call black_kernel_main

.black_halt:
    cli
    hlt
    jmp .black_halt

section .bss
align 16
black_kernel_stack_bottom:
    resb 131072
global black_kernel_stack_top
black_kernel_stack_top:

section .boot
black_multiboot_info_ptr:
    dd 0
black_multiboot_magic:
    dd 0
