# Building Pickup Truck Hill Assist

## Requirements

- Windows x64.
- Visual Studio 2022 Build Tools or Visual Studio with the C++ desktop workload.
- Windows SDK.
- An **x64 Native Tools Command Prompt for Visual Studio**.

## Build

Run:

`BUILD_RELEASE.cmd`

The output is written to:

`build\PickupTruckHillAssist.asi`

## Build model

The plugin is deliberately built without the C/C++ runtime:

- `/O2`
- `/GS-`
- `/GR-`
- `/EHs-c-`
- `/Zl`
- `/nodefaultlib`
- `/entry:DllMain`
- `kernel32.lib`

The released binary was compiled with clang-cl/lld-link targeting
`x86_64-pc-windows-msvc`, using equivalent flags. Because compiler and linker
metadata may differ, a locally rebuilt binary is not expected to have the same
SHA-256 hash as the validated release.

## Compatibility

The source contains game-build-specific streaming offsets, signatures and
resource layouts for Steam build 1.10.89.0. Do not claim compatibility with
another build without testing it.
