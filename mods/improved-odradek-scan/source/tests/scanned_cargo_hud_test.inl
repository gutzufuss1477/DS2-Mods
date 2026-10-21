static u8 hudTestMeta[0x180],hudTestOwner[0x200],hudTestDetected[0x100];
static u8 hudTestManager[0x80];
static u32 hudTestSortCalls=0,hudTestFindCalls=0,hudTestRecentCalls=0;
static bool hudTestMissing=false,hudTestExpired=false;
static u8* hudTestBegin=0;
static u8* hudTestEnd=0;
static void hudTestSort(u8* begin,u8* end){
    hudTestSortCalls++;hudTestBegin=begin;hudTestEnd=end;
}
static u8* hudTestFind(u8* components,const void* type){
    hudTestFindCalls++;
    if(components!=hudTestOwner+0xA0u||type!=g_gameBase+0x04490CD0u)return 0;
    return hudTestMissing?0:hudTestDetected;
}
static u8 hudTestRecent(u8* detected){
    hudTestRecentCalls++;
    return detected==hudTestDetected&&!hudTestExpired?1u:0u;
}
static void hudTestFixture(u8* base,u8* record){
    memset(hudTestMeta,0,sizeof(hudTestMeta));
    memset(hudTestOwner,0,sizeof(hudTestOwner));
    memset(hudTestDetected,0,sizeof(hudTestDetected));
    memset(hudTestManager,0,sizeof(hudTestManager));
    memset(record,0,48u);
    g_gameBase=base;g_scannedHudSort=hudTestSort;
    g_scannedHudFind=hudTestFind;g_scannedHudRecent=hudTestRecent;
    _InterlockedExchange(&g_scannedHudReady,1);
    _InterlockedExchange(&g_hooksReady,1);
    g_targetScanRadiusBits=floatBits(500.0f);
    *(double*)(hudTestManager+0x38u)=1000.0;
    *(double*)(hudTestManager+0x40u)=2000.0;
    *(double*)(hudTestManager+0x48u)=3000.0;
    invalidateScannedHudOrigin();
    publishScannedHudOrigin(hudTestManager);
    *(u64*)(hudTestMeta+0x18u)=0x1234567812345678ull;
    *(double*)(hudTestMeta+0x40u)=1300.0;
    *(double*)(hudTestMeta+0x48u)=2000.0;
    *(double*)(hudTestMeta+0x50u)=3000.0;
    *(u8**)(hudTestMeta+0xF8u)=hudTestOwner+0x20u;
    hudTestMeta[0x83u]=1;
    *(float*)(hudTestMeta+0x88u)=1.0f;*(float*)(hudTestMeta+0x8Cu)=1.0f;
    *(u64*)hudTestDetected=(u64)(base+RVA_DETECTED_COMPONENT_VTABLE);
    *(u8**)(hudTestDetected+0x48u)=hudTestOwner;
    hudTestDetected[0x51u]=1;hudTestDetected[0x56u]=1;
    *(u8**)record=hudTestMeta;record[0x0Cu]=1;
    hudTestSortCalls=0;hudTestFindCalls=0;hudTestRecentCalls=0;
    hudTestMissing=false;hudTestExpired=false;
}

extern "C" __declspec(dllexport) u32 RunScannedHudTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 9000u;
    u8 record[48],before[48],metaBefore[sizeof(hudTestMeta)],detectedBefore[sizeof(hudTestDetected)];
    hudTestFixture(base,record);
    memcpy(before,record,48u);before[0x0Eu]=1u;
    memcpy(metaBefore,hudTestMeta,sizeof(metaBefore));
    memcpy(detectedBefore,hudTestDetected,sizeof(detectedBefore));
    scannedCargoHudHook(record,record+48u);
    if(!bytesEqual(record,before,48u)||hudTestSortCalls!=1u||
       hudTestBegin!=record||hudTestEnd!=record+48u||hudTestFindCalls!=1u||
       hudTestRecentCalls!=1u)return 9001u;
    if(!bytesEqual(metaBefore,hudTestMeta,sizeof(metaBefore))||
       !bytesEqual(detectedBefore,hudTestDetected,sizeof(detectedBefore)))return 9002u;
    // Exact range boundary on all three axes, including behind the player.
    for(u32 axis=0;axis<3u;axis++)for(u32 sign=0;sign<2u;sign++){
        hudTestFixture(base,record);
        memcpy(hudTestMeta+0x40u,hudTestManager+0x38u,24u);
        double* coordinate=(double*)(hudTestMeta+0x40u+axis*8u);
        *coordinate+=sign?500.0:-500.0;
        scannedCargoHudHook(record,record+48u);
        if(record[0x0Eu]!=1u)return 9010u+axis*2u+sign;
        record[0x0Eu]=0u;*coordinate+=sign?0.01:-0.01;
        scannedCargoHudHook(record,record+48u);
        if(record[0x0Eu]!=0u||hudTestFindCalls!=1u)return 9020u+axis*2u+sign;
    }
    // Failure gates: no native hit, native visibility already on, invalid/touched
    // metadata, bad coordinates, missing/wrong component, expiry, disabled hook.
    for(u32 test=0;test<17u;test++){
        hudTestFixture(base,record);
        switch(test){
        case 0:record[0x0Cu]=0;break;
        case 1:record[0x0Eu]=1;break;
        case 2:*(u32*)(hudTestMeta+0xE8u)=0x200u;hudTestMeta[0x83u]=2;break;
        case 3:*(u32*)(hudTestMeta+0xE8u)=2u;break;
        case 4:hudTestMeta[0x83u]=0;break;
        case 5:hudTestMeta[0x83u]=3;break;
        case 6:*(u64*)(hudTestMeta+0x40u)=0x7FF8000000000000ull;break;
        case 7:*(u64*)(hudTestMeta+0x50u)=0x7FF0000000000000ull;break;
        case 8:*(u8**)(hudTestMeta+0xF8u)=0;break;
        case 9:hudTestMissing=true;break;
        case 10:*(u64*)hudTestDetected=0;break;
        case 11:*(u8**)(hudTestDetected+0x48u)=0;break;
        case 12:hudTestDetected[0x51u]=0;break;
        case 13:hudTestDetected[0x56u]=0;hudTestDetected[0x57u]=1;break;
        case 14:hudTestExpired=true;break;
        case 15:invalidateScannedHudOrigin();break;
        case 16:_InterlockedExchange(&g_scannedHudReady,0);break;
        }
        memcpy(before,record,48u);
        scannedCargoHudHook(record,record+48u);
        if(!bytesEqual(before,record,48u)||hudTestSortCalls!=1u)return 9040u+test;
        if(test<9u&&test!=1u&&hudTestFindCalls)return 9060u+test;
    }
    // Empty, reversed, malformed and over-budget input must still call original
    // exactly once with unchanged arguments and perform no component lookups.
    for(u32 test=0;test<4u;test++){
        hudTestFixture(base,record);
        u8* begin=record;u8* end=record;
        if(test==1u)begin=record+48u;
        if(test==2u)end=record+47u;
        if(test==3u)end=(u8*)((u64)record+1025u*48u);
        scannedCargoHudHook(begin,end);
        if(hudTestSortCalls!=1u||hudTestBegin!=begin||hudTestEnd!=end||
           hudTestFindCalls||record[0x0Eu])return 9080u+test;
    }
    // Realization presence is deliberately irrelevant; no loading required.
    hudTestFixture(base,record);
    *(u64*)(hudTestMeta+0x168u)=0x12345678u;
    scannedCargoHudHook(record,record+48u);
    if(record[0x0Eu]!=1u)return 9090u;
    // Spent cargo's original permission and every other byte remain untouched.
    // This applies both far away (0) and inside its vanilla range (1).
    for(u32 condition=0;condition<3u;condition++)for(u32 permitted=0;permitted<2u;permitted++){
        u8 resource[0x50]={};hudTestFixture(base,record);
        if(condition==0)*(float*)(hudTestMeta+0x88u)=0.0f;
        if(condition==1)*(float*)(hudTestMeta+0x8Cu)=0.0f;
        if(condition==2){*(u8**)(hudTestMeta+0x38u)=resource;resource[0x48u]=2;}
        record[0x0Eu]=(u8)permitted;memcpy(before,record,48u);
        scannedCargoHudHook(record,record+48u);
        if(!bytesEqual(before,record,48u)||g_scannedHudIdCount||hudTestFindCalls)return 9091u;
    }
    hudTestFixture(base,record);g_targetScanRadiusBits=floatBits(1000.0f);
    publishScannedHudOrigin(hudTestManager);*(double*)(hudTestMeta+0x40u)=1900.0;
    scannedCargoHudHook(record,record+48u);
    if(record[0x0Eu]!=1u||g_scannedHudIdCount!=1u)return 9092u;
    record[0x0Eu]=0u;*(double*)(hudTestMeta+0x40u)=2000.01;
    scannedCargoHudHook(record,record+48u);
    if(record[0x0Eu]!=0u||g_scannedHudIdCount)return 9093u;
    // Diagnostics include native non-hits without inventing a hit or retaining
    // any object pointer. Sampling is bounded and stops when DebugLog is off.
    bool savedDebug=g_debugLog;g_debugLog=true;
    hudTestFixture(base,record);record[0x0Cu]=0u;memcpy(before,record,48u);
    scannedCargoHudHook(record,record+48u);
    if(!bytesEqual(record,before,48u)||hudTestFindCalls||g_cargoCandidateCount!=1u||
       g_cargoCandidateSamples[0].admitted||g_cargoCandidateSamples[0].nativeFlags||
       g_cargoCandidateSamples[0].id!=*(u64*)(hudTestMeta+0x18u))return 9094u;
    g_cargoCandidateSampleTime=0;scannedCargoHudHook(record,record+48u);
    if(g_cargoCandidateCount!=1u)return 9095u;
    g_debugLog=false;hudTestFixture(base,record);scannedCargoHudHook(record,record+48u);
    if(g_cargoCandidateCount)return 9096u;
    g_debugLog=savedDebug;
    _InterlockedExchange(&g_scannedHudReady,0);
    invalidateScannedHudOrigin();
    return 0;
}

static u32 testMappedScannedHud(u8* base){
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateScannedHudTargets(&image))return 9100u;
    u8* address=base+RVA_SCANNED_HUD_WINDOW;
    DWORD protection=0;
    if(!queryProtection(address,&protection))return 9101u;
    u8* relay=0;
    if(installScannedHud(base,EXPECTED_IMAGE_SIZE,&relay)!=APPLY_READY||!relay)return 9102u;
    u64 patched=0;
    if(!readU64(address,&patched)||patched==0x4800039349E80000ull||
       !protectionMatchesAfterOwnedWrite(address,protection))return 9103u;
    u32 displacement=0;
    if(!relayDisplacement(base+RVA_SCANNED_HUD_CALL,relay,&displacement)||
       *(u32*)(address+3u)!=displacement||address[0]!=0||address[1]!=0||
       address[2]!=0xE8u||address[7]!=0x48u)return 9104u;
    // A second install must report a conflict without altering the first hook.
    u8* second=0;
    if(installScannedHud(base,EXPECTED_IMAGE_SIZE,&second)!=APPLY_CONFLICT||second)return 9105u;
    if(rollbackScannedHudPatch(base,patched,protection)!=APPLY_FAILED||
       !validateScannedHudTargets(&image)||!protectionMatchesAfterOwnedWrite(address,protection))return 9106u;
    VirtualFree(relay,0,MEM_RELEASE); // no game threads run in mapped-image tests
    // Execute the actual game's small-list sort (no allocator/imports) through
    // our wrapper. Entire 48-byte records, including permission byte, must move.
    u8 records[96];memset(records,0,sizeof(records));
    *(u64*)records=123u;*(float*)(records+8u)=9.0f;records[0x0Eu]=1;
    *(u64*)(records+48u)=456u;*(float*)(records+56u)=1.0f;
    _InterlockedExchange(&g_scannedHudReady,0);
    g_scannedHudSort=(ScannedHudSort)(base+RVA_SCANNED_HUD_SORT);
    scannedCargoHudHook(records,records+96u);
    if(*(u64*)records!=456u||*(u64*)(records+48u)!=123u||
       records[0x0Eu]!=0u||records[48u+0x0Eu]!=1u)return 9107u;
    // Run the native expiry predicate against a temporary mapped-image manager.
    // No game entry point or imported function is invoked by this leaf routine.
    u8 manager[0x30],config[0x30],detected[0x90];
    memset(manager,0,sizeof(manager));memset(config,0,sizeof(config));
    memset(detected,0,sizeof(detected));
    *(u8**)(manager+8u)=config;*(float*)(config+0x24u)=20.0f;
    u8* managerSlot=base+RVA_SENSOR_MANAGER_GLOBAL;
    u64 oldManager=0;
    DWORD oldProtection=0;
    // This global is zero until game startup; the function-pointer installer
    // deliberately rejects null expected targets, so use a local data fixture.
    if(!readU64(managerSlot,&oldManager)||
       !VirtualProtect(managerSlot,8u,PAGE_READWRITE,&oldProtection))return 9108u;
    *(u8**)managerSlot=manager;
    ScannedHudRecent recent=(ScannedHudRecent)(base+RVA_SCANNED_HUD_RECENT);
    bool expiryOk=recent(detected)==0u;
    *(u32*)(detected+0x68u)=1u;*(float*)(detected+0x7Cu)=19.99f;
    expiryOk=expiryOk&&recent(detected)==1u;
    *(float*)(detected+0x7Cu)=20.0f;expiryOk=expiryOk&&recent(detected)==0u;
    *(u32*)(detected+0x7Cu)=0x7FC00000u;expiryOk=expiryOk&&recent(detected)==0u;
    *(u64*)managerSlot=oldManager;
    DWORD ignored=0;
    if(!VirtualProtect(managerSlot,8u,oldProtection,&ignored)||
       !protectionMatchesAfterOwnedWrite(managerSlot,oldProtection))return 9109u;
    if(!expiryOk)return 9110u;
    return 0;
}
