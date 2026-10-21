// DS2 1.10.89.0: common native cargo candidate list, AFTER its normal sort.
// Records are owned by this HUD invocation. Never retain entity/record pointers.
static const u64 RVA_SCANNED_HUD_CALL=0x011D0522ull;
static const u64 RVA_SCANNED_HUD_WINDOW=0x011D0520ull;
static const u64 RVA_SCANNED_HUD_SORT=0x01209870ull;
static const u64 RVA_SCANNED_HUD_RECENT=0x01CE6300ull;
static const u8 EXPECTED_SCANNED_HUD_WINDOW[8]={0,0,0xE8,0x49,0x93,3,0,0x48};
static const u8 EXPECTED_SCANNED_HUD_CONTEXT[]={
    0x48,0x63,0x84,0x24,0x10,0xc1,0x06,0x00,0x48,0x8d,0x94,0x24,0x10,
    0x01,0x00,0x00,0x48,0x8d,0x0c,0x40,0x48,0xc1,0xe1,0x04,0x48,0x8d,
    0x84,0x24,0x10,0x01,0x00,0x00,0x48,0x03,0xd1,0x48,0x3b,0xc2,0x74,
    0x0d,0x48,0x8d,0x8c,0x24,0x10,0x01,0x00,0x00,0xe8,0x49,0x93,0x03,0x00
};
static const u8 EXPECTED_SCANNED_HUD_SORT[]={
    0x44,0x88,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,
    0x4c,0x24,0x08,0x53,0x56,0x57,0x48,0x81,0xec,0xb0,0x00,0x00
};
static const u8 EXPECTED_SCANNED_HUD_RECENT[]={
    0x83,0x79,0x68,0x00,0x7e,0x29,0x48,0x8b,0x05,0xe3,0x86,0x55,0x04,
    0x48,0x8b,0x50,0x08,0x48,0x85,0xd2,0x75,0x0a,0xc5,0xfa,0x10,0x05,
    0xaa,0xbc,0x77,0x01,0xeb,0x05,0xc5,0xfa,0x10,0x42,0x24,0xc5,0xf8,
    0x2f,0x41,0x7c,0x76,0x03,0xb0,0x01,0xc3,0x32,0xc0,0xc3
};
typedef void (*ScannedHudSort)(u8*,u8*);
typedef u8 (*ScannedHudRecent)(u8*);
static ScannedHudSort g_scannedHudSort=0;
static FindEntityComponent g_scannedHudFind=0;
static ScannedHudRecent g_scannedHudRecent=0;
static volatile long g_scannedHudReady=0;
static SRWLOCK_X64 g_scannedHudLock={0};
struct ScannedHudOrigin{double xyz[3];double radiusSquared;u32 epoch;bool valid;};
static ScannedHudOrigin g_scannedHudOrigin={};
// Frame-scoped identity allowlist for downstream HUD radii. No entity pointers.
static u64 g_scannedHudIds[128]={};
static u32 g_scannedHudIdCount=0;
static u64 g_scannedHudIdTime=0;
static volatile long g_scannedDistanceCounts[6]={};
static volatile long g_cargoSensorCounts[4]={};
struct CargoDistanceSample{u64 id;u32 originSquaredBits;u32 hudSquaredBits;u32 radiusBits;u32 flags;};
static CargoDistanceSample g_cargoDistanceSamples[32]={};
static u32 g_cargoDistanceSampleCount=0,g_cargoDistanceSampleLogged=0;
static u32 g_scannedHudSquaredBits[128]={};
// Bounded, pointer-free evidence including candidates our filters reject.
// This distinguishes a missing native candidate from a later marker-radius veto.
struct CargoCandidateSample{u64 id;u32 squaredBits,metaFlags,nativeFlags,state,admitted;};
static CargoCandidateSample g_cargoCandidateSamples[64]={};
static u32 g_cargoCandidateCount=0,g_cargoCandidateLogged=0;
static u64 g_cargoCandidateSampleTime=0;
// Totals count frame records, not distinct cargo objects. Only worker logs them.
static volatile long g_scannedHudCounts[7]={};

static void invalidateScannedHudOrigin(){
    AcquireSRWLockExclusive(&g_scannedHudLock);
    g_scannedHudOrigin.valid=false;
    g_scannedHudOrigin.epoch++;
    g_scannedHudIdCount=0;
    g_cargoDistanceSampleCount=0;g_cargoDistanceSampleLogged=0;
    g_cargoCandidateCount=0;g_cargoCandidateLogged=0;g_cargoCandidateSampleTime=0;
    ReleaseSRWLockExclusive(&g_scannedHudLock);
}

static void publishScannedHudOrigin(const u8* manager){
    if(_InterlockedCompareExchange(&g_scannedHudReady,0,0)!=1)return;
    ScannedHudOrigin snapshot={};
    float radius=0.0f;
    if(!manager||!finitePositiveFloat(g_targetScanRadiusBits,&radius)||
       !rangeReadable(manager+0x38u,24u))return;
    memcpy(snapshot.xyz,manager+0x38u,24u);
    for(u32 i=0;i<3u;i++){
        // NaN and infinity are rejected without a CRT dependency.
        u64 bits=0;memcpy(&bits,&snapshot.xyz[i],8u);
        if((bits&0x7FF0000000000000ull)==0x7FF0000000000000ull)return;
    }
    snapshot.radiusSquared=(double)radius*(double)radius;
    snapshot.valid=true;
    AcquireSRWLockExclusive(&g_scannedHudLock);
    snapshot.epoch=g_scannedHudOrigin.epoch;
    g_scannedHudOrigin=snapshot;
    ReleaseSRWLockExclusive(&g_scannedHudLock);
}

// Mirror the native packet builder's used/destroyed predicates BEFORE changing
// the temporary permission. This preserves the entire vanilla path for rubbish.
static bool spentCargoForExtension(const u8* meta){
    if(*(const float*)(meta+0x88u)<=0.0f)return true;
    const u8* resource=*(const u8* const*)(meta+0x38u);
    if(resource&&resource[0x48u]==2u)return true;
    const u8* parts=*(const u8* const*)(meta+0xD0u);
    if(parts){
        const u8* kind=*(const u8* const*)(meta+0xC0u);
        if(kind&&*(const unsigned short*)(kind+0x20u)==0xCEu)return false;
        // Native leaf 0x12389A0: all occupied parts depleted/destroyed.
        for(u32 i=0;i<6u&&parts[i]!=0xFFu;i++){
            if((*(const unsigned short*)(parts+6u+2u*i)||
                *(const unsigned short*)(parts+0x12u+2u*i))&&!(parts[0x36u]&(1u<<i)))return false;
        }
        return true;
    }
    const u8* kind=*(const u8* const*)(meta+0xC8u);
    if(kind&&kind[0x20u]==0x4Bu)return false;
    return *(const float*)(meta+0x8Cu)<=0.0f;
}

// The observed picked-up/dropped item has flags 0x02129301 and state 1.
// Bit 0x200 alone is not a reason to exclude a world cargo item. Keep all
// native-hit/component/lifetime checks and reject other touched states.
static bool eligibleCargoMetadata(const u8* meta){
    u32 flags=*(const u32*)(meta+0xE8u);u8 state=meta[0x83u];
    return !(flags&2u)&&state!=0u&&state!=3u&&(!(flags&0x200u)||state==1u);
}

static void extendScannedHudCandidates(u8* begin,u8* end){
    if(_InterlockedCompareExchange(&g_scannedHudReady,0,0)!=1||
       _InterlockedCompareExchange(&g_hooksReady,0,0)!=1)return;
    ScannedHudOrigin origin;
    u64 sampleTime=GetTickCount64();bool sampleCandidates=false;
    AcquireSRWLockShared(&g_scannedHudLock);
    origin=g_scannedHudOrigin;
    sampleCandidates=g_debugLog&&g_cargoCandidateCount<64u&&sampleTime-g_cargoCandidateSampleTime>=1000u;
    ReleaseSRWLockShared(&g_scannedHudLock);
    if(!origin.valid)return;
    // Native list can be much larger; bound our extra work and pass through
    // crowded/invalid batches without changing the game's own processing.
    u64 size=(u64)end-(u64)begin;
    if(!begin||(u64)end<(u64)begin||size>1024u*48u||size%48u){
        AcquireSRWLockExclusive(&g_scannedHudLock);
        if(g_scannedHudOrigin.epoch==origin.epoch)g_scannedHudIdCount=0;
        ReleaseSRWLockExclusive(&g_scannedHudLock);
        _InterlockedIncrement(&g_scannedHudCounts[6]);return;
    }
    u32 counts[7]={1u,(u32)(size/48u),0,0,0,0,0};
    u64 ids[128];u32 squaredBits[128];u32 idCount=0;
    // Builder retains only the first 128 sorted common-list records.
    if(size>128u*48u)end=begin+128u*48u;
    for(u8* record=begin;record<end;record+=48u){
        if(record[0x0Cu]!=1u)continue;
        u8* meta=*(u8**)record;
        if(!meta)continue;
        if(!eligibleCargoMetadata(meta))continue;
        double dx=*(double*)(meta+0x40u)-origin.xyz[0];
        double dy=*(double*)(meta+0x48u)-origin.xyz[1];
        double dz=*(double*)(meta+0x50u)-origin.xyz[2];
        double squared=dx*dx+dy*dy+dz*dz;
        if(!(squared>=0.0&&squared<=origin.radiusSquared)){counts[3]++;continue;}
        if(spentCargoForExtension(meta))continue;
        u8* anchor=*(u8**)(meta+0xF8u);
        if(!anchor){counts[4]++;continue;}
        u8* detected=g_scannedHudFind(anchor+0x80u,g_gameBase+0x04490CD0u);
        if(!detected||*(u64*)detected!=(u64)(g_gameBase+RVA_DETECTED_COMPONENT_VTABLE)||
           *(u8**)(detected+0x48u)!=anchor-0x20u||detected[0x51u]!=1u||
           detected[0x56u]!=1u){counts[4]++;continue;}
        // Keep the native recent-scan window. This never extends a lifetime,
        // generates a hit, forces realization, or modifies persistent flags.
        if(!g_scannedHudRecent(detected)){counts[5]++;continue;}
        if(record[0x0Eu]==0u){record[0x0Eu]=1u;counts[2]++;}
        u64 id=*(u64*)(meta+0x18u);
        if(id){
            ids[idCount]=id;
            float distance=(float)squared;memcpy(&squaredBits[idCount],&distance,4u);
            idCount++;
        }
    }
    CargoCandidateSample candidates[32];u32 candidateCount=0;
    if(sampleCandidates)for(u8* record=begin;record<end&&candidateCount<32u;record+=48u){
        const u8* meta=*(const u8* const*)record;if(!meta)continue;
        CargoCandidateSample* sample=&candidates[candidateCount++];
        sample->id=*(const u64*)(meta+0x18u);
        double squared=0.0;
        for(u32 axis=0;axis<3u;axis++){
            double delta=*(const double*)(meta+0x40u+8u*axis)-origin.xyz[axis];squared+=delta*delta;
        }
        sample->squaredBits=floatBits((float)squared);
        sample->metaFlags=*(const u32*)(meta+0xE8u);sample->state=meta[0x83u];
        sample->nativeFlags=(u32)record[0x0Cu]|((u32)record[0x0Du]<<8u)|((u32)record[0x0Eu]<<16u);
        sample->admitted=0;
        for(u32 i=0;i<idCount;i++)if(ids[i]==sample->id){sample->admitted=1;break;}
    }
    u64 now=GetTickCount64();
    AcquireSRWLockExclusive(&g_scannedHudLock);
    if(g_scannedHudOrigin.valid&&g_scannedHudOrigin.epoch==origin.epoch){
        memcpy(g_scannedHudIds,ids,idCount*sizeof(u64));
        memcpy(g_scannedHudSquaredBits,squaredBits,idCount*sizeof(u32));
        g_scannedHudIdCount=idCount;g_scannedHudIdTime=now;
        if(sampleCandidates){
            g_cargoCandidateSampleTime=sampleTime;
            for(u32 i=0;i<candidateCount&&g_cargoCandidateCount<64u;i++){
                bool seen=false;
                for(u32 j=0;j<g_cargoCandidateCount;j++){
                    const CargoCandidateSample* previous=&g_cargoCandidateSamples[j];
                    if(previous->id==candidates[i].id&&previous->nativeFlags==candidates[i].nativeFlags&&
                       previous->metaFlags==candidates[i].metaFlags&&previous->state==candidates[i].state&&
                       previous->admitted==candidates[i].admitted){seen=true;break;}
                }
                if(!seen)g_cargoCandidateSamples[g_cargoCandidateCount++]=candidates[i];
            }
        }
    }
    ReleaseSRWLockExclusive(&g_scannedHudLock);
    for(u32 i=0;i<7u;i++)_InterlockedExchangeAdd(&g_scannedHudCounts[i],(long)counts[i]);
}

extern "C" void scannedCargoHudHook(u8* begin,u8* end){
    g_scannedHudSort(begin,end);
    extendScannedHudCandidates(begin,end);
}

static void logScannedHudTelemetry(){
    static u32 ticks=0,lastPromoted=0,lastRejected=0,lastBatches=0,lastSensor=0;
    if(++ticks%8u)return; // worker polls every 500 ms
    u32 values[7];
    for(u32 i=0;i<7u;i++)values[i]=(u32)_InterlockedCompareExchange(&g_scannedHudCounts[i],0,0);
    // Also report batches without promotions to diagnose an unproductive path.
    u32 sensor=(u32)_InterlockedCompareExchange(&g_cargoSensorCounts[0],0,0);
    if(values[0]==lastBatches&&values[2]==lastPromoted&&values[6]==lastRejected&&sensor==lastSensor)return;
    lastSensor=sensor;
    lastBatches=values[0];lastPromoted=values[2];lastRejected=values[6];
    const char* labels[7]={" batches="," records="," promoted="," outside="," detectedRejected="," expired="," boundsRejected="};
    char buffer[384];u32 pos=appendText(buffer,0u,384u,"SCANNED-HUD: frame totals");
    for(u32 i=0;i<7u;i++){
        pos=appendText(buffer,pos,384u,labels[i]);
        pos=appendUnsigned(buffer,pos,384u,values[i]);
    }
    pos=appendText(buffer,pos,384u,"\r\n");logRaw(buffer,pos);
    const char* distanceLabels[6]={" calls="," eligible="," radiusRaised="," idMiss="," staleBatch="," spentVanilla="};
    pos=appendText(buffer,0u,384u,"SCANNED-DISTANCE:");
    for(u32 i=0;i<6u;i++){
        pos=appendText(buffer,pos,384u,distanceLabels[i]);
        pos=appendUnsigned(buffer,pos,384u,(u32)_InterlockedCompareExchange(&g_scannedDistanceCounts[i],0,0));
    }
    pos=appendText(buffer,pos,384u,"\r\n");logRaw(buffer,pos);
    const char* sensorLabels[4]={" limitedCandidates="," cargoMatched="," retries="," accepted="};
    pos=appendText(buffer,0u,384u,"CARGO-SENSOR:");
    for(u32 i=0;i<4u;i++){
        pos=appendText(buffer,pos,384u,sensorLabels[i]);
        pos=appendUnsigned(buffer,pos,384u,(u32)_InterlockedCompareExchange(&g_cargoSensorCounts[i],0,0));
    }
    pos=appendText(buffer,pos,384u,"\r\n");logRaw(buffer,pos);
    CargoCandidateSample candidates[64];u32 candidateCount=0;
    AcquireSRWLockExclusive(&g_scannedHudLock);
    for(u32 i=g_cargoCandidateLogged;i<g_cargoCandidateCount;i++)candidates[candidateCount++]=g_cargoCandidateSamples[i];
    g_cargoCandidateLogged=g_cargoCandidateCount;
    ReleaseSRWLockExclusive(&g_scannedHudLock);
    for(u32 i=0;i<candidateCount;i++){
        pos=appendText(buffer,0u,384u,"CARGO-CANDIDATE: id=");pos=appendHex64(buffer,pos,384u,candidates[i].id);
        pos=appendText(buffer,pos,384u," originSqBits=");pos=appendHex32(buffer,pos,384u,candidates[i].squaredBits);
        pos=appendText(buffer,pos,384u," metaFlags=");pos=appendHex32(buffer,pos,384u,candidates[i].metaFlags);
        pos=appendText(buffer,pos,384u," nativeFlags=");pos=appendHex32(buffer,pos,384u,candidates[i].nativeFlags);
        pos=appendText(buffer,pos,384u," state=");pos=appendUnsigned(buffer,pos,384u,candidates[i].state);
        pos=appendText(buffer,pos,384u," admitted=");pos=appendUnsigned(buffer,pos,384u,candidates[i].admitted);
        pos=appendText(buffer,pos,384u,"\r\n");logRaw(buffer,pos);
    }
    CargoDistanceSample samples[32];u32 count=0;
    AcquireSRWLockExclusive(&g_scannedHudLock);
    for(u32 i=g_cargoDistanceSampleLogged;i<g_cargoDistanceSampleCount;i++)samples[count++]=g_cargoDistanceSamples[i];
    g_cargoDistanceSampleLogged=g_cargoDistanceSampleCount;
    ReleaseSRWLockExclusive(&g_scannedHudLock);
    for(u32 i=0;i<count;i++){
        pos=appendText(buffer,0u,384u,"CARGO-DISTANCE-SAMPLE: id=");
        pos=appendHex64(buffer,pos,384u,samples[i].id);
        pos=appendText(buffer,pos,384u," originSqBits=");pos=appendHex32(buffer,pos,384u,samples[i].originSquaredBits);
        pos=appendText(buffer,pos,384u," hudSqBits=");pos=appendHex32(buffer,pos,384u,samples[i].hudSquaredBits);
        pos=appendText(buffer,pos,384u," radiusBits=");pos=appendHex32(buffer,pos,384u,samples[i].radiusBits);
        pos=appendText(buffer,pos,384u," flags=");pos=appendHex32(buffer,pos,384u,samples[i].flags);
        pos=appendText(buffer,pos,384u,"\r\n");logRaw(buffer,pos);
    }
}

static bool validateScannedHudTargets(const GameImage* image){
    if(!image||image->imageSize!=EXPECTED_IMAGE_SIZE)return false;
    u8* base=image->base;
    if(!bytesEqual(base+0x011D04F2u,EXPECTED_SCANNED_HUD_CONTEXT,sizeof(EXPECTED_SCANNED_HUD_CONTEXT))||
       !bytesEqual(base+RVA_SCANNED_HUD_WINDOW,EXPECTED_SCANNED_HUD_WINDOW,8u)||
       !bytesEqual(base+RVA_SCANNED_HUD_SORT,EXPECTED_SCANNED_HUD_SORT,sizeof(EXPECTED_SCANNED_HUD_SORT))||
       !bytesEqual(base+RVA_SCANNED_HUD_RECENT,EXPECTED_SCANNED_HUD_RECENT,sizeof(EXPECTED_SCANNED_HUD_RECENT))||
       !bytesEqual(base+RVA_FIND_ENTITY_COMPONENT,EXPECTED_FIND_ENTITY_COMPONENT,sizeof(EXPECTED_FIND_ENTITY_COMPONENT)))return false;
    for(u32 i=0;i<4u;i++){
        u64 value=0;
        if(!readU64(base+RVA_DETECTED_COMPONENT_VTABLE+8u*i,&value)||
           value!=(u64)(base+DETECTED_COMPONENT_VFUNCS[i]))return false;
        if(!readU64(base+RVA_BAGGAGE_COMPONENT_VTABLE+8u*i,&value)||
           value!=(u64)(base+BAGGAGE_COMPONENT_VFUNCS[i]))return false;
    }
    return true;
}

static bool scannedHudPatchMatches(const u8* base,u64 replacement){
    u64 current=0;
    return base&&readU64(base+RVA_SCANNED_HUD_WINDOW,&current)&&current==replacement;
}

static ApplyResult rollbackScannedHudPatch(
    u8* base,u64 replacement,DWORD originalProtection
){
    static const u64 expected=0x4800039349E80000ull;
    u8* address=base+RVA_SCANNED_HUD_WINDOW;
    DWORD beforeRollback=0;
    bool writable=VirtualProtect(address,8u,PAGE_EXECUTE_READWRITE,&beforeRollback)!=FALSE;
    bool restoredBytes=false;
    if(writable){
        u64 current=0;
        if(readU64(address,&current)){
            if(current==expected){
                restoredBytes=true;
            }else if(current==replacement){
                u64 observed=0;
                restoredBytes=compareExchangeU64(
                    address,expected,replacement,&observed
                )&&(observed==replacement||observed==expected);
            }
        }
        if(!FlushInstructionCache(GetCurrentProcess(),address,8u))restoredBytes=false;
        if(!readU64(address,&current)||current!=expected)restoredBytes=false;
    }
    DWORD ignored=0;
    bool restoredProtection=writable&&
        VirtualProtect(address,8u,originalProtection,&ignored)!=FALSE&&
        protectionMatchesAfterOwnedWrite(address,originalProtection);
    return restoredBytes&&restoredProtection?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult installScannedHud(
    u8* base,u32 imageSize,u8** relayOut
){
    static const u64 expected=0x4800039349E80000ull;
    if(!base||!imageSize||!relayOut||
       ((u64)(base+RVA_SCANNED_HUD_WINDOW)&7u)!=0u)return APPLY_FAILED;
    *relayOut=0;
    if(!bytesEqual(
        base+RVA_SCANNED_HUD_WINDOW,EXPECTED_SCANNED_HUD_WINDOW,
        (u32)sizeof(EXPECTED_SCANNED_HUD_WINDOW)
    ))return APPLY_CONFLICT;
    u8* relay=allocateRelay(
        base,imageSize,RVA_SCANNED_HUD_CALL,(u64)(const void*)&scannedCargoHudHook,false
    );
    if(!relay)return APPLY_FAILED;
    u32 displacement=0;
    if(!relayDisplacement(base+RVA_SCANNED_HUD_CALL,relay,&displacement)){
        VirtualFree(relay,0,MEM_RELEASE);
        return APPLY_FAILED;
    }
    u8 replacementBytes[8];
    memcpy(replacementBytes,EXPECTED_SCANNED_HUD_WINDOW,8u);
    memcpy(replacementBytes+3u,&displacement,4u);
    u64 replacement=0;
    memcpy(&replacement,replacementBytes,8u);

    DWORD originalProtection=0;
    u8* address=base+RVA_SCANNED_HUD_WINDOW;
    if(!VirtualProtect(address,8u,PAGE_EXECUTE_READWRITE,&originalProtection)){
        VirtualFree(relay,0,MEM_RELEASE);
        return APPLY_FAILED;
    }
    if(!bytesEqual(
        address,EXPECTED_SCANNED_HUD_WINDOW,
        (u32)sizeof(EXPECTED_SCANNED_HUD_WINDOW)
    )){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,8u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        VirtualFree(relay,0,MEM_RELEASE);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    u64 observed=0;
    bool published=compareExchangeU64(address,replacement,expected,&observed)&&observed==expected;
    if(!published){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,8u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        VirtualFree(relay,0,MEM_RELEASE);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    if(!FlushInstructionCache(GetCurrentProcess(),address,8u)||
       !scannedHudPatchMatches(base,replacement)){
        *relayOut=relay;
        return rollbackScannedHudPatch(base,replacement,originalProtection);
    }
    DWORD ignored=0;
    if(!VirtualProtect(address,8u,originalProtection,&ignored)||
       !protectionMatchesAfterOwnedWrite(address,originalProtection)){
        *relayOut=relay;
        return rollbackScannedHudPatch(base,replacement,originalProtection);
    }
    *relayOut=relay;
    return APPLY_READY;
}
