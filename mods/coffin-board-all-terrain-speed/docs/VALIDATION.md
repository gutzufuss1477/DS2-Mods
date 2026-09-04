# Validation — v1.1.7

Target: DEATH STRANDING 2: ON THE BEACH, Steam `DS2.exe 1.10.89.0`

## Build validation

The release ASI is the exact v1.1.7 `native-replay` binary that passed the final in-game test.

- PE32+ x64 DLL / ASI
- Deterministic double-build comparison: byte-identical
- Coffin Board resource patch: retained
- Floating Carrier mount/link preservation: retained
- RideVehicle resolver handling: native resolver replay before Coffin-only inspection

## Final in-game confirmation

Confirmed in one final validation session:

- Pickup can be entered normally
- Tri-Cruiser can be entered normally without CTD
- Coffin Board can be entered normally
- Coffin Board can be entered with a loaded Floating Carrier attached
- Configured high speed remains active
- Floating Carrier stays attached during the ride
- Obsolete Floating Carrier boundary warning and Sam dialogue stay suppressed

The final log reports all v1.1.7 native-replay carrier hooks installed and the speed patch transaction completed successfully.
