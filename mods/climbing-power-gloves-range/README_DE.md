# DS2 Climbing Power Gloves Range v1.1.0

Der Mod erweitert die magnetische Cargo-Fernaufnahme in DEATH STRANDING 2: ON THE BEACH auf dem PC.

Bestehende Climbing-Power-Gloves-Funktion:

- Level 1: nativ 8 m, Standard 30 m
- Level 2: nativ 10 m, Standard 50 m

Version 1.1.0 fuegt zusaetzlich eine optionale magnetische Fernaufnahme fuer die Combat Power Gloves hinzu.

## Konfiguration

ClimbingGlovesRange:
- Enabled=1
- Level1RangeMeters=30
- Level2RangeMeters=50
- DebugLog=0

CombatGloves:
- EnableCargoPickup=1

Die Combat Power Gloves besitzen in dieser Version keine separaten Reichweitenwerte. Ihre sieben nativen Combat-Parameter bleiben unveraendert.

## Teststatus v1.1.0

Bestaetigt:
- Combat Power Gloves koennen lose Fracht aus Distanz magnetisch aufnehmen.
- Die Fernaufnahme funktioniert ohne Absturz.
- Normales Zuschlagen mit den Combat Power Gloves funktioniert weiterhin.
- Climbing Power Gloves behalten ihre bestehende Fernaufnahme.

Unterstuetzte Version: Steam PC DS2.exe 1.10.89.0.
