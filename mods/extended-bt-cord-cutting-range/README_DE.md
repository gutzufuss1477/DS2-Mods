# DS2 Extended BT Cord Cutting Range v1.0.0

Erhöht die Entfernung, aus der Sam mit dem Blood Boomerang die Nabelschnur eines Gazers in **DEATH STRANDING 2: ON THE BEACH** durchtrennen kann.

Der Mod verändert den bestätigten Distanzwert in `DSPlayerCodeCutWithBoomerangState`, ohne die ursprünglichen Ziel-, State-, Stealth- oder Failure-Prüfungen des Spiels zu umgehen.

## Bestätigtes Verhalten

Der interne Vanilla-Wert für die Cord-Cut-Länge ist `4.0`.

Der konfigurierte Multiplikator wird vor dem originalen Helper und Distanzvergleich des Spiels auf diesen Wert angewendet:

- `1.0` = Vanilla
- `2.0` = 2x Vanilla
- `5.0` = 5x Vanilla
- `10.0` = 10x Vanilla

Version 1.0.0 wurde im Spiel mit `Multiplier=5.0` bestätigt. Der Cord-Cut-Prompt wurde deutlich früher verfügbar und das Spiel blieb stabil.

## Installation

Diese zwei Dateien aus dem Release-Archiv in denselben Ordner kopieren, in dem auch die anderen DS2-ASI-Mods liegen:

- `ds2_extended_bt_cord_cutting_range_v100.asi`
- `ds2_extended_bt_cord_cutting_range.ini`

Ältere Entwicklungs-/Testversionen dieses Mods vorher entfernen und das Spiel vollständig neu starten.

## Konfiguration

```ini
[Range]
Enabled=1
Multiplier=5.0
```

| Einstellung | Gültiger Wert | Beschreibung |
|---|---:|---|
| `Enabled` | `0` oder `1` | Aktiviert oder deaktiviert den Reichweiten-Patch. |
| `Multiplier` | `1.0` bis `50.0` | Multipliziert die Vanilla-Cord-Cut-Reichweite. |

Der Standardwert `5.0` ist der im Spiel bestätigte Wert.

## Technischer Umfang

Der Mod hookt den bestätigten Distanzpfad in `DSPlayerCodeCutWithBoomerangState`.

Der Originalcode lädt `mCodeCutWithBoomerangLength`, verarbeitet den Wert mit dem originalen Helper und vergleicht ihn mit der aktuellen Distanz. Der Mod multipliziert nur den Eingabewert vor diesem Helper.

Der originale Distanz-Failure-Branch bleibt unverändert.

Nicht umgangen werden:

- Prüfung auf einen gültigen Gazer
- Stealth-/State-Voraussetzungen
- normale Cord-Cut-Aktionslogik
- originaler Failure-/Controlflow

## Log

Der Mod erzeugt:

`ds2_extended_bt_cord_cutting_range.log`

Bei erfolgreichem Laden steht darin:

`status=patched`

## Build

```powershell
.\scripts\build-llvm.ps1
```

Das Build-Ergebnis liegt danach unter `build/public/`.

## Teststatus

Vor dem Release v1.0.0 bestätigt:

- Save lädt normal.
- Gameplay bleibt bedienbar.
- `Multiplier=5.0` erhöht die nutzbare Cord-Cut-Reichweite deutlich.
- Der originale Distanz-Failure-Branch bleibt aktiv.
