# Offline interface translations

The manager embeds ten UTF-8 JSON dictionaries: `en`, `de`, `fr`, `es`, `it`, `pt-BR`, `ru`, `zh-CN`, `ja` and `ko`. Each contains 553 strings. English source text is the stable lookup key; all dictionaries must contain the same keys. Language selection is stored separately from the central mod profile and passed to elevated installation operations.

`Localization.T` handles interface strings, catalog descriptions/categories, settings labels, help and groups. Original mod names, paths, INI sections/keys, enum tokens and saved values are not translated. A missing translation falls back to English. Low-level diagnostics and Windows messages can remain in their original language.

When adding visible text, add its key to every dictionary. Preserve format placeholders (`{0}`), leading/trailing spaces used by concatenated messages, filenames and file-dialog filter patterns. Keep the underlying machine choices (`0`, `1`, `inherit`, scan modes and language codes) unchanged. Short choice labels must fit the settings editor at its minimum window size.

Run `python installer/tools/check-locales.py` from the repository root, then `installer/build.ps1`. The executable self-test validates complete key sets, placeholders, catalog/settings coverage, persistence and byte-identical generated INIs across all ten languages. `tools/verify-languages-ui.ps1` renders the actual WPF main/settings windows offscreen at their minimum sizes and checks Crafting choices against their literal stored values; run it with Windows PowerShell in STA mode. It uses an isolated settings directory and never opens an interactive window or modifies the game.

Translations are AI-assisted, with terminology, safety guidance, formatting and selected rendered screens reviewed. They have not received comprehensive native-speaker or official game-terminology certification. Corrections are welcome in these dictionaries; no network translation service is used by the manager.
