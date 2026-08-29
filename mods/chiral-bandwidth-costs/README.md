# DS2 Chiral Bandwidth Costs v1.0.0

Configurable chiral-bandwidth costs for supported player constructions in **DEATH STRANDING 2: ON THE BEACH** on PC.

Version 1.0.0 supports one exact executable build. The packaged INI keeps every construction at `Native` by default, so installing the ASI alone does not change any cost. Enter a decimal number for only the constructions you want to customize.

## Installation

The release archive contains exactly two runtime files:

- `ds2_chiral_bandwidth_costs.asi`
- `ds2_chiral_bandwidth_costs.ini`

Install a compatible external 64-bit ASI loader, then copy both files into the game directory beside `DS2.exe`. Fully restart the game after installing the mod or changing the INI.

To uninstall, remove both files and restart the game. The ASI loader is not included.

## Configuration

```ini
[ChiralBandwidthCosts]
Enabled=1
DebugLog=0

Postbox=Native
SafeHouse=Native
Watchtower=Native
CargoCatapult=Native
Generator=Native
RainShelter=Native
Zipline=Native
Bridge=Native
ChiralBridge=Native
JumpRamp=Native
HotSpringDigger=Native
```

`Native` leaves that construction unchanged. A decimal integer from `0` through `1000000` requests that cost; `0` means no chiral-bandwidth cost. Values do not accept signs, decimal separators, hexadecimal notation, or inline comments. An invalid enabled configuration fails closed without applying cost writes.

Example:

```ini
Generator=100
Zipline=250
JumpRamp=300
```

The supported keys and known native reference values for the target build are:

| INI key | Native reference cost |
| --- | ---: |
| `Postbox` | 200 |
| `SafeHouse` | 2500 |
| `Watchtower` | 250 |
| `CargoCatapult` | 450 |
| `Generator` | 350 |
| `RainShelter` | 500 |
| `Zipline` | 500 |
| `Bridge` | 1000 |
| `ChiralBridge` | 250 |
| `JumpRamp` | 550 |
| `HotSpringDigger` | 0 |

The single `Bridge` key applies to the normal 30 m, 45 m, and 80 m bridge configurations. The three variants and both 30 m manager paths are covered by static and synthetic validation, but not every variant has received the same live gameplay coverage.

`HotSpringDigger` is experimental because its native cost is zero, although DS2 marks it as requiring Q-pid coverage. The Transponder is intentionally unsupported: its resource is a tool with `NeedWithinQpid=false`, so changing its otherwise present `QpidCost` field would not provide reliable chiral-bandwidth charging.

These values are target-build references. `DebugLog=1` creates `ds2_chiral_bandwidth_costs.log` beside the ASI. Errors are logged even when optional debug logging is disabled.

## Validation

On 2026-08-29, an exact custom cost of `5` was confirmed in the live construction HUD for both Generator and Zipline on the supported Steam build. The release also passes the packaged-INI parser tests, synthetic runtime transaction tests, target-executable validation, and binary hardening checks documented in [`docs/VALIDATION.md`](docs/VALIDATION.md).

## Save warning

Back up the save and test offline first. Keep the selected costs unchanged for a save after constructing anything with the mod.

DS2 may use the currently loaded construction-config cost when a structure is removed instead of storing the original build cost on that individual structure. Changing an INI cost after structures of that type already exist could therefore make used chiral bandwidth inconsistent. Build, save, reload, and dismantle behavior still requires in-game validation for every supported type.

## Compatibility and safety

- Supported executable: Steam PC `DS2.exe 1.10.89.0` only.
- Expected PE timestamp: `0x6A3DAE46`.
- Expected image size: `0x0B292000`.
- The mod validates the executable build and relevant runtime resource types before writing.
- Unexpected resource values are treated as a possible mod conflict and are not overwritten.
- The mod does not edit `DS2.exe`, game archives, or save files on disk.
- It does not intentionally change material requirements, placement restrictions, upgrades, repairs, network coverage, or total available chiral bandwidth.
- After a game update, remove the ASI until a compatible version is available.

Generator and Zipline have live HUD validation at a custom value of `5`. Exact charge/refund behavior, save/reload, map transitions, online structures, and the experimental Hot Spring Digger path have not received comprehensive manual coverage for every construction type.
