[bits 32]
section .text
extern main
global _start

_start:
    ; Pop argc and argv
    pop eax     ; argc
    mov ebx, esp ; argv
    
    push ebx
    push eax
    call main
    
    ; Exit syscall (1)
    mov ebx, eax ; status
    mov eax, 1   ; sys_exit
    int 0x80
    
    hlt
