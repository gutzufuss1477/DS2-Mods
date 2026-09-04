# Technical notes — v1.0.1

Validated target:

- Steam version: `1.10.89.0`
- PE timestamp: `0x6A3DAE46`
- image size: `0x0B292000`
- image base: `0x140000000`
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`

## Revision rationale

TEST v0.1.1 was withdrawn because it started a synchronous integer-level
ladder while the game acknowledged each visual level asynchronously. That
could leave an invisible object and a stuck construction transaction.

TEST v0.1.2 serialized every `SetLevel(current + 1)` call behind a complete
native acknowledgement. Gameplay confirmed that ordinary constructions then
reached Max, but each integer level replayed a full presentation. Safe House
and normal bridges stayed at level 1 because their initial foundation remains
in lifecycle state 2 and receives the common Update from a separate caller.

TEST v0.1.3 keeps the acknowledged state machine but uses native milestones:

- ordinary tuple `3 / 1 / 2`: stable level 1 directly requests Max 3;
- Safe House/bridge tuple `4 / 2 / 3`: foundation level 1 first requests exact
  Completion 2, waits for acknowledgement and Active state, then requests Max
  4.

Gameplay confirmed its ordinary path, including Cargo Catapult, but Safe House
and normal bridges still remained in `TRACKED` at level 1. The synthetic test
had hidden the cause by setting object build-pending `+0x3D1` to zero.

TEST v0.1.4 corrects that one foundation predicate. Fresh PCC creation writes
descriptor bytes `+0x94/+0x95 = 1/1` at RVA `0x0C96356`; Base Init copies
descriptor `+0x95` to object `+0x3D1` at RVA `0x127B6AA`. Terminal level ACK
does not clear object `+0x3D1` until RVA `0x1288D81`, so requiring zero before
the first Completion SetLevel was circular. The native foundation path instead
checks `+0x3D1` at RVA `0x12828BC` and, when it is nonzero, waits only until
float timer `+0x224 >= 0.1f`. v0.1.4 uses that same predicate while retaining
the two-poll dwell and every other gate.

Gameplay with v0.1.4 still left Safe House and normal bridges at `TRACKED`,
level 1, and produced no foundation `DIAG` line. Because that recorder existed
only after original Update, the log cannot distinguish marker loss inside or
before Update from a pre-hook Entity gate suppressing the Update call. A static
audit found no object-pointer, construction-ID, or VTable replacement between
Init and the foundation Update. It did find that the v0.1.4 SetLevel wrapper
cleared any tracked request below Completion, regardless of whether the
requested level was actually below Current. That marker-lifetime hole was
therefore silent whenever it occurred.

TEST v0.1.5 makes the general removal predicate exact: a tracked
below-Completion request clears its marker only when `requested < current` as
well as `requested < Completion`. Equal-level `SetLevel(1)` remains fully
native but does not discard the marker. A second narrow exception retains a
`CANDIDATE` foundation marker when the exact material caller normalizes a
positive value to level 1 in lifecycle state 2. Level-0 teardown and positive
downgrades outside that setup state retain their native/fail-closed behavior.
The foundation timer predicate and acknowledged state machine are unchanged.

The synthetic regressions invoke `SetLevel(1)` synchronously from the original
Update stub while Current is 1 and separately exercise a lifecycle-2 material
normalization from Current 2 to 1. They verify that the native calls still
occur, the marker remains `CANDIDATE`,
and the following two stable foundation polls request Completion 2.

Stable v1.0.0 retains the v0.1.5 state machine and promotes it after gameplay
confirmation. Ordinary constructions, explicitly including the
Transponder/Fast Travel Station and Cargo Catapult, build normally and perform
one native `1 -> Max (3)` upgrade. Safe House/Bunker and normal bridges build
visibly and then perform exactly two native upgrades,
`1 -> Completion (2) -> Max (4)`. Production builds default to
`DebugLog=0`; the bounded foundation entry/post diagnostic sampler runs only
when `DebugLog=1` is selected in the INI.

v1.0.1 retains that state machine but removes stable and unrelated objects
from its validated slow path. v1.0.0 acquired up to eight marker locks for
every recognized construction Update. A saved-Max `COMPLETE` marker then
repeated object/config metadata and gate validation on every Update. That work
scaled with construction density in mature saves.

The v1.0.1 fast reject uses aligned atomic loads of the fixed marker fields.
An unrelated object performs no marker lock and no memory-region validation.
A `COMPLETE` object whose live Current level still equals its stored expected
Max level also returns without either operation. If the levels differ, it
falls through to the original fully validated cleanup path, so stale markers
do not retain downgrade protection. SetLevel uses the same lock-free presence
filter before its infrequent validated marker handling.

No spawn descriptor, Current, Applied, config level, archive, material table,
or save field is directly patched.

## Why descriptor seeding and silent apply are rejected

Base Init at RVA `0x127B340` copies descriptor `InitialLevel +0x70` to object
`CurrentLevel +0x3F5` and `AppliedLevel +0x3F6`, but also rehydrates many
dependent descriptor flags and material structures. Mutating only `+0x70`
would not prove a coherent fresh Max construction and would skip Safe House's
class-specific Completion behavior.

Native SetLevel starts the same apply/callback chain that creates visual state
and terminal acknowledgement. Suppressing that chain cannot be separated
safely with the available static evidence. The stable implementation uses one
presentation for ordinary objects and exactly two for Safe House/normal
bridges; it does not attempt a silent write.

## Metadata and direct Max request

Config `+0x4E8` is the exact-equality Completion milestone and `+0x4EC` is the
exact-equality Customizable milestone. Base SetLevel checks them at RVAs
`0x12875F5` and `0x12876A8` after assigning Current.

The material calculator at RVA `0x128DBD0` walks cumulative funding and can
return the highest funded target level. Common Update passes that result
unchanged through the sole construction-range virtual SetLevel call at RVA
`0x1280A06`. This is the native structural evidence for a non-adjacent Max
request.

The tradeoff is explicit: `Completion -> Max` can skip exact equality with
Customizable. The two Customizable message calls at RVA `0x1287706` and
`0x1287715` have no later fallback. Other inspected consumers test
`Current >= Customizable`, so a Max object remains threshold-qualified.
Customization UI, default cosmetics, and individual level bonuses remain game
features outside the mod's level-transition contract.

## Hooked classes and installation

The eleven real object classes expose:

- class-specific Init at VTable slot `+0xE0`;
- common Update at slot `+0x140`, RVA `0x1280950`;
- class-specific SetLevel at slot `+0x238`.

SetLevel, Init and Update slots are installed with aligned 64-bit atomic
compare/exchange after every expected original pointer is validated. Any
conflict rolls back already-installed slots in reverse order. Safe House keeps
SetLevel RVA `0x13084A0`, whose exact Completion branch calls RVA `0x1286680`.
Bridge keeps SetLevel RVA `0x130E3C0`, which calls base SetLevel and always runs
the bridge-specific RVA `0x1310B00` tail.

The Init hook calls original Init with the descriptor unchanged. It claims only
an exact published canonical config, exact object class, `NetType == Player`,
expected native level tuple, valid construction ID and
`0 < InitialLevel <= EffectiveMax`. It verifies native Current/Applied after
Init before storing a marker. A saved Max object starts in `COMPLETE`; a saved
intermediate object resumes in `CANDIDATE`. Objects initialized before config
publication are intentionally not claimed retroactively.

## Exact Update dispatches

The hook classifies the call site, calls original common Update, and only then
advances the acknowledged gift state machine. With `DebugLog=1`, it also
captures an entry diagnostic (`point=0`) before the native call and the
resulting state (`point=1`) after it. It classifies only:

- primary: `updateFlags == 1`, exact return RVA `0x1281294`;
- foundation secondary: `updateFlags == 0`, exact return RVA `0x12825F6`.

Every other caller is forwarded without changing gift state. Normal
Completion-to-Max arming and calling are explicitly primary-only.

For the two recognized callers, the marker lookup is a non-mutating fast
filter before the validated state machine. Only `CANDIDATE`, `ARMED`,
`WAIT_ACK`, or an inconsistent `COMPLETE` marker enters runtime metadata and
gate validation. Unrelated and consistent `COMPLETE` objects do not acquire a
marker lock in this path.

Safe House and Bridge VTables use RVA `0x1282570` at slot `+0x110`; that path
invokes common Update with flags 0. Its streaming caller checks object byte
`+0x3C8 == 0` before the `+0x110` call at RVA `0x13764D0`. Native streaming
attach can change lifecycle 2 to 3 at RVAs `0x1376222..0x137622A`, so the
foundation predicate accepts either:

- exact secondary caller with lifecycle 2 and `+0x3C8 == 0`; or
- exact primary caller with lifecycle 3.

Both variants still require the supported Safe House/bridge target, tuple
`4 / 2 / 3`, `Current == Applied == 1`, two zero callback-count reads,
material dirty `+0x3CB == 0`, level dirty `+0x3E6 == 0`, and auxiliary pending
`+0x3EE == 0`. Build-pending `+0x3D1` is accepted only when it is zero or the
finite, non-negative timer bits at `+0x224` represent at least `0.1f`. Timer
values below the threshold, negative values, infinity, and NaN fail closed. A
lifecycle change between the two dwell polls does not discard the marker; the
next matching exact dispatch may perform the one Completion call.

## Acknowledged state machine

```text
ordinary:
  CANDIDATE --stable Active primary--> ARMED
  ARMED     --later stable primary---> SetLevel(Max), WAIT_ACK

Safe House / normal bridge foundation:
  CANDIDATE --stable matching state-2/3 dispatch--> ARMED
  ARMED     --later matching dispatch------------> SetLevel(Completion=2), WAIT_ACK
  WAIT_ACK  --native Current/Applied=2 and Active-> CANDIDATE
  CANDIDATE -> ARMED -> SetLevel(Max=4), WAIT_ACK

all:
  WAIT_ACK --stable acknowledged Max--> COMPLETE
```

Stable Active requires lifecycle `+0x34 == 3`, Current `+0x3F5` equal Applied
`+0x3F6`, material dirty `+0x3CB == 0`, build pending `+0x3D1 == 0`, level
dirty `+0x3E6 == 0`, auxiliary build pending `+0x3EE == 0`, and two reads of
deferred callback count `+0x2E8` both zero. The two reads protect against a
concurrent queue change. Byte `+0x3B7` is deliberately not a gate because no
matching clear is proven in the level acknowledgement path.

The marker lock remains held across exactly one original SetLevel call. A
synchronous nested Update therefore cannot start another transition. WAIT_ACK
issues no SetLevel. Level divergence, class/config mismatch, removal or object
reuse clears tracking fail-closed; there is no timeout-driven overlapping retry.
An atomic `nativeCallInProgress` reservation is published before the original
call and remains set through its immediate Current-level validation. If the
exact material SetLevel caller re-enters during that promotion window, a
positive rollback below the newly assigned Current level is suppressed without
acquiring the marker lock; level 0 remains native.

## Foundation material preservation and dismantle

The exact material recompute caller returns at RVA `0x1280A0D`. Ordinary
`COMPLETE` objects suppress only that caller's positive downgrade below current
while they remain Active at confirmed Max.

A gifted Safe House/bridge Completion must additionally survive the material
ledger still calculating foundation level 1. Its marker therefore carries a
foundation-preserved flag as soon as `SetLevel(2)` is started, and restores that
flag for a saved/reloaded supported object initialized at level 2 or above.
While the marked object remains at or above Completion, the exact material
caller cannot apply a positive lower level, including during WAIT_ACK.

Requested level 0 is never shielded. It remains native and clears the marker,
as do real non-material downgrades below Completion. An equal-current request
does not clear the marker. During foundation lifecycle 2 only, an exact
material-caller normalization to positive level 1 also remains native without
discarding a `CANDIDATE`; after gifted Completion the existing preservation
flag suppresses the known automatic `2/4 -> 1` material rollback.

## Tracking storage

The fixed 4096-entry table uses at most eight probes and a non-blocking
per-entry compare/exchange lock. Markers identify pointer, construction ID and
target index. No allocation, sleep or unbounded wait occurs inside Init,
Update or SetLevel. Pointer reinitialization clears the previous marker first;
table contention or saturation skips gifting rather than touching an
unverified object.

Marker keys and fast-filter fields use aligned acquire/release operations. A
claim invalidates an existing same-pointer key before replacing metadata and
publishes the object key last. Reset invalidates the key first. Readers sample
the key, metadata, and key again; an active mutating match is then revalidated
under the entry lock. This keeps the steady-state reject lock-free without
using a mixed marker snapshot.

A separate fixed diagnostic bank is available only for the four foundation
targets and is inactive at the production default `DebugLog=0`. When
`DebugLog=1`, it keeps at most eight distinct snapshots per target and uses a
non-blocking compare/exchange lock. Each relevant Update can contribute an
entry sample before original Update (`point=0`) and a post-native sample
(`point=1`). This makes marker or state changes caused inside original Update
visible instead of leaving only a silent post-call miss. `markerFlags` records
the marker's preservation bits at that sample point; stored/live construction
IDs and the object VTable RVA are logged as additional lifetime evidence. The
Update hook records only memory fields; the one-second worker drains new
samples to `DIAG` log lines. Timer motion is deduplicated by its
below/above-threshold blocker state so early frames cannot fill the bank with
near-identical samples. There is no allocation or file I/O in the hook. If all
eight distinct slots are consumed, the worker emits one bounded-limit warning
and later distinct states may be omitted.

The hexadecimal `blockers` mask in each `DIAG` line uses: bit 0 unknown
dispatch, bit 1 lifecycle mismatch, bit 2 secondary blocked, bits 3/4 the two
callback-count reads, bits 5/6 Current/Applied not equal to 1, bit 7 material
dirty, bit 8 level dirty, bit 9 auxiliary pending, bit 10 build-pending before
the 0.1-second dwell, and bit 11 failed runtime metadata validation. `B=1`
without bit 10 is the expected fresh-foundation state and is no longer a
blocker. Unlike v0.1.4, a runtime metadata failure is retained as an explicit
diagnostic state instead of silently discarding the sample.

The flags-0 call is produced by the repeated `MsgEntityConstantUpdate` handler
at RVA `0x1376440`, not by Init. Before it reaches construction slot `+0x110`,
the native handler checks its component/object attachment, resource/entity bit
9, object `+0x3C8`, and a global-bit-15/object-`+0x3DE` condition; the message
producer also gates constant updates on entity `+0x309`. Stable v1.0.1 does
not bypass those native gates. With `DebugLog=1`, `TRACKED` followed by no
`point=0` sample identifies a pre-hook Entity boundary rather than another
silent post-Update state.

## Canonical runtime targets

| Construction | Manager offset | Native Max / Completion / Customizable |
|---|---:|---:|
| Postbox | `+0x38` | `3 / 1 / 2` |
| Safe House | `+0x30` | `4 / 2 / 3` |
| Watchtower | `+0x40` | `3 / 1 / 2` |
| Cargo Catapult | `+0x48` | `3 / 1 / 2` |
| Generator | `+0x50` | `3 / 1 / 2` |
| Rain Shelter | `+0x58` | `3 / 1 / 2` |
| Zipline | `+0x68` | `3 / 1 / 2` |
| Bridge 30 m | `+0x80` (alias `+0x138`) | `4 / 2 / 3` |
| Bridge 45 m | `+0x140` | `4 / 2 / 3` |
| Bridge 80 m | `+0x148` | `4 / 2 / 3` |
| Chiral Bridge | `+0x98` | `3 / 1 / 2` |
| Jump Ramp | `+0xA0` | `3 / 1 / 2` |
| Fast Travel Station | `+0xC0` | `3 / 1 / 2` |

Road Rebuilder and Mining Machine upgrade-capable variants are shipped only as
Stage infrastructure; Player/Net variants are marked unused. Monorail has
`CompletionLevel == MaxLevel` and is excluded.

Hot unload/reload is unsupported because VTables and the monitoring worker are
process-lifetime state. Exit the game before replacing or removing files.
