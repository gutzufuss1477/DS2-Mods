# Windows x64 adapters for verified inline charm loops.
# Each patch site has an aligned caller stack and no live flags after the
# replaced pointer formation. Crypto also initializes the native loop counter;
# its comparison remains in the game because the loop branches back to it.
.intel_syntax noprefix
.text

.globl charm_snapshot_rax
.seh_proc charm_snapshot_rax
charm_snapshot_rax:
  sub rsp, 0xD8
  .seh_stackalloc 0xD8
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], rdx
  mov [rsp+0x30], r8
  mov [rsp+0x38], r9
  mov [rsp+0x40], r10
  mov [rsp+0x48], r11
  mov [rsp+0xB0], rax
  movdqu [rsp+0x50], xmm0
  movdqu [rsp+0x60], xmm1
  movdqu [rsp+0x70], xmm2
  movdqu [rsp+0x80], xmm3
  movdqu [rsp+0x90], xmm4
  movdqu [rsp+0xa0], xmm5
  lea rcx, [rax+0x1FC]
  call virtual_charm_array
  mov rcx, [rsp+0x20]
  mov rdx, [rsp+0x28]
  mov r8, [rsp+0x30]
  mov r9, [rsp+0x38]
  mov r10, [rsp+0x40]
  mov r11, [rsp+0x48]
  movdqu xmm0, [rsp+0x50]
  movdqu xmm1, [rsp+0x60]
  movdqu xmm2, [rsp+0x70]
  movdqu xmm3, [rsp+0x80]
  movdqu xmm4, [rsp+0x90]
  movdqu xmm5, [rsp+0xa0]
  add rsp, 0xD8
  ret
.seh_endproc

.globl charm_snapshot_rcx
.seh_proc charm_snapshot_rcx
charm_snapshot_rcx:
  sub rsp, 0xD8
  .seh_stackalloc 0xD8
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], rdx
  mov [rsp+0x30], r8
  mov [rsp+0x38], r9
  mov [rsp+0x40], r10
  mov [rsp+0x48], r11
  mov [rsp+0xB0], rax
  movdqu [rsp+0x50], xmm0
  movdqu [rsp+0x60], xmm1
  movdqu [rsp+0x70], xmm2
  movdqu [rsp+0x80], xmm3
  movdqu [rsp+0x90], xmm4
  movdqu [rsp+0xa0], xmm5
  lea rcx, [rax+0x1FC]
  call virtual_charm_array
  mov rcx, rax
  mov rax, [rsp+0xB0]
  mov rdx, [rsp+0x28]
  mov r8, [rsp+0x30]
  mov r9, [rsp+0x38]
  mov r10, [rsp+0x40]
  mov r11, [rsp+0x48]
  movdqu xmm0, [rsp+0x50]
  movdqu xmm1, [rsp+0x60]
  movdqu xmm2, [rsp+0x70]
  movdqu xmm3, [rsp+0x80]
  movdqu xmm4, [rsp+0x90]
  movdqu xmm5, [rsp+0xa0]
  add rsp, 0xD8
  ret
.seh_endproc

.globl charm_crypto_rax
.seh_proc charm_crypto_rax
charm_crypto_rax:
  sub rsp, 0xD8
  .seh_stackalloc 0xD8
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], rdx
  mov [rsp+0x30], r8
  mov [rsp+0x38], r9
  mov [rsp+0x40], r10
  mov [rsp+0x48], r11
  mov [rsp+0xB0], rax
  movdqu [rsp+0x50], xmm0
  movdqu [rsp+0x60], xmm1
  movdqu [rsp+0x70], xmm2
  movdqu [rsp+0x80], xmm3
  movdqu [rsp+0x90], xmm4
  movdqu [rsp+0xa0], xmm5
  mov rcx, rax
  test rdx, rdx
  mov eax, 0
  cmovz rcx, rax
  call virtual_charm_array
  mov ecx, r14d
  mov rdx, [rsp+0x28]
  mov r8, [rsp+0x30]
  mov r9, [rsp+0x38]
  mov r10, [rsp+0x40]
  mov r11, [rsp+0x48]
  movdqu xmm0, [rsp+0x50]
  movdqu xmm1, [rsp+0x60]
  movdqu xmm2, [rsp+0x70]
  movdqu xmm3, [rsp+0x80]
  movdqu xmm4, [rsp+0x90]
  movdqu xmm5, [rsp+0xa0]
  add rsp, 0xD8
  ret
.seh_endproc

.globl charm_full_snapshot_rax
.seh_proc charm_full_snapshot_rax
charm_full_snapshot_rax:
  sub rsp, 0xD8
  .seh_stackalloc 0xD8
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], rdx
  mov [rsp+0x30], r8
  mov [rsp+0x38], r9
  mov [rsp+0x40], r10
  mov [rsp+0x48], r11
  mov [rsp+0xB0], rax
  movdqu [rsp+0x50], xmm0
  movdqu [rsp+0x60], xmm1
  movdqu [rsp+0x70], xmm2
  movdqu [rsp+0x80], xmm3
  movdqu [rsp+0x90], xmm4
  movdqu [rsp+0xa0], xmm5
  lea rcx, [rax+0x48C]
  call virtual_charm_array
  mov rcx, [rsp+0x20]
  mov rdx, [rsp+0x28]
  mov r8, [rsp+0x30]
  mov r9, [rsp+0x38]
  mov r10, [rsp+0x40]
  mov r11, [rsp+0x48]
  movdqu xmm0, [rsp+0x50]
  movdqu xmm1, [rsp+0x60]
  movdqu xmm2, [rsp+0x70]
  movdqu xmm3, [rsp+0x80]
  movdqu xmm4, [rsp+0x90]
  movdqu xmm5, [rsp+0xa0]
  add rsp, 0xD8
  ret
.seh_endproc

.globl charm_menu_read
.seh_proc charm_menu_read
charm_menu_read:
  sub rsp, 0xD8
  .seh_stackalloc 0xD8
  .seh_endprologue
  mov [rsp+0x20], rcx
  mov [rsp+0x28], rdx
  mov [rsp+0x30], r8
  mov [rsp+0x38], r9
  mov [rsp+0x40], r10
  mov [rsp+0x48], r11
  mov [rsp+0xB0], rax
  movdqu [rsp+0x50], xmm0
  movdqu [rsp+0x60], xmm1
  movdqu [rsp+0x70], xmm2
  movdqu [rsp+0x80], xmm3
  movdqu [rsp+0x90], xmm4
  movdqu [rsp+0xa0], xmm5
  mov rcx, rdi
  mov edx, ebx
  call menu_charm_value
  mov rcx, rax
  mov rax, [rsp+0xB0]
  mov rdx, [rsp+0x28]
  mov r8, [rsp+0x30]
  mov r9, [rsp+0x38]
  mov r10, [rsp+0x40]
  mov r11, [rsp+0x48]
  movdqu xmm0, [rsp+0x50]
  movdqu xmm1, [rsp+0x60]
  movdqu xmm2, [rsp+0x70]
  movdqu xmm3, [rsp+0x80]
  movdqu xmm4, [rsp+0x90]
  movdqu xmm5, [rsp+0xa0]
  add rsp, 0xD8
  ret
.seh_endproc
