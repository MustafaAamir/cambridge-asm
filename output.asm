section .data
    format_int db "%ld", 10, 0
    format_char db "%c", 0
    memory: times 1024 dq 0  ; 1024 64-bit memory locations
    acc:    dq 0  ; Accumulator
    ix:     dq 0  ; Index Register
    flag:   dq 0  ; Comparison Flag

section .text
    extern printf
    extern scanf
    global main

; Macro for printing integer (debug)
%macro print_int 1
    mov rdi, format_int
    mov rsi, %1
    xor rax, rax
    call printf
%endmacro
; Macro for printing character
%macro print_char 1
    mov rdi, format_char
    mov rsi, %1
    xor rax, rax
    call printf
%endmacro
main:
    mov qword [acc], 10
    mov rax, [acc]
    mov [memory + 255*8], rax
    mov qword [acc], 255
    mov rax, [acc]
    mov [memory + 175*8], rax
    mov rbx, [memory + 175*8]
    mov rax, [memory + rbx*8]
    mov [acc], rax
x:
    dec qword [acc]
    print_int [acc]
    ; State inspection placeholder
    cmp qword [acc], 0
    jne x

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
