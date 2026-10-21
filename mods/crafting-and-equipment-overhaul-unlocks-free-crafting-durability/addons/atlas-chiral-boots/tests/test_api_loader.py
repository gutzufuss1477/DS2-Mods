from pathlib import Path
import ctypes as c,json,hashlib,argparse
ROOT=Path(__file__).resolve().parents[1]
class Host(c.Structure):
 _fields_=[('size',c.c_uint),('version',c.c_uint),('layout',c.c_uint),('id',c.c_uint),('high',c.c_ulonglong),('low',c.c_ulonglong)]
class Addon(c.Structure):
 _fields_=[('size',c.c_uint),('version',c.c_uint)]+[(n,c.c_void_p) for n in ['merge','release','commit','worn','hung','catalogue','icon']]
parser=argparse.ArgumentParser()
parser.add_argument('--host',type=Path,default=ROOT/'development/crafting-v198-addon-api/build/test15-api/ds2_crafting_unlocks.asi')
parser.add_argument('--addon',type=Path,default=ROOT/'release/test15-api/ds2_atlas_chiral_addon.asi')
parser.add_argument('--report',type=Path,default=ROOT/'evidence/test15-loader.json')
args=parser.parse_args()
host_path=args.host
addon_path=args.addon
k=c.WinDLL('kernel32',use_last_error=True)
k.LoadLibraryExW.argtypes=[c.c_wchar_p,c.c_void_p,c.c_uint];k.LoadLibraryExW.restype=c.c_void_p
k.GetProcAddress.argtypes=[c.c_void_p,c.c_char_p];k.GetProcAddress.restype=c.c_void_p
# Read ONLY the host's pure descriptor export without running its game worker.
h=k.LoadLibraryExW(str(host_path),None,1);assert h
ptr=k.GetProcAddress(h,b'DS2CraftingQueryAddonApi');assert ptr
query=c.CFUNCTYPE(c.POINTER(Host),c.c_uint)(ptr);host=query(1)
assert host and host.contents.id==103 and not query(2)
addon=c.CDLL(str(addon_path));addon.DS2CraftingAddonQueryV1.argtypes=[c.POINTER(Host),c.POINTER(Addon)]
api=Addon();api.size=c.sizeof(api);api.version=1
assert addon.DS2CraftingAddonQueryV1(host,c.byref(api))==1
assert all(getattr(api,n) for n in ['merge','release','commit','worn','hung','catalogue','icon'])
merge=c.CFUNCTYPE(c.c_int,c.c_void_p,c.c_uint,c.POINTER(c.c_void_p),c.POINTER(c.c_uint))(api.merge)
out=c.c_void_p();size=c.c_uint()
assert merge(None,0,c.byref(out),c.byref(size))==0 and not out and size.value==0
bad=Host.from_buffer_copy(host.contents);bad.version=2
assert addon.DS2CraftingAddonQueryV1(c.byref(bad),c.byref(api))==0
report={'actualHostDescriptorAccepted':True,'unknownAbiRefused':True,'nonGameProcessRefusedBeforeResourcesAndHooks':True,
        'hostSHA':hashlib.sha256(host_path.read_bytes()).hexdigest(),'addonSHA':hashlib.sha256(addon_path.read_bytes()).hexdigest()}
args.report.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
