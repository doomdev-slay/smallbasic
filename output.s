.intel_syntax noprefix
.global main
.text
main:
    .type main, @function
    # --- print "Hello, world!" ---
    mov rax, 1
    mov rdi, 1
    lea rsi, [txt0]
    mov rdx, 14
    syscall
    # --- Exit Program Gracefully ---
    mov rax, 60
    mov rdi, 0
    syscall

.section .note.GNU-stack,"",@progbits
.data
fmt_int:
    .ascii "%d\n\0"
txt0:
    .ascii "Hello, world!\n"
