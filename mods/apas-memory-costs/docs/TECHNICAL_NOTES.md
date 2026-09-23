# Unified APAS 3.0.0-rc.1 implementation

## Supported executable

Steam DS2.exe 1.10.89.0:

```text
SHA-256       BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B
PE timestamp  0x6A3DAE46
Image size    0x0B292000
Manager slot  0x0623E5D0
Resource VT   0x032088B8
```

Runtime verification checks PE metadata and exact independent APAS anchors, with
ASLR-aware validation of resource virtual-function addresses. It deliberately
refuses unknown builds instead of following unchecked fixed-address pointers.

## Cost consistency and base-node regression

Native node creation at RVA `BE0270` receives `(manager, position, resource, order)`.
At `BE04D2`, it reads `resource+0x24` and copies the value into `entry+0x50` at
`BE04DB`. The resource is retained at `entry+0x70`.

The old polling implementation changed only `resource+0x24` after construction.
That could leave the native cached cost different. It also overwrote zero-cost
resources and the special IDs 0..3. Native activation at `BE12E0` treats those IDs
specially but still performs the Memory-capacity test. Isolated execution of that
actual native check confirms that a nonzero base cost can fail at zero capacity,
where the native zero cost passes. This is a demonstrated mechanism, not a proven
reproduction of the Nexus user's whole new-game/UI failure.

The new wrapper validates the resource vtable and ID, preserves IDs 0..3 and native
zero/invalid costs, then changes valid positive costs before calling the native
constructor. The game therefore uses the same configured value for the resource
and its construction-time cache. No pointers to game resources are retained.

The game calls the constructor for later nodes and re-created nodes after loading;
the hook does not stop after a table-count threshold. No gameplay polling is used.
Installation after the APAS manager already exists is refused to avoid applying
the new construction rule to only part of an already populated manager.

## Executable hook and lifetime

The cost entry is 16-byte aligned. The first original instruction is exactly five
bytes (`mov [rsp+10h],rdx`). Replace only that instruction with a relative jump to
a nearby relay. Bytes 5..15 and all following native instruction boundaries stay
unchanged. The relay makes an absolute tail jump to a normal Windows-x64 C++
wrapper. A leaf trampoline executes the original five bytes and jumps back to
`BE0275`. It changes no stack allocation, so it needs no non-leaf unwind record.
The C++ wrapper has normal compiler-generated unwind metadata as needed.

Aligned `cmpxchg16b` validates and changes each code block atomically. It never
blindly overwrites a foreign patch. Preflight checks cover both features before
any patch; a later failure rolls back changes still owned by this build. Page
protections are restored and the instruction cache is flushed. Relay pages become
RX before activation and are retained even after rollback in case a thread has
already entered them. The ASI is pinned for process lifetime before installing
callbacks. An interlocked initialization guard prevents repeat startup workers.

## Unlock All in the same binary

Default: `[APASUnlocks] UnlockAll=0`. This is independent of cost `Enabled`.

The optional code change is now at `BE39A9`, inside the APAS unlock updater. Its
original four-byte resource-fact load is replaced with `EB 43 90 90`. The short
jump lands at `BE39EE`, which invokes the native locate/unlock routine at `BE1640`.
This bypasses this updater's APAS-specific fact and grade requirements, including
the fact gate left intact by the old `BE39C9` patch. Earlier native system-availability
guards remain. The entire surrounding 96-byte window is checked, so an older
Unlock All patch in the same routine is detected as a conflict.

No global mission, facility, or Porter Grade state is spoofed. Native unlocks can
persist in saves. Turning the setting off changes future runtime behavior; it is
not a rollback of already saved progression.

## Diagnostics and performance limits

Only the bounded startup worker performs INI I/O, code checks, allocation and
logging. It returns after installation or refusal. The gameplay hook performs
constant bounded work once per native node-creation call; no `VirtualQuery`,
`ReadProcessMemory`, logging or allocation occurs in that callback.

This eliminates the old recurring scan by construction. It does not constitute a
measured FPS result. New-game UI behavior and the reported Nexus regression still
require the user gameplay checks in `TEST_STATUS.md`.
