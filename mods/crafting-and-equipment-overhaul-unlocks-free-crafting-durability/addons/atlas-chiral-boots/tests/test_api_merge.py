"""Exercise the compiled runtime merger against real graphs and future-host fixtures."""
from pathlib import Path
import sys,ctypes as c,struct,hashlib,json,tempfile
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from build_api_payload import Graph,arrays,GAME,CACHE,OUT
DLL=c.CDLL(str(ROOT/'build/api/api_harness.dll'))
DLL.Merge.argtypes=[c.c_void_p,c.c_uint,c.c_void_p,c.c_uint,c.c_void_p,c.c_uint,c.POINTER(c.c_uint)]
DLL.Host198.argtypes=[c.c_void_p,c.c_uint,c.c_void_p,c.c_uint]
DLL.Cache.argtypes=[c.c_wchar_p,c.c_wchar_p]
DLL.RouteProbe.argtypes=[c.c_wchar_p,c.c_int]
DLL.Config.argtypes=[c.c_void_p,c.c_uint,c.POINTER(c.c_uint)]
DLL.Embedded.argtypes=[c.c_wchar_p,c.POINTER(c.c_void_p),c.POINTER(c.c_uint)]
checks=[]
def check(v,label):
 assert v,label
 checks.append(label)
def u(b,o=0):return struct.unpack_from('<I',b,o)[0]
def p(*v):return struct.pack('<'+'I'*len(v),*v)
def graph(b):
 with tempfile.NamedTemporaryFile(suffix='.core',delete=False) as f:f.write(b);name=f.name
 try:return Graph(name)
 finally:Path(name).unlink()
proof=json.loads((ROOT/'evidence/test15-install.json').read_text())
embedded={}
for name,digest in proof['payloadSHA256'].items():
 if not name.endswith('.bin'):continue
 ptr=c.c_void_p();n=c.c_uint()
 check(DLL.Embedded(name,c.byref(ptr),c.byref(n)),f'embedded payload available: {name}')
 embedded[name]=c.string_at(ptr,n.value)
 check(hashlib.sha256(embedded[name]).hexdigest()==digest,f'embedded bytes identical to confirmed Test15: {name}')
delta=embedded['graph-addon.bin']
for ini in [ROOT/'development/crafting-v198-addon-api/config/public-v198.ini', GAME/'ds2_crafting_unlocks.ini']:
 b=ini.read_bytes();line=c.c_uint();check(DLL.Config(b,len(b),c.byref(line)),f'1.9.8 parser accepts {ini.name}, ATLAS enabled (line {line.value})')
def merge(raw,payload=delta,expected=True,capacity=None):
 cap=capacity if capacity is not None else len(raw)+len(payload);out=c.create_string_buffer(cap);n=c.c_uint()
 ok=DLL.Merge(raw,len(raw),payload,len(payload),out,cap,c.byref(n))
 check(bool(ok)==expected,'merge accepted' if expected else 'merge refused')
 return out.raw[:n.value] if ok else None
vanilla=Graph(GAME/'LocalCacheWinGame/package/streaming_graph.core')
hostbuffer=c.create_string_buffer(len(vanilla.raw)+18932)
check(DLL.Host198(vanilla.raw,len(vanilla.raw),hostbuffer,len(hostbuffer)),'real 1.9.8 host graph built')
host=graph(hostbuffer.raw);output=graph(merge(host.raw));accepted=Graph(CACHE/'streaming_graph.core')
# Confirmed rendering data, excluding only the OLD host's subgroup arrays.
for key in output.parts:
 if key not in ['groups','subgroups']:
  check(output.parts[key]==accepted.parts[key],f'confirmed render data identical: {key}')
check(output.parts['subgroups']==host.parts['subgroups'],'1.9.8 host preloads preserved byte-for-byte')
owned={67696,31126,39987};gmap={r['id']:r for r in output.groups}
for gid in owned:
 r=gmap[gid];a=next(r for r in accepted.groups if r['id']==gid);n=68+r['uuidCount']*16
 check(output.parts['groups'][r['relative']:r['relative']+n]==accepted.parts['groups'][a['relative']:a['relative']+n],f'confirmed group record identical: {gid}')
for row in host.groups:
 if row['id'] in owned:continue
 n=gmap[row['id']];at=row['relative'];length=68+row['uuidCount']*16
 assert host.parts['groups'][at:at+length]==output.parts['groups'][n['relative']:n['relative']+length],row['id']
check(True,'all unrelated group records preserved')
(ROOT/'evidence/test16-merged-v198.core').write_bytes(output.raw)
# Append future host types, locators, spans, identities and preload membership.
future=graph(host.raw)
t=bytearray(future.parts['typeTable']);tc=u(t,12);t+=b'\x01\x00'*11
struct.pack_into('<I',t,0,len(t)-4);struct.pack_into('<II',t,12,tc+11,tc+11);future.parts['typeTable']=bytes(t)
for k,unit in [('locators',8),('spans',12),('typeHashes',8)]:
 b=future.parts[k];future.parts[k]=p(u(b)+1)+b[4:]+bytes(unit)
files=future.parts['files'];future.parts['files']=p(u(files)+1)+files[4:]+p(0)
for k in ['packOffsets','packLengths']:
 b=future.parts[k];future.parts[k]=p(u(b)+1)+b[4:]+p(0)
parent=next(r for r in future.groups if r['id']==499);subs=future.parts['subgroups'];start=u(subs)
members=subs[4+4*parent['subStart']:4+4*(parent['subStart']+parent['subCount'])]+p(8542)
future.parts['subgroups']=p(start+len(members)//4)+subs[4:]+members
rows=bytearray(future.parts['groups']);struct.pack_into('<II',rows,parent['relative']+16,start,parent['subCount']+1)
# Change an unrelated group, just as a future host resource update could.
unrelated=next(r for r in future.groups if r['id']==1);rows[unrelated['relative']+8]^=1;future.parts['groups']=bytes(rows)
newgroup=p(90000)+bytes(64)
future.parts['groups']=p(u(rows)+1)+bytes(rows[4:])+newgroup
reservations=future.parts['arrayTable'];future.parts['arrayTable']=p(u(reservations)+1)+reservations[4:]+p(0)
future=graph(future.bytes());merged=graph(merge(future.raw))
check(any(r['id']==90000 for r in merged.groups),'future additional resource group retained')
check(merged.parts['subgroups']==future.parts['subgroups'],'future preload list retained')
check(merged.parts['files']==future.parts['files'] and merged.parts['typeHashes']==future.parts['typeHashes'],'future file/type identities retained')
for key,basecount,unit in [('typeTable',tc,2),('locators',u(host.parts['locators']),8),('spans',u(host.parts['spans']),12)]:
 skip=24 if key=='typeTable' else 4
 check(merged.parts[key][skip:len(future.parts[key])]==future.parts[key][skip:],'future array data retained: '+key)
for gid in owned:
 a=next(r for r in output.groups if r['id']==gid);b=next(r for r in merged.groups if r['id']==gid)
 check(b['typeStart']==a['typeStart']+11 and b['locatorStart']==a['locatorStart']+1 and b['spanStart']==a['spanStart']+1,'rebased appended references '+str(gid))
bad=graph(host.raw);rows=bytearray(bad.parts['groups']);r=next(r for r in bad.groups if r['id']==31126);rows[r['relative']+8]^=1;bad.parts['groups']=bytes(rows)
merge(bad.bytes(),expected=False)
bad=bytearray(host.raw);struct.pack_into('<I',bad,8,1);merge(bytes(bad),expected=False)
for length in [0,4,32,100,len(delta)-1]:merge(host.raw,delta[:length],False)
merge(output.raw,expected=False) # duplicate application is a conflict, never doubled
merge(host.raw,expected=False,capacity=len(host.raw))
# A genuinely fresh directory with no external asset payloads. Keep all writes in build/.
case=Path(tempfile.mkdtemp(prefix='single-asi-',dir=ROOT/'build/api')).resolve()
cache=case/'Frischer Cache ü'
package=str(GAME/'LocalCacheWinGame/package')+'\\'
check(not cache.exists(),'fresh cache directory absent before initialization')
check(DLL.Cache(package,str(cache)+'\\'),'cache directory and full caches generated from embedded data only')
files=sorted(p.name for p in cache.iterdir())
check(files==['geometry-v1.cache','links-v1.cache'],'only two cache files created; no bin extraction required')
def filehash(path):
 with path.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
expected={'geometry-v1.cache':'14da2af2e3c86491b9a5909c8fa8740c6f93b46804eff52ac806d61668253753',
          'links-v1.cache':'8c3d7bc70e9eda0288f79e12cfc2da9448063ae35d38732962154be0c5f58ac7'}
for name,digest in expected.items():check(filehash(cache/name)==digest,'confirmed cache reconstructed byte-exact: '+name)
times={name:(cache/name).stat().st_mtime_ns for name in expected}
check(DLL.Cache(package,str(cache)+'\\'),'second launch reuses existing caches')
check(times=={name:(cache/name).stat().st_mtime_ns for name in expected},'valid cache files not rewritten')
# Legacy external files must never override embedded data, including during a rebuild.
for name in embedded:(cache/name).write_bytes(b'invalid obsolete external payload')
(cache/'links-v1.cache').write_bytes(b'damaged cache')
check(DLL.Cache(package,str(cache)+'\\'),'damaged cache rebuilt despite invalid legacy bin files')
check(filehash(cache/'links-v1.cache')==expected['links-v1.cache'],'repaired cache matches confirmed digest')
check((cache/'geometry-v1.cache').stat().st_mtime_ns==times['geometry-v1.cache'],'healthy geometry cache retained during repair')
check(DLL.RouteProbe('LocalCacheWinGame/package/package.42.02.core.stream',0),'game IAT + DirectStorage archive routing')
check(DLL.RouteProbe(str(GAME/'LocalCacheWinGame/package/streaming_links.stream'),1),'game IAT + DirectStorage links routing')
collision=case/'file-not-directory';collision.write_bytes(b'keep me')
check(not DLL.Cache(package,str(collision)+'\\'),'cache directory file collision refused')
check(collision.read_bytes()==b'keep me','colliding user file unchanged')
wrong=case/'wrong-originals';wrong.mkdir();(wrong/'package.42.02.core.stream').write_bytes(b'wrong original')
refused=case/'refused-cache'
check(not DLL.Cache(str(wrong)+'\\',str(refused)+'\\'),'modified original archive refused')
check(not list(refused.iterdir()),'no output archive after source validation failure')
# Exact known scratch files only; no recursive deletion or game-folder writes.
assert case.is_relative_to((ROOT/'build/api').resolve())
for name in list(expected)+list(embedded):(cache/name).unlink()
cache.rmdir();collision.unlink();(wrong/'package.42.02.core.stream').unlink();wrong.rmdir();refused.rmdir();case.rmdir()
report={'checks':len(checks),'passed':checks,'merged198sha':hashlib.sha256(output.raw).hexdigest(),'futureHostMergedSHA':hashlib.sha256(merged.raw).hexdigest()}
(ROOT/'evidence/test16-merge-tests.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
