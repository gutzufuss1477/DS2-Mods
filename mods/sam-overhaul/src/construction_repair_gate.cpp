// Sam Overhaul DS2 1.10.89.0: scoped native RepairSpray contact gate.
// dev23 revision: correct E9+5 jump and use prevalidated active shelter sources.
// Unknown/dormant/unregistered sources retain original DS2 contact predicate.
// No direct baggage/coating writes or synthetic contact dispatch.
#include <windows.h>
#include <stdint.h>
#include "native_jump_rel32.h"
extern "C" {
struct SamGateRegistration { uintptr_t source,resource,owner; };
extern uintptr_t SamGateComponentVT;
extern uintptr_t SamGateResourceVT;
extern uintptr_t SamGateOwnerVT;
extern uint32_t SamGateRadiusBits;
extern volatile LONG SamGateRegistrationCount;
extern SamGateRegistration SamGateRegistrations[32];
extern uintptr_t SamGateContinue;
extern uintptr_t SamGateSkip;
void SamRepairGateStub();
}
static_assert(sizeof(SamGateRegistration)==24u,"MASM ABI expects 24-byte gate entries");
namespace shelter_repair_gate {
static uintptr_t image=0;
static uint8_t* relay=nullptr;
static bool installed=false;
static HANDLE registrationLog=INVALID_HANDLE_VALUE;
static volatile LONG registrationLogged=0;
static volatile LONG whitelistSpin=0;
// Registration is a live native object identity, NOT a real-time lease:
 // GetTickCount-based expiration during paused/background game caused dev23
 // to lose the repair enhancement after 13 seconds without native ticks.
static DWORD lastNativeRegistrySweep=0;
static volatile LONG nativeSourcesPruned=0;
static volatile LONG nativeRegistryConfirmed=0;
static constexpr unsigned kEntries=32u;
static constexpr uintptr_t kGate=0x011B0698u;
static constexpr uintptr_t kContinue=0x011B069Eu;
static constexpr uintptr_t kSkip=0x011B0763u;
static constexpr uintptr_t kImageSize=0x0B292000u;
static constexpr DWORD kPageSize=0x1000u;
static constexpr DWORD kAllocation=MEM_COMMIT|MEM_RESERVE;

static void note(HANDLE log,const char* message){
 if(!log || log==INVALID_HANDLE_VALUE || !message)return;
 DWORD len=0,written=0;while(message[len])++len;
 if(len)WriteFile(log,message,len,&written,nullptr);
}
static bool readable(uintptr_t ptr,SIZE_T bytes){
 if(ptr<0x10000 || ptr>0x00007FFFFFFFFFFFULL || !bytes)return false;
 MEMORY_BASIC_INFORMATION mbi={};
 if(!VirtualQuery((LPCVOID)ptr,&mbi,sizeof(mbi)))return false;
 const uintptr_t end=(uintptr_t)mbi.BaseAddress+mbi.RegionSize;
 return mbi.State==MEM_COMMIT && !(mbi.Protect&PAGE_GUARD) &&
        !(mbi.Protect&PAGE_NOACCESS) && end>=ptr && bytes<=end-ptr;
}
static bool equal(const void* left,const uint8_t* right,unsigned size){
 const uint8_t* mem=(const uint8_t*)left;
 for(unsigned i=0;i<size;++i)if(mem[i]!=right[i])return false;
 return true;
}
static bool withinJumpReach(uintptr_t site,uintptr_t target){
 uint8_t tmp[6]={};
 return native_jump_rel32::build(site,target,tmp);
}
static bool writeCode(uint8_t* dest,const uint8_t* src,unsigned bytes){
 DWORD old=0;
 if(!VirtualProtect(dest,bytes,PAGE_EXECUTE_READWRITE,&old))return false;
 for(unsigned i=0;i<bytes;++i)dest[i]=src[i];
 const bool cache=FlushInstructionCache(GetCurrentProcess(),dest,bytes)!=0;
 DWORD unused=0;
 const bool protectedAgain=VirtualProtect(dest,bytes,old,&unused)!=0;
 return equal(dest,src,bytes)&&cache&&protectedAgain;
}
static uint8_t* allocateNear(uintptr_t patch){
 // RX FF25 indirect relay near DS2, forwards to MASM in the ASI DLL.
 // build() checks EXACT 5-byte instruction length for reachable addresses.
 uintptr_t preferred=(image+kImageSize+0xffffu)&~(uintptr_t)0xffffu;
 for(unsigned i=0;i<1024u;++i){
  uintptr_t addr=preferred+(uintptr_t)i*0x10000u;
  if(!withinJumpReach(patch,addr))break;
  uint8_t* allocation=(uint8_t*)VirtualAlloc((void*)addr,kPageSize,kAllocation,PAGE_READWRITE);
  if(allocation)return allocation;
 }
 uintptr_t aligned=patch&~(uintptr_t)0xffffu;
 for(unsigned i=1;i<=1024u;++i){
  const uintptr_t offset=(uintptr_t)i*0x10000u;
  const uintptr_t candidates[]={aligned+offset,aligned>offset?aligned-offset:0u};
  for(unsigned j=0;j<2u;++j){
   const uintptr_t addr=candidates[j];
   if(!addr||!withinJumpReach(patch,addr))continue;
   uint8_t* allocation=(uint8_t*)VirtualAlloc((void*)addr,kPageSize,kAllocation,PAGE_READWRITE);
   if(allocation)return allocation;
  }
 }
 return nullptr;
}
static void writeEntrySource(unsigned slot,uintptr_t source){
 InterlockedExchange64((volatile LONG64*)&SamGateRegistrations[slot].source,(LONG64)source);
}
// DS2 native source list uses the SAME SRW shared lock as the original
// baggage query RVA 0x11B04F0: root+0x60, count+0x540, rows+0x548.
// Validate membership on the existing Sam worker (at most every 1.2s).
// During pause/alt-tab sources still in this list remain whitelisted.
// Scene unload and source replacement remove invalid entries.
static void pruneAgainstNativeSourceList(){
 if(!image || !readable(image+0x0623EAD8u,8u))return;
 const uintptr_t root=*(const uintptr_t*)(image+0x0623EAD8u);
 if(!readable(root+0x60u,8u) || !readable(root+0x540u,16u))return;
 PSRWLOCK nativeLock=(PSRWLOCK)(root+0x60u);
 if(!TryAcquireSRWLockShared(nativeLock))return; // never block
 const unsigned count=*(const unsigned*)(root+0x540u);
 const uintptr_t array=*(const uintptr_t*)(root+0x548u);
 const bool validTable=count<=512u &&
                       (count==0u || readable(array,(SIZE_T)count*8u));
 if(validTable){
  if(InterlockedCompareExchange(&nativeRegistryConfirmed,1,0)==0)
   note(registrationLog,"construction_repair_gate=NATIVE_LIST_LIVENESS_ACTIVE\r\n");
  const LONG published=InterlockedCompareExchange(&SamGateRegistrationCount,0,0);
  if(published>0 && published<=32){
   for(unsigned slot=0;slot<(unsigned)published;++slot){
    const SamGateRegistration& e=SamGateRegistrations[slot];
    const uintptr_t src=e.source;
    if(!src)continue;
    bool found=false;
    for(unsigned j=0;j<count;++j){
     if(*(const uintptr_t*)(array+(uintptr_t)j*8u)==src){found=true;break;}
    }
    bool matches=false;
    if(found && readable(src,0x78u) && readable(e.resource,0x24u) &&
       readable(e.owner,0xB0u)){
     const uintptr_t members=*(const uintptr_t*)(e.owner+0xA8u);
     matches=*(const uintptr_t*)src==SamGateComponentVT &&
       *(const uintptr_t*)(src+0x30u)==e.resource &&
       *(const uintptr_t*)(src+0x48u)==e.owner &&
       *(const uintptr_t*)e.resource==SamGateResourceVT &&
       *(const uint32_t*)(e.resource+0x20u)==SamGateRadiusBits &&
       *(const uintptr_t*)e.owner==SamGateOwnerVT &&
       readable(members+0x28u,16u) &&
       (*(const uintptr_t*)(members+0x28u)==src ||
        *(const uintptr_t*)(members+0x30u)==src);
    }
    if(!matches){
     // Publish source=0 before allowing reuse of the entry.
     writeEntrySource(slot,0);
     if(InterlockedIncrement(&nativeSourcesPruned)<=6)
      note(registrationLog,"construction_repair_gate=STALE_NATIVE_SOURCE_PRUNED\r\n");
    }
   }
  }
 }
 ReleaseSRWLockShared(nativeLock);
}
} // namespace
extern "C" void SamConstructionRepairGateRegister(void* source,void* resource,void* owner){
 using namespace shelter_repair_gate;
 if(!installed || !source || !resource || !owner)return;
 if(InterlockedCompareExchange(&whitelistSpin,1,0)!=0)return;
 const uintptr_t s=(uintptr_t)source, r=(uintptr_t)resource, o=(uintptr_t)owner;
 // A source is permitted only if it was independently validated by
 // reconcileNativeRepairRadius() in an ACTIVE owner-linked shelter update.
 // Update existing tuple only when source/res/owner all still match.
 int freeSlot=-1;
 for(unsigned i=0;i<kEntries;++i){
  const SamGateRegistration& entry=SamGateRegistrations[i];
  if(entry.source==s && entry.resource==r && entry.owner==o){
   InterlockedExchange(&whitelistSpin,0);
   return;
  }
  if(entry.source==0 && freeSlot<0)freeSlot=(int)i;
 }
 if(freeSlot<0){
  InterlockedExchange(&whitelistSpin,0);
  return; // bounded cache full: fail closed to vanilla gate
 }
 const unsigned slot=(unsigned)freeSlot;
 // Release publication order: secondary pointers first, then source.
 SamGateRegistrations[slot].resource=r;
 SamGateRegistrations[slot].owner=o;
 writeEntrySource(slot,s);
 const LONG old=InterlockedCompareExchange(&SamGateRegistrationCount,0,0);
 if(old<=(LONG)slot)InterlockedExchange(&SamGateRegistrationCount,(LONG)(slot+1u));
 InterlockedExchange(&whitelistSpin,0);
 if(InterlockedCompareExchange(&registrationLogged,1,0)==0)
  note(registrationLog,"construction_repair_gate=ACTIVE_SOURCE_REGISTERED\r\n");
}
extern "C" void SamConstructionRepairGatePoll(){
 using namespace shelter_repair_gate;
 if(!installed)return;
 const DWORD now=GetTickCount();
 if((DWORD)(now-lastNativeRegistrySweep)<1200u)return;
 if(InterlockedCompareExchange(&whitelistSpin,1,0)!=0)return;
 lastNativeRegistrySweep=now;
 pruneAgainstNativeSourceList();
 InterlockedExchange(&whitelistSpin,0);
}
extern "C" bool SamConstructionRepairGateInstall(void* moduleBase,const wchar_t* ini,HANDLE log){
 using namespace shelter_repair_gate;
 if(installed)return true;
 if(!moduleBase||!ini)return false;
 const UINT enabled=GetPrivateProfileIntW(L"TimefallShelterRange",L"Enabled",0,ini);
 const UINT percent=GetPrivateProfileIntW(L"TimefallShelterRange",L"RangePercent",200,ini);
 // The actual native repair radius can exceed the 2D visible/protection
 // circle slightly to compensate a sloped-terrain 3D distance check.
 const UINT repairPercent=GetPrivateProfileIntW(L"TimefallShelterRange",L"RepairRadiusPercent",215,ini);
 if(enabled!=1 || percent<=100u){
  note(log,"construction_repair_gate=DISABLED_OR_VANILLA\r\n");return true;
 }
 if(percent>400u || repairPercent<100u || repairPercent>400u){
  note(log,"construction_repair_gate=REFUSED_BAD_PERCENT\r\n");return false;
 }
 image=(uintptr_t)moduleBase;
 static const uint8_t previous[5]={0x41,0x80,0x7E,0x70,0x00};
 static const uint8_t original[6]={0x0F,0x84,0xC5,0x00,0x00,0x00};
 static const uint8_t next[4]={0x49,0x8B,0x76,0x48};
 uint8_t* site=(uint8_t*)(image+kGate);
 if(!readable((uintptr_t)(site-5),15u) ||
    !equal(site-5,previous,5u) || !equal(site,original,6u) ||
    !equal(site+6,next,4u)){
  note(log,"construction_repair_gate=REFUSED_NATIVE_SIGNATURE_MISMATCH\r\n");
  return false;
 }
 union FloatBits { float value;uint32_t bits; };
 FloatBits limit={};
 limit.value=4.0f*(float)repairPercent/100.0f;
 if(limit.value<4.01f || limit.value>16.01f){
  note(log,"construction_repair_gate=REFUSED_TARGET_OUT_OF_RANGE\r\n");return false;
 }
 SamGateComponentVT=image+0x03297208u;
 SamGateResourceVT=image+0x03296670u;
 SamGateOwnerVT=image+0x03119BC8u;
 SamGateRadiusBits=limit.bits;
 SamGateContinue=image+kContinue;
 SamGateSkip=image+kSkip;
 InterlockedExchange(&SamGateRegistrationCount,0);
 for(unsigned i=0;i<kEntries;++i)writeEntrySource(i,0);
 relay=allocateNear((uintptr_t)site);
 if(!relay){note(log,"construction_repair_gate=REFUSED_RELAY_ALLOCATION\r\n");return false;}
 const uintptr_t stub=(uintptr_t)&SamRepairGateStub;
 static const uint8_t header[6]={0xFF,0x25,0x00,0x00,0x00,0x00};
 for(unsigned i=0;i<6u;++i)relay[i]=header[i];
 for(unsigned i=0;i<8u;++i)relay[6u+i]=(uint8_t)(stub>>(i*8u));
 const bool flushed=FlushInstructionCache(GetCurrentProcess(),relay,14u)!=0;
 DWORD prior=0u;
 const bool executable=VirtualProtect(relay,kPageSize,PAGE_EXECUTE_READ,&prior)!=0;
 if(!flushed || !executable){
  note(log,"construction_repair_gate=REFUSED_RELAY_EXECUTION\r\n");
  VirtualFree(relay,0,MEM_RELEASE);relay=nullptr;return false;
 }
 uint8_t patch[6]={};
 if(!native_jump_rel32::build((uintptr_t)site,(uintptr_t)relay,patch) ||
    native_jump_rel32::decodeTarget((uintptr_t)site,patch)!=(uintptr_t)relay){
  note(log,"construction_repair_gate=REFUSED_E9_TARGET_MISMATCH\r\n");
  VirtualFree(relay,0,MEM_RELEASE);relay=nullptr;return false;
 }
 if(!writeCode(site,patch,6u)){
  if(equal(site,patch,6u))writeCode(site,original,6u);
  if(equal(site,original,6u)){VirtualFree(relay,0,MEM_RELEASE);relay=nullptr;}
  note(log,"construction_repair_gate=REFUSED_PATCH_OR_PROTECTION_FAILURE\r\n");
  return false;
 }
 registrationLog=log;
 installed=true;
 note(log,"construction_repair_gate=SCOPED_JUMP_TARGET_VERIFIED active_only_registered_sources\r\n");
 note(log,"construction_repair_gate=OTHER_SOURCES_USE_NATIVE_CONTACT_CHECK\r\n");
 return true;
}
