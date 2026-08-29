# DS2 Chiral Bandwidth Costs v1.0.0

Anpassbare Kosten für chirale Bandbreite bei unterstützten Spielerkonstruktionen in **DEATH STRANDING 2: ON THE BEACH** auf dem PC.

Version 1.0.0 unterstützt genau eine ausführbare Spielversion. In der mitgelieferten INI stehen alle Konstruktionen standardmäßig auf `Native`. Allein durch die Installation werden daher noch keine Kosten verändert. Für jede gewünschte Konstruktion kann stattdessen eine Dezimalzahl eingetragen werden.

## Installation

Das Release-Archiv enthält genau zwei Laufzeitdateien:

- `ds2_chiral_bandwidth_costs.asi`
- `ds2_chiral_bandwidth_costs.ini`

Einen kompatiblen externen 64-Bit-ASI-Loader installieren und anschließend beide Dateien in den Spielordner neben `DS2.exe` kopieren. Nach der Installation und nach jeder INI-Änderung das Spiel vollständig neu starten.

Zur Deinstallation beide Dateien entfernen und das Spiel neu starten. Ein ASI-Loader ist nicht enthalten.

## Konfiguration

```ini
[ChiralBandwidthCosts]
Enabled=1
DebugLog=0

Postbox=Native
SafeHouse=Native
Watchtower=Native
CargoCatapult=Native
Generator=Native
RainShelter=Native
Zipline=Native
Bridge=Native
ChiralBridge=Native
JumpRamp=Native
HotSpringDigger=Native
```

`Native` lässt die jeweilige Konstruktion unverändert. Eine ganze Dezimalzahl von `0` bis `1000000` legt die gewünschten Kosten fest; `0` bedeutet keine Kosten für chirale Bandbreite. Vorzeichen, Dezimaltrennzeichen, Hexadezimalwerte und Kommentare hinter einem Wert sind nicht zulässig. Bei einer ungültigen aktivierten Konfiguration werden keine Kosten geschrieben.

Beispiel:

```ini
Generator=100
Zipline=250
JumpRamp=300
```

Unterstützte Schlüssel und bekannte native Referenzkosten des Ziel-Builds:

| INI-Schlüssel | Native Referenzkosten |
| --- | ---: |
| `Postbox` | 200 |
| `SafeHouse` | 2500 |
| `Watchtower` | 250 |
| `CargoCatapult` | 450 |
| `Generator` | 350 |
| `RainShelter` | 500 |
| `Zipline` | 500 |
| `Bridge` | 1000 |
| `ChiralBridge` | 250 |
| `JumpRamp` | 550 |
| `HotSpringDigger` | 0 |

Der einzelne Schlüssel `Bridge` gilt für die normalen Brückenkonfigurationen mit 30 m, 45 m und 80 m. Alle drei Varianten und beide Manager-Pfade der 30-m-Brücke sind statisch und synthetisch geprüft, besitzen aber noch nicht dieselbe praktische Spielabdeckung.

`HotSpringDigger` ist experimentell, weil seine nativen Kosten null betragen, obwohl DS2 dafür Q-pid-Abdeckung verlangt. Der Transponder wird bewusst nicht unterstützt: Seine Ressource ist ein Werkzeug mit `NeedWithinQpid=false`. Eine Änderung seines vorhandenen `QpidCost`-Feldes würde deshalb keine verlässliche Abbuchung chiraler Bandbreite bewirken.

Diese Werte sind Referenzen für den Ziel-Build. `DebugLog=1` erzeugt `ds2_chiral_bandwidth_costs.log` neben der ASI. Fehler werden auch bei deaktiviertem optionalem Debug-Log protokolliert.

## Validierung

Am 29.08.2026 wurde der exakte benutzerdefinierte Wert `5` im laufenden Konstruktions-HUD sowohl für Generator als auch für Seilrutsche auf dem unterstützten Steam-Build bestätigt. Zusätzlich besteht das Release die Tests des mitgelieferten INI-Parsers, die synthetischen Laufzeit- und Transaktionstests, die Zielprogramm-Prüfung sowie die Binär-Härtungsprüfungen aus [`docs/VALIDATION.md`](docs/VALIDATION.md).

## Warnung zum Spielstand

Vor dem Testen den Spielstand sichern und zunächst offline spielen. Sobald mit dem Mod eine Konstruktion gebaut wurde, sollten die gewählten Kosten für diesen Spielstand nicht mehr geändert werden.

DS2 könnte beim Abbau einer Konstruktion den aktuell geladenen Config-Wert verwenden, anstatt die ursprünglichen Baukosten pro Konstruktion zu speichern. Eine nachträgliche INI-Änderung bei bereits vorhandenen Konstruktionen könnte deshalb die verwendete chirale Bandbreite inkonsistent machen. Bau, Speichern, Neuladen und Abbau müssen für jeden unterstützten Typ noch im Spiel validiert werden.

## Kompatibilität und Sicherheit

- Unterstützt wird ausschließlich Steam-PC `DS2.exe 1.10.89.0`.
- Erwarteter PE-Zeitstempel: `0x6A3DAE46`.
- Erwartete Image-Größe: `0x0B292000`.
- Vor Schreibzugriffen werden der Spiel-Build und die relevanten Laufzeit-Ressourcentypen geprüft.
- Unerwartete Ressourcenwerte gelten als möglicher Mod-Konflikt und werden nicht überschrieben.
- `DS2.exe`, Spielarchive und Speicherdateien werden nicht auf der Festplatte verändert.
- Materialbedarf, Platzierungsregeln, Upgrades, Reparaturen, Netzwerkabdeckung und die insgesamt verfügbare chirale Bandbreite sollen unverändert bleiben.
- Nach einem Spielupdate die ASI entfernen, bis eine kompatible Version verfügbar ist.

Generator und Seilrutsche sind im Spiel mit dem benutzerdefinierten HUD-Wert `5` bestätigt. Exakte Abbuchung und Erstattung, Speichern/Neuladen, Kartenwechsel, Online-Konstruktionen und der experimentelle Thermalquellen-Bohrer wurden noch nicht für jeden Konstruktionstyp umfassend praktisch geprüft.
