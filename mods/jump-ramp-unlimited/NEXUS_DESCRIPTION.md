# Jump Ramp Unlimited

**One ramp. A much longer ride.**

Keep the trick chain going after a Jump Ramp launch in **DEATH STRANDING 2: ON THE BEACH**. When you reach the end of the normal aerial trick sequence, Jump Ramp Unlimited lets you repeat the final available trick and cover much greater distances.

## What it does

- Removes the normal limit on chaining Jump Ramp aerial tricks.
- Lets you repeat the final available trick using the game's normal jump/trick input.
- Keeps the game's input timing, action checks and trick cooldown.
- Works automatically once installed and enabled.

You still need enough airtime to continue. Try a ramp facing a clear drop or open stretch, and keep making the normal trick input while airborne. Landing ends the chain. No new keybind is required.

## Requirements

- **DEATH STRANDING 2: ON THE BEACH — PC, Steam version 1.10.89.0.**
- **A compatible x64 ASI loader**, such as [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).
- A Jump Ramp available in your game.

The ASI loader is a separate requirement and is **not included**. Other game builds and storefronts have not been verified. A future game update may require an updated version of this mod.

## Installation

1. Fully close the game.
2. Install an x64 ASI loader if you do not already use one.
3. Extract these two files directly beside **DS2.exe**:
   - `ds2_jump_ramp_unlimited.asi`
   - `ds2_jump_ramp_unlimited.ini`
4. Start the game normally and head to a Jump Ramp.

The mod is **enabled by default**. In Steam, use **Browse local files** to find the game folder.

**Coming from the private test build?** Remove `ds2_jump_ramp_probe.asi` and its old INI before installing 1.0. Do not run the test and release versions together.

## Settings

Open `ds2_jump_ramp_unlimited.ini`:

```ini
[JumpRampUnlimited]
Enabled=1
```

Set **Enabled=1** to enable the mod or **Enabled=0** to disable it. **Restart the game after changing the setting.**

## Compatibility and troubleshooting

The mod checks the supported game build and the code it needs to change. If those checks fail, it stays inactive and writes a message to `ds2_jump_ramp_unlimited.log` beside the ASI.

- **Still getting only the normal tricks?** Check that the log says `ACTIVE`, then try a ramp with more height. You may have landed before another trick could begin.
- **No log file?** Check that the x64 ASI loader is working and the mod files are beside `DS2.exe`.
- **Unsupported build or changed instructions in the log?** Check your game version and any other mod that changes Jump Ramp trick behavior.

Mods that alter the same ramp trick logic may conflict. Compatibility with every other mod has not been tested.

## Uninstall

Close the game and remove `ds2_jump_ramp_unlimited.asi` and `ds2_jump_ramp_unlimited.ini`. You can also delete the generated log. Keep the ASI loader if your other mods need it.

The mod changes the running game's memory. It does not edit your save files, game archives or `DS2.exe` on disk.

## Testing and source

The repeat logic was confirmed in gameplay in the private test build, allowing many consecutive tricks over a long distance. The cleaned-up 1.0 release passed native automated checks; the final release binary has not separately been replayed in game. Full test scope and limits are documented with the source.

[Source code, changelog and validation notes on GitHub](https://github.com/gutzufuss1477/DS2-Mods/tree/main/mods/jump-ramp-unlimited)
