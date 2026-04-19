[bits 16]
section .text
global black_ap_trampoline

black_ap_trampoline:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    lgdt [black_ap_gdt_ptr - black_ap_trampoline + 0x8000]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:(black_ap_protected - black_ap_trampoline + 0x8000)

[bits 32]
black_ap_protected:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, [black_ap_stack - black_ap_trampoline + 0x8000]
    mov eax, [black_ap_entry - black_ap_trampoline + 0x8000]
    call eax

    cli
    hlt

align 8
black_ap_gdt:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF

black_ap_gdt_ptr:
    dw 23
    dd black_ap_gdt - black_ap_trampoline + 0x8000

black_ap_stack:
    dd 0

black_ap_entry:
    dd 0
