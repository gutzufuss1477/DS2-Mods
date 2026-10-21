// Standalone Windows unit-test wrapper. Calls compiled test functions only.
// It does not attach to, inject into or modify any game process.
.intel_syntax noprefix
.text
.globl RunCostFixture0
.seh_proc RunCostFixture0
RunCostFixture0:
push rbx
.seh_pushreg rbx
push rbp
.seh_pushreg rbp
push rdi
.seh_pushreg rdi
push rsi
.seh_pushreg rsi
push r12
.seh_pushreg r12
push r13
.seh_pushreg r13
push r14
.seh_pushreg r14
push r15
.seh_pushreg r15
sub rsp, 40
.seh_stackalloc 40
.seh_endprologue
mov qword ptr [rip + FixtureExpectedRsp], rsp
mov rax, qword ptr [rip + FixtureInput]
mov rbx, qword ptr [rip + FixtureInput + 8]
mov rcx, qword ptr [rip + FixtureInput + 16]
mov rdx, qword ptr [rip + FixtureInput + 24]
mov rsi, qword ptr [rip + FixtureInput + 32]
mov rdi, qword ptr [rip + FixtureInput + 40]
mov rbp, qword ptr [rip + FixtureInput + 48]
mov r8, qword ptr [rip + FixtureInput + 64]
mov r9, qword ptr [rip + FixtureInput + 72]
mov r10, qword ptr [rip + FixtureInput + 80]
mov r11, qword ptr [rip + FixtureInput + 88]
mov r12, qword ptr [rip + FixtureInput + 96]
mov r13, qword ptr [rip + FixtureInput + 104]
mov r14, qword ptr [rip + FixtureInput + 112]
mov r15, qword ptr [rip + FixtureInput + 120]
movdqu xmm0, xmmword ptr [rip + FixtureXmmIn]
movdqu xmm1, xmmword ptr [rip + FixtureXmmIn + 16]
movdqu xmm2, xmmword ptr [rip + FixtureXmmIn + 32]
movdqu xmm3, xmmword ptr [rip + FixtureXmmIn + 48]
movdqu xmm4, xmmword ptr [rip + FixtureXmmIn + 64]
movdqu xmm5, xmmword ptr [rip + FixtureXmmIn + 80]
push qword ptr [rip + FixtureFlagsIn]
popfq
call qword ptr [rip + FixtureTarget]
mov qword ptr [rip + FixtureOutput], rax
mov qword ptr [rip + FixtureOutput + 8], rbx
mov qword ptr [rip + FixtureOutput + 16], rcx
mov qword ptr [rip + FixtureOutput + 24], rdx
mov qword ptr [rip + FixtureOutput + 32], rsi
mov qword ptr [rip + FixtureOutput + 40], rdi
mov qword ptr [rip + FixtureOutput + 48], rbp
mov qword ptr [rip + FixtureOutput + 56], rsp
mov qword ptr [rip + FixtureOutput + 64], r8
mov qword ptr [rip + FixtureOutput + 72], r9
mov qword ptr [rip + FixtureOutput + 80], r10
mov qword ptr [rip + FixtureOutput + 88], r11
mov qword ptr [rip + FixtureOutput + 96], r12
mov qword ptr [rip + FixtureOutput + 104], r13
mov qword ptr [rip + FixtureOutput + 112], r14
mov qword ptr [rip + FixtureOutput + 120], r15
movdqu xmmword ptr [rip + FixtureXmmOut], xmm0
movdqu xmmword ptr [rip + FixtureXmmOut + 16], xmm1
movdqu xmmword ptr [rip + FixtureXmmOut + 32], xmm2
movdqu xmmword ptr [rip + FixtureXmmOut + 48], xmm3
movdqu xmmword ptr [rip + FixtureXmmOut + 64], xmm4
movdqu xmmword ptr [rip + FixtureXmmOut + 80], xmm5
pushfq
pop qword ptr [rip + FixtureFlagsOut]
add rsp, 40
pop r15
pop r14
pop r13
pop r12
pop rsi
pop rdi
pop rbp
pop rbx
ret
.seh_endproc

// Standalone Windows unit-test wrapper. Calls compiled test functions only.
// It does not attach to, inject into or modify any game process.
.intel_syntax noprefix
.text
.globl RunCostFixture1
.seh_proc RunCostFixture1
RunCostFixture1:
push rbx
.seh_pushreg rbx
push rbp
.seh_pushreg rbp
push rdi
.seh_pushreg rdi
push rsi
.seh_pushreg rsi
push r12
.seh_pushreg r12
push r13
.seh_pushreg r13
push r14
.seh_pushreg r14
push r15
.seh_pushreg r15
sub rsp, 32
.seh_stackalloc 32
.seh_endprologue
mov qword ptr [rip + FixtureExpectedRsp], rsp
mov rax, qword ptr [rip + FixtureInput]
mov rbx, qword ptr [rip + FixtureInput + 8]
mov rcx, qword ptr [rip + FixtureInput + 16]
mov rdx, qword ptr [rip + FixtureInput + 24]
mov rsi, qword ptr [rip + FixtureInput + 32]
mov rdi, qword ptr [rip + FixtureInput + 40]
mov rbp, qword ptr [rip + FixtureInput + 48]
mov r8, qword ptr [rip + FixtureInput + 64]
mov r9, qword ptr [rip + FixtureInput + 72]
mov r10, qword ptr [rip + FixtureInput + 80]
mov r11, qword ptr [rip + FixtureInput + 88]
mov r12, qword ptr [rip + FixtureInput + 96]
mov r13, qword ptr [rip + FixtureInput + 104]
mov r14, qword ptr [rip + FixtureInput + 112]
mov r15, qword ptr [rip + FixtureInput + 120]
movdqu xmm0, xmmword ptr [rip + FixtureXmmIn]
movdqu xmm1, xmmword ptr [rip + FixtureXmmIn + 16]
movdqu xmm2, xmmword ptr [rip + FixtureXmmIn + 32]
movdqu xmm3, xmmword ptr [rip + FixtureXmmIn + 48]
movdqu xmm4, xmmword ptr [rip + FixtureXmmIn + 64]
movdqu xmm5, xmmword ptr [rip + FixtureXmmIn + 80]
push qword ptr [rip + FixtureFlagsIn]
popfq
call qword ptr [rip + FixtureTarget]
mov qword ptr [rip + FixtureOutput], rax
mov qword ptr [rip + FixtureOutput + 8], rbx
mov qword ptr [rip + FixtureOutput + 16], rcx
mov qword ptr [rip + FixtureOutput + 24], rdx
mov qword ptr [rip + FixtureOutput + 32], rsi
mov qword ptr [rip + FixtureOutput + 40], rdi
mov qword ptr [rip + FixtureOutput + 48], rbp
mov qword ptr [rip + FixtureOutput + 56], rsp
mov qword ptr [rip + FixtureOutput + 64], r8
mov qword ptr [rip + FixtureOutput + 72], r9
mov qword ptr [rip + FixtureOutput + 80], r10
mov qword ptr [rip + FixtureOutput + 88], r11
mov qword ptr [rip + FixtureOutput + 96], r12
mov qword ptr [rip + FixtureOutput + 104], r13
mov qword ptr [rip + FixtureOutput + 112], r14
mov qword ptr [rip + FixtureOutput + 120], r15
movdqu xmmword ptr [rip + FixtureXmmOut], xmm0
movdqu xmmword ptr [rip + FixtureXmmOut + 16], xmm1
movdqu xmmword ptr [rip + FixtureXmmOut + 32], xmm2
movdqu xmmword ptr [rip + FixtureXmmOut + 48], xmm3
movdqu xmmword ptr [rip + FixtureXmmOut + 64], xmm4
movdqu xmmword ptr [rip + FixtureXmmOut + 80], xmm5
pushfq
pop qword ptr [rip + FixtureFlagsOut]
add rsp, 32
pop r15
pop r14
pop r13
pop r12
pop rsi
pop rdi
pop rbp
pop rbx
ret
.seh_endproc

// Deliberately use all volatile integer/SSE registers allowed by the Windows ABI.
// The real production thunks must preserve the caller's values around this call.
.globl FreeCostCount
.seh_proc FreeCostCount
FreeCostCount:
sub rsp, 40
.seh_stackalloc 40
.seh_endprologue
call FixtureSelectCount
pcmpeqd xmm0, xmm0
pcmpeqd xmm1, xmm1
pcmpeqd xmm2, xmm2
pcmpeqd xmm3, xmm3
pcmpeqd xmm4, xmm4
pcmpeqd xmm5, xmm5
mov rcx, 12341
mov rdx, 12342
mov r8, 12343
mov r9, 12344
mov r10, 12345
mov r11, 12346
add rsp, 40
ret
.seh_endproc

.globl CostReference_0
CostReference_0:
.byte 0x48,0x63,0x40,0x58,0x4d,0x8d,0x0c,0xc2
ret

.globl CostReference_1
CostReference_1:
.byte 0x49,0x63,0x4a,0x58,0x4c,0x8d,0x04,0xc8
ret

.globl CostReference_2
CostReference_2:
.byte 0x48,0x63,0x40,0x58,0x4d,0x8d,0x0c,0xc2
ret

.globl CostReference_3
CostReference_3:
.byte 0x48,0x63,0x4a,0x58,0x85,0xc9
ret

.globl CostReference_4
CostReference_4:
.byte 0x49,0x63,0x48,0x58,0x85,0xc9
ret

.globl CostReference_5
CostReference_5:
.byte 0x49,0x63,0x48,0x58,0x85,0xc9
ret

.globl CostReference_6
CostReference_6:
.byte 0x49,0x63,0x48,0x58,0x85,0xc9
ret

.globl CostReference_7
CostReference_7:
.byte 0x49,0x63,0x48,0x58,0x85,0xc9
ret

.globl CostReference_8
CostReference_8:
.byte 0x49,0x63,0x48,0x58,0x85,0xc9
ret

.globl CostReference_9
CostReference_9:
.byte 0x4d,0x63,0x41,0x58,0x4a,0x8d,0x14,0xc1
ret

.globl CostReference_10
CostReference_10:
.byte 0x49,0x63,0x41,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_11
CostReference_11:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_12
CostReference_12:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_13
CostReference_13:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_14
CostReference_14:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_15
CostReference_15:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_16
CostReference_16:
.byte 0x4d,0x63,0x42,0x58,0x4a,0x8d,0x14,0xc0
ret

.globl CostReference_17
CostReference_17:
.byte 0x48,0x63,0x40,0x58,0x48,0x8b,0xd1
ret

.globl CostReference_18
CostReference_18:
.byte 0x48,0x63,0x4a,0x58,0x48,0x8d,0x14,0xc8
ret

.globl CostReference_19
CostReference_19:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_20
CostReference_20:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_21
CostReference_21:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_22
CostReference_22:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_23
CostReference_23:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_24
CostReference_24:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_25
CostReference_25:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_26
CostReference_26:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_27
CostReference_27:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_28
CostReference_28:
.byte 0x48,0x63,0x42,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_29
CostReference_29:
.byte 0x48,0x63,0x4a,0x58,0x48,0x8d,0x14,0xc8
ret

.globl CostReference_30
CostReference_30:
.byte 0x49,0x63,0x46,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_31
CostReference_31:
.byte 0x49,0x63,0x46,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_32
CostReference_32:
.byte 0x49,0x63,0x46,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_33
CostReference_33:
.byte 0x49,0x63,0x46,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_34
CostReference_34:
.byte 0x49,0x63,0x46,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_35
CostReference_35:
.byte 0x49,0x63,0x46,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_36
CostReference_36:
.byte 0x48,0x63,0x40,0x58,0x48,0x8b,0xd1
ret

.globl CostReference_37
CostReference_37:
.byte 0x48,0x63,0x40,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_38
CostReference_38:
.byte 0x49,0x63,0x41,0x58,0x48,0x8d,0x14,0xc1
ret

.globl CostReference_39
CostReference_39:
.byte 0x49,0x63,0x41,0x58,0x88,0x4d,0x48
ret

.globl CostReference_40
CostReference_40:
.byte 0x49,0x63,0x41,0x58,0x88,0x4d,0x49
ret

.globl CostReference_41
CostReference_41:
.byte 0x49,0x63,0x41,0x58,0x88,0x4d,0x4a
ret

.globl CostReference_42
CostReference_42:
.byte 0x49,0x63,0x41,0x58,0x88,0x4d,0x4b
ret

.globl CostReference_43
CostReference_43:
.byte 0x49,0x63,0x41,0x58,0x88,0x4d,0x4c
ret
