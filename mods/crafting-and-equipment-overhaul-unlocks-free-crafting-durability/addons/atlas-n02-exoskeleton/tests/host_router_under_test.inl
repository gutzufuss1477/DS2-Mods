namespace addon_host {using namespace crafting_addon;}
namespace addon_host {
const HostV1 descriptor={sizeof(HostV1),Version,NativeLayout,103,0xB7AE21D89446A581ull,0x4A634CBDCE790267ull};
AddonV1 callbacks={};volatile long active=0;
// A single active owner prevents two independent archive/visibility hooks.
// The skeleton runtime optionally handles the unchanged published boot visuals.
// Old payload-only skeleton prototypes have no API query and are ignored here.
HMODULE preferredModule(){
 HMODULE exo=GetModuleHandleW(L"ds2_atlas_exoskeleton_addon.asi");
 if(exo&&GetProcAddress(exo,"DS2CraftingAddonQueryV1"))return exo;
 return GetModuleHandleW(L"ds2_atlas_chiral_addon.asi");
}
bool requested(){return preferredModule()!=nullptr;}
bool merge(const Byte*source,U32 length,Byte**result,U32*size){
 *result=nullptr;*size=0;
 HMODULE addon=preferredModule();if(!addon)return false;
 auto query=(QueryAddon)GetProcAddress(addon,"DS2CraftingAddonQueryV1");if(!query)return false;
 AddonV1 candidate={};candidate.size=sizeof(candidate);candidate.version=Version;
 if(!query(&descriptor,&candidate)||candidate.size!=sizeof(candidate)||candidate.version!=Version||
    !candidate.mergeGraph||!candidate.releaseGraph||!candidate.graphCommitted||!candidate.worn||
    !candidate.hanging||!candidate.catalogue||!candidate.icon)return false;
 HMODULE pinned=nullptr;
 if(!GetModuleHandleExW(5,(const WCHAR*)addon,&pinned)||
    !GetModuleHandleExW(5,(const WCHAR*)&merge,&pinned))return false;
 callbacks=candidate;
 bool ok=callbacks.mergeGraph(source,length,result,size)!=0;
 if(!ok||!*result||*size<length||*size>128*1024*1024){
  callbacks.graphCommitted(0);if(*result)callbacks.releaseGraph(*result);*result=nullptr;*size=0;return false;
 }
 return true;
}
void finish(bool success,Byte*data){
 if(!data)return;
 callbacks.graphCommitted(success?1:0);
 _InterlockedExchange(&active,success?1:0);
 callbacks.releaseGraph(data);
}
void worn(void*p,MeshNext next){if(_InterlockedCompareExchange(&active,0,0))callbacks.worn(p,next);else next(p);}
void hanging(void*p,MeshNext next){if(_InterlockedCompareExchange(&active,0,0))callbacks.hanging(p,next);else next(p);}
void* catalogue(void*p,CatalogueNext next){return _InterlockedCompareExchange(&active,0,0)?callbacks.catalogue(p,next):next(p);}
U64 icon(U64 c,const char*const*n,IconNext next){return _InterlockedCompareExchange(&active,0,0)?callbacks.icon(c,n,next):next(c,n);}
}
extern "C" __declspec(dllexport) const crafting_addon::HostV1* DS2CraftingQueryAddonApi(unsigned version){
 return version==crafting_addon::Version?&addon_host::descriptor:nullptr;
}

// Stable additive capability. Never change the frozen HostV1/AddonV1 layouts.
extern "C" __declspec(dllexport) unsigned DS2CraftingAddonRouterV1(){return 1;}
