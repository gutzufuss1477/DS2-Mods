# Validation record — v1.1.2

## Release profile

```ini
[CoffinBoardAllTerrainSpeed]
SpeedPercent=500
AccelerationPercent=400
AllowFloatingCarrier=1
```

The packaged INI contains only these three settings. `DebugLog` is absent and therefore defaults to `0`.

## Build validation

- Public source compiles with `clang-cl` using `/W4 /WX`.
- The x64 ASI links successfully with `lld-link`.
- The synthetic runtime-test source compiles and links against the same public source.
- Release payload is exactly the ASI and INI.
- Public behavior contains no experimental Floating Carrier follow-speed hook or global distance-limit writes.
- Carrier path trace is disabled by default and runs only if `DebugLog=1` is explicitly supplied.

## Gameplay validation

The final v1.1.2 logic is based directly on the confirmed hybrid test performed on 2026-09-04 with Steam `DS2.exe 1.10.89.0`:

- `SpeedPercent=500` restored the expected high Coffin Board speed.
- `AccelerationPercent=400` remained active with the speed profile.
- A connected loaded Floating Carrier remained attached while riding.
- The Floating Carrier boundary HUD warning did not reappear.
- Sam's corresponding boundary dialogue/text spam did not reappear.

The fix preserves the gameplay-validated startup order: carrier guards first, drive hook second, streaming listener third. This is the relevant difference from the broken intermediate build in which the speed-resource patch timed out without completing.
