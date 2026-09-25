# APAS Early Access rc.11 handover

Target: Steam DS2.exe 1.10.89.0, SHA256 BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B.

Static finding:
- Ring Device state builder: RVA 0x16E5120.
- APAS is Ring slot index 4.
- Vanilla APAS story predicate is FUN_141486880(2).
- Primary slot byte: state+0x2F.
- Selectable byte: state+0x38.
- Context restriction byte: state+0x43.
- Native APAS locate routine: RVA 0xBE1640.
- APAS manager: RVA 0x623E5D0.
- Resource root: RVA 0x623E310; system resource at root+0x2C0.
- Runtime readiness gate: RVA 0x623E5E8.

rc.11 design:
- Hook 0x16E5120, run vanilla first, then override only APAS slot 4.
- Preserve the context restriction at state+0x43.
- With EarlyAccess=1 + UnlockAll=1, locate missing APAS nodes through native 0xBE1640 once system resources are ready.
- No global story/facility facts are written.
- Existing cost hook and UnlockAll patch remain unchanged.
- Exact anchors cover constructor, locate routine, accounting/activation, unlock block, Ring builder and resource vtable.
- One-shot EARLY_RUNTIME diagnostic is appended to ds2_apas_memory_costs.log.

Validation:
- /W4 /WX LLVM build PASS.
- 17 INI cases PASS.
- Cost, EarlyAccess state, constructor relay, Ring relay, mapped install and native cost tests PASS.
- Isolated real ASI loader lifetime test PASS.
- Test ASI SHA256: 3977668C68C4AF8C3B805C7E49B1EE86128647ADA98D79071A121F83F49DC1E3.

In-game validation still required on the Episode-2 test save.
