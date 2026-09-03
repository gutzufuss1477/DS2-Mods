#define CLIMBING_GLOVES_TEST_BUILD 1
#include "../src/climbing_gloves_range.cpp"

static void writePointer(u8* base,u64 rva,u64 targetRva){
    *(u64*)(base+rva)=(u64)(base+targetRva);
}

static void writeVtable(u8* base,u64 tableRva,const u64* functions,u32 count){
    for(u32 i=0;i<count;i++)writePointer(base,tableRva+(u64)i*8u,functions[i]);
}

static void seedStaticTargets(u8* base){
    memcpy(
        base+RVA_ITEM_SYSTEM_INIT,EXPECTED_ITEM_SYSTEM_INIT,
        sizeof(EXPECTED_ITEM_SYSTEM_INIT)
    );
    memcpy(
        base+RVA_ITEM_SYSTEM_CONSTRUCTOR,EXPECTED_ITEM_SYSTEM_CONSTRUCTOR,
        sizeof(EXPECTED_ITEM_SYSTEM_CONSTRUCTOR)
    );
    memcpy(base+RVA_ITEM_LOOKUP,EXPECTED_ITEM_LOOKUP,sizeof(EXPECTED_ITEM_LOOKUP));
    memcpy(
        base+RVA_ITEM_LOOKUP_CORE,EXPECTED_ITEM_LOOKUP_CORE,
        sizeof(EXPECTED_ITEM_LOOKUP_CORE)
    );
    memcpy(
        base+RVA_ITEM_PARAMETER_DESCRIPTOR,EXPECTED_ITEM_PARAMETER_DESCRIPTOR,
        sizeof(EXPECTED_ITEM_PARAMETER_DESCRIPTOR)
    );
    memcpy(
        base+RVA_ITEM_PARAMETER_TYPE_NAME,EXPECTED_ITEM_PARAMETER_TYPE_NAME,
        sizeof(EXPECTED_ITEM_PARAMETER_TYPE_NAME)
    );
    writeVtable(
        base,RVA_ITEM_SYSTEM_VTABLE,ITEM_SYSTEM_VFUNCS,
        (u32)(sizeof(ITEM_SYSTEM_VFUNCS)/sizeof(ITEM_SYSTEM_VFUNCS[0]))
    );
    writeVtable(
        base,RVA_ITEM_PARAMETER_VTABLE,ITEM_PARAMETER_VFUNCS,
        (u32)(sizeof(ITEM_PARAMETER_VFUNCS)/sizeof(ITEM_PARAMETER_VFUNCS[0]))
    );
    writePointer(
        base,RVA_ITEM_PARAMETER_DESCRIPTOR+0x18u,RVA_ITEM_PARAMETER_FACTORY
    );
    writePointer(
        base,RVA_ITEM_PARAMETER_DESCRIPTOR+0x20u,
        RVA_ITEM_PARAMETER_DESTRUCTOR_THUNK
    );
    writePointer(
        base,RVA_ITEM_PARAMETER_DESCRIPTOR+0x40u,
        RVA_ITEM_PARAMETER_TYPE_NAME
    );
}

static void seedPeHeader(u8* base){
    *(u16*)base=0x5A4Du;
    *(u32*)(base+0x3Cu)=0x100u;
    *(u32*)(base+0x100u)=0x00004550u;
    *(u32*)(base+0x108u)=EXPECTED_TIMESTAMP;
    *(u16*)(base+0x118u)=0x020Bu;
    *(u32*)(base+0x150u)=EXPECTED_IMAGE_SIZE;
}

static void seedGlove(
    u8* base,u64 objectRva,u64 paramsRva,u8 id,u8 level
){
    memset(base+objectRva,0,0x100u);
    memset(base+paramsRva,0,0x100u);
    writePointer(base,objectRva,RVA_ITEM_PARAMETER_VTABLE);
    base[objectRva+OFF_ITEM_ID]=id;
    base[objectRva+OFF_ITEM_LEVEL]=level;
    base[objectRva+OFF_ITEM_CATEGORY]=ITEM_CATEGORY_GLOVE;
    base[objectRva+OFF_ITEM_SUBCATEGORY]=
        ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE;
    RawArray* params=(RawArray*)(base+objectRva+OFF_ITEM_PARAMS);
    params->count=ITEM_PARAM_COUNT;
    params->capacity=ITEM_PARAM_COUNT;
    params->entries=(u64)(base+paramsRva);
    u32 values[ITEM_PARAM_COUNT];
    nativeParams(id,values);
    memcpy(base+paramsRva,values,sizeof(values));
}

extern "C" __declspec(dllexport) u32 RunPackagedSettingsTest(){
    Settings settings;
    SettingsResult result=loadSettings(&settings);
    closeLog();
    if(result!=SETTINGS_READY)return 1u;
    if(!settings.enabled||settings.debugLog||
       settings.level1Range!=DEFAULT_LEVEL1_RANGE||
       settings.level2Range!=DEFAULT_LEVEL2_RANGE||
       settings.level1Bits!=floatBits(DEFAULT_LEVEL1_RANGE)||
       settings.level2Bits!=floatBits(DEFAULT_LEVEL2_RANGE))return 2u;

    bool parsedBool=false;
    if(!parseBool(L"  YeS\t",&parsedBool)||!parsedBool||
       !parseBool(L" OFF ",&parsedBool)||parsedBool||
       parseBool(L"enabled",&parsedBool))return 3u;
    float parsed=0.0f;
    if(!parseDecimal(L"8",NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,&parsed)||
       parsed!=8.0f||
       !parseDecimal(L"100.000",NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,&parsed)||
       parsed!=100.0f||
       parseDecimal(L"7.999",NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,&parsed)||
       parseDecimal(L"100.001",NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,&parsed)||
       parseDecimal(L"10.",NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,&parsed)||
       parseDecimal(L"-10",NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,&parsed))return 4u;
    return 0u;
}

extern "C" __declspec(dllexport) u32 RunSyntheticClimbingGlovesTest(
    u8* base,u32 capacity
){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 1u;
    seedPeHeader(base);
    seedStaticTargets(base);
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    GameImage metadata;
    if(!readBuildMetadata(base,&metadata)||metadata.base!=base||
       metadata.timestamp!=EXPECTED_TIMESTAMP||
       metadata.imageSize!=EXPECTED_IMAGE_SIZE)return 2u;
    if(!validateStaticTargets(&image))return 3u;
    base[RVA_ITEM_LOOKUP_CORE]^=1u;
    if(validateStaticTargets(&image))return 4u;
    base[RVA_ITEM_LOOKUP_CORE]^=1u;

    const u64 systemRva=0x07000000ull;
    const u64 entriesRva=0x07001000ull;
    const u64 level1Rva=0x07002000ull;
    const u64 level2Rva=0x07003000ull;
    const u64 duplicateRva=0x07004000ull;
    const u64 params1Rva=0x07005000ull;
    const u64 paramsDuplicateRva=0x07006000ull;
    const u64 params2Rva=0x07007000ull;
    memset(base+systemRva,0,0x8000u);
    writePointer(base,systemRva,RVA_ITEM_SYSTEM_VTABLE);
    RawArray* items=(RawArray*)(base+systemRva+OFF_ITEM_SYSTEM_ARRAY);
    items->count=2u;
    items->capacity=2u;
    items->entries=(u64)(base+entriesRva);
    writePointer(base,entriesRva,level1Rva);
    writePointer(base,entriesRva+8u,level2Rva);
    seedGlove(base,level1Rva,params1Rva,ITEM_ID_LEVEL1,0u);
    seedGlove(base,level2Rva,params2Rva,ITEM_ID_LEVEL2,1u);

    Settings settings;
    settings.level1Range=30.0f;
    settings.level2Range=50.0f;
    settings.level1Bits=floatBits(settings.level1Range);
    settings.level2Bits=floatBits(settings.level2Range);
    settings.enabled=true;
    settings.debugLog=false;
    ResolvedItems resolved;
    *(u64*)(base+RVA_ITEM_SYSTEM_GLOBAL)=0u;
    if(resolveItems(&image,&resolved)!=RESOLVE_NOT_READY)return 5u;
    writePointer(base,RVA_ITEM_SYSTEM_GLOBAL,systemRva);
    items->count=0u;
    items->capacity=0u;
    items->entries=0u;
    if(resolveItems(&image,&resolved)!=RESOLVE_NOT_READY||
       reconcileGloves(&image,&settings,0)!=APPLY_NOT_READY)return 51u;
    items->count=2u;
    items->capacity=2u;
    items->entries=(u64)(base+entriesRva);
    if(resolveItems(&image,&resolved)!=RESOLVE_READY||
       resolved.system!=base+systemRva||resolved.level1!=base+level1Rva||
       resolved.level2!=base+level2Rva)return 6u;

    GloveView view1,view2;
    if(inspectGlove(
           &image,base+level1Rva,ITEM_ID_LEVEL1,0u,settings.level1Bits,&view1
       )!=INSPECT_NATIVE||view1.object!=base+level1Rva||
       view1.rangeCell!=base+params1Rva+24u||
       inspectGlove(
           &image,base+level2Rva,ITEM_ID_LEVEL2,1u,settings.level2Bits,&view2
       )!=INSPECT_NATIVE||view2.rangeCell!=base+params2Rva+24u)return 7u;

    if(exchangeBits(
           view1.rangeCell,view1.targetBits,view1.nativeBits
       )!=EXCHANGE_CHANGED||
       *(u32*)view1.rangeCell!=view1.targetBits)return 52u;
    if(exchangeBits(
           view1.rangeCell,view1.targetBits,view1.nativeBits
       )!=EXCHANGE_ALREADY_TARGET||
       *(u32*)view1.rangeCell!=view1.targetBits)return 53u;
    if(!rollbackChanged(&view1,true)||
       *(u32*)view1.rangeCell!=view1.nativeBits)return 55u;
    *(u32*)view1.rangeCell=view1.targetBits;
    if(!rollbackChanged(&view1,false)||
       *(u32*)view1.rangeCell!=view1.targetBits)return 54u;
    *(u32*)view1.rangeCell=view1.nativeBits;
    if(exchangeBits(
           view1.rangeCell,view1.targetBits,view1.nativeBits
       )!=EXCHANGE_CHANGED)return 57u;
    *(u32*)view1.rangeCell=floatBits(77.0f);
    if(rollbackChanged(&view1,true)||
       *(u32*)view1.rangeCell!=floatBits(77.0f))return 56u;
    *(u32*)view1.rangeCell=view1.nativeBits;

    u32 before1[ITEM_PARAM_COUNT],before2[ITEM_PARAM_COUNT];
    memcpy(before1,base+params1Rva,sizeof(before1));
    memcpy(before2,base+params2Rva,sizeof(before2));
    if(reconcileGloves(&image,&settings,&resolved)!=APPLY_READY||
       *(u32*)(base+params1Rva+24u)!=settings.level1Bits||
       *(u32*)(base+params2Rva+24u)!=settings.level2Bits)return 8u;
    for(u32 i=0;i<ITEM_PARAM_COUNT;i++){
        if(i==6u)continue;
        if(*(u32*)(base+params1Rva+(u64)i*4u)!=before1[i]||
           *(u32*)(base+params2Rva+(u64)i*4u)!=before2[i])return 9u;
    }
    if(reconcileGloves(&image,&settings,0)!=APPLY_READY)return 10u;

    *(u32*)(base+params1Rva+24u)=floatBits(NATIVE_LEVEL1_RANGE);
    *(u32*)(base+params2Rva+24u)=floatBits(77.0f);
    if(reconcileGloves(&image,&settings,0)!=APPLY_CONFLICT||
       *(u32*)(base+params1Rva+24u)!=floatBits(NATIVE_LEVEL1_RANGE)||
       *(u32*)(base+params2Rva+24u)!=floatBits(77.0f))return 11u;
    *(u32*)(base+params2Rva+24u)=floatBits(NATIVE_LEVEL2_RANGE);

    u32 savedParam0=*(u32*)(base+params1Rva);
    *(u32*)(base+params1Rva)=floatBits(4.0f);
    if(reconcileGloves(&image,&settings,0)!=APPLY_INVALID||
       *(u32*)(base+params2Rva+24u)!=floatBits(NATIVE_LEVEL2_RANGE))return 12u;
    *(u32*)(base+params1Rva)=savedParam0;

    DWORD oldProtection=0;
    if(!VirtualProtect(
           base+params2Rva,0x1000u,PAGE_READONLY,&oldProtection
       ))return 13u;
    ApplyResult protectedResult=reconcileGloves(&image,&settings,0);
    DWORD ignoredProtection=0;
    if(!VirtualProtect(
           base+params2Rva,0x1000u,oldProtection,&ignoredProtection
       ))return 14u;
    if(protectedResult!=APPLY_FAILED||
       *(u32*)(base+params1Rva+24u)!=floatBits(NATIVE_LEVEL1_RANGE)||
       *(u32*)(base+params2Rva+24u)!=floatBits(NATIVE_LEVEL2_RANGE))return 15u;

    items->count=1u;
    items->capacity=1u;
    if(resolveItems(&image,&resolved)!=RESOLVE_NOT_READY)return 16u;
    items->count=3u;
    items->capacity=3u;
    seedGlove(base,duplicateRva,paramsDuplicateRva,ITEM_ID_LEVEL1,0u);
    writePointer(base,entriesRva+16u,duplicateRva);
    if(resolveItems(&image,&resolved)!=RESOLVE_INVALID)return 17u;
    items->count=2u;
    items->capacity=2u;

    u64 savedVtable=*(u64*)(base+level1Rva);
    *(u64*)(base+level1Rva)=(u64)(base+RVA_ITEM_SYSTEM_VTABLE);
    if(resolveItems(&image,&resolved)!=RESOLVE_INVALID)return 18u;
    *(u64*)(base+level1Rva)=savedVtable;
    base[level2Rva+OFF_ITEM_CATEGORY]=0u;
    if(reconcileGloves(&image,&settings,0)!=APPLY_INVALID)return 19u;
    base[level2Rva+OFF_ITEM_CATEGORY]=ITEM_CATEGORY_GLOVE;
    ((RawArray*)(base+level2Rva+OFF_ITEM_PARAMS))->count=7u;
    if(reconcileGloves(&image,&settings,0)!=APPLY_INVALID)return 20u;
    ((RawArray*)(base+level2Rva+OFF_ITEM_PARAMS))->count=ITEM_PARAM_COUNT;

    Settings nativeSettings=settings;
    nativeSettings.level1Range=NATIVE_LEVEL1_RANGE;
    nativeSettings.level2Range=NATIVE_LEVEL2_RANGE;
    nativeSettings.level1Bits=floatBits(NATIVE_LEVEL1_RANGE);
    nativeSettings.level2Bits=floatBits(NATIVE_LEVEL2_RANGE);
    if(reconcileGloves(&image,&nativeSettings,0)!=APPLY_READY||
       *(u32*)(base+params1Rva+24u)!=nativeSettings.level1Bits||
       *(u32*)(base+params2Rva+24u)!=nativeSettings.level2Bits)return 21u;
    return 0u;
}
