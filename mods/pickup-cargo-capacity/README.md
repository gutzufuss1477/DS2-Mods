# DS2 Off-road Pickup Cargo Capacity Expansion

Source code for **Off-road Pickup Cargo Capacity Expansion v1.0.0** for *Death Stranding 2: On the Beach*.

The mod increases the Off-road Pickup's real cargo limit from the original 160 size units to an INI-configurable value from 160 to 480 units. The tested default is 320 units.

## Configuration

```ini
[PickupCargoCapacity]
Enabled=1
CapacityUnits=320
```

- `Enabled=1`: validates and applies the four coordinated runtime patches.
- `Enabled=0`: does not modify game code.
- `CapacityUnits`: requested real capacity. Values are clamped to 160–480 and rounded down to a multiple of 16.
- Cargo size units: `S=1`, `M=2`, `L=4`, `XL=6`.

## Source layout

- `src/pickup_cargo_capacity_patch.cpp` — complete mod source code
- `scripts/build-msvc.cmd` — MSVC build
- `scripts/build-llvm.cmd` — clang-cl/lld-link release-equivalent build
- `BUILDING.md` — detailed prerequisites and build instructions
- `SECURITY_NOTES.md` — runtime patch behaviour and imported APIs
- `RELEASE_VERIFICATION.md` — source-to-v1.0.0 binary verification
- `reference/PickupCargoCapacity_v1.0.0.asi` — exact v1.0.0 reference binary
- `tools/verify_pe_equivalence.py` — compares builds while ignoring only the PE timestamp

## Distribution

Binary release archives are published separately from the source branch. They are intentionally not committed here because GitHub already wraps the repository in a source ZIP; committing a release ZIP would create a ZIP-inside-ZIP download that can trigger antivirus archive heuristics.

Build locally with one of the documented scripts, or download the packaged mod from the GitHub Releases page when a release asset is available.

## How the patch works

The original pickup limit is generated from ten usable pack areas of sixteen size units each; pack-area index zero is reserved. The plugin validates and changes four mutually dependent constants in the pickup pack-area function:

- pack-area count;
- first index-array byte limit;
- second index-array byte limit;
- maximum pack-area index.

For `CapacityUnits=320`, the plugin configures twenty usable pack areas plus the reserved area.

The plugin is fail-closed: all four original byte sequences must match before any patch is written. A mismatch results in `STATE=ERROR` and no code modification.

## Build

See [BUILDING.md](BUILDING.md). The shortest MSVC build procedure is:

1. Install Visual Studio 2022 Build Tools with **Desktop development with C++** and a Windows SDK.
2. Open **x64 Native Tools Command Prompt for VS 2022**.
3. Run:

```bat
scripts\build-msvc.cmd
```

The output is `build/PickupCargoCapacity.asi`.

## Security-relevant implementation details

This is an in-process ASI plugin. It uses `VirtualProtect` and `FlushInstructionCache` only after exact-byte validation to modify four bytes/constants inside the current `DS2.exe` process. It does not open or modify another process.

It contains no networking, registry access, shell execution, downloads, telemetry or persistence. See [SECURITY_NOTES.md](SECURITY_NOTES.md).

## Known cosmetic limitation

The game's percentage display still uses the original 160-unit reference. At a configured capacity of 320 units, a fully loaded pickup can therefore display about 200%. The actual cargo limit is increased correctly.

## Test coverage

The 320-unit configuration was validated with loading to approximately 320 size units, more than 1800 kg of cargo, unloading, reloading, driving, saving, a full game restart and loading the save again.

## License

No separate open-source licence is granted by this repository. The source is published for transparency, auditing and build verification. All rights are reserved unless explicitly stated otherwise.
