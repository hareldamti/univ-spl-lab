;Defines
    CLOSE               EQU 6
    OPEN                EQU 5
    WRITE               EQU 4
    READ                EQU 3
    EXIT                EQU 1
    STDOUT              EQU 1
    STDERR              EQU 2
    STDIN               EQU 0
    EXIT_SUCCESS        EQU 0
    READ_COUNT          EQU 1 
    WRITE_COUNT         EQU 1 
    ASCII_A             EQU 0x41
    ASCII_Z             EQU 0x5A
    ASCII_a             EQU 0x61
    ASCII_z             EQU 0x7A
    O_RDWR              EQU 2
    O_APPEND            EQU 1024
    O_CREAT             EQU 00000100
    S_IRUSR             EQU 00400
    S_IWUSR             EQU 00200

global _start
global system_call
global strlen
global infection
global infector
extern main

section .rodata
    infection_print   : db "Hello, Infected File", 10, 0
    print_open_error  : db "Error in open file", 10, 0
    print_close_error : db "Error in close file", 10, 0

section .text

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* O_APPENDargv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
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


strlen:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state
    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ecx, 1          ; Next argument...
cont:
    cmp     byte [eax] , 0
    jz      done
    inc     ecx
    inc     eax
    jmp     cont
done:
    mov     [ebp-4] , ecx
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller


code_start:

infection:
    pushad
    mov     ecx , infection_print
    push    ecx
    call    strlen
    add     esp , 4
    mov     edx , eax
    mov     ebx , STDOUT
    mov     eax , WRITE
    int     0x80
    end:
        popad
        ret

infector:
    mov     eax , OPEN
    mov     ebx , [esp+4]
    mov     ecx , O_APPEND
    or      ecx , O_RDWR
    mov     edx , S_IRUSR
    or      edx , S_IWUSR
    int     0x80                        ;;open the File

    cmp     eax , 0
    jle     error_open

    push    eax
    mov     ebx , eax                   ;;file descriptor
    mov     eax , WRITE
    mov     ecx , code_start
    mov     edx , code_end
    sub     edx , ecx
    int     0x80                        ;;WRITE sys_call
    mov     eax , CLOSE
    pop     ebx
    int     0x80                        ;;close the file
    cmp     eax , 0
    jnz     error_close

    ret

error_open:
    push    print_open_error
    call    strlen
    add     esp , 4
    mov     edx , eax
    mov     ecx , print_open_error
    mov     ebx , STDOUT
    mov     eax , WRITE
    int     0x80
    ret

error_close:
    push    print_close_error
    call    strlen
    add     esp , 4
    mov     edx , eax
    mov     ecx , print_close_error
    mov     ebx , STDOUT
    mov     eax , WRITE
    int     0x80
    ret

code_end:


