# Technical notes

## Why this path was selected

Odradek's RTTI data for DS2 PC `1.10.89.0` defines:

- `DSWeaponParameter::IsApplyWeaponsProhibitedArea` as a `bool` at offset `0x1E7`;
- separate equipment-manager resources for `WeaponProhibitedRingEntityResource`, `Sound_WeaponProhibitedRingOn` and `Sound_WeaponProhibitedRingOff`.

Two paths use this field and must not be confused:

- `DS2.exe+0xF73922` feeds the prohibited-ring On/Off state and its sound resources. It deliberately remains native.
- The leaf function at `DS2.exe+0xFA4210` returns action-blocking reason codes. Its first branch resolves the same weapon parameter and returns reason `6` when the prohibited-area flag applies.

The isolated enforcement path is:

```text
DS2.exe+0xFA4210  mov   rax,[rcx+0x38]
DS2.exe+0xFA4217  cmp   byte ptr [rax+0xAA3],r9b
DS2.exe+0xFA421E  je    continue_checks
...
DS2.exe+0xFA424E  cmp   byte ptr [rax+0x1E7],r9b
DS2.exe+0xFA4255  je    continue_checks
DS2.exe+0xFA4257  mov   eax,6
DS2.exe+0xFA425C  ret
DS2.exe+0xFA425D  continue_checks:
```

Only the opcode of the `je continue_checks` instruction is replaced. Turning it into an unconditional `jmp` skips reason `6` while preserving all later native checks and return reasons.

## Original and replacement bytes

| RVA | Original | Replacement |
|---|---|---|
| `0x00FA4255` | `74` (`je`) | `EB` (`jmp`) |

## Fail-closed checks

Before writing, the ASI validates:

- process name `DS2.exe`;
- PE timestamp `0x6A3DAE46`;
- image size `0x0B292000`;
- the unique 22-byte context at `0x00FA4247`;
- the exact original opcode byte at the patch site.

The replacement is read back immediately. A failed verification triggers an attempt to restore the original bytes.

## Preserved systems

The ASI does not patch the presentation function at `0x00F7389C`, the zone trigger, ring entity, ring sounds, announcement state, UI text, facility state, save data, structure data or archive resources. This separation is why the warning presentation is expected to remain visible while enforcement reason `6` is bypassed.

## Runtime status and remaining uncertainty

Firearms and grenades were confirmed in game inside a facility/building weapons-prohibited area on 2026-08-28. Some melee, specialized or vehicle-mounted equipment may still have an independent restriction unrelated to `DSWeaponParameter::IsApplyWeaponsProhibitedArea`; such a case should be documented and isolated rather than widening the patch blindly.
