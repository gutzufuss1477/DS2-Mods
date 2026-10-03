# Jump Ramp Unlimited

Keep chaining aerial tricks after launching from a Jump Ramp in **DEATH STRANDING 2: ON THE BEACH**. Continue pressing the normal jump/trick input while airborne to repeat the final available trick and travel much farther.

**Version 1.0.0** · [Download ZIP](release/DS2_Jump_Ramp_Unlimited_v1.0.0.zip) · [Deutsch](README_DE.md)

## Requirements

- PC Steam executable **1.10.89.0**, 64-bit.
- A compatible **x64 ASI loader**, such as [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).
- Access to a Jump Ramp in the game.

The loader is a separate requirement and is not included in this download. The mod checks the supported executable identity and the relevant game instructions before installing its hook. A game update may require a mod update.

## Installation

1. Fully close DS2.
2. Install an x64 ASI loader according to its instructions if you do not already have one.
3. Extract **ds2_jump_ramp_unlimited.asi** and **ds2_jump_ramp_unlimited.ini** beside **DS2.exe**.
4. Start the game normally. The mod is enabled by default.

The ZIP contains exactly those two files. Steam's **Browse local files** option opens the game installation folder.

**Upgrading from the private test version:** remove `ds2_jump_ramp_probe.asi` and its old INI before installing 1.0. The release has new filenames; do not load both versions together.

## How to use

Launch from a Jump Ramp and keep using the normal jump/trick input while airborne. Once the normal sequence reaches its limit, you can repeat its final available trick. No new keybind is required.

Choose a ramp with enough height or a clear drop ahead when trying it for the first time. Landing ends the opportunity to continue the chain. The game's input timing, action checks and trick cooldown still apply, so holding a button or pressing too early does not guarantee another trick.

## Configuration and removal

```ini
[JumpRampUnlimited]
Enabled=1
```

Use `Enabled=0` to disable the mod. **Restart DS2 after changing the INI.** The only supported values are `0` and `1`; invalid values leave the mod inactive. A missing INI or missing setting defaults to enabled.

To uninstall, close DS2 and remove the two mod files. You may also remove the generated `ds2_jump_ramp_unlimited.log`. Keep your ASI loader if other mods use it. The mod applies its change in memory; it does not edit the executable, game archives or save files on disk.

## Troubleshooting and compatibility

- **Still limited to the normal tricks:** check the generated log for `ACTIVE`, confirm the ASI loader works and try a ramp with more airtime.
- **No log:** check the loader and make sure the ASI is beside `DS2.exe`.
- **Unsupported build / changed instructions:** check the game version and mods that alter Jump Ramp trick logic. The mod refuses the checked conflicts rather than replacing those instructions.
- **Old probe detected:** close DS2, remove the private test ASI, then restart.

Other game builds, storefronts and combinations with other ramp mods are not verified. Compatibility checks cover the listed code locations, not every possible interaction.

## Validation

The cap-aware repeat logic was confirmed in gameplay in the private v0.6-test5 build, with many consecutive tricks over a long distance. Version 1.0 removes probe telemetry and live INI reloading, adds startup checks and uses final filenames and version metadata.

The 1.0 build passed **24,576 native dispatcher cases**, **2,000 repeat cycles**, signature/build rejection checks and INI parser checks. Engine helper calls are test doubles in this automated test. The final 1.0 binary and the cap-3 path have not separately been confirmed in a live game session. See [validation details](docs/VALIDATION.md).

## Build

On Windows x64, install LLVM (`clang-cl`, `lld-link`), the Windows SDK resource compiler, and Python 3.10 or newer. No third-party Python packages are required.

```powershell
.\scripts\build.ps1
.\scripts\package-release.ps1 -GameExe 'D:\SteamLibrary\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe'
```

Packaging builds twice, checks reproducibility and PE version metadata, runs the native verification against a locally installed supported executable, then creates the two-file ZIP, manifest and SHA-256 list. It does not start the game or alter the installation. `-LlvmBin`, `-ResourceCompiler` and `-Python` support non-default tool locations.

See [technical notes](docs/TECHNICAL.md), [changelog](CHANGELOG.md) and [Nexus upload notes](NEXUS_UPLOAD.md).
