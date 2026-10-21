# Release Verification

## Original source archive

- File: `DS2_Vehicle_Core_v1.1.0_Source.zip`
- SHA-256: `B92E981B9D4760E6A26F1DAD14B71FB0847C4B071FBD50490BA88582590C6928`

## Published Nexus installer archive

- File: `Hill Assist and Speed Boost V2 Installer 53 2 2026-07-24T22-02Z KZZlWW3Yp.zip`
- SHA-256: `35648D929B4B3136A4E5A7E451410DFA8BB5E4565645480683C9C50C7433ACE8`

## Validated core binary

- File: `reference/DS2VehicleCore_v1.1.0.asi`
- SHA-256:
  `78F4D90B6473F9163E65A227AE6444D516133ABAC34C9E6575BD2B13C3CB60B6`

The ASI and both final INI files in the source archive match the files used by
the published combined release.

A newly compiled binary may differ because PE timestamps and compiler/linker
metadata can change. Runtime compatibility must therefore be validated in
addition to comparing hashes.
