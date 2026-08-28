# Security notes

`ds2_weapons_anywhere.asi` is an in-process native plugin. It should only be loaded into the matching `DS2.exe` build through a trusted external x64 ASI loader.

- It does not include or download an ASI loader.
- It does not access the network.
- It does not edit saves or original game files.
- It writes one branch-opcode byte to executable memory only after build and instruction-signature validation.
- Normal debug logging is disabled by default. A best-effort emergency log is created if an in-memory rollback cannot be completed safely.
- It refuses to patch unknown or conflicting instruction bytes.

Do not disable antivirus protection or add broad exclusions. If a false positive occurs, submit only the individual ASI to the antivirus vendor together with this source and its reproducible build instructions.
