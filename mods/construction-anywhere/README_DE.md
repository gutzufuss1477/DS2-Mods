# DS2 Construction Anywhere v1.0.0

ASI-Mod für **DEATH STRANDING 2 ON THE BEACH**, Steam-PC-Version **1.10.89.0**.

Der Mod ist dafür ausgelegt, zwei voneinander getrennte Baugrenzen aufzuheben:

- Mindestabstände zu bereits vorhandenen Konstruktionen
- `DSUnconstructableArea`-Sperrzonen, etwa No-Build-Bereiche an Einrichtungen

Eine dritte, riskantere Prüfung für echte Whitelist-Baugebiete ist enthalten, aber standardmäßig deaktiviert. Andere Prüfungen bleiben aktiv: Q-Pid-/Chiralnetz, Untergrund, Material, Straße/Highway, Anbringung am Boden und weitere Kollisionsregeln.

## Installation

1. Einen kompatiblen ASI-Loader verwenden. Wenn `winmm.dll` bereits neben `DS2.exe` liegt und andere ASI-Mods lädt, genügt dieser vorhandene Loader.
2. `ds2_construction_anywhere.asi` und `ds2_construction_anywhere.ini` in denselben Ordner wie `DS2.exe` kopieren.
3. Das Spiel vollständig neu starten.

Der Mod kann parallel zu `ds2_weapons_anywhere.asi` verwendet werden. Beide Dateien und INIs bleiben getrennt.

## Konfiguration

```ini
[ConstructionAnywhere]
Enabled=1
RemoveProximityLimit=1
AllowRestrictedAreas=1
IgnorePermittedAreaWhitelist=0
DebugLog=0
```

- `RemoveProximityLimit=1`: überspringt ausschließlich `NearbyFacility`. Dadurch entfallen lokale und Online-Mindestabstände sowie die zugehörigen Bounding-Volume-Abstände für Konstruktionen.
- `AllowRestrictedAreas=1`: überspringt `UnconstructableArea`. Konstruktionen innerhalb des gezeigten Einrichtungs-/Gebäude-Sperrbereichs wurden für Version 1.0.0 im Spiel erfolgreich bestätigt.
- `IgnorePermittedAreaWhitelist=1`: überspringt zusätzlich `PermittedArea`. Diese Option ist experimentell und standardmäßig aus, weil sie auch Missions- oder Spezialkonstruktionen außerhalb ihrer vorgesehenen Baufläche erlauben kann.
- `DebugLog=1`: schreibt `ds2_construction_anywhere.log` neben die ASI-Datei.

Änderungen an der INI werden beim nächsten vollständigen Spielstart übernommen.

## Wichtige Hinweise

- Vor dem ersten Test einen Spielstand sichern und möglichst zunächst offline testen.
- Zu eng oder in Story-/Kulissengeometrie gebaute Strukturen können kollidieren, schlecht erreichbar sein oder online nicht korrekt synchronisieren.
- Spezielle Monorail-/Streckenabstände bleiben absichtlich aktiv. Dort kann weiterhin eine eigene Meldung „zu nah“ erscheinen.
- Der Mod verändert `DS2.exe` nicht auf der Festplatte. Er ändert beim Start höchstens drei einzelne Bytes im Arbeitsspeicher.
- Bei einer unbekannten EXE-Version oder abweichenden Signatur wird nichts gepatcht.
- Nach einem Spielupdate muss eine neue, geprüfte Mod-Version erstellt werden.

Ein genauer Testablauf steht in `docs/TEST_PLAN.md`.

## Deinstallation

`ds2_construction_anywhere.asi`, `ds2_construction_anywhere.ini` und optional `ds2_construction_anywhere.log` aus dem Spielordner entfernen. Bereits gebaute Konstruktionen werden dadurch nicht automatisch gelöscht.
