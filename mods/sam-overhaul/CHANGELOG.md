# Changelog

## 1.1.0 - 2026-10-09

Adds three independent, optional and configurable quality-of-life upgrades to Sam Overhaul.

### Footprints
- Optional native visual suppression of both ordinary footprints and blue Odradek-highlighted footprint tracks.
- Also hides previously generated footprints restored when loading a save.
- No ReShade or separate footprint ASI. Save files and player movement mechanics are unchanged.
- New [Footprints] HideFootprints=1 switch; defaults to 0 for compatibility.

### Generators
- Optional doubled charging/coverage radius with matching blue visible Odradek circle.
- Verified native generator trigger expansion, Jolt collision refresh and component/resource ring synchronization on the tested Steam game.
- New [GeneratorRange] Enabled=1, RangePercent=200 switches. Defaults off.

### Timefall shelters
- Optional doubled timefall protection and blue ground-circle radius (4 m to 8 m).
- Independent native cargo-container repair sphere of 8.6 m (215%) for better alignment on sloping terrain.
- Source-validated native RepairSpray contact gate retains vanilla behavior for unrelated effects and preserves native repair amount/effects.
- Corrected oversized renderer circle and restores proper native 16 m draw diameter / visible 8 m radius.
- German rain-shelter prompt uses "Verschnaufen" instead of "In Bunker ausruhen" while preserving the original shelter-rest action.
- Native localized-text correction handles both preloaded and subsequently streamed text resources.
- Steady-state shelter optimization skips redundant full Jolt/renderer discovery only while all native owner, BodyID, source and geometry checks remain valid. Full validation is triggered again if any component changes. Verified in-game without perceptible stutter.
- New [TimefallShelterRange] Enabled, RangePercent, RepairRadiusPercent, FixRestPrompt and SpatialDiagnostics options. Enabled defaults to 0.

### Compatibility and validation
- Preserves existing cargo hiding, movement, Autodrive and four independent truck-weapon profiles and legacy INI values.
- Supports only the verified Steam DS2.exe 1.10.89.0 native layout (Windows x64).
- Source includes regression coverage for native jump rel32 instructions, repair-gate assembly and source liveness, original player actions, save/reload identity, visual geometry, localized text, 105,704 Autodrive checks and the 29-case shelter steady-state policy.

## 1.0.0 - 2026-09-30

Initial public release of Sam Overhaul.

- Added configurable hiding for shoulder cargo, hip cargo, backpack cargo and spare shoes.
- Keeps currently worn shoes visible and leaves inventory, weight and cargo ownership native.
- Added monorail dismount beyond the final vanilla safety/height blocker while preserving earlier state checks.
- Added wider zipline dismount availability using the native input and exit animation.
- Added qualifying landing rolls with backpack/cargo while preserving native landing thresholds and fall-damage logic.
- Added configurable Autodrive activation time with a tested 2.0-second default.
- Added independent Heavy Machine Gun tuning: range, aim speed, lock-on, burst duration and cooldown.
- Added independent Mortar tuning: range, aim speed, lock-on, recovery and cooldown.
- Added independent Chiral Cannon tuning: range, long-range profile, aim speed, lock-on, recovery, cooldown and charge time.
- Chiral Cannon default charge reduced from 3.0 seconds to 0.75 seconds after live A/B testing.
- Added independent Missile Launcher tuning: range, aim speed, lock-on, recovery and cooldown.
- Added strict target-build/context validation and local startup/configuration logging.
