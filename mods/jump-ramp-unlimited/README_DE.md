# Jump Ramp Unlimited 1.0.0

Mit **Jump Ramp Unlimited** kannst du nach dem Absprung von einer Sprungrampe in **DEATH STRANDING 2: ON THE BEACH** weitere Lufttricks ausführen. Sobald die normale Folge ihr Limit erreicht, lässt sich der letzte verfügbare Trick wiederholen.

[ZIP herunterladen](release/DS2_Jump_Ramp_Unlimited_v1.0.0.zip) · [English](README.md)

## Voraussetzungen und Installation

- PC-Steam-Version **1.10.89.0**, 64 Bit.
- Ein kompatibler **x64-ASI-Loader**, zum Beispiel [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader). Der Loader ist nicht im Mod-ZIP enthalten.

DS2 vollständig schließen. Die Dateien `ds2_jump_ramp_unlimited.asi` und `ds2_jump_ramp_unlimited.ini` aus dem ZIP direkt neben `DS2.exe` kopieren. Danach das Spiel starten. Der Mod ist standardmäßig aktiv.

Beim Wechsel von der privaten Testversion vorher `ds2_jump_ramp_probe.asi` und die alte INI entfernen. Beide Versionen dürfen nicht gemeinsam geladen werden.

## Verwendung

Von einer Sprungrampe abspringen und in der Luft weiterhin die normale Sprung-/Trickeingabe verwenden. Für einen ersten Test eine Rampe mit genügend Höhe oder freiem Abhang wählen. Bei der Landung endet die Möglichkeit, die Kette fortzusetzen. Eingabezeitpunkt, Aktionsbedingungen und Trick-Abklingzeit des Spiels gelten weiterhin.

In der INI schaltet `Enabled=1` den Mod ein und `Enabled=0` ihn aus. **Nach Änderungen DS2 neu starten.** Andere Werte werden abgelehnt; ohne INI oder ohne Einstellung ist der Mod standardmäßig aktiv.

## Deinstallation und Fehlersuche

DS2 schließen und die beiden Mod-Dateien entfernen. Die erzeugte `ds2_jump_ramp_unlimited.log` kann ebenfalls gelöscht werden. Einen gemeinsam genutzten ASI-Loader für andere Mods behalten.

Der Mod verändert den Arbeitsspeicher, keine Spieldateien oder Spielstände auf der Festplatte. In der Logdatei bedeutet `ACTIVE`, dass der Hook installiert wurde. Bei einer nicht unterstützten Spielversion oder abweichendem Code bleibt der Mod inaktiv. Fehlt die Logdatei, zuerst ASI-Loader und Installationsordner prüfen.

## Prüfstand

Die Wiederholungslogik wurde im privaten Build v0.6-test5 im Spiel bestätigt: viele aufeinanderfolgende Tricks über eine große Distanz. Für 1.0 wurden Diagnose-Telemetrie und laufendes INI-Nachladen entfernt sowie Versionsangaben, endgültige Dateinamen und zusätzliche Startprüfungen ergänzt.

1.0 besteht 24.576 native Zustandsfälle und 2.000 Wiederholungen. Die automatisierte Prüfung ersetzt die Engine-Hilfsfunktionen durch Testfunktionen. Die endgültige 1.0-Datei und der Cap-3-Pfad wurden noch nicht separat im laufenden Spiel bestätigt. Details stehen in [VALIDATION.md](docs/VALIDATION.md).
