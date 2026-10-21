"""Independent Odradek readback and simulation of native ArtParts slot binding."""
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
from export_native_resources import export
from prepare_resource_audit import ROOT, JAVA, MODULES
from serialize_native_geometry import ref, metadata as native_metadata
from fix_controller_offsets import COMPONENTS, rows
from fix_array_reservations import validate_cache_reservations


def validate_controller_gate(stage):
    report = json.loads((stage/'report.json').read_text())
    manifest = json.loads((stage/'bundle/overlay.json').read_text())
    proof = json.loads((ROOT/report['controllerValidation']).read_text())
    assert proof['cache'] == report['cache']
    assert proof['resultHashes'] == {f['gamePath']:f['resultSha256'] for f in manifest['files']}
    assert proof['controllerCount'] == 166 and proof['firstUnboundMatchesDumpIndexAndHash']
    assert proof['unboundControllers'] == proof['overlappingControllers'] == []
    assert proof.get('controllerAllocationCensusUnchanged',proof['allocationCensusUnchanged'])
    assert proof['boneMappingsUnchanged']


def main():
    stage = ROOT/'staging/test09-controller-offsets'
    report = json.loads((stage/'report.json').read_text())
    cache = Path(report['cache'])
    out = ROOT/'evidence/test09-resource-readback'
    assert not out.exists()
    out.mkdir()
    override = dict(DS2_KESTREL_GRAPH_OVERRIDE=str(cache/'streaming_graph.core'),
                    DS2_ATLAS_LINKTABLE_OVERRIDE=str(cache/'streaming_links.stream'),
                    DS2_KESTREL_EXT_STREAM_OVERRIDE=str(cache/'package.42.02.core.stream'))
    # Record every serialized object and every array in all three modified groups.
    env = {k:v for k,v in os.environ.items() if not k.startswith('DS2_')}
    env.update(override, DS2_LAB_AUDIT_DIR=str(out/'raw'), DS2_LAB_AUDIT_GROUPS='67696,31126,39987')
    refs = ['67696:0', '39987:0', '39987:1455', '31126:4064', '31126:9366',
            '31126:3575', '31126:20086']+['39987:'+str(i) for i in COMPONENTS]
    command = [str(JAVA/'java.exe'), '--enable-preview', '--enable-native-access=odradek.core',
               '--add-opens=java.base/sun.nio.ch=odradek.core',
               '--patch-module', 'odradek.game.ds2='+str(ROOT/'build/array-audit/classes'),
               '--module-path', str(MODULES), '-m', 'odradek.app/sh.adelessfox.odradek.app.Main',
               'export', '-s', r'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH',
               '-o', str(out), '-f', 'json', '67696:0', '31126:0', '39987:0']
    with (out/'audit.log').open('xb') as log:
        result = subprocess.run(command, env=env, stdout=log, stderr=subprocess.STDOUT)
    assert result.returncode == 0, (out/'audit.log').read_text()[-3000:]
    def read(key):
        g, i = key
        matches = list(out.glob(f'*_{g}_{i}.json'))
        assert len(matches) == 1, key
        return json.loads(matches[0].read_text())
    def fetch(keys):
        keys = sorted({k for k in keys if not list(out.glob(f'*_{k[0]}_{k[1]}.json'))})
        if keys:
            export([f'{g}:{i}' for g, i in keys], out, overrides=override)
    fetch([tuple(map(int,r.split(':'))) for r in refs])
    art = read((39987, 0))
    settings = art['CoverModelResources']+[art[k] for k in
                ['FacialAModelResource','HairAModelResource','ClothAModelResource','ClothBModelResource','ClothCModelResource']]
    entities = art['CoverModelEntityResources']+[art['FacialRigModelEntityResource']]+art['CoverAndAnimModelEntityResources']
    fetch(list(map(ref, settings+entities)))
    models = [ref(art['MainModelResource'])]+[ref(read(ref(s))['ModelResource']) for s in settings]
    fetch(models)
    all_models = models+[(31126,9366),(31126,20086)]
    fetch([ref(p) for key in all_models for p in read(key)['ExpandedModelPartResources']])
    blocks = []
    flat_names = []
    for j, key in enumerate(models):
        model = read(key)
        parts = model['ExpandedModelPartResources']
        flat_names += [read(ref(p))['NameHash'] for p in parts]
        if j:
            component_id = COMPONENTS[j-1]
            component = read((39987,component_id))
            assert component['CoverModelResource'] == settings[j-1]
            assert f'<ref to 39987:{component_id}>' in read(ref(entities[j-1]))['EntityComponentResources']
            offset = component['ControllerOffset']
        else:
            component_id, offset = None, 0
        blocks.append(dict(model=list(key), component=component_id, offset=offset,
                           count=len(parts), expectedOffset=len(flat_names)-len(parts)))
    assert flat_names == art['ModelPartNames']
    assert len(flat_names) == art['ExpandedModelPartResourceNum'] == 166
    assert len(entities) == art['NeedCreateChildEntitiesNum'] == 7
    def occupancy(blocks):
        owners = [None]*len(flat_names)
        overlaps = []
        for b in blocks:
            for i in range(b['offset'],b['offset']+b['count']):
                assert 0 <= i < len(owners)
                if owners[i] is not None: overlaps.append(i)
                owners[i] = b['model']
        return [i for i,x in enumerate(owners) if x is None], overlaps
    assert all(b['offset']==b['expectedOffset'] for b in blocks)
    assert occupancy(blocks) == ([], [])
    bad_blocks = [dict(b,offset=b['offset']-(2 if b['component'] else 0)) for b in blocks]
    missing, overlap = occupancy(bad_blocks)
    assert missing == [164,165] and overlap == [53,54]
    assert art['ModelPartNames'][164] == 0x3cc45148
    dump = json.loads((ROOT/'evidence/test08-minidump-triage.json').read_text())
    regs = {k:int(v,16) for k,v in dump['registers'].items()}
    assert (regs['rsi']-regs['r12'])//4 == 164
    assert (regs['r13']-regs['r12'])//4 == 166
    assert regs['rbx'] == 0 and regs['rcx'] == 0xa0 and regs['r8'] == 0x3cc45148
    # Original parent edges are retained under the insertion's index mapping.
    original_art = native_metadata(39987,0)
    new_index = {n:i for i,n in enumerate(flat_names)}
    assert len(new_index) == 166
    for j,name in enumerate(original_art['ModelPartNames']):
        p = original_art['ParentControllerIndex'][j]
        assert art['ParentControllerIndex'][new_index[name]] == (
            -1 if p < 0 else new_index[original_art['ModelPartNames'][p]])
    for aid, mid in [(4064,9366),(3575,20086)]:
        a,m = read((31126,aid)), read((31126,mid))
        assert a['ModelPartNames'] == [read(ref(p))['NameHash'] for p in m['ExpandedModelPartResources']]
        assert len(a['ModelPartNames']) == a['ExpandedModelPartResourceNum']
        assert a['NeedCreateChildEntitiesNum'] == 0
    changes = {}
    for gid in [31126,39987,67696]:
        previous = ROOT/f'evidence/test07-arrays-test07/{gid}.bin'
        actual = out/f'raw/{gid}.bin'
        a,b = previous.read_bytes(),actual.read_bytes()
        assert len(a)==len(b)
        changed = [i for i,(x,y) in enumerate(zip(a,b)) if x!=y]
        assert changed == (sorted(p['groupOffset']+p['fieldOffset'] for p in report['patches']) if gid==39987 else [])
        for suffix in ['.tsv','.arrays.tsv']:
            assert (out/f'raw/{gid}{suffix}').read_bytes() == (ROOT/f'evidence/test07-arrays-test07/{gid}{suffix}').read_bytes()
        changes[str(gid)] = dict(bytesChanged=changed, sha256=hashlib.sha256(b).hexdigest(),
                                dynamicArrayCensusByteIdentical=True, objectLayoutByteIdentical=True)
    for i in COMPONENTS:
        a = json.loads(next((ROOT/'evidence/test08-cover-metadata').glob(f'*_39987_{i}.json')).read_text())
        b = read((39987,i))
        assert b['ControllerOffset'] == a['ControllerOffset']+2
        b['ControllerOffset'] -= 2
        assert b == a
    validate_cache_reservations(stage)
    manifest = json.loads((stage/'bundle/overlay.json').read_text())
    evidence = dict(cache=str(cache), resultHashes={f['gamePath']:f['resultSha256'] for f in manifest['files']},
                    nativeSource=['0x33fe00','0x33dc40','0x34c4b0','0x1213c0'],
                    mainAndChildBlocks=blocks, controllerCount=166, unboundControllers=[], overlappingControllers=[],
                    test08Unbound=missing, test08Overlap=overlap, firstUnboundMatchesDumpIndexAndHash=True,
                    crashIndex=164, crashPartHash='0x3cc45148', serializedReadback=changes,
                    originalParentEdgesPreserved=True, boneMappingsUnchanged=True,
                    wornAndHangingNamesMatchModelOrder=True,
                    allocationCensusUnchanged=True,
                    limitation='Offline parsing and native-algorithm reconstruction; game initialization not executed.')
    (ROOT/'evidence/test09-controller-validation.json').write_text(json.dumps(evidence,indent=2))
    report['status'] = 'Test09 independently read back; game test pending'
    report['controllerValidation'] = 'evidence/test09-controller-validation.json'
    report['arrayCensusRevalidated'] = 'evidence/test09-resource-readback/raw'
    (stage/'report.json').write_text(json.dumps(report,indent=2))
    validate_controller_gate(stage)
    print(json.dumps(evidence,indent=2))


if __name__ == '__main__':
    main()
