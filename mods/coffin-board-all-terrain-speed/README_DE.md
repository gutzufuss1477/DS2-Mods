# Fast as Fuck Coffin Board + Floating Carrier v1.1.2

Buildgebundener ASI-Mod für **DEATH STRANDING 2: ON THE BEACH**, Steam-PC
`1.10.89.0`.

Der Mod gibt dem Coffin Board auf Land und Wasser dieselben erhöhten
Geschwindigkeitslimits, verwendet die getestete schnellere Beschleunigung und
erlaubt optional das Fahren mit einem angekoppelten, beladenen Lastenschweber.
Die Verbindung bleibt auch bei schneller Fahrt, Wasserüberquerungen,
Kollisionen und grossen Sprüngen erhalten, die normalerweise die native
Distanzgrenze auslösen würden.

Die finale Fassung verändert bewusst weder Lenkung noch Nasshaftung, generische
Fahrzeugphysik, Fracht- oder Kollisionsschaden, Spielarchive oder Spielstände.

## Installation

Das Release-Archiv enthält genau zwei Dateien:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Beide Dateien neben `DS2.exe` kopieren. Ein externer 64-Bit-ASI-Loader wird
vorausgesetzt. Von einer früheren Version unbedingt beide Dateien ersetzen,
das Spiel vollständig neu starten und ein neues Coffin Board ausbringen, da
eine bereits erzeugte Physikkomponente ihre alte Getriebetabelle behalten kann.

Zum Deinstallieren das Spiel schliessen, beide Dateien entfernen und DS2 neu
starten. Die ASI niemals bei laufendem Spiel ersetzen, entfernen oder entladen.

## Konfiguration

Die vollständige Release-INI lautet:

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400

; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.
AllowFloatingCarrier=1
```

`SpeedPercent=500` ergibt auf Land und Wasser normale/Boost-Limits von
`300/400 km/h` und ändert die Endübersetzung von `15` auf `3`.
Gültiger Bereich: `100-1000`.

`AccelerationPercent=400` setzt die Antriebskraft auf `20x`. Zusammen mit
der Übersetzung von `0,2x` ergibt das nominell das getestete vierfache
effektive Antriebsmoment. Gültiger Bereich: `100-500`.

`AllowFloatingCarrier=1` aktiviert die ausschliesslich für das Coffin Board
geltenden Aufstiegs- und Verbindungsschutz-Patches. Mit `0` bleibt die
native Einschränkung bestehen. Nach jeder Änderung DS2 vollständig neu starten.

## Verhalten des Lastenschwebers

Bei aktivierter Unterstützung bleibt der aktuell verbundene, beladene
Lastenschweber beim Aufsteigen an das Coffin Board gekoppelt. Solange genau
dieses Board-/Schweber-Paar aktiv ist, verhindert der Mod die native
Trenntransaktion bei zu grosser Distanz und unterdrückt die dadurch überflüssig
gewordene HUD-Meldung sowie Sams Kommentar. Nach dem Absteigen lässt sich der
Schweber weiterhin normal ab- und wieder ankoppeln und bei einer späteren Fahrt
erneut verwenden.

Die Spieltests umfassten schnelle Land- und Wasserfahrt, Kollisionen und grosse
Sprünge. In einem bestätigten Lauf wurden alle 77 erzeugten nativen
Überspannungswarnungen unterdrückt; der Schweber blieb angekoppelt und weder
Warntext noch Sams Reaktion erschienen.

Die Fracht wird dadurch nicht unzerstörbar. Fracht und Container können
weiterhin den nativen Aufprall-, Sturz-, Wasser- und Kollisionsschaden erleiden.

## Sicherheit und Prüfung

Die ASI akzeptiert ausschliesslich die geprüfte Steam-Version. PE-Metadaten,
StreamingSystem-Funktionen, Identität und Layout der Coffin-Ressource,
Antriebs-Hook-Signatur, Coffin-Physics-vtable sowie die Coffin-spezifischen
Lastenschweber-Instruktionsanker müssen exakt passen. Andere Fahrzeuge werden
nicht erfasst.

Die Geschwindigkeits-Transaktion ändert nur die vier Limits, die
Endübersetzung und den für den erhöhten Bereich nötigen Slip-Grenzwert. Die
Antriebskraft bleibt neutral, bis alle Ressourcenänderungen erfolgreich sind;
bei einem Fehler werden bereits ausgeführte Änderungen zurückgerollt.

v1.1.2 verwendet wieder die im Spiel bestätigte Startreihenfolge: zuerst
werden die Coffin-spezifischen Lastenschweber-Guards installiert, danach der
Antriebs-Hook und anschliessend der Streaming-Listener. Der begrenzte
Discovery-Worker sucht weiter nach der Coffin-Physics-Ressource, bis die
Geschwindigkeitstransaktion abgeschlossen ist. Dadurch funktioniert
`SpeedPercent=500` wieder zusammen mit der finalen Warnungsunterdrückung. Nach
erfolgreichem Patch oder Ablauf des Suchfensters entfernt der Worker den
Listener ausserhalb der Callbacks.

Public v1.1.2 enthält weder den wirkungslosen experimentellen
Follow-Step-Hook noch die verworfenen globalen „Distanz“-Writes. Die stabile
Funktion verwendet ausschliesslich exakte Coffin-Aufstiegs-, Verbindungs-,
Trennereignis- und Benachrichtigungsschutz-Patches.

Extreme Geschwindigkeiten können Kamera, World-Streaming und Aufprallschäden
beeinflussen. Vorher den Spielstand sichern und zunächst offline testen. Nach
einem Spielupdate die ASI entfernen, bis eine passende buildgebundene Version
verfügbar ist.
