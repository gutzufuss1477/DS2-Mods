# Fast as Fuck Coffin Board + Floating Carrier v1.1.7

Buildgebundener ASI-Mod für DEATH STRANDING 2: ON THE BEACH, Steam-PC `1.10.89.0`.

Der Mod gibt dem Coffin Board auf Land und Wasser dieselben erhöhten Geschwindigkeitslimits, verwendet die getestete schnellere Beschleunigung und erlaubt optional das Fahren mit einem angekoppelten, beladenen Lastenschweber. Die Verbindung bleibt auch bei schneller Fahrt, Wasserüberquerungen, Kollisionen und grossen Sprüngen erhalten, die normalerweise die native Distanzgrenze auslösen würden.

v1.1.7 behebt den durch den Lastenschweber-RideVehicle-Hook verursachten Absturz beim Aufsteigen auf andere Fahrzeuge wie den Tri-Cruiser. Der native Fahrzeug-Resolver wird jetzt zuerst mit seiner ursprünglichen Aufrufsemantik ausgeführt. Erst danach greift die ausschliesslich für das Coffin Board bestimmte Lastenschweber-Logik ein. Pickup und Tri-Cruiser bleiben damit im nativen Fahrzeugpfad.

## Installation

Das Nexus-Release enthält genau:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Beide Dateien neben `DS2.exe` kopieren. Ein externer 64-Bit-ASI-Loader wird vorausgesetzt. Von einer früheren Version beide Dateien ersetzen und DS2 vollständig neu starten. Nach Änderungen an der Geschwindigkeit empfiehlt sich ein neu erzeugtes Coffin Board, da eine bereits erstellte Physikkomponente ihre alte Getriebetabelle behalten kann.

Zum Deinstallieren das Spiel schliessen, beide Dateien entfernen und DS2 neu starten. Die ASI niemals bei laufendem Spiel ersetzen, entfernen oder entladen.

## Konfiguration

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400

; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.
AllowFloatingCarrier=1
```

`SpeedPercent=500` ergibt auf Land und Wasser normale/Boost-Limits von `300/400 km/h` und ändert die Endübersetzung von `15` auf `3`. Gültiger Bereich: `100-1000`.

`AccelerationPercent=400` setzt die Antriebskraft auf `20x`. Zusammen mit der Übersetzung von `0,2x` ergibt das nominell das getestete vierfache effektive Antriebsmoment. Gültiger Bereich: `100-500`.

`AllowFloatingCarrier=1` aktiviert die Coffin-spezifischen Aufstiegs- und Verbindungsschutz-Patches. Mit `0` bleibt die native Einschränkung bestehen. Nach Änderungen DS2 vollständig neu starten.

## Verhalten des Lastenschwebers

Bei aktivierter Unterstützung bleibt der aktuell verbundene, beladene Lastenschweber beim Aufsteigen an das Coffin Board gekoppelt. Solange genau dieses Board-/Schweber-Paar aktiv ist, verhindert der Mod die native Trenntransaktion bei zu grosser Distanz und unterdrückt die dadurch überflüssige HUD-Meldung sowie Sams Kommentar. Nach dem Absteigen lässt sich der Schweber weiterhin normal ab- und wieder ankoppeln.

Die Fracht wird dadurch nicht unzerstörbar. Fracht und Container können weiterhin nativen Aufprall-, Sturz-, Wasser- und Kollisionsschaden erleiden.

## Fahrzeug-Kompatibilitätsfix in v1.1.7

Der Carrier-Preserve-Trampoline aus v1.1.2 veränderte die ursprüngliche RideVehicle-Resolver-Aufrufsemantik, bevor das Spiel das bestiegene Fahrzeug vollständig aufgelöst hatte. Beim Coffin Board funktionierte dies, beim Tri-Cruiser konnte es jedoch direkt zu einem CTD führen.

v1.1.7 führt zuerst den originalen Resolver-Block mit nativem Register- und Aufrufzustand aus. Erst nach dem nativen Resolver wird der Coffin-spezifische Zustand geprüft und entschieden, ob die Carrier-Trenntransaktion umgangen werden muss.

Im Spiel bestätigt:

- Pickup lässt sich normal besteigen
- Tri-Cruiser lässt sich ohne CTD besteigen
- Coffin Board lässt sich normal besteigen
- Coffin Board lässt sich mit angekoppeltem, beladenem Lastenschweber besteigen
- hohe Geschwindigkeit bleibt aktiv
- Lastenschweber bleibt gekoppelt
- überflüssige Distanzwarnung und Sams Kommentar bleiben unterdrückt

## Sicherheit und Prüfung

Die ASI akzeptiert ausschliesslich die geprüfte Steam-Version. PE-Metadaten, StreamingSystem-Funktionen, Identität und Layout der Coffin-Ressource, Antriebs-Hook-Signatur, Coffin-Physics-vtable sowie die Carrier-Instruktionsanker müssen exakt passen.

Extreme Geschwindigkeiten können Kamera, World-Streaming und Aufprallschäden beeinflussen. Vorher den Spielstand sichern und zunächst offline testen. Nach einem Spielupdate die ASI entfernen, bis eine passende buildgebundene Version verfügbar ist.
