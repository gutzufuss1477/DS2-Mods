CRAFTING UNLOCKS - EQUIPMENT, BACKPACK AND CHARMS
Version 1.0.0

UMFANG
119 einzeln konfigurierbare Einträge: 90 für die normale Herstellung,
19 Rucksackmodule, zwei Regenschutz-Stufen und acht Charms. Alle sind
standardmässig aktiv. Die INI verwendet englische Gegenstandsbezeichnungen.

Die normale Herstellung wird ab einem geeigneten Herstellungsmenü erweitert.
Rucksackausrüstung erscheint in der nativen Rucksack-Anpassung, sobald dieses
Menü zugänglich ist. Das Menü selbst wird nicht früher freigeschaltet.
Materialkosten, Montagefläche, Ausrüstungslimits und Charm-Plätze bleiben nativ.
Fahrzeuge, Fahrzeug- und Kleidungsanpassungen, Rucksack-Flicken, reine Questobjekte
und besonders geschützte DLC-Rezepte werden nicht neu freigegeben.

INSTALLATION / UPDATE
Spiel vollständig schliessen und den verwendeten Spielstand sichern.
Einen bereits funktionierenden 64-Bit-ASI-Loader behalten; er ist nicht enthalten.
ds2_crafting_unlocks.asi und ds2_crafting_unlocks.ini direkt neben DS2.exe kopieren.
Keine umbenannten älteren Crafting-ASIs in aktiven Loaderordnern liegen lassen.

Die 119 Einstellungen aus 0.2.0-alpha bleiben gültig. Eine angepasste INI dieser
Version darf bleiben; alternativ die neue INI übernehmen und persönliche
0-Ausnahmen übertragen. Das Paket ist ein separater Mod; der Mod Manager wird
weder ersetzt noch um einen neuen Katalogeintrag erweitert.

EINSTELLUNGEN
1 = diesen Gegenstand vorzeitig verfügbar machen.
0 = normale Freischaltung durch den Spielverlauf abwarten, niemals verbieten.
Die vorhandene Zeile ändern, keinen zweiten Eintrag mit demselben Schlüssel
anlegen. Anschliessend das Spiel neu starten.

Bereits regulär freigeschaltete Gegenstände bleiben auch mit 0 verfügbar.
DefaultUnlock gilt nur für nicht gelistete bzw. auf inherit gesetzte Einträge.
DefaultUnlock=0 setzt explizite 1-Einträge nicht ausser Kraft.
Enabled=0 deaktiviert den gesamten Mod. ExportCatalogue=0 bleibt normalerweise so.
Nur ds2_crafting_unlocks.ini bearbeiten, nicht die optionalen Diagnose-Dateien.

SPIELFORTSCHRITT / SPIELSTAND
Der Mod setzt oder löscht keine originalen Rezept-Freischaltbits und keine
globalen Story-Facts. Er ergänzt Kopien in den Menülisten. Die normale
Freischaltung bleibt erhalten. Hergestellte Gegenstände, Materialverbrauch,
angewandte Ausrüstung und native Kauf-/Herstellungsflags können gespeichert
werden. Entfernen des Mods macht diese Spielhandlungen nicht rückgängig.
Ein vollständiger Test aller späteren Belohnungen und Meldungen liegt nicht vor.

KOMPATIBILITÄT
Analyseziel DS2.exe 1.10.89.0; zusätzlich ist dieser SHA-256 zwingend:
bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b
Andere EXE-Versionen/Storefronts sind nicht pauschal freigegeben.
Versions- und Signaturprüfungen nicht umgehen. Die Kombination mit sämtlichen
anderen Mods, insbesondere weiteren Rucksack-Mods, ist nicht vollständig geprüft.

PRÜFSTAND
Bestätigt wurden die frühe Liste am ersten Terminal in Episode 2, die tatsächlich
hergestellte Kampfgitarre im Inventar sowie die Freigabe der Rucksackausrüstung,
des Regenschutzes und der Charms mit 0.2.0-alpha. Für 1.0.0 wurde im Laufzeitcode
nur die Versionsmeldung geändert; die ASI wurde neu gebaut. Offline-Tests und
Strukturprüfungen wurden wiederholt. Kein unabhängiger Ingame-Lauf dieser neu
gebauten ASI wurde in der Buildumgebung ausgeführt. Nicht jedes Item, jeder
Materialabzug und jede Speicher-/Fortschrittsfolge wurde damit einzeln getestet.

ENTFERNEN
Spiel schliessen und nur die Crafting-Unlocks-ASI sowie ihre INI entfernen.
Gespeicherte Gegenstände und verbrauchte Materialien bleiben bestehen.
Einen gemeinsamen Loader für andere Mods nicht entfernen.
