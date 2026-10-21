.text

.globl SamStatsU32Hook
.globl SamStatsQ64Hook
.globl SamStatsDoubleHook

.extern g_u32Continue
.extern g_q64Continue
.extern g_doubleContinue
.extern g_managerSlot
.extern g_doubleCap
.extern ProcessSamStatsU32
.extern ProcessSamStatsQ64
.extern ProcessSamStatsDouble

.p2align 4, 0x90
SamStatsU32Hook:
    # The original producer had already mapped the skill to the Q32 play-record
    # ID in BX. Route the full native write through the release processor and resume
    # immediately after the original mutator call.
    movl 0x4(%rdi), %r8d
    movzwl %bx, %edx
    movq g_managerSlot(%rip), %rax
    movq (%rax), %rcx
    call ProcessSamStatsU32
    jmp *g_u32Continue(%rip)

.p2align 4, 0x90
SamStatsQ64Hook:
    # Lung Capacity is the only Sam Stats Q64 record. The original producer
    # tail-called the shared mutator; the release processor performs that call and
    # resumes at the producer's ordinary epilogue.
    movq 0x8(%rbx), %r8
    movl $6, %edx
    movabsq $0x000000e8d4a50fff, %r9
    movq g_managerSlot(%rip), %rax
    movq (%rax), %rcx
    call ProcessSamStatsQ64
    jmp *g_q64Continue(%rip)

.p2align 4, 0x90
SamStatsDoubleHook:
    # Reproduce the complete native physical-stat call sequence, but route it
    # through the C++ release processor, which reads the encrypted record before and after
    # DS2's original double mutator.
    movsd g_doubleCap(%rip), %xmm3
    movsd 0x8(%rdi), %xmm2
    movq g_managerSlot(%rip), %rax
    movq (%rax), %rcx
    movzwl %bx, %edx
    call ProcessSamStatsDouble
    jmp *g_doubleContinue(%rip)
