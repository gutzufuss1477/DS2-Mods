"""Convert the Blender render to the already validated native icon format."""
import hashlib
import json
from pathlib import Path
import shutil
import struct
import subprocess
import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'assets/rendered-menu-icon'
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()

def main():
    render = json.loads((ROOT/'evidence/test14-render-provenance.json').read_text())
    source = OUT/'atlas-chiral-menu.png'
    assert sha(source) == render['imageSha256']
    assert sha(Path(render['sourceBlend'])) == render['sourceSha256']
    converter = Path(shutil.which('texconv.exe'))
    commands = [
        [str(converter), '-f','BC3_UNORM_SRGB','-dx10','-m','1','-w','256','-h','160',
         '-srgbi','-if','CUBIC','-y','-o',str(OUT),str(source)],
        [str(converter), '-ft','png','-f','R8G8B8A8_UNORM','-srgbo','-sx','-native-preview',
         '-y','-o',str(OUT),str(OUT/'atlas-chiral-menu.dds')],
    ]
    for cmd in commands:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        print(result.stdout)
    dds = OUT/'atlas-chiral-menu.dds'
    data = dds.read_bytes()
    assert data[:4] == b'DDS ' and data[84:88] == b'DX10'
    assert struct.unpack_from('<II',data,12) == (160,256)
    assert struct.unpack_from('<I',data,28)[0] == 1
    assert struct.unpack_from('<5I',data,128) == (78,3,0,1,1)
    pixels = data[148:]
    assert len(pixels) == 40960
    preview = OUT/'atlas-chiral-menu-native-preview.png'
    image = Image.open(preview).convert('RGBA')
    rgba = np.array(image)
    alpha = rgba[:,:,3]
    y,x = np.where(alpha > 8)
    assert x.min() > 2 and x.max() < 253 and y.min() > 2 and y.max() < 157
    assert (alpha == 0).mean() > 0.35 and (alpha > 240).sum() > 7000
    assert ((alpha > 0) & (alpha < 255)).sum() > 100
    proof = dict(sourceRender=str(source.relative_to(ROOT)), renderSha256=sha(source),
                 dds=str(dds.relative_to(ROOT)), ddsSha256=sha(dds),
                 payloadSha256=hashlib.sha256(pixels).hexdigest(), payloadBytes=len(pixels),
                 width=256,height=160,mips=1,format='BC3_UNORM_SRGB',
                 nativePixelFormat=68, alphaMode='straight',
                 alphaBounds=[int(x.min()),int(y.min()),int(x.max()),int(y.max())],
                 fullyTransparentFraction=float((alpha==0).mean()),
                 antialiasedEdgePixels=int(((alpha>0)&(alpha<255)).sum()),
                 decodedPreview=str(preview.relative_to(ROOT)),decodedPreviewSha256=sha(preview),
                 converterSha256=sha(converter), commands=commands,
                 renderProvenance='evidence/test14-render-provenance.json',
                 sourceModelUnchanged=True)
    (OUT/'manifest.json').write_text(json.dumps(proof,indent=2))
    (ROOT/'evidence/test14-icon-pixel-provenance.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(proof,indent=2))

if __name__ == '__main__':
    main()
