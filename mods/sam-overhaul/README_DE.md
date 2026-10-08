# Sam Overhaul v1.1.0-dev.19 – Entwicklungsstand

Konfigurierbarer All-in-One-Mod für **DEATH STRANDING 2: ON THE BEACH**, Steam PC **1.10.89.0**.
Dies ist ein Entwicklungsbuild; das veröffentlichte Paket v1.0.0 und Nexus bleiben unverändert.

## Bauwerks-Reichweiten (optional, standardmässig aus)

- **Generator:** `[GeneratorRange] Enabled=1, RangePercent=200` verdoppelt die native Jolt-Ladereichweite und passt den blauen Odradek-Kreis an. Für den getesteten Generator wurde die Wirkung im Spiel und nach Neuladen bestätigt. Weitere Bauwerksstufen benötigen Regressionstests.
- **Zeitregenunterstand:** `[TimefallShelterRange] Enabled=1, RangePercent=200` setzt den nativen Regen-Schutzradius, die zwei Schutzzylinder und den echten Frachtreparaturradius von 4 auf 8 m. Die Reparaturwolke und das Wiederherstellen auf 100 % wurden ausserhalb des Vanilla-Radius bestätigt. **Am 09.10.2026 wurde der grössere sichtbare Kreis von dev.19 im Spiel als passend zum Reparatureffekt bestätigt.** Die native Odradek-Aktivierungsdistanz von 30 m bleibt unverändert.

Die gesamte Kombination, weitere Gebäudestufen, Save-Reload des Unterstands und die Performance müssen noch umfassend geprüft werden.

## Neue Option: Fussabdrücke ausblenden

In der bestehenden `ds2_sam_overhaul.ini` ergänzen:

```ini
[Footprints]
HideFootprints=1
```

`1` blendet neue Fussabdrücke sowie die beim Laden eines Spielstands wieder aufgebauten Abdrücke aus. Das betrifft sowohl die normalen Bodenabdrücke als auch die beim Odradek-Scan hervorgehobenen Spuren. `0` lässt die normale Darstellung unverändert. Die mitgelieferte INI verwendet **0**; bei alten INIs ohne diesen Eintrag bleibt die Funktion ebenfalls aus.

Nach einer Änderung das Spiel neu starten. Der Filter löscht keine Save-Dateien und schreibt die ursprüngliche DS2.exe nicht um. Es handelt sich nicht um einen zusätzlichen Lösch-Patch für bereits gerenderte Objekte im laufenden Spiel.

Die Funktion ist direkt in Sam Overhaul integriert. **Keine separate Fussabdruck-ASI, kein ReShade und kein ShaderToggler erforderlich.**

## Bisherige Funktionen

Schulter-, Hüft- und Rucksack-Cargo sowie Ersatzschuhe lassen sich weiterhin ausblenden. Getragene Schuhe bleiben sichtbar. Die Monorail- und Zipline-Ausstiegsoptionen, die qualifizierende Landerolle mit Rucksack, die einstellbare Autodrive-Aktivierungszeit und das Tuning der vier Truck-Waffen bleiben enthalten. Alle 33 bisherigen INI-Standardwerte wurden beibehalten.

## Installation des Entwicklungsbuilds

Spiel schliessen. Die bisherige Sam-Overhaul-ASI und die separate Testdatei `ds2_footprint_native_probe.asi` entfernen oder ausserhalb des Spielverzeichnisses sichern. Nicht mehrere Sam-Overhaul-Versionen gleichzeitig laden.

`DS2_Sam_Overhaul_v1.1.0-dev.19.asi` und `ds2_sam_overhaul.ini` gehören direkt neben DS2.exe. Der bestehende x64-ASI-Loader wird weiterverwendet. Eine bereits angepasste INI kann behalten werden; nur die neue Sektion ergänzen.

## Prüfstand

Der Filterkern wurde separat im Spiel für neue Spuren und einen zuvor spurenreichen Spielstand bestätigt. Die normale Bodenabdruck-Darstellung war nach Beobachtung des Testers ebenfalls verschwunden.

Für den integrierten Build bestanden die bisherigen 105’704 Autodrive-Prüfungen, die Fussabdruck-ABI- und Parallelitätstests, 104 Konfigurations-/Schutzprüfungen, ein DLL-Ladetest und die Kontrolle der Abhängigkeiten und Standardwerte. **Der gemeinsame Spieltest ist teilweise bestätigt (Unterstand-Kreis und Effekt in dev.19); die vollständige Regression steht noch aus.** Details: `docs/FOOTPRINTS_VALIDATION.md`.

Der Build liegt unter `development/DS2_Sam_Overhaul_v1.1.0-dev.19/`. `scripts/build-development.ps1` baut und prüft ihn erneut. Das Skript installiert nichts im Spiel und erstellt kein Nexus-Release.

## Native Zeitregenunterstand-Funktion – aktueller Stand

Das in dev.9/dev.10 falsch zugeordnete Kugelobjekt gehörte nicht zum Unterstand und wurde in dev.12 entfernt. Die aktuelle Lösung nutzt die tatsächlich vorhandenen nativen Regen- und Reparatur-Radiusfelder, nicht künstliche Reparaturkontakte. Der vergrösserte Effekt gehört zur passenden Unterstands-Instanz; die Originalwerte anderer Komponenten bleiben unberührt.

**Bestätigt:** Wetter-/Fracht-Schutzradius, Frachtreparatur auf 100 % ausserhalb des Vanilla-Radius und in dev.19 die Übereinstimmung von sichtbarem Kreis und Reparaturwolke. Der 200-%-Generator-Kreis wurde ebenfalls bereits im Spiel getestet.

**Offen:** Save-Reload mit dev.19, andere Bauwerksstufen, vollständiger Funktionstest der kombinierten Sam-Overhaul-ASI sowie die Auswertung hoher Discovery-Laufzeitwerte (in der letzten Messreihe Spitzen bis rund 192 ms). Solange dies nicht geklärt ist, gibt es keinen Nexus-Release.

Technisches Testprotokoll: `docs/CONSTRUCTION_RANGES_VALIDATION.md`.
