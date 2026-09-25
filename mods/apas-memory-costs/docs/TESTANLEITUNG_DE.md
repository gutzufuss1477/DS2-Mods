# APAS 3.0.0-rc.6: kurzer Spieltest

Dies ist eine vereinheitlichte Testversion, noch keine auf Nexus veroeffentlichte
stabile Version. Ich steuere das Spiel nicht weiter; der Spieltest erfolgt durch dich.
Wenn der Start abgelehnt wird, nennt die Logdatei jetzt die genaue Pruefadresse;
in diesem Fall keinen Funktions- oder Menutest bewerten.

1. Spiel vollstaendig beenden. ZIP entpacken und die ASI/INI neben `DS2.exe`
   kopieren. Alte oder umbenannte APAS-ASIs entfernen; genau eine APAS-ASI laden.
   Im zuvor gestarteten Prozess befand sich noch ein frueherer Zwischenbuild.
2. Zuerst die ausgelieferte INI verwenden: `Enabled=1`, `GlobalCost=1`,
   `UnlockAll=0`. Nach dem Start muss `ds2_apas_memory_costs.log` **READY** melden.
3. Vorhandenen Spielstand laden: normale freigeschaltete Upgrades sollen einen
   Punkt kosten und sich aktivieren/deaktivieren lassen. Grundknoten bleiben wie
   im Original. Gesperrte Upgrades sollen normal gesperrt bleiben.
4. Zum Titelbildschirm zurueck, erneut laden und dasselbe pruefen. Wenn moeglich
   auch einen anderen Spielstand laden und ein neu freigeschaltetes Upgrade testen.
5. Fuer `GlobalCost=0` und einen eigenen Wert (z.B. 25) jeweils komplett neu starten.
   Anzeige und tatsaechlich belegte Memory sollen zusammenpassen. Ein zu teures
   Upgrade darf weiterhin wegen fehlender Memory abgelehnt werden.
6. Einen neuen Spielstand nur mit Sicherung der bisherigen Saves testen: Beim
   erstmaligen Verfuegbarwerden von APAS muessen Grundknoten und Kauf/Aktivierung
   funktionieren. Das ist der wichtigste Test fuer die Nexus-Meldung.
7. `UnlockAll=1` ausschliesslich mit einem gesicherten/entbehrlichen Spielstand
   testen, jeweils nach Neustart. APAS-Knoten sollen zugaenglich werden, sobald das
   APAS-System verfuegbar ist. Sie werden nicht automatisch ausgeruestet. Auch die
   Kombination `Enabled=0`, `UnlockAll=1` mit Originalkosten testen.
8. FPS in derselben Szene mit gleichen Einstellungen mit/ohne APAS vergleichen.
   Der neue Code hat keinen wiederkehrenden Scan; die Messung bestaetigt die
   tatsaechliche Auswirkung im Spiel.

**Zum Rueckmelden reichen:** verwendete Einstellungen, ob Kostenanzeige/Kauf/
Neuladen funktionieren, ungefaehre FPS und die kleine APAS-Logdatei. Bei einem
Fehler bitte die genaue Spielphase bzw. das betroffene Upgrade nennen.

Bereits gespeicherte Freischaltungen bleiben moeglicherweise erhalten, auch wenn
`UnlockAll` wieder 0 ist. Die Rueckkehr zum alten Stand erfolgt ueber das Save-Backup.

Nexus wird erst nach diesem Spieltest auf einen einzigen Hauptdownload umgestellt.
