static u8 sensorTestOwner[0x200],sensorTestBaggage[0x258],sensorTestDetected[0x88],sensorTestManager[0x200];
static u32 sensorTestCalls=0,sensorTestFindCalls=0;
static bool sensorTestBlocked=false,sensorTestMissing=false;
static float sensorTestLastRadius=0.0f;
static u8* sensorTestFind(u8* components,const void* type){
    sensorTestFindCalls++;
    if(components!=sensorTestOwner+0xA0u||type!=g_gameBase+RVA_BAGGAGE_COMPONENT_RTTI||sensorTestMissing)return 0;
    return sensorTestBaggage;
}
static u8 sensorTestOriginal(u8* manager,const double* target,float radius){
    sensorTestCalls++;sensorTestLastRadius=radius;
    double squared=0.0;
    for(u32 axis=0;axis<3u;axis++){
        double delta=target[axis]-*(double*)(manager+0x38u+axis*8u);squared+=delta*delta;
    }
    float progress=*(float*)(manager+0x98u);
    return !sensorTestBlocked&&manager[0x28u]&&squared<=(double)progress*progress&&
        (radius<=0.0f||squared<=(double)radius*radius)?1u:0u;
}
static void sensorTestFixture(u8* base){
    memset(sensorTestOwner,0,sizeof(sensorTestOwner));memset(sensorTestBaggage,0,sizeof(sensorTestBaggage));
    memset(sensorTestDetected,0,sizeof(sensorTestDetected));memset(sensorTestManager,0,sizeof(sensorTestManager));
    g_gameBase=base;g_scannedHudFind=sensorTestFind;
    _InterlockedExchange64(&g_originalSensorGate,(long long)(u64)&sensorTestOriginal);
    _InterlockedExchange(&g_hooksReady,1);_InterlockedExchange(&g_scannedHudReady,1);
    g_gateCaptureActive=0;g_targetScanRadiusBits=floatBits(500.0f);
    sensorTestManager[0x28u]=1;*(float*)(sensorTestManager+0x98u)=400.0f;
    *(u64*)sensorTestDetected=(u64)(base+RVA_DETECTED_COMPONENT_VTABLE);
    *(u8**)(sensorTestDetected+0x48u)=sensorTestOwner;sensorTestDetected[0x51u]=1;
    *(u64*)sensorTestBaggage=(u64)(base+RVA_BAGGAGE_COMPONENT_VTABLE);
    *(u8**)(sensorTestBaggage+0xF8u)=sensorTestOwner+0x20u;
    *(u8**)(sensorTestBaggage+0x230u)=sensorTestDetected;sensorTestBaggage[0x83u]=1;
    *(float*)(sensorTestBaggage+0x88u)=1.0f;*(float*)(sensorTestBaggage+0x8Cu)=1.0f;
    sensorTestBlocked=false;sensorTestMissing=false;sensorTestCalls=0;sensorTestFindCalls=0;
}
extern "C" __declspec(dllexport) u32 RunCargoSensorTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 9400u;
    double target[3]={300.0,0.0,0.0};
    u8 beforeBaggage[sizeof(sensorTestBaggage)],beforeDetected[sizeof(sensorTestDetected)];
    sensorTestFixture(base);
    memcpy(beforeBaggage,sensorTestBaggage,sizeof(beforeBaggage));memcpy(beforeDetected,sensorTestDetected,sizeof(beforeDetected));
    if(sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)!=1u||
       sensorTestCalls!=2u||sensorTestFindCalls!=1u||sensorTestLastRadius!=500.0f||
       !bytesEqual(beforeBaggage,sensorTestBaggage,sizeof(beforeBaggage))||
       !bytesEqual(beforeDetected,sensorTestDetected,sizeof(beforeDetected)))return 9401u;
    // The observed dropped-world state qualifies, but not other touched states.
    for(u32 state=0;state<4u;state++){
        sensorTestFixture(base);g_targetScanRadiusBits=floatBits(300.0f);
        *(float*)(sensorTestManager+0x98u)=300.0f;
        *(u32*)(sensorTestBaggage+0xE8u)=0x02129301u;
        sensorTestBaggage[0x83u]=(u8)state;
        memcpy(beforeBaggage,sensorTestBaggage,sizeof(beforeBaggage));
        u8 result=sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected);
        if(result!=(state==1u?1u:0u)||sensorTestCalls!=(state==1u?2u:1u)||
           !bytesEqual(beforeBaggage,sensorTestBaggage,sizeof(beforeBaggage)))return 9490u+state;
    }
    for(u32 test=0;test<13u;test++){
        sensorTestFixture(base);
        switch(test){
        case 0:*(float*)(sensorTestManager+0x98u)=299.0f;break;
        case 1:sensorTestManager[0x28u]=0;break;
        case 2:*(u64*)sensorTestDetected=0;break;
        case 3:sensorTestDetected[0x51u]=0;break;
        case 4:*(u8**)(sensorTestDetected+0x48u)=0;break;
        case 5:sensorTestMissing=true;break;
        case 6:*(u64*)sensorTestBaggage=0;break;
        case 7:*(u8**)(sensorTestBaggage+0x230u)=0;break;
        case 8:*(u8**)(sensorTestBaggage+0xF8u)=0;break;
        case 9:*(u32*)(sensorTestBaggage+0xE8u)=0x202u;break;
        case 10:sensorTestBaggage[0x83u]=3;break;
        case 11:_InterlockedExchange(&g_scannedHudReady,0);break;
        case 12:_InterlockedExchange(&g_hooksReady,0);break;
        }
        if(sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)!=0u||sensorTestCalls!=1u)return 9410u+test;
    }
    sensorTestFixture(base);sensorTestBlocked=true;
    if(sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)!=0u||sensorTestCalls!=2u)return 9430u;
    sensorTestFixture(base);
    if(sensorGateTelemetryHook(sensorTestManager,target,400.0f,sensorTestDetected)!=1u||
       sensorTestCalls!=1u||sensorTestFindCalls)return 9431u;
    for(u32 axis=0;axis<3u;axis++)for(u32 sign=0;sign<2u;sign++){
        sensorTestFixture(base);*(float*)(sensorTestManager+0x98u)=600.0f;
        target[0]=target[1]=target[2]=0.0;target[axis]=sign?500.0:-500.0;
        if(sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)!=1u)return 9440u;
        sensorTestCalls=0;target[axis]+=sign?0.01:-0.01;
        if(sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)!=0u||sensorTestCalls!=1u)return 9441u;
    }
    _InterlockedExchange(&g_scannedHudReady,0);
    return 0;
}

// Execute the actual native gate in the offline image. Resolve only its two
// math imports in this test process; no game entry point or file is changed.
extern "C" __declspec(dllexport) u32 RunMappedCargoSensorTest(u8* base,void* atan2fProc,void* fmodfProc){
    if(!base||!atan2fProc||!fmodfProc)return 9450u;
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateCargoSensorTargets(&image))return 9454u;
    u8* slots[2]={base+0x2C17720u,base+0x2C17738u};
    void* values[2]={atan2fProc,fmodfProc};u64 saved[2];DWORD protection[2];
    for(u32 i=0;i<2u;i++){
        if(!readU64(slots[i],&saved[i])||!VirtualProtect(slots[i],8u,PAGE_READWRITE,&protection[i]))return 9451u;
        *(void**)slots[i]=values[i];
    }
    sensorTestFixture(base);
    SensorGate native=(SensorGate)(base+0xDE6E40u);
    _InterlockedExchange64(&g_originalSensorGate,(long long)(u64)native);
    *(float*)(sensorTestManager+0x94u)=6.28318530718f;
    double target[3]={300.0,0.0,0.0};
    bool ok=native(sensorTestManager,target,200.0f)==0u&&
        native(sensorTestManager,target,500.0f)==1u&&
        sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==1u;
    target[0]=-300.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==1u;
    *(float*)(sensorTestManager+0x98u)=299.0f;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==0u;
    *(float*)(sensorTestManager+0x98u)=600.0f;
    target[0]=500.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==1u;
    target[0]=501.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==0u;
    target[0]=300.0;*(float*)(sensorTestManager+0x94u)=0.0f;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==0u;
    *(float*)(sensorTestManager+0x94u)=6.28318530718f;
    *(float*)(sensorTestManager+0x98u)=1000.0f;g_targetScanRadiusBits=floatBits(1000.0f);
    target[0]=900.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==1u;
    target[0]=1001.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==0u;
    // Main INI preset: exact 700 m boundary, full circle and progressive arrival.
    g_targetScanRadiusBits=floatBits(700.0f);
    *(float*)(sensorTestManager+0x98u)=700.0f;target[0]=700.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==1u;
    target[0]=700.01;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==0u;
    target[0]=-700.0;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==1u;
    *(float*)(sensorTestManager+0x98u)=699.0f;
    ok=ok&&sensorGateTelemetryHook(sensorTestManager,target,200.0f,sensorTestDetected)==0u;
    // Both slots share a page; undo nested protection changes in reverse order.
    for(u32 n=2u;n>0u;){
        u32 i=--n;
        *(u64*)slots[i]=saved[i];DWORD ignored=0;
        if(!VirtualProtect(slots[i],8u,protection[i],&ignored)||
           !protectionMatchesAfterOwnedWrite(slots[i],protection[i]))return 9452u;
    }
    _InterlockedExchange(&g_scannedHudReady,0);
    return ok?0u:9453u;
}
