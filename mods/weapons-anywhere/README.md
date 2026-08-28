# DS2 Weapons Anywhere v1.0.0

ASI mod for **DEATH STRANDING 2: ON THE BEACH** that removes the technical weapon lock inside the game's weapons-prohibited areas while leaving the separate prohibited-area presentation path unchanged.

## Intended behaviour

- Weapons and weapon-type equipment remain usable near facilities and other buildings that normally activate a weapons-prohibited area.
- The native warning ring, announcement and related visual/audio feedback remain unchanged.
- No save data, structure data or original game archive is edited.

Version 1.0.0 has been confirmed in game with firearms and grenades inside a facility/building zone. Melee, specialized equipment and vehicle-mounted weapons have not each been validated as a complete matrix and remain listed in the included regression plan.

## Validated executable target

- Platform: Steam PC
- Game version: `1.10.89.0`
- PE timestamp: `0x6A3DAE46`
- Image size: `0x0B292000`
- DS2.exe SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`

The ASI validates both the build metadata and a unique 22-byte instruction signature. A mismatch disables the patch for that session.

## Installation

1. Use one working external x64 ASI loader for DS2.
2. Copy `ds2_weapons_anywhere.asi` and `ds2_weapons_anywhere.ini` beside `DS2.exe` or into the directory used by the loader.
3. Restart the game.

Uninstall by removing both files and restarting the game.

## Configuration

```ini
[WeaponsAnywhere]
Enabled=1
DebugLog=0
```

Set `Enabled=0` to leave the game code untouched. Set `DebugLog=1` to create `ds2_weapons_anywhere.log` beside the ASI during startup. With normal logging disabled, the ASI still makes a best-effort attempt to create this file if a memory-protection rollback is ever incomplete.

## Technical boundary

The patch changes only the branch opcode at `DS2.exe+0xFA4255` from `je` to `jmp`. The native function still checks the current zone and resolves `DSWeaponParameter::IsApplyWeaponsProhibitedArea`, but it skips only the immediate `return 6` weapon-prohibition reason. Later return reasons remain native. The separate presentation path for the prohibited ring, announcement and On/Off sounds is not patched.

See `docs/TECHNICAL_NOTES.md`, `docs/VALIDATION.md` and `docs/TEST_PLAN.md` for the exact validated scope and regression coverage.
