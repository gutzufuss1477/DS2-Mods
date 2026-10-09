# Sam Overhaul v1.1.0-dev.28 – Entwicklungsstand

Konfigurierbarer All-in-One-Mod für **DEATH STRANDING 2: ON THE BEACH**, Steam PC **1.10.89.0**.
Dies ist ein Entwicklungsbuild; das veröffentlichte Paket v1.0.0 und Nexus bleiben unverändert.

## Bauwerks-Reichweiten (optional, standardmässig aus)

- **Generator:** `[GeneratorRange] Enabled=1, RangePercent=200` verdoppelt die native Jolt-Ladereichweite und passt den blauen Odradek-Kreis an. Für den getesteten Generator wurde die Wirkung im Spiel und nach Neuladen bestätigt. Weitere Bauwerksstufen benötigen Regressionstests.
- **Zeitregenunterstand:** `[TimefallShelterRange] Enabled=1, RangePercent=200` setzt den nativen Regen-Schutzradius, die zwei Schutzzylinder und den echten Frachtreparaturradius von 4 auf 8 m. Die Reparaturwolke und das Wiederherstellen auf 100 % wurden ausserhalb des Vanilla-Radius bestätigt. **Ein späterer Screenshot nach Neuladen zeigte die Reparatur erst innerhalb des vergrösserten Kreises; dev.20 misst die 2D-/3D-Distanzen ohne Fracht-Eingriffe.** Die native Odradek-Aktivierungsdistanz von 30 m bleibt unverändert.

Die gesamte Kombination, weitere Gebäudestufen, Save-Reload des Unterstands und die Performance müssen noch umfassend geprüft werden.

## Experimenteller dev.21-Fix am Hang

Die zwei geprüften Jolt-Kontaktzylinder hatten trotz 8 m horizontalem Radius weiterhin die Vanilla-Halbhöhe von 3 m. Die native Funktion `CylinderShape::GetLocalBounds` bestätigt die getrennten Dimensionen. dev.21 skaliert bei 200 % auch die vertikale Halbhöhe von 3 auf 6 m und aktualisiert die registrierten Jolt-Körper. Dies soll die seitlich asymmetrischen Kontakte am Hang verbessern, **muss aber noch im Spiel getestet werden**. Der sichtbare Kreis, die Frachtmechanik, die deutschen Interaktionstexte und andere Mods bleiben unverändert.

## Dev.23 – nativer Reparaturkontakt und Crash-Korrektur

Ein Live-Test in dev.21 hat gezeigt, dass die Reparatur von beiden Richtungen näher am vergrösserten Kreis einsetzt, sobald die ursprüngliche Kontakt-Vorbedingung wegfällt. Von oben blieb sie etwas innerhalb des sichtbaren Kreises. Diese temporäre globale Änderung wird **nicht** in den Mod übernommen.

Stattdessen prüft dev.23 über einen eng begrenzten nativen Assembler-Codepfad Komponententyp, Ressource, Bauwerks-Eigentümer und den erweiterten Reparaturradius. Nur passende Unterstands-Reparaturquellen können die alte Kontaktbedingung überspringen; andere Reparaturquellen behalten die Vanilla-Abfrage. Die eigentliche 3D-Distanzprüfung, Frachtreparatur, Regen- und Generatorlogik bleiben original. Gesteuert wird dies durch den bestehenden `TimefallShelterRange`-INI-Schalter. **dev.22 verursachte beim Laden einen Absturz durch ein um ein Byte falsches Sprungziel. dev.23 korrigiert die relative Sprungadresse und nutzt ausschliesslich bei aktiven Unterständen vorab validierte Reparaturquellen. dev.23 lädt ohne Crash, verliert jedoch nach einer Pause die Reparaturfreigaben wegen eines 13-Sekunden-Timers. dev.24 benutzt stattdessen die native DS2-Reparaturquellenliste und entfernt Einträge nur bei tatsächlichem Entladen bzw. Änderungen. Ingame-Test noch ausstehend.**

Die Texte «In Bunker ausruhen» und «Verschnaufen» gehören weiterhin zu verschiedenen Interaktionszuständen; daran ändert dev.22 nichts.

## Native Zeitregenunterstand-Beschriftung (dev.25)

Der bestätigte Live-Test ersetzt die irreführende Unterstands-Interaktion "In Bunker ausruhen" durch **"Verschnaufen"**. dev.25 übernimmt genau diese Korrektur in den vorhandenen Sam-Overhaul-Streaminglistener – ohne zusätzliche Mod oder externe DLL. Die originale Unterstands-Aktion, ihre Zeitsprung-/Ausruhfunktion sowie andere Bunker und Sprachen bleiben unverändert.

Der Schalter `[TimefallShelterRange] FixRestPrompt=1` ist automatisch aktiv, wenn die Unterstands-Reichweitenerweiterung eingeschaltet ist. Mit `FixRestPrompt=0` bleibt die Originalbeschriftung. Die neue dev.25-Integration benötigt noch einen Spieltest nach Neustart.

Die Reparaturwolke wird durch einen nativen **dreidimensionalen** 8-m-Radius ausgelöst. Am Hang kann sie deshalb etwas innerhalb des blauen Kreisrandes beginnen. An der bewährten Reparaturmechanik und am Kreis wird nichts mehr verändert.

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

`DS2_Sam_Overhaul_v1.1.0-dev.28.asi` und `ds2_sam_overhaul.ini` gehören direkt neben DS2.exe. Der bestehende x64-ASI-Loader wird weiterverwendet. Eine bereits angepasste INI kann behalten werden; nur die neue Sektion ergänzen.

## Prüfstand

Der Filterkern wurde separat im Spiel für neue Spuren und einen zuvor spurenreichen Spielstand bestätigt. Die normale Bodenabdruck-Darstellung war nach Beobachtung des Testers ebenfalls verschwunden.

Für den integrierten Build bestanden die bisherigen 105’704 Autodrive-Prüfungen, die Fussabdruck-ABI- und Parallelitätstests, 104 Konfigurations-/Schutzprüfungen, ein DLL-Ladetest und die Kontrolle der Abhängigkeiten und Standardwerte. **Nach dev.19 trat die Reparatur tiefer im sichtbaren Radius auf; dev.20 ist noch nicht im Spiel getestet. Die vollständige Regression steht aus.** Details: `docs/FOOTPRINTS_VALIDATION.md`.

Der Build liegt unter `development/DS2_Sam_Overhaul_v1.1.0-dev.28/`. `scripts/build-development.ps1` baut und prüft ihn erneut. Das Skript installiert nichts im Spiel und erstellt kein Nexus-Release.

## Native Zeitregenunterstand-Funktion – aktueller Stand

Das in dev.9/dev.10 falsch zugeordnete Kugelobjekt gehörte nicht zum Unterstand und wurde in dev.12 entfernt. Die aktuelle Lösung nutzt die tatsächlich vorhandenen nativen Regen- und Reparatur-Radiusfelder, nicht künstliche Reparaturkontakte. Der vergrösserte Effekt gehört zur passenden Unterstands-Instanz; die Originalwerte anderer Komponenten bleiben unberührt.

**Bestätigt:** Nativer Wetter-/Fracht-Schutzradius sowie Frachtreparatur auf 100 % ausserhalb des Vanilla-Radius. Die dauerhafte Übereinstimmung von Reparatur-Auslöser und sichtbarem Kreis ist nach dem dev.19-Screenshot ausdrücklich **nicht** bestätigt. Der 200-%-Generator-Kreis wurde ebenfalls bereits im Spiel getestet.

**Offen:** Messung des Reparaturauslösers am Rand nach Reload mit dev.20, andere Bauwerksstufen, vollständiger Funktionstest der kombinierten ASI sowie CPU-Performance (in dev.19 Spitzen bis rund 252 ms). Solange dies nicht geklärt ist, gibt es keinen Nexus-Release.

Technisches Testprotokoll: `docs/CONSTRUCTION_RANGES_VALIDATION.md`.

### Reparaturradius separat einstellen (dev.26)

Die Reichweite des blauen Kreises und der Regenabwehr bleibt bei RangePercent=200 (8 m). Der neue INI-Wert RepairRadiusPercent=215 entspricht einer Reparaturreichweite von 8,6 m. Dadurch trifft die räumlich gemessene Reparatur an einem Hang etwas früher ein und passt besser zum sichtbaren Kreis. Die Erweiterung ist optional; RepairRadiusPercent=200 stellt die bisherige 8-m-Reparatur wieder her. Der originale Zeitregenunterstand bleibt funktional unverändert.

Die dev.26-Textkorrektur ber?cksichtigt auch Ressourcen, die bereits vor dem Streaminglistener geladen wurden. Dazu arbeitet der bestehende Mod-Worker eine einzige begrenzte Suchrunde in kleinen Abschnitten ab und beendet diese nach dem Finden der beiden best?tigten Texte. Es ist kein zus?tzliches Lokalisierungsprogramm erforderlich.

### Performance-Optimierung der Zeitregenunterstände (dev.28)

Nach dem ersten vollständigen, erfolgreichen Abgleich eines Unterstands läuft eine kurze Identitäts- und Werteprüfung. Sie kontrolliert dieselben nativen Unterstands-, Reparatur- und Render-Objekte sowie beide Jolt-Körper inklusive BodyID. Nur bei identischen Objekten und gültigen 8-m-/8,6-m-/16-m-Werten werden redundante vollständige Objektprüfungen übersprungen. Bei einem neu geladenen, veränderten oder ungültigen Objekt greift sofort die bewährte vollständige Erkennung.

Der optionale Eintrag SpatialDiagnostics=1 im Bereich TimefallShelterRange aktiviert die zusätzlichen räumlichen Entwickler-Logs; Standard ist 0. Die aggregierte Messung protokolliert neu stable (Schnellpfad), stableMaxUs und fullMaxUs für einen aussagekräftigen nächsten Ingame-Test. Die native Zeitregenunterstands-Funktion und alle bestätigten Reichweiten werden nicht verändert.
