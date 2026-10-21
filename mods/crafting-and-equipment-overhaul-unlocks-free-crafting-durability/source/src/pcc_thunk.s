.text
.globl PccCatalogueThunk
.def PccCatalogueThunk; .scl 2; .type 32; .endef
.seh_proc PccCatalogueThunk
PccCatalogueThunk:
    # Native callsite: RCX manager, RDX context UUID, R8 fact, RAX ammo, R15 behavior.
    # Preserve the original three arguments and add the two verified context arguments.
    subq $0x28, %rsp
    .seh_stackalloc 0x28
    .seh_endprologue
    movq %rax, %r9
    movq %r15, 0x20(%rsp)
    callq PccCatalogueCheck
    addq $0x28, %rsp
    retq
.seh_endproc
