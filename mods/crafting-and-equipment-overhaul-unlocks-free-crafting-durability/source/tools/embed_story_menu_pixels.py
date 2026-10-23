from pathlib import Path
import hashlib,json
R=Path(__file__).resolve().parents[1]
pixels=(R/'assets/story-coffin-menu.bc7').read_bytes()
spec=json.loads((R/'config/story-menu-source.json').read_text())
assert len(pixels)==163840 and hashlib.sha256(pixels).hexdigest()==spec['sha256']
with (R/'build/story_coffin_menu_pixels.inc').open('w') as f:
 f.write('static const unsigned char StoryCoffinMenuPixels[]={\n')
 for i in range(0,len(pixels),96):f.write(','.join(map(str,pixels[i:i+96]))+',\n')
 f.write('};\n')
print('ORIGINAL_STORY_MENU_BC7_SHA256_OK',spec['sha256'])
