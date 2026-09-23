static u32 distanceTestCalls=0;
static u8* distanceTestContext=0;
static u8* distanceTestPacket=0;
static u32 distanceTestType=0;
static u64 distanceTestExtra=0;
static u8 distanceTestMode=0;
static float distanceTestOriginal=25.0f;
static float distanceTestRadius(u8* context,u8* packet){
    distanceTestCalls++;distanceTestContext=context;distanceTestPacket=packet;
    return distanceTestOriginal;
}
static float distanceTestGeneral(u8* context,u32 type,u8* packet,u64 extra,u8 mode){
    distanceTestType=type;distanceTestExtra=extra;distanceTestMode=mode;
    return distanceTestRadius(context,packet);
}
static void distanceTestFixture(u8* base,u8* packet){
    u8 record[48];
    hudTestFixture(base,record);
    scannedCargoHudHook(record,record+48u);
    memset(packet,0,0x50u);
    *(u64*)(packet+0x20u)=*(u64*)(hudTestMeta+0x18u);
    packet[0x2Cu]=4;packet[0x4Eu]=2;
    g_cargoHudRadius=distanceTestRadius;g_cargoHudGeneralRadius=distanceTestGeneral;
    distanceTestCalls=0;distanceTestOriginal=25.0f;
}

extern "C" __declspec(dllexport) u32 RunScannedDistanceTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 9200u;
    u8 packet[0x50],before[0x50],context[0x30]={};
    distanceTestFixture(base,packet);memcpy(before,packet,sizeof(packet));
    if(cargoHudRadiusHook(context,packet)!=500.0f||distanceTestCalls!=1u||
       distanceTestContext!=context||distanceTestPacket!=packet||
       !bytesEqual(packet,before,sizeof(packet)))return 9201u;
    for(u32 kind=0xBDu;kind<=0xD0u;kind++){
        distanceTestCalls=0;
        if(cargoHudGeneralRadiusHook(context,kind,packet,0xFEDCBA9876543210ull,1u)!=500.0f||
           distanceTestCalls!=1u||distanceTestType!=kind||distanceTestExtra!=0xFEDCBA9876543210ull||
           distanceTestMode!=1u)return 9202u;
    }
    // Other icon types may use a completely different structure. Don't read it.
    if(cargoHudGeneralRadiusHook(context,0xBCu,(u8*)1u,0u,0u)!=25.0f||
       cargoHudGeneralRadiusHook(context,0xD1u,(u8*)1u,0u,0u)!=25.0f)return 9203u;
    for(u32 test=0;test<9u;test++){
        distanceTestFixture(base,packet);
        switch(test){
        case 0:*(u64*)(packet+0x20u)=0;break;
        case 1:*(u64*)(packet+0x20u)=123u;break;
        case 2:packet[0x2Cu]=0;break;
        case 3:packet[0x4Eu]=0;break;
        case 4:g_scannedHudIdTime=GetTickCount64()-251u;break;
        case 5:invalidateScannedHudOrigin();break;
        case 6:_InterlockedExchange(&g_scannedHudReady,0);break;
        case 7:_InterlockedExchange(&g_hooksReady,0);break;
        case 8:g_scannedHudIdCount=0;break;
        }
        if(cargoHudRadiusHook(context,packet)!=25.0f||distanceTestCalls!=1u)return 9210u+test;
    }
    distanceTestFixture(base,packet);
    for(u32 i=0;i<4u;i++){
        float originals[4]={0.0f,-1.0f,500.0f,800.0f};
        distanceTestOriginal=originals[i];
        if(cargoHudRadiusHook(context,packet)!=originals[i])return 9220u+i;
    }
    // Removing a candidate next frame clears authorization; no retained hits.
    distanceTestFixture(base,packet);
    u8 empty[48];scannedCargoHudHook(empty,empty);
    if(cargoHudRadiusHook(context,packet)!=25.0f)return 9224u;
    // Expiry and excluded metadata are filtered BEFORE issuing an ID authorization.
    for(u32 i=0;i<2u;i++){
        u8 record[48];hudTestFixture(base,record);
        if(i==0)hudTestExpired=true;else *(u32*)(hudTestMeta+0xE8u)=0x202u;
        scannedCargoHudHook(record,record+48u);
        if(g_scannedHudIdCount!=0u)return 9225u+i;
    }
    // Live observed untouched and picked-up/dropped flags: both must feed the
    // same 300 m downstream radius, only after a fresh native hit. Test the
    // candidate-to-ID-to-radius chain, including exclusion/expiry and no writes.
    for(u32 flagsCase=0;flagsCase<2u;flagsCase++)for(u32 condition=0;condition<10u;condition++){
        u8 record[48];hudTestFixture(base,record);
        *(u32*)(hudTestMeta+0xE8u)=flagsCase?0x02129301u:0x02529001u;
        g_targetScanRadiusBits=floatBits(300.0f);publishScannedHudOrigin(hudTestManager);
        *(double*)(hudTestMeta+0x40u)=1290.0;
        if(condition==1u)*(double*)(hudTestMeta+0x40u)=1300.01;
        if(condition==2u)hudTestExpired=true;
        if(condition==3u)record[0x0Cu]=0u;
        if(condition==4u)*(u32*)(hudTestMeta+0xE8u)|=2u;
        if(condition==5u)hudTestMeta[0x83u]=0u;
        if(condition==6u)hudTestMeta[0x83u]=3u;
        if(condition==7u)*(float*)(hudTestMeta+0x88u)=0.0f;
        if(condition==8u)*(float*)(hudTestMeta+0x8Cu)=0.0f;
        if(condition==9u)hudTestMissing=true;
        u8 metaBefore[sizeof(hudTestMeta)];memcpy(metaBefore,hudTestMeta,sizeof(metaBefore));
        scannedCargoHudHook(record,record+48u);
        memset(packet,0,sizeof(packet));packet[0x2Cu]=4u;packet[0x4Eu]=2u;
        *(u64*)(packet+0x20u)=*(u64*)(hudTestMeta+0x18u);
        memcpy(before,packet,sizeof(packet));g_cargoHudRadius=distanceTestRadius;
        distanceTestCalls=0;distanceTestOriginal=200.0f;
        float expectedRadius=condition==0u?300.0f:200.0f;
        if(cargoHudRadiusHook(context,packet)!=expectedRadius||distanceTestCalls!=1u||
           g_scannedHudIdCount!=(condition==0u?1u:0u)||
           !bytesEqual(packet,before,sizeof(packet))||
           !bytesEqual(hudTestMeta,metaBefore,sizeof(metaBefore)))return 9270u+condition;
    }
    // A fresh attempt must drop even a still-recent ID from the preceding scan.
    distanceTestFixture(base,packet);invalidateScannedHudOrigin();
    publishScannedHudOrigin(hudTestManager);
    if(cargoHudRadiusHook(context,packet)!=25.0f)return 9227u;
    // Candidate authorization obeys the game's 128-entry sorted-list budget.
    u8* many=(u8*)VirtualAlloc(0,129u*48u,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!many)return 9228u;
    hudTestFixture(base,many);
    for(u32 i=1u;i<129u;i++)memcpy(many+i*48u,many,48u);
    scannedCargoHudHook(many,many+129u*48u);
    bool capped=g_scannedHudIdCount==128u&&many[127u*48u+0x0Eu]==1u&&
        many[128u*48u+0x0Eu]==0u&&hudTestFindCalls==128u;
    VirtualFree(many,0,MEM_RELEASE);
    if(!capped)return 9229u;
    // Used/destroyed cargo keeps the original radius in both paths.
    for(u32 state=0;state<4u;state++){
        distanceTestFixture(base,packet);packet[0x4Eu]=(u8)(state<<5);
        memcpy(before,packet,sizeof(packet));
        float expected=25.0f;
        if(cargoHudRadiusHook(context,packet)!=expected||
           cargoHudGeneralRadiusHook(context,0xBEu,packet,0u,0u)!=expected||
           !bytesEqual(packet,before,sizeof(packet)))return 9230u+state;
    }
    distanceTestFixture(base,packet);packet[0x4Eu]=0x62u;
    _InterlockedExchange(&g_scannedHudReady,0);
    if(cargoHudRadiusHook(context,packet)!=25.0f)return 9234u;
    distanceTestFixture(base,packet);
    bool previousDebug=g_debugLog;g_debugLog=true;
    *(float*)(packet+0x10u)=300.0f;
    cargoHudRadiusHook(context,packet);cargoHudRadiusHook(context,packet);
    g_debugLog=previousDebug;
    if(g_cargoDistanceSampleCount!=1u||g_cargoDistanceSamples[0].originSquaredBits!=floatBits(90000.0f)||
       g_cargoDistanceSamples[0].hudSquaredBits!=floatBits(90000.0f))return 9235u;
    invalidateScannedHudOrigin();
    if(g_cargoDistanceSampleCount||g_cargoDistanceSampleLogged)return 9236u;
    _InterlockedExchange(&g_scannedHudReady,0);
    return 0;
}

static u32 testMappedHudDistances(u8* base){
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateHudDistanceTargets(&image))return 9300u;
    u8 packet[0x50],context[0x200];memset(context,0,sizeof(context));
    distanceTestFixture(base,packet);
    for(u32 i=0;i<5u;i++){
        HudDistancePatch patch={};
        if(installHudDistanceCall(base,i,&patch)!=APPLY_READY)return 9301u+i;
        const HudDistanceCall* site=&HUD_DISTANCE_CALLS[i];
        u32 delta=*(u32*)(base+site->callRva+1u);
        if(base[site->callRva]!=0xE8u||base+site->callRva+5u+(s32)delta!=patch.relay)return 9310u+i;
        HudDistancePatch duplicate={};
        if(installHudDistanceCall(base,i,&duplicate)!=APPLY_CONFLICT||duplicate.relay)return 9320u+i;
        distanceTestFixture(base,packet);
        float radius=site->targetRva==0x17CE830u?
            ((CargoHudRadius)patch.relay)(context,packet):
            ((CargoHudGeneralRadius)patch.relay)(context,0xBEu,packet,0xABCDEF9876543210ull,1u);
        if(radius!=500.0f||distanceTestCalls!=1u||distanceTestContext!=context||
           distanceTestPacket!=packet)return 9330u+i;
        if(site->targetRva==0x17CDDD0u&&(distanceTestType!=0xBEu||
           distanceTestExtra!=0xABCDEF9876543210ull||distanceTestMode!=1u))return 9340u+i;
        if(rollbackHudDistanceCall(base,i,&patch)!=APPLY_FAILED||
           !protectionMatchesAfterOwnedWrite(base+site->windowRva,patch.protection))return 9350u+i;
        VirtualFree(patch.relay,0,MEM_RELEASE);
    }
    if(!validateHudDistanceTargets(&image))return 9360u;

    // Install the whole group, then reverse it; inject a later conflict to
    // verify that earlier published call sites are rolled back as a group.
    memset(g_hudDistancePatches,0,sizeof(g_hudDistancePatches));
    if(installHudDistanceCalls(base)!=APPLY_READY)return 9370u;
    for(u32 i=5u;i>0u;){
        i--;
        if(rollbackHudDistanceCall(base,i,&g_hudDistancePatches[i])!=APPLY_FAILED)return 9371u;
        VirtualFree(g_hudDistancePatches[i].relay,0,MEM_RELEASE);
    }
    memset(g_hudDistancePatches,0,sizeof(g_hudDistancePatches));
    u8* conflicting=base+HUD_DISTANCE_CALLS[2].windowRva;
    DWORD conflictProtection=0,conflictIgnored=0;
    if(!VirtualProtect(conflicting,8u,PAGE_EXECUTE_READWRITE,&conflictProtection))return 9372u;
    conflicting[7]^=1u;
    if(!VirtualProtect(conflicting,8u,conflictProtection,&conflictIgnored))return 9373u;
    ApplyResult conflictResult=installHudDistanceCalls(base);
    if(!VirtualProtect(conflicting,8u,PAGE_EXECUTE_READWRITE,&conflictIgnored))return 9374u;
    conflicting[7]^=1u;
    if(!VirtualProtect(conflicting,8u,conflictProtection,&conflictIgnored)||
       !FlushInstructionCache(GetCurrentProcess(),conflicting,8u))return 9375u;
    if(conflictResult!=APPLY_CONFLICT||!validateHudDistanceTargets(&image))return 9376u;
    for(u32 i=0;i<5u;i++){
        if(g_hudDistancePatches[i].relay)VirtualFree(g_hudDistancePatches[i].relay,0,MEM_RELEASE);
    }
    memset(g_hudDistancePatches,0,sizeof(g_hudDistancePatches));

    // Reproduce BOTH native renderer range checks on a real mapped DS2 image:
    // unknown cargo label=25 m, normal scanned icon=200 m, actual fade at 300 m=0.
    // This fixture exercises the game's code, not a duplicate implementation.
    u8 cache[0xE0],resident[0x60],config[0xA0],manager[0xB0];
    memset(cache,0,sizeof(cache));memset(resident,0,sizeof(resident));
    memset(config,0,sizeof(config));memset(manager,0,sizeof(manager));
    *(u8**)(cache+0xD0u)=resident;*(u8**)(resident+0x50u)=config;
    *(float*)(config+0x48u)=25.0f;*(float*)(config+0x2Cu)=30.0f;
    *(float*)(config+0x30u)=200.0f;*(float*)(manager+0xA0u)=200.0f;
    u8 configBefore[sizeof(config)];memcpy(configBefore,config,sizeof(config));
    u8* slots[2]={base+0x0623EBF8u,base+RVA_SENSOR_MANAGER_GLOBAL};
    u8* values[2]={cache,manager};u64 saved[2]={};DWORD protections[2]={};
    for(u32 i=0;i<2u;i++){
        if(!readU64(slots[i],&saved[i])||!VirtualProtect(slots[i],8u,PAGE_READWRITE,&protections[i]))return 9361u+i;
        *(u8**)slots[i]=values[i];
    }
    distanceTestFixture(base,packet);
    g_cargoHudRadius=(CargoHudRadius)(base+0x17CE830u);
    g_cargoHudGeneralRadius=(CargoHudGeneralRadius)(base+0x17CDDD0u);
    typedef float (*NativeFade)(void*,float,float);
    NativeFade fade=(NativeFade)(base+0x17CE900u);
    float nativeLabel=g_cargoHudRadius(context,packet);
    float nativeIcon=g_cargoHudGeneralRadius(context,0xBEu,packet,0u,0u);
    float extendedLabel=cargoHudRadiusHook(context,packet);
    float extendedIcon=cargoHudGeneralRadiusHook(context,0xBEu,packet,0u,0u);
    bool ok=nativeLabel==25.0f&&nativeIcon==200.0f&&extendedLabel==500.0f&&extendedIcon==500.0f&&
        fade(0,300.0f*300.0f,nativeLabel)==0.0f&&fade(0,300.0f*300.0f,nativeIcon)==0.0f&&
        fade(0,300.0f*300.0f,extendedLabel)==1.0f&&fade(0,300.0f*300.0f,extendedIcon)==1.0f;
    // The live 495-unit marker sits in the last 10 units of the 500 radius.
    ok=ok&&fade(0,495.0f*495.0f,extendedLabel)==0.5f;
    g_targetScanRadiusBits=floatBits(1000.0f);
    float larger=cargoHudRadiusHook(context,packet);
    ok=ok&&larger==1000.0f&&fade(0,495.0f*495.0f,larger)==1.0f&&
        fade(0,900.0f*900.0f,larger)==1.0f&&fade(0,1001.0f*1001.0f,larger)==0.0f;
    // Existing configuration bytes must be unchanged by either wrapper.
    ok=ok&&bytesEqual(config,configBefore,sizeof(config));
    for(u32 condition=0x20u;condition<=0x60u;condition+=0x20u){
        packet[0x4Eu]=(u8)(condition|2u);
        float vanillaLabel=g_cargoHudRadius(context,packet);
        float vanillaIcon=g_cargoHudGeneralRadius(context,0xBEu,packet,0u,0u);
        float actualLabel=cargoHudRadiusHook(context,packet);
        float actualIcon=cargoHudGeneralRadiusHook(context,0xBEu,packet,0u,0u);
        ok=ok&&actualLabel==vanillaLabel&&actualIcon==vanillaIcon&&
            fade(0,1.0f,actualLabel)==1.0f&&fade(0,1.0f,actualIcon)==1.0f&&
            fade(0,90000.0f,actualLabel)==0.0f;
    }
    for(u32 i=0;i<2u;i++){
        *(u64*)slots[i]=saved[i];DWORD ignored=0;
        if(!VirtualProtect(slots[i],8u,protections[i],&ignored)||
           !protectionMatchesAfterOwnedWrite(slots[i],protections[i]))return 9363u+i;
    }
    invalidateScannedHudOrigin();_InterlockedExchange(&g_scannedHudReady,0);
    return ok?0u:9365u;
}
