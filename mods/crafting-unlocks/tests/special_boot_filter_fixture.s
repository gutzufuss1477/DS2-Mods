.intel_syntax noprefix
.text
.extern SpecialBootUsageThunk
.globl TestSpecialBootFilter
TestSpecialBootFilter:
    mov al, dl
    test al, al
    call SpecialBootUsageThunk
.normal_return:
    mov eax, 1
    ret
    .space 0xAD, 0x90
.skip_return:
    xor eax, eax
    ret
