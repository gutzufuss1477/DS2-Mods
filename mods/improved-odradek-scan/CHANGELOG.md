# Changelog

1.0.1 - 2026-09-26
- Fixed missing vanilla terrain scan colors/markings, including rough terrain, water and tar hazard visualization.
- Native terrain-survey values are now left untouched; the terrain overlay keeps its normal game range.
- The extended 500 m Odradek scan, 360-degree coverage, cargo detection and distance display remain active.
- Existing configuration remains compatible.

1.0.0 - 2026-09-23
First public release of Improved Odradek Scan.
- Tested 500 m default radius with configurable INI settings.
- Horizontal 360-degree scan with progressive native timing.
- Extended detection/display for eligible untouched and carried/dropped cargo.
- Distance after the recipient's likes, with no-likes and object-meter fallbacks.
- Spent/destroyed cargo retains vanilla nearby visibility.
- Exact target-build/code validation; diagnostics disabled by default.