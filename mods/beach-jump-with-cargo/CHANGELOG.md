# Changelog

## 1.0.0 — 2026-10-04

First public release.

- Keep cargo attached to Sam through Beach Jump fast travel.
- Keep backpack and body-mounted cargo through Transponder travel.
- Keep carried cargo through Hot Spring Jump travel.
- Hook the common `DSBaggageManager::HandlingBaggagesOnFastTravel(bool)` path instead of individual destinations.
- Preserve native destination, animation and fast-travel logic.
- Add a single `Enabled=1/0` setting.
- Remove private diagnostic modes and per-jump telemetry.
- Validate the supported Steam executable and exact hook prologue before patching.
- Add reproducible build/package scripts and embedded 1.0.0 version metadata.

Gameplay was confirmed across repeated Transponder/Facility travel and Hot Spring Jump travel before release.
