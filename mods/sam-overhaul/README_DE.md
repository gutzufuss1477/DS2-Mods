# Sam Overhaul v1.1.0 – Anleitung

Ein konfigurierbarer ASI-Mod für **DEATH STRANDING 2: ON THE BEACH** (Windows x64, Steam DS2.exe 1.10.89.0).

Sam Overhaul kombiniert die visuelle Fracht-Ausblendung, Bewegungsverbesserungen, schnellere Autodrive-Aktivierung und die Anpassungen der Truck-Waffen. **Version 1.1.0 ergänzt drei optionale Funktionen:** Fussabdrücke ausblenden, Generator-Reichweite erweitern und Zeitregenunterstand vergrössern.

## Neu in v1.1.0

### 1. Fussabdrücke ausblenden

- Entfernt sowohl normale Fussabdrücke im Boden als auch die blau markierten Odradek-Scan-Spuren aus der Darstellung.
- Funktioniert auch mit Spuren aus bereits gespeicherten Spielständen nach dem Laden.
- Verändert weder die gespeicherten Daten noch die Spielstände.
- Benötigt weder ReShade noch einen zusätzlichen Footprint-Mod.

~~~ini
[Footprints]
HideFootprints=1
~~~

### 2. Generator-Reichweite erweitern

- Erhöht den geprüften Ladebereich des Generators und passt die sichtbare Odradek-Kreismarkierung an.
- RangePercent=200 entspricht dem doppelten ursprünglichen Radius.
- Die vergrösserte Jolt-Kollision und die visuelle Anzeige werden für die geprüften Generatoren synchronisiert.
- Weitere Generatorstufen oder andere Spielversionen können sich anders verhalten.

~~~ini
[GeneratorRange]
Enabled=1
RangePercent=200
~~~

### 3. Zeitregenunterstand vergrössern

Die Reichweiten können separat abgestimmt werden:

| Funktion | Empfohlener Radius |
| --- | ---: |
| Schutz vor Zeitregen | 8,0 m (200 %) |
| Blauer Kreis am Boden | 8,0 m (200 %) |
| Frachtcontainer-Reparatur | 8,6 m (215 %) |

Der um 0,6 m grössere Reparaturradius berücksichtigt Höhenunterschiede im Gelände. Das Spiel verwendet für die Reparatur einen dreidimensionalen Abstand; deshalb kann die Reparaturwolke an einem Hang geringfügig innerhalb oder ausserhalb des Kreises beginnen.

Zusätzlich wird die missverständliche deutsche Interaktionsbeschriftung **«In Bunker ausruhen»** durch **«Verschnaufen»** ersetzt. Die eigentliche native Ausruhfunktion des Zeitregenunterstands bleibt dabei erhalten.

~~~ini
[TimefallShelterRange]
Enabled=1
RangePercent=200
RepairRadiusPercent=215
FixRestPrompt=1
SpatialDiagnostics=0
~~~

Die bewährten Werte für Regenabwehr, Reparaturmenge und die Ausruhfunktion werden nicht verändert. Die zusätzliche Performance-Optimierung vermeidet wiederholte vollständige Objektprüfungen, wenn der Unterstand bereits korrekt synchronisiert wurde. Das Spiel wurde mit diesen Einstellungen erfolgreich getestet.

## Bereits vorhandene Sam-Overhaul-Funktionen

**Fracht ausblenden:** Schultern, Hüften, Rucksackfracht und Ersatzschuhe auf dem Schuhclip. Getragene Schuhe sowie Gewicht, Besitz und Zustand der Fracht bleiben unverändert.

**Bewegung:** Abspringen von Monorails ausserhalb der ursprünglichen letzten Höhenblockade, zusätzliche Zipline-Absprungpositionen und reguläre Landerollen mit Rucksack beziehungsweise Fracht.

**Autodrive:** Konfigurierbare Aktivierungszeit von 0,5–5,0 Sekunden (Empfehlung: 2,0 Sekunden). Die nativen Voraussetzungen der Strasse und Fahrzeuge gelten weiterhin.

**Truck-Waffen:** Anpassungen für schweres MG, Mörser, Chiral Cannon und Raketenwerfer. Reichweite, Zielgeschwindigkeit und Schussabstände sind getrennt einstellbar.

## Installation und Update

1. **Spiel beenden.** Ein funktionierender ASI-Loader für Windows x64 wird benötigt; er ist nicht im Paket enthalten.
2. Alle alten DS2_Sam_Overhaul_v*.asi aus dem Spielverzeichnis entfernen. Es darf nur **eine** Version aktiv sein. Eine alte ds2_footprint_native_probe.asi darf nicht zusätzlich geladen werden.
3. Datei **DS2_Sam_Overhaul_v1.1.0.asi** ins Verzeichnis neben DS2.exe kopieren. Bei einer Neuinstallation zusätzlich die enthaltene **ds2_sam_overhaul.ini** kopieren.
4. Beim Update von v1.0.0 die eigene INI mit den bestehenden Waffen- und Bewegungs-Einstellungen **behalten**. Die drei neuen Bereiche aus dieser Anleitung ergänzen, statt die persönliche INI ungeprüft zu überschreiben.
5. Spiel starten. Änderungen an der INI benötigen einen Neustart des Spiels.

Üblicher Steam-Pfad:

    ...\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\

Zum Deinstallieren die Sam-Overhaul-ASI entfernen. DS2.exe und Spielstände werden auf der Festplatte nicht verändert.

## Standardwerte und optionale Funktionen

**Die drei neuen Funktionen sind im heruntergeladenen Standard-INI zunächst deaktiviert**, damit bestehende Installationen nicht unerwartet verändert werden:

- [Footprints] HideFootprints=0
- [GeneratorRange] Enabled=0
- [TimefallShelterRange] Enabled=0

Wer die Funktionen nutzen möchte, stellt die Werte gemäss den Beispielen oben auf 1. Fehlende neue Einträge in einer alten INI bleiben ebenfalls deaktiviert.

RangePercent=100 bedeutet den ursprünglichen Radius, 200 den doppelten Radius. Für Generator und Unterstand sind 100–400 zulässig. Der separate RepairRadiusPercent-Wert akzeptiert ebenfalls 100–400; für den 8-m-Unterstand sind **215** empfohlen.

FixRestPrompt=0 schaltet die Textkorrektur aus. SpatialDiagnostics=1 aktiviert zusätzliche Entwicklermessungen und wird für normales Spielen nicht empfohlen.

## Kompatibilität

Geprüft für **Steam DS2.exe 1.10.89.0 (Windows x64)**. SHA-256 der unterstützten DS2.exe:

    BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

Andere Spielversionen und die Epic-Games-Store-Version sind **nicht gesondert geprüft**. Änderungen an der Spiel-EXE können ein Update der ASI erforderlich machen. Andere Mods, die dieselben nativen Funktionen verändern, können Konflikte verursachen.

Das Protokoll **ds2_sam_overhaul.log** wird im Spielverzeichnis erstellt. MinHook ist in der ASI integriert; siehe THIRD_PARTY_NOTICES.md und LICENSE_MINHOOK.txt. Kein ReShade und kein separater Übersetzungs-Mod erforderlich.

## Teststatus

Der Release basiert auf der im Spiel bestätigten dev.28 mit Fussabdruck-Ausblendung, vergrössertem Generator, 8-m-Unterstand, 8,6-m-Frachtreparatur und stabiler Interaktion «Verschnaufen». Auch die abschliessende Performance-Optimierung wurde im Spiel überprüft; es wurden keine störenden Ruckler mehr festgestellt. Native Sprungadressen, Reparaturquellen, Objekt-Neuladen, Rendering und INI-Kompatibilität wurden mit automatisierten Tests geprüft.
