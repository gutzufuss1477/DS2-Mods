# Infrastructure One Unit

Reduziert die **noch benötigte** Materialmenge unterstützter Infrastruktur in Death Stranding 2 auf einen frei konfigurierbaren Wert pro noch offener Materialkategorie.

Standard:

```ini
RemainingUnits=1
```

Benötigt eine Strasse beispielsweise noch chirale Kristalle, Metalle und Keramik, bleibt pro Kategorie genau eine Einheit offen. Der normale Frachtablauf des Spiels bleibt erhalten: Es wird nur die benötigte Menge verbraucht; der Rest bleibt im Container oder wird vom Spiel regulär zurückgelegt.

## Unterstützte Infrastruktur

- Strassen-Erstbau, Stufe 1
- Strassen-Ausbau, Stufe 2
- Monorail-Erstbau, Stufe 1
- Minen-Erstbau, Stufe 1
- Minen-Ausbau, Stufen 2 und 3

Reparaturen, Fabrikation und normale Frachtverwaltung werden nicht verändert.

## Unterstützter Spielbuild

- Steam-PC-Version
- Death Stranding 2 v1.10.89.0

Die ASI prüft Build-Metadaten und die benötigten Instruktionssignaturen. Bei einem unbekannten Build bleibt sie inaktiv, statt unsicher zu patchen.

## Installation

1. Einen kompatiblen x64-ASI-Loader für das Spiel installieren.
2. Diese beiden Dateien neben den ASI-Loader beziehungsweise `DS2.exe` kopieren:

```text
ds2_infrastructure_one_unit.asi
ds2_infrastructure_one_unit.ini
```

3. Das Spiel ohne Cheat Engine starten.

## Konfiguration

```ini
[InfrastructureOneUnit]
Enabled=1
RemainingUnits=1

RoadConstruction=1
RoadUpgrades=1
MonorailConstruction=1
MineConstruction=1
MineUpgrades=1

DebugLog=0
```

`RemainingUnits` gilt **pro noch offener Materialkategorie**.

| Normaler Restbedarf | `RemainingUnits` | Effektiver Restbedarf |
|---:|---:|---:|
| 3000 | 1 | 1 |
| 3000 | 50 | 50 |
| 40 | 50 | 40 |
| 0 | 50 | 0 |

Der Mod erhöht keine Kategorie, deren Restbedarf bereits kleiner als die Konfiguration ist. Fertige und nicht benötigte Materialslots bleiben unverändert.

Nach einer Änderung der INI muss DS2 vollständig neu gestartet werden.

## Bereits vorhandene Online-Beiträge

Bereits vorhandene eigene oder Online-/Fremdbeiträge bleiben berücksichtigt. Der Mod ergänzt nur das Delta, das nötig ist, um den konfigurierten Restwert zu erreichen. Der Fortschritt wird nicht auf null zurückgesetzt.

## Anzeige

Bei geladenen unterstützten Objekten kann die reduzierte Menge bereits in der Spielwelt beziehungsweise Übersicht erscheinen, bevor das Detailmenü geöffnet wird. Der genaue Aktualisierungszeitpunkt wird weiterhin vom Spiel gesteuert.

## Hinweis zu Spielständen

Der Mod bearbeitet Save-Dateien nicht direkt. Er verändert den aktiven Infrastrukturfortschritt, den das Spiel regulär speichern kann. Vor der ersten Verwendung sollte ein Spielstand gesichert werden. Nach dem Entfernen der ASI werden bereits reduzierte und gespeicherte Anforderungen nicht zwingend auf die Originalwerte zurückgesetzt.

## Log

Mit:

```ini
DebugLog=1
```

entsteht neben der ASI:

```text
ds2_infrastructure_one_unit.log
```

Eine erfolgreiche Initialisierung enthält:

```text
ACTIVE: exact Need/Current caller-pair hooks armed. Known caller pairs: 16.
```

Jede angewendete Infrastruktur wird als `APPLY` protokolliert.
