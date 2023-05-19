; DEFINES
    SEED EQU 0x4d33

section .data:
    argc_format db "argc: %d",10,0
    loop_multi_format db "%02hhx",0

    address_format db "address: %x",10, 0

    x_struct: db 5
    x_num: db 0xaa, 1,2,0x44,0x4f
    y_struct: db 6
    y_num: db 0xaa, 1,2,3,0x44,0x4f

    MASK dw 0xb400

    linefeed: db 10,0
    
section .bss
   input resb 600
   STATE resd 1
   

section .text

extern printf, exit, puts, strlen, stdin, fgets, strtol, malloc
global main

PRmulti:
    push ebp
    mov ebp, esp
    sub esp, 8

    call rand_num
    and eax, 0xfe
    inc eax
    push eax
    call malloc
    pop ebx
    mov dword [ebp-4], eax
    dec ebx
    mov byte [eax], bl

    mov bh, 0

    loop_PRmulti:
        push ebx
        call rand_num
        pop ebx

        mov ecx, 0
        mov cl, bh
        add ecx, [ebp-4]
        inc ecx

        mov byte [ecx], al

        inc bh
        cmp byte bl, bh
        jl loop_PRmulti

    mov eax, [ebp-4]

    add esp, 8
    pop ebp
    ret

rand_num:
    push ebp
    mov ebp, esp
    sub esp, 4

    mov eax, 0
    mov edx, 0
    mov edi, STATE

    loop_rand_num:
        shl al, 1
        mov word cx, [STATE]
        and word cx, [MASK]
        popcnt cx, cx
        and cx, 0x0001
        cmp cx, 0x0001
        je rand_parity_1

        mov bx, [STATE]
        shr bx, 1
        and bx, 0x7fff
        mov word [edi], bx
        and byte al, 0xfe

        jmp end_rand_parity
        rand_parity_1:

        mov bx, [STATE]
        shr bx, 1
        or bx, 0x8000
        mov word [edi], bx
        or byte al, 0x01

        end_rand_parity:
        
        inc dl
        cmp byte dl, 8
        jl loop_rand_num
        

    add esp, 4
    pop ebp
    ret

parse_hex:
    add esi, input
    shl ebx, 1
    add esi, ebx
    shr ebx, 1
    cmp byte [esi], 0x30
    jl end_parse_hex
    cmp byte [esi], 0x39
    jg capital_hex
    sub byte [esi], 0x30
    mov byte dl, [esi]


    capital_hex:
    cmp byte [esi], 0x41
    jl end_parse_hex
    cmp byte [esi], 0x46
    jg lower_hex
    sub byte [esi], 0x37
    mov byte dl, [esi]


    lower_hex: 
    cmp byte [esi], 0x61
    jl end_parse_hex
    cmp byte [esi], 0x66
    jg end_parse_hex
    sub byte [esi], 0x57
    mov byte dl, [esi]


    end_parse_hex:
    ret

add_multi:
    push ebp
    mov ebp, esp
    sub esp, 4

    mov edi, [ebp+8]            ;; edi = min(q, p)
    mov ebx, [ebp+12]           ;; ebx = max(q, p)
    mov byte dh, [edi]          ;; cl = 1 + max(p.len, q.len)
    mov byte dl, [ebx]
    cmp byte dh, dl
    jg add_multi_find_max
    mov edi, [ebp+12]
    mov ebx, [ebp+8]
    add_multi_find_max:
    mov ecx, 1
    add byte cl, [ebx]

    push edi                    ;; esi = malloc(ecx)
    push ebx
    push ecx
    call malloc
    pop ecx
    pop ebx
    pop edi
    mov esi, eax

    mov byte [esi], cl
    dec cl
    mov ch, 0
    mov edx, 0
    loop_add_multi:
        cmp ch, cl
        jge end_loop_add_multi

        cmp byte ch, [edi]          ;; smaller number comparison
        jg add_one

        mov eax, 1                  ;; add smaller number
        add byte al, ch
        add eax, edi
        add dl, [eax]
        add_one:

        mov eax, 1
        add byte al, ch
        add eax, ebx
        add dl, [eax]
        mov dh, 1
        jnc no_carry
        jmp add_carry
        no_carry:
        dec dh
        add_carry:
        mov eax, 1
        add byte al, ch
        add eax, esi
        
        mov byte [eax], dl
        mov byte dl, dh

        inc ch
        jmp loop_add_multi
    end_loop_add_multi:

    mov eax, 1
    add byte al, ch
    add eax, esi
        
    mov byte [eax], dl
    mov eax, esi

    add esp, 4
    pop ebp
    ret

get_multi:
    push ebp
    mov ebp, esp
    sub esp, 4

    push dword [stdin]
    push 600
    push input
    call fgets
    add esp, 12
    
    push eax
    call strlen
    add esp, 4
    sub eax, 1
    mov ecx, eax

    and byte al, 0x01
    cmp byte al, 0
    je asserted_even

    mov ebx, input
    add ebx, ecx
    sub ebx, 1
    inc ecx
    mov byte al, [ebx]
    mov byte [ebx+1], '0'

    asserted_even:

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;   input.len %2 == 0;    ecx = input.len

    shr ecx, 1
    mov eax, ecx            ; eax = malloc(sizeof(multi(input.len)))
    add eax, 2
    push ecx
    push eax
    call malloc         
    add esp, 4
    pop ecx

    mov byte [eax], cl

    mov ebx, 0
    loop_get_multi:
        cmp ebx, ecx
        jge end_loop_get_multi
        
        mov esi, 0
        mov edi, eax
        add edi, ecx
        sub edi, ebx


        call parse_hex 
        mov byte [edi], dl
        shl byte [edi], 4

        mov esi, 1
        call parse_hex
        add byte [edi], dl

        add ebx, 1
        jmp loop_get_multi

    end_loop_get_multi:
    
    ;;push eax
    ;;call print_multi
    ;;add esp, 4

    add esp, 4
    pop ebp
    ret

print_multi:
    push ebp
    mov ebp, esp
    sub esp, 4
    pushad

    mov eax, [ebp+8]

    mov ecx, 0
    mov cl, [eax]           ; ecx = p->size
    mov ch, cl
    dec ch

    loop_print_multi:
        cmp byte ch, 0x00
        jl end_print_multi

        push ecx

        mov ebx, 0
        mov bl, ch

        mov eax, [ebp+8]    ; eax = p
        add eax, ebx
        inc eax             ; eax = &p->num[ch]

        mov edx, 0
        mov byte dl, [eax]  ; dl = p->num[ch]
        push dword edx
        push loop_multi_format
        call printf

        add esp, 8

        pop ecx
        dec ch
        jmp loop_print_multi

    end_print_multi:

    push linefeed           ; printf("\n")
    call printf
    pop eax

    popad
    add esp, 4
    pop ebp
    ret

main:
    push ebp
    mov ebp, esp
    mov ecx, [ebp+8]            ; ecx = argc
    mov edx, [ebp+12]           
    mov edx, [edx]              ; edx = *argv
    mov ebx, 0

    mov edi, STATE
    mov word [edi], SEED

    pushad
    push dword ecx              ; printf("argc: {argc}\n")
    push argc_format
    call printf
    add esp, 8
    popad

    loop_args:
        push ebx            ; partial pushad
        push ecx

        push dword edx
        call puts

        pop edx             ; partial popad 
        add edx, eax        ; edx += len(argv[ebx])
        pop ecx
        pop ebx

        inc ebx             ; ebx++
        cmp ebx, ecx        ; while ( ebx < ecx )
        jl loop_args

    call PRmulti
    push eax
    call print_multi
    pop eax
    push 0
    call exit

    call get_multi
    push eax
    call print_multi
    call get_multi
    push eax
    call print_multi

    call add_multi
    add esp, 8
    push eax
    call print_multi
    pop eax

    push 0
    call exit