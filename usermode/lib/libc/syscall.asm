global black_syscall0
global black_syscall1
global black_syscall2
global black_syscall3

section .text

black_syscall0:
    push ebx
    mov eax, [esp+8]
    int 0x80
    pop ebx
    ret

black_syscall1:
    push ebx
    mov eax, [esp+8]
    mov ebx, [esp+12]
    int 0x80
    pop ebx
    ret

black_syscall2:
    push ebx
    mov eax, [esp+8]
    mov ebx, [esp+12]
    mov ecx, [esp+16]
    int 0x80
    pop ebx
    ret

black_syscall3:
    push ebx
    mov eax, [esp+8]
    mov ebx, [esp+12]
    mov ecx, [esp+16]
    mov edx, [esp+20]
    int 0x80
    pop ebx
    ret
