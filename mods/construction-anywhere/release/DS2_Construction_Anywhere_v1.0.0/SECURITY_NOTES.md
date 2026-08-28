# Safety properties

- Runtime target gating by PE timestamp, image size, and exact signatures at every enabled patch site; the offline validator additionally checks the full executable SHA-256.
- Exact 20-byte signature validation at every enabled RVA.
- Fail-closed behavior: all enabled sites are preflighted before the first write.
- One-byte writes only; no on-disk executable modification.
- Instruction-cache flush and full 20-byte read-back after each write.
- Page protection restoration and multi-site rollback on any write failure.
- Existing identical `RET` patches are accepted but never claimed for rollback.
- The optional `PermittedArea` whitelist bypass is disabled by default.

Gameplay safety cannot be guaranteed inside story geometry or online synchronization zones. A backed-up save and an offline first test are required.
