# Security and save notes

- The release patches only the running `DS2.exe` process; it does not modify the executable on disk.
- It does not edit save files directly.
- It uses DS2's native component entries and persistence path.
- The ASI validates all supported patch sites before activation and fails closed on mismatch.
- Keep a normal backup save before first use.
- Before uninstalling, reduce the layout to one that fits the original component footprints and save again.
