// HUD permissions do not themselves set a display radius. The native alpha,
// selection and label paths calculate it again (often 25/30/200 m). Extend only
// packets whose stable IDs were admitted by the current native cargo batch.
typedef float (*CargoHudRadius)(u8*,u8*);
typedef float (*CargoHudGeneralRadius)(u8*,u32,u8*,u64,u8);
static CargoHudRadius g_cargoHudRadius=0;
static CargoHudGeneralRadius g_cargoHudGeneralRadius=0;

static float scopedCargoHudRadius(float nativeRadius,const u8* packet,const u8* context){
    _InterlockedIncrement(&g_scannedDistanceCounts[0]);
    if(!packet||_InterlockedCompareExchange(&g_scannedHudReady,0,0)!=1||
       _InterlockedCompareExchange(&g_hooksReady,0,0)!=1)return nativeRadius;
    // Used/destroyed cargo is excluded from our extension, not from vanilla HUD.
    if(packet[0x4Eu]&0x60u){
        _InterlockedIncrement(&g_scannedDistanceCounts[5]);return nativeRadius;
    }
    // Preserve native disabled/sentinel radii and other special restrictions.
    if(!(nativeRadius>0.0f)||!(packet[0x4Eu]&2u)||!(packet[0x2Cu]&4u))return nativeRadius;
    u64 id=*(const u64*)(packet+0x20u);
    if(!id)return nativeRadius;
    u64 now=GetTickCount64();bool matched=false,stale=false;
    u32 originSquaredBits=0,epoch=0;
    AcquireSRWLockShared(&g_scannedHudLock);
    stale=!g_scannedHudOrigin.valid||now-g_scannedHudIdTime>250u;
    epoch=g_scannedHudOrigin.epoch;
    if(!stale){
        for(u32 i=0;i<g_scannedHudIdCount;i++){
            if(g_scannedHudIds[i]==id){matched=true;originSquaredBits=g_scannedHudSquaredBits[i];break;}
        }
    }
    ReleaseSRWLockShared(&g_scannedHudLock);
    if(!matched){
        _InterlockedIncrement(&g_scannedDistanceCounts[stale?4u:3u]);
        return nativeRadius;
    }
    _InterlockedIncrement(&g_scannedDistanceCounts[1]);
    float target=0.0f;
    if(finitePositiveFloat(g_targetScanRadiusBits,&target)&&nativeRadius<target){
        _InterlockedIncrement(&g_scannedDistanceCounts[2]);
        if(g_debugLog&&context){
            // At most 32 unique IDs per scan. Only the worker writes the log.
            AcquireSRWLockExclusive(&g_scannedHudLock);
            if(g_scannedHudOrigin.valid&&epoch==g_scannedHudOrigin.epoch&&g_cargoDistanceSampleCount<32u){
                bool seen=false;
                for(u32 i=0;i<g_cargoDistanceSampleCount;i++)if(g_cargoDistanceSamples[i].id==id)seen=true;
                if(!seen){
                    CargoDistanceSample* sample=&g_cargoDistanceSamples[g_cargoDistanceSampleCount++];
                    sample->id=id;sample->originSquaredBits=originSquaredBits;
                    float squared=0.0f;
                    for(u32 axis=0;axis<3u;axis++){
                        float delta=*(const float*)(packet+0x10u+4u*axis)-*(const float*)(context+0x20u+4u*axis);
                        squared+=delta*delta;
                    }
                    memcpy(&sample->hudSquaredBits,&squared,4u);
                    sample->radiusBits=g_targetScanRadiusBits;sample->flags=packet[0x4Eu];
                }
            }
            ReleaseSRWLockExclusive(&g_scannedHudLock);
        }
        return target;
    }
    return nativeRadius;
}

extern "C" float cargoHudRadiusHook(u8* context,u8* packet){
    float radius=g_cargoHudRadius(context,packet);
    return scopedCargoHudRadius(radius,packet,context);
}

extern "C" float cargoHudGeneralRadiusHook(u8* context,u32 type,u8* packet,u64 extra,u8 mode){
    float radius=g_cargoHudGeneralRadius(context,type,packet,extra,mode);
    // These exact icon types are the cargo branches of the verified jump table.
    // Other entity types can have smaller, unrelated packet layouts.
    u32 kind=type&0xFFFFu;
    if(kind<0xBDu||kind>0xD0u)return radius;
    return scopedCargoHudRadius(radius,packet,context);
}

struct HudDistanceCall{
    u32 callRva,windowRva,targetRva;
    u8 expected[8];
};
// Definitions generated from the exact supported image and verified offline.
#include "scanned_cargo_distance_anchors.inl"

struct HudDistancePatch{
    u8* relay;
    u64 replacement;
    DWORD protection;
    bool published;
};
static HudDistancePatch g_hudDistancePatches[5]={};

static bool validateHudDistanceTargets(const GameImage* image){
    if(!image||image->imageSize!=EXPECTED_IMAGE_SIZE)return false;
    if(!bytesEqual(image->base+0x17CE830u,EXPECTED_CARGO_RADIUS,sizeof(EXPECTED_CARGO_RADIUS))||
       !bytesEqual(image->base+0x17CDDD0u,EXPECTED_CARGO_GENERAL_RADIUS,sizeof(EXPECTED_CARGO_GENERAL_RADIUS))||
       !bytesEqual(image->base+0x17CE700u,EXPECTED_CARGO_TYPE_TABLE,sizeof(EXPECTED_CARGO_TYPE_TABLE)))return false;
    for(u32 i=0;i<5u;i++){
        const HudDistanceCall* site=&HUD_DISTANCE_CALLS[i];
        u32 rel=(u32)(site->targetRva-site->callRva-5u);
        if(site->windowRva&7u||site->callRva+1u<site->windowRva||
           site->callRva+5u>site->windowRva+8u||
           !bytesEqual(image->base+site->windowRva,site->expected,8u)||
           image->base[site->callRva]!=0xE8u||
           !bytesEqual(image->base+site->callRva+1u,(u8*)&rel,4u))return false;
    }
    return true;
}

static ApplyResult rollbackExactCall(u8* base,const HudDistanceCall* site,HudDistancePatch* state){
    u8* address=base+site->windowRva;
    DWORD before=0,ignored=0;u64 expected=0,observed=0;
    memcpy(&expected,site->expected,8u);
    if(!VirtualProtect(address,8u,PAGE_EXECUTE_READWRITE,&before))return APPLY_CRITICAL;
    bool restored=compareExchangeU64(address,expected,state->replacement,&observed)&&
        (observed==expected||observed==state->replacement);
    bool flushed=FlushInstructionCache(GetCurrentProcess(),address,8u)!=FALSE;
    bool protectedAgain=VirtualProtect(address,8u,state->protection,&ignored)!=FALSE&&
        protectionMatchesAfterOwnedWrite(address,state->protection);
    return restored&&flushed&&protectedAgain&&bytesEqual(address,site->expected,8u)?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult installExactCall(u8* base,const HudDistanceCall* site,u64 wrapper,HudDistancePatch* state,u32 captureMode=0u){
    u8* address=base+site->windowRva;
    if(!bytesEqual(address,site->expected,8u)||base[site->callRva]!=0xE8u)return APPLY_CONFLICT;
    u8* relay=allocateRelay(base,EXPECTED_IMAGE_SIZE,site->callRva,wrapper,captureMode);
    if(!relay)return APPLY_FAILED;
    u32 displacement=0;
    if(!relayDisplacement(base+site->callRva,relay,&displacement)){
        VirtualFree(relay,0,MEM_RELEASE);return APPLY_FAILED;
    }
    u8 changed[8];u64 expected=0,observed=0;
    memcpy(changed,site->expected,8u);memcpy(&expected,changed,8u);
    memcpy(changed+site->callRva+1u-site->windowRva,&displacement,4u);
    memcpy(&state->replacement,changed,8u);
    if(!VirtualProtect(address,8u,PAGE_EXECUTE_READWRITE,&state->protection)){
        VirtualFree(relay,0,MEM_RELEASE);return APPLY_FAILED;
    }
    bool published=compareExchangeU64(address,state->replacement,expected,&observed)&&observed==expected;
    if(!published){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,8u,state->protection,&ignored)!=FALSE&&
            protectionMatchesAfterOwnedWrite(address,state->protection);
        VirtualFree(relay,0,MEM_RELEASE);return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    state->relay=relay;state->published=true;
    bool flushed=FlushInstructionCache(GetCurrentProcess(),address,8u)!=FALSE;
    bool verified=bytesEqual(address,changed,8u);
    DWORD ignored=0;
    bool restored=VirtualProtect(address,8u,state->protection,&ignored)!=FALSE&&
        protectionMatchesAfterOwnedWrite(address,state->protection);
    if(!flushed||!verified||!restored)return rollbackExactCall(base,site,state);
    return APPLY_READY;
}

static ApplyResult rollbackHudDistanceCall(u8* base,u32 index,HudDistancePatch* state){
    return rollbackExactCall(base,&HUD_DISTANCE_CALLS[index],state);
}

static ApplyResult installHudDistanceCall(u8* base,u32 index,HudDistancePatch* state){
    const HudDistanceCall* site=&HUD_DISTANCE_CALLS[index];
    u64 wrapper=site->targetRva==0x17CE830u?(u64)&cargoHudRadiusHook:(u64)&cargoHudGeneralRadiusHook;
    return installExactCall(base,site,wrapper,state);
}

static ApplyResult installHudDistanceCalls(u8* base){
    g_cargoHudRadius=(CargoHudRadius)(base+0x17CE830u);
    g_cargoHudGeneralRadius=(CargoHudGeneralRadius)(base+0x17CDDD0u);
    for(u32 i=0;i<5u;i++){
        ApplyResult result=installHudDistanceCall(base,i,&g_hudDistancePatches[i]);
        if(result==APPLY_READY)continue;
        for(u32 previous=i;previous>0u;){
            previous--;
            if(rollbackHudDistanceCall(base,previous,&g_hudDistancePatches[previous])==APPLY_CRITICAL)result=APPLY_CRITICAL;
        }
        return result;
    }
    return APPLY_READY;
}
