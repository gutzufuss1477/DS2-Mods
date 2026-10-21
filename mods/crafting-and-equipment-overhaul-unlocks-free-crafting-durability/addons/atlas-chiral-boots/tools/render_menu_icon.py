"""Render the accepted boot to a separate transparent menu image; source is read-only."""
import bpy
import hashlib
import json
import math
from pathlib import Path
from mathutils import Vector

ROOT = Path('D:/DS2-Asset-Lab/mods/atlas-chiral-addon')
SOURCE = Path('D:/DS2-Asset-Lab/analysis/chiral-crafting-v198-integration-20261019/accepted-prototype/Chiral_DS2_Rigged_Sam_Cuff_v3.blend')
OUT = ROOT/'assets/rendered-menu-icon'
EXPECTED = '6ec1bcb1d467f8915cf922129d6c63ea1a3f1589b32d05a2c856d580c5b57cb1'
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
assert sha(SOURCE) == EXPECTED
OUT.mkdir(exist_ok=True)
bpy.ops.wm.open_mainfile(filepath=str(SOURCE), use_scripts=False)
assets = ROOT/'assets/chiral-cuff-v3'
manifest = json.loads((assets/'manifest.json').read_text())
# The accepted snapshot keeps relative image paths from its authoring folder.
# Resolve them to the byte-identical, hashed export in this render copy only.
texture_bindings = []
for mat in bpy.data.materials:
    if not mat.use_nodes:
        continue
    for node in mat.node_tree.nodes:
        if node.type != 'TEX_IMAGE' or not node.image:
            continue
        old = node.image
        entry = manifest['images'].get(old.name)
        if entry is None:
            continue
        path = assets/entry['file']
        assert sha(path) == entry['sha256']
        fresh = bpy.data.images.load(str(path), check_existing=True)
        fresh.colorspace_settings.name = 'Non-Color' if old.name == 'entry-2.png' else old.colorspace_settings.name
        node.image = fresh
        texture_bindings.append(dict(originalName=old.name, sha256=entry['sha256'],
                                     colorSpace=fresh.colorspace_settings.name))
scene = bpy.context.scene
scene.frame_set(1)
names = {'C | Chiral left sole', 'C | Chiral left upper'}
for collection in bpy.data.collections:
    collection.hide_render = False
for obj in scene.objects:
    obj.hide_render = obj.name not in names
    if obj.type == 'ARMATURE':
        obj.data.pose_position = 'REST'
for name in names:
    bpy.data.objects[name].hide_set(False)

points = [obj.matrix_world @ v.co for obj in scene.objects if obj.name in names for v in obj.data.vertices]
low = Vector(tuple(min(v[i] for v in points) for i in range(3)))
high = Vector(tuple(max(v[i] for v in points) for i in range(3)))
center = (low+high)/2

cam_data = bpy.data.cameras.new('ATLAS menu orthographic')
cam = bpy.data.objects.new('ATLAS menu camera', cam_data)
scene.collection.objects.link(cam)
cam.location = center+Vector((-1.0, 0.16, 0.18))*3
cam.rotation_euler = (center-cam.location).to_track_quat('-Z', 'Y').to_euler()
cam_data.type = 'ORTHO'
scene.camera = cam
bpy.context.view_layer.update()
inv = cam.matrix_world.inverted()
projected = [inv @ p for p in points]
left, right = min(p.x for p in projected), max(p.x for p in projected)
bottom, top = min(p.y for p in projected), max(p.y for p in projected)
aspect = 256/160
cam_data.ortho_scale = max((right-left)/0.86, (top-bottom)*aspect/0.86)
cam.location += cam.rotation_euler.to_matrix() @ Vector(((left+right)/2, (bottom+top)/2, 0))

world = bpy.data.worlds.new('ATLAS menu neutral studio')
world.use_nodes = True
world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.55, 0.59, 0.66, 1)
world.node_tree.nodes['Background'].inputs['Strength'].default_value = 0.12
scene.world = world

def area(name, offset, watts, size, color):
    data = bpy.data.lights.new(name, 'AREA')
    data.energy, data.shape, data.size, data.color = watts, 'DISK', size, color
    obj = bpy.data.objects.new(name, data)
    scene.collection.objects.link(obj)
    obj.location = center+Vector(offset)
    obj.rotation_euler = (center-obj.location).to_track_quat('-Z','Y').to_euler()

area('Broad neutral key', (-0.5,0.45,0.8), 18, 0.45, (1,0.96,0.88))
area('Soft front fill', (-0.45,0.8,0.1), 7, 0.6, (0.86,0.92,1))
area('Upper rear edge', (0.45,-0.4,0.6), 22, 0.35, (1,1,1))
scene.render.engine = 'CYCLES'
scene.cycles.device = 'CPU'
scene.cycles.samples = 96
scene.cycles.use_denoising = True
scene.render.resolution_x, scene.render.resolution_y = 1024, 640
scene.render.resolution_percentage = 100
scene.render.film_transparent = True
scene.render.image_settings.file_format = 'PNG'
scene.render.image_settings.color_mode = 'RGBA'
scene.render.image_settings.color_depth = '8'
scene.view_settings.view_transform = 'AgX'
scene.view_settings.look = 'AgX - Medium High Contrast'
scene.view_settings.exposure = 0
scene.view_settings.gamma = 1
scene.use_nodes = False
scene.render.filepath = str(OUT/'atlas-chiral-menu.png')
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'atlas-chiral-menu.blend'))
bpy.ops.render.render(write_still=True)
assert sha(SOURCE) == EXPECTED
report = dict(sourceBlend=str(SOURCE), sourceSha256=EXPECTED,
              sourceUnchanged=True, geometryObjects=sorted(names),
              geometryModified=False, materialShadingNodesModified=False,
              textureBindings=texture_bindings,
              reflectanceInterpretation='Linear, matching original DS1 TextureSet and installed native material',
              image=str(OUT/'atlas-chiral-menu.png'), imageSha256=sha(OUT/'atlas-chiral-menu.png'),
              width=1024, height=640, transparent=True, renderer='Blender Cycles',
              camera='Orthographic; toe left, outer side, slightly above',
              samples=scene.cycles.samples)
(ROOT/'evidence/test14-render-provenance.json').write_text(json.dumps(report,indent=2))
print('ATLAS_MENU_RENDER_COMPLETE', json.dumps(report))
