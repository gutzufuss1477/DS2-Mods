"""Package the single-file addon; preserve the main 1.9.9 release byte-for-byte."""
from pathlib import Path
from datetime import datetime, timezone
import hashlib, json, shutil, struct, zipfile
import package_nexus_v199 as old

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'release/nexus-1.9.9-single-asi'
ASI = ROOT/'release/test16-single-asi/ds2_atlas_chiral_addon.asi'
PREVIOUS = ROOT/'release/nexus-1.9.9'

README_EN = '''ATLAS Chiral Boots Addon 1.0.0
Optional file for Crafting & Equipment Overhaul

Installation
With the game closed, copy ds2_atlas_chiral_addon.asi beside DS2.exe. Done.
No additional folder, BIN files or addon INI need to be installed.
Requires Crafting & Equipment Overhaul 1.9.9 (or a later version retaining
Addon API V1), its working ASI loader, and DS2 game build 1.10.89.0.
Keep the main mod and its existing INI installed.

First launch
All additional asset data is embedded in the ASI. The addon automatically creates
its atlas_chiral cache folder on first use. Allow at least 1.6 GB of free space;
the first loading sequence can take longer. Later launches verify and reuse the
cache. Original game archives are read only and never replaced. No Python,
Blender, separate installer or manual cache preparation is needed.

Appearance
Gold Chiral ATLAS boots, both worn and on the boot clip, an adapted trouser
transition and a matching rendered menu icon. ATLAS remains a separate craftable
item with all combined properties supplied by the main mod. Ordinary boots,
recipes and item identities are not changed by the addon.

Disable / remove
Close the game and remove ds2_atlas_chiral_addon.asi. Keep the main mod installed.
Existing ATLAS items then use the standard main-mod appearance again. No save
editing or conversion is performed. The automatically generated atlas_chiral
folder can also be removed to reclaim its disk space after closing the game.
The folder and any legacy BIN files alone cannot activate the addon.

Updates
Normal Crafting extensions retaining Addon API V1 can use the same addon.
Game patches and conflicting modifications to the same resources may need a
new adapter. Diagnostic log: ds2_atlas_chiral_addon.log.
'''
README_DE = '''ATLAS Chiral Boots Addon 1.0.0
Optionale Datei für Crafting & Equipment Overhaul

Installation
Bei geschlossenem Spiel ds2_atlas_chiral_addon.asi neben DS2.exe kopieren. Fertig.
Keinen zusätzlichen Ordner, keine BIN-Dateien und keine Addon-INI installieren.
Voraussetzung: Crafting & Equipment Overhaul 1.9.9 (oder später mit erhaltener
Addon-API V1), dessen funktionierender ASI-Loader und DS2-Spielversion 1.10.89.0.
Den Hauptmod und seine bisherige INI installiert lassen.

Erster Start
Alle Zusatzdaten stecken in der ASI. Das Addon legt seinen Cache-Ordner
atlas_chiral beim ersten Einsatz automatisch an. Mindestens 1,6 GB freien
Speicher vorsehen; der erste Ladevorgang kann länger dauern. Spätere Starts
prüfen den Cache und verwenden ihn wieder. Originalarchive werden nur gelesen,
nicht ersetzt. Kein Python, Blender, Installer oder manuelles Vorbereiten nötig.

Optik
Goldene Chiral-ATLAS-Stiefel, getragen und an der Stiefelklammer, mit angepasstem
Hosenübergang und passendem gerendertem Menübild. ATLAS bleibt ein eigenes
herstellbares Item mit allen kombinierten Eigenschaften des Hauptmods. Normale
Stiefel, Rezepte und Gegenstandskennungen bleiben durch das Addon unverändert.

Deaktivieren / Entfernen
Spiel schließen und ds2_atlas_chiral_addon.asi entfernen. Den Hauptmod behalten.
Vorhandene ATLAS-Items nutzen dann wieder die Standardoptik des Hauptmods.
Keine Spielstände werden bearbeitet oder konvertiert. Den automatisch erzeugten
Ordner atlas_chiral kann man nach Spielende ebenfalls löschen, um Speicher
freizugeben. Der Ordner und alte BIN-Dateien allein aktivieren das Addon nicht.

Updates
Normale Crafting-Erweiterungen mit erhaltener Addon-API V1 können dasselbe Addon
nutzen. Spielpatches und Mods an denselben Ressourcen können einen neuen Adapter
erfordern. Diagnoseprotokoll: ds2_atlas_chiral_addon.log.
'''
CHANGE_EN = '''ATLAS Chiral Boots Addon 1.0.0
- Gold Chiral models for ATLAS Boots, worn and on the boot clip, with a matching rendered menu icon.
- Single-file installation: copy only the ASI beside DS2.exe. All asset data is embedded; the local cache is created automatically.
- ATLAS properties and save identity remain with the main mod. Removing only the addon restores its standard appearance without replacing game archives.
'''
CHANGE_DE = '''ATLAS Chiral Boots Addon 1.0.0
- Goldene Chiral-Modelle für ATLAS-Stiefel, getragen und an der Stiefelklammer, mit passendem gerendertem Menübild.
- Installation mit nur einer Datei: ASI neben DS2.exe kopieren. Alle Zusatzdaten sind eingebettet; der lokale Cache entsteht automatisch.
- ATLAS-Eigenschaften und Save-Kennung bleiben beim Hauptmod. Entfernen des Addons stellt dessen Standardoptik wieder her, ohne Originalarchive zu ersetzen.
'''

def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def write(path, text): path.write_text(text, encoding='utf-8', newline='\n')

def main():
    assert not OUT.exists(), 'Do not overwrite a prepared release'
    source = ASI.read_bytes()
    inputs = json.loads((ROOT/'evidence/test16-embedded-inputs.json').read_text())
    tests = json.loads((ROOT/'evidence/test16-merge-tests.json').read_text())
    loader = json.loads((ROOT/'evidence/test16-loader.json').read_text())
    assert loader['addonSHA'] == sha(ASI) and loader['actualHostDescriptorAccepted']
    # Locate each exact payload in the FINAL ASI, not just in a test harness.
    pe = struct.unpack_from('<I', source, 0x3c)[0]
    assert source[pe:pe+4] == b'PE\0\0'
    count = struct.unpack_from('<H', source, pe+6)[0]
    sections = pe+24+struct.unpack_from('<H', source, pe+20)[0]
    embedded = {}
    for name, spec in inputs['payloads'].items():
        blob = (ROOT/'release/test15-api/atlas_chiral'/name).read_bytes()
        assert len(blob) == spec['bytes'] and hashlib.sha256(blob).hexdigest() == spec['sha256']
        pos = source.find(blob)
        assert pos >= 0 and source.find(blob, pos+1) == -1, name
        matches = []
        for i in range(count):
            off = sections+40*i
            rawsize, rawoffset = struct.unpack_from('<II', source, off+16)
            flags = struct.unpack_from('<I', source, off+36)[0]
            if rawoffset <= pos and pos+len(blob) <= rawoffset+rawsize:
                assert flags & 0x40000000 and not flags & 0xA0000000, 'assets must be read-only, non-executable'
                matches.append(source[off:off+8].rstrip(b'\0').decode('ascii'))
        assert len(matches) == 1
        embedded[name] = dict(spec, section=matches[0], fileOffset=pos)
    # The accepted game rendering and item behavior code is unchanged.
    with zipfile.ZipFile(ROOT/'evidence/test16-before-single-asi.zip') as z:
        changed = [n for n in z.namelist() if n.startswith('src/') and (ROOT/n).read_bytes().replace(b'\r\n',b'\n') != z.read(n).replace(b'\r\n',b'\n')]
    assert set(changed) == {'src/addon.cpp','src/portable_cache.inl','src/vendor/win_api.hpp','src/kernel32.def'}, changed
    old_manifest = json.loads((PREVIOUS/'RELEASE_MANIFEST.json').read_text())
    OUT.mkdir();stage = OUT/'addon';stage.mkdir()
    shutil.copy2(ASI, stage/ASI.name)
    for name, text in [('README_ATLAS_CHIRAL_EN.txt',README_EN),('README_ATLAS_CHIRAL_DE.txt',README_DE),
                       ('CHANGELOG_ATLAS_CHIRAL.txt',CHANGE_EN),('CHANGELOG_ATLAS_CHIRAL_DE.txt',CHANGE_DE)]:write(stage/name,text)
    # Reuse only the existing generic ZIP/hash validator, with this new output folder.
    old.OUT = OUT
    addon = old.package(stage, 'DS2_ATLAS_Chiral_Boots_Addon_v1.0.0_Single_ASI.zip', 'SHA256SUMS_ATLAS_CHIRAL.txt')
    with zipfile.ZipFile(addon['file']) as z:
        assert all('/' not in n and not n.endswith(('.bin','.ini')) for n in z.namelist())
        assert [n for n in z.namelist() if not n.endswith('.txt')] == [ASI.name]
        assert z.read(ASI.name) == source
    maininfo = old_manifest['packages'][0]
    mainzip = Path(maininfo['file']);shutil.copy2(mainzip,OUT/mainzip.name)
    assert sha(OUT/mainzip.name) == maininfo['sha256']
    for n in ['CHANGELOG_MAIN_EN.txt','CHANGELOG_MAIN_DE.txt']:shutil.copy2(PREVIOUS/n,OUT/n)
    write(OUT/'CHANGELOG_ADDON_EN.txt',CHANGE_EN);write(OUT/'CHANGELOG_ADDON_DE.txt',CHANGE_DE)
    notes = (PREVIOUS/'NEXUS_UPLOAD_NOTES.txt').read_text()
    notes = notes.replace('DS2_ATLAS_Chiral_Boots_Addon_v1.0.0.zip',Path(addon['file']).name)
    notes = notes.replace('Visuals only; ATLAS stats retained.', 'Install only the ASI beside DS2.exe; no extra data folder to copy.\nVisuals only; ATLAS stats retained.')
    write(OUT/'NEXUS_UPLOAD_NOTES.txt',notes)
    report = {'createdUtc':datetime.now(timezone.utc).isoformat(),'addonVersion':'1.0.0','internalBuild':'Test16',
      'addon':addon,'main':dict(maininfo,file=str(OUT/mainzip.name)), 'mainUnchanged':True,
      'addonAsiSha256':sha(ASI),'addonAsiBytes':len(source),'embeddedAssets':embedded,
      'runtimeInstallFiles':[ASI.name],'externalAssetFilesRequired':False,'automaticCacheFolder':'atlas_chiral',
      'cacheBytes':1537599967+31725738,'freshCacheWithoutBinFilesTested':True,'corruptedCacheRecoveryTested':True,
      'compiledIntegrationChecks':tests['checks'],'loaderEvidence':'evidence/test16-loader.json',
      'sourceChanges':changed,'assetsIdenticalToUserConfirmedTest15':True,
      'test16InGameConfirmed':False,'gameInstallationModified':False,'saveFilesAccessed':False,'nexusUploaded':False,
      'supersedesAddonPackage':old_manifest['packages'][1]['file']}
    write(OUT/'RELEASE_MANIFEST.json',json.dumps(report,indent=2)+'\n')
    write(ROOT/'evidence/test16-single-asi-package.json',json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__ == '__main__': main()
