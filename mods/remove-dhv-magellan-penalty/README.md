# Remove DHV Magellan Penalty — v1.1.0

**Preserve normal delivery Likes and the game's earned rank when cargo travels aboard the DHV Magellan.** This mod removes the transport-specific evaluation penalty; it does **not** award maximum Likes, force S/Legend grades, or bypass ordinary delivery requirements.

[Latest downloads on Nexus Mods](https://www.nexusmods.com/deathstranding2onthebeach/mods/63)

## What changes

- Restores the original reward-selector path for affected Magellan deliveries. Normal Likes are calculated by the game.
- Prevents the Magellan-specific evaluation override from replacing the native rank in both the results screen and recorded Standard Order best-rank history.
- Keeps the game's own criteria: cargo condition, delivered quantity, time limits, order type, selected difficulty, and other normal scoring logic.
- Enables an earned S/Legend grade to be recorded like a normal delivery. **No automatic S grade or direct achievement manipulation.**
- Leaves non-Magellan deliveries alone. Removal of the ASI restores vanilla behavior on the next launch; ranks already legitimately recorded in the save are not retroactively reverted.

## Requirements

- **Steam PC** Death Stranding 2: On the Beach, **DS2.exe 1.10.89.0** (exact executable build).
- Windows x64 and a working **x64 Ultimate ASI Loader** or equivalent (not included).

Unsupported builds fail closed: the mod checks the executable metadata and all hook signatures before enabling.

## Install / upgrade

1. Completely exit DS2.
2. Extract `ds2_no_magellan_evaluation.asi` and `ds2_no_magellan_evaluation.ini` into the game directory beside `DS2.exe`, where your x64 ASI loader loads ASIs.
3. Replace the old files if upgrading, and start the game.

This is a copy-only release. No installer, game executable replacement or asset modification is needed. To uninstall, exit the game, remove these two files, then restart.

## Configuration

```ini
[NoMagellanEvaluation]
Enabled=1
RemoveLikePenalty=1
RemoveMagellanRank=1
DebugLog=0
```

- `RemoveLikePenalty=1`: normalize Magellan Likes without adding a fixed amount.
- `RemoveMagellanRank=1`: preserve the game's native delivery evaluation and recorded rank.
- `DebugLog=1`: write `ds2_no_magellan_evaluation.log` for support.
- `Enabled=0`: disable all changes.

## Validation and scope

Confirmed on **Steam DS2.exe 1.10.89.0**, Standard Order **310**, carried via the DHV Magellan. The game evaluated the delivery with its native rank value 5 (Legend/S on the tested difficulty). The saved best-grade mask changed from the Magellan-only value `0x01` to the normal grade `0x20`, and the gold badge appeared in the Orders menu. The Likes correction was also exercised.

The same code handles valid native selectors 1–8, but **other selectors (including 7), different order categories, other difficulty settings, and Steam achievement progression have not all been independently tested**. A qualifying order still needs to meet the game's S-rank requirements. Savegames should be backed up before changing mods; other ASIs hooking the same game code may conflict.

See [Release notes](CHANGELOG.md), [technical design](source/docs/TECHNICAL.md), [validation](source/docs/VALIDATION.md), and [source/build instructions](source/BUILD.md).

## Credits

Thanks to the DS2 modding community and the player who reported an unhandled selector in a Magellan delivery. Original third-party rights and licensing details remain in `source/THIRD_PARTY_NOTICES.txt`.
