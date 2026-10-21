"""Independent native shader-input contracts and Test11 allocation readback."""
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
from collections import defaultdict
from native_bundle import ROOT, Graph, DSAR
from serialize_native_geometry import PACKAGE, GAME, ref
from prepare_resource_audit import JAVA, MODULES
from export_native_resources import export
from fix_stream_ownership import audited_group
from fix_hanging_shader import STAGE
from fix_array_reservations import validate_cache_reservations
from validate_controller_offsets import validate_controller_gate
from validate_stream_ownership import validate_stream_gate


def vertex_signature(meta, graph):
    s=meta['StreamingDataSource'];loc=s['Locator']
    path=PACKAGE/graph.files[loc&0xffffff].split('/')[-1]
    data=DSAR(path).read((loc>>24)+s['Offset'],s['Length'])
    start=-1
    while True:
        start=data.find(b'DXBC',start+1)
        if start<0:raise AssertionError('Vertex DXIL container not found')
        size,count=struct.unpack_from('<II',data,start+24)
        blob=data[start:start+size]
        if len(blob)!=size:continue
        chunks={}
        for offset in struct.unpack_from('<'+'I'*count,blob,32):
            tag=blob[offset:offset+4]
            length=struct.unpack_from('<I',blob,offset+4)[0]
            chunks[tag]=blob[offset+8:offset+8+length]
        if b'DXIL' not in chunks or struct.unpack_from('<H',chunks[b'DXIL'],2)[0]!=1:continue
        sig=chunks[b'ISG1'];n,first=struct.unpack_from('<II',sig)
        rows=[]
        for j in range(n):
            at=first+j*32+4
            nameoff,index,system,typ,register=struct.unpack_from('<5I',sig,at)
            rows.append(dict(semantic=sig[nameoff:].split(b'\0')[0].decode(),index=index,
                             system=system,componentType=typ,register=register,mask=sig[at+20]))
        return dict(streamSha256=hashlib.sha256(data).hexdigest(),vertexShaderSha256=hashlib.sha256(blob).hexdigest(),
                    vertexContainerOffset=start,inputs=rows)


def missing_inputs(va, signature):
    semantic={'Pos':('POSITION',0),'Normal':('NORMAL',0),'TangentBFlip':('TANGENT_BFLIP',0),
              'BlendIndices':('BLENDINDICES',0),'BlendIndices2':('BLENDINDICES',1),
              'BlendWeights':('BLENDWEIGHT',0),'BlendWeights2':('BLENDWEIGHT',1)}
    semantic.update({f'UV{i}':('TEXCOORD',i) for i in range(4)})
    supplied={semantic[e['Element']] for s in va['Streams'] for e in s['Elements']}
    required={(e['semantic'],e['index']) for e in signature['inputs'] if e['system']==0}
    return sorted(required-supplied)


def validate_shader_gate(stage):
    r=json.loads((stage/'report.json').read_text())
    m=json.loads((stage/'bundle/overlay.json').read_text())
    v=json.loads((ROOT/r['hangingShaderValidation']).read_text())
    assert v['cache']==r['cache']
    assert v['resultHashes']=={f['gamePath']:f['resultSha256'] for f in m['files']}
    assert v['oldShaderMissingInputs']==[['BLENDINDICES',1],['BLENDWEIGHT',1]]
    assert len(v['compatibleHangingPasses'])==3 and all(x['missingInputs']==[] for x in v['compatibleHangingPasses'])
    assert v['workingWornBytesAndReferencesUnchanged'] and v['isolatedClipOwnershipPreserved']
    assert v['independentArrayCensusMatchesReservations']


def main():
    r=json.loads((STAGE/'report.json').read_text());cache=Path(r['cache'])
    oldcache=Path(r['previousCache'])
    out=ROOT/'evidence/test11-resource-readback';out.mkdir()
    overrides=dict(DS2_KESTREL_GRAPH_OVERRIDE=str(cache/'streaming_graph.core'),
                   DS2_ATLAS_LINKTABLE_OVERRIDE=str(cache/'streaming_links.stream'),
                   DS2_KESTREL_EXT_STREAM_OVERRIDE=str(cache/'package.42.02.core.stream'))
    env={k:v for k,v in os.environ.items() if not k.startswith('DS2_')}
    env.update(overrides,DS2_LAB_AUDIT_DIR=str(out/'raw'),DS2_LAB_AUDIT_GROUPS='67696,31126,39987')
    command=[str(JAVA/'java.exe'),'--enable-preview','--enable-native-access=odradek.core',
             '--add-opens=java.base/sun.nio.ch=odradek.core','--patch-module',
             'odradek.game.ds2='+str(ROOT/'build/array-audit/classes'),'--module-path',str(MODULES),
             '-m','odradek.app/sh.adelessfox.odradek.app.Main','export','-s',str(GAME),
             '-o',str(out),'-f','json','67696:0','31126:0','39987:0']
    with (out/'audit.log').open('xb') as log:p=subprocess.run(command,env=env,stdout=log,stderr=subprocess.STDOUT)
    assert p.returncode==0,(out/'audit.log').read_text()[-3000:]
    census=json.loads((ROOT/r['arrayReservationProof']['census']).read_text())
    raw_proof={}
    for gid in [67696,31126,39987]:
        a=(ROOT/f'evidence/test10-resource-readback/raw/{gid}.bin').read_bytes()
        b=(out/f'raw/{gid}.bin').read_bytes()
        assert b[:len(a)]==a
        assert len(b)-len(a)==(r['appendedRecordBytes'] if gid==31126 else 0)
        old_arrays=(ROOT/f'evidence/test10-resource-readback/raw/{gid}.arrays.tsv').read_text()
        new_arrays=(out/f'raw/{gid}.arrays.tsv').read_text()
        assert new_arrays.startswith(old_arrays)
        counts=defaultdict(lambda:[0,0])
        for line in new_arrays.splitlines():
            owner,typ,n,kind=line.split('\t')
            if kind=='Array':
                counts[typ][0]+=1;counts[typ][1]+=int(n)
        assert dict(counts)==census[str(gid)]['actual'],gid
        raw_proof[str(gid)]=dict(existingBytesUnchanged=True,dynamicArraysUnchanged=new_arrays==old_arrays,
                                streamingResourceArraysUnchanged=True,finalSha256=hashlib.sha256(b).hexdigest())
    graph=Graph(cache/'streaming_graph.core');oldgraph=Graph(oldcache/'streaming_graph.core')
    g=audited_group(graph,(cache/'streaming_links.stream').read_bytes(),31126,out/'raw')
    old=audited_group(oldgraph,(oldcache/'streaming_links.stream').read_bytes(),31126,
                      ROOT/'evidence/test10-resource-readback/raw')
    assert len(g.rows)==len(old.rows)+2
    assert g.locators==old.locators
    changed=[i for i in range(len(old.rows)) if g.original(i).links!=old.original(i).links]
    assert changed==[48122,48132]
    material=r['hangingMaterial'];effect=material['effect'];shade=material['shadingGroup']
    for mesh in changed:
        a,b=old.original(mesh).links,g.original(mesh).links
        assert b==[(ext,shade if ext is None and j==48075 else j) for ext,j in a]
    for gid in [39987,67696]:
        assert next(x for x in graph.groups if x['id']==gid)==next(x for x in oldgraph.groups if x['id']==gid)
    keys=[48116,48126,48083,48074,48075,43244,effect,shade,48122,48132]
    export([f'31126:{i}' for i in keys],out,overrides=overrides)
    def read(i):return json.loads(next(out.glob(f'*_31126_{i}.json')).read_text())
    new_effect,donor=read(effect),read(43244)
    # Apart from UUID and texture binding refs, native hanging effect is exact.
    def normalize(d):
        if isinstance(d,list):return [normalize(x) for x in d]
        if isinstance(d,dict):return {k:normalize(v) for k,v in d.items() if k!='ObjectUUID'}
        if d=='<ref to 67696:16>':return '<ref to 67696:0>'
        if d=='<ref to 67696:17>':return '<ref to 67696:1>'
        return d
    assert normalize(new_effect)==normalize(donor)
    assert read(shade)['RenderEffect']==f'<ref to 31126:{effect}>'
    shader_dir=ROOT/'evidence/test10-shader-signatures'
    signatures={}
    for file in shader_dir.glob('ShaderResource_*.json'):
        _,gid,idx=file.stem.split('_')
        signatures[(int(gid),int(idx))]=vertex_signature(json.loads(file.read_text()),Graph(PACKAGE/'streaming_graph.core'))
    bad=missing_inputs(read(48116),signatures[(31126,1170)])
    assert bad==[('BLENDINDICES',1),('BLENDWEIGHT',1)]
    passes=[]
    for tech in new_effect['TechniqueSets'][0]['RenderTechniques']:
        key=ref(tech['Shader']);s=signatures[key]
        for va in [48116,48126]:assert missing_inputs(read(va),s)==[]
        passes.append(dict(shader=tech['Shader'],missingInputs=[],signature=s))
    for tech in read(48074)['TechniqueSets'][0]['RenderTechniques']:
        assert missing_inputs(read(48083),signatures[ref(tech['Shader'])])==[]
    # All eight private clip resources and their unique ownership remain.
    for fix in r['clipOwnershipFixes']:
        for child,owner in [(fix['primitive'],fix['mesh']),(fix['vertices'],fix['primitive']),
                            (fix['indices'],fix['primitive'])]:
            assert [i for i in range(len(g.rows)) if (None,child) in g.original(i).links]==[owner]
    manifest=json.loads((STAGE/'bundle/overlay.json').read_text())
    hashes={f['gamePath']:f['resultSha256'] for f in manifest['files']}
    body=json.loads((ROOT/'evidence/test10-controller-validation.json').read_text())
    body.update(cache=str(cache),resultHashes=hashes,allocationCensusUnchanged=False,
                controllerAllocationCensusUnchanged=True,test11Readback=raw_proof,
                inheritedSlotSimulation='evidence/test10-controller-validation.json')
    (ROOT/'evidence/test11-controller-validation.json').write_text(json.dumps(body,indent=2))
    stream=json.loads((ROOT/'evidence/test10-stream-ownership-validation.json').read_text())
    stream.update(cache=str(cache),resultHashes=hashes,rawReadback=raw_proof,
                  inheritedOwnershipProof='evidence/test10-stream-ownership-validation.json',
                  currentCrashRva='nvwgf2umx.dll+0x7e1b40')
    (ROOT/'evidence/test11-stream-ownership-validation.json').write_text(json.dumps(stream,indent=2))
    v=dict(cache=str(cache),resultHashes=hashes,oldShaderMissingInputs=bad,compatibleHangingPasses=passes,
           workingWornBytesAndReferencesUnchanged=True,isolatedClipOwnershipPreserved=True,
           independentArrayCensusMatchesReservations=True,rawReadback=raw_proof,
           changedLinks=changed,newEffect=effect,newShade=shade,
           limitation='Native compiled vertex signatures checked offline; in-game render and animation pending.')
    (ROOT/'evidence/test11-hanging-shader-validation.json').write_text(json.dumps(v,indent=2))
    r.update(controllerValidation='evidence/test11-controller-validation.json',
             streamOwnershipValidation='evidence/test11-stream-ownership-validation.json',
             hangingShaderValidation='evidence/test11-hanging-shader-validation.json',
             status='Test11 shader inputs, native readback and allocations verified; game test pending')
    (STAGE/'report.json').write_text(json.dumps(r,indent=2))
    validate_cache_reservations(STAGE);validate_controller_gate(STAGE);validate_stream_gate(STAGE);validate_shader_gate(STAGE)
    print(json.dumps({k:v[k] for k in ['cache','oldShaderMissingInputs','workingWornBytesAndReferencesUnchanged',
                                     'isolatedClipOwnershipPreserved','independentArrayCensusMatchesReservations',
                                     'changedLinks','newEffect','newShade']},indent=2))


if __name__=='__main__':
    main()
