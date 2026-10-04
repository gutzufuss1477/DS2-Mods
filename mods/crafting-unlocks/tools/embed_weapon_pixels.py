"""Validate and embed unchanged native pixels for privately owned red GPU views."""
from pathlib import Path
import hashlib
root=Path(__file__).resolve().parents[1]
assets=[
 ('bigbore-albedo.bc1','PistolPixels','pistol_pixels.inc',699064,'517319d1c6d53df4fc1cd8e3dd3c430f2b994db568e22e8be4fba7b789745420'),
 ('shotgun-icon.bc7','ShotgunIconPixels','shotgun_icon_pixels.inc',163840,'a86f3120e90b307746ed88b0fc8c3769c9927cfc50a505b1f8539b3ae1badb8b'),
 ('bigbore-icon.bc7','BigboreIconPixels','bigbore_icon_pixels.inc',163840,'d175202e1ce11337588cb46db06926ae09ae01320c7e1cdc6536f3bd53065f64'),
]
for file,symbol,out,size,expected in assets:
 data=(root/'assets'/file).read_bytes()
 assert len(data)==size, (file,len(data))
 digest=hashlib.sha256(data).hexdigest()
 assert digest==expected, (file,digest)
 (root/'build'/out).write_text('static const unsigned char '+symbol+'[]={\n'+'\n'.join(','.join(str(b) for b in data[i:i+64])+',' for i in range(0,len(data),64))+'\n};\n',encoding='ascii')
 print(file, 'SHA256:',digest)
