# DS2 Mod Suite 1.13.0

Updated for Crafting 1.10.1, optional ATLAS addons 1.0.1, Magellan 1.1.0 and
Climbing/Combat Gloves 1.1.2. Ten interface languages, central INI migration,
dependency selection and tested installation/removal.

Both **1.13.0** uploads are **quarantined on Nexus**: full file **398** and no-APAS comparison **399**. Neither is available for download. The previous 1.12.1 no-APAS file **379** remains primary; this does not certify its current antivirus status.

[Current Nexus files](https://www.nexusmods.com/deathstranding2onthebeach/mods/72?tab=files)
· [Build instructions](source/BUILD.md)
· [Validation](source/publication/v1.13.0/VALIDATION.md)
· [Scan evidence](source/publication/v1.13.0/scan-results.json)

The full source catalog contains 29 components and 398 settings, including the
unchanged published APAS 3.0.0 binary. Earlier experimental APAS rewrites are not
packaged. The no-APAS variant contains 28 components and 394 settings and leaves
existing standalone APAS files untouched.

Both variants passed the complete regression and language tests. Local Defender
reported no threats, but VirusTotal reports detections for several bundled
components and for both the old and new manager executables. These findings
remain unresolved; no confirmed false-positive claim is made.

ATLAS addons select the required Crafting mod and are deselected with it.
Existing INIs, user choices and personal Story Coffin metadata are preserved.
New Warrior recipe overrides follow DefaultUnlock unless explicitly configured.
No new in-game test was performed for the suite.

Git contains source, build instructions, component provenance and validation
records; compiled payloads and game assets remain outside the repository.
