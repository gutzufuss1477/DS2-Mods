.intel_syntax noprefix
.text
.globl bt_cord_range_hook
.extern g_helper_addr
.extern g_return_addr
.extern g_range_multiplier

.p2align 4
bt_cord_range_hook:
    vmovss xmm0, dword ptr [rax+0xD0]
    vmulss xmm0, xmm0, dword ptr [rip + g_range_multiplier]
    mov r11, qword ptr [rip + g_helper_addr]
    call r11
    vcomiss xmm6, xmm0
    mov r11, qword ptr [rip + g_return_addr]
    jmp r11
