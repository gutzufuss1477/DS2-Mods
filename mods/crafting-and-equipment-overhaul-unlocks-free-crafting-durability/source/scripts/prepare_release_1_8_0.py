"""Prepare the existing Crafting & Equipment Overhaul for its 1.8.0 PCC update.

This script modifies source and documentation inside this mod only.
It does not touch the game directory or user saves.
"""
from pathlib import Path
import re
import zipfile
import subprocess

ROOT = Path(__file__).resolve().parents[1]
VERSION = "1.8.0"
OLD_ZIP = ROOT / "release/DS2_Crafting_Equipment_Overhaul_v1.7.0.zip"
assert OLD_ZIP.is_file(), "Public v1.7.0 archive is required for an immutable baseline"
with zipfile.ZipFile(OLD_ZIP) as z:
    baseline_ini = z.read("ds2_crafting_unlocks.ini")
    old_en = z.read("README_EN.txt").decode("utf-8-sig")
    old_de = z.read("README_DE.txt").decode("utf-8-sig")
    old_log = z.read("CHANGELOG.txt").decode("utf-8-sig")
assert b"[PCCStructures]" not in baseline_ini
assert old_en.startswith("# Crafting & Equipment Overhaul 1.7.0")
assert old_de.startswith("# Crafting & Equipment Overhaul 1.7.0")

def put(name, data):
    p = ROOT / name
    p.parent.mkdir(parents=True, exist_ok=True)
    if isinstance(data, str):
        p.write_text(data, encoding="utf-8", newline="\n")
    else:
        p.write_bytes(data)
    print("UPDATED", name)

def replace_once(text, old, new, description):
    assert text.count(old) == 1, f"{description}: expected 1 occurrence, got {text.count(old)}"
    return text.replace(old, new, 1)

# Native code: same exact hook, signatures and logic as the verified dev1 build.
put("src/version.hpp", '#pragma once\n#define CRAFT_VERSION "1.8.0"\n')
runtime = (ROOT / "src/pcc_runtime.inl").read_text(encoding="utf-8")
runtime = runtime.replace("Development test build.", "Standard construction unlocks; native placement retained.")
put("src/pcc_runtime.inl", runtime)

# New public Nexus INI retains every 1.7.0 setting, with one new opt-out option.
public_ini = baseline_ini + (
    b"\r\n; NEW in 1.8.0: Early PCC structure choice unlocks (native PCC level is retained).\r\n"
    b"[PCCStructures]\r\n"
    b"UnlockAll=1            ; 1 = unlock standard building choices early; 0 = native story progression.\r\n"
    b"; Hot Spring Digger and special mission structures excluded.\r\n"
    b"; Network, bandwidth, materials, terrain, location rules and bridge lengths remain native.\r\n"
)
assert public_ini.startswith(baseline_ini) and public_ini.count(b"[PCCStructures]") == 1
put("build/release-1.8.0/ds2_crafting_unlocks.ini", public_ini)

en_section = """## Early PCC structures — new in 1.8.0

The new `[PCCStructures] UnlockAll=1` option unlocks the **native, standard PCC construction choices** ahead of their story progression. It works with PCC Lv.1, Lv.2 and Lv.3, including PCC levels fabricated early through the existing [Items] configuration.

- 11 ordinary construction categories (13 native variants): Postbox, Watchtower, Generator, Timefall Shelter, Bridge, Jump Ramp, Zipline, Chiral Bridge, Safehouse, Cargo Catapult and Transponder/Fast Travel Station.
- **PCC tiers are preserved:** Lv.1 keeps its five native choices; Lv.2 and Lv.3 unlock choices available to that tier. Native higher-level bridge/safehouse variants remain as designed.
- Native building costs, collision/terrain checks, chiral network requirements, bandwidth and story-specific world restrictions still apply. The mod does not unlock 80-m bridge variants or automatically upgrade structure levels.
- Hot Spring Digger/Boring Machine, special mission/hostile structures and any non-standard construction options remain under their original game rules.
- **Enabled by default in the supplied 1.8.0 INI.** Set `UnlockAll=0` to preserve normal progression. If you retain an older INI without this section, the new option stays off. Restart the game after a change.

**In-game check:** In Episode 2, standard locked PCC options were visible and a Safehouse could be placed successfully. Other construction categories were not all individually built and tested. This does not promise unrestricted placement.

This feature is complementary to structure level-up mods: it makes a building **selectable**, while those mods can change the building's **upgrade level**.

```ini
[PCCStructures]
UnlockAll=1
```

"""
de_section = """## PCC-Bauwerke früh freischalten — neu in 1.8.0

Mit `[PCCStructures] UnlockAll=1` werden die **normalen PCC-Bauwerktypen** bereits vor ihrer Story-Freischaltung zur Auswahl angeboten. Das funktioniert mit PCC Stufe 1, 2 und 3, auch wenn du diese über die bestehende [Items]-Funktion vorzeitig herstellen kannst.

- 11 normale Baukategorien (13 native Varianten): Postbox, Wachturm, Generator, Zeitregen-Unterstand, Brücke, Sprungrampe, Seilrutsche, Chiralbrücke, Schutzhütte, Frachtkatapult und Transponder/Fernreisestation.
- **PCC-Stufen bleiben erhalten:** Stufe 1 bietet ihre fünf vorgesehenen Möglichkeiten. Stufe 2 und 3 schalten ausschliesslich ihre jeweils eigenen Bauwerktypen frei. Ihre Brücken- und Schutzhüttenvarianten bleiben unterschiedlich.
- Baukosten, Gelände/Kollision, chirales Netzwerk, Bandbreite sowie besondere Bauverbote bleiben unverändert. Die Freischaltung für die 80-m-Brücke und der Ausbaugrad von Bauwerken werden nicht verändert.
- Thermalquellenbohrer/Hot Spring Digger sowie besondere Missions- und Gegnerbauwerke werden ausdrücklich nicht zusätzlich freigeschaltet.
- **In der mitgelieferten INI standardmässig aktiviert.** `UnlockAll=0` erhält den normalen Spielfortschritt. Wird eine alte INI ohne diesen Abschnitt beibehalten, bleibt die neue Funktion aus. Nach Änderungen neu starten.

**Im Spiel geprüft:** Bereits in Episode 2 waren zuvor gesperrte Bauwerktypen sichtbar und eine Schutzhütte liess sich erfolgreich platzieren. Nicht alle Bauwerktypen wurden einzeln gebaut und geprüft; die normalen Platzierungsregeln gelten weiterhin.

Diese Funktion ergänzt Mods für den automatischen maximalen Ausbaustand: Hier wird die **Auswahl** freigeschaltet, während der andere Mod die **Ausbaustufe** verändert.

```ini
[PCCStructures]
UnlockAll=1
```

"""
en = old_en.replace("1.7.0", VERSION)
en = replace_once(en, "Early unlocks, Free Crafting, equipment durability, ATLAS gear and four red suppressed weapons",
                  "Early unlocks for PCC structures and items, Free Crafting, equipment durability, ATLAS gear and four red suppressed weapons",
                  "English summary")
en = replace_once(en, "## New suppressed weapons", en_section + "## New suppressed weapons", "English section")
en = replace_once(en, "- Four additional suppressed weapons with black/red bodies, red menu images and visible suppressors.",
                  "- Four additional suppressed weapons with black/red bodies, red menu images and visible suppressors.\n- Early unlocks for native PCC structure choices at all three PCC levels (opt-out in INI).",
                  "English features")
en = en.replace("The supplied Nexus INI enables ATLAS recipes, the gold ATLAS Skeleton skin and all four suppressed weapons.",
                "The supplied Nexus INI enables early PCC structures, ATLAS recipes, the gold ATLAS Skeleton skin and all four suppressed weapons.")
en = en.replace("If keeping an older INI, add the ATLAS/weapon sections above or just the missing keys; do not duplicate existing keys.",
                "If keeping an older INI, add [PCCStructures] UnlockAll=1 to enable the new feature, plus any missing ATLAS/weapon keys; do not duplicate existing keys.")
en = replace_once(en, "The integrated 1.8.0 build passes", "The integrated 1.8.0 build passes", "English validation")
en = en.replace("All 94 native patch sites", "All 95 native patch sites")
en = replace_once(en,
                  "The combined 1.8.0 ASI still needs a gameplay check after migration; these local tests do not establish long-session stability or complete AI hearing behavior.",
                  "Early PCC construction selection and actual Safehouse placement have been confirmed in Episode 2. The 1.8.0 release is built from the tested PCC implementation and passed the combined native regression suite; exhaustive construction/long-session testing has not been completed.",
                  "English gameplay status")
put("README.md", en)
put("release/README_EN.txt", en)

de = old_de.replace("1.7.0", VERSION)
de = replace_once(de, "Frühe Freischaltungen, kostenlose Herstellung, Ausrüstungshaltbarkeit",
                  "Frühe Freischaltungen von PCC-Bauwerken und Gegenständen, kostenlose Herstellung, Ausrüstungshaltbarkeit",
                  "German summary")
de = replace_once(de, "## Neue schallgedämpfte Waffen", de_section + "## Neue schallgedämpfte Waffen", "German section")
de = replace_once(de, "- Vier zusätzliche schallgedämpfte Waffen mit schwarz/roten Modellen, roten Menübildern und sichtbaren Schalldämpfern.",
                  "- Vier zusätzliche schallgedämpfte Waffen mit schwarz/roten Modellen, roten Menübildern und sichtbaren Schalldämpfern.\n- Frühe Freischaltung der normalen PCC-Bauwerktypen auf allen drei PCC-Stufen.",
                  "German features")
de = de.replace("Die Nexus-INI aktiviert ATLAS-Rezepte, den goldenen ATLAS-Skelett-Look",
                "Die Nexus-INI aktiviert die frühen PCC-Bauwerke, ATLAS-Rezepte, den goldenen ATLAS-Skelett-Look")
de = de.replace("Wer seine alte INI behält, ergänzt die obigen Abschnitte bzw. nur die fehlenden Schlüssel.",
                "Wer seine alte INI behält, ergänzt [PCCStructures] UnlockAll=1 und weitere fehlende Schlüssel.")
de = de.replace("Alle 94 Eingriffspunkte", "Alle 95 Eingriffspunkte")
de = replace_once(de,
                 "Für die zusammengeführte ASI steht der Spieltest nach dem Update noch aus; längere Stabilität und vollständiges KI-Hörverhalten sind damit nicht belegt.",
                 "Die PCC-Bauauswahl und die Platzierung einer Schutzhütte in Episode 2 wurden im Spiel bestätigt. Die 1.8.0-Version verwendet denselben geprüften PCC-Eingriff; nicht sämtliche Bauwerke und längere Spielsitzungen wurden getestet.",
                 "German gameplay status")
put("README_DE.md", de)
put("release/README_DE.txt", de)

new_changes = """## 1.8.0 - Early PCC structure unlocks

- Added [PCCStructures] UnlockAll=1/0 to reveal standard PCC construction choices before their story unlock; enabled in the supplied Nexus INI.
- Works with native PCC Lv.1, Lv.2 and Lv.3 selection lists: 11 building categories represented by 13 original game variants. No custom or invalid construction IDs are inserted.
- Retains the game's placement, bandwidth, chiral network, terrain, material and PCC tier restrictions. Hot Spring Digger, mission-specific structures and 80-m bridge unlocks remain native.
- Verified in an early Episode 2 save: locked choices appeared and a Safehouse could actually be placed. Other building types still rely on native constraints and are not all individually game-tested.
- Preserves all 1.7.0 crafting, FreeCrafting, ATLAS, suppressed weapons, backpack and durability features, including stable custom item identities. No save edits or broad story flag changes.

"""
assert "## 1.7.0" in old_log
put("CHANGELOG.md", "# Changelog\n\n" + new_changes + old_log[old_log.index("## 1.7.0"): ])
put("release/CHANGELOG.txt", "Crafting & Equipment Overhaul\n\n" + new_changes + old_log[old_log.index("## 1.7.0"): ])

# Nexus metadata: update existing page, do not create a standalone PCC mod page.
put("docs/NEXUS_TITLE.txt", "Crafting and Equipment Overhaul - Early PCC, ATLAS, Suppressed Weapons\n")
short = ("Unlock standard PCC buildings early, alongside 132 craftable items, ATLAS gear and four suppressed weapons. "
         "Includes Free Crafting, equipment durability and an INI toggle. One ASI, one INI.\n")
assert len(short.strip()) < 255
put("docs/NEXUS_SHORT_DESCRIPTION.txt", short)

nexus_new = """[b]NEW in 1.8.0 - Early PCC Structure Unlocks[/b]
[list]
[*]Use PCC Lv.1, Lv.2 or Lv.3 to select standard building types ahead of story progression.
[*]13 native building variants across 11 structure categories, including Zipline, Safehouse, Jump Ramp, Chiral Bridge, Cargo Catapult and Transponder.
[*]Each PCC level keeps its own native selection list; no out-of-tier choices are injected.
[*]The original building costs, bandwidth, terrain, chiral network and placement restrictions remain active.
[*]Hot Spring Digger and special mission constructions are not unlocked. Bridge length/story restrictions remain native.
[/list]

[b]Enabled by default[/b] in the bundled INI. Set `UnlockAll=0` to keep the normal progression:
[code][PCCStructures]
UnlockAll=1[/code]

[b]In-game confirmation:[/b] Early Episode 2: previously locked choices appeared, and a Safehouse was successfully placed. All structure types have not been individually placed and tested.

"""
bb = subprocess.check_output(["git", "-C", str(ROOT.parent.parent), "show", "e0b801a:mods/crafting-unlocks/docs/NEXUS_DESCRIPTION_BBCODE.txt"]).decode("utf-8")
bb = replace_once(bb, "Version 1.7.0", "Version 1.8.0", "Nexus BBCode version")
bb = replace_once(bb, "Suppressed Weapons\n\n", "Suppressed Weapons and Early PCC Buildings\n\n", "Nexus heading")
bb = replace_once(bb, "[b]New Suppressed Weapons[/b]", nexus_new + "[b]New Suppressed Weapons[/b]", "Nexus BBCode section")
bb = replace_once(bb, "[*]Four new suppressed weapons with black/red models, red crafting-menu images and visible suppressors.",
                  "[*]Four new suppressed weapons with black/red models, red crafting-menu images and visible suppressors.\n[*]Early PCC structure unlocks for all three native PCC levels.",
                  "Nexus list")
bb = replace_once(bb, "The supplied INI enables ATLAS recipes and all four red suppressed weapons.",
                  "The supplied INI enables early PCC structures, ATLAS recipes and all four red suppressed weapons.",
                  "Nexus defaults")
bb = bb.replace("If keeping an older INI, add the missing options from the new file without duplicating existing keys.",
                "If keeping an older INI, add [b][PCCStructures] UnlockAll=1[/b] to enable early PCC options; do not duplicate keys.")
bb = bb.replace("Older INIs without the new options retain their previous behavior;",
                "Older INIs without [PCCStructures] keep native PCC story progression;")
bb = replace_once(bb, "Local checks for the combined 1.7.0 build passed; its final combined gameplay test is still pending.",
                  "The PCC feature was tested in Episode 2 with a successful Safehouse placement. All 95 combined native patch sites passed offline checks; some structure types and long play sessions remain untested.",
                  "Nexus compatibility")
assert "[code][PCCStructures]" in bb and "[/code]" in bb
put("docs/NEXUS_DESCRIPTION_BBCODE.txt", bb)
plain = subprocess.check_output(["git", "-C", str(ROOT.parent.parent), "show", "e0b801a:mods/crafting-unlocks/docs/NEXUS_DESCRIPTION.txt"]).decode("utf-8")
plain = replace_once(plain, "Version 1.7.0", "Version 1.8.0", "Nexus plaintext version")
plain = plain.replace("Suppressed Weapons\n\n", "Suppressed Weapons and Early PCC Buildings\n\n", 1)
plain_section = ("""New in 1.8.0 - Early PCC Structure Unlocks

Unlock the 11 standard construction categories (13 native variants) ahead of story progression while preserving PCC Lv.1/2/3 native lists, materials, terrain and chiral bandwidth restrictions. The Hot Spring Digger and special mission structures are excluded. The new option is enabled in the supplied INI and can be disabled:

[PCCStructures]
UnlockAll=1

Tested in Episode 2: previously locked structures appeared, and a Safehouse could be placed. Not every building type has been tested.

""")
plain = replace_once(plain, "New Suppressed Weapons", plain_section + "New Suppressed Weapons", "Nexus plain section")
plain = plain.replace("Local checks for the combined 1.7.0 build passed; its final combined gameplay test is still pending.",
                      "In-game PCC selection and Safehouse placement were verified in Episode 2; all 95 native patch sites pass exact-byte checks. Individual building types and long-session stability are not exhaustively verified.")
put("docs/NEXUS_DESCRIPTION.txt", plain)
put("docs/NEXUS_CHANGELOG_1.8.0.txt", """1.8.0 - Early PCC structure unlocks

- Unlock normal PCC building choices before story progression, with [PCCStructures] UnlockAll=1 enabled by default in the new INI.
- Supports PCC Lv.1, Lv.2 and Lv.3 within each tier's existing building list: 13 game-native variants covering 11 building categories.
- Retains normal terrain, chiral network, bandwidth, resource costs and placement checks. Hot Spring Digger and special mission structures excluded.
- Confirmed in Episode 2: locked choices visible and Safehouse successfully placed.
- Preserves all 1.7.0 Crafting, ATLAS, weapon, backpack, FreeCrafting and durability features; unchanged custom saved item IDs.
""")
print("PREPARATION_COMPLETE: existing mod updated; build, regressions and release ZIP still pending")
