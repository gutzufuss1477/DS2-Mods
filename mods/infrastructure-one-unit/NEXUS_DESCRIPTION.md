# Infrastructure One Unit

## Description

Reduces the remaining materials needed for supported infrastructure to a configurable amount per open material category.

With the default `RemainingUnits=1`, every still-open category requires only one unit. The game continues to handle containers natively, so only the required unit is consumed and the remainder stays in the container or is returned normally.

## Features

- Road construction, stage 1
- Road upgrades, stage 2
- Monorail construction
- Mine construction
- Mine upgrades, stages 2 and 3
- Configurable amount through the INI
- Existing online/player contributions are preserved
- Completed categories are not reopened
- Native partial-container handling
- Automatic application to loaded supported infrastructure

## Installation

Copy the ASI and INI next to `DS2.exe` / your x64 ASI loader:

```text
ds2_infrastructure_one_unit.asi
ds2_infrastructure_one_unit.ini
```

Supported build: Steam PC v1.10.89.0.

## Configuration

```ini
RemainingUnits=1
```

Set a higher number when you want reduced requirements without making every category cost only one unit.

## Requirements

- Death Stranding 2 Steam PC v1.10.89.0
- Compatible x64 ASI loader

## Important

Back up a save before first use. The mod changes live infrastructure progress, which the game may persist normally. Removing the ASI does not necessarily restore original requirements already reduced and saved.
