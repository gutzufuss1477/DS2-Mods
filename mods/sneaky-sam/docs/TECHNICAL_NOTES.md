# Sneaky Sam - Technical Notes

Target: DEATH STRANDING 2 v1.10.89.0

## Native findings

`EDSBaggageSlotType`:
- 4 RightArm
- 5 LeftArm
- 6 RightWaist
- 7 LeftWaist

`DSPlayerHideEquipmentEventResource` also exposes native fields:
- HideBaggagesOnShoulders
- HideBaggagesOnWaists

The selected release implementation does not instantiate or modify sequence-event resources. Instead it uses the lower-level visibility path already used by the player equipment manager.

## Hook

Native refresh function: RVA `0xF6BF30`

Callsite:
- RVA `0xF6C09D`
- original bytes: `E8 7E 84 22 00`
- original target: RVA `0x1194520`

At this callsite:
- RCX = baggage entity
- DL = native visibility boolean
- R8D = native reason hash `0x5253195E`

The wrapper reads the baggage slot byte at entity offset 0. For slot 4..7 only, DL is replaced with false and the original native function is called.

## Safety properties

- exact callsite validation before patching;
- PE timestamp and SizeOfImage validation;
- near relay only;
- no breakpoint/PAGE_GUARD/single-step mechanism;
- no inventory mutation;
- no save-data writes;
- no capacity or weight modification;
- no detection/AI modification.
