# DS2 update checklist

After a DS2 game update, do not assume the existing binary is compatible.

1. Record the new `DS2.exe` version, PE timestamp, image size and SHA-256.
2. Verify whether the reward-selector path still corresponds to the old `DS2.exe+0x1430FF1` logic.
3. Re-identify the sequence that loads `[RDX+0x5D]`, tests bit 0 of `[RDX+0x60]`, chooses the lookup selector, and calls the reward multiplier lookup.
4. Update the RVA and expected instruction signature only after static verification.
5. Build a diagnostic candidate before changing the public release.
6. Repeat an A/B test with the same order/state:
   - normal transport
   - Magellan transport without normalization/baseline if available
   - Magellan transport with candidate fix
7. Confirm normal delivery reward is unchanged and the Magellan reward matches the normal delivery reward.
8. Confirm no crash on delivery, save reload, game restart and multiple sequential deliveries.
9. Rebuild twice and compare ASI SHA-256 values for reproducibility.
10. Update README, validation notes, hashes and changelog before release.
