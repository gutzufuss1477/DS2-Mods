from pathlib import Path
import ctypes as c, hashlib, json, subprocess, sys, os, argparse
R=Path(__file__).resolve().parents[1]
HOST=Path(os.environ.get('ATLAS_TEST_HOST','__supply_host__'))
BOOT=Path(os.environ.get('ATLAS_TEST_BOOT',str(R.parent/'atlas-chiral-boots/release/ds2_atlas_chiral_addon.asi')))
REPORT=R/'build/combination-checks'
class Host(c.Structure):
    _fields_=[('size',c.c_uint),('version',c.c_uint),('layout',c.c_uint),('id',c.c_uint),('high',c.c_uint64),('low',c.c_uint64)]
class Addon(c.Structure):
    _fields_=[('size',c.c_uint),('version',c.c_uint)]+[(n,c.c_void_p) for n in ['merge','release','commit','worn','hung','catalogue','icon']]
class Peer(c.Structure):
    _fields_=[('size',c.c_uint),('version',c.c_uint),('layout',c.c_uint),('id',c.c_uint),('asset',c.c_char_p)]+[(n,c.c_void_p) for n in ['shim','prepare','sync','passive']]
def digest(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def child(mode,order,version):
    k=c.WinDLL('kernel32',use_last_error=True)
    k.LoadLibraryExW.argtypes=[c.c_wchar_p,c.c_void_p,c.c_uint];k.LoadLibraryExW.restype=c.c_void_p
    k.GetProcAddress.argtypes=[c.c_void_p,c.c_char_p];k.GetProcAddress.restype=c.c_void_p
    main=HOST
    host_module=k.LoadLibraryExW(str(main),None,1);assert host_module
    query=c.CFUNCTYPE(c.POINTER(Host),c.c_uint)(k.GetProcAddress(host_module,b'DS2CraftingQueryAddonApi'))
    host=query(1);assert host and host.contents.size==32 and host.contents.id==103 and not query(2)
    assert c.CFUNCTYPE(c.c_uint)(k.GetProcAddress(host_module,b'DS2CraftingAddonRouterV1'))()==1
    boot={
        'new':BOOT,
        'legacy':Path(os.environ.get('ATLAS_TEST_LEGACY','__none__')),
        'test17':Path(os.environ.get('ATLAS_TEST_REJECTED','__none__')),
    }[version]
    paths={'boots':boot,'exo':R/'release/ds2_atlas_exoskeleton_addon.asi'}
    modules={}
    for name in order.split(','):
        if mode in (name,'both'): modules[name]=c.CDLL(str(paths[name]))
    harness=c.CDLL(str(R/'build/companion-harness.dll'))
    assert harness.Selected()=={'none':0,'boots':1,'exo':2,'both':2}[mode]
    if mode in ('exo','both'):
        expected=-1 if mode=='both' and version=='test17' else (3 if mode=='both' else 2)
        assert harness.Mode()==expected,(mode,version,'mode')
        assert harness.HasPeer()==(mode=='both' and version=='new')
    peer=None
    if 'boots' in modules and version=='new':
        q=modules['boots'].DS2AtlasBootCompanionQueryV1;q.argtypes=[c.c_uint];q.restype=c.POINTER(Peer)
        assert not q(0) and not q(2)
        peer=q(1).contents
        assert peer.size==56 and peer.version==1 and peer.layout==host.contents.layout and peer.id==103
        assert peer.asset==b'769f7357c42e38f8fcb5a273c0c7a5ffbd939e2f6bf7fc72c256961d7afeefe2'
        assert c.CFUNCTYPE(c.c_int)(peer.passive)()==1
        harness.AcceptStorage.argtypes=[c.c_void_p]
        # Exercise the exact production whitelist used by the DirectStorage shim.
        assert harness.AcceptStorage(peer.shim)==1
        assert harness.AcceptStorage(peer.prepare)==0
        assert harness.AcceptStorage(c.c_void_p(1))==0
        if mode=='both': assert harness.PreparePeer()==0 # refuses non-game EXE before native calls
    descriptors={}
    for name,module in modules.items():
        module.DS2CraftingAddonQueryV1.argtypes=[c.POINTER(Host),c.POINTER(Addon)]
        api=Addon(size=64,version=1);assert module.DS2CraftingAddonQueryV1(host,c.byref(api))==1
        descriptors[name]=api
        assert all(getattr(api,n) for n in ['merge','release','commit','worn','hung','catalogue','icon'])
        merge=c.CFUNCTYPE(c.c_int,c.c_void_p,c.c_uint,c.POINTER(c.c_void_p),c.POINTER(c.c_uint))(api.merge)
        out=c.c_void_p();size=c.c_uint();assert merge(None,0,c.byref(out),c.byref(size))==0 and not out and size.value==0
        wrong=Host.from_buffer_copy(host.contents);wrong.version=2
        assert module.DS2CraftingAddonQueryV1(c.byref(wrong),c.byref(api))==0
    if peer:
        c.CFUNCTYPE(None,c.c_int)(descriptors['boots'].commit)(1)
        assert c.CFUNCTYPE(c.c_int)(peer.passive)()==0
        assert harness.AcceptStorage(peer.shim)==0 # never permit two active cache owners
        if mode=='both': assert harness.Mode()==-1
    print(json.dumps({'mode':mode,'order':order,'bootsVersion':version,'passed':True}))
def main():
    cases=[('none','boots,exo','new'),('boots','boots,exo','new'),('exo','boots,exo','new')]
    cases += [('both',order,version) for version in ['new']+(['legacy'] if os.environ.get('ATLAS_TEST_LEGACY') else [])+(['test17'] if os.environ.get('ATLAS_TEST_REJECTED') else []) for order in ('boots,exo','exo,boots')]
    REPORT.mkdir(parents=True,exist_ok=True)
    results=[]
    for case in cases:
        p=subprocess.run([sys.executable,__file__,'child',*case],capture_output=True,text=True)
        (REPORT/('matrix-'+'-'.join(case).replace(',','_')+'.txt')).write_text(p.stdout+p.stderr)
        assert p.returncode==0,(case,p.returncode,p.stdout,p.stderr)
        results.append(json.loads(p.stdout));print(p.stdout.strip(),flush=True)
    report={'cases':results,'gameStarted':False,'scope':'Real DLL discovery, production peer negotiation/DirectStorage acceptance, both load orders; simulated native materials tested separately. No in-game rendering claim.',
            'hostSHA':digest(HOST),'bootsSHA':digest(BOOT),'exoSHA':digest(R/'release/ds2_atlas_exoskeleton_addon.asi')}
    (REPORT/'matrix.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
    if len(sys.argv)>1 and sys.argv[1]=='child':child(*sys.argv[2:])
    else:
        p=argparse.ArgumentParser();p.add_argument('--host',required=True);p.add_argument('--boots',default=str(BOOT));p.add_argument('--legacy-boots');p.add_argument('--incompatible-boots')
        a=p.parse_args();os.environ['ATLAS_TEST_HOST']=a.host;os.environ['ATLAS_TEST_BOOT']=a.boots
        if a.legacy_boots:os.environ['ATLAS_TEST_LEGACY']=a.legacy_boots
        if a.incompatible_boots:os.environ['ATLAS_TEST_REJECTED']=a.incompatible_boots
        HOST=Path(a.host);BOOT=Path(a.boots);main()
