# DS2 APAS Memory Costs

INI-configurable APAS Enhancement memory costs for **DEATH STRANDING 2: ON THE BEACH**.

## Release

Current version: **v1.0.0**

Default configuration:

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1
DebugLog=0
```

`GlobalCost=1` makes every loaded APAS Enhancement cost one APAS Memory point.

## Installation

Copy the files from `release/` beside `DS2.exe`:

- `ds2_apas_memory_costs.asi`
- `ds2_apas_memory_costs.ini`

A compatible 64-bit ASI loader is required.

Restart the game after changing the INI.

## Configuration

- `Enabled=1` enables the mod.
- `GlobalCost=0` makes APAS Enhancements free.
- `GlobalCost=1` makes every enhancement cost one point.
- `GlobalCost=2..1000000` sets an exact global cost.
- `DebugLog=1` enables detailed runtime logging.

## Runtime behavior

The mod continuously walks the game's APAS resource entries and overwrites
`DSApasEnhancementResource::EnhancementPoint` with the configured value.
Resources that become available later in the session are handled automatically.

The modification is runtime-only. The executable and save files are not modified on disk.

## Compatibility policy

There is intentionally **no hard game-version check**.

The current offsets were discovered and tested on Steam `DS2.exe 1.10.89.0`.
If a later game update moves the APAS manager or changes the resource layout,
the offsets in the source may need to be updated.

See `docs/TECHNICAL_NOTES.md` for the confirmed runtime path and research notes.
