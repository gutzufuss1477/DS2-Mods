"""Independent native readback of final private graph, hierarchy and hanging mesh."""
import json
from pathlib import Path
import sys
import numpy as np
from native_bundle import ROOT
from serialize_native_geometry import metadata
from export_native_resources import export

def main():
    r = json.loads((ROOT/'staging/native-complete-v1/report.json').read_text())
    cache = Path(r['cache'])
    ov = {k:str(cache/v) for k,v in [
        ('DS2_KESTREL_GRAPH_OVERRIDE','streaming_graph.core'),
        ('DS2_ATLAS_LINKTABLE_OVERRIDE','streaming_links.stream'),
        ('DS2_KESTREL_EXT_STREAM_OVERRIDE','package.42.02.core.stream')]}
    out = ROOT/'evidence/native-complete-readback-v1'
    refs = ['31126:9366','31126:4064','39987:1455','39987:0','31126:20086','31126:3575']
    refs += [f"{m['group']}:{m['mesh']}" for m in r['meshes']]
    refs += ['31126:48109','31126:48115','31126:48125','31126:48135','39987:2600','39987:2603']
    export(refs, out, overrides=ov)
    def read(g,i):
        return json.loads(next(out.glob(f'*_{g}_{i}.json')).read_text())
    for g,i in [(31126,9366),(39987,1455),(31126,20086)]:
        a,b=metadata(g,i),read(g,i)
        for key in ['ModelPartResources','ExpandedModelPartResources']:
            assert b[key][:len(a[key])]==a[key]
        assert a.get('Helper')==b.get('Helper')
    for g,i in [(31126,4064),(39987,0),(31126,3575)]:
        a,b=metadata(g,i),read(g,i)
        indices={n:i for i,n in enumerate(b['ModelPartNames'])}
        assert len(indices)==len(b['ModelPartNames'])
        for j,name in enumerate(a['ModelPartNames']):
            p=a['ParentControllerIndex'][j]
            assert b['ParentControllerIndex'][indices[name]]==(-1 if p<0 else indices[a['ModelPartNames'][p]])
    for g,i in [(31126,48109),(31126,48115),(31126,48125),(31126,48135),(39987,2600),(39987,2603)]:
        assert read(g,i)['IsHideAndDisableDefault']
    export(['31126:48122','31126:48132'], ROOT/'evidence/native-complete-hanging-cast-v1', overrides=ov, format='model.cast')
    print('Final native hierarchy parsed and original parent relationships preserved.')

if __name__=='__main__': main()
