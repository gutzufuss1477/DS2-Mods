"""Package the confirmed ABI build as main v1.9.9 + optional Chiral v1.0.0."""
from pathlib import Path
from datetime import datetime, timezone
import configparser, hashlib, json, shutil, zipfile, subprocess, sys

ROOT=Path(__file__).resolve().parents[1]
HOST=ROOT/'development/crafting-v198-addon-api'
OUT=ROOT/'release/nexus-1.9.9'
TEST=ROOT/'release/test15-api'
BASE=ROOT/'evidence/published-v198-baseline'

MAIN_EN='''Crafting & Equipment Overhaul 1.9.9
- Added a stable interface for optional ATLAS visual addons, starting with the Chiral Boots addon.
- Addon resources are merged with the current Crafting resources, preserving compatible additions in future updates.
- Retained all 1.9.8 recipes, ATLAS properties, item identities and default settings. No new INI options are required.
'''
MAIN_DE='''Crafting & Equipment Overhaul 1.9.9
- Feste Schnittstelle für optionale ATLAS-Optik-Addons ergänzt, beginnend mit dem Chiral-Stiefel-Addon.
- Addon-Ressourcen werden mit dem aktuellen Crafting-Stand zusammengeführt; kompatible spätere Erweiterungen bleiben erhalten.
- Rezepte, ATLAS-Eigenschaften, Gegenstandskennungen und Voreinstellungen aus 1.9.8 bleiben erhalten. Keine neuen INI-Optionen nötig.
'''
ADDON_EN='''ATLAS Chiral Boots Addon 1.0.0
- Initial release: gold Chiral models for ATLAS Boots, including equipped boots, the boot clip and a matching rendered menu icon.
- Visual changes only: ATLAS remains a separate craftable item with all properties provided by the main mod.
- Small portable payload; no original game archives are replaced. Removing the addon restores the main mod's standard ATLAS appearance while keeping the main mod installed.
'''
ADDON_DE='''ATLAS Chiral Boots Addon 1.0.0
- Erstveröffentlichung: goldene Chiral-Modelle für ATLAS-Stiefel, getragen und an der Stiefelklammer, mit passendem gerendertem Menübild.
- Reine Optikänderung: ATLAS bleibt ein eigenes herstellbares Item mit allen Eigenschaften des Hauptmods.
- Kleines portables Datenpaket ohne Austausch von Originalarchiven. Nach Entfernen des Addons erscheint wieder die normale ATLAS-Optik des weiterhin installierten Hauptmods.
'''

README_MAIN_EN='''Crafting & Equipment Overhaul 1.9.9
DEATH STRANDING 2: ON THE BEACH – Windows x64 – supported game build 1.10.89.0

This update adds support for optional visual addons. The Chiral Boots appearance
is a separate optional download; the main mod alone keeps its usual appearance.

Features retained from 1.9.8
- Early fabrication of 135 supported items, including special boots, backpack equipment and enemy weapons.
- ATLAS Boots and ATLAS Skeleton; four additional suppressed weapons.
- Three craftable Warrior weapons with configurable finite ammunition.
- Red Higgs Battle Guitar alongside Sam's original guitar; optional crystal-free shots.
- Optional Free Crafting, durability changes and early PCC building selections.

Updating from 1.9.8
Exit the game and replace ds2_crafting_unlocks.asi beside DS2.exe. Keep your existing
ds2_crafting_unlocks.ini: this update adds no settings. The included INI contains
the unchanged public defaults and is intended for new installations or resetting settings.

Fresh installation
Copy ds2_crafting_unlocks.asi and ds2_crafting_unlocks.ini beside DS2.exe.
A compatible x64 ASI loader is required separately and is not included.
When upgrading from releases before 1.9.8, merge missing [WarriorWeapons],
[RedGuitar] and [Guitars] sections from the supplied INI without duplicating keys.
Remove the old standalone ds2_overpowered_equipment.asi and
ds2_more_silenced_guns.asi if present; their features are integrated here.

Defaults
Free Crafting and durability modification remain disabled. Warrior magazines /
base reserve: rifle 30/150, shotgun 12/60, grenade launcher 6/30. Red guitar and
FreeChiralShots remain enabled. Settings can be adjusted in the INI; restart afterward.

Optional Chiral addon
Use ATLAS Chiral Boots Addon 1.0.0 with this main mod. Future Crafting releases
retaining Addon API V1 and the supported native adapter can use the same addon.
It does not require rebuilding for ordinary recipe or item additions. This does
not promise compatibility with arbitrary game patches or conflicting resource changes.

Resources and saved equipment
Do not replace streaming_graph.core or other original archives. The main mod
creates a temporary resource graph (about 28 MB) from the unmodified supported
game files. Previously modified original resource files can prevent this feature.
Item identities and stats are unchanged from 1.9.8. Removing only the optional
visual addon keeps ATLAS usable with the main mod's standard appearance.
Removing the main mod itself is different: saves containing its custom equipment
still require the corresponding item definitions. Keep the main mod installed.
'''
README_MAIN_DE='''Crafting & Equipment Overhaul 1.9.9
DEATH STRANDING 2: ON THE BEACH – Windows x64 – Spielversion 1.10.89.0

Diese Version ergänzt die Unterstützung optionaler Optik-Addons. Der Chiral-Look
ist ein separater optionaler Download; der Hauptmod allein behält seine bisherige Optik.

Funktionen aus 1.9.8 bleiben erhalten
- Frühe Herstellung von 135 Gegenständen, darunter besondere Stiefel, Rucksackausrüstung und Gegnerwaffen.
- ATLAS-Stiefel und ATLAS-Skelett sowie vier zusätzliche schallgedämpfte Waffen.
- Drei herstellbare Krieger-Waffen mit einstellbarer endlicher Munition.
- Rote Higgs-Kampfgitarre zusätzlich zu Sams Gitarre; optional ohne Kristallverbrauch.
- Optional kostenlose Herstellung, Haltbarkeitsänderungen und frühe PCC-Bauwerkswahl.

Update von 1.9.8
Spiel schließen und ds2_crafting_unlocks.asi neben DS2.exe ersetzen.
Die bestehende ds2_crafting_unlocks.ini behalten: es gibt keine neuen Einstellungen.
Die beigelegte INI enthält die unveränderten öffentlichen Standardwerte für eine
Neuinstallation oder das Zurücksetzen der Einstellungen.

Neuinstallation
ds2_crafting_unlocks.asi und ds2_crafting_unlocks.ini neben DS2.exe kopieren.
Ein kompatibler 64-Bit-ASI-Loader ist separat erforderlich und nicht enthalten.
Beim Update von Versionen vor 1.9.8 fehlende Abschnitte [WarriorWeapons], [RedGuitar]
und [Guitars] aus der INI übernehmen; keine doppelten Schlüssel anlegen.
Alte separate ds2_overpowered_equipment.asi und ds2_more_silenced_guns.asi entfernen,
falls vorhanden; ihre Funktionen sind bereits integriert.

Standardwerte
Kostenlose Herstellung und Haltbarkeitsänderungen bleiben ausgeschaltet.
Krieger-Magazine / Basisreserve: Sturmgewehr 30/150, Schrotflinte 12/60,
Granatwerfer 6/30. Rote Gitarre und FreeChiralShots bleiben eingeschaltet.
Einstellungen in der INI ändern und das Spiel anschließend neu starten.

Optionales Chiral-Addon
ATLAS Chiral Boots Addon 1.0.0 benötigt diesen Hauptmod. Künftige Crafting-Versionen
mit erhaltener Addon-API V1 und passendem Spieladapter können dieselbe Addon-Datei
nutzen. Normale Rezept- und Item-Erweiterungen benötigen keinen neuen Addon-Build.
Beliebige Spielpatches oder Änderungen an denselben Ressourcen sind nicht abgedeckt.

Ressourcen und Spielstände
Keine Originalarchive und keine streaming_graph.core ersetzen. Der Hauptmod
erstellt einen temporären Graphen (ca. 28 MB) aus den unveränderten unterstützten
Spieldateien. Bereits veränderte Originaldateien können diese Funktion verhindern.
Gegenstandskennungen und Werte bleiben gegenüber 1.9.8 unverändert. Nur das
Optik-Addon zu entfernen stellt die normale ATLAS-Optik des Hauptmods wieder her.
Den Hauptmod selbst weiter installiert lassen: eigene gespeicherte Ausrüstung
benötigt weiterhin dessen Gegenstandsdefinitionen.
'''
README_ADDON_EN='''ATLAS Chiral Boots Addon 1.0.0
Optional file for Crafting & Equipment Overhaul – same Nexus mod page

Requirements
- Crafting & Equipment Overhaul 1.9.9, or a later release retaining Addon API V1.
- DEATH STRANDING 2: ON THE BEACH 1.10.89.0, Windows x64, unmodified original resources.
- The main mod's working ASI loader. No second loader, Blender or Python is needed.
Original Crafting 1.9.8 does not contain the required interface.

Installation
1. Exit the game and install/update the main mod first.
2. Copy ds2_atlas_chiral_addon.asi and the complete atlas_chiral folder beside DS2.exe.
3. Keep the main mod's INI. Start the game normally.

The first launch builds an addon-owned cache in atlas_chiral. Allow at least
1.6 GB of free disk space; initial loading can take longer on slower drives.
The download contains only the small payloads, not the large cache. Subsequent
launches verify and reuse it. No original game archive is overwritten.

Appearance
ATLAS Boots use gold Chiral models when worn and on the boot clip, an adapted
trouser transition and a matching rendered menu icon. ATLAS remains its own
craftable item and keeps the main mod's combined boot properties. Ordinary
boots, recipes and statistics are not changed by this addon.

Disable / remove
Exit the game and remove ds2_atlas_chiral_addon.asi. The main mod must remain
installed. ATLAS then uses its standard main-mod appearance again, including
existing saved ATLAS items. The addon does not edit or convert save files.
The atlas_chiral folder may also be removed to reclaim the cache space.
Do not remove files while the game or its crash-report window is still running.

Updates
Compatible main-mod updates retain Addon API V1, so ordinary Crafting additions
do not need a new addon download. Game patches or mods altering the same boot,
Sam or texture resources can require an updated adapter. Diagnostic output is
written to ds2_atlas_chiral_addon.log. The internal build label is Test15;
version 1.0.0 distributes that exact game-tested addon binary and payload.
'''
README_ADDON_DE='''ATLAS Chiral Boots Addon 1.0.0
Optionale Datei für Crafting & Equipment Overhaul auf derselben Nexus-Seite

Voraussetzungen
- Crafting & Equipment Overhaul 1.9.9 oder eine spätere Version mit Addon-API V1.
- DEATH STRANDING 2: ON THE BEACH 1.10.89.0, Windows x64, unveränderte Originalressourcen.
- Der bereits funktionierende ASI-Loader des Hauptmods. Kein zweiter Loader, Blender oder Python nötig.
Die ursprüngliche Crafting-Version 1.9.8 besitzt diese Schnittstelle noch nicht.

Installation
1. Spiel schließen und zunächst den Hauptmod installieren bzw. aktualisieren.
2. ds2_atlas_chiral_addon.asi und den vollständigen Ordner atlas_chiral neben DS2.exe kopieren.
3. Die INI des Hauptmods behalten und das Spiel normal starten.

Beim ersten Start entsteht ein eigener Cache im Ordner atlas_chiral. Mindestens
1,6 GB freien Speicher vorsehen; auf langsamen Laufwerken kann der erste Ladevorgang
länger dauern. Im Download sind nur die kleinen Zusatzdaten enthalten. Spätere
Starts prüfen den Cache und verwenden ihn wieder. Keine Originalarchive werden überschrieben.

Optik
ATLAS-Stiefel erhalten goldene Chiral-Modelle, getragen und an der Stiefelklammer,
einen angepassten Hosenübergang und ein passendes gerendertes Menübild. ATLAS bleibt
ein eigenes herstellbares Item mit allen kombinierten Stiefeleigenschaften des
Hauptmods. Normale Stiefel, Rezepte und Werte werden vom Addon nicht verändert.

Deaktivieren / Entfernen
Spiel schließen und ds2_atlas_chiral_addon.asi entfernen. Der Hauptmod muss
installiert bleiben. ATLAS verwendet dann wieder seine Standardoptik aus dem
Hauptmod, auch bei bereits gespeicherten ATLAS-Items. Das Addon bearbeitet oder
konvertiert keine Spielstände. Der Ordner atlas_chiral kann zum Freigeben des
Cache-Speichers ebenfalls entfernt werden. Keine Dateien bei laufendem Spiel
oder geöffnetem Absturzbericht entfernen.

Updates
Kompatible Hauptmod-Updates behalten API V1 bei; normale Crafting-Erweiterungen
benötigen dadurch keinen neuen Addon-Download. Spielpatches oder Mods an denselben
Stiefel-, Sam- oder Texturressourcen können einen aktualisierten Adapter benötigen.
Diagnose: ds2_atlas_chiral_addon.log. Interne Build-Kennung: Test15. Version 1.0.0
enthält genau diese im Spiel geprüfte Addon-ASI und deren Zusatzdaten.
'''

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def write(p,text):p.write_text(text,encoding='utf-8',newline='\n')
def cfg(text):
 c=configparser.ConfigParser(inline_comment_prefixes=(';',));c.read_string(text);return {s:dict(c[s]) for s in c.sections()}
def package(folder,name,sums):
 paths=sorted(p for p in folder.rglob('*') if p.is_file())
 write(folder/sums,''.join(sha(p)+'  '+p.relative_to(folder).as_posix()+'\n' for p in paths))
 target=OUT/name
 with zipfile.ZipFile(target,'x',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  for p in sorted(folder.rglob('*')):
   if p.is_file():z.write(p,p.relative_to(folder).as_posix())
 with zipfile.ZipFile(target) as z:
  assert z.testzip() is None
  assert not any(n.endswith(('.cache','.log','.core','.stream','.py','.blend')) for n in z.namelist())
  for line in z.read(sums).decode().splitlines():
   digest,name=line.split('  ',1);assert hashlib.sha256(z.read(name)).hexdigest()==digest
 return {'file':str(target),'bytes':target.stat().st_size,'sha256':sha(target)}

def main():
 assert not OUT.exists(),'Release destination already exists; do not overwrite a release.'
 # Source proof: only the log version differs from the confirmed main-mod build.
 with zipfile.ZipFile(ROOT/'evidence/test15-confirmed-host-source.zip') as z:
  changes=[]
  for n in z.namelist():
   if n.endswith('/'):continue
   current=(HOST/n).read_bytes();before=z.read(n)
   if current.replace(b'\r\n',b'\n')!=before.replace(b'\r\n',b'\n'):changes.append(n)
  assert changes==['src/version.hpp'],changes
  assert (HOST/'src/version.hpp').read_text().strip()=='#pragma once\n#define CRAFT_VERSION "1.9.9"'
 mainasi=HOST/'build/nexus-v199/ds2_crafting_unlocks.asi'
 assert b'DS2 Crafting & Equipment Overhaul 1.9.9' in mainasi.read_bytes()
 assert b'1.9.8-addon-api1-test15' not in mainasi.read_bytes()
 install=json.loads((ROOT/'evidence/test15-install.json').read_text())
 assert sha(TEST/'ds2_atlas_chiral_addon.asi')==install['installedSha256']['ds2_atlas_chiral_addon.asi']
 OUT.mkdir();main=OUT/'main';addon=OUT/'addon';main.mkdir();addon.mkdir();(addon/'atlas_chiral').mkdir()
 shutil.copy2(mainasi,main/'ds2_crafting_unlocks.asi')
 with zipfile.ZipFile(BASE/'DS2_Crafting_Equipment_Overhaul_v1.9.8.zip') as z:
  ini=z.read('ds2_crafting_unlocks.ini').decode('utf-8-sig')
  old_changelog=z.read('CHANGELOG.txt').decode('utf-8-sig')
 newini=ini.replace('; DS2 Crafting & Equipment Overhaul 1.9.8','; DS2 Crafting & Equipment Overhaul 1.9.9',1)
 assert cfg(ini)==cfg(newini) and len(cfg(newini)['Items'])==135
 write(main/'ds2_crafting_unlocks.ini',newini)
 for n,t in [('README_EN.txt',README_MAIN_EN),('README_DE.txt',README_MAIN_DE),('CHANGELOG.txt',MAIN_EN),('CHANGELOG_DE.txt',MAIN_DE)]:write(main/n,t)
 shutil.copy2(TEST/'ds2_atlas_chiral_addon.asi',addon/'ds2_atlas_chiral_addon.asi')
 payloads={k:v for k,v in install['payloadSHA256'].items() if k.endswith('.bin')}
 assert len(payloads)==4
 for n,h in payloads.items():
  source=TEST/'atlas_chiral'/n;assert sha(source)==h;shutil.copy2(source,addon/'atlas_chiral'/n)
 for n,t in [('README_ATLAS_CHIRAL_EN.txt',README_ADDON_EN),('README_ATLAS_CHIRAL_DE.txt',README_ADDON_DE),('CHANGELOG_ATLAS_CHIRAL.txt',ADDON_EN),('CHANGELOG_ATLAS_CHIRAL_DE.txt',ADDON_DE)]:write(addon/n,t)
 subprocess.run([sys.executable,str(ROOT/'tests/test_api_loader.py'),'--host',str(main/'ds2_crafting_unlocks.asi'),
  '--addon',str(addon/'ds2_atlas_chiral_addon.asi'),'--report',str(ROOT/'evidence/nexus-v199-loader.json')],check=True)
 # The loader smoke log is evidence, not part of the package.
 smoke=addon/'ds2_atlas_chiral_addon.log'
 if smoke.exists():smoke.replace(ROOT/'evidence/nexus-v199-loader-smoke.log')
 results=[package(main,'DS2_Crafting_Equipment_Overhaul_v1.9.9.zip','SHA256SUMS.txt'),
          package(addon,'DS2_ATLAS_Chiral_Boots_Addon_v1.0.0.zip','SHA256SUMS_ATLAS_CHIRAL.txt')]
 write(OUT/'CHANGELOG_MAIN_EN.txt',MAIN_EN);write(OUT/'CHANGELOG_ADDON_EN.txt',ADDON_EN)
 write(OUT/'CHANGELOG_MAIN_DE.txt',MAIN_DE);write(OUT/'CHANGELOG_ADDON_DE.txt',ADDON_DE)
 write(OUT/'NEXUS_UPLOAD_NOTES.txt','''Nexus upload – existing Crafting & Equipment Overhaul page

Main Files
Name: Crafting & Equipment Overhaul 1.9.9
Version: 1.9.9
File: DS2_Crafting_Equipment_Overhaul_v1.9.9.zip
Description: Adds stable support for optional ATLAS visual addons. All 1.9.8
features and defaults retained. Updating from 1.9.8: replace the ASI and keep your INI.
Changelog: CHANGELOG_MAIN_EN.txt

Optional Files
Name: ATLAS Chiral Boots – Visual Addon
Version: 1.0.0
File: DS2_ATLAS_Chiral_Boots_Addon_v1.0.0.zip
Description: Gold Chiral ATLAS boots with boot-clip models and matching menu icon.
Visuals only; ATLAS stats retained. Requires main mod 1.9.9 / Addon API V1 and
DS2 1.10.89.0. First launch creates a local cache (allow at least 1.6 GB free).
Changelog: CHANGELOG_ADDON_EN.txt

No files have been uploaded automatically. Main files and optional addon are
separate downloads on the SAME Nexus page. Do not upload the staging folders,
source archives, logs, generated caches or the internal release manifest.
''')
 report={'createdUtc':datetime.now(timezone.utc).isoformat(),'packages':results,'nexusUploaded':False,
  'mainVersion':'1.9.9','addonVersion':'1.0.0','supportedGame':'1.10.89.0','addonApi':1,
  'mainSHA256':sha(main/'ds2_crafting_unlocks.asi'),'addonSHA256':sha(addon/'ds2_atlas_chiral_addon.asi'),
  'mainSourceChangesFromConfirmedTest15':changes,'addonBinaryAndPayloadByteIdenticalToConfirmedTest15':True,
  'defaultConfigurationEquivalentToPublic198':True,'customIniIncluded':False,'cachesIncluded':False,
  'userReportedTest15Working':True,'runtimeEvidence':'evidence/test15-user-confirmed-runtime.log',
  'test15RemovalRetestExplicitlyReported':False,'test14RemovalUserConfirmed':True,
  'gameInstallationChangedDuringPackaging':False,'saveFilesAccessed':False}
 write(OUT/'RELEASE_MANIFEST.json',json.dumps(report,indent=2)+'\n')
 write(ROOT/'evidence/nexus-v199-packaging.json',json.dumps(report,indent=2)+'\n')
 print(json.dumps(report,indent=2))
if __name__=='__main__':main()
