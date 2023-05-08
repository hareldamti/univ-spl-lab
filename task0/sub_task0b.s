;Defines
    WRITE               EQU 4
    EXIT                EQU 1
    STDOUT              EQU 1
    EXIT_SUCCESS        EQU 0

global _start

section .rodata
    toPrint : db "Hello World", 10, 0

section .text
_start:
    mov edx , 0
    mov ecx , toPrint
    
    loop:                       ;; count the chars in the string
        mov bl , [ecx]
        inc ecx
        inc edx
        cmp byte bl, 0
        jnz loop
    
    mov ecx , toPrint
    mov ebx , STDOUT
    mov eax , WRITE
    int 0x80
    
    end:
        mov ebx , EXIT_SUCCESS
        mov eax , EXIT
        int 0x80