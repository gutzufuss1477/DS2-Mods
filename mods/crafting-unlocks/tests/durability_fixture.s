.intel_syntax noprefix
.text
.extern DurabilityContentsThunk
.extern DurabilityBootsWearThunk

.globl InvokeDurabilityContents
.seh_proc InvokeDurabilityContents
InvokeDurabilityContents:
    push rdi
    .seh_pushreg rdi
    sub rsp, 0x40
    .seh_stackalloc 0x40
    movdqu xmmword ptr [rsp + 0x20], xmm6
    .seh_savexmm xmm6, 0x20
    movdqu xmmword ptr [rsp + 0x30], xmm13
    .seh_savexmm xmm13, 0x30
    .seh_endprologue
    mov rdi, rcx
    vmovd xmm6, edx
    vmovd xmm1, r8d
    vxorps xmm13, xmm13, xmm13
    call DurabilityContentsThunk
    mov eax, dword ptr [rdi + 0x88]
    movdqu xmm6, xmmword ptr [rsp + 0x20]
    movdqu xmm13, xmmword ptr [rsp + 0x30]
    add rsp, 0x40
    pop rdi
    ret
.seh_endproc

.globl CaptureBootCmpFlags
.seh_proc CaptureBootCmpFlags
CaptureBootCmpFlags:
    .seh_endprologue
    cmp ecx, 0x13579BDF
    pushfq
    pop rax
    ret
.seh_endproc

.globl InvokeDurabilityBoots
.seh_proc InvokeDurabilityBoots
InvokeDurabilityBoots:
    push rbx
    .seh_pushreg rbx
    push rbp
    .seh_pushreg rbp
    sub rsp, 0x38
    .seh_stackalloc 0x38
    movdqu xmmword ptr [rsp + 0x20], xmm6
    .seh_savexmm xmm6, 0x20
    .seh_endprologue
    mov rbx, rcx
    mov rbp, rdx
    cmp r8d, 0x13579BDF
    call DurabilityBootsWearThunk
    pushfq
    pop r9
    vmovd eax, xmm6
    shl r9, 32
    or rax, r9
    movdqu xmm6, xmmword ptr [rsp + 0x20]
    add rsp, 0x38
    pop rbp
    pop rbx
    ret
.seh_endproc
