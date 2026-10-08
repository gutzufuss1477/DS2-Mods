# Sam Overhaul v1.1.0-dev.1 – Entwicklungsstand

Konfigurierbarer All-in-One-QoL-Mod für **DEATH STRANDING 2: ON THE BEACH**, Zielversion Steam PC **1.10.89.0**.

Dies ist ein Git-Entwicklungsbuild, kein neues Nexus-Release. Das veröffentlichte Paket v1.0.0 bleibt unverändert.

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

`DS2_Sam_Overhaul_v1.1.0-dev.1.asi` und `ds2_sam_overhaul.ini` gehören direkt neben DS2.exe. Der bestehende x64-ASI-Loader wird weiterverwendet. Eine bereits angepasste INI kann behalten werden; nur die neue Sektion ergänzen.

## Prüfstand

Der Filterkern wurde separat im Spiel für neue Spuren und einen zuvor spurenreichen Spielstand bestätigt. Die normale Bodenabdruck-Darstellung war nach Beobachtung des Testers ebenfalls verschwunden.

Für den integrierten Build bestanden die bisherigen 105’704 Autodrive-Prüfungen, die Fussabdruck-ABI- und Parallelitätstests, 104 Konfigurations-/Schutzprüfungen, ein DLL-Ladetest und die Kontrolle der Abhängigkeiten und Standardwerte. **Der gemeinsame Spieltest mit allen Sam-Overhaul-Funktionen steht noch aus.** Details: `docs/FOOTPRINTS_VALIDATION.md`.

Der Build liegt unter `development/DS2_Sam_Overhaul_v1.1.0-dev.1/`. `scripts/build-development.ps1` baut und prüft ihn erneut. Das Skript installiert nichts im Spiel und erstellt kein Nexus-Release.
