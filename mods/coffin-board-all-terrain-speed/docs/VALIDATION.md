# Validation record — v1.0.0

## Final release profile

The packaged INI is checked during packaging and contains exactly one section
and two settings:

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400
```

All absent optional controls resolve to their safe defaults: steering angle
`100`, steering response `100`, wet grip `100`, and telemetry `0`. Consequently
the final profile installs no steering hook, performs no steering or wet-grip
resource write, starts no telemetry loop, and selects the dedicated drive
trampoline with no velocity sampling instruction.

## Synthetic runtime validation

`scripts/test-synthetic.ps1` builds the production source directly into a
native test DLL and verifies:

- exact group/index/UUID/RTTI resource targeting and loaded-group backfill;
- `SpeedPercent=500` derivation to `300/400 km/h` caps on land and water,
  `FinalGearRatio 15 -> 3`, and aligned `SlipSpeed 39 -> 300 km/h`;
- `AccelerationPercent=400` derivation to a 20x drive factor, producing
  nominally 4x effective torque with the 0.2x final gearing;
- final-profile preservation of `SteeringDegree=50` and wet side grip `0.09`;
- inactive steering coupling, null telemetry slot, and absent steering hook;
- byte-level branch/RIP target validation for the 72-byte no-telemetry drive
  trampoline and selector coverage proving that it is used when telemetry is
  zero;
- transaction rollback, reapplication without compounding, conflict refusal,
  and drive-factor neutralization on exact resource unload;
- exact StreamingSystem Add/Remove slot validation, refusal of a mismatched
  Remove slot, atomic callback-to-worker completion visibility, duplicate-add
  refusal, exactly one removal outside callback execution, idempotent removal,
  and bounded cleanup when completion is never published;
- dormant legacy diagnostic paths independently, so retained compatibility
  cannot silently regress the final profile.

## Target executable validation

`tools/validate_target.py` requires:

- Steam DS2 `1.10.89.0`, PE timestamp `0x6A3DAE46`, image size `0x0B292000`,
  and SHA-256
  `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`;
- the exact StreamingManager signature at RVA `0x693674` resolving to global
  RVA `0x6266938`;
- StreamingSystem vtable slot 3 resolving to `AddListener` RVA `0x26F6E40` and
  slot 4 resolving to `RemoveListener` RVA `0x26F6EE0`;
- the target RTTI names and serialized Coffin resource anchors;
- the unique drive-moment signature at RVA `0x247A431` and exact Coffin
  component vtable;
- the known steering and wet-grip consumers retained only for validation of
  dormant backward-compatible code.

The release script runs the complete synthetic suite and target validator,
requires both English INI comments, rejects any additional effective key,
stages exactly the ASI and INI, writes them in fixed order with a fixed ZIP
timestamp, and validates the final entry list before reporting success.
