"""Export lossless authoring inputs for the separate visual addon.

Run with Blender --background --factory-startup. This does NOT claim the
result is an engine-ready DS2 resource pack; no game archives are accessed.
"""
import bpy
import hashlib
import json
from pathlib import Path
import numpy as np

ROOT = Path('D:/DS2-Asset-Lab/mods/atlas-chiral-addon')
SOURCE = Path('D:/DS2-Asset-Lab/analysis/chiral-crafting-v198-integration-20261019/accepted-prototype/Chiral_DS2_Rigged_Sam_Cuff_v3.blend')
ORIGINAL_AUTHORING = Path('D:/DS2-Asset-Lab/assets/ds1-chiral-ds2/Chiral_DS2_Rigged_Sam_Cuff_v3.blend')
OUT = ROOT/'assets/chiral-cuff-v3'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    source_hash = sha(SOURCE)
    assert not OUT.exists() or not any(OUT.iterdir()), 'Keep existing accepted export unchanged'
    OUT.mkdir(parents=True, exist_ok=True)
    bpy.ops.wm.open_mainfile(filepath=str(SOURCE), use_scripts=False)
    bpy.context.scene.frame_set(1)
    names = {
        'C | Chiral left sole':'left-sole', 'C | Chiral right sole':'right-sole',
        'C | Chiral left upper':'left-upper', 'C | Chiral right upper':'right-upper',
        'C | Sam Beine 270':'pants-body', 'C | Sam Beine 638':'pants-strips',
    }
    assert set(o.name for o in bpy.context.scene.objects if o.type=='MESH') == set(names)
    records, rigs, materials, images = [], {}, {}, {}
    for source_name, name in names.items():
        obj = bpy.data.objects[source_name]
        assert np.allclose(np.array(obj.matrix_world), np.eye(4), atol=1e-8), 'Unexpected authoring transform'
        mesh = obj.data
        assert 0<len(mesh.uv_layers)<=4, 'Unexpected UV layout'
        mesh.calc_loop_triangles()
        mesh.calc_tangents(uvmap=mesh.uv_layers.active.name)
        rig = next(m.object for m in obj.modifiers if m.type=='ARMATURE')
        groups = [g.name for g in obj.vertex_groups]
        assert all(g in rig.data.bones for g in groups)
        if rig.name not in rigs:
            rigs[rig.name] = {'jointNames':groups, 'restMatrices':{
                g:[list(r) for r in rig.data.bones[g].matrix_local] for g in groups},
                'parents':{g:rig.data.bones[g].parent.name if rig.data.bones[g].parent else None for g in groups}}
        assert rigs[rig.name]['jointNames']==groups
        base_bones = np.zeros((len(mesh.vertices),8),dtype='<u2')
        base_weights = np.zeros((len(mesh.vertices),8),dtype='<f4')
        max_influences = 0
        for v in mesh.vertices:
            pairs = sorted([(g.group,g.weight) for g in v.groups if g.weight>1e-8],key=lambda x:(-x[1],x[0]))
            assert 0<len(pairs)<=8, 'Unsupported or unweighted vertex'
            assert abs(sum(w for _,w in pairs)-1)<1e-5
            max_influences=max(max_influences,len(pairs))
            for i,(bone,weight) in enumerate(pairs):
                base_bones[v.index,i]=bone
                base_weights[v.index,i]=weight
        corners, positions, normals, tangents, uv, vertex_sources, faces = {}, [], [], [], [], [], []
        for tri in mesh.loop_triangles:
            assert tri.material_index==0, 'Multiple materials require primitive separation'
            face=[]
            for loop_index in tri.loops:
                loop=mesh.loops[loop_index]
                n=tuple(mesh.corner_normals[loop_index].vector)
                t=(*tuple(loop.tangent),float(loop.bitangent_sign))
                tex=tuple(tuple(layer.data[loop_index].uv) for layer in mesh.uv_layers)
                key=(loop.vertex_index,n,t,tex)
                if key not in corners:
                    corners[key]=len(positions)
                    positions.append(tuple(mesh.vertices[loop.vertex_index].co))
                    normals.append(n);tangents.append(t);uv.append(tex);vertex_sources.append(loop.vertex_index)
                face.append(corners[key])
            faces.append(face)
        assert len(positions)<65536, 'Needs explicit 32-bit native index layout'
        source_vertices=np.array(vertex_sources,dtype='<u4')
        data=dict(positions=np.asarray(positions,dtype='<f4'), normals=np.asarray(normals,dtype='<f4'),
                  tangents=np.asarray(tangents,dtype='<f4'), uv=np.asarray(uv,dtype='<f4'),
                  triangles=np.asarray(faces,dtype='<u2'), bones=base_bones[source_vertices],
                  weights=base_weights[source_vertices], sourceVertex=source_vertices)
        assert all(np.isfinite(v).all() for v in data.values())
        out=OUT/(name+'.npz')
        np.savez_compressed(out,**data)
        with np.load(out) as loaded:
            assert set(loaded.files)==set(data)
            assert all(np.array_equal(loaded[k],v) for k,v in data.items())
        records.append(dict(name=name, sourceObject=source_name, file=out.name, sha256=sha(out),
                            sourceVertices=len(mesh.vertices), exportedVertices=len(positions), triangles=len(faces),
                            maxInfluences=max_influences, discardedInfluences=0,
                            maxWeightSumError=float(abs(data['weights'].sum(1)-1).max()),
                            rig=rig.name, material=obj.data.materials[0].name, uvLayers=[layer.name for layer in mesh.uv_layers],
                            bounds=[data['positions'].min(0).tolist(),data['positions'].max(0).tolist()]))
        mat=obj.data.materials[0]
        if mat.name not in materials:
            materials[mat.name]={'images':[], 'links':[dict(fromNode=l.from_node.name,fromSocket=l.from_socket.name,
                toNode=l.to_node.name,toSocket=l.to_socket.name) for l in mat.node_tree.links]}
            for n in mat.node_tree.nodes:
                if n.type!='TEX_IMAGE' or not n.image:
                    continue
                im=n.image
                if im.name not in images:
                    origin=None
                    if im.packed_file:
                        raw=bytes(im.packed_file.data)
                    else:
                        # The accepted scene was copied byte-for-byte. Its relative
                        # texture references remain relative to the authoring location.
                        origin=Path(bpy.path.abspath(im.filepath,start=str(ORIGINAL_AUTHORING.parent))).resolve()
                        assert origin.is_relative_to(Path('D:/DS2-Asset-Lab').resolve()) and origin.is_file()
                        raw=origin.read_bytes()
                    assert raw.startswith(b'\x89PNG\r\n\x1a\n'), 'Unrecognized source image container'
                    h=hashlib.sha256(raw).hexdigest()
                    filename='texture-'+h+'.png'
                    p=OUT/filename
                    if not p.exists():p.write_bytes(raw)
                    images[im.name]=dict(file=filename,sha256=h,bytes=len(raw),size=list(im.size),
                                         colorSpace=im.colorspace_settings.name,sourceBytesUnchanged=True,
                                         origin=str(origin) if origin else 'packed in accepted scene')
                materials[mat.name]['images'].append(dict(node=n.name,image=im.name))
    assert sha(SOURCE)==source_hash, 'Accepted scene was modified'
    manifest=dict(schemaVersion=1,status='Authoring inputs exported; native DS2 resource binding still required',
                  source=str(SOURCE),sourceSha256=source_hash,sourceUnchanged=True,
                  sourceCoordinates='Unmodified Blender mesh coordinates; world transform identity; native binding must validate axes',
                  meshes=records,rigs=rigs,materials=materials,images=images,
                  nativeResourcePackageReady=False,gameTested=False)
    (OUT/'manifest.json').write_text(json.dumps(manifest,indent=2,ensure_ascii=False),encoding='utf-8')
    print('ADDON_ASSET_EXPORT',json.dumps(dict(meshes=len(records),triangles=sum(x['triangles'] for x in records),
          bootTriangles=sum(x['triangles'] for x in records if not x['name'].startswith('pants')),
          textures=len(images),maxInfluences=max(x['maxInfluences'] for x in records),sourceUnchanged=True)))


main()
