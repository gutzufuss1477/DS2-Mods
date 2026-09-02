# DS2 Coffin Board: All-Terrain Speed v1.0.0

Buildgebundener ASI-Mod für **DEATH STRANDING 2: ON THE BEACH**, Steam-PC
`1.10.89.0`.

Der Mod gibt dem Coffin Board auf Land und Wasser dieselben erhöhten
Geschwindigkeitslimits und verwendet die getestete schnellere Beschleunigung.
Die finale Fassung verändert bewusst weder Lenkung noch Nasshaftung, generische
Fahrzeugphysik, Archive oder Spielstände. Die Testtelemetrie ist deaktiviert.

Sobald der Patch der Coffin-Ressource abgeschlossen ist, entfernt der Worker
den nativen Streaming-Listener sofort und beendet sich. Wird das Ziel nicht
gefunden, wird der Listener nach einem begrenzten Suchfenster von 60 Sekunden
entfernt. Damit nimmt der Mod am späteren Streaming-Shutdown von DS2 nicht mehr
teil.

## Installation

Das Release-Archiv enthält genau zwei Dateien:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Beide Dateien neben `DS2.exe` kopieren. Ein externer 64-Bit-ASI-Loader wird
vorausgesetzt. Von einer früheren Testversion unbedingt beide Dateien ersetzen,
das Spiel vollständig neu starten und ein neues Coffin Board ausbringen, da
eine bereits erzeugte Physikkomponente ihre alte Getriebetabelle behalten kann.

## Konfiguration

Die vollständige Release-INI lautet:

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400
```

`SpeedPercent=500` ergibt auf Land und Wasser normale/Boost-Limits von
`300/400 km/h` und ändert die Endübersetzung von `15` auf `3`. Gültiger
Bereich: `100-1000`.

`AccelerationPercent=400` setzt die Antriebskraft auf `20x`. Zusammen mit der
Übersetzung von `0,2x` ergibt das nominell das getestete vierfache effektive
Antriebsmoment. Gültiger Bereich: `100-500`.

Alle fehlenden optionalen Schlüssel fallen sicher auf native beziehungsweise
inaktive Werte zurück: Lenkung `100 %`, Nasshaftung `100 %`, Telemetrie `0`.
Der finale Antriebs-Trampolin ohne Telemetrie enthält keinen Messwertzugriff;
ein Lenk-Hook wird nicht installiert.

## Sicherheit und Prüfung

Der ASI akzeptiert ausschließlich die geprüfte Steam-Version. PE-Metadaten,
StreamingManager-Anker, Identität und Layout der Coffin-Ressource,
Antriebs-Hook-Signatur und Coffin-Physics-vtable müssen exakt passen. Der
Antriebs-Hook filtert jeden Aufruf über genau diese Coffin-vtable; andere
Fahrzeuge bleiben unverändert.

Die Ressourcentransaktion ändert nur die vier Geschwindigkeitslimits, die
Endübersetzung und den für den erhöhten Geschwindigkeitsbereich nötigen
Slip-Grenzwert. Der Antriebsfaktor bleibt neutral, bis alle Writes erfolgreich
sind. Fehler rollen bereits ausgeführte Writes zurück. Beim Entladen der
exakten Ressource wird der Faktor innerhalb des kurzen Suchfensters weiterhin
neutralisiert. Nach der erfolgreichen One-shot-Entfernung existiert absichtlich
kein Listener mehr, der ein späteres Entladen oder Neuladen verfolgt. Für diesen
Release-Build DS2 neu starten, bevor nach einem Regionswechsel, der die
Coffin-Physik neu erzeugt haben könnte, ein Ersatz-Board ausgebracht wird.

Synthetische Laufzeittests prüfen die finale Speed-only-Transaktion, native
Lenkung und Nasshaftung, deaktivierte Lenk-/Telemetriepfade, den separaten
Trampolin ohne Telemetrie, Rollback, idempotente Wiederholung und
Neutralisierung beim Ressourcen-Unload. Zusätzliche Lifecycle-Tests prüfen die
exakten nativen Add-/Remove-Slots, die Ablehnung eines falschen Remove-Slots,
die atomare Übergabe vom Callback zum Worker, genau eine Entfernung außerhalb
von Callbacks, idempotentes Cleanup und den begrenzten Pfad ohne gefundenes
Ziel. `tools/validate_target.py` prüft das unterstützte Spielprogramm und die
exakten Laufzeitanker unabhängig.

Den ASI niemals bei laufendem Spiel entladen. Zum Entfernen das Spiel beenden,
beide Moddateien löschen und neu starten.
