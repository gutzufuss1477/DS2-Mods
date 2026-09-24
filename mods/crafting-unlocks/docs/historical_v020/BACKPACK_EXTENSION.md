# Rucksack-Erweiterung: statischer Befund und Implementierung 0.2.0-alpha

## Ergebnis

Ein zusätzlicher Aufrufstellen-Hook erweitert ausschliesslich die Rezeptliste der
nativen Rucksack-Anpassung. Die bestehende normale Herstellung bleibt mit ihrem
ursprünglichen Policy-Kern erhalten. Neu konfigurierbar sind 19 Module, zwei
Regenschutz-Stufen und acht Charms (insgesamt 29 neue, 119 gesamte INI-Einträge).
Die Daten stammen aus dem bereits gelieferten Master-Export und dem Laufzeitkatalog.
Kein zusätzlicher Ghidra-Export wurde benötigt.

## Belastbare native Kette

| Rolle | RVA | Befund |
|---|---|---|
| Aufbau der fünf Rucksack-Unterlisten | 0x01518CC0 | ruft 01529860 für die Untertypen auf |
| Rucksack-Katalog/Unterlisten | 0x01529860 | ruft 00B6FC20 mit Usage=5 und MenuInfo+0x228 auf |
| Neuer enger Hook | 0x01529896 | direkter CALL auf 00B6FC20; RCX=Manager, DL=5, R8=Ausgabevektor |
| Originaler Listenproduzent | 0x00B6FC20 | native Fact-/DLC-Prüfung, Ausgabe eigener Instanzkopien |
| Native Vektorkopie | 0x00B7E950 | Zielvektor in RCX, Quellinstanzen in RDX, Anzahl in R8D |
| Auswahl/Verteilung | 0x01526F50 | Instanzindex -> Rezeptschlüssel -> native Modul-/Charm-/Cover-Typen |
| Materialbedarf/Verfügbarkeit | 0x01529430 / 0x015294A0 | liest originale Rezeptkosten; Charm-Belegung separat |
| Modul-Vorbereitung | 0x00B41B10 | legt native Ausrüstungsdaten anhand des Optionstyps an |
| Charm anwenden | 0x0151A5C0 | schreibt nativen Slot nur bei Index <2, Manager+0x4498/+0x4499 |
| Cover anwenden | 0x00B43390 | native Cover-Konfiguration |

Belege: gleichnamige `evidence/backpack/*_decompile.txt` und `*_asm.txt`.
Die Callback-Thunk-Labels um 01582100 sind im Master nicht als Funktionskörper
enthalten. Sie werden weder gepatcht noch als vollständig rekonstruierte
Callback-Transaktion ausgegeben. Die oben analysierten Konsumenten, Typzuordnungen
und Original-Materialabfragen begründen den eng gefassten Listen-Hook; die
vollständige Anwendung und ihr Speichern müssen im Spiel bestätigt werden.

Der Master enthält an der neuen Aufrufstelle die native Instruktion, aber keine
rohen Bytes. `E8 85 63 64 FF` wurde aus der überprüften CALL-rel32-Adresse und dem
Ziel berechnet. Diese fünf Bytes werden vor dem Patch im Prozess geprüft. Das ist
keine behauptete rohe Byteaufnahme. Die Datei-SHA-256 bleibt zusätzlich Pflicht.

## Besitzer und Lebensdauer

Die Rucksackliste ist ein dynamischer Vektor mit 16-Byte-Instanzen. Anders als der
normale Herstellungs-Hook darf hier kein VirtualAlloc-Zeiger in einen später vom
Spiel freigegebenen Vektor eingesetzt werden. Deshalb bleibt der Besitzer nativ:

1. Der ursprüngliche Produzent wird exakt einmal ausgeführt.
2. Seine gesamte Liste wird als unveränderter Präfix übernommen.
3. Aus dem geladenen Katalog werden nur freigegebene zusätzliche Module/Covers/
   Charms ausgewählt; Originalkatalog und Fact-Zustand werden nicht beschrieben.
4. Die Arbeitskopien werden mit der bereits im Originalproduzenten benutzten
   Routine 00B7E950 in den menüeigenen Vektor kopiert.
5. Reallocation, Instanzkonstruktion und spätere Freigabe bleiben im Engine-Allocator.

Die Kopie erfolgt auf dem aufrufenden nativen Menüthread, ausserhalb des
Katalog-Locks. Der vorhandene Busy-Schutz wird zwischen beiden Hooks geteilt.
Originaleinträge werden auch bei INI-Wert 0 erhalten; nur zusätzliche Kopien
bekommen das Bit 0x8000. Bei ungültigen Daten, Konflikten oder Überlauf wird die
native Liste beibehalten. Das Limit von 512 gilt als konservative Modgrenze,
nicht als neu behauptetes natives Rucksacklimit.

## Identitäten und Grenzen

Die 29 Rezeptschlüssel stammen aus `ds2_crafting_catalogue.tsv`; Usage ist jeweils
BackPackCustomize. Die statischen Typzuordnungen im Menü sind:

- Optionstabelle 0x02DD25B0, 95 Datensätze, Stride 8; Typen 20 bis 94 sind Flicken.
- Charmtabelle 0x02DD37D0, neun Datensätze einschliesslich Nulloption.
- Covertabelle 0x02DD3780, zwei Datensätze.

Die Charm- und Covertabellen konnten aus den überlappenden GLOBAL_CONTEXT-Auszügen
vollständig zusammengesetzt werden. Die mittleren Modul-/Flicken-Tabellenbytes
fehlen dort. `PARTIAL_NATIVE_TABLES.tsv` markiert diese Lücken ausdrücklich.
Vor Hook-Installation liest der Mod alle Tabellen direkt und überprüft sämtliche
29 Identitäten und ihre passende Kategorie. Das Typfeld wird wie in der nativen
Instruktion als Byte gelesen; die drei Padding-Bytes werden nicht ausgewertet.
Es werden keine Typ-IDs geraten.

75 Katalogeinträge dieser Gruppe sind Flicken. Sie werden nicht vorzeitig ergänzt,
bleiben jedoch als native Einträge erhalten. Fahrzeug- und Kleidungsmenüs bleiben
unverändert. Es wird weder die Rucksack-Menüfreischaltung noch die native Zahl der
Charm-Slots oder das Backpack-Layout verändert. Bekannte Platzierungs-/Visibility-
Hooks des separaten Backpack-Mods werden hier nicht gepatcht; eine kombinierte
Ingame-Kompatibilität ist damit noch nicht bewiesen.

## Fortschritt und Speicherstand

INI 0 bedeutet Vanilla: ein nativ freigeschalteter Eintrag wird immer behalten.
Weder globale Story-Facts noch originale Rezept-Freischaltbits werden vom neuen
Hook gesetzt oder gelöscht. Das normale Ausrüsten und Herstellen darf weiterhin
Ausrüstung, Materialien und native Kauf-/Herstellungsflags speichern. Das Verhalten
sämtlicher späterer Story-Belohnungen ist nicht durch einen vollständigen
Spieldurchlauf verifiziert. Kein pauschales Versprechen eines unveränderten Saves.

## Validierung

- Originaler core.hpp unverändert; bisherige 90 INI-Schlüssel unverändert.
- Produktionsparser liest die ausgelieferte 119-Zeilen-Auswahl mit Inline-Kommentaren.
- Tests für 0-Ausnahmen und später nativ gelieferte Einträge auf beiden Pfaden.
- Getrennte Kategorien; keine neuen Flicken-/Fahrzeugeinträge über Backpack-Policy.
- Master-/native Eingaben bytegenau unverändert; private Kopien erhalten 0x8000.
- Konservative Fehler-/Overflowbehandlung, bekannte DLC-/Quest-/Duplikatfilter.
- Windows-x64-Cross-Build, PE-Header/Import-/Unwind-/Relokationskontrolle.
- Hosttests und ASan/UBSan bestanden; konkrete Ergebnisse in den aktuellen Docs.

**Nicht durchgeführt:** Windows-Ladetest dieser ASI, tatsächlicher Aufruf der
Engine-Kopierroutine, Ausrüstung/Materialabzug und Save/Reopen im Spiel. Der
Gitarren-Erfolg des Nutzers gehört zur vorherigen normalen Herstellung.

## Einmaliger nächster Spieltest

ASI und INI ersetzen; ein bisher gesperrtes Modul und einen Charm über die normale
Rucksack-Anpassung ausrüsten und übernehmen. Menü verlassen und wieder öffnen.
Prüfen, ob beide ausgerüstet bleiben und bei kostenpflichtiger Herstellung die
Materialien stimmen. Log plus Beobachtung zurückgeben. Kein Prolog-Neustart und
kein weiterer vollständiger Export für diesen Schritt nötig.

## Öffentliche ABI-/API-Referenzen (nicht Quellen der Spiellogik)

Die Spiellogik oben stammt ausschliesslich aus den bereitgestellten Spiel-Exports.
Die Implementierung berücksichtigt die offizielle Microsoft-x64-Registerkonvention
und Cache-Kohärenz bei geänderten Codebereichen:

- https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions
- https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention
- https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect
- https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-flushinstructioncache
