# Security / safety notes

- No Cheat Engine runtime dependency.
- No network access.
- No external process creation.
- No save-file parser or direct save-file editing.
- The release modifies only the in-process Porter Grade add amount at the validated delivery path.
- The mutator hook is installed only when its AOB signature resolves uniquely.
- Only Porter Grade stat IDs 0..4 are eligible.
- Only the confirmed delivery caller is eligible.
- Zero original gains are ignored.
- Values are clamped to the upper cap supplied by DS2.
- If the signature cannot be resolved, the mod leaves the game code untouched.
