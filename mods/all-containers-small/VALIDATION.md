# All Containers Small 1.0.0 validation

Supported target: Steam DS2.exe 1.10.89.0, SHA256 `bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b`.

The author tested development build 0.1.1 on 12 November 2026 and supplied the two gallery screenshots. Confirmed: successful startup, cargo collection with S cases, S size labels in inventory, unchanged displayed weights and compact cases on Sam's backpack. Resins [320], Ceramics [320] and Chemicals [240] appear as S while retaining their material quantities and 32/32/24 kg weights.

The public 1.0.0 runtime differs from the confirmed build only in its version log string. Its 17 local tests cover size mutation, catalogue validation, native cache and selector behavior, preservation of functional objects, the previously observed Cart crash path and helper-process log isolation. The native Cart regression uses synthetic component storage and is not a full game simulation.

The resource checks covered 2,058 exported definitions: 833 converted, 1,150 already S, 75 additional non-S definitions preserved. Native model and appearance checks cover 90 and 450 combinations respectively.

Not comprehensively tested: every equipment deployment path, every mission-specific object, long sessions, or saving/reloading cargo repacked with this mod. These limits apply to the release; the screenshots are not evidence of every possible combination.

The release archive contains only the ASI, English/German instructions, optional installer and manifest. No original game executable, extracted game resources, crash dumps, private logs or credentials are included. Gallery PNGs are the author's supplied screenshots, copied without altering the gameplay image.
