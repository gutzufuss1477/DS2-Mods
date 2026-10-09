; Steam DS2.exe v1.10.89.0: safe scoped RepairSpray contact predicate.
; Replacement for native JE +0xC5 at RVA 0x11B0698.
; dev22 crashed at save load due to E9 displacement calculated from site+6
; instead of the proper site+5. dev23 fixes this with a shared verified helper.
;
; Safety revision: NO dereference of resource, owner, member array or any
; external pointer on a native source that was NOT already registered by
; SamConstructionShelter's owner/type/radius-validated active object scan.
; Gate compares the source and its inline resource/owner pointers against
; previously validated entries. Failure falls through to exact original test.
;
; Hook does not call, allocate, sleep, use SSE, edit cargo, repair contact or
; game world. Saves/restores RAX, RCX, RDX (all other regs untouched).
OPTION CASEMAP:NONE

PUBLIC SamGateComponentVT
PUBLIC SamGateResourceVT
PUBLIC SamGateOwnerVT
PUBLIC SamGateRadiusBits
PUBLIC SamGateRegistrationCount
PUBLIC SamGateRegistrations
PUBLIC SamGateContinue
PUBLIC SamGateSkip
PUBLIC SamRepairGateStub
PUBLIC SamGateTestInvoke

.data
ALIGN 8
SamGateComponentVT DQ 0
SamGateResourceVT  DQ 0
SamGateOwnerVT     DQ 0
SamGateRadiusBits  DD 0
ALIGN 4
SamGateRegistrationCount DD 0
ALIGN 8
; Entry 0..31, each 24-byte tuple (source,resource,owner).
; Must exactly match struct SamGateRegistration in C++.
SamGateRegistrations DQ 96 DUP(0)
SamGateContinue     DQ 0
SamGateSkip         DQ 0

.code
SamRepairGateStub PROC
    ; ENDBR64 supports CET indirect-branch tracking.
    db 0F3h, 0Fh, 1Eh, 0FAh
    push rax
    push rcx
    push rdx
    mov ecx, dword ptr [SamGateRegistrationCount]
    test ecx,ecx
    jle native_contact_check
    cmp ecx,32
    ja native_contact_check
    lea rax,SamGateRegistrations

source_loop:
    ; Compare pointer identity without dereferencing unvalidated pointers.
    cmp qword ptr [rax],r14
    jne next_entry

    ; Original r14 source is valid enough for the vanilla +0x70 read.
    mov rdx,qword ptr [r14]
    cmp rdx,qword ptr [SamGateComponentVT]
    jne next_entry
    mov rdx,qword ptr [r14+30h]
    cmp rdx,qword ptr [rax+8]
    jne next_entry
    mov rdx,qword ptr [r14+48h]
    cmp rdx,qword ptr [rax+10h]
    je eligible_expanded_source

next_entry:
    add rax,18h
    dec ecx
    jnz source_loop

native_contact_check:
    pop rdx
    pop rcx
    pop rax
    ; Restore exactly original predicate for ANY other repair source.
    cmp byte ptr [r14+70h],0
    je original_skip
    jmp original_continue

eligible_expanded_source:
    pop rdx
    pop rcx
    pop rax
original_continue:
    jmp qword ptr [SamGateContinue]
original_skip:
    jmp qword ptr [SamGateSkip]
SamRepairGateStub ENDP

; Synthetic unit test calls SAME production MASM stub with mock target
; functions. No game or process injection by the test harness.
SamGateTestInvoke PROC
    push r14
    mov r14,rcx
    call SamRepairGateStub
    pop r14
    ret
SamGateTestInvoke ENDP

END
