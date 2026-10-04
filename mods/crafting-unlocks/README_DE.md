# Crafting & Equipment Overhaul 1.7.0

Frühe Freischaltungen, kostenlose Herstellung, Ausrüstungshaltbarkeit, ATLAS-Ausrüstung und vier rote schallgedämpfte Waffen in einer ASI mit einer INI für DEATH STRANDING 2: ON THE BEACH.

## Enthaltene Funktionen

- 132 einzeln einstellbare Originalrezepte/-gegenstände, einschließlich Rucksackmodulen, Hüllen und Anhängern.
- 11 nutzbare Gegnerwaffen einschließlich Ghost Blade sowie herstellbare Omnireflektor- und chirale Stiefel.
- ATLAS-Stiefel kombinieren Halt, Stoßabsorption, stärkere Tritte und leise Schritte.
- ATLAS-Skelett kombiniert Kampf-, Boost- und Bokka-Effekte auf Stufe 3, einschließlich Doppelsprung und gleichzeitiger Hüftfracht.
- Vier zusätzliche schallgedämpfte Waffen mit schwarz/roten Modellen, roten Menübildern und sichtbaren Schalldämpfern.
- Gemeinsame Optionen für kostenlose Herstellung, Haltbarkeitsmultiplikator (1,0–1000,0) und Unzerstörbarkeit. Diese gelten auch für ATLAS und die neuen Waffen.

## Neue schallgedämpfte Waffen

| Eigene Variante | Basis |
|---|---|
| Schallgedämpftes Sturmgewehr [MZ] St.2 | Standard St.2, 42 Schuss |
| Schallgedämpftes Maschinengewehr [MZ] St.2 | Standard St.2, 120 Schuss |
| Schallgedämpfte Schrotflinte [MZ] St.2 | Standard St.2, 10 Schuss |
| Schallgedämpfte Großkaliber-Handfeuerwaffe [MZ] | Standard-Großkaliber-Handfeuerwaffe |

Es sind eigene herstellbare Gegenstände. Schaden, Feuerrate, Rückstoß, Projektile, Magazingröße, Gewicht und Materialkosten entsprechen der Basis. Die Granatwerfer von Sturmgewehr und Schrotflinte bleiben unverändert und ungedämpft. MG und Schrotflinte verwenden vorhandenen gedämpften Gewehrsound, die Handfeuerwaffe vorhandenen gedämpften Pistolensound. Einschläge, Explosionen und sichtbare Angriffe können Gegner weiterhin alarmieren. Originalwaffen behalten ihre Darstellung und Werte.

Die zusätzliche Test-Maschinenpistole ist nicht mehr herstellbar. Ihre Definition bleibt für gespeicherte Verweise intern erhalten. Alte IDs werden nicht neu vergeben.

## ATLAS und Voreinstellungen

Die Nexus-INI aktiviert ATLAS-Rezepte, den goldenen ATLAS-Skelett-Look und alle vier neuen Waffen. Kostenlose Herstellung und Haltbarkeitsänderungen sind standardmäßig aus.

```ini
[AtlasEquipment]
Enabled=1
GoldSkeletonSkin=1

[SuppressedWeapons]
Enabled=1
AssaultRifleL2=1
MachineGunL2=1
ShotgunL2=1
BigBoreHandgun=1
BlackRed=1
MGVisualSuppressor=1
ShotgunVisualSuppressor=1
BigBoreVisualSuppressor=1
Language=0
```

`GoldSkeletonSkin=0` schaltet auf den normalen Boost-Skelett-St.3-Look zurück. Dies betrifft Menüvorschau und getragenes ATLAS-Skelett; die kombinierten Effekte bleiben erhalten. `BlackRed=0` verwendet die ursprünglichen Waffenfarben/-bilder. Die drei VisualSuppressor-Schalter steuern nur die sichtbaren Anbauten. `Language`: automatisch (`0`), Deutsch (`1`) oder Englisch (`2`).

ATLAS-Stiefel verwenden den Pizzabäcker-Look, wiegen 0,2 kg und haben 3400 Basishaltbarkeit. Das ATLAS-Skelett wiegt 4,0 kg und hat 20000 Basishaltbarkeit. Der Tragkraftbonus beträgt bis zu 180 kg mit Akku und 100 kg ohne Akku; die Boni werden nicht addiert. Akkuverbrauch und native Sprungbedingungen bleiben bestehen. Bei kurzen Sprüngen kann der Doppelsprung-Hinweis spät erscheinen.

ATLAS und neue Waffen haben keine zusätzlichen Story- oder Vorlagen-Freischaltbedingungen, sobald ein reguläres Herstellungsmenü verfügbar ist. ATLAS verwendet die Materialkosten von Transportstiefeln/Kampfskelett St.1. Die eigenen Rezeptschalter sind unabhängig von `DefaultUnlock`. Der normale Fortschritt bleibt erhalten. Das schwere Maschinengewehr [MZ] bleibt wegen der fehlenden regulären Inventarübergabe ausgeschlossen.

## Installation und Update

1. Spiel beenden und die bisherige Mod-INI sichern.
2. Falls vorhanden, `ds2_overpowered_equipment.asi` und `ds2_more_silenced_guns.asi` entfernen. Ihre Funktionen sind integriert; diese Dateien dürfen nicht gleichzeitig geladen werden.
3. `ds2_crafting_unlocks.asi` und `ds2_crafting_unlocks.ini` neben `DS2.exe` kopieren. Ein kompatibler 64-Bit-ASI-Loader wird separat benötigt.
4. Für die neuen Voreinstellungen die mitgelieferte INI verwenden und persönliche Einstellungen erneut übernehmen. Wer seine alte INI behält, ergänzt die obigen Abschnitte bzw. nur die fehlenden Schlüssel. Keine Schlüssel doppelt eintragen. Ohne neue Optionen bleiben die bisherigen INIs bei ihren bisherigen Freischaltungen und dem normalen ATLAS-Look.
5. Nach jeder INI-Änderung das Spiel vollständig neu starten.

`[CraftingUnlocks] Enabled=0` deaktiviert die gesamte Mod. `[AtlasEquipment] Enabled=0` und `[SuppressedWeapons] Enabled=0` blenden nur die jeweiligen Rezepte aus; vorhandene Gegenstände bleiben unterstützt. Einzelne Waffenschalter verhalten sich ebenso. Der globale Schalter muss dafür aktiv bleiben.

ATLAS-IDs 103/104 und Waffen-IDs 300/301/303/304 sowie die ausgeblendete Altdefinition 302 bleiben erhalten. Zum Deaktivieren neuer Herstellung die Rezeptschalter verwenden. Entfernen der ASI oder Downgrades entfernen eigene Definitionen; vor der Deinstallation zu einem Spielstand vor Nutzung der Mod zurückkehren. Fehlende alte Waffendefinitionen haben bereits einen Startabsturz verursacht und werden deshalb beibehalten.

## Kompatibilität und Prüfung

Unterstützt: DS2 1.10.89.0, SHA-256 `bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b`. EXE und Eingriffspunkte werden exakt geprüft. Der Installer verändert keine Spielarchive oder Spielstände.

Der Nutzer hat den separaten Waffenbuild akzeptiert. ATLAS-Gold, Menü-/Spielansicht und Speichern/Laden sind mit 1.6.1 bestätigt. Der Gesamtbuild 1.7.0 besteht die nativen ATLAS- und Waffentests, echte GPU-Uploads mit Rücklesen, gemeinsame Konfigurations-/Kosten-/Haltbarkeitstests und Windows-Ladetests. Alle 94 Eingriffspunkte sind auf Signaturen und Überschneidungen geprüft. Für die zusammengeführte ASI steht der Spieltest nach dem Update noch aus; längere Stabilität und vollständiges KI-Hörverhalten sind damit nicht belegt. Einzelne Stiefelwirkungen und Kampfschutz wurden nicht separat vollständig nachgewiesen.
