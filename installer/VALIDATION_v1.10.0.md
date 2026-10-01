# DS2 Mod Suite 1.10.0 validation

Validated on Windows on 2026-10-01, based on `origin/main` at `50e7b7f`.

## Scope

Eight new interface languages alongside English and German: French, Spanish, Italian, Brazilian Portuguese, Russian, Simplified Chinese, Japanese and Korean. Ten embedded UTF-8 dictionaries contain 553 strings each. Interface, catalog and setting text is translated; mod identities and configuration keys/values stay invariant. Native language names, saved selection, culture-aware formatting, suitable CJK fonts and wrapping layouts are included.

The mod catalog is identical to v1.9.0 apart from the suite version. The settings schema and all mod/loader payload bytes are unchanged: 24 distinct mods, 25 selectable entries, 325 settings and 21 INIs.

## Verification

- Language checks passed for equal key sets, source string coverage, nonempty UTF-8 values, placeholders, concatenation whitespace and file-dialog patterns.
- Full executable self-tests passed, including all existing installation, migration, conflict, variant, backup, rollback and elevated-transport cases.
- Every language persists correctly without changing the selected game path. Native names, regional aliases, unknown-language fallback and missing-text fallback were checked.
- All 325 field identities/defaults remain unchanged across languages. All 21 generated INIs are byte-identical across all ten languages. The compressed elevated configuration payload is also identical.
- The actual WPF main window, Gloves settings and Crafting settings were rendered offscreen in all ten languages at minimum supported sizes. Forty images include scrolled Crafting choices. Five glove controls and 139 Crafting controls were present; displayed Crafting choices round-tripped to the literal `inherit` value. Selected French, Russian, Portuguese and CJK views were visually inspected.
- The final ZIP was extracted into a separate directory, checked against its complete SHA256SUMS manifest, and its executable passed the full self-test again.

No desktop/game control, real game-file modification or new in-game test was performed. Translations are AI-assisted; comprehensive native-speaker validation remains open. The existing untested-in-game status of Backpack Classic Overlap remains unchanged.

## Nexus text

The public description was rewritten from 1,862 to 493 words, grouping all included mods and retaining installation, central settings, APAS persistence, Backpack removal and compatibility guidance. Short description, main-file text and v1.10.0 changelog were updated together. Historical changelog entries remain available.

Archive: `DS2_Mod_Suite_v1.10.0.zip`

SHA-256: `F6A37FDDFA0A8388AAC83A4F5ECAC095B62744F8FC79991656E342EE21790EB5`
