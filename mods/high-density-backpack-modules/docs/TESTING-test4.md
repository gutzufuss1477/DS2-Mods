# Test4: Frachtvorschau

Test4 ergänzt die Vorschau der Ausrüstungsverwaltung im bestehenden Sichtbarkeits- und Platzierungsfilter. Keine zusätzlichen Hooks; Slots, Effekte und logische Modulplätze bleiben wie in Test3. Der Erfolg im Spiel ist noch offen.

1. Spiel beenden. Die bisherige Backpack-ASI durch die einzelne ASI aus `dist/DS2_High_Density_Backpack_Modules_v1.1.0-test4.zip` ersetzen. Nur eine Backpack-Version aktiv lassen; die bestehende Charm-INI behalten.
2. Mit mehreren überschüssigen Modulen die Frachtverwaltung öffnen. Prüfen, ob die Überlappungen verschwunden sind; Ansicht drehen und das Menü mehrmals schließen/öffnen.
3. Zur Rucksackanpassung wechseln, ein Modul hinzufügen oder entfernen und erneut die Frachtverwaltung sowie normales Gameplay prüfen. Die sichtbare Auswahl darf sich entsprechend der Bestückung ändern.
4. Ausrüstungsansicht innerhalb der Frachtverwaltung umschalten, falls verfügbar. Auch nach diesem Wechsel sollten die überschüssigen Modelle verborgen bleiben.

Das Log neben dem Spiel sollte Test4, `status=PATCH_APPLIED` und beim Erreichen der zusätzlichen Vorschau `EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE` enthalten. Diese Meldung bestätigt den Aufruf, nicht das endgültige Bild nach allen Render-Schritten.

Falls das Problem bleibt oder ein neuer Fehler auftritt: Test3-ZIP ist unverändert als Rückfall vorhanden. Vor dem Start wieder nur dessen ASI aktiv lassen. Veröffentlichung des neuen Kandidaten erst nach dem Ergebnis dieses Spieltests; bei aufwendigerer Restursache bleibt Test3 die vorgesehene Update-Basis.
