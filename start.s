section .data:
    _argc db "argc: %d",10,0
section .text

extern printf, exit, puts, strlen
global main

main:
    push ebp

    mov ebp, esp
    mov ecx, [ebp+8]            ; ecx = argc
    mov edx, [ebp+12]           
    mov edx, [edx]              ; edx = *argv
    mov ebx, 0

    pushad                      ; printf("argc: {argc}\n")
    push dword ecx
    push _argc
    call printf
    add esp, 8
    popad

    loop_args:
        cmp ebx, ecx
        jge endloop_args

        push ebx
        push ecx

        push dword edx
        call puts

        pop edx  
        add edx, eax
        pop ecx
        pop ebx


        inc ebx
        jmp loop_args

    endloop_args:
    push 0
    call exit