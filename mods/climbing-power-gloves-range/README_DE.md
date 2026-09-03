# DS2 Climbing Power Gloves Range v1.0.0

Climbing Power Gloves Range erhoeht gezielt die magnetische Cargo-Aufnahmereichweite beider Stufen der **Climbing Power Gloves** in **DEATH STRANDING 2: ON THE BEACH** auf dem PC.

- Level 1: nativ `8 m`, konfigurierbarer Standard `30 m`
- Level 2: nativ `10 m`, konfigurierbarer Standard `50 m`

Beide Stufen besitzen getrennte Einstellungen. Dadurch kann Level 2 seinen deutlichen Reichweitenvorteil behalten. Der Mod reduziert bei der normalen Fernaufnahme von Cargo die Abhaengigkeit von Sticky Gun oder Sticky Cannon; deren Ziel-, Seil- und sonstige Sonderfunktionen werden nicht nachgebildet.

Version 1.0.0 wurde im Spiel bestaetigt: Mit Level 2 laesst sich Cargo aus grosser Entfernung aufnehmen, nach dem Wechsel auf Level 1 wird die Reichweite direkt kleiner, und ohne Handschuhe gilt weiterhin nur die normale Nahreichweite. Save/Load und Gebietswechsel bleiben sinnvolle Regressionstests.

## Voraussetzungen

- Steam-PC-Version `DS2.exe 1.10.89.0`
- EXE-SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- Kompatibler externer 64-Bit-ASI-Loader

Andere Spielversionen werden absichtlich abgelehnt. Nach einem Spielupdate sollte der Mod erst wieder aktiviert werden, wenn eine ausdruecklich kompatible Version veroeffentlicht wurde.

## Installation

Diese beiden Dateien aus dem Release-Archiv in den Spielordner neben `DS2.exe` kopieren:

- `ds2_climbing_gloves_range.asi`
- `ds2_climbing_gloves_range.ini`

Nach Installation oder INI-Aenderungen das Spiel vollstaendig neu starten.

Zur Deinstallation beide Dateien entfernen und das Spiel neu starten. Der Mod veraendert weder Spielstaende noch `DS2.exe` oder Spielarchive auf der Festplatte.

## Konfiguration

Die mitgelieferten Standardwerte sind:

```ini
[ClimbingGlovesRange]
Enabled=1
Level1RangeMeters=30
Level2RangeMeters=50
DebugLog=0
```

| Einstellung | Gueltiger Wert | Beschreibung |
|---|---:|---|
| `Enabled` | `0` oder `1` | Aktiviert oder deaktiviert den Reichweiten-Patch. |
| `Level1RangeMeters` | `8` bis `100` | Aufnahmereichweite fuer Level 1. |
| `Level2RangeMeters` | `10` bis `100` | Aufnahmereichweite fuer Level 2; darf nicht kleiner als Level 1 sein. |
| `DebugLog` | `0` oder `1` | Schreibt zusaetzliche Diagnosen in `ds2_climbing_gloves_range.log`. |

Gleiche Werte geben beiden Stufen dieselbe Reichweite. Bei ungueltigen Werten bricht der Mod sicher ab und protokolliert den Grund. Kritische Fehler werden auch mit `DebugLog=0` protokolliert.

## Technischer Umfang

Der Mod loest die exakten `DSItemParameter`-Ressourcen beider Handschuhstufen auf und aendert nur deren Reichweitenfeld:

| Gegenstand | Item-ID | Ressourcen-Level | Natives Reichweitenfeld |
|---|---:|---:|---:|
| Climbing Power Glove Lv1 | 53 | 0 | `Params[6] = 8.0` |
| Climbing Power Glove Lv2 | 54 | 1 | `Params[6] = 10.0` |

Die globale Hand-Aufnahmereichweite, andere Handschuhe, Aufnahmegeschwindigkeit, Wurfwerte und `Params[7]` bleiben unveraendert. Native Sicht-, Ziel-, Cargo-, Hindernis- und Zustandspruefungen gelten weiterhin. Sticky Gun und Sticky Cannon werden nicht veraendert.

Vor jedem Schreibzugriff prueft der Mod den EXE-Build, die Initialisierung und den Lookup von `DSItemSystem`, VTables, Typmetadaten und beide Ressourcenidentitaeten. Unbekannte oder bereits von einem anderen Mod veraenderte Werte gelten als Konflikt und werden nicht ueberschrieben.

## Ziel-EXE offline pruefen

Aus diesem Mod-Ordner:

```powershell
python .\tools\validate_target.py "C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe"
```

Der Validator prueft Hash, PE-Metadaten, relevante Signaturen, VTables und RVAs. Nur ein vollstaendiges `OK` kennzeichnet das unterstuetzte Ziel.

## Teststatus

Im Spiel fuer v1.0.0 bestaetigt:

- Level 2 besitzt die groesste Aufnahmereichweite.
- Beim Wechsel auf Level 1 wird die Reichweite direkt kleiner.
- Ohne Handschuhe gilt wieder nur die normale Nahaufnahme.

Empfohlene Regressionstests umfassen weiterhin Save/Load, Gebietswechsel, Reisen oder Schlafen, Fahrzeugaufnahme, mehrere Cargo-Teile, Hoehenunterschiede und Hindernisse. Die vollstaendige Checkliste steht in [TEST_PLAN.md](TEST_PLAN.md).
