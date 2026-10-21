from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def edit(name,fn):
 p=ROOT/name;p.write_text(fn(p.read_text()))
def overlay(s):
 a=s.index('bool verifyOverlay(){');b=s.index('void cacheDiagnostics()',a)
 s=s[:a]+'bool verifyOverlay(){return _InterlockedCompareExchange(&overlayState,0,0)==2;}\n'+s[b:]
 a=s.index('bool initializeHostCacheName(){');b=s.index('bool earlyOverlay(){',a)
 s=s[:a]+s[b:]
 s=s.replace('#include "cache_config.hpp"','#include "portable_cache.inl"').replace('HANDLE cachePins[3]={};','').replace('WCHAR hostCacheName[2048]={};','')
 s=s.replace(' if(hostCacheName[0]&&pathEqual(path,hostCacheName))return 1; // graph is manifest file 1\n','')
 s=s.replace('i<3;++i)if(pathEqual(path,CacheFiles','i<2;++i)if(pathEqual(path,CacheFiles').replace('for(u32 i=0;i<3;++i){\n  const WCHAR*p=CacheFiles','for(u32 i=0;i<2;++i){\n  const WCHAR*p=CacheFiles')
 s=s.replace('||!initializeHostCacheName()','')
 s=s.replace('index==1?"CHIRAL_REDIRECT_GRAPH":"CHIRAL_REDIRECT_LINKS"','"CHIRAL_REDIRECT_LINKS"')
 return s
edit('src/file_overlay.inl',overlay)
edit('src/storage_overlay.inl',lambda s:s.replace('index==1?"CHIRAL_DSTORAGE_GRAPH":"CHIRAL_DSTORAGE_LINKS"','"CHIRAL_DSTORAGE_LINKS"'))
edit('src/vendor/win_api.hpp',lambda s:s+'\nAPI BOOL WINAPI MoveFileExW(const WCHAR*,const WCHAR*,DWORD);\nAPI BOOL WINAPI DeleteFileW(const WCHAR*);\n')
edit('src/kernel32.def',lambda s:s+'\n MoveFileExW\n')
edit('src/menu_icon.inl',lambda s:s.replace('IconLookup chainIcon=nullptr;','').replace('void*(*chainCatalogue)(void*)=nullptr;','').replace('void* catalogueWithChiralIcon(void*p){','void* catalogueWithChiralIcon(void*p,crafting_addon::CatalogueNext chainCatalogue){').replace('u64 chiralCatalogueIcon(u64 catalogue,const char*const*name){','u64 chiralCatalogueIcon(u64 catalogue,const char*const*name,IconLookup chainIcon){'))
def addon(s):
 s=s.replace('#include "host_compatibility.hpp"','#include "crafting_addon_api.h"\n#include "graph_merge.hpp"')
 # sameDigest is kept independent of the removed whole-host SHA allowlist.
 s=s.replace('using namespace chiral;','using namespace chiral;\nbool sameDigest(const char*a,const char*b){if(!a||!b)return false;for(unsigned i=0;i<64;++i)if(!a[i]||a[i]!=b[i])return false;return a[64]==0&&b[64]==0;}')
 s=s.replace('void(*chainWorn)(void*)=nullptr;void(*chainHung)(void*)=nullptr;','')
 s=s.replace('void worn(void*p){','void worn(void*p,crafting_addon::MeshNext chainWorn){').replace('void hung(void*p){','void hung(void*p,crafting_addon::MeshNext chainHung){')
 a=s.index('Patch patches[5]');b=s.index('struct Thread ',a)
 s=s[:a]+'''Patch patches[1]={};void* relay=nullptr;
bool prepare(){
 const u8 prologue[]={0x48,0x83,0xec,0x38,0x48,0x8b,0x41,0x60};u8 prior[8];
 if(!readmem(image+VisibilityRva,prior,8)||!same(prior,prologue,8))return false;
 patches[0].address=image+VisibilityRva;copy(patches[0].before,prior,5);
 u64 center=(image+0x1000000)&~u64(0xffff);
 for(u64 distance=0x10000;distance<0x60000000&&!relay;distance+=0x10000){
  relay=VirtualAlloc((void*)(center+distance),0x10000,0x3000,4);
  if(!relay&&center>distance+0x10000)relay=VirtualAlloc((void*)(center-distance),0x10000,0x3000,4);
 }
 if(!relay)return false;
 u8*stub=(u8*)relay;const u8 abs[]={0xff,0x25,0,0,0,0};copy(stub,abs,6);at<u64>(stub,6)=u64(&visibility);
 u8*next=stub+32;copy(next,prior,8);copy(next+8,abs,6);at<u64>(next,14)=image+VisibilityRva+8;
 chainVisibility=(void(*)(u64,i32,u8))next;setControllerVisibility=(void(*)(u64,u8))(image+0x3478E0);
 if(!relativeJump(patches[0].after,patches[0].address,u64(stub)))return false;
 DWORD old=0;return VirtualProtect(relay,0x10000,0x20,&old)&&FlushInstructionCache(GetCurrentProcess(),relay,0x10000);
}
''' + s[b:]
 a=s.index('[[maybe_unused]] DWORD WINAPI worker');s=s[:a]+'''
int mergeCurrentGraph(const u8*src,u32 size,u8**out,u32*outSize){
 *out=nullptr;*outSize=0;openLog();log("ATLAS Chiral Test15 | Crafting ABI v1 | portable semantic graph merge");
 char digest[65];
 if(!overlayInstalled||!moduleDigest(nullptr,digest)||!sameDigest(digest,GameSHA)){
  log("CHIRAL_ABI_REFUSED: game identity or early file hooks unavailable.");return 0;}
 if(storageState!=2&&!installStorageCore()){log("CHIRAL_ABI_REFUSED: DirectStorage unavailable.");return 0;}
 if(!preparePortable()){log("CHIRAL_ABI_REFUSED: original/cache/payload validation failed.");return 0;}
 WCHAR path[2048];u8*delta=nullptr;u32 deltaSize=0;
 if(!pathJoin(path,payloadFolder,L"graph-addon.bin")||!hashFile(path,digest)||!sameDigest(digest,GraphPayloadSHA)||!loadSmall(path,&delta,&deltaSize)){
  log("CHIRAL_ABI_REFUSED: graph payload missing or modified.");return 0;}
 u32 capacity=size+deltaSize;u8*merged=(u8*)VirtualAlloc(nullptr,capacity,0x3000,4);u32 length=0;
 bool ok=merged&&chiral_graph::merge(src,size,delta,deltaSize,merged,capacity,length);VirtualFree(delta,0,0x8000);
 if(!ok){if(merged)VirtualFree(merged,0,0x8000);log("CHIRAL_GRAPH_REFUSED: owned groups conflict; current host graph retained.");return 0;}
 if(!prepare()){VirtualFree(merged,0,0x8000);log("CHIRAL_VISIBILITY_REFUSED: native signature unavailable.");return 0;}
 int result=0;for(u32 i=0;i<20&&!result;++i){result=install();if(!result)Sleep(50);}
 if(result!=1){VirtualFree(merged,0,0x8000);log(result==2?"CHIRAL_CRITICAL: native visibility rollback/thread resume failure.":"CHIRAL_VISIBILITY_REFUSED: native hook not installed.");return 0;}
 *out=merged;*outSize=length;log("CHIRAL_GRAPH_MERGED: three owned groups appended to CURRENT host graph; host preload list preserved.");return 1;
}
void releaseGraph(u8*p){if(p)VirtualFree(p,0,0x8000);}
void commitGraph(int success){_InterlockedExchange(&overlayState,success?2:-1);
 log(success?"CHIRAL_API1_READY: graph committed; worn, hanging and menu callbacks active.":"CHIRAL_API1_INACTIVE: host graph not committed; native appearance retained.");}
}
extern "C" __declspec(dllexport) int DS2CraftingAddonQueryV1(const crafting_addon::HostV1*host,crafting_addon::AddonV1*out){
 using namespace crafting_addon;
 if(!validHost(host)||!out||out->size!=sizeof(AddonV1)||out->version!=Version)return 0;
 *out={sizeof(AddonV1),Version,&mergeCurrentGraph,&releaseGraph,&commitGraph,&worn,&hung,&catalogueWithChiralIcon,&chiralCatalogueIcon};return 1;
}
#ifndef CHIRAL_SELF_TEST
extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,void*){
 if(reason==1){selfModule=module;DisableThreadLibraryCalls(module);if(earlyOverlay()&&!earlyStorage())overlayInstalled=false;}
 return 1;
}
#endif
'''
 s=s.replace('// Test14: rendered Chiral menu image; confirmed Test13 hooks and Test11 cache.','// Test15: versioned host ABI; current graph merge; portable cache. Confirmed visuals retained.')
 return s
edit('src/addon.cpp',addon)
print('Addon API dispatch and portable cache wired.')
