# DS2 Konstruktionen ohne manuelle Upgrades v1.0.0

Stabiler, versionsgebundener ASI-Mod für **DEATH STRANDING 2: ON THE BEACH**,
Steam-PC-Version `1.10.89.0`.

Eine neue eigene PCC-Konstruktion wird weiterhin sichtbar durch den normalen
Spielablauf erzeugt. Anschließend schenkt der Mod die fehlenden Stufen über die
originalen, klassenspezifischen Level-Funktionen des Spiels. Dafür werden keine
Upgrade-Materialien und keine manuellen Upgrade-Aktionen benötigt.

## Im Spiel bestätigtes Verhalten

- Normale Konstruktionen, darunter Transponder/Schnellreisestation und
  Frachtkatapult, werden regulär gebaut und danach mit einem nativen Vorgang
  direkt von Stufe 1 auf Max-Stufe 3 aktualisiert.
- Schutzhütte/Bunker und normale Brücken werden sichtbar gebaut und führen
  anschließend exakt zwei native Upgrades aus:
  `1 -> Fertigstellung (2) -> Max (4)`.

Die zwei Einblendungen bei Schutzhütte und normalen Brücken sind beabsichtigt.
Der klassenspezifische Fertigstellungsschritt erzeugt und bestätigt zuerst den
benötigten Fundamentzustand. Sichtbarkeit, Levelbestätigung und Bauereignisse
verwenden denselben nativen Ablauf; dieser notwendige Zwischenschritt wird
daher nicht unterdrückt.

## Unterstützte Konstruktionen

Die stabile Version erkennt dreizehn kanonische Spieler-Konfigurationen:

- Briefkasten, Schutzhütte, Wachturm, Frachtkatapult und Generator
- Regenschutz, Seilrutsche, Chirale Brücke und Sprungschanze
- normale Brücken mit 30 m, 45 m und 80 m
- Transponder/Schnellreisestation

Fremde Online-Konstruktionen (`Net`) und Story-/Weltobjekte (`Stage`) werden
nicht verändert. Straßen-Rebuilder und Minen sind in den ausgelieferten Daten
Stage-Infrastruktur. Monorail besitzt intern keine getrennte Upgrade-Stufe.

## Sicherheitsmodell

Der Mod verändert weder den Bau-Descriptor noch direkt Current-/Applied-Level,
Materialtabellen, Archive oder Speicherdateien. Vor jedem geschenkten Schritt
müssen die exakte Player-Konfiguration und Objektklasse, ein zugelassener
nativer Update-Caller, der passende Lebenszyklus, übereinstimmende
Current-/Applied-Level, leere Callback-Warteschlangen sowie freie belegte
Pending-/Dirty-Flags vorliegen.

Schutzhütte und Brücken verwenden ihren nativen, zeitqualifizierten
Fundamentzustand und fordern zuerst exakt Fertigstellungsstufe 2 an. Erst nach
der nativen Bestätigung und dem Wechsel zu `Active` folgt Max-Stufe 4. Ein
echter Abbau auf Stufe 0 bleibt immer nativ. Rückstufungen aus der exakten
Materialberechnung werden nur in den belegten, bereits geschenkten Fällen
unterdrückt.

Executable-Identität, relevante Codebereiche, VTables, Konfigurationspointer,
Klassenmetadaten und Level-Tupel werden fail-closed geprüft. Passt der
installierte Spiel-Build nicht exakt, werden keine Hooks installiert.

## Installation

1. Spiel vollständig beenden.
2. Für den ersten Lauf den verwendeten Spielstand sichern.
3. `ds2_construction_max_level_on_build.asi` und
   `ds2_construction_max_level_on_build.ini` neben `DS2.exe` kopieren.
4. Das Spiel mit einem externen 64-Bit-ASI-Loader starten.

Den ASI niemals bei laufendem Spiel austauschen oder entladen. Zum Entfernen
das Spiel beenden, beide Moddateien löschen und neu starten. Bereits vom Spiel
gespeicherte Level werden dadurch nicht rückwirkend geändert.

TEST v0.1.1 bleibt zurückgezogen. Ein mit dieser alten Version bereits
unsichtbar oder festhängend gespeicherter Bauvorgang lässt sich durch reinen
Dateiaustausch nicht zuverlässig reparieren; dafür einen Spielstand von vor
dem betroffenen Bau laden.

## Log und Diagnose

Mit dem Produktionsstandard `DebugLog=0` enthält das Log kompakte
Lebenszyklus-, Abschluss- und Fehlermeldungen. Ausführliche
Fundamentdiagnostik ist optional: Vor dem Spielstart in der INI `DebugLog=1`
setzen, damit begrenzte `DIAG`-Zustände beim Eintritt in den Update-Hook
(`point=0`) und nach dem nativen Update (`point=1`) ausgegeben werden. Die
Zustände enthalten unter anderem gespeicherte/live Construction-ID, VTable-RVA,
Markerflags, Caller, Lebenszyklus, Timer und Pending-Werte. Blocker-Bit 11
kennzeichnet eine fehlgeschlagene Runtime-Metadatenvalidierung. Im Update-Hook
selbst findet kein Datei-I/O statt.

## Verlauf der TEST-Versionen

- **v0.1.1** verwendete eine synchrone Ganzzahl-Levelkette und wurde
  zurückgezogen, weil sie ein unsichtbares Objekt und einen festhängenden
  Bauvorgang erzeugen konnte.
- **v0.1.2** serialisierte die einzelnen nativen Levelaufrufe. Damit
  überlappten die Bauvorgänge nicht mehr, aber jede Zwischenstufe wurde erneut
  eingeblendet.
- **v0.1.3** führte direkte native Meilensteinaufrufe ein: normale Bauten
  `1 -> Max`, Fundamente `1 -> Fertigstellung -> Max`.
- **v0.1.4** übernahm die native Fundament-Timerregel bei noch gesetztem
  Build-Pending. Ein nativer Refresh `SetLevel(1)` konnte den Marker jedoch
  weiterhin löschen.
- **v0.1.5** korrigierte die Marker-Lebensdauer für Refreshes ohne
  Levelsenkung und die exakte Materialnormalisierung im Lifecycle 2. Zusätzlich
  kamen Eintritts-/Post-Update-Diagnosen hinzu.
- **v1.0.0** macht diese Zustandsmaschine zur stabilen Version, nachdem im
  Spiel normale Konstruktionen sowie sichtbarer Bau und exakt zwei native
  Upgrades von Schutzhütte/Bunker und Brücke bis Max bestätigt wurden.

Die exakte Zielversion, Zustandsmaschine, Regressionstests und finale
Release-Prüfung stehen in `docs/TECHNICAL_NOTES.md` und `docs/VALIDATION.md`.
