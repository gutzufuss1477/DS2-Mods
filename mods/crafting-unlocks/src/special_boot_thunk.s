.intel_syntax noprefix
.text
.globl SpecialBootUsageThunk
.seh_proc SpecialBootUsageThunk
SpecialBootUsageThunk:
.seh_endprologue
    jnz .allow
    cmp dword ptr [rcx + 0x20], 0x75D99124
    je .allow
    cmp dword ptr [rcx + 0x20], 0x07B21227
    je .allow
    cmp dword ptr [rcx + 0x20], 0x6C3A478B
    je .allow
    cmp dword ptr [rcx + 0x20], 0x1EA3AF0B
    je .allow
    cmp dword ptr [rcx + 0x20], 0x6CC82C08
    je .allow
    cmp dword ptr [rcx + 0x20], 0x1E51C488
    je .allow
    cmp dword ptr [rcx + 0x20], 0x380248E3
    je .allow
    cmp dword ptr [rcx + 0x20], 0x4A69CBE0
    je .allow
    cmp dword ptr [rcx + 0x20], 0x38F02360
    je .allow
    cmp dword ptr [rcx + 0x20], 0x14889C47
    je .allow
    cmp dword ptr [rcx + 0x20], 0x4B7F7765
    je .allow
    cmp dword ptr [rcx + 0x20], 0x3914F466
    je .allow
    cmp dword ptr [rcx + 0x20], 0x0CE5E07A
    je .allow
    add qword ptr [rsp], 0xB3
.allow:
    ret
.seh_endproc
