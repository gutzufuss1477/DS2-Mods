"""Repack original Chiral texture channels for the DS2 native boot shader.

No generated detail and no authoring-source changes. Reflectance remains linear
as specified by the original DS1 TextureSet (the Blender preview used an sRGB
approximation). Original source resolutions are retained.
"""
from pathlib import Path
import io
import hashlib
import json
import shutil
import struct
import subprocess
import numpy as np
from PIL import Image
from native_bundle import ROOT, murmur128
from overlay_cache import digest

OUT = ROOT/'staging/native-materials-v2'


def build():
    assert not OUT.exists()
    OUT.mkdir()
    assets = ROOT/'assets/chiral-cuff-v3'
    manifest = json.loads((assets/'manifest.json').read_text())
    def source(name):
        item = manifest['images'][name]
        path = assets/item['file']
        assert digest(path) == item['sha256']
        return Image.open(path).convert('RGBA')
    color = source('entry-1.png')
    specular = source('entry-2.png')
    normal_xy = np.array(source('entry-0.png'))
    rough = np.array(source('entry-4.png').resize(
        (normal_xy.shape[1], normal_xy.shape[0]), Image.Resampling.BILINEAR))[:, :, 2]
    packed = normal_xy.copy()
    packed[:, :, 2] = rough
    packed[:, :, 3] = 255  # No separate AO map in accepted authoring material.
    recipes = [
        ('normal-roughness-ao', Image.fromarray(packed), 'BC7_UNORM', 1, 75),
        ('diffuse', color, 'BC1_UNORM_SRGB', 12, 66),
        ('reflectance', specular, 'BC1_UNORM', 13, 66),
        ('paint-mask', Image.new('RGBA', (4, 4), (0, 0, 0, 255)), 'BC1_UNORM', 4, 66),
        ('curvature', Image.new('RGBA', (4, 4), (128, 128, 128, 255)), 'BC1_UNORM', 5, 66),
        ('luminance', Image.new('RGBA', (4, 4), (255, 255, 255, 255)), 'BC1_UNORM', 11, 66),
        ('mask-alpha', Image.new('RGBA', (4, 4), (255, 255, 255, 255)), 'BC1_UNORM', 9, 66),
    ]
    converter = shutil.which('texconv.exe')
    assert converter
    rows = []
    for name, img, fmt, source_id, pixel_format in recipes:
        png = OUT/(name+'.png')
        img.save(png)
        command = [converter, '-f', fmt, '-m', '0', '-o', str(OUT), str(png)]
        result = subprocess.run(command, capture_output=True, text=True)
        (OUT/(name+'.log')).write_text(result.stdout+result.stderr)
        assert result.returncode == 0, name
        dds = (OUT/(name+'.dds')).read_bytes()
        assert dds[:4] == b'DDS '
        offset = 148 if dds[84:88] == b'DX10' else 128
        height, width = struct.unpack_from('<II', dds, 12)
        mips = struct.unpack_from('<I', dds, 28)[0]
        pixels = dds[offset:]
        block_bytes = 16 if fmt.startswith('BC7') else 8
        expected = sum(max(1, (max(1, width >> mip)+3)//4) *
                       max(1, (max(1, height >> mip)+3)//4)*block_bytes for mip in range(mips))
        assert len(pixels) == expected
        (OUT/(name+'.pixels')).write_bytes(pixels)
        # DDS is independently decoded by Pillow before native serialization.
        readback_dds = bytearray(dds)
        # Pillow lacks DXGI 72 (BC1 sRGB), whose blocks are identical to 71.
        # Relabel only the in-memory decoder input; stored DDS remains untouched.
        if offset == 148 and struct.unpack_from('<I', readback_dds, 128)[0] == 72:
            struct.pack_into('<I', readback_dds, 128, 71)
        decoded = Image.open(io.BytesIO(readback_dds))
        decoded.load()
        assert decoded.size == img.size
        rows.append(dict(name=name, donorTexture=source_id, width=width, height=height,
                         mips=mips, pixelFormat=pixel_format, bytes=len(pixels),
                         sha256=hashlib.sha256(pixels).hexdigest(),
                         format=fmt, colorSpace='sRGB' if 'SRGB' in fmt else 'Linear'))
    report = dict(status='Native texture payloads built; native parser validation pending',
                  textures=rows, converterSha256=digest(converter),
                  normalChannels='RG original normal XY; B accepted roughness; A neutral AO=1',
                  reflectance='Original DS1 RGB reflectance, linear per original TextureSet',
                  generatedSurfaceDetail=False, originalImagesUnchanged=True,
                  neutralMaps='Paint mask disabled; neutral curvature, luminance, alpha',
                  caveat='Native lighting and optional painting/wetness behavior require game review')
    (OUT/'report.json').write_text(json.dumps(report, indent=2))
    print(json.dumps(report))


def add_material(boots, textures):
    """Append private TextureSet, textures, shader bindings, shading group."""
    report = json.loads((OUT/'report.json').read_text())
    texture_set = textures.clone(0, 'chiral/native-boot-texture-set')
    cloned = {}
    for row in report['textures']:
        source_id = row['donorTexture']
        texture = textures.clone(source_id, 'chiral/'+row['name'])
        original = textures.original(source_id)
        assert original.links == [(None, 0)] and len(original.locators) == 0
        assert struct.unpack_from('<I', original.raw, 17)[0] == 8
        old_total = struct.unpack_from('<I', original.raw, 94)[0]
        assert len(original.raw) == 110+old_total-12
        pixels = (OUT/(row['name']+'.pixels')).read_bytes()
        assert hashlib.sha256(pixels).hexdigest() == row['sha256']
        texture.raw = texture.raw[:110]+pixels
        texture.raw[21:53] = b'\xff'*32
        struct.pack_into('<HH', texture.raw, 64, row['width'], row['height'])
        texture.raw[70] = row['mips']
        texture.raw[71] = row['pixelFormat']
        texture.raw[73] = int(row['colorSpace'] == 'sRGB')
        # RVA 0x24e5d30: selector zero keeps every mip resident even with the
        # original usage flags. Preserve bit 0x08: RVA 0x2112c70 otherwise
        # requests D3D12 ALLOW_UNORDERED_ACCESS, invalid for BC1/BC7.
        # DDS export ignores these allocation settings; GPU validation matters.
        assert texture.raw[75] == 0xa9
        texture.raw[76] = 0  # Disable automatic (0xff) streamed mip selection.
        texture.raw[78:94] = murmur128(pixels)
        struct.pack_into('<4I', texture.raw, 94, len(pixels)+12, len(pixels), 0, 0)
        texture.links = [(None, texture_set.id)]
        cloned[source_id] = texture.id
    texture_set.remap({(None, k):(None, v) for k, v in cloned.items()})
    # All private textures are embedded. Remove the original streamed source
    # while retaining its constant/default external texture and native preset.
    original_set = json.loads((ROOT/'evidence/native-boot-texture-v1/TextureSet_67696_0.json').read_text())
    length = original_set['StreamingDataSource']['Length']
    texture_set.replace(bytes([40])+struct.pack('<II', 0, length), bytes([50])+bytes(8))
    assert len(texture_set.locators) == 1
    texture_set.locators = []
    sub_start, sub_count = boots.info['subStart'], boots.info['subCount']
    children = struct.unpack_from('<'+'I'*sub_count, boots.graph.parts['subgroups'], 4+4*sub_start)
    ordinal = children.index(67696)
    effect = boots.clone(44191, 'chiral/native-boot-effect')
    assert (ordinal, 0) in effect.links and (ordinal, 1) in effect.links
    effect.remap({(ordinal, 0):(ordinal, texture_set.id), (ordinal, 1):(ordinal, cloned[1])})
    shade = boots.clone(27025, 'chiral/native-boot-shading-group')
    assert shade.links == [(None, 44191)]
    shade.links = [(None, effect.id)]
    return dict(shadingGroup=shade.id, effect=effect.id, textureSet=texture_set.id,
                textures=cloned, subgroupOrdinal=ordinal)


def add_hanging_material(boots, material):
    """Two-influence shader variant; the worn six-influence VS is incompatible."""
    effect = boots.clone(43244, 'chiral/native-hanging-effect')
    ordinal = material['subgroupOrdinal']
    assert (ordinal, 0) in effect.links and (ordinal, 1) in effect.links
    effect.remap({(ordinal, 0): (ordinal, material['textureSet']),
                  (ordinal, 1): (ordinal, material['textures'][1])})
    shade = boots.clone(27077, 'chiral/native-hanging-shading-group')
    assert shade.links == [(None, 43244)]
    shade.links = [(None, effect.id)]
    return dict(effect=effect.id, shadingGroup=shade.id, donorEffect=43244,
                donorShadingGroup=27077)


if __name__ == '__main__':
    build()
