from pathlib import Path
import shutil
ROOT=Path(__file__).resolve().parents[1]
HOST=ROOT/'development/crafting-v198-addon-api'
def change(p,a,b):
    p=HOST/p;s=p.read_text();assert a in s,(p,a);p.write_text(s.replace(a,b))
shutil.copy2(ROOT/'src/crafting_addon_api.h',HOST/'src/crafting_addon_api.h')
change('src/mod.cpp','#include "virtual_graph_runtime.inl"','#include "addon_host.inl"\n#include "virtual_graph_runtime.inl"')
change('src/atlas/runtime.cpp','#include "../atlas_integration.hpp"','#include "../atlas_integration.hpp"\n#include "../addon_host.hpp"')
change('src/atlas/atlas_runtime.inl','((void(*)(void*))wornMeshOriginal)(p);','addon_host::worn(p,(void(*)(void*))wornMeshOriginal);')
change('src/atlas/atlas_runtime.inl','((void(*)(void*))hangingMeshOriginal)(p);','addon_host::hanging(p,(void(*)(void*))hangingMeshOriginal);')
change('src/weapons/runtime.cpp','#include "../silenced_integration.hpp"','#include "../silenced_integration.hpp"\n#include "../addon_host.hpp"')
change('src/weapons/runtime.cpp','registerWeapons();return original(p);','registerWeapons();return addon_host::catalogue(p,original);')
change('src/weapons/runtime.cpp','u64 catalogueIcon(u64 catalogue,const char*const*name){','u64 catalogueIcon(u64 catalogue,const char*const*name);\nu64 catalogueIconDispatch(u64 catalogue,const char*const*name){return addon_host::icon(catalogue,name,&catalogueIcon);}\nu64 catalogueIcon(u64 catalogue,const char*const*name){')
change('src/weapons/runtime.cpp','u64(&catalogueIcon)','u64(&catalogueIconDispatch)')
# Snapshot presence during prepare(), so settings do not determine addon icon support.
change('src/weapons/runtime.cpp','bool blackRed=true;','bool addonIconRequested=false;\nbool blackRed=true;')
p=HOST/'src/weapons/runtime.cpp';s=p.read_text();pivot=s.index('bool prepareHook(')
s=s[:pivot]+s[pivot:].replace('blackRed||recipeEnabled[GuitarVariant]','blackRed||recipeEnabled[GuitarVariant]||addonIconRequested')
s=s.replace('bool prepareHook(){','bool prepareHook(){\n addonIconRequested=addon_host::requested();');p.write_text(s)
change('src/virtual_graph_runtime.inl',
 'if(ok)ok=writeExact(virtualPath,out,PatchedFileSize);',
 'u8*addonGraph=nullptr;u32 addonSize=0;\n if(ok)addon_host::merge(out,PatchedFileSize,&addonGraph,&addonSize);\n if(ok)ok=writeExact(virtualPath,addonGraph?addonGraph:out,addonGraph?addonSize:PatchedFileSize);\n addon_host::finish(ok,addonGraph);')
change('src/version.hpp','"1.9.8"','"1.9.8-addon-api1-test15"')
# API routines required by the main TU (submodule headers already import these).
p=HOST/'src/win_api.hpp';s=p.read_text()
for line in ['API void* WINAPI GetProcAddress(HMODULE,const char*);','API BOOL WINAPI GetModuleHandleExW(DWORD,const WCHAR*,HMODULE*);']:
    if line.split(' WINAPI ')[1].split('(')[0] not in s:s+='\n'+line+'\n'
p.write_text(s)
p=HOST/'src/kernel32.def';s=p.read_text()
for name in ['GetProcAddress','GetModuleHandleExW']:
    if name not in s:s+='\n'+name+'\n'
p.write_text(s)
print('Host 1.9.8: ABI v1 and dispatch integrated; release baseline untouched.')
