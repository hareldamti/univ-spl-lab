;Defines
    CLOSE               EQU 6
    OPEN                EQU 5
    WRITE               EQU 4
    READ                EQU 3
    EXIT                EQU 1
    STDOUT              EQU 1
    STDERR              EQU 2
    STDIN               EQU 0
    O_RDONLY            EQU 0
    O_WRONLY            EQU 1
    O_RDRW              EQU 2
    O_APPEND            EQU 1024
    O_TRUNC             EQU 512
    O_CREATE            EQU 64
    S_IRUSR             EQU 00400
    S_IWUSR             EQU 00200

section .data
    msg db "Hello World",10,0
    space :      db ' '
    newline :    db 10
    input :      dd 0
    infile :     dd STDIN
    outfile :    dd STDOUT

section .text
    global _start
    global system_call
    extern strlen
    extern main

_start:
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

main:
    mov     esi,[esp+4]      ;argc
    mov     edi,[esp+8]      ;argv pointer
    
loop:   cmp     esi, 0
        jz      done
        dec     esi
        mov     ecx , edi
        push    dword ecx
        call    strlen
        add     esp , 4 ; removes the last item in stack

        pushad
        push edi
        call change_IO
        add     esp , 4
        popad

        mov     edx, eax ; length of the arg
        mov     eax , WRITE 
        mov     ebx , STDERR
        mov     ecx , edi ; pointer to next string
        int     0x80 
        add     edi, eax ; adds the length of the arg to edi
        inc     edi 
   
        ; print space
        mov     edx, 1
        mov     eax , WRITE 
        mov     ebx , STDERR
        mov     ecx , space
        int     0x80 
        jmp     loop
        
done:
    ;; print newline
    mov     edx, 1
    mov     eax , WRITE 
    mov     ebx , STDERR
    mov     ecx , newline
    int     0x80 

    call encoder
    ret                     ; Back to caller
    
encoder:
    enc_loop:
        mov     edx, 1 ; length of bytes to read
        mov     eax , READ ; read
        mov     ebx , &infile ; from STDIN
        mov     ecx , input  ; pointer to the buffer
        int     0x80            ; Transfer control to operating system
        cmp     eax,0
        jl      end_enc
        cmp     eax, 0
        jz      end_enc
        mov     esi ,&input    ; put the value read in esi
        call    AtoZCheck
        mov     edx, 1 ; length of the ' '
        mov     eax , WRITE 
        mov     ebx , &outfile
        mov     ecx , input  ; pointer to ' '
        int     0x80            
        jmp     enc_loop
    end_enc:
        ret

AtoZCheck:
            cmp     esi , 0x7A
            jg      skip_1 ; jump if esi>'z'
            cmp     esi , 0x41
            jl      skip_1 ; jump if esi<'A'
            inc     esi             ; add 1 to the value
            mov     &input ,esi    ; store the value back in input
            skip_1:
            ret

change_IO: 
            mov    edi , [esp+4]              ; the argument   
            cmp    word [edi], '-'+(256*'i')  ; equivalently "-i"
            jz     change_input
            cmp    word [edi], '-'+(256*'o')  ; equivalently "-o"
            jz     change_output
            ret

change_output:
            mov     eax , OPEN 
            mov     ebx , edi
            add     ebx , 2
            mov     ecx , O_RDRW  ;premissions to file
            or      ecx , O_CREATE
            mov     edx , S_IRUSR
            or      edx , S_IWUSR
            int     0x80  
            mov     [outfile], eax 
            ret         

change_input:
            mov     eax , OPEN 
            mov     ebx , edi
            add     ebx , 2
            mov     ecx , O_RDONLY; 
            mov     edx , S_IRUSR
            or      edx , S_IWUSR
            int     0x80  
            mov     [infile], eax
            ret         
        
