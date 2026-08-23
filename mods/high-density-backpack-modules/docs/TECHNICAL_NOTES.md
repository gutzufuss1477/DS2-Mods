# Technical notes

## Design

The release separates three concepts that DS2 normally treats together:

1. **Logical ownership**: replacement collection and final grid commit use a `1 x 1` footprint.
2. **Visible/native shape**: the original component dimensions at definition offsets `+0x24` and `+0x28` remain available.
3. **Boundary and anchor selection**: a wrapper searches for the nearest free anchor from which the complete native shape stays inside the `5 x 6` backpack grid.

The mod does not create a custom save format and does not virtualize multiple owners for one exact grid cell.

## Supported executable

- Version: `1.10.89.0`
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- PE timestamp: `0x6A3DAE46`
- Image size: `0x0B292000`

## Key native locations

- Shape lookup: `DS2.exe + 0x00BAD600`
- Native reference check: `DS2.exe + 0x00B406F0`
- Wrapped call site: `DS2.exe + 0x00B43043`
- Replacement collection: `DS2.exe + 0x00B40090`
- Grid validation / commit: `DS2.exe + 0x00B40310`
- Native occupancy flag immediate: `DS2.exe + 0x00B404BC`
- Native conflict branch: `DS2.exe + 0x01525D18`

The occupancy flag and conflict branch remain native. See `PATCH_MAP.tsv` for all modified instruction windows.

## Fail-closed behaviour

Before activation, the ASI validates:

- PE timestamp and image size;
- reference-check call site;
- manager-pointer load context;
- native reference-check prefix;
- shape-lookup prefix;
- all native full-shape reads;
- native occupancy flag;
- native conflict branch;
- every original byte sequence replaced by the one-cell patches.

Any mismatch leaves the mod inactive and records the failure in the log.

## Runtime hook

The original call at `0x00B43043` is redirected through a near relay when required by x64 relative-call range limits. The wrapper can:

- accept the selected anchor when free and shape-valid;
- search nearest anchors by Manhattan distance;
- write the selected remapped coordinates back to the staged candidate and manager state;
- force a local native rejection if no valid anchor exists.

No PAGE_GUARD, debugger breakpoints, single-step tracing or Cheat Engine runtime dependency is used.
