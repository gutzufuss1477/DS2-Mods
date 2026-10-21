"""Embed the verified, transparent model render without changing native layout."""
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[1]
sha = lambda data: hashlib.sha256(data).hexdigest()

def main():
    p = json.loads((ROOT/'assets/rendered-menu-icon/manifest.json').read_text())
    b = (ROOT/p['dds']).read_bytes()
    assert sha(b) == p['ddsSha256']
    assert sha((ROOT/p['sourceRender']).read_bytes()) == p['renderSha256']
    assert b[:4] == b'DDS ' and b[84:88] == b'DX10'
    assert struct.unpack_from('<II',b,12) == (160,256)
    assert struct.unpack_from('<I',b,28)[0] == 1
    assert struct.unpack_from('<5I',b,128) == (78,3,0,1,1)
    pixels = b[148:]
    assert len(pixels) == 40960 and sha(pixels) == p['payloadSha256']
    (ROOT/'build/chiral_icon_pixels.inc').write_text(
        'static const unsigned char ChiralIconPixels[]={\n' +
        '\n'.join(','.join(f'0x{x:02x}' for x in pixels[i:i+32])+','
                  for i in range(0,len(pixels),32))+'\n};\n')
    print('RENDERED_ICON_EMBED: 40960 verified BC3 bytes; transparent Chiral model render')

if __name__ == '__main__':
    main()
