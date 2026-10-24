// DStorageCore read overlay: standalone, or chained after the optional unchanged
// ATLAS Chiral Boots v1.0.0 hook. Original files and saves are never written.
// Fails closed on any version, checksum, owner or native file mismatch.
#include "coffin_archive_path_scope.hpp"
namespace coffin_archive_storage {
using namespace craft;
using OpenW=HANDLE(WINAPI*)(const WCHAR*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
constexpr u64 ImportRva=0x58158ull;
constexpr u32 CoreTimestamp=0x34C782E3u;
constexpr u32 CoreImageSize=0x176000u;
OpenW originalOpenW=nullptr;
WCHAR corePath[2048]={},shoeModulePath[2048]={};
volatile long state=0,openCount=0,linksOpenCount=0,failedOpenCount=0,forwardCount=0;
struct MemoryBasic {
 void*base,*allocationBase;DWORD allocationProtect;
 unsigned short partitionId,padding;SIZE_T regionSize;
 DWORD state,protect,type;
};
static_assert(sizeof(MemoryBasic)==48,"Windows x64 VirtualQuery result");
bool safeRead(u64 a,void*p,u32 n){SIZE_T got=0;return a>0x10000ull&&ReadProcessMemory(GetCurrentProcess(),(const void*)a,p,n,&got)&&got==n;}
WCHAR lower(WCHAR a){return a>=L'A'&&a<=L'Z'?a+32:a;}
// Bounded to the exact published canonical package path, NOT l200_aus or
// another original DS2 region/localized archive with the same basename.
bool matchesOriginal(const WCHAR*name){
 return coffin_archive_path::matchesCanonical(name);
}
HANDLE WINAPI readOverlay(const WCHAR*name,DWORD access,DWORD share,void*sa,DWORD disposition,DWORD flags,HANDLE templ){
 if(!originalOpenW)return InvalidHandle;
 if(disposition==3&&!(access&0x40000000u)&&
    _InterlockedCompareExchange(&state,0,0)==2&&
    coffin_archive_cache::ready()){
  const bool archive=matchesOriginal(name);
  const bool links=coffin_archive_path::matchesCanonicalLinks(name);
  if(archive||links){
   const WCHAR* target=links?coffin_archive_cache::privateLinksPath:
                              coffin_archive_cache::privatePath;
   HANDLE result=originalOpenW(target,access,share,sa,disposition,flags,templ);
   if(result!=InvalidHandle){
    if(links)_InterlockedIncrement(&linksOpenCount);
    else _InterlockedIncrement(&openCount);
    return result;
   }
   // Never pass through an old file when the private graph references
   // offsets beyond its end. A missing private link cache fails closed.
   _InterlockedIncrement(&failedOpenCount);
   return InvalidHandle;
  }
 }
 _InterlockedIncrement(&forwardCount);
 return originalOpenW(name,access,share,sa,disposition,flags,templ);
}
// Accept only the OS import in standalone mode, or the checksum-verified
// addon's committed allocation in composition mode. Never replace unknown hooks.
bool trustedPrior(bool withAddon,u64 prior,void*owner,HMODULE shoe,u64 nativeBase,u64 native32){
 if(!prior||prior==(u64)&readOverlay)return false;
 if(withAddon)return shoe&&owner==shoe&&prior!=nativeBase&&prior!=native32;
 return !shoe&&((nativeBase&&prior==nativeBase)||(native32&&prior==native32));
}
bool install(bool withAddon) {
 const long prev=_InterlockedCompareExchange(&state,1,0);
 if(prev==2)return coffin_archive_cache::profile->addon==withAddon;
 if(prev==-1)return false;
 if(prev==1)return false; // no install while another thread is installing
 if(!coffin_archive_cache::prepare(withAddon)){
  _InterlockedExchange(&state,-1);return false;
 }
 HMODULE core=GetModuleHandleW(L"DStorageCore.dll");
 const auto*visual=withAddon?visual_archive::skeleton():nullptr;
 HMODULE shoe=GetModuleHandleW(visual?L"ds2_atlas_exoskeleton_addon.asi":L"ds2_atlas_chiral_addon.asi");
 u64 base=(u64)core;
 u32 pe=0;
 bool ok=core&&(!withAddon||shoe)&&
    safeRead(base+0x3C,&pe,4)&&pe>=0x40&&pe<=0x1000;
 u32 nt=0,timestamp=0,imagesize=0;
 if(ok)ok=safeRead(base+pe,&nt,4)&&nt==0x4550&&
    safeRead(base+pe+8,&timestamp,4)&&timestamp==CoreTimestamp&&
    safeRead(base+pe+0x50,&imagesize,4)&&imagesize==CoreImageSize;
 if(ok)ok=GetModuleFileNameW(core,corePath,2048)>0&&
    coffin_archive_cache::shaMatches(corePath,1451520ull,coffin_archive_cache::StorageCoreSha);
 if(ok&&withAddon&&!visual)ok=GetModuleFileNameW(shoe,shoeModulePath,2048)>0&&
    coffin_archive_cache::shaMatches(shoeModulePath,8018944ull,coffin_archive_cache::ShoeAddonSha);
 u64 prior=0;
 if(ok)ok=safeRead(base+ImportRva,&prior,8)&&prior>0x10000ull&&
    prior!=(u64)&readOverlay;
 if(ok&&visual)ok=prior==(u64)visual->nativeFileOpen; // Exact reported shim, not an arbitrary pointer in its module.
 MemoryBasic info={};
 if(ok)ok=VirtualQuery((void*)prior,&info,sizeof(info))==sizeof(info)&&
    info.state==0x1000&&trustedPrior(withAddon,prior,info.allocationBase,shoe,
      (u64)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CreateFileW"),
      (u64)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"CreateFileW"));
 if(ok){
  DWORD old=0;
  u64 slot=base+ImportRva;
  ok=VirtualProtect((void*)slot,8,4,&old)!=0;
  if(ok){
   originalOpenW=(OpenW)prior;
   const u64 actual=(u64)_InterlockedCompareExchange64(
    (volatile long long*)slot,(long long)(u64)&readOverlay,(long long)prior);
   ok=actual==prior;
   DWORD unused=0;
   if(!VirtualProtect((void*)slot,8,old,&unused)){
    if(ok)_InterlockedCompareExchange64((volatile long long*)slot,(long long)prior,(long long)(u64)&readOverlay);
    VirtualProtect((void*)slot,8,old,&unused);
    ok=false;
   }
  }
 }
 _InterlockedExchange(&state,ok?2:-1);
 return ok;
}
bool ready(){return _InterlockedCompareExchange(&state,0,0)==2&&coffin_archive_cache::ready();}
u32 served(){return (u32)_InterlockedCompareExchange(&openCount,0,0);}
u32 servedLinks(){return (u32)_InterlockedCompareExchange(&linksOpenCount,0,0);}
u32 failedOpens(){return (u32)_InterlockedCompareExchange(&failedOpenCount,0,0);}
u32 passthrough(){return (u32)_InterlockedCompareExchange(&forwardCount,0,0);}
} // namespace coffin_archive_storage
