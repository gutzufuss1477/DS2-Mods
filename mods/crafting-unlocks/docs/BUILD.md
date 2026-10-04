# Build and package 1.7.0

## Windows

Use LLVM clang-cl/lld-link, Python 3.9+ and a Windows SDK with D3D12 headers.
Run `build.ps1` from the mod folder. It embeds the hash-checked weapon pixel
payloads and compiles the host, ATLAS and weapons into one ASI with one DllMain,
initialization worker, INI and log. Only KERNEL32.dll is imported; D3D12 calls
use the game's existing device. No game files are changed during the build.

Run `scripts/build_tests.cmd`, `scripts/test_atlas.ps1` and
`scripts/test_weapons.ps1`. The latter two need Python Capstone and the exact
local game executable at the workspace's `analysis/DS2.exe`. Weapon resource
fixtures are extracted from `tests/fixtures/weapons-native.zip` into `build/`.
The GPU tests run real D3D12 uploads/readbacks in separate test processes.
The Windows C++ host tests require Visual Studio Build Tools.

Run `python tests/verify_build.py`, `python tests/verify_integration.py` and
`python scripts/smoke_loader.py` for PE/config, all 94 exact signatures and
overlap checks, and disposable-process loader rejection tests. These tests
do not start or write to DS2 and do not replace a combined gameplay test.

## Configuration and policies

`python tools/generate_english_config.py` regenerates the public defaults.
`python scripts/generate_durability_scope.py` regenerates the shared native
durability thunks/allowlists, including four new and one legacy weapon bags.
Do not run config generation on a player's personalized INI.

## Linux policy checks

`bash tools/validate.sh` runs host policy/config tests and their ASan/UBSan
variants only. Build the complete ASI on Windows because the weapon GPU
translation unit requires the Windows SDK. Linux checks do not overwrite the
validated Windows binary.

## Packaging

After validation run `python scripts/package_release.py`. It creates
`release/DS2_Crafting_Equipment_Overhaul_v1.7.0.zip` containing the single ASI,
INI, English/German instructions, changelog and SHA256SUMS. Weapon texture
payloads are embedded in the ASI; no extra runtime ASIs or texture files are
needed. Keep the exact uploaded artifact's checksum.

This standalone Nexus package does not update the separate DS2 Mod Suite
installer or its embedded catalog. Existing Mod Suite behavior remains at its
published version.
