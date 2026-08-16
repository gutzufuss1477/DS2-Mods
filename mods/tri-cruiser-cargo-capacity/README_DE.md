# DS2 Tri-Cruiser Cargo Capacity v1.1.0

High-Capacity-Mod für den Tri-Cruiser in **DEATH STRANDING 2: ON THE BEACH**.

## Kapazität

- Zielkapazität: **320 Fracht-Einheiten gesamt**
- Native Grössen bleiben erhalten: S=1, M=2, L=4, XL=6
- S/M/L/XL können beliebig gemischt und in beliebiger Reihenfolge geladen werden
- **v1.1.0 behält die vollen 320 Einheiten auch mit 0, 1 oder 2 montierten Seiten-Zubehörteilen**
- Getestet mit Akku-Einheiten und Anti-Schwebe-/Seitenzubehör
- Laden, Abladen, Fahren sowie Save/Reload bleiben im nativen Cargo-Lifecycle

Zusätzliche Fracht kann im Frachtmenü optisch überlappen oder am Bike unsichtbar/überlappend dargestellt werden. Das ist beabsichtigt. Priorität hat die stabile logische Frachtkapazität, nicht die optische Platzierung.

Bei stark beladenem Tri-Cruiser kann das Fahrzeuganpassungsmenü Änderungen an Seitenkomponenten blockieren, bis Fracht abgeladen wurde. Dieses native Garagenverhalten wird vom Mod nicht verändert.

## Installation / Update von v1.0.0

1. Einen funktionierenden 64-Bit-ASI-Loader installieren, z. B. Ultimate ASI Loader von ThirteenAG.
2. **Die alte `DS2_TriCruiser_CargoCapacity_v1.0.0.asi` löschen/ersetzen. Nicht beide Versionen gleichzeitig verwenden.**
3. `DS2_TriCruiser_CargoCapacity_v1.1.0.asi` in denselben Spiel-/ASI-Loader-Ordner kopieren.
4. Spiel normal starten.

Zum Deinstallieren die ASI-Datei entfernen.

## Kompatibilität / Baseline

Gebaut und validiert für **DS2.exe v1.10.89.0**.

Der Mod prüft vor der Installation die erwarteten Originalbytes an allen Patchstellen. Passt die EXE nicht zur unterstützten Version, wird fail-closed abgebrochen statt nur teilweise zu patchen.

## Validiert mit

- frei gemischter S/M/L/XL-Fracht
- voller 320-Einheiten-Kapazität
- Menü schliessen/öffnen und Fahren
- komplettem Abladen/Wiederaufladen
- Save/Reload
- zusätzlicher Rucksackfracht beim Aufsteigen
- einem und zwei montierten Seiten-Zubehörteilen
- Akku-Einheiten und Anti-Schwebe-/Seitenzubehör

## Technische Zusammenfassung

- normaler Cargo-Modus: 160 Einheiten pro nativer Bike-Seite = 320 gesamt
- nativer One-Side-Zubehörmodus: volle 320 Einheiten auf dem aktiven virtuellen Cargo-Pfad
- der echte Dual-Accessory-Zustand wird in diesen stabilen nativen One-Side-Pfad geleitet
- zyklische Wiederverwendung gültiger nativer Slot-Keys für S/M/L/XL
- virtuelle Tri-Cruiser-UI-Itemgrenze 13 -> 320 für die relevante Kapazitätsrechnung
- reale Zubehördaten werden weder gelöscht noch überschrieben
- keine NULL-Transform-Guards
- keine Ownership-/Relation-Manipulation
- keine Fake-Success-/Return-Overrides
- keine Laufzeitdiagnose, Statusdatei oder Telemetrie
