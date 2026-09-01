# DS2 Construction Max Level on Build v1.0.0

Stable, build-locked ASI mod for **DEATH STRANDING 2: ON THE BEACH** Steam PC
`1.10.89.0`.

Fresh Player PCC constructions are created visibly by the normal game flow.
The mod then supplies the missing levels through the original class-specific
level functions, without upgrade materials or manual upgrade actions.

## Confirmed gameplay behavior

- Ordinary constructions, including the Transponder/Fast Travel Station and
  Cargo Catapult, are built normally and then perform one native upgrade from
  level 1 directly to Max level 3.
- Safe House/Bunker and normal bridges are built visibly and then perform
  exactly two native upgrades: `1 -> Completion (2) -> Max (4)`.

The two presentations for Safe House and normal bridges are intentional. Their
class-specific Completion step creates and acknowledges the required
foundation state before the Max step. Removing that native transition is not
safe: visual completion, level acknowledgement, and construction events share
the same processing path.

## Supported constructions

The stable release recognizes 13 canonical Player configurations:

- Postbox, Safe House, Watchtower, Cargo Catapult, and Generator
- Rain Shelter, Zipline, Chiral Bridge, and Jump Ramp
- normal bridges in 30 m, 45 m, and 80 m variants
- Transponder/Fast Travel Station

Foreign online constructions (`Net`) and story/world constructions (`Stage`)
are not changed. Road Rebuilders and Mines are Stage infrastructure in the
shipped data. Monorail has no separate internal upgrade level.

## Safety model

The mod does not edit the spawn descriptor, Current/Applied level fields,
material tables, archives, or save files directly. Before each gifted level
step it requires the exact Player config and object class, an approved native
Update caller, the matching lifecycle, equal Current/Applied levels, no queued
callbacks, and clear proven pending/dirty flags.

Safe House and bridge foundations use their native timer-qualified build state
and first request exact Completion level 2. Only after native acknowledgement
and transition to Active do they request Max level 4. A real level-0 dismantle
always remains native. Exact material-ledger rollbacks are suppressed only for
the verified gifted-level cases.

The executable identity, semantic code regions, VTables, config pointers,
class metadata, and level tuples are validated fail-closed. If the installed
game build does not match, no hooks are installed.

## Installation

1. Close the game completely.
2. Back up the save used for the first run.
3. Place `ds2_construction_max_level_on_build.asi` and
   `ds2_construction_max_level_on_build.ini` beside `DS2.exe`.
4. Start the game through an external 64-bit ASI loader.

Never replace or unload the ASI while the game is running. To remove the mod,
close the game, delete both mod files, and restart. Levels already stored by
the game are not reverted by removing the files.

TEST v0.1.1 remains withdrawn. A construction already saved inside that old,
broken transaction cannot be repaired reliably merely by replacing the ASI;
load a save from before the affected construction.

## Logging and diagnostics

With the production default `DebugLog=0`, the log retains concise lifecycle,
completion, and error information. Detailed foundation diagnostics are opt-in:
set `DebugLog=1` in the INI before starting the game to emit bounded `DIAG`
samples at Update entry (`point=0`) and after native Update (`point=1`). These
samples include stored/live construction IDs, VTable RVA, marker flags, caller,
lifecycle, timer, and pending state. Blocker bit 11 indicates failed runtime
metadata validation. The Update hook itself performs no file I/O.

## TEST release history

- **v0.1.1** attempted a synchronous integer-level ladder and was withdrawn
  after it could leave an invisible object and a stuck build transaction.
- **v0.1.2** serialized individual native level calls, fixing the transaction
  overlap but replaying every intermediate presentation.
- **v0.1.3** introduced direct native milestone requests: ordinary
  `1 -> Max`, foundation `1 -> Completion -> Max`.
- **v0.1.4** matched the native foundation timer rule while build-pending is
  still set, but an equal-level native `SetLevel(1)` could still erase the
  tracking marker.
- **v0.1.5** corrected marker lifetime for equal-level refreshes and the exact
  lifecycle-2 material normalization, and added entry/post foundation
  diagnostics.
- **v1.0.0** promotes that state machine to stable after gameplay confirmed
  ordinary constructions and visible Safe House/Bunker and bridge completion,
  including exactly two native foundation upgrades to Max.

See `docs/TECHNICAL_NOTES.md` and `docs/VALIDATION.md` for the exact target,
state machine, regression coverage, and release verification record.
