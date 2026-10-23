"""Embed validated private 2048x2048 BC1 12-mip red Coffinboard color texture."""
from pathlib import Path
import hashlib,struct
root=Path(__file__).resolve().parents[1]
path=root/'assets/coffinboard-red-albedo.bc1'
raw=path.read_bytes()
expected=sum(max(1,((max(1,2048>>i)+3)//4))**2*8 for i in range(12))
assert len(raw)==expected,(len(raw),expected)
assert hashlib.sha256(raw).hexdigest()=='9c50df008b1500e829dba525b4fa03eb49cc12daf93cbd0f1d10031c69ee593d'
out=root/'build/coffin_red_pixels.inc'
out.parent.mkdir(parents=True,exist_ok=True)
with out.open('w',encoding='ascii',newline='\n') as f:
 f.write('static const unsigned char CoffinRedPixels[]={\n')
 for i in range(0,len(raw),96):
  f.write(','.join(str(b) for b in raw[i:i+96])+',\n')
 f.write('};\n')
print('COFFIN_CUSTOM_RED_BC1_EMBEDDED',len(raw),'MIP_COUNT=12','SHA256=',hashlib.sha256(raw).hexdigest())
