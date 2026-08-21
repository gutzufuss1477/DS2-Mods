# Safety and scope

- The ASI validates the supported DS2 executable metadata and required instruction signatures.
- Unknown builds fail closed and receive no hooks.
- Only known infrastructure VTables and stage modes are accepted.
- The mod does not hook cargo ownership, weights, container finalizers or fabrication.
- Progress updates are written transactionally and verified; a failed write is rolled back and disables the mod for that session.
- The release targets Steam PC v1.10.89.0 only.
