# Validation record — v1.0.0

## Exact target

- Steam version: `1.10.89.0`
- PE timestamp: `0x6A3DAE46`
- image size: `0x0B292000`
- image base: `0x140000000`
- executable SHA-256:
  `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- final v1.0.0 ASI SHA-256:
  `074150110ACE040F1F6D57E0C761E2FA5A39FBCD3E26305130C4ECE4CB595672`
- final v1.0.0 INI SHA-256:
  `18B2B8C4751964F05A6CE0538548D985AC828D97CB24102928A6926834F26228`
- deterministic v1.0.0 ZIP SHA-256:
  `D0F3986F9CD435FB98CFBFAC75C1B29524D62508A4CF514313DD2F0A5CEC779F`

The exact-target validator passes against the installed `ds2.exe`. It checks
PE identity, all semantic regions used by this build, and the relevant VTables.
The new foundation evidence includes:

- native level-1 foundation entry into lifecycle state 2;
- Player selection between foundation state 2 and Active state 3;
- the exact flags-0 secondary Update context and return address;
- streaming attach transition from state 2 to 3;
- object byte `+0x3C8` gating the secondary `+0x110` call;
- the lifecycle transition implementation itself.
- fresh PCC descriptor seeding of pending byte `+0x95 = 1`;
- Init copying descriptor `+0x95` to object build-pending `+0x3D1`;
- native `+0x3D1` handling through timer `+0x224 >= 0.1f` and the exact
  `0.1f` literal.

Existing checks continue to cover Init, primary Update, material-driven
SetLevel and return address, level assignment, Completion/Customizable
equality, EffectiveMax, descriptor/net evidence, dirty/pending fields,
callback queues and drains, terminal acknowledgement and asynchronous Applied
acknowledgements. High-risk byte sequences required to be unique in `.text`
are also unique.

## Hook and metadata targets

- Manager, common facility and all 13 canonical Player config pointers match.
- Native tuples match: ordinary `3 / 1 / 2`; Safe House and bridges
  `4 / 2 / 3`.
- Eleven object classes match their expected VTables, including real
  `DSCatapult`.
- All class-specific Init `+0xE0`, common Update `+0x140`, and class-specific
  SetLevel `+0x238` slots match: 33 aligned atomic hook slots.
- Installer conflict handling and reverse rollback remain enabled.

## Synthetic native runtime test

The test DLL compiles with `/W4 /WX` and passes. Coverage includes:

- descriptor remains unchanged through original Init;
- Player/config/class/metadata eligibility and fail-closed exclusions;
- ordinary secondary dispatch cannot arm or call Max;
- ordinary stable primary dwell requests one direct `1 -> 3` transition;
- no second transition occurs before Current/Applied and all pending gates
  acknowledge the first;
- Safe House/bridge state-2 secondary dwell requests exact Completion 2;
- the fresh native state `+0x3D1 = 1`, timer below `0.1f` remains blocked;
- the regression state `+0x3D1 = 1`, timer at least `0.1f` completes the
  two-poll `1 -> 2` transition for Safe House and all three bridge indices;
- original Update synchronously re-entering `SetLevel(1)` while Current is
  already 1 no longer erases the foundation `CANDIDATE`; after the equal-level
  native call, the normal two-poll path still requests Completion 2;
- exact material normalization `2 -> 1` in foundation lifecycle state 2 remains
  native without erasing `CANDIDATE`, and the following two-poll path still
  requests Completion 2;
- with diagnostic mode enabled, foundation diagnostics distinguish Update
  entry (`point=0`) from post-native Update (`point=1`), retain `markerFlags`,
  stored/live IDs and VTable RVA, and expose failed runtime metadata validation
  through blocker bit 11;
- foundation progression remains blocked by callback, `+0x3C8`, material,
  level and auxiliary pending states;
- a state-2 to state-3 change between the two dwell polls retains liveness and
  completes through the exact primary dispatch;
- a foundation first observed at Active level 1 also reaches Completion;
- WAIT_ACK at level 2 does not request Max until Active acknowledgement;
- acknowledged Completion then requests one direct `2 -> 4` transition;
- material level-1 rollback is suppressed immediately after gifted Completion,
  at confirmed Max, and after a saved Max object is reinitialized;
- exact material rollback protection does not block level-0 teardown;
- non-material downgrade and unmarked objects remain native;
- synchronous Update re-entry cannot acquire the marker and overlap SetLevel;
- a material SetLevel re-entry during the locked native call observes the
  atomic in-progress reservation and cannot roll the promoted level back;
- pointer reuse clears stale state and independent objects remain independent;
- saved ordinary Max/intermediate levels resume the intended marker phases.

## Package

The deterministic packaging check requires exactly:

- `ds2_construction_max_level_on_build.asi`
- `ds2_construction_max_level_on_build.ini`

Two consecutive package builds produced the same ASI, ZIP, and sidecar hashes:

- ASI size: `33792` bytes
- INI size: `131` bytes
- ZIP size: `17252` bytes
- deterministic rebuild result: `PASS`

## Confirmed gameplay validation

The v0.1.5 state machine is promoted to stable v1.0.0 based on these in-game
results:

- ordinary constructions build normally and reach Max through one native
  direct upgrade; the confirmed set explicitly includes the
  Transponder/Fast Travel Station and Cargo Catapult;
- Safe House/Bunker and normal bridges build visibly instead of remaining at
  `TRACKED` or becoming invisible;
- each of those foundation constructions performs exactly two native upgrades,
  `1 -> Completion (2) -> Max (4)`.

The two foundation presentations are the intended stable behavior. Their exact
Completion milestone must be acknowledged before Max. UI/callback suppression
remains deliberately excluded because it would risk reintroducing invisible or
stuck objects.

Detailed `DIAG point=0/1` foundation samples are production-debug output and
require `DebugLog=1` in the INI. They are not emitted with the default
`DebugLog=0` configuration.
