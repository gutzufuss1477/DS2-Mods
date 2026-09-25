# Crafting Overhaul - Unlocks, Free Crafting & Durability

Version 1.3.0 für DEATH STRANDING 2: ON THE BEACH.

Funktionen
- 120 einzeln konfigurierbare unterstützte Rezepte/Gegenstände.
- Frühe Freischaltung bei weiterhin funktionierender Vanilla-Progression.
- Rucksackmodule, Abdeckungen und Anhänger über das native Rucksackmenü.
- Omnireflektor-Stiefel herstellbar.
- Optionales FreeCrafting.
- Optionaler Haltbarkeitsfaktor von 1.0 bis 1000.0 und Unbreakable-Modus.
- Eigener Stiefel-Verschleisspfad für die 7 unterstützten herstellbaren Stiefel.
- Fracht, Auftragsfracht und Frachtcontainer bleiben Vanilla.

Schnellkonfiguration
[CraftingUnlocks]
Enabled=1
DefaultUnlock=1
FreeCrafting=0

[Durability]
Enabled=0
Multiplier=2.0
Unbreakable=0

CraftingUnlocks.Enabled=0 ist der globale Hauptschalter und installiert keine Patches.
Unbreakable=1 benötigt Durability.Enabled=1 und hat Vorrang vor Multiplier.

Kompatibilität
Unterstützte DS2.exe: 1.10.89.0
SHA-256: bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b
