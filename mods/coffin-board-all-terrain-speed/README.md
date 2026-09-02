# DS2 Coffin Board All-Terrain Speed v1.0.0

Build-locked ASI mod for **DEATH STRANDING 2: ON THE BEACH**, Steam PC
`1.10.89.0`.

The mod gives the Coffin Board the same raised speed limits on land and water
and adds the tested acceleration multiplier. The final profile deliberately
does not alter steering, wet grip, generic vehicle handling, archives, or save
files. Diagnostic telemetry is disabled.

After the Coffin resource patch completes, the worker removes its native
streaming listener immediately and then exits. If the target does not appear,
the listener is removed after a bounded 60-second discovery window. This keeps
the mod out of DS2's later streaming shutdown.

## Installation

The release archive contains exactly:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Copy both files beside `DS2.exe`. An external x64 ASI loader is required.
Replace both files from any earlier test version, fully restart the game, and
deploy a newly created Coffin Board because an existing physics component can
retain its old gear table.

## Configuration

The complete release INI is:

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400
```

`SpeedPercent=500` derives normal/boost caps of `300/400 km/h` on both land
and water and changes the final gearing from `15` to `3`. Accepted values are
`100-1000`.

`AccelerationPercent=400` publishes 20x drive force to compensate the 0.2x
gear ratio and then provide nominally 4x native effective drive moment.
Accepted values are `100-500`.

Missing optional keys resolve to native/inactive values: steering `100%`, wet
grip `100%`, and telemetry `0`. The final no-telemetry drive trampoline has no
velocity sampling load/store, and no steering hook is installed.

## Safety and validation

The ASI accepts only the validated Steam executable. It requires exact PE
metadata, the StreamingManager anchor, the Coffin resource identity/layout,
the drive-hook signature, and the Coffin physics vtable. The drive hook filters
every update by that exact Coffin vtable, so other vehicles remain untouched.

The resource transaction changes only the four speed caps, final gearing, and
the slip threshold needed for the raised speed range. Drive force remains
neutral until every resource write succeeds; failures roll back completed
writes. During the short discovery window, an exact resource unload still
neutralizes the factor. After a successful one-shot removal there is
intentionally no listener left to follow a later resource unload/reload. For
this release, restart DS2 before deploying a replacement board after a region
transition that may have recreated the Coffin physics resource.

Synthetic runtime tests cover the final speed-only transaction, preservation
of native steering and wet grip, absence of steering/telemetry activation,
the dedicated no-telemetry trampoline, rollback, idempotence, and unload
neutralization. Lifecycle tests additionally verify the exact native Add/Remove
slots, refusal of a mismatched Remove slot, callback-to-worker publication,
single removal outside callbacks, idempotent cleanup, and the bounded no-target
path. `tools/validate_target.py` independently checks the supported game
executable and exact runtime anchors.

Do not unload the ASI while the game is running. To remove it, close the game,
delete both mod files, and restart.
