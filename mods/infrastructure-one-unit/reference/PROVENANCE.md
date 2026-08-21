# Validated binary provenance

The release/reference ASI is the exact binary that passed the final integrated in-game test, with one release-label-only patch:

```text
1.0.3-testfix -> 1.0.0-release
```

The files are equal in size. Only seven ASCII bytes in the embedded version label differ. No executable instruction, caller-pair table, VTable constant, hook address or policy data was changed.

The source tree implements the same 16-pair hook model and delta policy. A cross-build is validated separately, while public release packaging uses the in-game validated reference binary by default.
