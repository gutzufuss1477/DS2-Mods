"""Inspect the accepted asset with Blender in background; never save its source."""
import bpy, json
from pathlib import Path

root = Path('D:/DS2-Asset-Lab/mods/atlas-chiral-addon')
source = Path('D:/DS2-Asset-Lab/analysis/chiral-crafting-v198-integration-20261019/accepted-prototype/Chiral_DS2_Rigged_Sam_Cuff_v3.blend')
bpy.ops.wm.open_mainfile(filepath=str(source), use_scripts=False)
bpy.context.scene.frame_set(1)
rows = []
for obj in bpy.context.scene.objects:
    if obj.type != 'MESH':
        continue
    mats = []
    for mat in obj.data.materials:
        if mat is None:
            continue
        images = [dict(node=n.name, image=n.image.name, path=n.image.filepath, size=list(n.image.size), packed=bool(n.image.packed_file))
                  for n in mat.node_tree.nodes if n.type == 'TEX_IMAGE' and n.image] if mat.use_nodes else []
        mats.append(dict(name=mat.name, images=images))
    rows.append(dict(name=obj.name, vertices=len(obj.data.vertices), polygons=len(obj.data.polygons),
                     groups=[g.name for g in obj.vertex_groups],
                     armatures=[m.object.name if m.object else None for m in obj.modifiers if m.type=='ARMATURE'],
                     matrix=[list(r) for r in obj.matrix_world], uvLayers=[x.name for x in obj.data.uv_layers], materials=mats))
(root/'evidence').mkdir(exist_ok=True)
(root/'evidence/scene-inspection.json').write_text(json.dumps(rows, indent=2), encoding='utf-8')
print('SCENE_INSPECTED', json.dumps([dict(name=r['name'], vertices=r['vertices'], materials=[m['name'] for m in r['materials']]) for r in rows]))
