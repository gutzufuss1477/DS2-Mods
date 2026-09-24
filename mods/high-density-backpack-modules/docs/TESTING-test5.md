# Test5: Gameplay und Frachtvorschau gemeinsam

Test5 ist als einzige aktive Backpack-ASI installiert. Die Charm-INI wurde unverändert übernommen; Test3 und Test4 bleiben als Sicherungen ausserhalb des Spielordners erhalten.

1. Spiel frisch starten und denselben zuvor erstellten Spielstand laden.
2. Zuerst direkt im Gameplay den Rucksack ansehen, ohne vorher die Rucksackanpassung zu öffnen. Überschüssige Modelle sollten verborgen sein.
3. Frachtverwaltung öffnen und die Figur drehen. Auch dort sollte die Anordnung sauber bleiben.
4. Menü schliessen und erneut im Gameplay prüfen; den Wechsel wiederholen. Wenn möglich, den Spielstand noch einmal laden und Schritt 2 wiederholen.

Das Log muss Test5 und `status=PATCH_APPLIED` zeigen. Die zusätzlichen Meldungen `GAMEPLAY_BACKPACK_VISIBILITY=ACTIVE; live_player_lookup` und `EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE; render_mesh_only` bestätigen, dass die jeweiligen zusätzlichen Erkennungswege verwendet wurden. Wenn ein Rucksack schon im ursprünglichen Manager registriert ist, ist die zusätzliche Meldung für ihn nicht erforderlich.

Bei weiterhin fehlerhafter Darstellung gilt der Wunsch des Nutzers: Test3 als Rückfall, Gameplay vor kosmetischer Frachtvorschau. Das endgültige Git-/Nexus-Update wartet auf dieses Testergebnis.
