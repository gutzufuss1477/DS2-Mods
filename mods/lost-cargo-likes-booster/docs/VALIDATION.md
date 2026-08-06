# Validation

Runtime validation on DS2 Steam build 1.10.89.0:

- Lost cargo at the validated writer used component sequence `34 + 17 + 12` (normal reward +63).
- A regular order used component signature `RBX=1`; it is excluded by the production guard.
- Production effect test logged `APPLIED=1`, adjusting the first validated component input `10209` to `15209`.
- The player observed `+5063`: normal `+63` plus configured `+5000`.

The binary SHA-256 is listed in `SHA256SUMS.txt`.
