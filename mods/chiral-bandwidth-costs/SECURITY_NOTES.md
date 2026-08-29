# Security notes

## Scope

The ASI is intended to modify only the live `QpidCost` fields of explicitly configured construction resources in the current `DS2.exe` process. It does not need network access and does not intentionally write to the executable, game archives, or save files on disk.

The only optional persistent output is `ds2_chiral_bandwidth_costs.log` beside the ASI.

## Fail-closed checks

Before cost writes, the implementation is expected to verify:

- the host process name is `DS2.exe`;
- PE timestamp `0x6A3DAE46` and image size `0x0B292000`;
- the construction manager and target config VTables;
- the canonical root, manager, facility, and config pointer chain;
- readable committed memory without guard or no-access protection;
- the current field is either the documented native value or the requested target value;
- object identities are unchanged immediately before the transaction.

An unsupported build, invalid pointer, unexpected VTable, invalid INI value, or third-party field value must prevent the affected transaction. Unexpected values must never be normalized or overwritten silently.

## Transaction and rollback

Configured writes should be collected and applied as one verified transaction for a stable resource generation. Every original value and page protection must be captured before the first write. All writes require read-back verification.

If a write or protection restoration fails, every value already changed by that transaction must be restored and verified. An incomplete rollback or page-protection restoration is a critical error; no further writes should be attempted during that game session.

The `QpidCost` field is a naturally aligned 32-bit value at config offset `+0x4E4`. Writes use atomic compare/exchange from the captured value, and rollback changes a field back only while it still contains this mod's target. This avoids overwriting a concurrent third-party value. The field is data, so instruction-cache flushing is not required.

## Loader and lifecycle

The release does not include an ASI loader. Loading arbitrary ASIs grants native code execution inside the game process, so users should obtain a trusted 64-bit loader separately and verify release hashes when provided.

Initialization should only start a worker from `DllMain` or `InitializeASI`; INI parsing and resource traversal must happen outside the loader lock. INI values are read once per process. Hot-unloading the ASI is unsupported; restart the game after installing, changing, or removing it.

## Save and online caution

The mod does not directly edit save files, but DS2 can persist construction and bandwidth state produced while modified values are active. Costs should not be changed after structures of that type exist in a save. Back up the save and test offline until construction, reload, dismantling, online structures, and map transitions have been validated.
