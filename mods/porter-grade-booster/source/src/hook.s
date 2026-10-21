.text
.globl HookEntry
.extern g_deliveryReturn
.extern g_enabledMask
.extern g_multiplier
.extern g_half
.extern g_extraPoints
.extern g_mode
.extern g_trampoline
.p2align 4, 0x90
HookEntry:
    movq (%rsp), %rax
    cmpq g_deliveryReturn(%rip), %rax
    jne .Lcontinue

    movzwl %dx, %eax
    cmpl $4, %eax
    ja .Lcontinue

    testq %r8, %r8
    je .Lcontinue

    movl g_enabledMask(%rip), %ecx
    btl %eax, %ecx
    jnc .Lcontinue

    movl g_mode(%rip), %ecx
    cmpl $1, %ecx
    je .Ladditive

    movq %r8, %rax
    cvtsi2sd %rax, %xmm0
    mulsd g_multiplier(%rip), %xmm0
    addsd g_half(%rip), %xmm0
    cvttsd2si %xmm0, %r8

    cmpl $2, %ecx
    jne .Lclamp

.Ladditive:
    movq g_extraPoints(%rip), %rax
    addq %rax, %r8
    jc .Lcap

.Lclamp:
    testq %r9, %r9
    je .Lcontinue
    cmpq %r9, %r8
    jbe .Lcontinue
.Lcap:
    movq %r9, %r8

.Lcontinue:
    jmp *g_trampoline(%rip)
