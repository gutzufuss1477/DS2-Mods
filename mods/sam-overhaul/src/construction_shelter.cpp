// DS2 Sam Overhaul: experimental guarded Timefall Shelter protected-radius expansion.
// Targets the loaded shelter's two RotatedTranslatedShape(CylinderShape) trigger bodies.
// All addresses and vtables are for DS2.exe Steam v1.10.89.0 only.
#include <windows.h>
#include <stdint.h>
#include "MinHook.h"
#include "shelter_visual_policy.h"
#include "shelter_steady_state.h"
extern "C" void SamConstructionRepairGateRegister(void*,void*,void*);
extern "C" void SamShelterRestLabelNotifyActiveShelter();
namespace shelter_range {
static uintptr_t base=0;
static HANDLE logger=INVALID_HANDLE_VALUE;
static bool enabled=false,installed=false,spatialDiagnostics=false;
static unsigned percent=200;
static unsigned repairPercent=215;
static volatile LONG spin=0,firstSeen=0,firstQueue=0,completed=0,skipped=0;
typedef void (__fastcall* ShelterTick)(void*);
static ShelterTick original=nullptr;
struct Job {
 uintptr_t shelter,owner,trigger,world,outer,cylinder,body;
 uint32_t id;
 DWORD queuedAt;
};
static Job jobs[256]={};
static unsigned head=0,tail=0;
struct Done {uintptr_t world,cylinder,body;uint32_t id;};
static Done done[2048]={};
static unsigned doneCount=0;
struct Throttle {uintptr_t object;DWORD last,fallbackAt;};
static Throttle throttled[1024]={};
// Cache native ownership, Jolt BodyIDs and visual geometry only after all
// required objects have been independently validated on the full path.
struct VisualSync {
 shelter_steady_state::Key key;
 Job validated[2];
 unsigned triggerSlot[2];
 unsigned odradekSlot;
 unsigned repairSlot;
 DWORD lastGood,lastRegister;
 bool stable;
};
static VisualSync visualSync[256]={};

static bool readable(uintptr_t p,SIZE_T n,bool wr=false){
 if(p<0x10000||p>0x00007FFFFFFFFFFFULL||!n)return false;
 MEMORY_BASIC_INFORMATION m={};
 if(!VirtualQuery((void*)p,&m,sizeof(m)))return false;
 const uintptr_t end=(uintptr_t)m.BaseAddress+m.RegionSize;
 const DWORD access=m.Protect&0xff;
 if(m.State!=MEM_COMMIT || (m.Protect&PAGE_GUARD) ||
    access==PAGE_NOACCESS || p<((uintptr_t)m.BaseAddress) || end<p || n>end-p)return false;
 if(wr && (m.Type!=MEM_PRIVATE ||
  !(access==PAGE_READWRITE || access==PAGE_WRITECOPY ||
    access==PAGE_EXECUTE_READWRITE || access==PAGE_EXECUTE_WRITECOPY)))return false;
 return true;
}
static uintptr_t ptr(uintptr_t p,unsigned o=0){
 uintptr_t a=p+(uintptr_t)o;
 return readable(a,8)?*(const uintptr_t*)a:0;
}
static uint32_t u32(uintptr_t p,unsigned o=0){
 uintptr_t a=p+(uintptr_t)o;
 return readable(a,4)?*(const uint32_t*)a:0xffffffffu;
}
static float getf(uintptr_t p,unsigned o=0){
 uintptr_t a=p+(uintptr_t)o;
 return readable(a,4)?*(const float*)a:-1.0f;
}
static void note(const char *msg){
 if(!msg||logger==INVALID_HANDLE_VALUE)return;
 DWORD n=0,w=0;while(msg[n])++n;
 if(n)WriteFile(logger,msg,n,&w,nullptr);
}
static void lock(){while(InterlockedCompareExchange(&spin,1,0)!=0)Sleep(0);}
static void unlock(){InterlockedExchange(&spin,0);}
static bool approximate(float a,float b){
 return a>b-0.06f && a<b+0.06f;
}
static bool isShelter(uintptr_t object) {
 uintptr_t vt=ptr(object);
 return vt==base+0x032851C8 || vt==base+0x03280968;
}
// DS2's actual rain-protection radius is DS Rain Shelter ConstructionConfig:
// reflected property "Range" at +0x760, baseline 4m. In-game live 4->8 m
// restored protection from timefall beyond the vanilla circle and enabled the
// "rest in shelter" action at the larger distance (2026-10-08).
//
// Unlike temporary effect and Jolt scalars, this config is native gameplay data
// and can be created/reloaded independently. Resolve it via the LIVE shelter's
// category bytes exactly as DS2::DSRainShelter native code does. Never hardcode
// a heap address, specific world/category index, or replace weather logic.
static volatile LONG rangeSyncBusy=0;
static DWORD rangeCheckTick=0;
static volatile LONG rangeUpdatedCount=0;
static volatile LONG rangeSkippedCount=0;
static void reconcileNativeShelterRange(uintptr_t shelter){
 if(!enabled || !base || !shelter)return;
 const DWORD now=GetTickCount();
 // No heap/vtable query on the vast majority of native shelter ticks.
 if((DWORD)(now-rangeCheckTick)<750u)return;
 if(!isShelter(shelter))return;
 if(InterlockedCompareExchange(&rangeSyncBusy,1,0)!=0)return;
 rangeCheckTick=now;
 const uintptr_t root=ptr(base+0x0623EAD8);
 const uintptr_t manager=ptr(root,0x28);
 if(!manager || !readable(shelter+0x28,2)){
  InterlockedExchange(&rangeSyncBusy,0);return;
 }
 const unsigned kind=((const unsigned char*)shelter)[0x28];
 const unsigned group=((const unsigned char*)shelter)[0x29];
 const unsigned slot=(group==0x0eu)?(0x20u+kind*8u):(0x138u+group*8u);
 // Use only the bounded native table slots from the DS2 decompile.
 if(slot>0x300u){
  InterlockedExchange(&rangeSyncBusy,0);return;
 }
 const uintptr_t entry=ptr(manager,slot);
 const uintptr_t config=ptr(entry,0x30);
 if(ptr(config)!=base+0x03291EC0){
  if(InterlockedIncrement(&rangeSkippedCount)<=2)
   note("construction_shelter=RANGE_CONFIG_NOT_READY\r\n");
  InterlockedExchange(&rangeSyncBusy,0);return;
 }
 const float current=getf(config,0x760);
 const float target=4.0f*(float)percent/100.0f;
 if(approximate(current,4.0f)&&readable(config+0x760,4,true)){
  *(volatile float*)(config+0x760)=target;
  if(approximate(getf(config,0x760),target) &&
     InterlockedIncrement(&rangeUpdatedCount)<=16)
   note("construction_shelter=NATIVE_CONFIG_RANGE_SYNCED\r\n");
 }
 // If already equal to target, do nothing. Unknown values ALWAYS fail open;
 // do not double-scale a shared asset, and do not alter likes/weather factors.
 InterlockedExchange(&rangeSyncBusy,0);
}
// DSConstructionRepairSprayComponentResource at +0x20 contains the actual
// cargo-coating repair coverage radius. 4.0 -> 8.0m was confirmed live:
// at a verified 4.98m the native cloud appeared and cargo recovered to 100%.
// DS2's native DSBaggageComponent repair calculation consumes this property.
// Do not synthesize native contact events or modify individual cargo.
static volatile LONG repairRangeBusy=0;
static volatile LONG repairRangeUpdates=0;
static void reconcileNativeRepairRadius(uintptr_t shelter,uintptr_t owner,
                                        uintptr_t members){
 if(!enabled || !base || !isShelter(shelter))return;
 if(ptr(ptr(shelter,0xa0),0x88)!=owner ||
    ptr(owner)!=base+0x03119BC8)return;
 if(!readable(members,0x38))return;
 if(InterlockedCompareExchange(&repairRangeBusy,1,0)!=0)return;
 // Confirmed DS2 shelter has its RepairSpray in members+0x28. A second
 // layout can use +0x30, but only accept exact component+owner identity.
 uintptr_t repair=ptr(members,0x28);
 if(ptr(repair)!=base+0x03297208 || ptr(repair,0x48)!=owner)
  repair=ptr(members,0x30);
 if(ptr(repair)!=base+0x03297208 || ptr(repair,0x48)!=owner){
  InterlockedExchange(&repairRangeBusy,0);return;
 }
 const uintptr_t resource=ptr(repair,0x30);
 if(ptr(resource)!=base+0x03296670){
  InterlockedExchange(&repairRangeBusy,0);return;
 }
 const float current=getf(resource,0x20);
 // Independent repair compensation on slopes: native per-cargo checks
 // use a 3D sphere while the Odradek visual circle follows the ground.
 // Default 8.6m coating against 8.0m visual/protective shelter radius.
 const float target=4.0f*(float)repairPercent/100.0f;
 if(approximate(current,4.0f) && readable(resource+0x20,4,true)){
  *(volatile float*)(resource+0x20)=target;
  if(approximate(getf(resource,0x20),target) &&
     InterlockedIncrement(&repairRangeUpdates)<=12)
   note("construction_shelter=REPAIR_RADIUS_NATIVE_SYNCED\r\n");
 }
 // Register an expanded repair source only after owner, membership, component,
 // resource and native target radius were independently verified by this
 // active shelter update. The native per-baggage hook itself never scans
 // arbitrary pointers during save loading/unloading.
 if(approximate(getf(resource,0x20),target)){
  SamConstructionRepairGateRegister((void*)repair,(void*)resource,(void*)owner);
 }
 // Other RepairSpray properties are unmodified (effectiveness, jetting time).
 // Repeat calls are idempotent; save reload/new instances auto-resynchronize.
 InterlockedExchange(&repairRangeBusy,0);
}

// Scoped performance timing is limited to the 1600ms active-shelter
// discovery path. It excludes vanilla engine update and avoids high-frequency
// QueryPerformanceCounter calls. Records number/maximum/slow callbacks only.
static LONGLONG perfFrequency=0;
static volatile LONG perfCount=0,perfMaxUs=0,perfOver2Ms=0,perfOver8Ms=0;
static volatile LONG perfFallbackCount=0;
static volatile LONG perfSteadyCount=0,perfSteadyMaxUs=0,perfFullMaxUs=0;
static DWORD perfLastLog=0;
static void perfPeak(volatile LONG* location,LONG current){
 LONG previous=InterlockedCompareExchange(location,0,0);
 while(current>previous){
  const LONG exchanged=InterlockedCompareExchange(location,current,previous);
  if(exchanged==previous)break;
  previous=exchanged;
 }
}
struct DiscoveryMeter {
 LARGE_INTEGER before;
 bool measured,steady;
 DiscoveryMeter():measured(false),steady(false){
  if(perfFrequency>0){
   QueryPerformanceCounter(&before);
   measured=true;
  }
 }
 ~DiscoveryMeter(){
  if(!measured)return;
  LARGE_INTEGER after={};
  QueryPerformanceCounter(&after);
  const LONGLONG delta=after.QuadPart-before.QuadPart;
  if(delta<0 || delta>perfFrequency)return;
  const LONG us=(LONG)(delta*1000000LL/perfFrequency);
  InterlockedIncrement(&perfCount);
  if(us>2000)InterlockedIncrement(&perfOver2Ms);
  if(us>8000)InterlockedIncrement(&perfOver8Ms);
  perfPeak(&perfMaxUs,us);
  if(steady){
   InterlockedIncrement(&perfSteadyCount);
   perfPeak(&perfSteadyMaxUs,us);
  } else perfPeak(&perfFullMaxUs,us);
 }
};
static unsigned addDigits(char* dst,unsigned n,unsigned v){
 char reversed[15];unsigned count=0;
 if(v==0)reversed[count++]='0';
 while(v && count<15u){reversed[count++]=(char)('0'+v%10u);v/=10u;}
 while(count && n<185u)dst[n++]=reversed[--count];
 return n;
}
static unsigned addLiteral(char* dst,unsigned n,const char* msg){
 while(*msg && n<185u)dst[n++]=*msg++;
 return n;
}
static void logDiscoveryPerformance(){
 if(!perfFrequency||!enabled)return;
 const DWORD now=GetTickCount();
 if((DWORD)(now-perfLastLog)<15000u)return;
 perfLastLog=now;
 const LONG num=InterlockedExchange(&perfCount,0);
 const LONG maximum=InterlockedExchange(&perfMaxUs,0);
 const LONG over2=InterlockedExchange(&perfOver2Ms,0);
 const LONG over8=InterlockedExchange(&perfOver8Ms,0);
 const LONG fallbacks=InterlockedExchange(&perfFallbackCount,0);
 const LONG steady=InterlockedExchange(&perfSteadyCount,0);
 const LONG steadyMax=InterlockedExchange(&perfSteadyMaxUs,0);
 const LONG fullMax=InterlockedExchange(&perfFullMaxUs,0);
 if(num==0 && fallbacks==0)return;
 char line[200];unsigned n=0;
 n=addLiteral(line,n,"construction_shelter=PERF_DISCOVER_TOTAL count=");
 n=addDigits(line,n,(unsigned)num);
 n=addLiteral(line,n," max_us=");
 n=addDigits(line,n,(unsigned)maximum);
 n=addLiteral(line,n," over2ms=");
 n=addDigits(line,n,(unsigned)over2);
 n=addLiteral(line,n," over8ms=");
 n=addDigits(line,n,(unsigned)over8);
 n=addLiteral(line,n," fallbacks=");
 n=addDigits(line,n,(unsigned)fallbacks);
 n=addLiteral(line,n," stable=");
 n=addDigits(line,n,(unsigned)steady);
 n=addLiteral(line,n," stableMaxUs=");
 n=addDigits(line,n,(unsigned)steadyMax);
 n=addLiteral(line,n," fullMaxUs=");
 n=addDigits(line,n,(unsigned)fullMax);
 n=addLiteral(line,n,"\r\n");
 line[n]=0;note(line);
}

static bool inspect(uintptr_t shelter,uintptr_t owner,uintptr_t trigger,Job& job) {
 if(!isShelter(shelter))return false;
 const uintptr_t wrapper=ptr(shelter,0xa0);
 if(ptr(wrapper,0x88)!=owner)return false;
 if(ptr(trigger)!=base+0x03135648 && ptr(trigger)!=base+0x031360E8)return false;
 if(ptr(trigger,0x48)!=owner)return false;
 uintptr_t resource=ptr(trigger,0x30);
 if(ptr(resource)!=base+0x03136180)return false;
 uintptr_t collision=ptr(resource,0x20);
 if(ptr(collision)!=base+0x03413EF0)return false;
 uintptr_t simple=ptr(collision,0xb0);
 if(ptr(simple)!=base+0x03414EE8)return false;
 uintptr_t outer=ptr(simple,0x20);
 if(ptr(outer)!=base+0x0345B768)return false;
 uintptr_t cylinder=ptr(outer,0x20);
 if(ptr(cylinder)!=base+0x0345CA98)return false;
 // Native JPH CylinderShape::GetLocalBounds (RVA 0x278D790) proves
 // +0x30 is Y-axis HALF-HEIGHT and +0x34 is horizontal X/Z radius.
 // The unmodified half-height stays 3m after the 4->8m radius patch,
 // clipping contacts on sloped terrain at the expanded ring boundary.
 // Accept only exact original or desired 200%-style native dimensions.
 const float halfHeight=getf(cylinder,0x30);
 const float targetHeight=3.0f*(float)percent/100.0f;
 if((!approximate(halfHeight,3.0f) && !approximate(halfHeight,targetHeight)) ||
    !approximate(getf(cylinder,0x38),0.05f))return false;
 const float radius=getf(cylinder,0x34);
 const float target=4.0f*(float)percent/100.0f;
 if(!approximate(radius,4.0f)&&!approximate(radius,target))return false;
 uintptr_t inner=ptr(trigger,0x50);
 if(ptr(inner)!=base+0x03135708)return false;
 uintptr_t world=ptr(inner,0x58);
 uint32_t id=u32(inner,0x88);
 if(!world || id==0xffffffffu)return false;
 uintptr_t sys=ptr(world,0x2b0);
 if(!readable(sys+0x1c0,24))return false;
 uintptr_t manager=ptr(sys+0x1c0);
 uintptr_t bodyArray=ptr(ptr(manager,8));
 uintptr_t index=(uintptr_t)(id&0x7fffffu);
 if(index>=0x7fffffu)return false;
 uintptr_t body=ptr(bodyArray,(unsigned)(index*8u)) & ~(uintptr_t)1u;
 if(!readable(body+0x78,4)||u32(body,0x70)!=id||ptr(body,0x40)!=outer)return false;
 job.shelter=shelter;job.owner=owner;job.trigger=trigger;
 job.world=world;job.outer=outer;job.cylinder=cylinder;
 job.body=body;job.id=id;job.queuedAt=GetTickCount();
 return true;
}
static bool queue(const Job& job){
 lock();
 // Reconcile BOTH radius and vertical half-height after reload.
 // Never mistake radius=8 / half-height=3 for a completed expanded body.
 const float current=getf(job.cylinder,0x34);
 const float currentHeight=getf(job.cylinder,0x30);
 const float target=4.0f*(float)percent/100.0f;
 const float heightTarget=3.0f*(float)percent/100.0f;
 for(unsigned i=0;i<doneCount;++i){
  const Done& old=done[i];
  if(old.world==job.world&&old.cylinder==job.cylinder&&old.body==job.body&&old.id==job.id
     &&approximate(current,target) && approximate(currentHeight,heightTarget)){unlock();return true;}
 }
 for(unsigned i=head;i<tail;++i){
  const Job& old=jobs[i%256u];
  if(old.world==job.world&&old.cylinder==job.cylinder&&old.body==job.body&&old.id==job.id){
   unlock();return true;
  }
 }
 if(tail-head>=256){unlock();return false;}
 jobs[tail%256u]=job;++tail;
 unlock();
 if(InterlockedCompareExchange(&firstQueue,1,0)==0)
  note("construction_shelter=CYLINDER_BODY_QUEUED\r\n");
 return true;
}
// The shelter's visual Odradek ring uses a DIAMETER-sized visual parameter.
// Native FUN_141D72260 reads DSOdradekEffectInstance+0x2A0 and multiplies
// the value by exactly 0.5f to calculate rendered circular coverage.
// Thus the 200%-expanded 8m repair/protection radius requires a 16m visual
// size. Game repair/protection geometry remains 8m. The component and resource
// are owner-checked, and the renderer instance is separately type-checked.
// Never change OdradekEffectEnableRadius (+0x30): its genuine vanilla 30m
// controls when to enable/show the effect, not its visible circle radius.
static bool syncVisual(uintptr_t shelter,uintptr_t owner,uintptr_t members,
                       VisualSync& stable){
 if(!enabled||!isShelter(shelter)||ptr(ptr(shelter,0xa0),0x88)!=owner)return false;
 const float target=4.0f*(float)percent/100.0f;
 unsigned ready=0,recognized=0;
 Job validatedJobs[2]={};
 unsigned validatedSlots[2]={};
 unsigned odradekSlot=0xffffffffu;
 // Stage 3 uses +0xF8/+0x100 (confirmed on 2026-10-09),
 // while another observed stage uses +0xF0/+0xF8.
 const unsigned visualFastSlots[]={0xf0u,0xf8u,0x100u};
 const float targetHeight=3.0f*(float)percent/100.0f;
 for(unsigned i=0;i<3u;++i){
  const uintptr_t trg=ptr(members,visualFastSlots[i]);
  if(ptr(trg)!=base+0x03135648 && ptr(trg)!=base+0x031360E8)continue;
  Job job={};
  if(inspect(shelter,owner,trg,job)){
   ++recognized;
   if(approximate(getf(job.cylinder,0x34),target) &&
      approximate(getf(job.cylinder,0x30),targetHeight)){
    if(ready<2u){
     validatedJobs[ready]=job;
     validatedSlots[ready]=visualFastSlots[i];
    }
    ++ready;
   }
  }
 }
 // Valid known cylinders with vanilla radius wait for the queued Jolt
 // refresh rather than searching all 160 unrelated owner components.
 if(ready<2u && recognized<2u){
  for(unsigned off=0;off<0x500u && ready<2u;off+=8u){
   if(off==0xf0u || off==0xf8u || off==0x100u)continue;
   const uintptr_t trg=ptr(members,off);
   if(ptr(trg)!=base+0x03135648 && ptr(trg)!=base+0x031360E8)continue;
   Job job={};
   if(inspect(shelter,owner,trg,job) &&
      approximate(getf(job.cylinder,0x34),target) &&
      approximate(getf(job.cylinder,0x30),targetHeight)){
    if(ready<2u){validatedJobs[ready]=job;validatedSlots[ready]=off;}
    ++ready;
   }
  }
 }
 if(ready!=2u)return false;
 // Search owner membership rather than assume the level-three +0xd0 slot.
 uintptr_t odradek=0;
 for(unsigned off=0;off<0x200;off+=8){
  const uintptr_t candidate=ptr(members,off);
  if(ptr(candidate)==base+0x03296A68 && ptr(candidate,0x48)==owner){
   odradek=candidate;odradekSlot=off;break;
  }
 }
 if(!odradek)return false;
 const uintptr_t resource=ptr(odradek,0x30);
 if(ptr(resource)!=base+0x032972E8)return false;
 const uintptr_t instanceField=odradek+0x5c;
 const uintptr_t resourceField=resource+0x34;
 if(!readable(instanceField,4)||!readable(resourceField,4))return false;
 // This is visual geometry, not the native protection radius. Native
 // DSOdradekEffectInstance::GetEffectRange() multiplies this size by 0.5f.
 // At 200% the genuine radius is 8m, so the visual size must be 16m.
 const float visualSize=sam_shelter_visual::renderer_diameter(target);
 lock();
 const float instanceRadius=getf(odradek,0x5c);
 const float resourceRadius=getf(resource,0x34);
 const bool instDone=approximate(instanceRadius,visualSize);
 const bool resDone=approximate(resourceRadius,visualSize);
 const bool instKnown=approximate(instanceRadius,4.0f) ||
                      approximate(instanceRadius,target) || instDone;
 const bool resKnown=approximate(resourceRadius,4.0f) ||
                     approximate(resourceRadius,target) || resDone;
 // DSConstructionOdradekEffectComponentResource's reflected OverrideSize
 // at +0x38 was 1 in the actual shelter. Never alter this switch, and do
 // not force the diameter on resources where the override is disabled.
 const bool overrideEnabled=readable(resource+0x38,1) &&
                             *(const unsigned char*)(resource+0x38)==1u;
 if(!instKnown || !resKnown || !overrideEnabled){
  unlock();return false;
 }
 if((!instDone&&!readable(instanceField,4,true)) ||
    (!resDone&&!readable(resourceField,4,true))){
  unlock();return false;
 }
 if(!instDone)*(volatile float*)instanceField=visualSize;
 if(!resDone)*(volatile float*)resourceField=visualSize;
 const bool compOk=approximate(getf(odradek,0x5c),visualSize) &&
                   approximate(getf(resource,0x34),visualSize);
 unlock();
 if(!compOk)return false;
 if(!instDone || !resDone){
  static volatile LONG updated=0;
  if(InterlockedIncrement(&updated)<=16)
   note("construction_shelter=ODRADEK_VISUAL_DIAMETER_SYNCED\r\n");
 }
 // The renderer is created separately and may already hold the original
 // 8m size from resource initialization. Synchronize that one per-shelter
 // instance, not the globally shared DSOdradekEffectResource or other rings.
 const uintptr_t renderer=ptr(odradek,0x50);
 if(ptr(renderer)!=base+0x0338E2C8)return false;
 const uintptr_t rendererRes=ptr(renderer,0xc0);
 if(ptr(rendererRes)!=base+0x0338E5C8)return false;
 const uintptr_t rendererField=renderer+0x2a0;
 const float rendererSize=getf(renderer,0x2a0);
 if(!approximate(rendererSize,visualSize)){
  // Permit only the original 4m visual size and the already extended native
  // 8m numerical size as sources; unknown animation/renderer data fail open.
  // DS2 sometimes creates a renderer with double the component's size:
  // 16 -> 32 (an incorrectly displayed 16m RADIUS instead of 8m).
  // Correct only the known 4/8/16/32m native sizes; reject arbitrary FX.
  if(!sam_shelter_visual::accept_renderer_correction(rendererSize,target))
   return false;
  if(!readable(rendererField,4,true))return false;
  *(volatile float*)rendererField=visualSize;
  if(!approximate(getf(renderer,0x2a0),visualSize))return false;
  static volatile LONG rendererUpdated=0;
  if(InterlockedIncrement(&rendererUpdated)<=16) {
   note("construction_shelter=ODRADEK_RENDER_INSTANCE_DIAMETER_SYNCED\r\n");
   if(approximate(rendererSize,visualSize*2.0f))
    note("construction_shelter=OVERSIZED_RENDERER_32_TO_16_FIXED\r\n");
  }
 }
 // Collect a stable snapshot only when exactly two distinct complete native
 // Jolt bodies, the renderer and the exact original repair source are valid.
 // Unknown shelter layouts still use the original full update path.
 stable.key.shelter=shelter;
 stable.key.owner=owner;
 stable.key.members=members;
 stable.key.odradek=odradek;
 stable.key.odradekResource=resource;
 stable.key.renderer=renderer;
 stable.odradekSlot=odradekSlot;
 for(unsigned i=0;i<2u;++i){
  stable.validated[i]=validatedJobs[i];
  stable.triggerSlot[i]=validatedSlots[i];
  stable.key.triggers[i]=validatedJobs[i].trigger;
  stable.key.cylinders[i]=validatedJobs[i].cylinder;
  stable.key.ids[i]=validatedJobs[i].id;
 }
 const uintptr_t firstRepair=ptr(members,0x28u);
 const bool firstValid=ptr(firstRepair)==base+0x03297208 &&
                       ptr(firstRepair,0x48u)==owner;
 const uintptr_t repair=firstValid?firstRepair:ptr(members,0x30u);
 const uintptr_t repairResource=ptr(repair,0x30u);
 const float expectedRepair=4.0f*(float)repairPercent/100.0f;
 if(ptr(repair)==base+0x03297208 &&
    ptr(repair,0x48u)==owner &&
    ptr(repairResource)==base+0x03296670 &&
    approximate(getf(repairResource,0x20u),expectedRepair) &&
    validatedJobs[0].trigger!=validatedJobs[1].trigger &&
    validatedJobs[0].body!=validatedJobs[1].body &&
    validatedSlots[0]!=validatedSlots[1]){
  stable.key.repair=repair;
  stable.key.repairResource=repairResource;
  stable.repairSlot=firstValid?0x28u:0x30u;
  stable.lastRegister=GetTickCount();
  stable.stable=true;
 }
 // The native renderer can be rebuilt independently; each steady-state
 // check verifies current object identity and the 16m draw diameter.
 return true;
}

// Passive spatial diagnostics: active shelter only, no engine calls and no
// cargo/weather/effect writes. 'player' is a proxy for held cargo position;
// only the engine's native per-item repair decision proves actual eligibility.
// Output stores squared distances * 100 to avoid CRT math/formatting.
struct SpatialProbeStamp { uintptr_t shelter,owner;DWORD last; };
static SpatialProbeStamp spatialStamp[128]={};
static volatile LONG spatialSamples=0;
static void probeSpatial(uintptr_t shelter,uintptr_t owner,uintptr_t members,DWORD now){
 if(InterlockedCompareExchange(&spatialSamples,0,0)>=96)return;
 const unsigned bucket=(unsigned)(((shelter>>8)^(shelter>>20))&127u);
 SpatialProbeStamp& st=spatialStamp[bucket];
 if(st.shelter==shelter && st.owner==owner &&
    (DWORD)(now-st.last)<3400u)return;
 st.shelter=shelter;st.owner=owner;st.last=now;
 if(ptr(owner)!=base+0x03119BC8 || ptr(ptr(shelter,0xa0),0x88)!=owner)return;
 uintptr_t repair=ptr(members,0x28);
 if(ptr(repair)!=base+0x03297208 || ptr(repair,0x48)!=owner)
  repair=ptr(members,0x30);
 if(ptr(repair)!=base+0x03297208 || ptr(repair,0x48)!=owner)return;
 const uintptr_t resource=ptr(repair,0x30);
 if(ptr(resource)!=base+0x03296670 || !readable(repair+0x70,1))return;
 const float radius=getf(resource,0x20);
 if(radius<3.0f || radius>20.0f)return;
 const uintptr_t playerManager=ptr(base+0x0623DF40);
 const uintptr_t playerRows=ptr(playerManager,0x40);
 const uintptr_t firstRow=ptr(playerRows);
 const uintptr_t player=ptr(firstRow,0x48);
 const uintptr_t playerVt=ptr(player);
 if(playerVt!=base+0x03233158 && playerVt!=base+0x03233A68)return;
 // Same WorldPosition +0xE8 triple used by native baggage repair at
 // RVA 0x11B04F0. Game WorldPosition is X/Y ground and Z elevation;
 // the old dev21 diagnostic mistakenly treated Y as elevation.
 // dev20 also started at +0xF0 and omitted world X entirely.
 if(!readable(player+0xe8,24)||!readable(owner+0xe8,24))return;
 const double* pp=(const double*)(player+0xe8);
 const double* sp=(const double*)(owner+0xe8);
 const double dx=pp[0]-sp[0],dy=pp[1]-sp[1],dz=pp[2]-sp[2];
 const double horizontal=dx*dx+dy*dy, threeDim=horizontal+dz*dz;
 if(!(horizontal>=0 && horizontal<400 && threeDim>=0 && threeDim<900))return;
 // Probe only near this structure; bounded integer outputs in centi-m^2.
 if(horizontal>radius*radius*2.25)return;
 char line[200];unsigned n=0;
 n=addLiteral(line,n,"construction_shelter=SPATIAL_SNAPSHOT xy2x100=");
 n=addDigits(line,n,(unsigned)(horizontal*100.0+0.5));
 n=addLiteral(line,n," xyz2x100=");
 n=addDigits(line,n,(unsigned)(threeDim*100.0+0.5));
 n=addLiteral(line,n," verticalZ2x100=");
 n=addDigits(line,n,(unsigned)(dz*dz*100.0+0.5));
 n=addLiteral(line,n," repairRadius2x100=");
 n=addDigits(line,n,(unsigned)(radius*radius*100.0+0.5));
 n=addLiteral(line,n," repair_contact=");
 n=addDigits(line,n,(unsigned)*(const unsigned char*)(repair+0x70));
 if(readable(shelter+0x422,1)){
  n=addLiteral(line,n," rest_contact=");
  n=addDigits(line,n,(unsigned)*(const unsigned char*)(shelter+0x422));
 }
 n=addLiteral(line,n," note=PLAYER_PROXY\r\n");
 line[n]=0;
 note(line);
 InterlockedIncrement(&spatialSamples);
}
// Steady-state is legal ONLY after original full native sync returned
// a fully validated snapshot. The per-tick shortcut checks all material
// source/owner/Jolt BodyID/cylinder/radius/renderer identities. A mismatch
// immediately re-enters the original full native discovery on this tick.
// No worker-thread dereference of untrusted objects or extra native hook.
static bool steadyShelterValid(const VisualSync& v,
                               uintptr_t shelter,uintptr_t owner,
                               uintptr_t members){
 if(!v.stable || !shelter || !owner || !members)return false;
 const shelter_steady_state::Key& key=v.key;
 if(key.shelter!=shelter || key.owner!=owner || key.members!=members)
  return false;
 if(!isShelter(shelter) ||
    ptr(ptr(shelter,0xa0u),0x88u)!=owner ||
    ptr(owner)!=base+0x03119BC8 ||
    ptr(owner,0xa8u)!=members)
  return false;
 if(v.odradekSlot>=0x200u || v.repairSlot>0x30u ||
    ptr(members,v.odradekSlot)!=key.odradek ||
    ptr(members,v.repairSlot)!=key.repair)
  return false;
 if(ptr(key.odradek)!=base+0x03296A68 ||
    ptr(key.odradek,0x48u)!=owner ||
    ptr(key.odradek,0x30u)!=key.odradekResource ||
    ptr(key.odradekResource)!=base+0x032972E8 ||
    ptr(key.odradek,0x50u)!=key.renderer ||
    ptr(key.renderer)!=base+0x0338E2C8 ||
    ptr(ptr(key.renderer,0xc0u))!=base+0x0338E5C8 ||
    !readable(key.odradekResource+0x38u,1u) ||
    *(const unsigned char*)(key.odradekResource+0x38u)!=1u)
  return false;
 if(ptr(key.repair)!=base+0x03297208 ||
    ptr(key.repair,0x48u)!=owner ||
    ptr(key.repair,0x30u)!=key.repairResource ||
    ptr(key.repairResource)!=base+0x03296670)
  return false;

 // Verify TWO independent native Jolt triggers and body generations.
 // Without these identities, a new save or scene/structure upgrade may
 // reuse an old address and must NOT bypass the full reconciliation.
 shelter_steady_state::Key current=key;
 for(unsigned i=0;i<2u;++i){
  const Job& job=v.validated[i];
  if(job.shelter!=shelter || job.owner!=owner ||
     v.triggerSlot[i]>=0x500u ||
     ptr(members,v.triggerSlot[i])!=job.trigger)
   return false;
  const uintptr_t triggerVT=ptr(job.trigger);
  if((triggerVT!=base+0x03135648 &&
      triggerVT!=base+0x031360E8) ||
     ptr(job.trigger,0x48u)!=owner ||
     ptr(job.cylinder)!=base+0x0345CA98 ||
     ptr(job.outer)!=base+0x0345B768 ||
     ptr(job.outer,0x20u)!=job.cylinder ||
     !readable(job.body+0x78u,4u) ||
     u32(job.body,0x70u)!=job.id ||
     ptr(job.body,0x40u)!=job.outer)
   return false;
  const uintptr_t inner=ptr(job.trigger,0x50u);
  if(ptr(inner)!=base+0x03135708 ||
     ptr(inner,0x58u)!=job.world ||
     u32(inner,0x88u)!=job.id)
   return false;
  current.triggers[i]=ptr(members,v.triggerSlot[i]);
  current.cylinders[i]=ptr(job.outer,0x20u);
  current.ids[i]=u32(inner,0x88u);
 }
 if(!shelter_steady_state::identity(key,current))return false;
 const float rainTarget=4.0f*(float)percent/100.0f;
 const float repairTarget=4.0f*(float)repairPercent/100.0f;
 return shelter_steady_state::dimensions(
    rainTarget,getf(key.repairResource,0x20u),
    getf(key.renderer,0x2a0u),
    getf(key.odradekResource,0x34u),
    getf(key.odradek,0x5cu),
    getf(v.validated[0].cylinder,0x34u),
    getf(v.validated[0].cylinder,0x30u),
    getf(v.validated[1].cylinder,0x34u),
    getf(v.validated[1].cylinder,0x30u),
    rainTarget,repairTarget);
}
static void discover(void* object){
 if(!enabled||!object)return;
 uintptr_t shelter=(uintptr_t)object;
 // Fast negative lookup without VirtualQuery; stale/replaced native shelters
 // retry through this cache automatically after save reload.
 unsigned bucket=(unsigned)(((shelter>>8)^(shelter>>20))&1023u);
 DWORD now=GetTickCount();
 if(throttled[bucket].object==shelter &&
    (DWORD)(now-throttled[bucket].last)<3600u)return;
 if(!isShelter(shelter))return;
 DiscoveryMeter performanceScope;
 if(InterlockedCompareExchange(&firstSeen,1,0)==0){
  note("construction_shelter=LIVE_SHELTER_SEEN\r\n");
  // Decima preloads localized rest text AFTER Sam's worker is attached.
  // Start one-shot text reconciliation ONLY after actual shelter objects
  // exist; the early dev26 global heap scan finished before save load.
  SamShelterRestLabelNotifyActiveShelter();
 }
 if(throttled[bucket].object!=shelter)throttled[bucket].fallbackAt=0;
 throttled[bucket].object=shelter;
 throttled[bucket].last=now;
 uintptr_t wrapper=ptr(shelter,0xa0);
 uintptr_t owner=ptr(wrapper,0x88);
 uintptr_t members=ptr(owner,0xa8);
 if(!readable(members,0x100))return;
 const unsigned vbucket=(unsigned)(((shelter>>8u)^(shelter>>20u))&255u);
 VisualSync& v=visualSync[vbucket];
 if(steadyShelterValid(v,shelter,owner,members)){
  // Reaffirm verified native RepairSpray registration after streaming changes.
  // This matches the original repair registry and does not alter radii.
  if((DWORD)(now-v.lastRegister)>=10000u){
   SamConstructionRepairGateRegister((void*)v.key.repair,
       (void*)v.key.repairResource,(void*)owner);
   v.lastRegister=now;
  }
  performanceScope.steady=true;
  return;
 }
 v.stable=false;
 reconcileNativeRepairRadius(shelter,owner,members);
 if(spatialDiagnostics)probeSpatial(shelter,owner,members,now);
 unsigned accepted=0;
 // Native active shelter collision components confirmed at +0xF0/+0xF8.
 // Two cheap guarded reads, rather than 160 sequential VirtualQuery calls.
 const unsigned fastSlots[]={0xf0u,0xf8u,0x100u};
 for(unsigned i=0;i<3u && accepted<2u;++i){
  const uintptr_t trigger=ptr(members,fastSlots[i]);
  if(ptr(trigger)!=base+0x03135648 && ptr(trigger)!=base+0x031360E8)continue;
  Job j={};
  if(!inspect(shelter,owner,trigger,j))continue;
  if(queue(j))++accepted;
 }
 if(accepted<2u &&
    (throttled[bucket].fallbackAt==0 ||
     (DWORD)(now-throttled[bucket].fallbackAt)>=10000u)){
  throttled[bucket].fallbackAt=now;
  InterlockedIncrement(&perfFallbackCount);
  // Unknown layouts get a full scan at most once every 10 seconds.
  for(unsigned off=0;off<0x500u && accepted<2u;off+=8u){
   if(off==0xf0u || off==0xf8u || off==0x100u)continue;
   const uintptr_t trigger=ptr(members,off);
   if(ptr(trigger)!=base+0x03135648 && ptr(trigger)!=base+0x031360E8)continue;
   Job j={};
   if(!inspect(shelter,owner,trigger,j))continue;
   if(queue(j))++accepted;
  }
 }
 // Only instantiated shelters have validated protection cylinders.
 // Streamed, inactive structures must not pay the cost of visual scanning.
 if(accepted>0){
  if(v.key.shelter!=shelter || v.key.owner!=owner ||
     !v.stable || (DWORD)(now-v.lastGood)>=4000u){
   VisualSync next={};
   if(syncVisual(shelter,owner,members,next)){
    next.lastGood=now;
    v=next;
   }
  }
 }
}
static void __fastcall hooked(void* object) {
 bool active=false;
 if(enabled && object && base){
  // DS2 itself calls this method with a valid native this pointer.
  // The original already branches on +0x34 == 3. Compare it directly on
  // this trusted call path, before expensive object/heap queries.
  const uintptr_t obj=(uintptr_t)object;
  const uintptr_t vt=*(const uintptr_t*)obj;
  active=(vt==base+0x032851C8 || vt==base+0x03280968) &&
         (*(const unsigned char*)(obj+0x34)==3u);
  if(active)reconcileNativeShelterRange(obj);
 }
 if(original)original(object);
 if(active)discover(object);
}
}
extern "C" bool SamConstructionShelterEnabled(){return shelter_range::enabled;}
extern "C" bool SamConstructionShelterInstall(void* image,const wchar_t* ini,HANDLE log){
 using namespace shelter_range;
 logger=log;
 if(!image||!ini)return false;
 enabled=GetPrivateProfileIntW(L"TimefallShelterRange",L"Enabled",0,ini)==1;
 percent=GetPrivateProfileIntW(L"TimefallShelterRange",L"RangePercent",200,ini);
 repairPercent=GetPrivateProfileIntW(L"TimefallShelterRange",L"RepairRadiusPercent",215,ini);
 spatialDiagnostics=GetPrivateProfileIntW(L"TimefallShelterRange",L"SpatialDiagnostics",0,ini)==1;
 if(!enabled){note("construction_shelter=DISABLED\r\n");return true;}
 if(percent<100||percent>400){note("construction_shelter=REFUSED invalid_percent\r\n");enabled=false;return false;}
 if(repairPercent<100||repairPercent>400){
  note("construction_shelter=REFUSED invalid_repair_percent\r\n");
  enabled=false;return false;
 }
 if(percent==100){enabled=false;note("construction_shelter=VANILLA_100_PERCENT\r\n");return true;}
 base=(uintptr_t)image;
 // DSRainShelter per-instance update; Steam DS2 1.10.89.0.
 const unsigned char expected[]={
  0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0xe8,0x22,0xb0,0xf7,0xff,
  0x80,0x7b,0x34,0x03};
 const uintptr_t target=base+0x013061E0;
 if(!readable(target,sizeof(expected))){note("construction_shelter=REFUSED unreadable_update\r\n");enabled=false;return false;}
 for(unsigned i=0;i<sizeof(expected);++i){
  if(((const unsigned char*)target)[i]!=expected[i]){
   note("construction_shelter=REFUSED update_signature_mismatch\r\n");enabled=false;return false;
  }
 }
 const unsigned char notifySig[]={
  0x48,0x89,0x74,0x24,0x18,0x44,0x88,0x4c,0x24,0x20,0x57,0x41,0x56,
  0x41,0x57,0x48,0x83,0xec,0x30};
 uintptr_t notify=base+0x027BD170;
 if(!readable(notify,sizeof(notifySig))){enabled=false;return false;}
 for(unsigned i=0;i<sizeof(notifySig);++i)
  if(((const unsigned char*)notify)[i]!=notifySig[i]){
   note("construction_shelter=REFUSED notify_signature_mismatch\r\n");
   enabled=false;return false;
  }
 MH_STATUS s=MH_Initialize();
 if(s!=MH_OK&&s!=MH_ERROR_ALREADY_INITIALIZED){enabled=false;return false;}
 s=MH_CreateHook((void*)target,(void*)&hooked,(void**)&original);
 if(s!=MH_OK){note("construction_shelter=REFUSED hook_create\r\n");enabled=false;return false;}
 s=MH_EnableHook((void*)target);
 if(s!=MH_OK){
  MH_RemoveHook((void*)target);
  enabled=false;note("construction_shelter=REFUSED hook_enable\r\n");return false;
 }
 installed=true;
 LARGE_INTEGER f={};
 if(QueryPerformanceFrequency(&f) && f.QuadPart>0)perfFrequency=f.QuadPart;
 perfLastLog=GetTickCount();
 note("construction_shelter=ACTIVE_ONLY_FASTPATH_INSTALLED\r\n");
 note("construction_shelter=UPDATE_HOOK_INSTALLED\r\n");
 return true;
}
extern "C" void SamConstructionShelterPoll(HANDLE log){
 using namespace shelter_range;
 if(!enabled||!installed)return;
 logDiscoveryPerformance();
 unsigned budget=8u;
 while(budget--){
  Job job={};
  lock();
  if(head==tail){unlock();break;}
  if((DWORD)(GetTickCount()-jobs[head%256u].queuedAt)<500u){unlock();break;}
  job=jobs[head%256u];++head;
  unlock();
  Job current={};
  if(!inspect(job.shelter,job.owner,job.trigger,current) ||
     current.cylinder!=job.cylinder || current.outer!=job.outer ||
     current.world!=job.world || current.id!=job.id || current.body!=job.body){
   if(InterlockedIncrement(&skipped)<=8)
    note("construction_shelter=SKIP stale_or_invalid_body\r\n");
   continue;
  }
  const float currentRadius=getf(job.cylinder,0x34);
  const float currentHeight=getf(job.cylinder,0x30);
  const float target=4.0f*(float)percent/100.0f;
  const float targetHeight=3.0f*(float)percent/100.0f;
  if(!approximate(currentRadius,4.0f) && !approximate(currentRadius,target))continue;
  if(!approximate(currentHeight,3.0f) && !approximate(currentHeight,targetHeight))continue;
  if(approximate(currentRadius,target) && approximate(currentHeight,targetHeight))continue;
  if(!readable(job.cylinder+0x30,8,true))continue;
  // Keep JPH::CylinderShape 0x38 convex radius 0.05f unmodified.
  *(volatile float*)(job.cylinder+0x30)=targetHeight;
  *(volatile float*)(job.cylinder+0x34)=target;
  if(!approximate(getf(job.cylinder,0x30),targetHeight) ||
     !approximate(getf(job.cylinder,0x34),target))continue;
  // Jolt recomputes AABB and broadphase after the paired geometry mutation.
  uintptr_t system=ptr(job.world,0x2b0);
  uintptr_t iface=system+0x1c0;
  typedef void (__fastcall* NotifyFn)(void*,const uint32_t*,const float*,bool);
  NotifyFn notify=(NotifyFn)(base+0x027BD170);
  __declspec(align(16)) float previousCentre[4]={0.0f,0.0f,0.0f,0.0f};
  const uint32_t bodyid=job.id;
  notify((void*)iface,&bodyid,previousCentre,false);
  lock();
  if(doneCount<2048)done[doneCount++]={job.world,job.cylinder,job.body,job.id};
  unlock();
  LONG n=InterlockedIncrement(&completed);
  if(n<=16){
   if(log && log!=logger && log!=INVALID_HANDLE_VALUE){
    const char msg[]="construction_shelter=CYLINDER_RADIUS_HEIGHT_AND_JOLT_REFRESHED\r\n";
    DWORD w=0;WriteFile(log,msg,sizeof(msg)-1,&w,nullptr);
   } else note("construction_shelter=CYLINDER_RADIUS_HEIGHT_AND_JOLT_REFRESHED\r\n");
  }
 }
}
