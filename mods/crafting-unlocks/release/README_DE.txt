DS2 CRAFTING & EQUIPMENT OVERHAUL 1.6.0
Unlocks, Free Crafting, Durability & ATLAS Gear

Funktionen:
- 132 konfigurierbare unterstützte Gegenstände/Varianten.
- 11 zusätzliche Gegner-Drop-Waffen inklusive Geisterklinge.
- Frühe Freischaltungen bei weiterhin funktionierender Vanilla-Progression.
- Optionales FreeCrafting.
- Herstellbare Omnireflektor-Stiefel und Chirale Stiefel.
- Optionaler Haltbarkeitsfaktor (1.0-1000.0).
- Optionaler Unbreakable-Modus.
- Backpack Cover Lv.1/Lv.2 mit Haltbarkeitsunterstützung.
- Schweres MG [MZ] bewusst nicht enthalten.

Installation:
ds2_crafting_unlocks.asi und ds2_crafting_unlocks.ini neben DS2.exe kopieren.
Ein kompatibler 64-Bit-ASI-Loader wird separat benötigt.
Nach INI-Änderungen das Spiel neu starten.

Unterstützte DS2.exe: 1.10.89.0

ATLAS-Ausrüstung (optional, neu in 1.6.0)

[AtlasEquipment]
Enabled=0

Mit Enabled=1 in diesem Abschnitt sind ATLAS-Stiefel und ATLAS-Skelett an regulären Herstellungsstationen ohne Story- oder Vorlagen-Freischaltung herstellbar. Standardmäßig ist die Option aus. Nach INI-Änderungen neu starten.

ATLAS-Stiefel vereinen Halt, Stoßabsorption, stärkere Tritte und leise Schritte. Sie verwenden die Optik der Pizzabäcker-Stiefel, wiegen 0,2 kg und haben 3400 Basishaltbarkeit.
Das ATLAS-Skelett vereint Kampf-, Boost- und Bokka-Effekte auf Stufe 3, einschließlich Bokka-Doppelsprung und gleichzeitiger Hüftfracht. Es verwendet die Optik des normalen Boost-Skeletts St.3, wiegt 4,0 kg und hat 20000 Basishaltbarkeit. Der Tragkraftbonus beträgt bis zu 180 kg mit Akku / 100 kg ohne Akku; die Boni werden nicht addiert. Akkuverbrauch und die nativen Sprung-/Eingabebedingungen gelten weiterhin.

FreeCrafting sowie die gemeinsamen Haltbarkeitsoptionen Multiplier und Unbreakable gelten auch für beide ATLAS-Items. Ohne diese Optionen gelten die ATLAS-Basiswerte und die Materialkosten der Transportstiefel bzw. des Kampfskeletts St.1. Die 132 bisherigen Item-Regeln steuern weiter die normale Ausrüstung. ATLAS wird über seinen eigenen Abschnitt unabhängig von DefaultUnlock angeboten.

Namen und Beschreibungen erscheinen bei deutscher UI auf Deutsch, sonst auf Englisch. Originalausrüstung behält ihre eigenen Werte und ihre Darstellung.

Umstieg und Spielstände
Die alte ds2_overpowered_equipment.asi entfernen. Nur ds2_crafting_unlocks.asi und die dazugehörige INI verwenden. Die ATLAS-Kennungen bleiben für vorhandene Exemplare erhalten.
AtlasEquipment.Enabled=0 blendet die beiden Rezepte aus; Ressourcen und Effekte bereits gespeicherter ATLAS-Items bleiben verfügbar. Dafür muss CraftingUnlocks.Enabled=1 bleiben. Der globale Hauptschalter, eine Deinstallation oder eine Rückkehr zu einer Version vor 1.6.0 schalten diese Unterstützung ab; für eine Deinstallation einen Spielstand vor der Herstellung von ATLAS verwenden.

Prüfstand
Der Nutzer hat ATLAS-Spielverhalten, Herstellung mit frühem Spielstand, Erhalt nach Neustart/Laden und das Anbringen von Hüftfracht mit der eigenständigen Fassung bestätigt. Die gemeinsame Version 1.6.0 besteht native Code-, Ressourcen-, Konfigurations-, Kosten- und Haltbarkeitstests. Spielstart und Registrierung beider Items sind lesend bestätigt; im laufenden Spiel wurden die angelegten IDs 103/104, alle drei versorgten Skelett-Kennzeichen auf Stufe 3 und +180 kg Tragkraftbonus erfasst. Der Nutzer meldet einen erfolgreichen ersten Spieltest mit dem integrierten Build. Ein eigener neuer Speicher-/Ladedurchlauf und die Optionswechsel im Spiel wurden noch nicht separat bestätigt. Einzelne Schuhwirkungen und Kampfschutz wurden noch nicht vollständig im Spiel nachgewiesen. Der Doppelsprung-Hinweis kann bei kurzen Sprüngen wie im akzeptierten Testbuild spät erscheinen.
