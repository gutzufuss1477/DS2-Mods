# Technical Notes

## Objective

Scale only the native positive progression delta for Sam's activity-based proficiency system. Do not set a skill level directly and do not intercept unrelated play records.

## Target files analysed

| File | SHA-256 |
|---|---|
| `DS2.exe` | `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B` |
| `fullgame.dll` | `CAAB5764C41BBBAFB80499CC623D807F266F57C5B6D93062DA93EDFA83124832` |

Runtime hooks are placed only in `DS2.exe`. `fullgame.dll` was part of the static context and is not modified.

## Recovered skill types

| Internal type | Internal name | Visible category | Native record path |
|---:|---|---|---|
| 0 | `PowerOfGoingThroughRoughTerrein` | Terrain Traversal | Double record ID 8 |
| 1 | `LoadCapacity` | Carrying Capacity | Double record ID 10 |
| 2 | `FatigueTolerance` | Endurance | Double record ID 14 |
| 3 | `MeleeAttack` | Melee Combat | Q32 record ID `0x3F` |
| 4 | `MachineGun` | Machine Guns | Q32 record ID `0x41` |
| 5 | `AssaultRifle` | Assault Rifles | Q32 record ID `0x43` |
| 6 | `ShotGun` | Shotguns | Q32 record ID `0x45` |
| 7 | `SniperRifle` | Sniper Rifles | Q32 record ID `0x47` |
| 8 | `HandGun` | Handguns | Q32 record ID `0x49` |
| 9 | `Boomerang` | Strand Skills | Q32 record ID `0x4B` |
| 10 | `FaintTolerance` | KO Resistance | Double record ID 12 |
| 11 | `BreathHoldingCapacity` | Lung Capacity | Q64 record ID 6 |
| 12 | `StealthTakeDown` | Internal/non-radar skill | Q32 record ID `0x0B` |

## Native mutators

The producer functions ultimately call three DSPlayRecordManager add paths:

| Data type | RVA | Relevant semantics |
|---|---:|---|
| Q32 | `DS2.exe+C00D70` | `EDX=record ID`, `R8D=positive delta`, native total cap `999,999,999` |
| Q64 | `DS2.exe+C00E60` | `EDX=record ID`, `R8=positive delta`, `R9=cap` |
| Double | `DS2.exe+C00FA0` | `EDX=record ID`, `XMM2=positive delta`, `XMM3=cap` |

The central mutators serve many systems. The mod therefore does not patch their entries globally.

## Dedicated producer hooks

### U32 combat and Strand producer

- Function anchor: `DS2.exe+D92F30`
- Patch site: `DS2.exe+D92FCA`
- Stolen length: 19 bytes
- Continuation: `DS2.exe+D92FDD`, immediately after the original mutator call
- Native sequence covered by the patch:
  - load positive event delta from `[RDI+4]` into `R8D`;
  - copy mapped play-record ID from `BX` to `EDX`;
  - load the DSPlayRecordManager singleton into `RCX`;
  - call the native Q32 mutator at `DS2.exe+C00D70`.

The hook reconstructs the arguments and calls `ProcessSamStatsU32`. The release processor reads the Q32 total through `DS2.exe+C01120`, invokes the original mutator, reads the total again, and logs the committed difference.

### Q64 Lung Capacity producer

- Function anchor: `DS2.exe+D92FF0`
- Patch site: `DS2.exe+D93010`
- Stolen length: 29 bytes
- Continuation: `DS2.exe+D9302D`, the producer's ordinary epilogue
- Native sequence covered by the patch:
  - load event delta from `[RBX+8]` into `R8`;
  - set record ID 6;
  - set cap `0xE8D4A50FFF` (`999,999,999,999`);
  - unwind the producer stack and tail-jump to `DS2.exe+C00E60`.

The hook calls `ProcessSamStatsQ64` before the epilogue. The release processor reads the Q64 total through `DS2.exe+C011D0`, invokes the original Q64 mutator, reads the total again, and logs the committed difference.

### Double physical-stat producer

- Function anchor: `DS2.exe+D93040`
- Patch site: `DS2.exe+D9309A`
- Stolen length: 28 bytes
- Continuation: `DS2.exe+D930B6`, immediately after the original mutator call
- Native sequence covered by the patch:
  - load cap from `DS2.exe+3462020` into `XMM3`;
  - load event delta from `[RDI+8]` into `XMM2`;
  - load the DSPlayRecordManager singleton;
  - map `BX` into the record ID argument;
  - call the native double mutator at `DS2.exe+C00FA0`.

The hook reconstructs these arguments and calls `ProcessSamStatsDouble`. That release processor reads the current value through the native getter at `DS2.exe+C012B0`, calls the original native mutator with the configured delta, reads the value again, and records the committed difference. It does not write the encrypted record directly.

## Patching model

Each site receives a 12-byte absolute x64 jump (`mov rax, imm64; jmp rax`), followed by NOP padding to the validated stolen length. The validated windows are 19 bytes for Q32, 29 bytes for Q64, and 28 bytes for Double. All three anchors, patch windows, getters, and mutators must validate before the complete hook set is accepted. If a later patch fails, earlier patches are rolled back.

The current executable header is also locked to:

- timestamp `0x6A3DAE46`;
- image size `0x0B292000`.

## Gain handling

- Only positive native deltas are multiplied.
- Q32 and Q64 results are rounded to the nearest integer.
- Each result is clamped to the same cap supplied or enforced by DS2.
- Double gains are multiplied directly and clamped to the native `XMM3` cap.
- DS2 remains responsible for stored totals, threshold crossing, level changes, unlocks, UI, and save persistence.

## Porter Grade Booster compatibility

`Porter Grade Booster` patches the entry of `DS2.exe+C00E60` and filters its own delivery caller. `Sam Stats Booster` patches only the dedicated Lung Capacity producer at `D93010`. Its verifier calls the Q64 mutator entry from the ASI, so a Porter Grade hook sees a non-delivery return address and passes the call through its original trampoline. The Sam Stats installer accepts either the untouched Q64 entry or the known Porter Grade absolute detour. No producer patch bytes overlap.

## v1.0.0 runtime validation

The Double-backed path stored the exact requested `20.0x` delta for all four physical records. The Q64 path stored `2150` from `43` native Lung Capacity points at `50.0x`. The Q32 path stored `100` from `2` native points for both Melee Combat and Assault Rifles at `50.0x`. The user visually confirmed rapid Lung Capacity progression to level 5 in Sam's Stats menu.

The release does not hook HUD or notification code. A high multiplier may cross several visible thresholds in very few native events; the Stats menu remains the reliable presentation of the resulting level.
