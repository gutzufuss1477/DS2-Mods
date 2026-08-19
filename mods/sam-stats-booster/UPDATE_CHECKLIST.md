# Update Checklist

- [ ] Record the new `DS2.exe` version, SHA-256, PE timestamp, and image size.
- [ ] Run `tools/validate_target.py` against the new executable.
- [ ] Re-resolve all three producer functions from their full signatures.
- [ ] Revalidate U32 patch site, manager-slot RIP target, stolen length, and continuation.
- [ ] Revalidate Q64 patch site, native cap constant, stolen length, and continuation.
- [ ] Revalidate double patch site, cap RIP target/value, stolen length, and continuation.
- [ ] Reconfirm the skill-type-to-record mappings for all 12 visible categories.
- [ ] Confirm no other project mod patches the same producer bytes.
- [ ] Rebuild with the provided LLVM script.
- [ ] Verify the resulting ASI has no imports and the three hook functions disassemble correctly.
- [ ] Run activation, targeted progression, level-up, save/reload, and compatibility tests.
- [ ] Update hashes, technical notes, validation output, and changelog.
