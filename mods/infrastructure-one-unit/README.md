# Infrastructure One Unit

Reduces the **remaining** material requirement of supported Death Stranding 2 infrastructure to a configurable amount per still-open material category.

Default:

```ini
RemainingUnits=1
```

A road that still needs crystals, metals and ceramics will therefore require one unit of each category. The game keeps its native cargo handling: only the required amount is consumed and the remainder stays in the container or is returned by the game normally.

## Supported infrastructure

- Road construction, stage 1
- Road upgrades, stage 2
- Monorail construction, stage 1
- Mine construction, stage 1
- Mine upgrades, stages 2 and 3

Repairs, fabrication and ordinary cargo management are not modified.

## Supported game build

- Steam PC
- Death Stranding 2 v1.10.89.0

The ASI validates the executable metadata and required instruction signatures. On a different build it remains inactive instead of applying an unsafe patch.

## Installation

1. Install a compatible x64 ASI loader for the game.
2. Copy these files next to the ASI loader / `DS2.exe`:

```text
ds2_infrastructure_one_unit.asi
ds2_infrastructure_one_unit.ini
```

3. Start the game without Cheat Engine.

## Configuration

```ini
[InfrastructureOneUnit]
Enabled=1
RemainingUnits=1

RoadConstruction=1
RoadUpgrades=1
MonorailConstruction=1
MineConstruction=1
MineUpgrades=1

DebugLog=0
```

`RemainingUnits` is applied **per still-open material category**.

Examples:

| Native remainder | `RemainingUnits` | Effective remainder |
|---:|---:|---:|
| 3000 | 1 | 1 |
| 3000 | 50 | 50 |
| 40 | 50 | 40 |
| 0 | 50 | 0 |

The mod never increases a requirement that is already below the configured value. Fully completed and unused material slots remain unchanged.

Restart the game after editing the INI.

## Existing online contributions

Existing player/online contributions are preserved. The mod adds only the missing delta required to reach the configured remainder. It does not reset the infrastructure to zero and does not discard prior progress.

## Display behaviour

For loaded supported infrastructure, the reduced amount can already appear in the world/overview UI before opening the detailed contribution menu. The exact refresh timing remains controlled by the game.

## Save warning

The mod does not edit save files directly. It changes the live infrastructure progress used by the game, and the game can persist that progress in normal saves. Back up a save before first use. Removing the ASI later does not necessarily restore requirements already reduced and saved.

## Logging

Set:

```ini
DebugLog=1
```

The mod then creates:

```text
ds2_infrastructure_one_unit.log
```

A successful startup contains:

```text
ACTIVE: exact Need/Current caller-pair hooks armed. Known caller pairs: 16.
```

Applied infrastructure is logged as `APPLY` entries.

## Build

See [BUILD.md](BUILD.md). The repository includes a minimal Windows compatibility header and does not require the Microsoft SDK for the supplied LLVM build path.
