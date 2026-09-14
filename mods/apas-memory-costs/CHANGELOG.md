# Changelog

## v1.0.0

- Initial public release.
- Added global INI-controlled APAS Enhancement memory cost.
- Default `GlobalCost=1`.
- Supports `GlobalCost=0..1000000`.
- Applies the configured value to APAS resources loaded after startup.
- Optional debug logging.
- Runtime-only modification; no executable or save-file edits.
- Removed hard game-version verification for the public release.
- Added process-local single-instance protection without creating a persistent lock file.
