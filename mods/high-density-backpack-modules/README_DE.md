# High-Density Backpack Modules v1.1.0


Mehr Rucksackmodule, eine aufgeräumte Darstellung und acht Accessoire-Plätze für **DEATH STRANDING 2: ON THE BEACH**.

## Funktionen

- Bis zu **30 funktionale Module**, unabhängig von ihrer ursprünglichen Rastergrösse. Jedes benötigt einen logischen Platz.
- Belegte Positionen werden automatisch auf einen freien Platz verschoben. Bei 30 Modulen gilt weiterhin der normale Ersetzen-Dialog.
- Sichtbare Modelle behalten ihre Originalgrösse und werden separat angeordnet. Was optisch nicht mehr passt, wird in Rucksackanpassung, Frachtverwaltung und normalem Gameplay ausgeblendet, bleibt aber montiert.
- **Acht Accessoire-Plätze:** Die letzten beiden Zeilen sind sichtbar, die ersten sechs unsichtbar. Nur freigeschaltete und tatsächlich ausgerüstete Charms werden für zusätzliche Effekte berücksichtigt.
- Die spielinternen Wirkungsgrenzen einzelner Module bleiben bestehen. Nicht jede Effektkombination wurde im Spiel vermessen.

## Installation und Update

1. Spiel vollständig beenden. Ein kompatibler externer 64-Bit-ASI-Loader wird benötigt.
2. Frühere Backpack-ASIs aus allen aktiven Loader-Ordnern entfernen. **Nur eine Version gleichzeitig aktiv lassen**, sonst kann weiterhin eine ältere Version geladen werden.
3. `DS2_HighDensityBackpackModules_v1.1.0.asi` neben `DS2.exe` kopieren.
4. Eine bereits vorhandene `DS2_HighDensityBackpackModules.charms.ini` behalten.
5. Im Log `DS2_HighDensityBackpackModules.log` die Version **v1.1.0** und `status=PATCH_APPLIED` prüfen.

Das Download-ZIP enthält genau eine ASI, keinen Loader und keinen Installer. Die sechs zusätzlichen Charm-Plätze beginnen leer. Ihre Auswahl wird beim Verlassen der Anpassung automatisch in der INI neben der Spiel-EXE gespeichert. Diese Einstellung gilt für die gesamte Installation, also auch bei einem Spielstandwechsel; nur aktuell verfügbare Charms werden berücksichtigt. Die zwei sichtbaren Charms bleiben im normalen Spielstand.

## Optionale Variante: Classic Overlap

`DS2_HighDensityBackpackModules_v1.1.0_Classic_Overlap.asi` bietet ebenfalls 30 logische Modulplätze und acht Charm-Plätze. Die Modulmodelle bleiben an ihren gewählten Positionen sichtbar und dürfen sich optisch überlappen. Den freien Rasterplatz wählst du selbst; eine belegte Zelle wird nicht automatisch umgangen. Module auf verschiedenen Zellen können sich durch ihre Originalgröße trotzdem überdecken.

Installiere **entweder** die normale 1.1.0-ASI **oder** Classic Overlap. Beide verwenden dieselbe Charm-INI; beim Wechsel bleiben die Zusatz-Charms erhalten. Die Variante besteht die lokalen Bau- und Codeprüfungen. Darstellung und Menüverhalten müssen noch im Spiel geprüft werden. [Details zur Variante](docs/CLASSIC_OVERLAP.md).

## Bekannte Einschränkungen

- Die Darstellung in Gameplay, Rucksackanpassung und Frachtverwaltung wurde vom Nutzer bestätigt, auch für den gemeldeten Fall mit einem vorhandenen Spielstand. Ungewöhnliche Modellformen und Animationen können weiterhin Clipping verursachen.
- Die Grenze bleibt bei 30 Modulen. Welche davon sichtbar sind, hängt von Bestückung und verfügbarem Platz ab.
- Im Spieltest konnten alle sieben aktuell verfügbaren Charms gleichzeitig montiert werden. Der achte Platz bleibt für den noch nicht verfügbaren Charm frei. Acht Menüplätze sind lokal geprüft.
- Nicht alle Charm-Effekte, Neustart-/Spielstandwechsel und Menüwege wurden im Spiel vollständig geprüft. Details: [Validierung](docs/VALIDATION.md).

## Unterstützte Spielversion

`DS2.exe v1.10.89.0`, PC / Steam. Andere EXE-Versionen werden nicht zugesichert. Bei nicht passenden Versions- oder Codeprüfungen bleibt die Mod inaktiv.

## Deinstallation

Mit aktiver Mod die zusätzlichen Charms entfernen und die Module auf eine mit den Originalgrössen passende Bestückung reduzieren. Anwenden, speichern und das Spiel beenden. Anschliessend die ASI entfernen; INI und Log können ebenfalls entfernt werden.
