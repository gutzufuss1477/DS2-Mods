# Improved Odradek Scan – 1.0.0

360°-Odradek-Scan mit **500 m Standardreichweite**, im Spiel bestätigt.
Entfernte unberührte und brauchbare abgelegte Fracht wird beim Scan markiert.
Die Meterzahl steht hinter Empfänger und Likes und aktualisiert sich beim Gehen.
Verbrauchte/zerstörte Fracht behält ihre normale Anzeige in der Nähe.

## Installation

Spiel schließen. Ein funktionierender x64-ASI-Loader wird benötigt und ist nicht
enthalten; einen bereits funktionierenden Loader weiterverwenden.
`ds2_odradek_scan.asi` und `ds2_odradek_scan.ini` neben `DS2.exe` kopieren.
Den Ordner über Steam > DS2 > Verwalten > Lokale Dateien durchsuchen öffnen.
Danach normal starten und scannen.

## Einstellungen

`RangeMeters=500` in der INI ändern und das Spiel neu starten. Der Parser akzeptiert
50–1000; empfohlen sind die bestätigten 500 m. Höhere Werte garantieren keinen
entsprechenden Fundradius (frühere 1000-m-Tests fanden neue Fracht erst um 700 m).
Unter 200 m kann die native Welle größer als die eingestellte Markerreichweite bleiben.
`VisualWaveScale=1` erhält die getestete Welle. `Enabled=0` deaktiviert den Mod.
`DebugLog=1` ist nur zur Diagnose vorgesehen, standardmäßig aus.

Nur Steam PC 1.10.89.0, Windows x64. Gleiche Scan-/HUD-Patches anderer Mods können
kollidieren; ältere Testversionen nicht parallel laden. Die native Anzeigedauer
und Markerbudgets gelten weiterhin. Es werden keine Spielstände bearbeitet.
Zum Entfernen bei geschlossenem Spiel die ASI und INI löschen; einen von anderen
Mods verwendeten ASI-Loader behalten. Bei Updates eigene INI-Einstellungen sichern.

[English / Buildanleitung](README.md) · [Änderungen](CHANGELOG.md)
