# DS2 APAS Memory Costs

INI-configurable APAS Enhancement Memory costs for **DEATH STRANDING 2: ON THE BEACH**.

## Available versions

### Main File - APAS Memory Costs v1.0.3

Changes APAS Memory costs only.

Default:

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1
```

`GlobalCost=1` makes every loaded APAS Enhancement cost one APAS Memory point.

### Optional File - Unlock All v1.1.0

Replacement build that includes the complete APAS Memory Costs functionality plus:

```ini
[APASUnlocks]
UnlockAll=1
```

It unlocks all valid APAS Enhancements through the game's native APAS unlock path.

**Do not load the Main and Optional ASIs together.**
The Optional version replaces the Main version and already includes `GlobalCost`.

## Main File installation

Copy from `release/`:

- `ds2_apas_memory_costs.asi`
- `ds2_apas_memory_costs.ini`

beside `DS2.exe`.

## Optional Unlock All installation

Copy from:

`release/optional-unlock-all/`

beside `DS2.exe`, replacing the normal APAS ASI/INI.

A compatible 64-bit ASI loader is required.

## Configuration

Main cost settings:

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1
```

- `Enabled=1` enables cost modification.
- `GlobalCost=0` makes APAS Enhancements free.
- `GlobalCost=1` makes every Enhancement cost one point.
- `GlobalCost=2..1000000` sets an exact global cost.

Optional build only:

```ini
[APASUnlocks]
UnlockAll=1
```

If you want Unlock All but vanilla APAS costs, set:

```ini
[APASMemoryCosts]
Enabled=0

[APASUnlocks]
UnlockAll=1
```

## Performance

Both versions use the v1.0.3 low-overhead APAS cost worker.

It patches resources during loading and exits completely once the APAS resource table
is populated and stable. There is no permanent 1-second gameplay scan.

## Unlock All warning

Unlock All intentionally bypasses normal APAS progression requirements.

It does not globally complete missions, change facility friendship, modify Porter Grade
or unlock unrelated game systems.

Because it uses the game's native APAS unlock routine, unlocked Enhancements may be
persisted in the save. Back up the save before first use if vanilla progression matters.

## Compatibility

There is no hard global game-version gate.

Known/tested research target:

```text
Steam DS2.exe 1.10.89.0
```

The cost path and Unlock All patch both use local/runtime validation where possible.
Future game updates may still require updated offsets/signatures.

See the files under `docs/` for full technical and research notes.
