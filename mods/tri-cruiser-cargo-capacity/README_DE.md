# DS2 Tri-Cruiser Cargo Capacity v1.0.0

Stabile Kapazitätserweiterung für den Tri-Cruiser in **DEATH STRANDING 2: ON THE BEACH**.

## Funktion

- Zielkapazität: **320 Footprint-Einheiten gesamt**
- Native Grössen bleiben unverändert: S=1, M=2, L=4, XL=6
- S/M/L/XL können frei gemischt und in beliebiger Reihenfolge geladen werden
- Laden, Abladen, Fahren sowie Save/Reload bleiben im nativen DS2-Lifecycle

Zusätzliche Container können im Frachtmenü optisch überlappen oder am Bike unsichtbar sein. Das ist beabsichtigt. Priorität haben Stabilität und nutzbare Frachtkapazität, nicht die physische Darstellung aller Overflow-Container.

## Installation

1. Ältere Tri-Cruiser-Test-/Entwicklungs-ASIs entfernen.
2. `DS2_TriCruiser_CargoCapacity_v1.0.0.asi` in den verwendeten ASI-/Mod-Loader-Ordner kopieren.
3. Spiel normal starten.

## Unterstützte Spielversion

Gebaut und getestet für **DS2.exe v1.10.89.0**.

Vor jedem Patch prüft der Mod die erwarteten Originalbytes. Bei einer nicht unterstützten EXE oder einem Konflikt mit einem anderen Patch wird fail-closed abgebrochen, statt nur teilweise zu patchen.

## Validierter Entwicklungsstand

Die zugrunde liegende 320er-Testversion wurde mit frei gemischter S/M/L/XL-Fracht getestet. Menü schliessen/öffnen, Fahren, komplettes Abladen/Wiederaufladen sowie Save/Reload funktionierten stabil. Die Kapazitätsgrenze wurde anschliessend wieder über den nativen Reject-Pfad erreicht.

## Technischer Release-Stand

Enthalten sind nur die bewährten Kernänderungen:

- 160 Footprint-Einheiten pro nativer Bike-Seite
- zyklische Wiederverwendung der nativen Slot-Key-Bänke für S/M/L/XL
- virtuelle Tri-Cruiser-Itemgrenze 13 -> 320 nur für die relevante Kapazitätsrechnung
- keine NULL-Transform-Guards
- keine Ownership-/Relation-Manipulation
- keine Fake-Success-/Result-Overrides
- keine Runtime-Diagnosehooks und keine Statusdatei
