Crafting & Equipment Overhaul 1.10.1
DEATH STRANDING 2: ON THE BEACH | Windows x64 | Spielversion 1.10.89.0

NEU IN 1.10.1
Der rote Story-Surfsarg kann zusammen mit ATLAS und anderen Gegenständen hergestellt
werden. Das optionale N02-Skelett-Addon wird separat oder gemeinsam mit dem bisherigen
Schuhaddon unterstützt. Keine neuen INI-Einstellungen erforderlich.

WEITERHIN ENTHALTEN
Der rote Story-Surfsarg ist Bestandteil des Hauptmods: Herstellung, rotes Menübild,
3D-Vorschau, Fahren, Ziehen, Verstecken und gespeicherte Farbzuordnung.
Kein Schuhaddon erforderlich. Dazu kommen 52 native Outfits und Accessoires
samt Farbauswahl im Anpassungsmenü. Einzelheiten stehen in CHANGELOG_DE.txt.

UPDATE
DS2 vollständig beenden und ds2_crafting_unlocks.asi neben DS2.exe ersetzen.
Die vorhandene ds2_crafting_unlocks.ini und ds2_story_coffin_variants.bin behalten.
Die neue Kosmetikfreischaltung ist auch ohne zusätzlichen INI-Abschnitt aktiv.
Wer sie anpassen möchte, übernimmt einmalig den Abschnitt [Cosmetics] aus der
beigelegten INI. Keine doppelten Abschnitte oder Schlüssel anlegen.
Die beigelegte INI dient der Neuinstallation bzw. dem Zurücksetzen der Einstellungen.

NEUINSTALLATION
ds2_crafting_unlocks.asi und ds2_crafting_unlocks.ini neben DS2.exe kopieren.
Ein kompatibler 64-Bit-ASI-Loader ist separat erforderlich und nicht enthalten.
Alte separate ds2_overpowered_equipment.asi und ds2_more_silenced_guns.asi entfernen,
falls vorhanden; diese Funktionen sind bereits im Hauptmod enthalten.
Beim Update von Versionen vor 1.9.8 fehlende Abschnitte [WarriorWeapons], [RedGuitar]
und [Guitars] aus der INI übernehmen.

ROTER STORY-SURFSARG
Im Herstellungsmenü den Story-Eintrag wählen. Eine gemeinsame Auswahl mit ATLAS
und anderen Gegenständen ist möglich. Die Zuordnung erfolgt pro hergestelltem
Sarg; der normale Surfsarg bleibt erhalten. Der Story-Sarg verwendet dessen
native Funktionen und Kennung.

Beim ersten Start erzeugt der Hauptmod seinen eigenen Ressourcen-Cache im Ordner
ds2_crafting_story_cache neben DS2.exe. Dafür werden ungefähr 1,5 GB freier Platz
benötigt; der erste Start kann länger dauern. Mit optionalen Schuh-/Skelett-Addons werden
passende separate Caches verwendet. Nach einem Wechsel können mehrere Varianten
vorhanden sein; der gesamte Platzbedarf steigt entsprechend. Dieser Cache lässt sich bei geschlossenem Spiel löschen und wird
bei Bedarf neu erzeugt. Originalarchive werden ausschließlich gelesen.

Die Datei ds2_story_coffin_variants.bin neben DS2.exe speichert nur die Farbzuordnung
des Mods. Zusammen mit den Spielständen sichern und bei einem PC-Wechsel mitnehmen.
Sie gehört nicht zum Download und darf beim Update nicht überschrieben werden.
Ohne diese Zuordnung erscheinen gespeicherte Story-Särge wieder als normale Särge.
Der Mod verändert keine Originalarchive und schreibt nicht in die Save-Dateien.

OUTFITS UND ACCESSOIRES
Die 52 Einträge umfassen Outfits/Tarnungen, Brillen, Hüte/Bandanas, Kapuzen, Masken,
BB-Pod-Designs und Dollmans Outfit. Die vollständige Liste steht in cosmetics.native.tsv.
Native Kombinationsregeln und DLC-Berechtigungen bleiben bestehen.
[Cosmetics] Enabled=0 deaktiviert die zusätzliche Freischaltung;
UnlockColors=0 belässt die native Farbauswahl. Der globale Hauptschalter und
DefaultUnlock sowie individuelle [Items]-Regeln gelten weiterhin.

BESTEHENDE FUNKTIONEN UND STANDARDWERTE
Frühe Herstellung der bisherigen 135 Einträge, ATLAS-Ausrüstung, zusätzliche
schallgedämpfte Waffen, Krieger-Waffen, rote Higgs-Gitarre und PCC-Bauwerkswahl
bleiben enthalten. Kostenlose Herstellung und Haltbarkeitsänderungen sind in
der öffentlichen Standard-INI ausgeschaltet. Nach INI-Änderungen neu starten.

KOMPATIBILITÄT
ATLAS Chiral Boots Addon 1.0.0 bleibt ein separater optionaler Download. Seine ASI
muss für dieses Update nicht ersetzt werden; die Addon-Schnittstelle V1 bleibt erhalten.
ATLAS N02 Exoskeleton Addon 1.0.0 benötigt Hauptmod 1.10.1 oder eine spätere Version,
die Addon API V1 / Visual Router V1 und die ATLAS-Kennungen beibehält.
Beide Erscheinungsbild-Addons können unabhängig oder gemeinsam verwendet werden.
Normale Inhalts-, Rezept- und Werte-Updates benötigen keinen Neubau der Addons.
Coffin Board Reworked wurde in der Kombination mit Version 1.83.0 geprüft.
Andere Spielversionen oder Mods, die dieselben Ressourcen verändern, sind nicht abgedeckt.
Keine streaming_graph.core oder andere Originalarchive austauschen.

ENTFERNEN
DS2 beenden und die Mod-ASI entfernen. Die Story-Sarg-Erweiterung verwendet native
Sargkennungen und separate Farbmetadaten; ohne Mod entfällt die rote Darstellung.
Das ist keine pauschale Zusage für sämtliche alten Spezialgegenstände des Hauptmods:
gespeicherte eigene ATLAS-/Waffen-Varianten benötigen weiterhin dessen Definitionen.
Vor vollständiger Entfernung des Hauptmods einen separaten Spielstand sichern und
die selbst hinzugefügte Ausrüstung berücksichtigen. INI und Farbmetadaten können
für eine spätere Neuinstallation aufbewahrt werden.
