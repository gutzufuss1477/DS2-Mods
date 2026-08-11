# Technical notes

## Isolated path

Validated upstream path on Steam PC build `1.10.89.0`:

- `DS2.exe+0x1430FD0` loads the default reward multiplier state.
- `DS2.exe+0x1430FE3` tests bit 0 of `[RDX+0x60]`.
- `DS2.exe+0x1430FEA` loads selector byte `[RDX+0x5D]`.
- `DS2.exe+0x1430FEE` conditionally selects the lookup input.
- `DS2.exe+0x1430FF1` calls the multiplier lookup at `DS2.exe+0x141C590`.
- `DS2.exe+0x1431022` multiplies the base reward by the returned factor.

The release hook is placed at `DS2.exe+0x1430FF1` because the normal and Magellan states can be distinguished there without directly rewriting the reward value.

## Why the result label remains

The `DHV-Magellan-Lieferung` indicator is based on transport/evaluation state elsewhere in DS2. v1.0.0 intentionally leaves that state intact. This keeps the scope limited to the reward penalty and avoids changing cargo history or UI state.

## Why the mod does not multiply Likes afterward

The A/B trace showed the penalty affects one specific reward component while other components remain unchanged. Applying a global multiplier to the final Like total would therefore be broader and less accurate than normalizing the selector at the original multiplier lookup.
