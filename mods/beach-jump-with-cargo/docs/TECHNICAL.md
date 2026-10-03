# Technical notes

## Target

- DEATH STRANDING 2 PC Steam 1.10.89.0
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- PE timestamp: `0x6A3DAE46`
- SizeOfImage: `0x0B292000`

## Hook

RTTI and decompilation identify the common cargo routine as:

`DSBaggageManager::HandlingBaggagesOnFastTravel(bool)`

Entry RVA: `0x011E34A0`

Expected first eight bytes:

`40 55 41 55 41 56 41 57`

When enabled, the entry is replaced with a relative jump to a near relay. The relay performs an absolute jump to a small wrapper that returns immediately. This skips only the baggage-handling routine; the game's fast-travel destination, loading and arrival code continues normally.

The private test build exposed four live modes. Gameplay confirmed that completely skipping this handler retained Sam's carried cargo, so the public 1.0.0 build removes the experimental modes and keeps only that behavior.

## Safety checks

The ASI activates only inside a process named `DS2.exe`. It checks the PE timestamp, SizeOfImage and exact target prologue before changing executable memory. If a check fails, the mod remains inactive and records the reason in its log.

No game files or saves are edited on disk.
