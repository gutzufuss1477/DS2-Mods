// The cargo detail job owns its text widgets. Append to its temporary native
// row's final string before native layout/measurement, never to widget memory.
typedef void (*RecipientRender)(u8*,u8*,float);
typedef char** (*RecipientGet)(char**,u8*);
typedef void (*RecipientStringSet)(char**,const char*,u32);
typedef void (*RecipientFormat)(char**,u32,const void*);
static RecipientRender g_recipientRender=0;
static RecipientGet g_recipientGet=0;
static RecipientStringSet g_recipientStringSet=0;
static RecipientFormat g_recipientFormat=0;
static DWORD g_recipientTls=0xFFFFFFFFu;
static volatile long g_recipientReady=0,g_recipientUpdates=0;
static HudDistancePatch g_recipientPatches[3]={};
static SRWLOCK_X64 g_recipientLock={0};
struct RecipientCache{u64 controller,id,time;u32 type,meters;bool shown;};
static RecipientCache g_recipientCache[16]={};
struct RecipientScope{u64 id;u32 meters;bool appended,useLikes;};
#include "cargo_recipient_anchors.inl"

static bool recipientMeterReady(u8* controller,const u8* record){
    if(_InterlockedCompareExchange(&g_recipientReady,0,0)!=1)return false;
    bool found=false;u64 now=GetTickCount64();
    AcquireSRWLockShared(&g_recipientLock);
    for(u32 i=0;i<16u;i++){
        const RecipientCache* cache=&g_recipientCache[i];
        if(cache->shown&&cache->controller==(u64)controller&&cache->id==*(const u64*)record&&
           cache->type==*(const unsigned short*)(record+8u)&&now-cache->time<1000u){found=true;break;}
    }
    ReleaseSRWLockShared(&g_recipientLock);
    return found;
}

static bool recipientDistance(u8* controller,u8* detail,u32* slot,u32* meters){
    if(!controller||!detail)return false;
    u64 offset=(u64)detail-((u64)controller+0x231B0u);
    if(offset>=16u*0x830u||offset%0x830u)return false;
    *slot=(u32)(offset/0x830u);
    u32 kind=*(const unsigned short*)(detail+8u);
    if(kind<0xBDu||kind>0xD0u||detail[0x30u]!=1u||!*(const u64*)detail)return false;
    // Read only the frame's prepared icon data. Native jobs also search this
    // array by ID/type; no world entity lookup or cross-job record writes.
    for(u32 i=0;i<256u;i++){
        const u8* icon=controller+0x1B0u+i*0x230u;
        if(*(const u64*)icon!=*(const u64*)detail||*(const unsigned short*)(icon+8u)!=kind||
           icon[0x28u]!=1u||icon[0x70u]!=0u)continue;
        float distance=*(const float*)(icon+0x74u);
        if(!(distance>=0.0f&&distance<=10000.0f))return false;
        *meters=(u32)distance;return true;
    }
    return false;
}

// Same reward-row branch as native 17EA2F3 (including 122F430's predicate).
// These are metadata already owned/read by this native detail job.
static bool recipientHasLikes(const u8* baggage){
    const u8* resource=*(const u8* const*)(baggage+0x38u);
    if(!resource||resource[0x48u]==1u)return false;
    if(*(const u32*)(baggage+0xE8u)&0x8000u)return true;
    u64 token=*(const u64*)(baggage+0x28u);
    return (baggage[0xECu]&1u)&&*(const u32*)(baggage+0x120u)>=2u&&
        (token&0x1FFFFFFFu)&&((token>>32u)&0x3Fu);
}

static void appendRecipientMeters(char** out,RecipientScope* scope){
    if(!out||!*out||!scope||scope->appended)return;
    const char* text=*out;
    u32 length=*(const u32*)(text-8u);
    // Preserve all UTF-8/markup bytes, and let the game's allocator own the
    // replacement. No truncation of names and no suffix on an empty recipient.
    if(!length||length>480u||text[length]!=0)return;
    char buffer[512];memcpy(buffer,text,length);
    u32 pos=appendText(buffer,length,sizeof(buffer)," | ");
    pos=appendUnsigned(buffer,pos,sizeof(buffer),scope->meters);
    pos=appendText(buffer,pos,sizeof(buffer)," m");buffer[pos]=0;
    g_recipientStringSet(out,buffer,pos);
    scope->appended=true;_InterlockedIncrement(&g_recipientUpdates);
}

extern "C" char** recipientNameHook(char** out,u8* baggage){
    char** result=g_recipientGet(out,baggage);
    if(_InterlockedCompareExchange(&g_hooksReady,0,0)!=1||
       _InterlockedCompareExchange(&g_recipientReady,0,0)!=1||
       g_recipientTls==0xFFFFFFFFu||!out||result!=out||!baggage)return result;
    RecipientScope* scope=(RecipientScope*)TlsGetValue(g_recipientTls);
    if(!scope||*(const u64*)(baggage+0x18u)!=scope->id)return result;
    scope->useLikes=recipientHasLikes(baggage);
    if(!scope->useLikes)appendRecipientMeters(out,scope);
    return result;
}

extern "C" void recipientLikesHook(char** out,u32 index,const void* argument){
    g_recipientFormat(out,index,argument);
    if(_InterlockedCompareExchange(&g_hooksReady,0,0)!=1||
       _InterlockedCompareExchange(&g_recipientReady,0,0)!=1||
       g_recipientTls==0xFFFFFFFFu||index!=2u)return;
    RecipientScope* scope=(RecipientScope*)TlsGetValue(g_recipientTls);
    // Only callsite 17EA3AA. Slots 2 and 3 have native type=0 (no-op), while
    // slot 0 has already inserted the likes. Slot 3 preserves our final text.
    if(scope&&scope->useLikes)appendRecipientMeters(out,scope);
}

extern "C" void recipientRenderHook(u8* controller,u8* detail,float elapsed){
    if(_InterlockedCompareExchange(&g_hooksReady,0,0)!=1||
       _InterlockedCompareExchange(&g_scannedHudReady,0,0)!=1||
       _InterlockedCompareExchange(&g_recipientReady,0,0)!=1){
        g_recipientRender(controller,detail,elapsed);return;
    }
    u32 slot=0xFFFFFFFFu,meters=0;
    if(!recipientDistance(controller,detail,&slot,&meters)){
        if(slot<16u){
            AcquireSRWLockExclusive(&g_recipientLock);
            if(g_recipientCache[slot].controller==(u64)controller)g_recipientCache[slot].shown=false;
            ReleaseSRWLockExclusive(&g_recipientLock);
        }
        g_recipientRender(controller,detail,elapsed);return;
    }
    u64 id=*(const u64*)detail;u32 type=*(const unsigned short*)(detail+8u);
    AcquireSRWLockShared(&g_recipientLock);
    RecipientCache previous=g_recipientCache[slot];
    ReleaseSRWLockShared(&g_recipientLock);
    bool cached=previous.shown&&previous.controller==(u64)controller&&previous.id==id&&
        previous.type==type&&previous.meters==meters;
    RecipientScope scope={id,meters,false,false};
    LPVOID prior=TlsGetValue(g_recipientTls);
    if(!TlsSetValue(g_recipientTls,&scope)){g_recipientRender(controller,detail,elapsed);return;}
    // Rebuild only when the integer changes or native code marks text dirty.
    if(!cached)detail[0x31u]=1u;
    bool rebuilding=detail[0x31u]!=0u;
    g_recipientRender(controller,detail,elapsed);
    TlsSetValue(g_recipientTls,prior);
    RecipientCache current={(u64)controller,id,GetTickCount64(),type,meters,
        scope.appended||(!rebuilding&&cached)};
    AcquireSRWLockExclusive(&g_recipientLock);
    g_recipientCache[slot]=current;
    ReleaseSRWLockExclusive(&g_recipientLock);
}

static ApplyResult installRecipientMeter(u8* base){
    g_recipientTls=TlsAlloc();
    if(g_recipientTls==0xFFFFFFFFu)return APPLY_FAILED;
    g_recipientRender=(RecipientRender)(base+RECIPIENT_CALLS[0].targetRva);
    g_recipientGet=(RecipientGet)(base+RECIPIENT_CALLS[1].targetRva);
    g_recipientStringSet=(RecipientStringSet)(base+0xA4100u);
    g_recipientFormat=(RecipientFormat)(base+RECIPIENT_CALLS[2].targetRva);
    const u64 wrappers[3]={(u64)&recipientRenderHook,(u64)&recipientNameHook,(u64)&recipientLikesHook};
    for(u32 i=0;i<3u;i++){
        u64 wrapper=wrappers[i];
        ApplyResult result=installExactCall(base,&RECIPIENT_CALLS[i],wrapper,&g_recipientPatches[i]);
        if(result==APPLY_READY)continue;
        for(u32 n=i;n>0u;){u32 prior=--n;
            if(rollbackExactCall(base,&RECIPIENT_CALLS[prior],&g_recipientPatches[prior])==APPLY_CRITICAL)
                result=APPLY_CRITICAL;
        }
        // Keep a published relay and its TLS slot alive on failure; readiness
        // stays false, so the wrapper is pass-through even during rollback.
        return result;
    }
    _InterlockedExchange(&g_recipientReady,1);return APPLY_READY;
}

static void logRecipientMeter(){
    static u32 last=0;u32 count=(u32)_InterlockedCompareExchange(&g_recipientUpdates,0,0);
    if(count==last)return;last=count;
    char buffer[96];u32 pos=appendText(buffer,0,96u,"RECIPIENT-METERS: nativeTextUpdates=");
    pos=appendUnsigned(buffer,pos,96u,count);pos=appendText(buffer,pos,96u,"\r\n");logRaw(buffer,pos);
}
