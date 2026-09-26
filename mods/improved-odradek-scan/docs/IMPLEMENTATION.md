# Improved Odradek Scan 1.0.1 – implementation

Target: Steam 1.10.89.0, PE timestamp 0x6A3DAE46, image size 0x0B292000,
SHA-256 BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B.

The scan-start wrapper sets the requested diameter (twice the INI radius) and
horizontal full-circle angle. Native code retains progressive expansion. The
terrain-survey resource itself is no longer modified: its ShapeMode, directional
angle and range remain native so rough-ground, water and tar color overlays keep
their original rendering behaviour and native range.
Recent native cargo hits qualify for scoped candidate permission, sensor cap and
HUD display-radius adjustments. Used/destroyed cargo stays on its vanilla path;
usable dropped cargo is eligible. No global HUD range, forced entity loading,
full-world sweeps, synthetic hit replay or persistent metadata writes are enabled.

Cargo distance comes from the native prepared icon record. A scoped TLS context
connects the cargo detail job to native string construction. The distance suffix
is appended after the native likes text when that row exists, otherwise to the
recipient name. Native string ownership and UI measurement remain authoritative.
The old object-meter field is a fallback. A bounded cache rebuilds when the integer
metre value or native dirty state changes.

Build identity, target code, callsites and pointer/record conditions are checked.
Exact-call groups support rollback with page protections restored. Unsupported
or conflicting targets fail closed for the affected feature.

The complete test pipeline covers INI parsing, synthetic cargo filtering/expiry,
prepared-field isolation, actual native sensor boundaries/progression, text ABI,
UTF-8/bounds, native reward predicate/formatter, cache refresh and hook rollback.
It maps the game image without executing its entry point. Full rendered gameplay
cannot be validated offline. The 500 m preset and final label order were confirmed
by the user in gameplay before the public release.

Version 1.0.1 separates extended gameplay scanning from the terrain survey.
Gameplay testing confirmed restored terrain colors while the 500 m scan and
360-degree coverage remain active. Diagnostic logging is off by default.
