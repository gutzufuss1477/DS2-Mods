// Generator-instance reconciliation for Sam Overhaul (experimental).
// Unlike TriggerComponent initialization, DSCharger receives per-instance
// updates after loading; use these to discover late-bound generator triggers.
// Game build v1.10.89.0 (exact signature guards); no global Jolt patches.
#include <windows.h>
#include <stdint.h>
#include "MinHook.h"

extern "C" void SamConstructionRangesScaleLate(void*);
extern "C" bool SamConstructionRefreshQueue(void*);
extern "C" bool SamConstructionRefreshEnabled();
extern "C" unsigned SamConstructionRangesGeneratorPercent();
extern "C" bool SamConstructionGeneratorVisualSync(void*);

namespace charger_update {
static uintptr_t base=0;
static HANDLE logger=INVALID_HANDLE_VALUE;
static volatile LONG updateCalls=0;
static volatile LONG matchedGenerator=0;
static volatile LONG activeLateTriggers=0;
static volatile LONG lateQueueAccepted=0, visualSynced=0;
static volatile LONG throttle[4096]={};
static bool hookUpdate=false,hookScan=false;
typedef void (__fastcall*Update)(void*,float);
typedef void (__fastcall*Scan)(void*);
static Update originalUpdate=nullptr;
static Scan originalScan=nullptr;

static bool readable(uintptr_t p,SIZE_T n){
 if(p<0x10000||p>0x00007FFFFFFFFFFFULL||!n)return false;
 MEMORY_BASIC_INFORMATION mbi={};
 if(!VirtualQuery((const void*)p,&mbi,sizeof(mbi)))return false;
 const uintptr_t end=(uintptr_t)mbi.BaseAddress+mbi.RegionSize;
 return mbi.State==MEM_COMMIT && !(mbi.Protect&PAGE_GUARD) &&
  ((mbi.Protect&0xff)!=PAGE_NOACCESS) && end>=p && n<=end-p;
}
static uintptr_t ptr(uintptr_t p,unsigned off=0){
 const uintptr_t at=p+(uintptr_t)off;
 return readable(at,8)?*(const uintptr_t*)at:0;
}
static void say(const char* msg){
 if(logger==INVALID_HANDLE_VALUE||!msg)return;
 DWORD n=0,w=0;while(msg[n])++n;
 if(n)WriteFile(logger,msg,n,&w,nullptr);
}
static bool hasSignature(uintptr_t address,const unsigned char* sig,unsigned bytes){
 if(!readable(address,bytes))return false;
 for(unsigned i=0;i<bytes;++i)if(((const unsigned char*)address)[i]!=sig[i])return false;
 return true;
}
static void reconcile(void* context){
 if(!SamConstructionRefreshEnabled()||!context)return;
 const uintptr_t charger=(uintptr_t)context;
 if(ptr(charger)!=base+0x03284108)return;
 LONG first=InterlockedIncrement(&matchedGenerator);
 if(first==1)say("construction_charger_update=CHARGER_INSTANCE_SEEN\r\n");
 // Limit a charger to one inspection per ~700 ms even if its engine update runs every frame.
 const unsigned slot=(unsigned)(((charger>>8)^(charger>>20))&4095u);
 const DWORD now=GetTickCount();
 const DWORD last=(DWORD)InterlockedExchange(&throttle[slot],(LONG)now);
 if(last && (DWORD)(now-last)<700u)return;
 const uintptr_t wrapper=ptr(charger,0xa0);
 const uintptr_t owner=ptr(wrapper,0x88);
 const uintptr_t memberArray=ptr(owner,0xa8);
 if(!readable(memberArray,0x100))return;

 bool ownedGenerator=false;
 for(unsigned i=0;i<0x100;i+=8){
  const uintptr_t component=ptr(memberArray,i);
  const uintptr_t vt=ptr(component);
  if(vt==base+0x03295030||vt==base+0x03295348){ownedGenerator=true;break;}
 }
 if(!ownedGenerator)return;

 // In observed save sessions, the active TriggerComponent is at +0xb8.
 // Validate its vtable, otherwise search the bounded owner component array.
 uintptr_t trigger=ptr(memberArray,0xb8);
 uintptr_t vt=ptr(trigger);
 if(vt!=base+0x03135648 && vt!=base+0x031360E8){
  trigger=0;
  for(unsigned i=0;i<0x100;i+=8){
   uintptr_t item=ptr(memberArray,i);
   vt=ptr(item);
   if(vt==base+0x03135648||vt==base+0x031360E8){trigger=item;break;}
  }
 }
 if(!trigger)return;
 if(InterlockedIncrement(&activeLateTriggers)==1)
  say("construction_charger_update=GENERATOR_TRIGGER_FOUND\r\n");
 // The sphere may already have been registered at its vanilla radius.
 // Scale it once, then queue the engine's own NotifyShapeChanged.
 SamConstructionRangesScaleLate((void*)trigger);
 SamConstructionGeneratorVisualSync((void*)trigger);
 if(SamConstructionRefreshQueue((void*)trigger)){
  if(InterlockedIncrement(&lateQueueAccepted)<=12)
   say("construction_charger_update=LATE_BODY_REFRESH_QUEUED\r\n");
 }
}

// The generator's visible blue ring is driven by the DSConstructionOdradekEffect
// component and its resource, not by the transient LineEffectComponent float.
// A live test (2026-10-08) confirmed that both +0x5c and resource+0x34 must
// match the physical Jolt trigger radius (9->18, 12->24) even after save reload.
extern "C" bool SamConstructionGeneratorVisualSync(void* triggerObj) {
 using namespace charger_update;
 if(!SamConstructionRefreshEnabled() || !triggerObj || !base)return false;
 const uintptr_t trg=(uintptr_t)triggerObj;
 if(ptr(trg)!=base+0x03135648 && ptr(trg)!=base+0x031360E8)return false;
 const uintptr_t owner=ptr(trg,0x48);
 const uintptr_t members=ptr(owner,0xa8);
 if(!readable(members,0x100))return false;
 bool hasGenerator=false;
 for(unsigned i=0;i<0x100;i+=8){
  const uintptr_t c=ptr(members,i);
  if(ptr(c)==base+0x03295030 || ptr(c)==base+0x03295348){
   hasGenerator=true;break;
  }
 }
 if(!hasGenerator)return false;
 const uintptr_t odradek=ptr(members,0x98);
 if(ptr(odradek)!=base+0x03296A68 || ptr(odradek,0x48)!=owner)return false;
 const uintptr_t effectRes=ptr(odradek,0x30);
 if(ptr(effectRes)!=base+0x032972E8)return false;
 const uintptr_t resource=ptr(trg,0x30);
 if(ptr(resource)!=base+0x03136180)return false;
 const uintptr_t coll=ptr(resource,0x20);
 if(ptr(coll)!=base+0x03413EF0)return false;
 const uintptr_t simple=ptr(coll,0xb0);
 if(ptr(simple)!=base+0x03414EE8)return false;
 const uintptr_t sphere=ptr(simple,0x20);
 if(ptr(sphere)!=base+0x03414868 || !readable(sphere+0x30,4))return false;
 const unsigned percent=SamConstructionRangesGeneratorPercent();
 if(percent<=100||percent>400)return false;
 const float physical=*(const float*)(sphere+0x30);
 const uintptr_t instanceField=odradek+0x5cu;
 const uintptr_t resourceField=effectRes+0x34u;
 if(!readable(instanceField,4)||!readable(resourceField,4))return false;
 const float vanillaRadii[]={9.0f,12.0f,15.0f};
 for(unsigned i=0;i<3;++i){
  const float vanilla=vanillaRadii[i];
  const float target=vanilla*(float)percent/100.0f;
  if(!(physical>target-0.06f && physical<target+0.06f))continue;

  // Scanner and pending worker may run concurrently. Lock the pair so neither
  // can double-scale an already patched shared resource.
  static volatile LONG visualSpin=0;
  while(InterlockedCompareExchange(&visualSpin,1,0)!=0)Sleep(0);

  const float inst=*(const float*)instanceField;
  const float res=*(const float*)resourceField;
  const bool instanceDone=inst>target-0.06f && inst<target+0.06f;
  const bool resourceDone=res>target-0.06f && res<target+0.06f;
  const bool instanceVanilla=inst>vanilla-0.06f && inst<vanilla+0.06f;
  const bool resourceVanilla=res>vanilla-0.06f && res<vanilla+0.06f;
  if((!instanceDone&&!instanceVanilla)||(!resourceDone&&!resourceVanilla)){
   InterlockedExchange(&visualSpin,0);return false;
  }
  const uintptr_t fields[]={instanceField,resourceField};
  const bool done[]={instanceDone,resourceDone};
  for(unsigned j=0;j<2;++j){
   if(done[j])continue;
   MEMORY_BASIC_INFORMATION mbi={};
   if(!VirtualQuery((void*)fields[j],&mbi,sizeof(mbi))){
    InterlockedExchange(&visualSpin,0);return false;
   }
   const DWORD access=mbi.Protect&0xff;
   if(mbi.State!=MEM_COMMIT || (mbi.Protect&PAGE_GUARD) ||
      mbi.Type!=MEM_PRIVATE ||
      !(access==PAGE_READWRITE || access==PAGE_WRITECOPY ||
        access==PAGE_EXECUTE_READWRITE || access==PAGE_EXECUTE_WRITECOPY)){
    InterlockedExchange(&visualSpin,0);return false;
   }
  }
  if(!instanceDone)*(volatile float*)instanceField=target;
  if(!resourceDone)*(volatile float*)resourceField=target;
  const float afterInstance=*(const float*)instanceField;
  const float afterResource=*(const float*)resourceField;
  const bool ok=(afterInstance>target-0.06f && afterInstance<target+0.06f &&
                 afterResource>target-0.06f && afterResource<target+0.06f);
  InterlockedExchange(&visualSpin,0);
  if(ok&&(!instanceDone||!resourceDone)){
   if(InterlockedIncrement(&visualSynced)<=20)
    say("construction_generator_visual=ODRADEK_COMPONENT_AND_RESOURCE_SYNCED\r\n");
  }
  return ok;
 }
 return false;
}

static void __fastcall hookedUpdate(void* o,float elapsed){
 InterlockedIncrement(&updateCalls);
 if(originalUpdate)originalUpdate(o,elapsed);
 reconcile(o);
}
static void __fastcall hookedScan(void* o){
 if(originalScan)originalScan(o);
 reconcile(o);
}
}
extern "C" bool SamConstructionChargerUpdateInstall(void* image,HANDLE log){
 using namespace charger_update;
 if(!image||!SamConstructionRefreshEnabled())return false;
 base=(uintptr_t)image;
 logger=log;
 const unsigned char u[]={0x40,0x53,0x48,0x83,0xec,0x30,0xc5,0xf8,0x29,0x74,0x24,0x20};
 const unsigned char s[]={0x48,0x8b,0xc4,0x41,0x57,0x48,0x81,0xec,0xe0,0x00,0x00,0x00};
 const uintptr_t update=base+0x0131A4F0;
 const uintptr_t scan=base+0x0131A840;
 if(hasSignature(update,u,sizeof(u))){
  MH_STATUS v=MH_CreateHook((void*)update,(void*)&hookedUpdate,(void**)&originalUpdate);
  if(v==MH_OK){
   v=MH_EnableHook((void*)update);
   if(v==MH_OK){hookUpdate=true;say("construction_charger_update=UPDATE_HOOK_INSTALLED\r\n");}
   else MH_RemoveHook((void*)update);
  }
 }
 if(hasSignature(scan,s,sizeof(s))){
  MH_STATUS v=MH_CreateHook((void*)scan,(void*)&hookedScan,(void**)&originalScan);
  if(v==MH_OK){
   v=MH_EnableHook((void*)scan);
   if(v==MH_OK){hookScan=true;say("construction_charger_update=SCAN_HOOK_INSTALLED\r\n");}
   else MH_RemoveHook((void*)scan);
  }
 }
 if(!hookUpdate&&!hookScan)say("construction_charger_update=WARNING_NO_UPDATE_HOOK\r\n");
 return hookUpdate||hookScan;
}
