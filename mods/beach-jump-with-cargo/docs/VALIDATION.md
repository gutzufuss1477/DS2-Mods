# Validation

## Gameplay confirmation

The private test build was used repeatedly in the supported Steam build.

Confirmed scenarios:

- Facility to Transponder with carried cargo.
- Repeated travel back and forth through Transponder / Beach Jump destinations.
- Hot Spring Jump from the Maternity Clinic area to Heartman's destination.
- Cargo remained on Sam after arrival, including backpack and body-mounted cargo.

The diagnostic log confirmed that the common `HandlingBaggagesOnFastTravel` hook was reached during the tested travel paths and that the tested mode skipped the handler.

## Release verification

The v1.0.0 release pipeline checks:

- Exact supported `DS2.exe` SHA-256.
- PE timestamp and SizeOfImage.
- Exact eight-byte prologue at RVA `0x011E34A0`.
- Source/version-resource agreement with `VERSION.txt`.
- Embedded PE file and product version.
- Reproducible ASI build across two consecutive builds.
- Deterministic two-file Nexus ZIP creation.

The public release removes diagnostic mode switching and per-jump logging, but uses the same confirmed skip mechanism.

## Scope

Vehicles, Floating Carriers, cargo on the ground and cargo not attached to Sam were not targeted by this mod and are outside the validated scope.
