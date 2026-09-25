.intel_syntax noprefix
.text
.extern DurabilityMultiplierBits
.extern DurabilityUnbreakableValue

.globl DurabilityContentsThunk
DurabilityContentsThunk:
    pushfq
    push rax
    mov rax, qword ptr [rdi + 0x38]
    test rax, rax
    jz .contents_vanilla
    mov eax, dword ptr [rax + 0x44]
    cmp eax, 0x011A1F8A
    je .contents_supported
    cmp eax, 0x030F5B99
    je .contents_supported
    cmp eax, 0x0429360A
    je .contents_supported
    cmp eax, 0x0576D524
    je .contents_supported
    cmp eax, 0x06027FF9
    je .contents_supported
    cmp eax, 0x0781F8E0
    je .contents_supported
    cmp eax, 0x07C9439C
    je .contents_supported
    cmp eax, 0x07E6A8FF
    je .contents_supported
    cmp eax, 0x0BFB7A5C
    je .contents_supported
    cmp eax, 0x0C944864
    je .contents_supported
    cmp eax, 0x0D831036
    je .contents_supported
    cmp eax, 0x0EF06EC5
    je .contents_supported
    cmp eax, 0x0F7FDA29
    je .contents_supported
    cmp eax, 0x10ADC3EE
    je .contents_supported
    cmp eax, 0x1499B068
    je .contents_supported
    cmp eax, 0x14D10B14
    je .contents_supported
    cmp eax, 0x15528C0D
    je .contents_supported
    cmp eax, 0x15AB0055
    je .contents_supported
    cmp eax, 0x15C65346
    je .contents_supported
    cmp eax, 0x1779C5FE
    je .contents_supported
    cmp eax, 0x178BAE7D
    je .contents_supported
    cmp eax, 0x1859E22B
    je .contents_supported
    cmp eax, 0x18AB89A8
    je .contents_supported
    cmp eax, 0x1C2F29DD
    je .contents_supported
    cmp eax, 0x1CDD425E
    je .contents_supported
    cmp eax, 0x1D339654
    je .contents_supported
    cmp eax, 0x1D52F6B2
    je .contents_supported
    cmp eax, 0x1ED3E3C2
    je .contents_supported
    cmp eax, 0x1FA9E883
    je .contents_supported
    cmp eax, 0x1FC4BB90
    je .contents_supported
    cmp eax, 0x2168A474
    je .contents_supported
    cmp eax, 0x227ABA61
    je .contents_supported
    cmp eax, 0x2288D1E2
    je .contents_supported
    cmp eax, 0x22AD398D
    je .contents_supported
    cmp eax, 0x22C06A9E
    je .contents_supported
    cmp eax, 0x236C06E4
    je .contents_supported
    cmp eax, 0x23D732CC
    je .contents_supported
    cmp eax, 0x244A6B77
    je .contents_supported
    cmp eax, 0x28C28248
    je .contents_supported
    cmp eax, 0x292C5642
    je .contents_supported
    cmp eax, 0x2A7AC0DE
    je .contents_supported
    cmp eax, 0x2A889790
    je .contents_supported
    cmp eax, 0x303CF510
    je .contents_supported
    cmp eax, 0x3087C138
    je .contents_supported
    cmp eax, 0x30EA922B
    je .contents_supported
    cmp eax, 0x312A4995
    je .contents_supported
    cmp eax, 0x3190996A
    je .contents_supported
    cmp eax, 0x32385780
    je .contents_supported
    cmp eax, 0x33F36BE5
    je .contents_supported
    cmp eax, 0x371A9883
    je .contents_supported
    cmp eax, 0x37E8F300
    je .contents_supported
    cmp eax, 0x39655C78
    je .contents_supported
    cmp eax, 0x39D858A9
    je .contents_supported
    cmp eax, 0x3A7CA5B6
    je .contents_supported
    cmp eax, 0x3A8529EE
    je .contents_supported
    cmp eax, 0x3A8ECE35
    je .contents_supported
    cmp eax, 0x3DC3C04C
    je .contents_supported
    cmp eax, 0x401B6FFF
    je .contents_supported
    cmp eax, 0x4053D483
    je .contents_supported
    cmp eax, 0x416A8365
    je .contents_supported
    cmp eax, 0x4198E8E6
    je .contents_supported
    cmp eax, 0x42577613
    je .contents_supported
    cmp eax, 0x42811128
    je .contents_supported
    cmp eax, 0x4341CA96
    je .contents_supported
    cmp eax, 0x43B3A115
    je .contents_supported
    cmp eax, 0x43FB1A69
    je .contents_supported
    cmp eax, 0x45711B80
    je .contents_supported
    cmp eax, 0x45837003
    je .contents_supported
    cmp eax, 0x481726B5
    je .contents_supported
    cmp eax, 0x49F9F2BF
    je .contents_supported
    cmp eax, 0x4B0EDF7B
    je .contents_supported
    cmp eax, 0x50113962
    je .contents_supported
    cmp eax, 0x50ABE99D
    je .contents_supported
    cmp eax, 0x50E352E1
    je .contents_supported
    cmp eax, 0x510785E7
    je .contents_supported
    cmp eax, 0x51BCB1CF
    je .contents_supported
    cmp eax, 0x51D1E2DC
    je .contents_supported
    cmp eax, 0x52C81B12
    je .contents_supported
    cmp eax, 0x53032777
    je .contents_supported
    cmp eax, 0x5621E874
    je .contents_supported
    cmp eax, 0x56D383F7
    je .contents_supported
    cmp eax, 0x581143DD
    je .contents_supported
    cmp eax, 0x585E2C8F
    je .contents_supported
    cmp eax, 0x5AA30247
    je .contents_supported
    cmp eax, 0x5B47D541
    je .contents_supported
    cmp eax, 0x5BB5BEC2
    je .contents_supported
    cmp eax, 0x5ED5C9B1
    je .contents_supported
    cmp eax, 0x62342B6E
    je .contents_supported
    cmp eax, 0x62C640ED
    je .contents_supported
    cmp eax, 0x649779AC
    je .contents_supported
    cmp eax, 0x651246FD
    je .contents_supported
    cmp eax, 0x65E02D7E
    je .contents_supported
    cmp eax, 0x66BA8817
    je .contents_supported
    cmp eax, 0x66F2336B
    je .contents_supported
    cmp eax, 0x67390F0E
    je .contents_supported
    cmp eax, 0x67ADD045
    je .contents_supported
    cmp eax, 0x67C08356
    je .contents_supported
    cmp eax, 0x6A326128
    je .contents_supported
    cmp eax, 0x6AC00AAB
    je .contents_supported
    cmp eax, 0x6C04BC0A
    je .contents_supported
    cmp eax, 0x6E44AADE
    je .contents_supported
    cmp eax, 0x6EB6C15D
    je .contents_supported
    cmp eax, 0x6F3975B1
    je .contents_supported
    cmp eax, 0x70800F9C
    je .contents_supported
    cmp eax, 0x7164D89A
    je .contents_supported
    cmp eax, 0x7196B319
    je .contents_supported
    cmp eax, 0x73719C89
    je .contents_supported
    cmp eax, 0x7469FCFA
    je .contents_supported
    cmp eax, 0x74FD23B1
    je .contents_supported
    cmp eax, 0x758D2BFC
    je .contents_supported
    cmp eax, 0x75A2C09F
    je .contents_supported
    cmp eax, 0x76B0DE8A
    je .contents_supported
    cmp eax, 0x77C78A58
    je .contents_supported
    cmp eax, 0x7990F95F
    je .contents_supported
    cmp eax, 0x7C9BEDC6
    je .contents_supported
    cmp eax, 0x7D14592A
    je .contents_supported
    cmp eax, 0x7EFFCB67
    je .contents_supported
    cmp eax, 0x7F544FFE
    je .contents_supported
    cmp eax, 0x7F85C026
    je .contents_supported
    cmp eax, 0x7FE89335
    je .contents_supported
    jmp .contents_vanilla
.contents_supported:
    cmp dword ptr [rip + DurabilityUnbreakableValue], 0
    jne .contents_unbreakable
    vsubss xmm1, xmm6, xmm1
    vdivss xmm1, xmm1, dword ptr [rip + DurabilityMultiplierBits]
    vsubss xmm1, xmm6, xmm1
    vmaxss xmm1, xmm1, xmm13
    jmp .contents_store
.contents_unbreakable:
    vmovaps xmm1, xmm6
    jmp .contents_store
.contents_vanilla:
    # Unsupported baggage (normal cargo/order/material/etc.): keep native value.
.contents_store:
    pop rax
    popfq
    vmovss dword ptr [rdi + 0x88], xmm1
    ret

.globl DurabilityBootsWearThunk
DurabilityBootsWearThunk:
    # Replays the displaced native load. Flags from VCOMISS at E70C52 are live.
    vmovss xmm6, dword ptr [rbp + 0x200]
    pushfq
    push rax
    mov rax, qword ptr [rbx + 0x418]
    test rax, rax
    jz .boots_done
    mov rax, qword ptr [rax + 0x38]
    test rax, rax
    jz .boots_done
    mov eax, dword ptr [rax + 0x44]
    cmp eax, 0x244A6B77
    je .boots_supported
    cmp eax, 0x371A9883
    je .boots_supported
    cmp eax, 0x3DC3C04C
    je .boots_supported
    cmp eax, 0x45711B80
    je .boots_supported
    cmp eax, 0x5621E874
    je .boots_supported
    cmp eax, 0x5ED5C9B1
    je .boots_supported
    cmp eax, 0x70800F9C
    je .boots_supported
    jmp .boots_done
.boots_supported:
    cmp dword ptr [rip + DurabilityUnbreakableValue], 0
    jne .boots_unbreakable
    vdivss xmm6, xmm6, dword ptr [rip + DurabilityMultiplierBits]
    jmp .boots_done
.boots_unbreakable:
    vxorps xmm6, xmm6, xmm6
.boots_done:
    pop rax
    popfq
    ret
