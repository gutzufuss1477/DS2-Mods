# Technical Notes

## Confirmed APAS runtime path

Static analysis and runtime testing established the following chain:

```text
DS2.exe + 0x623E5D0
    -> global APAS manager pointer

manager + 0x30 + index * 8
    -> APAS entry pointer

entry + 0x70
    -> DSApasEnhancementResource*

resource + 0x24
    -> EnhancementPoint / APAS Memory cost
```

The APAS manager contains `0x37` entry slots.

## Why `resource + 0x24` is the real cost field

The APAS accounting function at the analyzed build's RVA `0xBE3D00`
walks active APAS entries, resolves `entry + 0x70`, and adds
`[resource + 0x24]` to the occupied APAS Memory total.

The APAS activation path also reads the same field and compares it against:

```text
total APAS capacity - currently occupied APAS Memory
```

Therefore the field is not only a UI value. It participates in both:

1. occupied-memory accounting;
2. the activation capacity check.

The reflected resource terminology found during static analysis includes:

- `DSApasEnhancementResource`
- `DSApasEnhancementResources`
- `EnhancementPoint`
- `EnhancementCategory`
- `EnhancementId`
- `InitialMemoryCapacity`
- `AdditionalMemoryCapacityByHouseholdFriendshipLevel`
- `apas_enhancement_array`

## Current implementation

The release patches `EnhancementPoint` in memory for each loaded APAS resource.
The worker repeats periodically so resources that appear after startup are covered.

### Constants

```text
Manager global RVA : 0x623E5D0
Entry count        : 0x37
Entry array offset : 0x30
Resource ptr       : +0x70
EnhancementPoint   : +0x24
```

## Version policy

v1.0.0 deliberately does not perform a hard PE timestamp, image-size, or signature gate.

The constants above were discovered and functionally tested on:

```text
Steam DS2.exe 1.10.89.0
```

A future game update may require rediscovering only the manager RVA and/or structure offsets.
The static discovery history is summarized here so later maintenance can resume from the
confirmed data path instead of repeating the full investigation.

## Safety characteristics

- Pointer regions are checked with `VirtualQuery` before dereferencing.
- Only 32-bit non-negative cost fields in the accepted range are changed.
- `VirtualProtect` is used only around the four-byte cost write.
- A compare/exchange write avoids replacing a value that changed between read and write.
- The mod never writes to `DS2.exe` on disk.
- The mod never writes to save files on disk.
