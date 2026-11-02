# The single retained native prologue is an ABI fact, not generated code.
# Revalidated in research/export/BE0270.txt, Steam 1.10.89.0.
.intel_syntax noprefix
.set @feat.00, 0x800
.text
.p2align 4
.globl ApasNativeConstructor
.def ApasNativeConstructor; .scl 2; .type 32; .endef
.seh_proc ApasNativeConstructor
ApasNativeConstructor:
    mov qword ptr [rsp+0x10],rdx
    push rbx
    .seh_pushreg rbx
    push rbp
    .seh_pushreg rbp
    push rsi
    .seh_pushreg rsi
    push r12
    .seh_pushreg r12
    push r13
    .seh_pushreg r13
    push r15
    .seh_pushreg r15
    .seh_endprologue
    mov r11,qword ptr [rip+ApasResume]
    jmp r11
.seh_endproc
.section .gfids$y,"dr"
.symidx ApasNativeConstructor
