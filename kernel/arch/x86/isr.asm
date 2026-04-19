bits 32

section .text

global black_gdt_flush
black_gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.black_flush_done
.black_flush_done:
    ret

global black_tss_flush
black_tss_flush:
    mov ax, 0x2B
    ltr ax
    ret

global black_idt_flush
black_idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

%macro BLACK_ISR_NOERR 1
global black_isr%1
black_isr%1:
    push dword 0
    push dword %1
    jmp black_isr_common
%endmacro

%macro BLACK_ISR_ERR 1
global black_isr%1
black_isr%1:
    push dword %1
    jmp black_isr_common
%endmacro

%macro BLACK_IRQ 2
global black_irq%1
black_irq%1:
    push dword 0
    push dword %2
    jmp black_isr_common
%endmacro

BLACK_ISR_NOERR 0
BLACK_ISR_NOERR 1
BLACK_ISR_NOERR 2
BLACK_ISR_NOERR 3
BLACK_ISR_NOERR 4
BLACK_ISR_NOERR 5
BLACK_ISR_NOERR 6
BLACK_ISR_NOERR 7
BLACK_ISR_ERR   8
BLACK_ISR_NOERR 9
BLACK_ISR_ERR   10
BLACK_ISR_ERR   11
BLACK_ISR_ERR   12
BLACK_ISR_ERR   13
BLACK_ISR_ERR   14
BLACK_ISR_NOERR 15
BLACK_ISR_NOERR 16
BLACK_ISR_ERR   17
BLACK_ISR_NOERR 18
BLACK_ISR_NOERR 19

BLACK_IRQ 0,  32
BLACK_IRQ 1,  33
BLACK_IRQ 2,  34
BLACK_IRQ 3,  35
BLACK_IRQ 4,  36
BLACK_IRQ 5,  37
BLACK_IRQ 6,  38
BLACK_IRQ 7,  39
BLACK_IRQ 8,  40
BLACK_IRQ 9,  41
BLACK_IRQ 10, 42
BLACK_IRQ 11, 43
BLACK_IRQ 12, 44
BLACK_IRQ 13, 45
BLACK_IRQ 14, 46
BLACK_IRQ 15, 47

extern black_isr_dispatch

black_isr_common:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call black_isr_dispatch
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    iret

global black_context_switch
black_context_switch:
    mov eax, [esp+4]
    mov edx, [esp+8]
    push ebx
    push esi
    push edi
    push ebp
    mov [eax], esp
    mov esp, edx
    pop ebp
    pop edi
    pop esi
    pop ebx
    ret
