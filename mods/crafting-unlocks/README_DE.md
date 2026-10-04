# Crafting & Equipment Overhaul

Unlocks, Free Crafting, Durability & ATLAS Gear

Version 1.6.1 für DEATH STRANDING 2: ON THE BEACH.

Bisher Crafting Overhaul. ASI- und INI-Dateinamen bleiben beim Update gleich.

Funktionen
- Optionale ATLAS-Stiefel und ATLAS-Skelett mit vereinten Schuh- und Kampf-/Boost-/Bokka-Effekten.
- Optionaler nativer Gold-Skin des Boost-Skeletts St.3 für ATLAS mit GoldSkeletonSkin=1.
- 132 einzeln konfigurierbare unterstützte Rezepte/Gegenstände.
- Frühe Freischaltung bei weiterhin funktionierender Vanilla-Progression.
- 11 zusätzliche Gegner-Drop-Waffen, die Sam regulär benutzen kann, inklusive Geisterklinge.
- Rucksackmodule, Regenschutz und Anhänger über das native Rucksackmenü.
- Omnireflektor-Stiefel und Chirale Stiefel herstellbar.
- Optionales FreeCrafting.
- Optionaler Haltbarkeitsfaktor von 1.0 bis 1000.0 und Unbreakable-Modus.
- Eigener Haltbarkeitspfad für die 8 bisherigen herstellbaren Stiefel plus ATLAS-Stiefel.
- Backpack Cover Lv.1/Lv.2 verwenden denselben Multiplier-/Unbreakable-Schalter.
- Fracht, Auftragsfracht und Frachtcontainer bleiben Vanilla.
- Das schwere MG [MZ] ist bewusst nicht enthalten, da kein normaler Cargo-/Inventar-Übergabepfad existiert.

Schnellkonfiguration
[CraftingUnlocks]
Enabled=1
DefaultUnlock=1
FreeCrafting=0

[AtlasEquipment]
Enabled=0
GoldSkeletonSkin=0

[Durability]
Enabled=0
Multiplier=2.0
Unbreakable=0

CraftingUnlocks.Enabled=0 ist der globale Hauptschalter und installiert keine Patches.
Unbreakable=1 benötigt Durability.Enabled=1 und hat Vorrang vor Multiplier.

Kompatibilität
Unterstützte DS2.exe: 1.10.89.0
SHA-256: bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b

ATLAS-Ausrüstung (optional, neu in 1.6.0)

[AtlasEquipment]
Enabled=0
GoldSkeletonSkin=0

Mit Enabled=1 in diesem Abschnitt sind ATLAS-Stiefel und ATLAS-Skelett an regulären Herstellungsstationen ohne Story- oder Vorlagen-Freischaltung herstellbar. GoldSkeletonSkin=1 ändert nur die Optik des ATLAS-Skeletts auf die native goldene Boost-St.3-Variante. Beide Optionen stehen standardmässig auf 0. Nach INI-Änderungen neu starten.

ATLAS-Stiefel vereinen Halt, Stossabsorption, stärkere Tritte und leise Schritte. Sie verwenden die Optik der Pizzabäcker-Stiefel, wiegen 0,2 kg und haben 3400 Basishaltbarkeit.
Das ATLAS-Skelett vereint Kampf-, Boost- und Bokka-Effekte auf Stufe 3, einschliesslich Bokka-Doppelsprung und gleichzeitiger Hüftfracht. Standardmässig verwendet es die Optik des normalen Boost-Skeletts St.3; mit GoldSkeletonSkin=1 wird die native goldene Boost-St.3-Optik verwendet. Es wiegt 4,0 kg und hat 20000 Basishaltbarkeit. Der Tragkraftbonus beträgt bis zu 180 kg mit Akku / 100 kg ohne Akku; die Boni werden nicht addiert. Akkuverbrauch und die nativen Sprung-/Eingabebedingungen gelten weiterhin.

FreeCrafting sowie die gemeinsamen Haltbarkeitsoptionen Multiplier und Unbreakable gelten auch für beide ATLAS-Items. Ohne diese Optionen gelten die ATLAS-Basiswerte und die Materialkosten der Transportstiefel bzw. des Kampfskeletts St.1. Die 132 bisherigen Item-Regeln steuern weiter die normale Ausrüstung. ATLAS wird über seinen eigenen Abschnitt unabhängig von DefaultUnlock angeboten.

Namen und Beschreibungen erscheinen bei deutscher UI auf Deutsch, sonst auf Englisch. Originalausrüstung behält ihre eigenen Werte und ihre Darstellung.

Umstieg und Spielstände
Die alte ds2_overpowered_equipment.asi entfernen. Nur ds2_crafting_unlocks.asi und die dazugehörige INI verwenden. Die ATLAS-Kennungen bleiben für vorhandene Exemplare erhalten.
AtlasEquipment.Enabled=0 blendet die beiden Rezepte aus; Ressourcen und Effekte bereits gespeicherter ATLAS-Items bleiben verfügbar. Dafür muss CraftingUnlocks.Enabled=1 bleiben. Der globale Hauptschalter, eine Deinstallation oder eine Rückkehr zu einer Version vor 1.6.0 schalten diese Unterstützung ab; für eine Deinstallation einen Spielstand vor der Herstellung von ATLAS verwenden.

Prüfstand
Der Nutzer hat ATLAS-Spielverhalten, Herstellung mit frühem Spielstand, Erhalt nach Neustart/Laden und das Anbringen von Hüftfracht bestätigt. Die gemeinsame Version 1.6.1 besteht die vollständige native Code-, Ressourcen-, Konfigurations-, Kosten- und Haltbarkeitstestreihe. Der optionale Gold-Skin wurde auf einer Installation ohne Deluxe-Version sowohl in der Herstellungsvorschau als auch am ausgerüsteten ATLAS-Skelett bestätigt und blieb nach Speichern/Laden korrekt. Einzelne Schuhwirkungen und Kampfschutz wurden noch nicht vollständig im Spiel nachgewiesen. Der Doppelsprung-Hinweis kann bei kurzen Sprüngen wie im akzeptierten Testbuild spät erscheinen.
