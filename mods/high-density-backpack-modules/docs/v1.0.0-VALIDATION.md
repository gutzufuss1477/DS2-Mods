# Validation

## Runtime validation completed

The release logic is behaviour-identical to the validated v0.15.0 prototype except for release naming, version strings, filenames and documentation.

Confirmed in the live game:

- supported executable accepted and all patch-site checks passed;
- mixed high-density layout reached 16 installed components;
- backpack menu closed and reopened successfully;
- save completed successfully;
- game was fully restarted and the save loaded successfully;
- the compact component list remained present after load;
- multiple installed batteries produced substantially higher battery capacity;
- occupied anchors were automatically remapped;
- visible components were kept within the backpack boundary.

## Known and intentional behaviour

- Visible models can overlap and clip.
- The exact same logical anchor is not stored twice.
- The theoretical grid maximum is 30 unique anchors.
- Large shapes can exhaust their valid anchor set before all 30 cells are used.
- At capacity, the native replacement dialog can reappear.

## Not exhaustively measured

- Every individual special effect from every module class
- Every possible mixed layout near the 30-anchor theoretical limit
- Compatibility with third-party mods that patch the same backpack-placement functions
