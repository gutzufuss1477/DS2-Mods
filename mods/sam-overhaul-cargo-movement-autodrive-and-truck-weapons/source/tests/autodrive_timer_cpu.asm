; Isolated native test adapter. No game process is accessed.
.const
ALIGN 16
TimerCap dd 41200000h,0,0,0
DeltaSeed dd 0,11223344h,55667788h,99AABBCCh
.code
RunTimer PROC FRAME
    push rbx
    .pushreg rbx
    sub rsp,40h
    .allocstack 40h
    vmovdqu xmmword ptr [rsp+20h],xmm13
    .savexmm128 xmm13,20h
    vmovdqu xmmword ptr [rsp+30h],xmm15
    .savexmm128 xmm15,30h
    .endprolog
    mov rbx,rdx
    vmovaps xmm15,xmmword ptr [DeltaSeed]
    vmovss xmm15,xmm15,xmm2
    vmovaps xmm13,xmmword ptr [TimerCap]
    xor eax,eax
    pushfq
    pop rax
    mov qword ptr [r9+24],rax
    mov rax,1122334455667788h
    call rcx
    mov qword ptr [r9],rax
    pushfq
    pop rdx
    mov qword ptr [r9+16],rdx
    vmovss dword ptr [r9+8],xmm15
    vmovdqu xmmword ptr [r9+32],xmm15
    vmovdqu xmmword ptr [r9+48],xmm0
    mov qword ptr [r9+64],rbx
    vmovdqu xmm13,xmmword ptr [rsp+20h]
    vmovdqu xmm15,xmmword ptr [rsp+30h]
    add rsp,40h
    pop rbx
    ret
RunTimer ENDP
END
