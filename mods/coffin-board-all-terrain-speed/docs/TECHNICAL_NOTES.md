# Technical notes

Validated target metadata:

- Steam version: `1.10.89.0`
- PE timestamp: `0x6A3DAE46`
- image size: `0x0B292000`
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`

Odradek's serialized type data and a full resource scan established these
runtime layouts and native values:

| Type | Offset | Field | Native |
|---|---:|---|---:|
| `DSPhysicsVehicleResource` base | `+0x40` | `SteeringDegree` | 50 |
| `DSPhysicsVehicleResource` base | `+0x44` | `TopSpeedKMH` | 40 |
| `DSPhysicsVehicleResource` base | `+0x48` | `FinalGearRatio` | 15 |
| `DSPhysicsCoffinResource` | `+0x94` | `BoostTopSpeedKMH` | 60 |
| `DSPhysicsCoffinResource` | `+0x98` | `OnWaterTopSpeedKMH` | 60 |
| `DSPhysicsCoffinResource` | `+0x9C` | `OnWaterBoostTopSpeedKMH` | 80 |
| `DSPhysicsVehicleResource` extension | `+0xA4` | `SteeringScaleInHiSpeed` | 1 |
| `DSPhysicsVehicleResource` extension | `+0xA8` | `SteeringScaleInBoost` | 1 |
| `DSPhysicsCoffinResource` | `+0xC8` | wet lateral-grip scalar | 0.09 |
| `DSPhysicsCoffinResource` | `+0xD8` | `SlipSpeedKMH` | 39 |
| `DSPlayerRideCoffinConfig` | `+0x34` | `StandardSpeed` | 40 |

Odradek graph checksum `3b11a80e0c3b582e808cf07fc8266ddc`
places `DSPhysicsCoffinResource` in group `499`, object `71288`, with raw UUID
bytes `87 C3 F0 DC 46 94 4E 6B BA 63 C4 45 59 AD DB 26`. That group also contains an object at
index `87602`, proving a size of at least 87,603 objects. TEST v0.1.0 rejected
every callback with more than 65,536 objects, so it could never patch this
target and instead ran an unbounded generic RTTI scan over later groups.

TEST v0.1.1 and later perform no generic object scan. A callback checks only the exact
graph position and then validates UUID, RTTI type and all native layout anchors.
After listener registration it also queries the already-loaded group table
under the engine's shared SRW lock, because native `AddListener` does not replay
completed groups. The exact native locations are:

| Item | Address/layout |
|---|---:|
| Streaming signature | RVA `0x693674` |
| StreamingManager global | RVA `0x6266938` |
| StreamingSystem vtable | RVA `0x34532A0` |
| `AddListener` | RVA `0x26F6E40` |
| `RemoveListener` | RVA `0x26F6EE0` |
| loaded-group count/capacity/table | system `+0x150288/+0x15028C/+0x150290` |
| group-table stride | `0x21` |
| loaded group's callback array | loaded group `+0x20` |

Exact runtime type names are obtained from the Decima RTTI object (`vtable[0]`,
name at RTTI `+0x40`) only after the UUID matches. In the packaged default only
`DSPhysicsCoffinResource` is required. The v1.0.0 release removes its one-shot
listener on the worker immediately after successful patching, or after a
bounded 60-second discovery window. The unload callback can track and
neutralize the exact graph position only while that short listener window is
still open; later replacement resources intentionally are not re-armed.
`DSPlayerRideCoffinConfig +0x34` is
kept only as an explicitly experimental, disabled option because static
analysis places it in rider unbalance/animation tuning rather than proving it
as a physics cap. If that optional field is enabled, its last value is tracked
so a replacement physics resource can safely raise it to a newly calculated
target without treating the mod's own prior value as a conflict.

Static consumer evidence for `SlipSpeedKMH`: function RVA `0x1F43120` compares
current speed with resource `+0xD8` at RVA `0x1F43159`; above the threshold it
updates four per-wheel slots. Keeping the native 39 km/h threshold with a
60 km/h land cap would activate that path across almost the entire new band,
so the default aligns the threshold with the new normal cap.

The four speed-cap ratio reads are in the Coffin physics vtable function at RVA
`0x1F43410`: water boost/land boost at `0x1F43465/0x1F4346D`, and water
normal/land normal at `0x1F43476/0x1F4347E`. `EngineCurve +0x20` is null in the
serialized Coffin resource. `DSPlayerRideCoffinConfig SpeedRateCoeff +0x38` is
consumed by the rider balance/request path at RVA `0x1EC106A`; it is not proven
to control vehicle acceleration and is intentionally left untouched.

The generic vehicle cap consumer at RVA `0x1F4BF10` reads boost `+0x94` at
`0x1F4BFFF` and normal `+0x44` at `0x1F4C009/0x1F4C037`, converting km/h to
m/s with `0.27777779`.

TEST v0.2.0 adds a narrowly scoped drive-force hook. Generic vehicle update
RVA `0x247A431` calculates `throttle * selectedGearRatio * EngineCurveOutput`
and stores the resulting total drive moment at component `+0xE0`
(`rdi=component+0xC8`, store `[rdi+0x18]`). Its unique 18-byte anchor is:

`C4 C1 4A 59 CC C5 FA 59 C1 C5 FA 11 47 18 C5 E2 5C C6`

The trampoline reproduces the displaced instructions and multiplies the
result only when the component vtable equals RVA `0x33AD0B0`, the exact Coffin
physics component vtable. It preserves `rax`, does not add calls or locks, and
returns at RVA `0x247A43F`. A separate worker thread performs logging.

Before publishing the 14-byte detour, the installer suspends all other process
threads, requires a stable second Toolhelp enumeration, reads each Win64
`CONTEXT`, and retries if any RIP is inside the overwrite range. It restores
page protection and resumes every captured thread before continuing. The hook
is installed before listener registration with a neutral `1.0` factor. If
force or gearing coupling is requested and exact installation fails, the
worker returns before registering a listener, so no resource value changes.

The same generic update computes the component's 3D velocity magnitude,
converts it to km/h and stores it at component `+0x120` at RVA `0x247A744`.
The Coffin-filtered trampoline copies the previous frame's aligned float to a
private telemetry slot. Sampling that slot provides observed 3D speed without
file I/O or synchronization in the physics thread. This is a magnitude, not
signed forward speed: vertical collision/fall velocity can affect a maximum,
and the vtable filter covers every live Coffin instance rather than proving
player ownership. Test comparisons therefore use one board on a level route.

TEST v0.2.1 deliberately uses `FinalGearRatio +0x48` as a separate high-speed
gearing control, not as an acceleration control. Constructor RVA `0x2479554`
derives all five gears as `base * 1.414^n`. Update RVAs `0x247A3DD/0x247A3ED`
use the selected ratio for the RPM/EngineCurve path before the drive-moment
calculation. The v0.2.0 gameplay trace proved the hook and caps were active but
plateaued at `94.25 km/h`; multiplying a near-zero redline curve output cannot
move that ceiling. The v0.2.1 test profile therefore changes the exact native
ratio `15 -> 3` (20%), moving the RPM-limited speed by approximately 5x. Since
the same selected ratio is a linear factor in the drive moment at `0x247A431`,
`DriveForcePercent=500` compensates that nominal 0.2x torque factor. Engine
curve shape, shift behavior and collision/handling remain experimental.

The private aligned factor slot at trampoline `+72` remains `1.0` until
`SteeringDegree`, all land/water caps, `FinalGearRatio`, and `SlipSpeedKMH`
have committed. Publishing
the configured factor is the final transaction step; a failure neutralizes
the slot and rolls every completed resource write back. While the listener is
registered, the exact resource-unload callback neutralizes the slot and
re-arms targeting within the remaining discovery window. Because
the constructor copies `FinalGearRatio` into a five-entry component gear table,
the gameplay check requires a full restart and a newly created/deployed board.

TEST v0.2.2 adds only a guarded steering-angle adjustment. Coffin function RVA
`0x1F41EF0` derives its speed-dependent steering scale from resource `+0xA4`
and `+0xA8`. The normal path interpolates from `1.0` toward `+0xA4` and clamps
the result to at most `1.0`; the boost path uses `+0xA4 * +0xA8`. Both native
values are already `1.0`, so increasing them cannot improve normal high-speed
response and would only create inconsistent boost behavior. They remain
untouched.

The wheel-steering consumer at RVA `0x1F46AD7` instead loads
`SteeringDegree +0x40`. RVA `0x1F46AEE` multiplies it by the exact
`pi/180` constant (`0.0174532923847`), and RVA `0x1F46AF6` multiplies the
result by signed input and the current steering scale before per-wheel writes
begin at RVA `0x1F46B1F`. The packaged `SteeringAnglePercent=130` therefore
changes only the proven angle from `50` to `65` degrees. The write participates
in the existing resource transaction, preserves a native baseline for
idempotent reapplication, and is limited to `100-160%`. Tire friction and
generic input filtering are deliberately unchanged.

TEST v0.2.3 raises the gameplay-test angle to the existing guarded maximum,
`SteeringPercent=160` (`50 -> 80` degrees). It also introduces a compact
profile parser. When `SpeedPercent` exists, it atomically derives both cap
percentages, all-surface scaling, drive force, and inverse gearing
(`gearPercent=floor(10000/speedPercent)`, clamped to `10-100`). Thus `500`
reproduces the validated `300/400 km/h`, `500%` drive, and `20%` gearing
profile with one setting. `SteeringPercent` and `Telemetry` similarly override
their legacy counterparts. Missing new keys retain the complete v0.2.2 parser
path, so old INIs remain compatible.

TEST v0.2.4 separates acceleration from maximum speed. The v0.2.3 compact
profile used `DriveForcePercent=500` with `GearRatioPercent=20`; because the
selected gear ratio is itself a linear drive-moment term, those factors yielded
approximately native effective torque (`5.0 * 0.2 = 1.0`). The new
`AccelerationPercent` multiplies that compensated force. The packaged
`SpeedPercent=500, AccelerationPercent=400` profile therefore publishes
`DriveForcePercent=2000` while retaining `GearRatioPercent=20`, for nominally
four times native effective torque (`20.0 * 0.2 = 4.0`). The guarded advanced
drive-force ceiling is now `5000%`.

The same version adds a second Coffin-filtered detour at RVA `0x1F46AF6` over
the exact bytes
`C5 E2 59 E0 45 8B C1 4C 63 10 49 83 FA 04`, returning before the untouched
`jl` at RVA `0x1F46B04`. The trampoline reproduces the final wheel-angle
multiply and its following integer setup, then uses component `+0x120` as the
proven prior-frame 3D speed. `SteeringPercent=250` leaves response unchanged
through `80 km/h`, raises it linearly to `2.5x` at `220 km/h`, and clamps the
final command to `+/-85 degrees`. This amplifies partial and reversing input at
high speed while retaining the v0.2.3 static `80-degree` limit; it is not a
yaw-torque or tire-grip modification.

The steering trampoline validates vtable slot 6 as RVA `0x1F46AB0`. Its aligned
slope slot is installed as zero and becomes nonzero only after the same
resource transaction that activates drive force. Resource unload or rollback
atomically restores both the drive factor and steering slope to neutral before
the listener can accept a replacement resource.

TEST v0.2.5 leaves every v0.2.4 configuration value and physics calculation
unchanged. It extends the same steering trampoline with diagnostics for the
remaining handling bottleneck. At the hook point, the native calculation is
exactly `-[component+0xF4] * [component+0x110] * SteeringDegree*pi/180`.
The trampoline records component speed `+0x120`, that consumer input `+0xF4`,
the live scale `+0x110`, the amplified output before the mod clamp, and the
final output after it.

The expanded 260-byte cave stores its vtable/return addresses at `+192/+200`,
the seven existing response constants at `+208..+232`, and an aligned sample
record at `+236..+256`: sequence, speed, pre-clamp output, final output, raw
input, and scale. The physics thread increments the sequence before and after
the five scalar stores. The worker accepts a record only if two atomic reads
observe the same nonzero even sequence, so it cannot combine fields from an
in-progress update. No call, lock, allocation, or file I/O was added to the
physics path. The existing 200 ms worker logs relevant input/output changes and
a one-second held-input heartbeat, never more than about five samples per
second.

This trace is necessary because the packaged profile already patches the base
angle to `80 degrees`, while the runtime response remains clamped to
`+/-85 degrees`. If input and scale are both near `1.0`, full steering reaches
that clamp above approximately `85.8 km/h`; increasing `SteeringPercent` alone
then cannot materially improve full-lock steering. A runtime sample with
`raw~=1`, `scale~=1`, `pre>85`, `final=85`, and `clamp=yes` proves that the next
change must target the clamp or downstream yaw/traction rather than the gain.

TEST v0.2.6 adds one isolated resource scalar and leaves the v0.2.5 speed,
acceleration, steering, and telemetry paths unchanged. At RVA `0x1F48C92`, the
Coffin function loads its exact resource pointer from component `+0x48`; RVA
`0x1F48C96` reads resource `+0xC8`, RVA `0x1F48CA3` multiplies the destination
value by it, and RVA `0x1F48CA7` stores the result. The complete validated
consumer bytes are:

`48 8B 47 48 C5 FA 10 80 C8 00 00 00 48 8B 44 24 50 C5 FA 59 08 C5 FA 11 08`

Odradek established the native `+0xC8` value as `0.09`. `WetGripPercent=280`
therefore writes exactly `0.09 * 2.8 = 0.252`; the accepted `100-555` range
caps the result at approximately `0.5`. The patch accepts only native `0.09`
or the mod's recorded prior result on the same live resource. Range checks
reject NaN, infinities, implausible results, and foreign intermediate edits.
The scalar write uses a tighter `0.00005` verification tolerance than the
speed fields, participates in the existing ordered transaction, and is
explicitly restored if any later factor publication fails. Successful
reapplication derives from the preserved `0.09` baseline rather than
compounding `0.252` again.

No array is traversed or modified for this feature. In particular, component
`+0x50`, tire arrays, dry-grip fields, generic steering input, and downstream
yaw logic remain untouched.

## v1.0.0 one-shot lifecycle

The speed-only profile remains identical to v1.0.0. Before native
`AddListener`, the build now validates both vtable slot 3 at RVA `0x26F6E40`
and slot 4 at RVA `0x26F6EE0`. A callback publishes completion with release
semantics and the worker polls it with acquire semantics. The worker alone
calls `RemoveListener`; no callback and no `DllMain` path performs removal.
Native dispatch holds the listener lock shared across callbacks, while
`RemoveListener` takes it exclusive, so return from removal also synchronizes
with any callback already in flight. A CAS state transition makes removal
exactly once and later cleanup calls no-ops.

After one-shot removal, the mod does not observe a later Coffin resource
unload/reload during the same process. This bounded lifetime avoids retaining
an ASI callback in DS2's later streaming-shutdown path. Restart DS2 if a region
transition appears to recreate the Coffin physics resource.

## Speed-only profile

The final release keeps the proven `SpeedPercent=500` and
`AccelerationPercent=400` profile while removing every steering, wet-grip and
telemetry key from the packaged INI. Missing keys resolve to
`SteeringAnglePercent=100`, `SteeringResponsePercent=100`,
`WetGripPercent=100`, and telemetry off. Therefore the resource transaction
leaves `SteeringDegree=50` and wet side grip `0.09` unchanged, and
`install_steering_response_hook` returns before allocating or patching code.

Acceleration still requires the Coffin-filtered drive-moment detour. To make
the no-telemetry claim structural rather than merely suppressing log output,
v1.0.0 adds a dedicated 72-byte drive trampoline. It contains only the two
displaced multiplies, exact-vtable comparison, configured drive-factor
multiply, displaced store and absolute return. Its runtime fields are the
vtable immediate at `+16`, return qword at `+52`, and aligned factor at `+68`.
There is no `component+0x120` velocity load and no telemetry store. The older
80-byte diagnostic trampoline remains dormant for compatibility and is chosen
only by an explicitly enabled legacy telemetry key.

## v1.1.2 Floating Carrier support

The public v1.1.2 carrier implementation uses four narrowly scoped Coffin-only
changes. The linked-object type gate accepts the Floating Carrier case while
mounting, the RideVehicle transition shim snapshots and preserves the exact
linked carrier instead of running the native disconnect transaction, the
confirmed detach event is suppressed only while the captured player/carrier
and active Coffin handles still match, and the corresponding overextension
notification is filtered for that same exact pair.

This design preserves native detach/reattach behavior after dismounting and
does not change cargo or collision damage. Gameplay validation covered fast
land and water travel, collisions, and large jumps. A confirmation run saw 77
overextension notification producer hits and 77 exact-pair suppressions.

Earlier experimental follow-step scaling and global distance writes were not
required for stable retention and are not installed by the public release. The
unused follow-update executable anchor was also removed from final target
validation so the release validator checks only instructions used by the
shipping carrier path.


## v1.1.2 startup-order regression resolution

A later cleanup attempted to install the streaming listener before the Coffin-only carrier and drive hooks. In gameplay this produced a 60-second discovery timeout and left `SpeedPercent=500` inactive, even though the carrier mount/link hooks still worked. The final v1.1.2 release restores the order proven by the successful recovery build: carrier guards, drive hook, then streaming listener. The final warning guard remains in place, so the native Floating Carrier boundary HUD message and Sam reaction stay suppressed.
