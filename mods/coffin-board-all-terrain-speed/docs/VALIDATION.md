# Validation record — v1.1.0

## Final release profile

The packaged INI is checked during packaging and contains exactly one section
and three settings:

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400

; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.
AllowFloatingCarrier=1
```

All absent legacy controls resolve to their safe defaults: steering angle
`100`, steering response `100`, wet grip `100`, and telemetry `0`. The final
profile therefore installs no steering hook, performs no steering or wet-grip
resource write, starts no telemetry loop, and selects the dedicated drive
trampoline with no velocity sampling instruction.

`AllowFloatingCarrier=1` installs only the validated Coffin-specific carrier
mount, link-preservation, detach-event, and notification guards. Public v1.1.0
contains no carrier follow-step scaling hook and performs no global
carrier-distance writes.

## Synthetic runtime validation

`scripts/test-synthetic.ps1` builds the production source directly into a
native test DLL and verifies the existing speed transaction plus the v1.1.0
carrier guard templates. Coverage includes:

- exact group/index/UUID/RTTI resource targeting and loaded-group backfill;
- `SpeedPercent=500` derivation to `300/400 km/h` caps on land and water,
  `FinalGearRatio 15 -> 3`, and the raised slip threshold;
- `AccelerationPercent=400` derivation to the tested effective drive profile;
- preservation of native steering and wet side grip;
- inactive steering coupling, null telemetry slot, and the no-telemetry drive
  trampoline;
- transaction rollback, reapplication without compounding, conflict refusal,
  and drive-factor neutralization on exact resource unload;
- exact StreamingSystem Add/Remove slot validation, callback-to-worker
  publication, exactly-once removal outside callbacks, and bounded no-target
  cleanup;
- byte-level validation of the Coffin-only linked-carrier preservation shim,
  the confirmed detach-event guard, and the exact-pair overextension warning
  filter;
- absence of the discarded public follow-step and global-distance patch paths.

## Gameplay validation

The v1.1.0 carrier path was exercised with a loaded Floating Carrier during
high-speed land travel, water crossings, collisions, and large jumps. The
carrier remained linked to the active Coffin Board. After dismounting, native
manual detach and reattach behavior remained available.

A confirmation run recorded 77 native overextension notification producer
hits and 77 Coffin-specific suppressions. The obsolete HUD warning and Sam
voice reaction did not appear for the exact active board/carrier pair.

Native cargo and collision damage were intentionally left unchanged.

## Target executable validation

`tools/validate_target.py` requires:

- Steam DS2 `1.10.89.0`, PE timestamp `0x6A3DAE46`, image size `0x0B292000`,
  and SHA-256
  `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`;
- the exact StreamingManager signature and native StreamingSystem Add/Remove
  listener slots;
- the target RTTI names and serialized Coffin resource anchors;
- the unique drive-moment signature and exact Coffin component vtable;
- the Coffin linked-object type-limit byte, linked-carrier preservation hook,
  confirmed Floating Carrier detach-event gate, and overextension-notification
  gate used by the public v1.1.0 carrier path.

The release script runs the complete synthetic suite and target validator,
requires all three English INI comments, rejects additional effective keys,
stages exactly the ASI and INI, writes them in fixed order with a fixed ZIP
timestamp, and validates the final entry list before reporting success.
