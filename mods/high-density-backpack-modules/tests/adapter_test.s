.intel_syntax noprefix
.text
.globl test_adapter
.seh_proc test_adapter
test_adapter:
  sub rsp, 0xD8
  .seh_stackalloc 0xD8
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], rdx
  mov [rsp+0x30], r8
  mov [rsp+0x38], r9d
  mov rax, rdx
  mov ecx, 0x11
  mov edx, 0x22
  mov r8d, 0x88
  mov r9d, 0x99
  mov r10d, 0xAA
  mov r11d, 0xBB
  pcmpeqd xmm0, xmm0
  movdqa xmm1, xmm0
  movdqa xmm2, xmm0
  movdqa xmm3, xmm0
  movdqa xmm4, xmm0
  movdqa xmm5, xmm0
  call qword ptr [rsp+0x20]
  pushfq
  pop qword ptr [rsp+0x40]
  cmp dword ptr [rsp+0x38], 1
  je check_rcx
  cmp rax, [rsp+0x30]
  jne fail
  cmp dword ptr [rsp+0x38], 2
  je check_crypto_counter
  cmp rcx, 0x11
  jne fail
  jmp check_rest
check_crypto_counter:
  cmp ecx, r14d
  jne fail
  jmp check_rest
check_rcx:
  cmp rcx, [rsp+0x30]
  jne fail
  cmp rax, [rsp+0x28]
  jne fail
check_rest:
  cmp rdx, 0x22
  jne fail
  cmp r8, 0x88
  jne fail
  cmp r9, 0x99
  jne fail
  cmp r10, 0xAA
  jne fail
  cmp r11, 0xBB
  jne fail
  pmovmskb eax, xmm0
  cmp eax, 0xFFFF
  jne fail
  pmovmskb eax, xmm1
  cmp eax, 0xFFFF
  jne fail
  pmovmskb eax, xmm2
  cmp eax, 0xFFFF
  jne fail
  pmovmskb eax, xmm3
  cmp eax, 0xFFFF
  jne fail
  pmovmskb eax, xmm4
  cmp eax, 0xFFFF
  jne fail
  pmovmskb eax, xmm5
  cmp eax, 0xFFFF
  jne fail
success:
  xor eax, eax
  jmp done
fail:
  mov eax, 1
done:
  add rsp, 0xD8
  ret
.seh_endproc

# Execute the original game's Cryptobiosis loop bytes in a private allocation.
# RCX = loop entry, RDX = component; R14D is the native zero loop initializer.
.globl test_crypto_loop
.seh_proc test_crypto_loop
test_crypto_loop:
  push r14
  .seh_pushreg r14
  sub rsp, 0x20
  .seh_stackalloc 0x20
  .seh_endprologue
  xor r14d, r14d
  call rcx
  add rsp, 0x20
  pop r14
  ret
.seh_endproc

.globl test_module_relay
.seh_proc test_module_relay
test_module_relay:
  push rdi
  .seh_pushreg rdi
  push rsi
  .seh_pushreg rsi
  sub rsp, 0x28
  .seh_stackalloc 0x28
  .seh_endprologue
  mov rax, rcx
  mov rcx, rdx
  mov dl, r8b
  mov rdi, r9
  mov rsi, [rsp+0x60]
  call rax
  add rsp, 0x28
  pop rsi
  pop rdi
  ret
.seh_endproc

.globl test_menu_read
.seh_proc test_menu_read
test_menu_read:
  push rdi
  .seh_pushreg rdi
  push rbx
  .seh_pushreg rbx
  sub rsp, 0x38
  .seh_stackalloc 0x38
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], r9d
  mov rdi, rdx
  mov ebx, r8d
  mov eax, 0xABC
  call qword ptr [rsp+0x20]
  cmp ecx, [rsp+0x28]
  jne menu_read_fail
  cmp eax, 0xABC
  jne menu_read_fail
  xor eax, eax
  jmp menu_read_done
menu_read_fail:
  mov eax, 1
menu_read_done:
  add rsp, 0x38
  pop rbx
  pop rdi
  ret
.seh_endproc
