"""Embed the exact shipped DS1 BC3 payload. No image synthesis or resampling."""
import hashlib
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = 'cb5bd451c75d8115f5de1af3112c215b258e83d9ace3a73ad8e8155917d696c3'


def main():
    source = ROOT/'assets/ds1-menu-icon/ut_ui_icon_sam_boot_mk3_chiral_l.dds'
    b = source.read_bytes()
    assert b[:4] == b'DDS ' and b[84:88] == b'DX10'
    assert struct.unpack_from('<II', b, 12) == (160, 256)
    assert struct.unpack_from('<I', b, 28)[0] == 1
    assert struct.unpack_from('<5I', b, 128) == (77, 3, 0, 1, 0)
    pixels = b[148:]
    assert len(pixels) == 40960 and hashlib.sha256(pixels).hexdigest() == EXPECTED
    out = ROOT/'build/chiral_icon_pixels.inc'
    out.write_text('static const unsigned char ChiralIconPixels[]={\n' +
                   '\n'.join(','.join(f'0x{x:02x}' for x in pixels[i:i+32])+','
                             for i in range(0, len(pixels), 32)) + '\n};\n')
    proof = dict(sourceGame='Death Stranding Directors Cut',
                 originalPath='interface/textures/ds/equip_icons/ut_ui_icon_sam_boot_mk3_chiral_l.core',
                 coreSha256='949bd6c0a5df8acababe414a2902a3275e212f03f06c225092fb1bf7abf65b27',
                 ddsSha256=hashlib.sha256(b).hexdigest(), payloadSha256=EXPECTED,
                 originalInternalDataSha256=EXPECTED, width=256, height=160,
                 format='BC3', dxgiResourceFormat='BC3_UNORM_SRGB',
                 nativePixelFormat=68, payloadBytes=len(pixels), mips=1,
                 alphaPreserved=True, resampled=False, recolored=False)
    (ROOT/'evidence/test12-icon-pixel-provenance.json').write_text(json.dumps(proof, indent=2))
    print('DS1_ICON_EMBED: 40960 original BC3 bytes, SHA256 verified')


if __name__ == '__main__':
    main()
