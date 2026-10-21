OPTION CASEMAP:NONE
PUBLIC SamGateNativeTestInvoke
.code
; rcx = mock native source (DSConstructionRepairSprayComponent)
; rdx = executable snippet start (with ENDBR64 if available).
; Preserve nonvolatile registers that native baggage code clobbers.
SamGateNativeTestInvoke PROC
    push r14
    push rsi
    sub rsp,28h
    mov r14,rcx
    call rdx
    add rsp,28h
    pop rsi
    pop r14
    ret
SamGateNativeTestInvoke ENDP
END
