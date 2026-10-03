.intel_syntax noprefix
.text
.globl TestThunk
TestThunk:
 push rdi
 push rsi
 push r13
 sub rsp, 0x20
 mov rdi, rcx
 mov rsi, rdx
 mov r13, r8
 mov r8, r9
 call LoadingThunk
 add rsp, 0x20
 pop r13
 pop rsi
 pop rdi
 ret
