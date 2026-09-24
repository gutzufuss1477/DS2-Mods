# Ingame-Test: v1.1.0-test3

Test2 erreicht laut Nutzertest 30 Module und erleichtert das Hinzufügen. Test3 ergänzt acht bedienbare Accessoire-Plätze und eine getrennte Anordnung der sichtbaren Module. Test3 ist noch nicht ingame bestätigt.

1. Spiel schließen und die bisherige Backpack-ASI durch die einzelne test3-ASI ersetzen.
2. Spielstand laden und die Rucksackanpassung öffnen. Unter Accessoires sollen acht Zeilen erscheinen. Die letzten zwei Zeilen steuern die sichtbaren Anhänger; die ersten sechs sind zusätzliche unsichtbare Plätze. Anfangs sind die sechs neuen Plätze leer.
3. Alle verfügbaren unterschiedlichen Charms ausrüsten. Einen zusätzlichen Platz ersetzen, einen einzelnen entfernen und mehrere gemeinsam entfernen. Andere Plätze müssen erhalten bleiben; derselbe Typ darf nicht mehrfach ausgewählt werden.
4. Eine andere Auswahl ansehen und abbrechen: Der zuvor ausgerüstete Charm muss zurückkommen. Den Zähler im Abschnitt prüfen.
5. Menü verlassen, Spiel vollständig neu starten und erneut öffnen. Die sechs zusätzlichen Plätze werden beim Verlassen in `DS2_HighDensityBackpackModules.charms.ini` neben der ASI gespeichert. Diese Auswahl gilt installationsweit, auch beim Wechsel des Spielstands. Nur dort freigeschaltete Charms wirken. Die zwei sichtbaren Plätze bleiben Teil des normalen Spielstands.
6. Einen messbaren Effekt auf einem unsichtbaren Platz prüfen, z. B. Batteriebonus. Entfernen muss den zusätzlichen Effekt beenden. Anders als test2 aktiviert test3 nicht mehr automatisch alle freigeschalteten Charms.
7. Das bisherige Layout mit 30 Modulen laden. Die sichtbaren Modelle werden unabhängig von ihren gespeicherten Montagekoordinaten verteilt. Kleine unterschiedliche Typen haben Vorrang, danach weitere passende Exemplare. Nur Modelle, deren Originalfläche nicht mehr hineinpasst, werden verborgen. Beim Entfernen/Ergänzen und nach Menüwechsel darf nichts dauerhaft falsch ausgeblendet bleiben.
8. Mit einem kleinen gemischten Layout anfangen und dann bis 30 auffüllen. Die Liste muss alle installierten Einträge behalten; das einfache Hinzufügen bleibt erhalten. Die gerade ausgewählte Vorschau kann sich vorübergehend überlagern.

Das Log soll `v1.1.0-test3`, `status=PATCH_APPLIED` und nach Öffnen `ACCESSORY_MENU_SLOTS=8` enthalten. Bei Auffälligkeiten sind Log, betroffener Platz und der genaue Bedienungsschritt hilfreich.
