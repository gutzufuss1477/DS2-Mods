# DS2 Floating Carrier Cargo Capacity v1.0.0

Kapazitätsmod für den **Lastenschweber / Floating Carrier** in **DEATH STRANDING 2: ON THE BEACH**.

## Kapazität

- Gesamt-Footprint: **240 Einheiten** statt nativ 36.
- Native Frachtgrössen bleiben erhalten: **S=1, M=2, L=4, XL=6**.
- Beide Carrier-Seiten werden von **18 auf 120 Einheiten** erweitert.
- Gemischte S/M/L/XL-Fracht wird unterstützt.
- Die sichtbare Platzierung bleibt nativ; zusätzliche Fracht wächst weiter nach oben.

## Gewichtslimits

Beide Lastenschweber-Level werden unterstützt:

- **Level 1**: 300 kg -> **5000 kg**.
- **Level 2**: 900 kg -> **5000 kg**.
- Die zugehörigen Action-Schwellen werden von 150/450 kg auf **2500 kg** erhöht.

5000 kg ist der konfigurierte Mod-Grenzwert. Ingame wurde für beide Level bestätigt, dass die nativen Überlastgrenzen überschritten werden können und der Schweber weiterhin ziehbar bleibt. Die vollen 5000 kg wurden nicht praktisch ausgereizt, weil vorher die physische Höhe des Frachtturms relevanter wird.

## Physische Grenze

Der Lastenschweber verwendet weiterhin die native sichtbare Frachtplatzierung. Im Test wuchs auch wild gemischte Fracht einfach weiter nach oben, ohne dass zusätzliche Placement-Hooks nötig waren.

Sehr hohe Stapel können weiterhin mit Decken, Terminaldächern oder anderer Weltgeometrie kollidieren. Diese physische Kollision wird absichtlich nicht deaktiviert.

## Installation

1. Einen funktionierenden 64-Bit-ASI-Loader installieren.
2. Ältere Floating-Carrier-Test-/RC-ASIs entfernen.
3. `DS2_FloatingCarrier_CargoCapacity_v1.0.0.asi` in dasselbe Spiel-/ASI-Verzeichnis kopieren, das auch für die anderen DS2-ASIs verwendet wird.
4. Spiel normal starten.

Zum Deinstallieren die ASI-Datei entfernen.

## Kompatibilität / Basis

- Erstellt und validiert für **DS2.exe v1.10.89.0**.
- Erwartete Originalbytes und Constructor-Werte werden vor dem Patch geprüft. Bei einer nicht unterstützten EXE-Version wird nicht teilweise gepatcht.
- Pickup und Tri-Cruiser verwenden separate Cargo-Pfade; der Mod ist daher für die parallele Verwendung mit diesen Kapazitätsmods ausgelegt.

## Technische Zusammenfassung

- ItemCap **36 -> 240**.
- Carrier-Packer Seite A/B **18 -> 120**.
- Live-Patch der globalen `DSBaggageConfig`.
- Lv1 **300 / 150 / 300 -> 5000 / 2500 / 5000**.
- Lv2 **900 / 450 / 900 -> 5000 / 2500 / 5000**.
- Constructor-Fallback für alle sechs Werte.
- Kein Cheat Engine.
- Kein PAGE_GUARD, keine Breakpoints und kein Single-Step.
- Keine Ownership-/Relation-Manipulation.
- Kein Fake-Success.
- Keine STATUS-Datei oder Telemetrie im Release-Build.

## Sicheres Entfernen

Vor dem Entfernen des Mods nach Möglichkeit wieder auf native Werte reduzieren:

- maximal **36 Footprint-Einheiten**;
- Lv1 maximal **300 kg**;
- Lv2 maximal **900 kg**.
