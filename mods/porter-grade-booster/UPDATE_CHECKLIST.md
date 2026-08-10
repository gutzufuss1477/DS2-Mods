# Update checklist

For a future DS2 executable update:

1. Confirm the game starts without the mod.
2. Confirm the Q64 stat-mutator AOB still resolves uniquely.
3. Reconfirm the delivery caller used by Porter Grade awards.
4. Run the repeatable baseline delivery without the booster.
5. Verify Porter/Combat/Stealth/Service/Bridge Link mapping remains IDs 0..4.
6. Test `Multiplier=2.0`.
7. Confirm displayed gains equal exactly 2x baseline for all positive categories.
8. Confirm zero-gain categories remain zero.
9. Confirm category level-ups and Porter Class progression remain native.
10. Test once with Lost Cargo Likes Booster loaded simultaneously.
11. Rebuild the reference ASI and update SHA256SUMS.txt.
