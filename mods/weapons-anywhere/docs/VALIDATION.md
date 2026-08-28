# Static and runtime validation

Validated statically against the installed Steam executable:

- version `1.10.89.0`;
- SHA-256 `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`;
- PE timestamp `0x6A3DAE46`;
- image size `0x0B292000`;
- unique expected 22-byte context at `0x00FA4247`;
- original branch opcode `74` at `0x00FA4255`.

The build and target validator are reproducible from the checked-in source. Core runtime behavior was subsequently confirmed in game as described below.

## Completed checks

- `tools/validate_target.py` passed all six checks against the installed, unmodified `DS2.exe`, including exactly one occurrence of the full instruction context.
- Two consecutive `/Brepro` 1.0.0 builds produced the same ASI SHA-256:
  `20B922F78393B44E61CA9BE44C689652554A7BA8398C34FED1CCE35D2EEC10D2`.
- The result is a native x86-64 DLL with DEP/ASLR flags and imports only the documented Win32 functions from `KERNEL32.dll`.
- A manual loader-harness check named `DS2.exe` loaded and unloaded the ASI. Because the harness intentionally had an undersized PE image, the ASI exited cleanly with code `0` and logged `ERROR: could not read DS2.exe build metadata; no patch applied.` The historical harness itself is not part of this repository.
- A second manual synthetic-harness check exposed matching PE metadata and the exact 22-byte context in private executable/read-only memory. The ASI changed only the expected branch byte to `EB`, logged `ACTIVE`, restored the region to executable/read-only protection, and exited with code `0`. The historical harness itself is not part of this repository.

## Runtime validation completed on 2026-08-28

- The user confirmed that ordinary weapons function inside a facility/building weapons-prohibited area.
- The user separately confirmed that grenades can be readied and used indoors in that area; screenshot evidence shows the grenade-aim state inside the facility.
- No crash or save-related problem was reported during these checks.

## Remaining regression coverage

- the native warning ring and announcement should be reconfirmed after game updates;
- melee actions, specialized equipment and vehicle-mounted weapons may have additional independent locks;
- entering and leaving the area, saving and reloading remain stable.
