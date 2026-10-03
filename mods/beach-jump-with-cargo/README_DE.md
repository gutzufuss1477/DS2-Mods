# Beach Jump with Cargo 1.0.0

Mit **Beach Jump with Cargo** bleibt die von Sam getragene Fracht bei Beach-Jump-Schnellreisen in **DEATH STRANDING 2: ON THE BEACH** erhalten.

[ZIP herunterladen](release/DS2_Beach_Jump_with_Cargo_v1.0.0.zip) · [English](README.md)

## Funktion

Der Mod überspringt die Vanilla-Verarbeitung der aktuell von Sam getragenen Fracht beim Fast Travel. Dadurch bleibt Fracht auf dem Rücken sowie an Sams Körper befestigte Fracht beim Sprung erhalten.

Im Spiel bestätigt wurden:

- Einrichtung → Transponder.
- Mehrere Sprünge zwischen Transponder-/Fast-Travel-Zielen.
- Hot Spring Jump.
- Rückenfracht und direkt an Sam montierte Fracht.

Nicht umfasst sind Fahrzeuge, Floating Carrier, am Boden liegende Fracht oder andere Fracht, die Sam beim Start nicht selbst trägt.

## Voraussetzungen und Installation

- PC-Steam-Version **1.10.89.0**, 64 Bit.
- Ein kompatibler **x64-ASI-Loader**.

DS2 vollständig schliessen und `ds2_beach_jump_with_cargo.asi` sowie `ds2_beach_jump_with_cargo.ini` direkt neben `DS2.exe` kopieren. Danach das Spiel starten.

## Einstellung

```ini
[BeachJumpWithCargo]
Enabled=1
```

`Enabled=0` deaktiviert den Mod. Nach einer Änderung DS2 neu starten.

Die Logdatei `ds2_beach_jump_with_cargo.log` zeigt mit `status=ACTIVE`, dass der Hook aktiv ist. Bei einer nicht unterstützten Spielversion oder veränderten Zielbytes bleibt der Mod inaktiv.

Der Mod verändert nur den laufenden Arbeitsspeicher und keine Spielstände, Archive oder die EXE auf der Festplatte.
