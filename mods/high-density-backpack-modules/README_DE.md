# DS2 High-Density Backpack Modules v1.0.0

Mod für eine deutlich höhere Anzahl funktionaler Rucksackkomponenten in **DEATH STRANDING 2: ON THE BEACH**.

Der Mod reduziert den logischen Rasterbedarf funktionaler Rucksackmodule auf einen Ankerplatz und verschiebt blockierte Platzierungen automatisch auf einen nahegelegenen freien, formgültigen Anker. Die sichtbare Originalgrösse bleibt erhalten. Dadurch dürfen sich die Modelle optisch überlappen, während sie als getrennte native Einträge mit ihren normalen Effekten bestehen bleiben.

## Funktionen

- Jedes funktionale Rucksackmodul benötigt logisch nur einen Rasteranker.
- Belegte Anker werden automatisch auf einen nahegelegenen freien Anker umgelegt.
- Die vollständige native Modulform bleibt für die Randprüfung erhalten.
- Grosse Komponenten werden nicht mehr mit sichtbaren Teilen ausserhalb des Rucksacks gespeichert.
- Komponenten-ID, Stufe, Effekt, Objektliste sowie Speichern und Laden bleiben nativ.
- Gemischte Konfigurationen mit Akkus, Granatentaschen, Munitionsbehältern, Vorratstaschen, Solargeneratoren, Stabilisatoren und weiteren funktionalen Modulen werden unterstützt.
- Bei einer unbekannten oder veränderten EXE-Struktur bleibt der Mod inaktiv.

## Praktische Kapazität

DS2 verwendet ein natives `5 x 6`-Raster mit 30 eindeutigen logischen Ankerfeldern. Derselbe exakte Anker wird nicht mehrfach belegt.

Die praktische Grenze hängt von der Zusammenstellung ab:

- Kleine Komponenten können viele Anker verwenden.
- Grosse Akkus und Stabilisatoren benötigen einen Anker, von dem aus ihre vollständige sichtbare Form innerhalb des Rucksacks bleibt.
- Ist kein freier und formgültiger Anker mehr vorhanden, kann DS2 wieder den normalen Ersetzen-Dialog anzeigen.

Der Mod erhöht die Kapazität daher stark, ist aber technisch nicht wörtlich unbegrenzt.

## Validierter Stand

Im Entwicklungstest wurden bestätigt:

- 16 gemischte funktionale Komponenten gleichzeitig;
- Menü schliessen und erneut öffnen mit erhaltener Konfiguration;
- speichern, Spiel vollständig neu starten und laden;
- deutlich grössere Akkukapazität mit mehreren montierten Akkus;
- automatische Umlegung belegter Anker;
- grosse sichtbare Modelle bleiben innerhalb der nativen Rucksackgrenze.

Optische Überschneidungen und Clipping zwischen Modulen sind beabsichtigt.

## Unterstützte Version

- `DS2.exe v1.10.89.0`
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`

## Installation

1. Einen kompatiblen externen 64-Bit-ASI-Loader für Death Stranding 2 installieren.
2. `DS2_HighDensityBackpackModules_v1.0.0.asi` in denselben Spiel-/ASI-Ordner wie die anderen DS2-ASIs kopieren.
3. Spiel starten.
4. Prüfen, ob `DS2_HighDensityBackpackModules.log` erstellt wurde und `status=PATCH_APPLIED` enthält.

Der ASI-Loader ist nicht enthalten.

## Hinweise

- Komponenten werden regulär an einem Einrichtungsterminal über die Rucksackanpassung montiert.
- Bei einem belegten Anker kann der Mod die neue Komponente beim Bestätigen auf einen nahegelegenen gültigen Anker verschieben.
- Die sichtbaren Modelle können stark überlappen. Massgebend ist die Objektliste im Menü.
- Erscheint wegen fehlender gültiger Anker der native Ersetzen-Dialog, `Abbrechen` wählen, ausser eine vorhandene Komponente soll absichtlich ersetzt werden.
- Die Akkukapazität mehrerer Akkus wurde ausdrücklich geprüft. Nicht jede denkbare Kombination aller Spezialeffekte wurde einzeln vermessen.

## Deinstallation

Vor dem Entfernen des Mods:

1. Mit aktivem Mod den Rucksack auf eine Konfiguration reduzieren, die auch mit den ursprünglichen Modulgrössen passt.
2. Änderungen anwenden und neu speichern.
3. `DS2_HighDensityBackpackModules_v1.0.0.asi` entfernen.
4. Die Logdatei kann ebenfalls gelöscht werden.

Eine Deinstallation mit weiterhin gespeicherter High-Density-Konfiguration wird nicht empfohlen.
