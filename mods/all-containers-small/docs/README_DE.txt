ALL CONTAINERS SMALL 1.0.0
DEATH STRANDING 2: ON THE BEACH – Steam / PC 1.10.89.0
Autor: gutzufuss1477

FUNKTION
Normale Frachtcontainer von M bis 4L erhalten Grösse S, eine Platzeinheit
und ein kompaktes S-Kistenmodell. Das gilt auch für Materialcontainer,
Leiter, PCC-Pakete und normale verpackte Waffen/Ausrüstung.
Keine INI und keine Tastenkombination erforderlich.

Inhalt, Materialmenge, Item-Identität, Gewichts- und Haltbarkeitsfelder
bleiben erhalten. Beispiel: Resine [320] enthält weiterhin 320 Einheiten
und wiegt 32 kg, benötigt aber eine S-Kiste. Dies ist kein Gewichts-Mod.

AUSNAHMEN
Schwebende Träger/EnemyCart, gefangene Tiere samt Käfigen, BB-Pods und
Prototyp, Menschen/Leichensäcke, Fahrzeuge, Coffinboard, GhostMech-
Geschütze, eine geskriptete Sonderidentität, Nicht-Fracht sowie
Human-/Coffinboard-Volumen bleiben original. Sie benötigen eigene
funktionale Komponenten. Nicht jeder Sonderbehälter wird deshalb ersetzt.

INSTALLATION
1. Spiel vollständig schliessen.
2. Ein funktionierender x64-ASI-Loader muss vorhanden sein, etwa
   Ultimate ASI Loader von ThirteenAG:
   https://github.com/ThirteenAG/Ultimate-ASI-Loader
   Der Loader ist nicht enthalten. Einen bereits funktionierenden
   Loader beibehalten.
3. ds2_all_containers_small.asi in den Ordner neben DS2.exe kopieren.
4. Spiel normal starten und Spielstand laden. Der Mod ist automatisch aktiv.

Optional prüft install.ps1 die Spielversion und den Paket-Hash und sichert
eine vorherige gleichnamige ASI. Für einen abweichenden Spielpfad
-GameDir mit dem tatsächlichen Ordner verwenden.

KOMPATIBILITÄT
Windows x64 und die exakt geprüfte Steam-EXE 1.10.89.0:
bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b
Andere Builds und Änderungen an denselben Hookstellen blockieren die
Aktivierung. Nicht jede Modkombination ist getestet. Spielarchive und
DS2.exe auf Datenträger werden nicht verändert. Das normale Speichern
des Spiels bleibt aktiv.

BESTÄTIGT
Am 12.11.2026 bestätigte der Autor den erfolgreichen Spielstart,
Frachtabholung mit S-Kisten, S-Grösse im Inventar, unveränderte angezeigte
Gewichte und kleine Kisten am Rucksack. Die Galerie zeigt echte Spielbilder.
Version 1.0.0 verwendet diesen 0.1.1-Laufzeitcode mit geänderter
Versionsanzeige. Zusätzlich bestehen 17 lokale Tests.
Nicht umfassend geprüft: alle Ausrüstungsfunktionen, lange Sitzungen und
Speichern/Neuladen mit neu gepackter Fracht.

FEHLERSUCHE
ds2_all_containers_small.log liegt neben der ASI. HOOKS_READY und
CATALOGUE_READY bestätigen Aktivierung und Katalogverarbeitung.
VERSION_BLOCKED, HOOK_BLOCKED oder LATE_LOAD_BLOCKED bedeutet:
Mod konnte nicht aktiviert werden. CATALOGUE_BLOCKED bedeutet:
Unerwartete Frachtdefinitionen; Grössen wurden nicht geändert.
Nach einem ASI-Wechsel das Spiel vollständig neu starten.

ENTFERNEN
Spiel schliessen und ds2_all_containers_small.asi entfernen. Nach dem
Neustart gelten Originalgrössen. Fracht vorher so umlagern, dass sie
mit Originalgrössen Platz findet. Vor Änderungen an Frachtmods einen
separaten Spielstand behalten.

QUELLEN UND ANLEITUNG
https://github.com/gutzufuss1477/DS2-Mods/tree/main/mods/all-containers-small
