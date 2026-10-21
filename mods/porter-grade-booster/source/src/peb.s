.text
.globl get_peb
.p2align 4, 0x90
get_peb:
    movq %gs:0x60, %rax
    retq
