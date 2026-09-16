# DS2 APAS Memory Costs

INI-configurable APAS Enhancement Memory costs for **DEATH STRANDING 2: ON THE BEACH**.

## Current release

**v1.0.3**

The v1.0.3 performance build has been promoted to the main release after testing showed
significantly better runtime behaviour than the earlier continuously polling version.

Default configuration:

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1
```

`GlobalCost=1` makes every loaded APAS Enhancement cost one APAS Memory point.

## Installation

Copy these files from `release/` beside `DS2.exe`:

- `ds2_apas_memory_costs.asi`
- `ds2_apas_memory_costs.ini`

A compatible 64-bit ASI loader is required.

Restart the game after changing the INI.

## Configuration

- `Enabled=1` enables the mod.
- `GlobalCost=0` makes APAS Enhancements free.
- `GlobalCost=1` makes every enhancement cost one point.
- `GlobalCost=2..1000000` sets the same custom cost for all APAS Enhancements.

## v1.0.3 performance behaviour

The worker no longer scans APAS resources permanently during normal gameplay.

It:

1. waits through initial process startup;
2. patches the currently available APAS resources;
3. polls lightly while the APAS table is still loading;
4. remains for several stable passes to catch late startup resources;
5. terminates completely once the normal APAS table is populated and stable.

The number of `VirtualQuery` calls per pass was also reduced.

This keeps the APAS cost behaviour while removing the permanent polling overhead present
in the earlier implementation.

## Runtime modification

The mod changes the runtime `EnhancementPoint` field used by the APAS system.

It does not modify:

- `DS2.exe` on disk;
- save files on disk.

## Compatibility

There is intentionally no hard game-version check.

The current offsets were discovered and tested on Steam `DS2.exe 1.10.89.0`.
A future game update may require the manager RVA or structure offsets to be updated.

See `docs/TECHNICAL_NOTES.md` and `docs/RESEARCH_HANDOVER.md` for maintenance details.
