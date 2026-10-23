"""Preserve the optional shoe addon's public ABI without requiring its installation."""
from pathlib import Path
import argparse,hashlib,json,zipfile
R=Path(__file__).resolve().parents[1]
spec=json.loads((R/'config/addon-api-v1-baseline.json').read_text())
def sha(b):return hashlib.sha256(b).hexdigest()
for relative,expected in spec['addonApiV1SourceHashes'].items():
    assert sha((R/relative).read_bytes().replace(b'\r\n',b'\n'))==expected,relative+' changed: review API V1 compatibility'
host=(R/'src/addon_host.inl').read_text()
graph=(R/'src/virtual_graph_runtime.inl').read_text()
for name in ('worn','hanging','catalogue','icon','graphCommitted'):assert 'callbacks.'+name in host,name
for name in ('addon_host::merge(','addon_host::finish('):assert name in graph,name
assert 'DS2CraftingQueryAddonApi' in host+(R/'src/crafting_addon_api.h').read_text()
p=argparse.ArgumentParser();p.add_argument('--addon-zip',type=Path);args=p.parse_args()
if args.addon_zip:
    assert sha(args.addon_zip.read_bytes())==spec['publishedAddonZipSHA256']
    with zipfile.ZipFile(args.addon_zip) as z:
        assert sha(z.read('ds2_atlas_chiral_addon.asi'))==spec['publishedAddonASI_SHA256']
print('SHOE_API_V1_BUILD_GATE_PASS:',len(spec['addonApiV1SourceHashes']),'protected source files; optional addon not required.')
