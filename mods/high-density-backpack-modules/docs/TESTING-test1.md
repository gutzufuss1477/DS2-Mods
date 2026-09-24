# Ingame-Test: v1.1.0-test1

Diese Version ist gebaut und lokal geprüft, aber noch nicht ingame bestätigt. Ziel: gleiche 30 Plätze für kleine und große Module, überzählige Modelle unsichtbar und alle freigeschalteten Charm-Effekte bei zwei sichtbaren Anhängern.

## Einsetzen

1. Spiel schließen. Für den Test einen separaten Spielstand verwenden.
2. Die bisherige High-Density-Backpack-ASI aus dem aktiven Mod-Ordner nehmen und die einzelne ASI aus dem test1-ZIP einsetzen. Vorhandenen ASI-Loader weiterverwenden.
3. Starten und im Log `DS2_HighDensityBackpackModules.log` nach `status=PATCH_APPLIED` schauen. Bei `...REJECTED...` bitte das Log aufbewahren.

## Module

1. Einige kleine und große Module gemischt montieren. Sam und das Rucksackgrundmodell müssen sichtbar bleiben; überlappende installierte Module sollen verschwinden. Die Liste muss weiterhin alle enthalten.
2. Ein sichtbares Modul entfernen bzw. verschieben/drehen: Ein zuvor ausgeblendetes Modul darf wieder sichtbar werden, wenn seine Fläche frei wird. Alle vier Drehungen ausprobieren.
3. Große Module bis insgesamt 30 Einträge ergänzen. Ein 31. Modul darf keinen vorhandenen Eintrag unbemerkt ersetzen. Batterieanzeige und andere verwendete Effekte vergleichen.
4. Anpassungsmenü verlassen, wieder öffnen, speichern, Spiel vollständig neu starten und laden. Anzahl, Effekte und Darstellung erneut prüfen.

Eine gerade platzierte Vorschau darf sich vorübergehend überlagern; sie ist noch kein installiertes Modul. Einblendung richtet sich nach den nativen Modellflächen. Bestimmte Spezialmodule können weiterhin native Effektbegrenzungen haben.

## Charms

1. Zwei sichtbare Anhänger normal wählen. Alle anderen bereits freigeschalteten Typen sollen ihre Effekte zusätzlich erhalten, auch wenn sie nicht als weiterer Slot im Menü erscheinen.
2. Im Log zeigt `UNLOCKED_CHARM_MASK` die verfügbaren Typen. `0x000000FF` entspricht allen acht. Die Maske wird nach Freischaltungen während des Rucksack-Updates aktualisiert, üblicherweise innerhalb einer Sekunde.
3. Einen gut messbaren Effekt testen, dessen Anhänger nicht sichtbar gewählt ist: beispielsweise Batteriebonus oder Ausdauer. Dann den zugehörigen Anhänger sichtbar wählen: Der zusätzliche Bonus darf sich dadurch nicht nochmals erhöhen.
4. Auch die situativen Effekte (Likes, Kampf, Tiere, Cryptobiosis, Bell, Porter) prüfen, soweit im Spielstand möglich. Menü-Navigation allein darf keine noch gesperrten Typen hinzufügen.
5. Die Auswahl ändern und nach Speichern/Neustart erneut prüfen.

Für Rückmeldung hilfreich: Log, Modulanzahl und -typen, sichtbare Charm-Auswahl sowie der konkrete Schritt, an dem etwas abweicht. Falls das Ausblenden nicht greift, reichen zuerst ein Vergleich im Menü und außerhalb des Menüs. Vor einem Wechsel zurück zu Vanilla das Layout wieder auf Vanilla-Größe reduzieren.
