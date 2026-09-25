.intel_syntax noprefix
.text
.globl SpecialBootUsageThunk
.seh_proc SpecialBootUsageThunk
SpecialBootUsageThunk:
.seh_endprologue
    jnz .allow
    cmp dword ptr [rcx + 0x20], 0x75D99124
    je .allow
    add qword ptr [rsp], 0xB3
.allow:
    ret
.seh_endproc
