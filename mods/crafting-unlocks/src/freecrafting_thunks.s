.intel_syntax noprefix
.text
.extern FreeCostCount
.globl FcThunk_0
.seh_proc FcThunk_0
FcThunk_0:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 0]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x4D,0x8D,0x0C,0xC2
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_1
.seh_proc FcThunk_1
FcThunk_1:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 40]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x4C,0x8D,0x04,0xC8
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_2
.seh_proc FcThunk_2
FcThunk_2:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 0]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x4D,0x8D,0x0C,0xC2
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_3
.seh_proc FcThunk_3
FcThunk_3:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x85,0xC9
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_4
.seh_proc FcThunk_4
FcThunk_4:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 24]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x85,0xC9
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_5
.seh_proc FcThunk_5
FcThunk_5:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 24]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x85,0xC9
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_6
.seh_proc FcThunk_6
FcThunk_6:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 24]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x85,0xC9
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_7
.seh_proc FcThunk_7
FcThunk_7:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 24]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x85,0xC9
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_8
.seh_proc FcThunk_8
FcThunk_8:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 24]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x85,0xC9
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_9
.seh_proc FcThunk_9
FcThunk_9:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 24], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x4A,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_10
.seh_proc FcThunk_10
FcThunk_10:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_11
.seh_proc FcThunk_11
FcThunk_11:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_12
.seh_proc FcThunk_12
FcThunk_12:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_13
.seh_proc FcThunk_13
FcThunk_13:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_14
.seh_proc FcThunk_14
FcThunk_14:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_15
.seh_proc FcThunk_15
FcThunk_15:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_16
.seh_proc FcThunk_16
FcThunk_16:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 40]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 24], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x4A,0x8D,0x14,0xC0
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_17
.seh_proc FcThunk_17
FcThunk_17:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 0]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8B,0xD1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_18
.seh_proc FcThunk_18
FcThunk_18:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC8
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_19
.seh_proc FcThunk_19
FcThunk_19:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_20
.seh_proc FcThunk_20
FcThunk_20:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_21
.seh_proc FcThunk_21
FcThunk_21:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_22
.seh_proc FcThunk_22
FcThunk_22:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_23
.seh_proc FcThunk_23
FcThunk_23:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_24
.seh_proc FcThunk_24
FcThunk_24:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_25
.seh_proc FcThunk_25
FcThunk_25:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_26
.seh_proc FcThunk_26
FcThunk_26:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_27
.seh_proc FcThunk_27
FcThunk_27:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_28
.seh_proc FcThunk_28
FcThunk_28:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_29
.seh_proc FcThunk_29
FcThunk_29:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 16]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 8], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC8
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_30
.seh_proc FcThunk_30
FcThunk_30:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, r14
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_31
.seh_proc FcThunk_31
FcThunk_31:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, r14
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_32
.seh_proc FcThunk_32
FcThunk_32:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, r14
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_33
.seh_proc FcThunk_33
FcThunk_33:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, r14
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_34
.seh_proc FcThunk_34
FcThunk_34:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, r14
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_35
.seh_proc FcThunk_35
FcThunk_35:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, r14
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_36
.seh_proc FcThunk_36
FcThunk_36:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 0]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8B,0xD1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_37
.seh_proc FcThunk_37
FcThunk_37:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 0]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_38
.seh_proc FcThunk_38
FcThunk_38:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x48,0x8D,0x14,0xC1
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_39
.seh_proc FcThunk_39
FcThunk_39:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x88,0x4D,0x48
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_40
.seh_proc FcThunk_40
FcThunk_40:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x88,0x4D,0x49
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_41
.seh_proc FcThunk_41
FcThunk_41:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x88,0x4D,0x4A
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_42
.seh_proc FcThunk_42
FcThunk_42:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x88,0x4D,0x4B
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

.globl FcThunk_43
.seh_proc FcThunk_43
FcThunk_43:
push r12
.seh_pushreg r12
pushfq
.seh_stackalloc 8
sub rsp, 160
.seh_stackalloc 160
mov r12, rsp
.seh_setframe r12, 0
.seh_endprologue
mov qword ptr [r12 + 0], rax
mov qword ptr [r12 + 8], rcx
mov qword ptr [r12 + 16], rdx
mov qword ptr [r12 + 24], r8
mov qword ptr [r12 + 32], r9
mov qword ptr [r12 + 40], r10
mov qword ptr [r12 + 48], r11
movdqu xmmword ptr [r12 + 64], xmm0
movdqu xmmword ptr [r12 + 80], xmm1
movdqu xmmword ptr [r12 + 96], xmm2
movdqu xmmword ptr [r12 + 112], xmm3
movdqu xmmword ptr [r12 + 128], xmm4
movdqu xmmword ptr [r12 + 144], xmm5
and rsp, -16
sub rsp, 32
mov rcx, qword ptr [r12 + 32]
call FreeCostCount
movsxd rax, eax
mov qword ptr [r12 + 0], rax
movdqu xmm0, xmmword ptr [r12 + 64]
movdqu xmm1, xmmword ptr [r12 + 80]
movdqu xmm2, xmmword ptr [r12 + 96]
movdqu xmm3, xmmword ptr [r12 + 112]
movdqu xmm4, xmmword ptr [r12 + 128]
movdqu xmm5, xmmword ptr [r12 + 144]
push qword ptr [r12 + 160]
popfq
mov rax, qword ptr [r12 + 0]
mov rcx, qword ptr [r12 + 8]
mov rdx, qword ptr [r12 + 16]
mov r8, qword ptr [r12 + 24]
mov r9, qword ptr [r12 + 32]
mov r10, qword ptr [r12 + 40]
mov r11, qword ptr [r12 + 48]
.byte 0x88,0x4D,0x4C
lea rsp, [r12 + 168]
pop r12
ret
.seh_endproc

