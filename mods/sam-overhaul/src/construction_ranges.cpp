// Sam Overhaul experimental construction effect range hook.
// TriggerComponent initializes the Jolt trigger after this pre-call.
// DS2 Steam 1.10.89.0 only; exact signature and vtables must match.
#include <windows.h>
#include <stdint.h>
#include "MinHook.h"
extern "C" bool SamConstructionRefreshQueue(void*);
extern "C" bool SamConstructionRefreshConfigure(void*,bool,unsigned,HANDLE);
extern "C" bool SamConstructionChargerUpdateInstall(void*,HANDLE);
extern "C" bool SamConstructionGeneratorVisualSync(void*);
namespace range_ext {
static uintptr_t base=0;
static bool genEnabled=false,shelterEnabled=false,installed=false;
static unsigned genPercent=200,shelterPercent=200;
static volatile LONG genChanges=0,shelterChanges=0,skips=0,lockFlag=0;
static HANDLE runtimeLog=INVALID_HANDLE_VALUE;
struct Seen { uintptr_t address; float lastScaled; };
static Seen seen[1024]={};
static unsigned seenCount=0;
typedef void (__fastcall* TriggerInit)(void*);
static TriggerInit original=0;

static bool mem(uintptr_t p,SIZE_T n,bool write=false) {
    if(p<0x10000 || p>0x00007FFFFFFFFFFFULL || n==0)return false;
    MEMORY_BASIC_INFORMATION mi={};
    if(!VirtualQuery((const void*)p,&mi,sizeof(mi)))return false;
    if(mi.State!=MEM_COMMIT || (mi.Protect&PAGE_GUARD) || (mi.Protect&PAGE_NOACCESS))return false;
    if(write) {
        DWORD access=mi.Protect&0xff;
        if(mi.Type!=MEM_PRIVATE || (access!=PAGE_READWRITE && access!=PAGE_WRITECOPY &&
            access!=PAGE_EXECUTE_READWRITE && access!=PAGE_EXECUTE_WRITECOPY))return false;
    }
    uintptr_t end=(uintptr_t)mi.BaseAddress+mi.RegionSize;
    return end>=p && n<=end-p;
}
static uintptr_t ptr(uintptr_t p,unsigned offset=0) {
    uintptr_t at=p+(uintptr_t)offset;
    return mem(at,sizeof(uintptr_t))?*(const uintptr_t*)at:0;
}
static void say(HANDLE f,const char* s) {
    if(!f || f==INVALID_HANDLE_VALUE || !s)return;
    DWORD len=0,done=0;while(s[len])++len;
    if(len)WriteFile(f,s,len,&done,nullptr);
}
static void makeLog() {
    wchar_t path[1024]={};
    DWORD n=GetModuleFileNameW(nullptr,path,1024);
    if(!n || n>980)return;
    DWORD cut=0;for(DWORD i=0;i<n;++i)if(path[i]==L'\\'||path[i]==L'/')cut=i+1;
    const wchar_t filename[]=L"ds2_sam_overhaul_ranges.log";
    unsigned i=0;while(filename[i]){path[cut+i]=filename[i];++i;}path[cut+i]=0;
    runtimeLog=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                           nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    say(runtimeLog,"Construction range dev probe - triggers only\r\n");
}
static unsigned percent(const wchar_t* ini,const wchar_t* sec) {
    UINT n=GetPrivateProfileIntW(sec,L"RangePercent",200,ini);
    if(n<100)return 100;
    if(n>400)return 400;
    return n;
}
static bool componentTypes(uintptr_t owner,bool& gen,bool& shelter) {
    uintptr_t members=ptr(owner,0xA8);
    if(!mem(members,sizeof(uintptr_t)))return false;
    MEMORY_BASIC_INFORMATION mi={};
    if(!VirtualQuery((const void*)members,&mi,sizeof(mi)))return false;
    uintptr_t end=(uintptr_t)mi.BaseAddress+mi.RegionSize;
    SIZE_T bytes=end>=members?end-members:0;
    if(bytes>0x400)bytes=0x400;
    for(SIZE_T off=0;off+8<=bytes;off+=8) {
        uintptr_t comp=*(const uintptr_t*)(members+off);
        if(!mem(comp,8))continue;
        uintptr_t vt=ptr(comp);
        if(vt==base+0x03295030 || vt==base+0x03295348)gen=true;
        if(vt==base+0x03280968 || vt==base+0x032851C8)shelter=true;
    }
    return gen||shelter;
}
static bool isGeneratorRadius(float x) {
    return (x>8.95f&&x<9.05f)||(x>11.95f&&x<12.05f)||(x>14.95f&&x<15.05f);
}
static void before(void* v) {
    if(!v||(!genEnabled&&!shelterEnabled))return;
    uintptr_t t=(uintptr_t)v;
    // TriggerComponentResource -> PhysicsCollisionResource ->
    // PhysicsSimpleShapeResource -> JPH::SphereShape, as observed live.
    uintptr_t resource=ptr(t,0x30);
    if(ptr(resource)!=base+0x03136180)return;
    uintptr_t collision=ptr(resource,0x20);
    if(ptr(collision)!=base+0x03413EF0)return;
    uintptr_t simple=ptr(collision,0xB0);
    if(ptr(simple)!=base+0x03414EE8)return;
    uintptr_t sphere=ptr(simple,0x20);
    if(ptr(sphere)!=base+0x03414868 || !mem(sphere+0x30,sizeof(float),true))return;
    bool gen=false,shelter=false;
    // Component ownership, rather than any radius-only/global Jolt filter.
    if(!componentTypes(ptr(t,0x48),gen,shelter) || gen==shelter)return;
    if((gen&&!genEnabled)||(shelter&&!shelterEnabled))return;
    float radius=*(volatile float*)(sphere+0x30);
    if(!(radius>=1.0f && radius<=50.0f))return;
    if(gen&&!isGeneratorRadius(radius)){InterlockedIncrement(&skips);return;}
    while(InterlockedCompareExchange(&lockFlag,1,0)!=0)Sleep(0);
    unsigned slot=0;
    for(;slot<seenCount;++slot){
        if(seen[slot].address==sphere){
            if(seen[slot].lastScaled==radius){InterlockedExchange(&lockFlag,0);return;}
            break; // A reused sphere address needs fresh scaling after a save reload.
        }
    }
    if(slot==seenCount&&seenCount>=1024){
        InterlockedIncrement(&skips);InterlockedExchange(&lockFlag,0);return;
    }
    const float scaled=radius*(float)(gen?genPercent:shelterPercent)/100.0f;
    if(!(scaled>=1.0f&&scaled<=200.0f)){InterlockedExchange(&lockFlag,0);return;}
    *(volatile float*)(sphere+0x30)=scaled; // Before original registers the body.
    if(slot==seenCount)++seenCount;
    seen[slot].address=sphere;
    seen[slot].lastScaled=scaled;
    InterlockedExchange(&lockFlag,0);
    if(gen){InterlockedIncrement(&genChanges);say(runtimeLog,"generator=TRIGGER_SPHERE_SCALED\r\n");}
    else{InterlockedIncrement(&shelterChanges);say(runtimeLog,"shelter=TRIGGER_SPHERE_SCALED\r\n");}
}
// Keep candidate generator triggers until the entity owner and Jolt body are fully active.
// The initial resource hook sometimes runs before owner+0xA8 contains charge components.
struct Pending { uintptr_t trigger; DWORD firstTick; };
static Pending pending[2048]={};
static unsigned pendingCount=0, pendingCursor=0;
static volatile LONG pendingSpin=0, pendingQueued=0, pendingCompleted=0, pendingExpired=0;
static void lockPending(){while(InterlockedCompareExchange(&pendingSpin,1,0))Sleep(0);}
static void unlockPending(){InterlockedExchange(&pendingSpin,0);}
static bool isExpectedRadius(float radius) {
    if(isGeneratorRadius(radius))return true;
    const float factor=(float)genPercent/100.0f;
    const float vanilla[]={9.0f,12.0f,15.0f};
    for(unsigned i=0;i<3;++i) {
        float scaled=vanilla[i]*factor;
        if(radius>scaled-0.08f&&radius<scaled+0.08f)return true;
    }
    return false;
}
static void trackAfterOriginal(void* source) {
    if(!genEnabled||!source||!base)return;
    uintptr_t t=(uintptr_t)source;
    if(ptr(t)!=base+0x03135648 && ptr(t)!=base+0x031360E8)return;
    uintptr_t resource=ptr(t,0x30);
    if(ptr(resource)!=base+0x03136180)return;
    uintptr_t collision=ptr(resource,0x20);
    if(ptr(collision)!=base+0x03413EF0)return;
    uintptr_t simple=ptr(collision,0xb0);
    if(ptr(simple)!=base+0x03414EE8)return;
    uintptr_t sphere=ptr(simple,0x20);
    if(ptr(sphere)!=base+0x03414868 || !mem(sphere+0x30,4))return;
    if(!isExpectedRadius(*(const float*)(sphere+0x30)))return;
    lockPending();
    for(unsigned i=0;i<pendingCount;++i) {
       if(pending[i].trigger==t){unlockPending();return;}
    }
    if(pendingCount<2048) {
       pending[pendingCount++]={t,GetTickCount()};
       InterlockedIncrement(&pendingQueued);
    } else InterlockedIncrement(&pendingExpired);
    unlockPending();
}
static void __fastcall hooked(void* trigger) {
    before(trigger);
    if(original)original(trigger);
    before(trigger); // Owner can become available only during original initialization.
    trackAfterOriginal(trigger); // Deferred Jolt refresh, even for a formerly vanilla sphere.
}
}
extern "C" unsigned SamConstructionRangesGeneratorPercent(){return range_ext::genPercent;}
extern "C" void SamConstructionRangesScaleLate(void* trigger) {
    using namespace range_ext;
    if(genEnabled && trigger)before(trigger);
}
extern "C" void SamConstructionRangesPollPending(HANDLE log) {
    using namespace range_ext;
    if(!genEnabled)return;
    unsigned budget=24;
    while(budget--) {
        lockPending();
        if(pendingCount==0){unlockPending();break;}
        if(pendingCursor>=pendingCount)pendingCursor=0;
        const unsigned index=pendingCursor;
        Pending item=pending[index];
        pendingCursor=(pendingCursor+1u)%pendingCount;
        unlockPending();
        DWORD elapsed=GetTickCount()-item.firstTick;
        if(elapsed<600u)continue;
        uintptr_t trigger=item.trigger;
        // Resource pointers are validated by before() and SamConstructionRefreshQueue().
        before((void*)trigger);
        const bool visualSynced=SamConstructionGeneratorVisualSync((void*)trigger);
        const bool physicsSynced=SamConstructionRefreshQueue((void*)trigger);
        const bool finished=visualSynced && physicsSynced;
        bool expired=elapsed>30000u;
        if(!finished&&!expired)continue;
        lockPending();
        for(unsigned i=0;i<pendingCount;++i) {
          if(pending[i].trigger==trigger && pending[i].firstTick==item.firstTick) {
            pending[i]=pending[pendingCount-1u];
            --pendingCount;
            if(pendingCount==0||pendingCursor>=pendingCount)pendingCursor=0;
            break;
          }
        }
        unlockPending();
        if(finished) {
           LONG n=InterlockedIncrement(&pendingCompleted);
           if(n<=10)say(log,"construction_ranges=GENERATOR_PENDING_RESOLVED queued_jolt_refresh\r\n");
        } else {
           LONG n=InterlockedIncrement(&pendingExpired);
           if(n<=5)say(log,"construction_ranges=PENDING_EXPIRED no_generator_owner_or_body\r\n");
        }
    }
}
extern "C" bool SamConstructionRangesInstall(void* image,const wchar_t* ini,HANDLE log) {
    using namespace range_ext;
    if(installed)return true;
    if(!image||!ini)return false;
    genEnabled=(GetPrivateProfileIntW(L"GeneratorRange",L"Enabled",0,ini)==1);
    shelterEnabled=(GetPrivateProfileIntW(L"TimefallShelterRange",L"Enabled",0,ini)==1);
    genPercent=percent(ini,L"GeneratorRange");
    shelterPercent=percent(ini,L"TimefallShelterRange");
    if(!genEnabled&&!shelterEnabled) {
        say(log,"construction_ranges=DISABLED defaults_vanilla\r\n");return true;
    }
    base=(uintptr_t)image;
    const unsigned char expected[]={
        0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x56,
        0x57,0x41,0x56,0x48,0x81,0xec,0x80,0x00,0x00,0x00};
    unsigned char* target=(unsigned char*)(base+0x002D8350);
    if(!mem((uintptr_t)target,sizeof(expected))) {
        say(log,"construction_ranges=REFUSED initializer_unreadable\r\n");return false;
    }
    for(unsigned i=0;i<sizeof(expected);++i){
        if(target[i]!=expected[i]){
            say(log,"construction_ranges=REFUSED initializer_signature_mismatch\r\n");return false;
        }
    }
    if(!SamConstructionRefreshConfigure(image,genEnabled,genPercent,log)) {
        say(log,"construction_ranges=WARNING jolt_refresh_not_ready\r\n");
    }
    MH_STATUS status=MH_Initialize();
    if(status!=MH_OK&&status!=MH_ERROR_ALREADY_INITIALIZED) {
        say(log,"construction_ranges=REFUSED minhook_initialization\r\n");return false;
    }
    status=MH_CreateHook(target,(void*)&hooked,(void**)&original);
    if(status!=MH_OK) {
        say(log,"construction_ranges=REFUSED hook_creation\r\n");return false;
    }
    status=MH_EnableHook(target);
    if(status!=MH_OK) {
        MH_RemoveHook(target);original=nullptr;
        say(log,"construction_ranges=REFUSED hook_enable\r\n");return false;
    }
    installed=true;
    if(genEnabled && !SamConstructionChargerUpdateInstall(image,log)) {
        say(log,"construction_ranges=WARNING charger_update_hook_not_installed\r\n");
    }
    makeLog();
    say(log,"construction_ranges=HOOK_INSTALLED experimental_trigger_creation\r\n");
    say(log,"construction_ranges=in_game_effects_not_yet_confirmed\r\n");
    return true;
}
extern "C" __declspec(dllexport) unsigned SamOverhaulGeneratorRangeChanges(){
    return (unsigned)range_ext::genChanges;
}
extern "C" __declspec(dllexport) unsigned SamOverhaulShelterRangeChanges(){
    return (unsigned)range_ext::shelterChanges;
}
