// Sam Overhaul native TimefallShelter "Rest in Shelter" HUD label adjustment.
// Steam DS2.exe 1.10.89.0. Preserves EXACT native TakeABreakRainShelter
// action, normal rest logic and input IDs; changes ONLY two verified German
// localized text resources on their normal native streaming load event.
//
// The two resource UUIDs and memory layout below were independently verified
// in the live 2026-10-09 DS2 process, with the display successfully changed.
// Self-contained, no extra DLL/texture/localizer; no heap allocations.
// All other localization resources and languages remain unmodified.
 // Preloaded reconciliation reads bounded game heap chunks through the
 // kernel API; no additional runtime module, exception handling library
 // or unbounded memory dereferences are needed.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>

namespace sam_rest_label {
static uintptr_t gameBase=0;
static bool enabled=false;
static volatile LONG applied=0;
static volatile LONG scanBusy=0;
static volatile LONG editBusy=0;
static uintptr_t scanCursor=0x10000u;
static bool scanFinished=false;
static volatile LONG scanArmed=0;
static DWORD scanNotBefore=0u;
static unsigned completedPasses=0u;
static uint64_t totalScanBytes=0;
static constexpr uintptr_t kNativeTextVtable=0x03455CC0u;
static constexpr uint16_t kOldBytes=18u;
static constexpr uint16_t kNewBytes=12u;
static const char original[kOldBytes]={'I','n',' ','B','u','n','k','e','r',' ','a','u','s','r','u','h','e','n'};
static const char corrected[kNewBytes+1]={'V','e','r','s','c','h','n','a','u','f','e','n',0};
static const uint8_t uuidA[16]={
 0xFC,0x5D,0x04,0x6C,0x18,0x27,0x43,0x60,0x96,0xBB,0x10,0x49,0xDE,0xAE,0xEB,0xAC
};
static const uint8_t uuidB[16]={
 0xAD,0x51,0x6D,0xDE,0x5D,0x04,0x46,0x38,0x9F,0xF4,0xE3,0x6B,0xE4,0xEF,0x33,0x7B
};
static bool same(const void* one,const void* two,size_t count){
 const uint8_t* a=(const uint8_t*)one;
 const uint8_t* b=(const uint8_t*)two;
 if(!a || !b)return false;
 for(size_t i=0;i<count;++i)if(a[i]!=b[i])return false;
 return true;
}
static bool writable(const void* address,size_t length){
 if(!address || length==0)return false;
 const uintptr_t ptr=(uintptr_t)address;
 if(ptr<0x10000u || ptr>0x00007FFFFFFFFFFFULL)return false;
 MEMORY_BASIC_INFORMATION mbi={};
 if(!VirtualQuery(address,&mbi,sizeof(mbi)))return false;
 const uintptr_t start=(uintptr_t)mbi.BaseAddress;
 const uintptr_t end=start+mbi.RegionSize;
 if(end<start || end<ptr || length>end-ptr)return false;
 const DWORD protection=mbi.Protect & 0xffu;
 return mbi.State==MEM_COMMIT && !(mbi.Protect&PAGE_GUARD) &&
  (protection==PAGE_READWRITE || protection==PAGE_WRITECOPY ||
   protection==PAGE_EXECUTE_READWRITE || protection==PAGE_EXECUTE_WRITECOPY);
}
}

// Called once from the existing Sam Overhaul worker before native streaming
// listener installation. Default opt-in only if ShelterRange is enabled.
// FixRestPrompt=0 disables localized text modifications; legacy INIs without
// this key automatically get the fix when shelter extension is active.
extern "C" bool SamShelterRestLabelConfigure(void* moduleBase,const wchar_t* ini){
 if(!moduleBase || !ini)return false;
 const UINT shelterEnabled=GetPrivateProfileIntW(L"TimefallShelterRange",L"Enabled",0,ini);
 const UINT wanted=GetPrivateProfileIntW(L"TimefallShelterRange",L"FixRestPrompt",1,ini);
 sam_rest_label::gameBase=(uintptr_t)moduleBase;
 sam_rest_label::enabled=shelterEnabled!=0u && wanted!=0u;
 return sam_rest_label::enabled;
}
extern "C" bool SamShelterRestLabelEnabled(){
 return sam_rest_label::enabled;
}
extern "C" LONG SamShelterRestLabelChangedCount(){
 return InterlockedCompareExchange(&sam_rest_label::applied,0,0);
}

// Return 1 when changed, 2 when same resource is already corrected, 0 for
// unrelated/unexpected object. No writes if game pointer/vtable/UUID/text/
// memory protection don't match. Caller passes normal streamed native object.
extern "C" int SamShelterRestLabelOnStreamResource(void* object){
 using namespace sam_rest_label;
 if(!enabled || !gameBase || !object)return 0;
 const uintptr_t address=(uintptr_t)object;
 if(address<0x10000u || address>0x00007FFFFFFFFFFFULL)return 0;

 // Hot path for unrelated resources: a SINGLE vtable pointer comparison.
 if(*(const uintptr_t*)object!=gameBase+kNativeTextVtable)return 0;
 const uint8_t* bytes=(const uint8_t*)object;
 if(!same(bytes+0x10u,uuidA,16u) && !same(bytes+0x10u,uuidB,16u))return 0;

 // Verify actual DS2 LocalizedTextResource layout (original text and size).
 // The original buffer has >=18 bytes, so shorter wording fits in place,
 // preserving Decima's original memory ownership and object lifecycle.
 if(!writable(bytes+0x20u,10u))return 0;
 char* text=*(char* const*)(bytes+0x20u);
 const uint16_t length=*(const uint16_t*)(bytes+0x28u);
 if(length==kNewBytes && writable(text,kNewBytes) &&
    same(text,corrected,kNewBytes))return 2;
 if(length!=kOldBytes || !writable(text,kOldBytes))return 0;
 if(!same(text,original,kOldBytes))return 0;

 // Actual two game pointers and text lengths were verified live. Never
 // touch game input/action IDs or any global user-facing "Bunker" string.
 // Native streamer and preload scanner may run on different threads.
 if(InterlockedCompareExchange(&editBusy,1,0)!=0)return 0;
 if(*(const uint16_t*)(bytes+0x28u)!=kOldBytes ||
    !same(text,original,kOldBytes)){
  InterlockedExchange(&editBusy,0);return 0;
 }
 for(unsigned i=0;i<kNewBytes+1u;++i)text[i]=corrected[i];
 *(uint16_t*)(bytes+0x28u)=kNewBytes;
 InterlockedIncrement(&applied);
 InterlockedExchange(&editBusy,0);
 return 1;
}

// Start the preload reconciliation after the first game shelter actually
// exists, not at process initialization before save data has loaded.
extern "C" void SamShelterRestLabelNotifyActiveShelter(){
 using namespace sam_rest_label;
 if(!enabled || InterlockedCompareExchange(&applied,0,0)>=2)return;
 if(InterlockedCompareExchange(&scanArmed,1,0)!=0)return;
 scanCursor=0x10000u;
 scanFinished=false;
 completedPasses=0u;
 // Allow save-localized resources and HUD to finish loading.
 scanNotBefore=GetTickCount()+2000u;
}

// Some LocalizedTextResource instances are loaded BEFORE Sam's native
// streaming listener is registered. A one-shot, time-sliced discovery walk
// over existing writable private memory is needed to reconcile them.
// It scans only on the existing worker, beginning 2s after actual shelter
// creation (rather than before the save is loaded). Stops after both known
// GUIDs are patched or two bounded passes have completed.
// No per-frame hook or separate localization mod/process is introduced.
//
// Budget per 250ms worker tick: max 64MiB, max ~12ms wall time. This
// intentionally avoids reading 9GiB in a single blocking frame. Once both
// GUIDs have been seen, the scanner never runs again this process; the
// existing native streaming listener handles subsequent asset reloads.
extern "C" void SamShelterRestLabelPoll(HANDLE log){
 using namespace sam_rest_label;
 if(!enabled || !gameBase || scanFinished ||
    InterlockedCompareExchange(&scanArmed,0,0)==0 ||
    InterlockedCompareExchange(&applied,0,0)>=2)return;
 if((LONG)(GetTickCount()-scanNotBefore)<0)return;
 if(InterlockedCompareExchange(&scanBusy,1,0)!=0)return;

 const DWORD started=GetTickCount();
 static constexpr uintptr_t kMaxUser=0x00007FFFFFFFFFFFULL;
 static constexpr SIZE_T kMaxBytesPerPoll=64u*1024u*1024u;
 static constexpr DWORD kMaxMsPerPoll=12u;
 SIZE_T processed=0;
 unsigned regions=0;
 while(scanCursor<kMaxUser &&
       processed<kMaxBytesPerPoll &&
       regions<2048u &&
       (DWORD)(GetTickCount()-started)<kMaxMsPerPoll &&
       InterlockedCompareExchange(&applied,0,0)<2){
  MEMORY_BASIC_INFORMATION mbi={};
  if(!VirtualQuery((const void*)scanCursor,&mbi,sizeof(mbi))){
   scanFinished=true;break;
  }
  ++regions;
  const uintptr_t first=(uintptr_t)mbi.BaseAddress;
  const uintptr_t end=first+mbi.RegionSize;
  if(end<=scanCursor){scanFinished=true;break;}
  const DWORD p=mbi.Protect&0xffu;
  const bool candidate=mbi.State==MEM_COMMIT && mbi.Type==MEM_PRIVATE &&
   !(mbi.Protect&PAGE_GUARD) &&
   (p==PAGE_READWRITE || p==PAGE_WRITECOPY ||
    p==PAGE_EXECUTE_READWRITE || p==PAGE_EXECUTE_WRITECOPY);
  if(!candidate){
   scanCursor=end;
   continue;
  }
  uintptr_t limit=scanCursor+(uintptr_t)(kMaxBytesPerPoll-processed);
  if(limit<scanCursor || limit>end)limit=end;
  // Copy source bytes using ReadProcessMemory from our own process. If
  // the original game allocation is freed while scanning, the OS returns
  // a failed read, rather than causing an access violation/crash in DS2.
  // No SEH runtime dependency (__C_specific_handler) in no-CRT ASI.
  static unsigned char snapshot[1024u*1024u];
  const SIZE_T block=(limit-scanCursor)>sizeof(snapshot) ?
                     sizeof(snapshot) : (SIZE_T)(limit-scanCursor);
  SIZE_T got=0;
  const bool readOk=ReadProcessMemory(GetCurrentProcess(),
                           (const void*)scanCursor,snapshot,block,&got)!=0;
  if(readOk && got==block && block>=0x38u){
   // Native LocalizedTextResource allocation fields are 8-byte aligned.
   uintptr_t pos=(scanCursor+7u)&~(uintptr_t)7u;
   while(pos+0x38u<=scanCursor+block){
    uintptr_t vt=0;
    const SIZE_T offset=(SIZE_T)(pos-scanCursor);
    for(unsigned j=0;j<sizeof(uintptr_t);++j)
     ((unsigned char*)&vt)[j]=snapshot[offset+j];
    if(vt==gameBase+kNativeTextVtable &&
       writable((const void*)pos,0x38u)){
     // The exact type, UUID, UTF8 original and length are revalidated
     // by the production routine before writing any game-owned text.
     const int changed=SamShelterRestLabelOnStreamResource((void*)pos);
     if(changed==1 && log && log!=INVALID_HANDLE_VALUE){
      const char message[]="shelter_rest_label=PRELOADED_RESOURCE_PATCHED native_action_preserved\r\n";
      DWORD wrote=0;WriteFile(log,message,sizeof(message)-1u,&wrote,nullptr);
     }
     if(InterlockedCompareExchange(&applied,0,0)>=2)break;
    }
    pos+=8u;
   }
  }
  processed+=block;
  totalScanBytes+=(uint64_t)block;
  scanCursor+=block;
 }
 if(scanCursor>=kMaxUser)scanFinished=true;
 if(scanFinished){
  ++completedPasses;
  if(InterlockedCompareExchange(&applied,0,0)<2 && completedPasses<2u){
   // Retry once after a short delay in case text is streamed later than
   // the shelter itself. Never perform an unbounded/background full scan.
   scanCursor=0x10000u;
   scanFinished=false;
   scanNotBefore=GetTickCount()+6500u;
   if(log && log!=INVALID_HANDLE_VALUE){
    const char message[]="shelter_rest_label=PRELOAD_SECOND_PASS_SCHEDULED\r\n";
    DWORD wrote=0;WriteFile(log,message,sizeof(message)-1u,&wrote,nullptr);
   }
  }else{
   InterlockedExchange(&scanArmed,0);
   if(log && log!=INVALID_HANDLE_VALUE){
    const char message[]="shelter_rest_label=PRELOAD_SCAN_COMPLETE original_action_preserved\r\n";
    DWORD wrote=0;WriteFile(log,message,sizeof(message)-1u,&wrote,nullptr);
   }
  }
 }
 InterlockedExchange(&scanBusy,0);
}
