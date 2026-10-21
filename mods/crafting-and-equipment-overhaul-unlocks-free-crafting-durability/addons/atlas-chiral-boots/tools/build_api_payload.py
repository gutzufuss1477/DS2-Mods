"""Extract only Chiral-owned data from confirmed Test11. Never embed host preloads."""
from pathlib import Path
import sys, struct, hashlib, json, shutil
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT.parents[1]/'scripts'))
from atlas_graph import Graph
GAME=Path(r'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH')
CACHE=ROOT/'staging/test11-hanging-shader/cache/6616c0a3be60872c60e5e00844fda9212164604c3257f08fa6fbccc9b5535e41'
BUNDLE=ROOT/'staging/test11-hanging-shader/bundle'
OUT=ROOT/'release/test15-api/atlas_chiral'
def sha(b): return hashlib.sha256(b).digest()
def u32(b,o=0): return struct.unpack_from('<I',b,o)[0]
def pack(*v): return struct.pack('<'+'I'*len(v),*v)
def arrays(g):
    b=g.parts['arrayTable']; p=4; result={}
    for row in g.groups:
        n=u32(b,p); length=4+12*n; result[row['id']]=(p,b[p:p+length]); p+=length
    assert p==len(b)
    return result
def main():
    OUT.mkdir(parents=True,exist_ok=True)
    a=Graph(GAME/'LocalCacheWinGame/package/streaming_graph.core')
    b=Graph(CACHE/'streaming_graph.core'); aa,ba=arrays(a),arrays(b)
    keys=['typeTable','locators','spans']
    tails=[b.parts[k][len(a.parts[k]):] for k in keys]
    counts=[u32(a.parts['typeTable'],12),u32(a.parts['locators']),u32(a.parts['spans'])]
    # Fingerprint stable native type/file identities, not the changing host graph.
    data=pack(0x31444743,1,*counts,u32(a.parts['linkSize']),u32(b.parts['linkSize']))
    for k in ['typeHashes','files']:
        data+=pack(len(a.parts[k]),u32(a.parts[k]))+sha(a.parts[k])
    for tail in tails: data+=pack(len(tail))+tail
    data+=pack(3)
    for gid in [67696,31126,39987]:
        old=next(r for r in a.groups if r['id']==gid); new=next(r for r in b.groups if r['id']==gid)
        oldrow=a.parts['groups'][old['relative']:old['relative']+64]
        newrow=b.parts['groups'][new['relative']:new['relative']+64]
        oldarr,newarr=aa[gid][1],ba[gid][1]; assert len(oldarr)==len(newarr)
        types=a.parts['typeTable'][24+old['typeStart']*2:24+(old['typeStart']+old['typeCount'])*2]
        loc=a.parts['locators'][4+old['locatorStart']*8:4+(old['locatorStart']+old['locatorCount'])*8]
        data+=oldrow+newrow+sha(types)+sha(loc)+pack(len(oldarr))+oldarr+newarr
    (OUT/'graph-addon.bin').write_bytes(data)
    manifest=json.loads((BUNDLE/'overlay.json').read_text())
    for name in ['geometry-table.bin','geometry-append.bin','links-delta.bin']: shutil.copy2(BUNDLE/name,OUT/name)
    # Constants contain relative names and cryptographic identities only.
    files=[manifest['files'][0],manifest['files'][2]]
    lines=['#pragma once','struct PayloadSegment {const wchar_t* name; unsigned long long offset,length; const char* hash;};',
           'struct PortableFile {const wchar_t* name; const char* originalHash; const char* resultHash; unsigned long long resultSize; unsigned count; PayloadSegment segments[3];};',
           'constexpr PortableFile PortableFiles[]={']
    for f in files:
        segs=[]
        for s in f['segments']:
            segs.append('{'+(f'L"{s["payload"]}"' if 'payload' in s else 'nullptr')+f',{s.get("original",s.get("offset",0))}ull,{s["length"]}ull,'+(f'"{s["sha256"]}"' if 'sha256' in s else 'nullptr')+'}')
        lines.append('{L"'+Path(f['gamePath']).name+'","'+f['originalSha256']+'","'+f['resultSha256']+'",'+str(f['resultSize'])+'ull,'+str(len(segs))+',{'+','.join(segs)+'}},')
    lines+=['};',f'constexpr char GraphPayloadSHA[]="{sha(data).hex()}";']
    (ROOT/'src/portable_manifest.hpp').write_text('\n'.join(lines)+'\n')
    report={'schema':1,'baseline':'Crafting 1.9.8','ownedGroups':[67696,31126,39987],
            'excludedHostGroup':499,'payloads':{p.name:{'size':p.stat().st_size,'sha256':sha(p.read_bytes()).hex()} for p in OUT.glob('*.bin')}}
    (ROOT/'evidence/test15-payload.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__': main()
