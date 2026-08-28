# DS2 Weapons Anywhere v1.0.0

ASI-Mod für **DEATH STRANDING 2: ON THE BEACH**. Er hebt die technische Waffenblockade in den Waffenverbotszonen bei Einrichtungen und Gebäuden auf. Der separate Darstellungs- und Ansagepfad der Zone bleibt unverändert.

## Beabsichtigtes Verhalten

- Waffen und waffenähnliche Ausrüstung sollen auch innerhalb der bisherigen Sperrzone benutzbar bleiben.
- Warnring, Sprachausgabe sowie die übrige optische und akustische Rückmeldung bleiben unverändert.
- Spielstände, Bauwerke und originale Spielarchive werden nicht verändert.

Für Version 1.0.0 wurden Schusswaffen und Granaten innerhalb einer Gebäude-/Einrichtungszone im Spiel erfolgreich bestätigt. Nahkampf, Spezialausrüstung und Fahrzeugwaffen sind nicht einzeln als vollständige Matrix verifiziert und bleiben im mitgelieferten Regressionstest aufgeführt.

## Unterstützter Ziel-Build

- Plattform: Steam-PC
- Spielversion: `1.10.89.0`
- PE-Zeitstempel: `0x6A3DAE46`
- Image-Grösse: `0x0B292000`
- SHA-256 von DS2.exe: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`

Die ASI prüft Build-Metadaten und eine eindeutige, 22 Byte lange Instruktionssignatur. Bei einer Abweichung bleibt der Patch für diese Spielsitzung aus.

## Installation

1. Einen funktionierenden externen x64-ASI-Loader für DS2 verwenden.
2. `ds2_weapons_anywhere.asi` und `ds2_weapons_anywhere.ini` neben `DS2.exe` beziehungsweise in den vom Loader verwendeten Ordner kopieren.
3. Das Spiel neu starten.

Zum Deinstallieren beide Dateien entfernen und das Spiel neu starten.

## Konfiguration

```ini
[WeaponsAnywhere]
Enabled=1
DebugLog=0
```

Mit `Enabled=0` bleibt der Spielcode unangetastet. `DebugLog=1` erzeugt beim Start die Datei `ds2_weapons_anywhere.log` neben der ASI. Auch bei deaktiviertem Debug-Log versucht die ASI im Ausnahmefall eines unvollständigen Speicherschutz-Rollbacks automatisch, diese Datei mit einer `CRITICAL`-Meldung anzulegen.

## Regressionstest

1. Einen separaten Testspielstand sichern und DS2 zunächst offline starten.
2. Mit `Enabled=0` an einer Einrichtung Warnring, Ansage und die originale Waffenblockade bestätigen.
3. DS2 beenden, `Enabled=1` und `DebugLog=1` setzen und neu starten.
4. Im Log die Zeile `ACTIVE` bestätigen.
5. An derselben Stelle prüfen, dass Warnring und Ansage weiter erscheinen, die gleiche Waffe sich aber ziehen, zielen und abfeuern lässt.
6. Bei einem Absturz, fehlender Warnanzeige oder ungewöhnlichem Einrichtungs-/Speicherverhalten abbrechen und die ASI entfernen.

## Technische Grenze

Der Patch ändert bei `DS2.exe+0xFA4255` ausschliesslich den Sprungbefehl von `je` zu `jmp`. Die Originalfunktion prüft weiterhin die Zone und löst `DSWeaponParameter::IsApplyWeaponsProhibitedArea` auf, überspringt aber gezielt den direkt folgenden Sperrgrund `return 6`. Spätere Sperrgründe bleiben unverändert. Der separate Präsentationspfad für Warnring, Ansage sowie On/Off-Sounds wird nicht gepatcht.

Der mitgelieferte Testplan unter `docs/TEST_PLAN.md` dient für spätere Spielupdates und zusätzliche Ausrüstungskategorien als Regressionstest.
