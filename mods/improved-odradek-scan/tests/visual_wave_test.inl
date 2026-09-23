static u32 visualTestCalls=0,visualTestCount=0;
static u8* visualTestBinding=0;
static u64 visualTestHandle=0;
static const float* visualTestInput=0;
static float visualTestValues[4];
static u64 visualTestWrite(u8* binding,u64 handle,u32 count,const float* value){
    visualTestCalls++;visualTestBinding=binding;visualTestHandle=handle;
    visualTestCount=count;visualTestInput=value;
    if(value)memcpy(visualTestValues,value,16u);
    return 0x123456789ABCDEFull;
}

static u32 testMappedVisualWave(u8* base){
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateVisualWave(&image))return 9700u;
    // Native-style caller puts the fifth test argument into RDI. Invoke the
    // actual executable relay, not just the C++ wrapper, to check Windows ABI.
    u8 shimBytes[]={0x57,0x48,0x8B,0x7C,0x24,0x30,0x48,0x83,0xEC,0x20,
        0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xD0,0x48,0x83,0xC4,0x20,0x5F,0xC3};
    u8* shim=(u8*)VirtualAlloc(0,4096u,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if(!shim)return 9701u;
    typedef u64 (*Invoke)(u8*,u64,u32,const float*,u8*);
    __declspec(align(16)) u8 instance[0x2C0]={};
    u8 expected[0x2C0];
    float values[4]={12.0f,34.0f,56.0f,600.0f};
    g_gameBase=base;g_targetEffectDiameterBits=floatBits(600.0f);
    *(u64*)instance=(u64)(base+RVA_EFFECT_INSTANCE_VTABLE);
    *(float*)(instance+0x294u)=600.0f;
    *(float*)(instance+OFF_EFFECT_SIZE_OVERRIDE)=600.0f;
    _InterlockedExchange64(&g_visualWaveInstance,(long long)instance);
    g_visualWaveScale=0.5f;_InterlockedExchange(&g_hooksReady,1);
    for(u32 i=0;i<2u;i++){
        const HudDistanceCall* site=&VISUAL_WAVE_CALLS[i];
        HudDistancePatch patch={},duplicate={};DWORD before=0,ignored=0;
        if(!queryProtection(base+site->windowRva,&before)||
           installExactCall(base,site,(u64)&visualWaveHook,&patch,2u)!=APPLY_READY)return 9702u;
        if(!protectionMatchesAfterOwnedWrite(base+site->windowRva,before)||
           !protectionMatches(patch.relay,PAGE_EXECUTE_READ))return 9703u;
        if(installExactCall(base,site,(u64)&visualWaveHook,&duplicate,2u)!=APPLY_CONFLICT)return 9704u;
        memcpy(shimBytes+12u,&patch.relay,8u);
        if(!VirtualProtect(shim,4096u,PAGE_READWRITE,&ignored))return 9705u;
        memcpy(shim,shimBytes,sizeof(shimBytes));
        if(!VirtualProtect(shim,4096u,PAGE_EXECUTE_READ,&ignored)||
           !FlushInstructionCache(GetCurrentProcess(),shim,sizeof(shimBytes)))return 9706u;
        g_visualWaveWrite=visualTestWrite;
        for(u32 condition=0;condition<11u;condition++){
            g_visualWaveScale=0.5f;_InterlockedExchange(&g_hooksReady,1);
            *(u64*)instance=(u64)(base+RVA_EFFECT_INSTANCE_VTABLE);
            *(float*)(instance+OFF_EFFECT_SIZE_OVERRIDE)=600.0f;
            *(float*)(instance+0x294u)=600.0f;values[3]=600.0f;
            u8* context=instance+0x60u;u32 count=4u;
            const float* input=values;
            if(condition==1u)g_visualWaveScale=1.0f;
            if(condition==2u)_InterlockedExchange(&g_hooksReady,0);
            if(condition==3u)context++;
            if(condition==4u)count=3u;
            if(condition==5u)*(u64*)instance=0;
            if(condition==6u)*(float*)(instance+OFF_EFFECT_SIZE_OVERRIDE)=400.0f;
            if(condition==7u)*(float*)(instance+0x294u)=599.0f;
            if(condition==8u){*(u32*)(values+3u)=0x7FC00000u;*(u32*)(instance+0x294u)=0x7FC00000u;}
            if(condition==9u)input=0;
            if(condition==10u)g_visualWaveScale=0.75f;
            memcpy(expected,instance,sizeof(instance));visualTestCalls=0;
            u64 result=((Invoke)shim)(instance,0x123456789ull,count,input,context);
            if(result!=0x123456789ABCDEFull||visualTestCalls!=1u||
               visualTestBinding!=instance||visualTestHandle!=0x123456789ull||visualTestCount!=count||
               !bytesEqual(expected,instance,sizeof(instance)))return 9720u+condition;
            if(condition==0u||condition==10u){
                if(visualTestInput==input||visualTestValues[0]!=12.0f||visualTestValues[1]!=34.0f||
                   visualTestValues[2]!=56.0f||visualTestValues[3]!=(condition==0u?300.0f:450.0f)||values[3]!=600.0f)return 9740u;
            }else if(visualTestInput!=input)return 9741u+condition;
        }
        if(rollbackExactCall(base,site,&patch)!=APPLY_FAILED||!validateVisualWave(&image)||
           !protectionMatchesAfterOwnedWrite(base+site->windowRva,before))return 9760u;
        VirtualFree(patch.relay,0,MEM_RELEASE);
    }
    // A collision at the second site must restore the already-published first.
    HudDistancePatch conflict={};
    if(installExactCall(base,&VISUAL_WAVE_CALLS[1],(u64)&visualWaveHook,&conflict,2u)!=APPLY_READY)return 9761u;
    if(installVisualWave(base)!=APPLY_CONFLICT||
       !bytesEqual(base+VISUAL_WAVE_CALLS[0].windowRva,VISUAL_WAVE_CALLS[0].expected,8u))return 9762u;
    if(rollbackExactCall(base,&VISUAL_WAVE_CALLS[1],&conflict)!=APPLY_FAILED||!validateVisualWave(&image))return 9763u;
    VirtualFree(conflict.relay,0,MEM_RELEASE);
    VirtualFree(g_visualWavePatches[0].relay,0,MEM_RELEASE);
    memset(g_visualWavePatches,0,sizeof(g_visualWavePatches));
    VirtualFree(shim,0,MEM_RELEASE);
    g_visualWaveScale=0.5f;_InterlockedExchange64(&g_visualWaveInstance,0);
    return 0;
}
