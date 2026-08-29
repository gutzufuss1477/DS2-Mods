# Technical notes

## Supported target

The release targets only the Steam PC `DS2.exe 1.10.89.0` image identified by:

- PE timestamp `0x6A3DAE46`;
- image size `0x0B292000`;
- file SHA-256 `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B` for offline validation.

Runtime build gating uses PE metadata plus expected type and instruction context. The full file hash is checked by `tools/validate_target.py`, not calculated by the injected ASI.

## Resource path

The canonical construction-resource path for the target build is:

```text
DS2.exe + 0x623EAD8       global root pointer
root + 0x28               DSConstructionManagerConfig pointer
manager + target offset   DSConstructionFacilityResource pointer
facility + 0x30           target construction-config pointer
config + 0x4E4            QpidCost (aligned 32-bit integer)
```

The expected manager VTable is at RVA `0x3280940`. A facility and target config are accepted only after their exact expected VTables and readable memory ranges have been confirmed.

## Descriptor table

| INI key | Manager offset | Config VTable RVA | Native reference cost |
| --- | ---: | ---: | ---: |
| `Postbox` | `0x38` | `0x3291028` | 200 |
| `SafeHouse` | `0x30` | `0x32913C0` | 2500 |
| `Watchtower` | `0x40` | `0x327F8F8` | 250 |
| `CargoCatapult` | `0x48` | `0x3293488` | 450 |
| `Generator` | `0x50` | `0x32938E8` | 350 |
| `RainShelter` | `0x58` | `0x3291EC0` | 500 |
| `Zipline` | `0x68` | `0x32863A8` | 500 |
| `Bridge` | `0x80`, `0x138`, `0x140`, `0x148` | `0x3292F68` | 1000 |
| `ChiralBridge` | `0x98` | `0x3292870` | 250 |
| `JumpRamp` | `0xA0` | `0x3291CF0` | 550 |
| `HotSpringDigger` | `0xF0` | `0x32931F8` | 0 |

All ten regular PCC construction baselines were statically cross-checked against the target resource data. The live placement HUD has confirmed exact custom values for Generator and Zipline. Gameplay accounting, refunds, and save-state behavior still require broader runtime validation across every supported type.

Normal bridges use three logical config resources with the same type VTable and native cost: the 30 m config `499:97902` is reachable through manager slots `+0x80` and `+0x138`, the 45 m config `499:38043` through `+0x140`, and the 80 m config `499:77692` through `+0x148`. The two 30 m slots identify the same static resource, but their runtime paths are validated independently. If they resolve to the same live config address it is written once; if DS2 supplies two valid equivalent instances, both unique addresses receive the one `Bridge` setting. A non-null invalid alias fails closed.

The Transponder resource is reachable through manager offset `0x130`, uses config VTable RVA `0x3293108`, and contains a native `QpidCost` value of zero. It is deliberately absent from the supported descriptor table because it is marked `IsTool=true` and `NeedWithinQpid=false`. Patching that field would not establish reliable chiral-bandwidth accounting. Hot Spring Digger also has a native zero cost, but it remains an experimental supported type because its resource is marked `NeedWithinQpid=true`.

## QpidCost consumers

The target executable contains validated cost-reading contexts at these RVAs:

- `0x133755C`;
- `0x127D859`;
- `0x127D87C`;
- `0x129CCC5`;
- `0x16BCFF8`.

Each context references the common config displacement `+0x4E4`. Together they provide offline evidence that the field participates in multiple construction, bandwidth-accounting, and presentation paths. The validator checks the exact target-build byte sequence and uniqueness at every fixed RVA, then independently confirms a compatible memory operation using this displacement. The complete executable hash remains the hard offline identity check.

## INI policy

The INI is loaded once from the ASI directory. `Native` or a missing construction key excludes that descriptor from writes. Decimal integers from `0` through `1000000` are accepted. Invalid enabled settings fail closed.

A numeric value equal to the native baseline is an effective no-op. Unknown keys, duplicates, truncation, malformed booleans, overflow, signs, hexadecimal input, fractions, and trailing text should be rejected rather than corrected silently.

## Lifecycle

The ASI worker waits for the canonical construction manager and requested resources. It must not write from `DllMain`. Resource identities are sampled until stable and re-resolved immediately before a transaction.

The worker continues low-frequency maintenance after the first successful application. If the game replaces a manager, facility, or config object during a map or resource transition, a new stable resource generation can receive the same startup settings. The INI is not hot-reloaded.

Recurring pending, invalid, conflict, and failure states should be logged only on state transitions to avoid log spam. A critical rollback failure stops maintenance for the session.

## Transaction policy

For the configured descriptor set:

1. Resolve every canonical object and validate its type.
2. Read all current costs without changing memory.
3. Accept only the descriptor's native value or requested target value.
4. Capture field values and page protections, deduplicating shared pages.
5. Make required pages writable.
6. Re-resolve every identity and revalidate all current values.
7. Use an aligned atomic compare/exchange from the captured value to the target, so a concurrent third-party value is never overwritten.
8. Read back and verify every field.
9. Restore and verify all page protections.

Any preflight or compare/exchange conflict results in no new writes for that generation, after conditionally releasing any earlier writes owned by the batch. Rollback uses the reverse atomic transition only while a field still equals this mod's target; a later third-party value is preserved and reported as a conflict. Existing fields that already equal their target remain unchanged.

## Save-state limitation

This patch changes shared construction configuration, not a cost stored explicitly by this mod on each built object. If DS2 credits a dismantled structure using the currently loaded config cost, changing the INI after construction could credit a different amount than was charged. The safe operating rule is therefore to keep costs fixed for a save once construction begins and to preserve a backup.
