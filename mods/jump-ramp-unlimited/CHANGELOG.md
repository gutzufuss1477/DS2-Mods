# Changelog

## 1.0.0 — 2026-10-03

First public release.

- Repeat the final available aerial trick after a Jump Ramp launch, removing the normal sequence limit.
- Use the ramp's actual trick cap, with repeat paths for cap 2 and cap 3.
- Retain the game's input checks, action checks and native cooldown.
- Restore the trick stage when a repeated attempt is rejected.
- Read the single `Enabled` option at startup; configuration changes require a restart.
- Use final `ds2_jump_ramp_unlimited` filenames and embedded 1.0.0 version metadata.
- Check the supported Steam executable identity, surrounding instructions and old probe conflict before patching.
- Replace private-test telemetry and polling with a short startup log.
- Include reproducible build/package scripts, native verification, documentation and a two-file Nexus ZIP.

Gameplay confirmation comes from private v0.6-test5. See [validation details](docs/VALIDATION.md) for the final release's automated checks and live-test limits.
