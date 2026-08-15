.intel_syntax noprefix
.text
# Release v1.0.0
# All hooked instructions are MOV/MOVZX and therefore do not modify flags.
# Each stub restores incoming RFLAGS exactly. Only the original lookup register
# (or ECX for the item-cap read) is intentionally virtualized.

.macro RETURN_PTR name
.p2align 3
\name\()ReturnPtr: .quad 0
.endm

.globl SKeyAStubStart
.globl SKeyAStubEnd
.globl SKeyAReturnPtr
SKeyAStubStart:
    mov r12, qword ptr [rsp+0x20]
    pushfq
    cmp r12, 160
    jae .sA_done
.sA_fold:
    cmp r12, 6
    jb .sA_done
    sub r12, 6
    jmp .sA_fold
.sA_done:
    popfq
    jmp qword ptr [rip+SKeyAReturnPtr]
RETURN_PTR SKeyA
SKeyAStubEnd:

.globl SKeyBStubStart
.globl SKeyBStubEnd
.globl SKeyBReturnPtr
SKeyBStubStart:
    mov r12, qword ptr [rsp+0x30]
    pushfq
    cmp r12, 160
    jae .sB_done
.sB_fold:
    cmp r12, 6
    jb .sB_done
    sub r12, 6
    jmp .sB_fold
.sB_done:
    popfq
    jmp qword ptr [rip+SKeyBReturnPtr]
RETURN_PTR SKeyB
SKeyBStubEnd:

.globl MKeyAStubStart
.globl MKeyAStubEnd
.globl MKeyAReturnPtr
MKeyAStubStart:
    mov r8, qword ptr [rsp+0x28]
    pushfq
    cmp r8, 160
    jae .mA_done
.mA_fold:
    cmp r8, 3
    jb .mA_done
    sub r8, 3
    jmp .mA_fold
.mA_done:
    popfq
    jmp qword ptr [rip+MKeyAReturnPtr]
RETURN_PTR MKeyA
MKeyAStubEnd:

.globl MKeyBStubStart
.globl MKeyBStubEnd
.globl MKeyBReturnPtr
MKeyBStubStart:
    mov r8, qword ptr [rsp+0x38]
    pushfq
    cmp r8, 160
    jae .mB_done
.mB_fold:
    cmp r8, 3
    jb .mB_done
    sub r8, 3
    jmp .mB_fold
.mB_done:
    popfq
    jmp qword ptr [rip+MKeyBReturnPtr]
RETURN_PTR MKeyB
MKeyBStubEnd:

.globl LKeyAStubStart
.globl LKeyAStubEnd
.globl LKeyAReturnPtr
LKeyAStubStart:
    mov r12, qword ptr [rsp+0x40]
    pushfq
    cmp r12, 160
    jae .lA_done
.lA_fold:
    cmp r12, 2
    jb .lA_done
    sub r12, 2
    jmp .lA_fold
.lA_done:
    popfq
    jmp qword ptr [rip+LKeyAReturnPtr]
RETURN_PTR LKeyA
LKeyAStubEnd:

.globl LKeyBStubStart
.globl LKeyBStubEnd
.globl LKeyBReturnPtr
LKeyBStubStart:
    mov r13, qword ptr [rsp+0x50]
    pushfq
    cmp r13, 160
    jae .lB_done
.lB_fold:
    cmp r13, 2
    jb .lB_done
    sub r13, 2
    jmp .lB_fold
.lB_done:
    popfq
    jmp qword ptr [rip+LKeyBReturnPtr]
RETURN_PTR LKeyB
LKeyBStubEnd:

.globl XLKeyAStubStart
.globl XLKeyAStubEnd
.globl XLKeyAReturnPtr
XLKeyAStubStart:
    mov r12, qword ptr [rsp+0x48]
    pushfq
    cmp r12, 160
    jae .xlA_done
    test r12, r12
    jz .xlA_done
    xor r12, r12
.xlA_done:
    popfq
    jmp qword ptr [rip+XLKeyAReturnPtr]
RETURN_PTR XLKeyA
XLKeyAStubEnd:

.globl XLKeyBStubStart
.globl XLKeyBStubEnd
.globl XLKeyBReturnPtr
XLKeyBStubStart:
    mov r12, qword ptr [rsp+0x58]
    pushfq
    cmp r12, 160
    jae .xlB_done
    test r12, r12
    jz .xlB_done
    xor r12, r12
.xlB_done:
    popfq
    jmp qword ptr [rip+XLKeyBReturnPtr]
RETURN_PTR XLKeyB
XLKeyBStubEnd:

.globl ItemCapStubStart
.globl ItemCapStubEnd
.globl ItemCapReturnPtr
ItemCapStubStart:
    movzx ecx, word ptr [r15+0x26]
    pushfq
    cmp ecx, 13
    jne .item_done
    mov ecx, 320
.item_done:
    popfq
    jmp qword ptr [rip+ItemCapReturnPtr]
RETURN_PTR ItemCap
ItemCapStubEnd:
