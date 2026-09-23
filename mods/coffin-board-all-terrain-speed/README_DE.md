# Coffin Board Reworked v1.83.0

Buildgebundener ASI-Mod für DEATH STRANDING 2: ON THE BEACH, Steam-PC 1.10.89.0.

Der Mod erweitert das Coffin Board um konfigurierbare Geschwindigkeit und
Beschleunigung auf Land und Wasser, Unterstützung für einen beladenen
Lastenschweber, Grenzübertritt des chiralen Netzes sowie Auf- und Wiedereinstieg
außerhalb des Netzes. Der Ablauf mit angekoppeltem Lastenschweber ist im Spiel
bestätigt.

## Funktionen

- Standardwerte von 500 % Geschwindigkeit und 400 % Beschleunigung.
- Identische erhöhte Geschwindigkeitslimits auf Land und Wasser.
- Fahren mit angekoppeltem, beladenem Lastenschweber.
- Erhalt der Schweberverbindung bei schneller Fahrt, Wasserüberquerungen,
  Kollisionen und großen Sprüngen.
- Mit dem Coffin Board die Grenze des chiralen Netzes überqueren.
- Außerhalb absteigen und wieder auf das Coffin Board steigen.
- Pickup und Tri-Cruiser behalten ihren nativen Aufstiegspfad.

## Installation

Das Nexus-Archiv enthält genau zwei Dateien im Stammordner:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Spiel schließen, Archiv entpacken und beide Dateien neben `DS2.exe` ablegen.
Ein kompatibler externer 64-Bit-ASI-Loader wird benötigt. Bei einem Update beide
Dateien ersetzen. Nach Änderungen an der INI DS2 neu starten.

## Konfiguration

```ini
[CoffinBoardAllTerrainSpeed]
Enabled=1
SpeedPercent=500
AccelerationPercent=400
AllowFloatingCarrier=1
EnableNetworkTraversal=1
EnableOutsideNetworkMount=1
```

`SpeedPercent` akzeptiert 100-1000, `AccelerationPercent` 100-500. Die drei
Schalter für Lastenschweber, Grenzübertritt und Aufsteigen außerhalb aktivieren
die entsprechenden Funktionen.

## Prüfung und Kompatibilität

Die ASI prüft vor dem Patchen die exakte unterstützte Steam-Exe, Ressourcen,
VTable-Slots und Instruktionsanker. Version 1.83.0 wurde im Spiel für
Grenzübertritt, Absteigen und Wiedereinstieg außerhalb sowie denselben Ablauf
mit Lastenschweber getestet.

Lenkung, Nasshaftung, manuelles Ab- und Wiederankoppeln des Schwebers,
Fracht- und Kollisionsschaden bleiben nativ. Der Mod verändert weder DS2.exe,
Spieldateien noch Savegames auf dem Datenträger. Nach einem Spielupdate die ASI
entfernen, bis eine passende Version geprüft ist.
