# DS2 Floating Carrier Cargo Capacity v1.0.0

High-capacity Floating Carrier mod for **DEATH STRANDING 2: ON THE BEACH**.

## Capacity

- Total cargo footprint: **240 units** instead of the native 36.
- Native cargo footprint sizes are preserved: **S=1, M=2, L=4, XL=6**.
- Both carrier packing sides are expanded from **18 to 120 units**.
- Mixed S/M/L/XL cargo is supported.
- The game keeps its native visible placement logic, so additional cargo continues stacking vertically.

## Weight limits

Both Floating Carrier levels are supported:

- **Level 1**: native 300 kg limit raised to **5000 kg**.
- **Level 2**: native 900 kg limit raised to **5000 kg**.
- Related action thresholds are raised proportionally from 150/450 kg to **2500 kg**.

The 5000 kg value is the configured mod limit. Development testing confirmed that both carrier levels can exceed their original overload thresholds and remain pullable; the full 5000 kg ceiling was not practically tested because physical stack height becomes the more relevant limitation first.

## Physical stacking limitation

The Floating Carrier uses native visible cargo placement. Unlike the Tri-Cruiser, the carrier was observed to continue building the cargo stack upward with mixed cargo without requiring custom placement hooks.

Very tall stacks can still collide with ceilings, terminal roofs or other world geometry. The mod intentionally does not disable those physical collisions.

## Installation

1. Install a working 64-bit ASI loader.
2. Remove any older Floating Carrier test/RC ASI files.
3. Copy `DS2_FloatingCarrier_CargoCapacity_v1.0.0.asi` into the same game/ASI-loader directory used by your other DS2 ASI mods.
4. Start the game normally.

To uninstall, remove the ASI file.

## Compatibility / baseline

- Built and validated for **DS2.exe v1.10.89.0**.
- The mod validates the expected original instructions and constructor values before applying the runtime patches. If the supported executable layout does not match, the patch fails closed.
- The Floating Carrier cargo path is separate from the Pickup and Tri-Cruiser cargo-capacity paths, so this mod is designed to coexist with those mods.

## Technical summary

The release contains only the proven runtime mechanism:

- Floating Carrier item-cap virtualization: **36 -> 240**.
- Native carrier packer side limits: **18 -> 120** on both sides.
- Live global `DSBaggageConfig` patch for both carrier levels.
- Level 1 config: **300 / 150 / 300 -> 5000 / 2500 / 5000**.
- Level 2 config: **900 / 450 / 900 -> 5000 / 2500 / 5000**.
- Constructor fallback patches the same six values if the config is created again later.
- No Cheat Engine dependency.
- No PAGE_GUARD, breakpoints or single-step tracing.
- No ownership/relation manipulation.
- No fake-success/result overrides.
- No runtime STATUS writer or telemetry in the release build.

## Validation completed

Development testing covered:

- more than the native 36-unit capacity;
- large mixed S/M/L/XL loads;
- native vertical visual stacking;
- loads beyond the native Level 2 900 kg overload threshold while remaining pullable;
- loads beyond the native Level 1 300 kg overload threshold while remaining pullable;
- cargo-menu close/reopen during development testing.

## Safe removal

Before removing the mod from a save, reduce the carrier to native-safe limits where practical:

- cargo footprint at or below **36 units**;
- Level 1 weight at or below **300 kg**;
- Level 2 weight at or below **900 kg**.

Removing the mod while a save contains cargo above native limits is not recommended.

## Source / licence

Source is published for transparency, auditing and reproducible build verification. No separate open-source licence is granted unless explicitly stated otherwise.
