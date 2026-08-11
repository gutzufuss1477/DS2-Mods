# Security and safety notes

This ASI is an in-process DS2 plugin. It is build-specific and validates both the known executable metadata and the exact instruction signature at the isolated reward-selector lookup before enabling.

The runtime hook targets `DS2.exe+0x1430FF1`. At that point the validated Magellan case has:

- selector byte `[RDX+0x5D] = 2`
- flags byte `[RDX+0x60]` with bit 0 set
- `ECX = 0`

Only that combination is normalized to `ECX = 2`. DS2 then performs its own original multiplier lookup and subsequent reward calculation.

The mod does not directly edit save files, cargo records, the final Like total, connection/star values, networking, the registry, or the Magellan result-screen label.

The hook uses a vectored exception handler and a one-byte software execution breakpoint that is restored before the original instruction executes and re-armed immediately afterward. On any validation or hook-install failure, no reward normalization is enabled for that session.

Revalidate the offsets, instruction bytes and A/B behavior after any DS2 update before publishing an updated binary.
