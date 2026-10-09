// Development experiment: Jolt BodyInterface refresh for scaled generator triggers.
// Runs after trigger creation, on the existing Sam Overhaul worker thread.
// Strictly validates DS2 build, generator component, Jolt BodyID and Shape identity.
#include <windows.h>
#include <stdint.h>
namespace physics_refresh {
static uintptr_t image=0;
static bool enabled=false;
static unsigned activePercent=200;
static volatile LONG spin=0;
static volatile LONG queued=0, refreshed=0, rejected=0, firstQueueReport=0;
struct Job {uintptr_t world;uintptr_t shape;uintptr_t body;uint32_t id;DWORD queuedAt;};
struct Done {uintptr_t world;uintptr_t shape;uintptr_t body;uint32_t id;};
static Done done[4096]={};
static unsigned doneCount=0;
static Job jobs[512]={};
static unsigned head=0,tail=0;
static bool readable(uintptr_t a,SIZE_T n) {
 if(a<0x10000 || a>0x00007FFFFFFFFFFFULL || n==0)return false;
 MEMORY_BASIC_INFORMATION m={};
 if(!VirtualQuery((const void*)a,&m,sizeof(m)))return false;
 const DWORD p=m.Protect&0xff;
 uintptr_t end=(uintptr_t)m.BaseAddress+m.RegionSize;
 return m.State==MEM_COMMIT && !(m.Protect&PAGE_GUARD) &&
        p!=PAGE_NOACCESS && end>=a && a>=((uintptr_t)m.BaseAddress) && n<=end-a;
}
static uintptr_t at(uintptr_t p,unsigned off=0) {
 const uintptr_t a=p+(uintptr_t)off;
 return readable(a,8)?*(const uintptr_t*)a:0;
}
static uint32_t idAt(uintptr_t p,unsigned off) {
 const uintptr_t a=p+(uintptr_t)off;
 return readable(a,4)?*(const uint32_t*)a:0xffffffffu;
}
static float radius(uintptr_t a) {
 return readable(a+0x30,4)?*(const float*)(a+0x30):0.0f;
}
static void print(HANDLE h,const char* s) {
 if(!h||h==INVALID_HANDLE_VALUE||!s)return;
 DWORD n=0,w=0;while(s[n])++n;
 if(n)WriteFile(h,s,n,&w,nullptr);
}
static void enter(){while(InterlockedCompareExchange(&spin,1,0))Sleep(0);}
static void leave(){InterlockedExchange(&spin,0);}
static bool isGeneratorOwner(uintptr_t owner) {
 uintptr_t members=at(owner,0xa8);
 if(!readable(members,0x100))return false;
 for(unsigned i=0;i<0x100;i+=8) {
  uintptr_t ptr=at(members,i);
  if(!readable(ptr,8))continue;
  const uintptr_t vt=at(ptr);
  if(vt==image+0x03295030 || vt==image+0x03295348)return true;
 }
 return false;
}
static bool signature() {
 const BYTE expected[]={0x48,0x89,0x74,0x24,0x18,0x44,0x88,0x4c,0x24,0x20,
                        0x57,0x41,0x56,0x41,0x57,0x48,0x83,0xec,0x30};
 const uintptr_t p=image+0x027BD170;
 if(!readable(p,sizeof(expected)))return false;
 const BYTE* b=(const BYTE*)p;
 for(unsigned i=0;i<sizeof(expected);++i)if(b[i]!=expected[i])return false;
 return true;
}
} // namespace physics_refresh
extern "C" bool SamConstructionRefreshConfigure(void* imageBase,bool generatorEnabled,unsigned percent,HANDLE log) {
 using namespace physics_refresh;
 image=(uintptr_t)imageBase;
 activePercent=percent;
 enabled=generatorEnabled && percent>100 && percent<=400 && signature();
 print(log,enabled?"construction_physics_refresh=ENABLED Jolt_NotifyShapeChanged\r\n":
                   "construction_physics_refresh=DISABLED no_generator_or_signature_mismatch\r\n");
 return !generatorEnabled || activePercent==100 || enabled;
}
extern "C" bool SamConstructionRefreshEnabled(){return physics_refresh::enabled;}
extern "C" bool SamConstructionRefreshQueue(void* source) {
 using namespace physics_refresh;
 if(!enabled||!source)return false;
 const uintptr_t t=(uintptr_t)source;
 if(!isGeneratorOwner(at(t,0x48)))return false;
 const uintptr_t resource=at(t,0x30);
 if(at(resource)!=image+0x03136180)return false;
 const uintptr_t physics=at(resource,0x20);
 if(at(physics)!=image+0x03413EF0)return false;
 const uintptr_t simple=at(physics,0xb0);
 if(at(simple)!=image+0x03414EE8)return false;
 const uintptr_t shape=at(simple,0x20);
 if(at(shape)!=image+0x03414868)return false;
 const float r=radius(shape);
 // Only the intended 200% generator test shape radii.
 bool matched=false;
 const float vanilla[]={9.0f,12.0f,15.0f};
 for(unsigned i=0;i<3;++i){
    const float expected=vanilla[i]*(float)activePercent/100.0f;
    if(r>expected-0.08f && r<expected+0.08f){matched=true;break;}
 }
 if(!matched)return false;
 const uintptr_t inner=at(t,0x50);
 if(at(inner)!=image+0x03135708)return false;
 const uintptr_t world=at(inner,0x58);
 const uint32_t bodyid=idAt(inner,0x88);
 if(!world||bodyid==0xffffffffu)return false;
 // A trigger may exist before Jolt has registered its active body. Keep retrying
 // instead of prematurely removing it from the post-initialization candidate list.
 const uintptr_t system=at(world,0x2b0);
 if(!readable(system+0x1c0,24))return false;
 const uintptr_t manager=at(system+0x1c0);
 const uintptr_t bodyArray=at(at(manager,8));
 const uintptr_t body=at(bodyArray,(unsigned)((bodyid&0x7fffffu)*8u)) & ~(uintptr_t)1u;
 if(!readable(body+0x78,4)||idAt(body,0x70)!=bodyid||at(body,0x40)!=shape)return false;
 enter();
 // Suppress repeated NotifyShapeChanged calls for a stable active body.
 for(unsigned i=0;i<doneCount;++i){
  if(done[i].world==world && done[i].shape==shape && done[i].body==body && done[i].id==bodyid){
    leave(); return true;
  }
 }
 for(unsigned i=head;i<tail;++i){
  const Job& old=jobs[i%512u];
  if(old.world==world && old.shape==shape && old.body==body && old.id==bodyid){
    leave(); return true;
  }
 }
 if(tail-head<512u) {
  Job& job=jobs[tail%512u];
  job.world=world;job.shape=shape;job.body=body;job.id=bodyid;job.queuedAt=GetTickCount();
  ++tail;InterlockedIncrement(&queued);
 } else {InterlockedIncrement(&rejected);leave();return false;}
 leave();
 return true;
}

extern "C" void SamConstructionRefreshPoll(HANDLE logger) {
 using namespace physics_refresh;
 if(!enabled)return;
 if(queued>0 && InterlockedCompareExchange(&firstQueueReport,1,0)==0)
  print(logger,"construction_physics_refresh=QUEUE_DETECTED\r\n");
 unsigned budget=16u;
 while(budget--) {
  Job job={};
  enter();
  if(head==tail) {leave();break;}
  if((DWORD)(GetTickCount()-jobs[head%512u].queuedAt)<500u){leave();break;}
  job=jobs[head%512u];
  ++head;leave();
  // Resolve the currently live Jolt Body through the engine's actual BodyManager.
  const uintptr_t system=at(job.world,0x2b0);
  if(!readable(system+0x1c0,24)) {
   InterlockedIncrement(&rejected);
   print(logger,"construction_physics_refresh=SKIPPED invalid_physics_system\r\n");
   continue;
  }
  const uintptr_t bodyInterface=system+0x1c0;
  const uintptr_t manager=at(bodyInterface);
  const uintptr_t intermediate=at(manager,8);
  const uintptr_t bodyArray=at(intermediate);
  const uintptr_t index=(uintptr_t)(job.id & 0x7fffffu);
  const uintptr_t body=at(bodyArray,(unsigned)(index*8u)) & ~(uintptr_t)1u;
  if(!readable(body+0x78,4) || body!=job.body || idAt(body,0x70)!=job.id ||
     at(body,0x40)!=job.shape || radius(job.shape)<8.9f) {
   InterlockedIncrement(&rejected);
   if(rejected<=8)print(logger,"construction_physics_refresh=SKIPPED stale_or_unmatched_body\r\n");
   continue;
  }
  // Confirmed game function RVA 0x027BD170: BodyInterface::NotifyShapeChanged.
  // It updates the Jolt bounds and broadphase queue under its internal locks.
  // The sphere's previous center of mass is the origin; no mass recalculation.
  typedef void (__fastcall* NotifyShapeFn)(void*,const uint32_t*,const float*,bool);
  const NotifyShapeFn notify=(NotifyShapeFn)(image+0x027BD170u);
  __declspec(align(16)) float oldCenterOfMass[4]={0.0f,0.0f,0.0f,0.0f};
  const uint32_t id=job.id;
  notify((void*)bodyInterface,&id,oldCenterOfMass,false);
  enter();
  if(doneCount<4096){done[doneCount++]={job.world,job.shape,job.body,job.id};}
  leave();
  InterlockedIncrement(&refreshed);
  if(refreshed<=12)print(logger,"construction_physics_refresh=NOTIFY_INVOKED validated_generator_body\r\n");
 }
}
