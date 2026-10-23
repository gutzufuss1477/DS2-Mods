.text
.globl CoffinStoryVisualThunk
.def CoffinStoryVisualThunk; .scl 2; .type 32; .endef
.seh_proc CoffinStoryVisualThunk
CoffinStoryVisualThunk:
    # Entry from DS2.exe+0x1236CC6, where RBX is the current
    # baggage-view context, RCX=model manager and RDX=canonical bag.
    # Call the game ORIGINAL selector first with untouched parameters.
    # Preserve all Windows x64 nonvolatile registers and stack alignment.
    pushq %rbx
    .seh_pushreg %rbx
    subq $0x40, %rsp
    .seh_stackalloc 0x40
    .seh_endprologue
    movq %rdx, 0x20(%rsp)
    movq CoffinStoryOriginalResolver(%rip), %rax
    callq *%rax
    # Only the returned VISUAL entity resource may change. Never change
    # the underlying baggage list item, cargo handle or vehicle resources.
    movq %rbx, %r8
    movq 0x20(%rsp), %rdx
    movq %rax, %rcx
    callq CoffinStoryAfterResolve
    addq $0x40, %rsp
    popq %rbx
    retq
.seh_endproc
