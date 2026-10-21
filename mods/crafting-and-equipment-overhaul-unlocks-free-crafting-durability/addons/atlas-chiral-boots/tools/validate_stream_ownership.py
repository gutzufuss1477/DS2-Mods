"""Independent Test10 resource readback and stream-ownership regression gate."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
from native_bundle import ROOT, Graph, DSAR
from prepare_resource_audit import JAVA, MODULES
from serialize_native_geometry import PACKAGE, GAME, ref
from export_native_resources import export
from fix_stream_ownership import audited_group
from fix_array_reservations import reservation_proof, validate_cache_reservations
from validate_controller_offsets import validate_controller_gate


def validate_stream_gate(stage):
    report = json.loads((stage/'report.json').read_text())
    manifest = json.loads((stage/'bundle/overlay.json').read_text())
    proof = json.loads((ROOT/report['streamOwnershipValidation']).read_text())
    assert proof['cache'] == report['cache']
    assert proof['resultHashes'] == {f['gamePath']:f['resultSha256'] for f in manifest['files']}
    assert proof['addedResources'] == 6 and proof['isolatedStreamingBuffers'] == 8
    assert len(proof['ownership']) == 2
    assert all(x['oldSharedResources'] and not x['newSharedResources'] for x in proof['ownership'])
    assert all(x['existingBytesUnchanged'] and
               x.get('streamingResourceArraysUnchanged',x['dynamicArraysUnchanged'])
               for x in proof['rawReadback'].values())


def main():
    stage = ROOT/'staging/test10-private-clip-buffers'
    report = json.loads((stage/'report.json').read_text())
    cache = Path(report['cache'])
    out = ROOT/'evidence/test10-resource-readback'
    out.mkdir()
    overrides = dict(DS2_KESTREL_GRAPH_OVERRIDE=str(cache/'streaming_graph.core'),
                     DS2_ATLAS_LINKTABLE_OVERRIDE=str(cache/'streaming_links.stream'),
                     DS2_KESTREL_EXT_STREAM_OVERRIDE=str(cache/'package.42.02.core.stream'))
    env = {k:v for k,v in os.environ.items() if not k.startswith('DS2_')}
    env.update(overrides, DS2_LAB_AUDIT_DIR=str(out/'raw'), DS2_LAB_AUDIT_GROUPS='67696,31126,39987')
    command = [str(JAVA/'java.exe'),'--enable-preview','--enable-native-access=odradek.core',
               '--add-opens=java.base/sun.nio.ch=odradek.core','--patch-module',
               'odradek.game.ds2='+str(ROOT/'build/array-audit/classes'),
               '--module-path',str(MODULES),'-m','odradek.app/sh.adelessfox.odradek.app.Main',
               'export','-s',str(GAME),'-o',str(out),'-f','json','67696:0','31126:0','39987:0']
    with (out/'audit.log').open('xb') as log:
        p = subprocess.run(command,env=env,stdout=log,stderr=subprocess.STDOUT)
    assert p.returncode == 0,(out/'audit.log').read_text()[-3000:]
    raw_proof = {}
    for gid in [67696,31126,39987]:
        a = (ROOT/f'evidence/test09-resource-readback/raw/{gid}.bin').read_bytes()
        b = (out/f'raw/{gid}.bin').read_bytes()
        assert b[:len(a)] == a
        assert len(b)-len(a) == (report['appendedRecordBytes'] if gid==31126 else 0)
        assert (out/f'raw/{gid}.arrays.tsv').read_bytes() == (
            ROOT/f'evidence/test09-resource-readback/raw/{gid}.arrays.tsv').read_bytes()
        raw_proof[str(gid)] = dict(originalPrefixSha256=hashlib.sha256(a).hexdigest(),
                                  finalSha256=hashlib.sha256(b).hexdigest(),
                                  existingBytesUnchanged=True,dynamicArraysUnchanged=True)
    graph = Graph(cache/'streaming_graph.core')
    old_cache = Path(report['previousCache'])
    old_graph = Graph(old_cache/'streaming_graph.core')
    group = audited_group(graph,(cache/'streaming_links.stream').read_bytes(),31126,out/'raw')
    old = audited_group(old_graph,(old_cache/'streaming_links.stream').read_bytes(),31126,
                        ROOT/'evidence/test09-resource-readback/raw')
    assert len(group.rows) == len(old.rows)+6
    changed = [i for i in range(len(old.rows)) if old.original(i).links != group.original(i).links]
    assert changed == [48122,48132]
    assert group.locators == old.locators
    assert len({bytes(group.original(i).raw[:16]) for i in range(len(group.rows))}) == len(group.rows)
    # Unchanged groups retain their exact link tables, not merely their bytes.
    for gid in [39987,67696]:
        a = next(g for g in old_graph.groups if g['id']==gid)
        b = next(g for g in graph.groups if g['id']==gid)
        assert a == b
    keys = set()
    for fix in report['clipOwnershipFixes']:
        keys.update((31126,fix[k]) for k in ['mesh','donor','primitive','originalPrimitive',
                                           'vertices','originalVertices','indices','originalIndices'])
    export([f'{g}:{i}' for g,i in sorted(keys)],out,overrides=overrides)
    def read(i):
        return json.loads(next(out.glob(f'*_31126_{i}.json')).read_text())
    def streamed_refs(g, mesh):
        refs = []
        for ext, i in g.original(mesh).links:
            if ext is None and g.rows[i][1] == 'PrimitiveResource':
                refs += [j for ext,j in g.original(i).links
                         if ext is None and g.rows[j][1] in ('VertexArrayResource','IndexArrayResource')]
        return set(refs)
    lifetimes = []
    for fix in report['clipOwnershipFixes']:
        m, donor = fix['mesh'],fix['donor']
        shared_before = streamed_refs(old,m)&streamed_refs(old,donor)
        shared_after = streamed_refs(group,m)&streamed_refs(group,donor)
        assert shared_before == {fix['originalVertices'],fix['originalIndices']}
        assert shared_after == set()
        mesh, original = read(m),read(donor)
        assert ref(mesh['Primitives'][0]) == (31126,fix['primitive'])
        assert ref(original['Primitives'][0]) == (31126,fix['originalPrimitive'])
        for key,oldkey in [('vertices','originalVertices'),('indices','originalIndices')]:
            a,b = read(fix[oldkey]),read(fix[key])
            assert a.pop('ObjectUUID') != b.pop('ObjectUUID')
            assert a == b and a['IsStreaming']
        # Each cloned primitive/buffer is referenced by exactly one owner.
        for child,owner in [(fix['primitive'],m),(fix['vertices'],fix['primitive']),
                            (fix['indices'],fix['primitive'])]:
            users = [i for i in range(len(group.rows)) if (None,child) in group.original(i).links]
            assert users == [owner],(child,users)
        # One mutable backing-allocation pointer per GPU buffer, unbound once
        # per owner: the old cross-owner list has duplicates, the new one none.
        before = list(streamed_refs(old,m))+list(streamed_refs(old,donor))
        after = list(streamed_refs(group,m))+list(streamed_refs(group,donor))
        assert len(before)>len(set(before)) and len(after)==len(set(after))
        lifetimes.append(dict(mesh=m,donor=donor,oldSharedResources=sorted(shared_before),
                              newSharedResources=[],independentMutableBuffers=fix['streamedBuffers']))
    manifest = json.loads((stage/'bundle/overlay.json').read_text())
    # Byte-identical body and controller resources permit carrying forward the
    # full native slot simulation, pinned to this independently verified cache.
    prior_proof = json.loads((ROOT/'evidence/test09-controller-validation.json').read_text())
    prior_proof.update(cache=report['cache'],
                       resultHashes={f['gamePath']:f['resultSha256'] for f in manifest['files']},
                       inheritedSlotSimulation='evidence/test09-controller-validation.json',
                       inheritedBecauseBodyGroupAndAllControllerResourcesByteIdentical=True,
                       test10Readback=raw_proof)
    proof_path = ROOT/'evidence/test10-controller-validation.json'
    proof_path.write_text(json.dumps(prior_proof,indent=2))
    report['controllerValidation'] = 'evidence/test10-controller-validation.json'
    report['arrayReservationProof'] = reservation_proof(graph,manifest)
    report['arrayCensusRevalidated'] = 'evidence/test10-resource-readback/raw'
    report['status'] = 'Test10 independently parsed, streaming owners isolated; game test pending'
    (stage/'report.json').write_text(json.dumps(report,indent=2))
    validate_cache_reservations(stage)
    validate_controller_gate(stage)
    result = dict(cache=str(cache),rawReadback=raw_proof,changedLinkRecords=changed,
                  resultHashes={f['gamePath']:f['resultSha256'] for f in manifest['files']},
                  addedResources=6,isolatedStreamingBuffers=8,ownership=lifetimes,
                  priorControllersStillValid=True,arrayReservationsValid=True,
                  currentCrashRva='0x20e2074',
                  limitation='Offline lifetime reconstruction; exact crashing buffer unavailable in minidump heap.')
    (ROOT/'evidence/test10-stream-ownership-validation.json').write_text(json.dumps(result,indent=2))
    report['streamOwnershipValidation'] = 'evidence/test10-stream-ownership-validation.json'
    (stage/'report.json').write_text(json.dumps(report,indent=2))
    validate_stream_gate(stage)
    print(json.dumps(result,indent=2))


if __name__ == '__main__':
    main()
