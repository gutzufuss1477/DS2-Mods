// Combat Power Gloves cargo-pickup extension for v1.1.0.
// Keeps all seven native Combat Glove parameters unchanged.
static const u64 RVA_GLOVE_ACTION_COST=0x00E9E400ull;
static const u64 RVA_GLOVE_ACTION_AVAILABLE=0x00E9E6B0ull;
static const u8 COMBAT_ID_LEVEL1=55u;
static const u8 COMBAT_ID_LEVEL2=56u;
static const u8 COMBAT_SUBCATEGORY=7u;
static const u32 COMBAT_NATIVE_PARAM_COUNT=7u;
static const u32 COMBAT_SHADOW_PARAM_COUNT=8u;
static const float COMBAT_LEVEL1_PICKUP_ANGLE=100.0f;
static const float COMBAT_LEVEL2_PICKUP_ANGLE=120.0f;

static const u8 EXPECTED_COST_ENTRY[7]={
    0x48,0x8B,0x42,0x20,0x4C,0x8B,0xCA
};
static const u8 EXPECTED_AVAILABLE_ENTRY[5]={
    0x48,0x89,0x5C,0x24,0x08
};

typedef float (*GloveActionCostFn)(u64,u8*,u8);
typedef u8 (*GloveActionAvailableFn)(u8*,u8,u8);

static GloveActionCostFn g_originalGloveActionCost=0;
static GloveActionAvailableFn g_originalGloveActionAvailable=0;
static u8* g_combatHookPage=0;
static u8* g_combatShadowPage=0;
static u8* g_combatObject1=0;
static u8* g_combatObject2=0;
static bool g_combatHooksInstalled=false;
static bool combatPickupIniEnabled(){
    WCHAR path[520],value[32];
    modulePath(path,L"ds2_climbing_gloves_range.ini");
    if(!path[0])return false;
    DWORD length=GetPrivateProfileStringW(
        L"CombatGloves",L"EnableCargoPickup",L"1",value,32u,path
    );
    if(!length||length>=31u)return false;
    bool enabled=false;
    return parseBool(value,&enabled)&&enabled;
}

static void combatNativeParams(u8 id,u32* values){
    if(id==COMBAT_ID_LEVEL1){
        values[0]=floatBits(3.0f);values[1]=floatBits(3.0f);
        values[2]=floatBits(3.0f);values[3]=floatBits(10.0f);
        values[4]=floatBits(10.0f);values[5]=floatBits(10.0f);
        values[6]=floatBits(100.0f);
    }else{
        values[0]=floatBits(1.5f);values[1]=floatBits(1.5f);
        values[2]=floatBits(1.5f);values[3]=floatBits(5.0f);
        values[4]=floatBits(5.0f);values[5]=floatBits(5.0f);
        values[6]=floatBits(50.0f);
    }
}

static bool isWritableRange(void* address,SIZE_T size){
    if(!address||!size)return false;
    MEMORY_BASIC_INFORMATION_X64 info;
    memset(&info,0,sizeof(info));
    if(VirtualQuery(address,&info,sizeof(info))!=sizeof(info)||
       info.State!=MEM_COMMIT||(info.Protect&PAGE_GUARD))return false;
    DWORD protection=info.Protect&0xFFu;
    if(protection!=PAGE_READWRITE&&protection!=PAGE_EXECUTE_READWRITE)return false;
    u64 begin=(u64)address;
    u64 end=(u64)info.BaseAddress+info.RegionSize;
    return begin+size>=begin&&begin+size<=end;
}

static u8* combatShadowForId(u8 id){
    if(!g_combatShadowPage)return 0;
    return id==COMBAT_ID_LEVEL1?g_combatShadowPage:
           id==COMBAT_ID_LEVEL2?g_combatShadowPage+0x40u:0;
}

static bool inspectCombatIdentity(
    const GameImage* image,u8* object,u8 expectedId,RawArray* paramsOut
){
    if(!image||!object||!paramsOut||
       !hasExactVtable(image,object,RVA_ITEM_PARAMETER_VTABLE))return false;
    u8 id=0,category=0,subcategory=0;
    if(!readU8(object+OFF_ITEM_ID,&id)||!readU8(object+OFF_ITEM_CATEGORY,&category)||
       !readU8(object+OFF_ITEM_SUBCATEGORY,&subcategory)||
       id!=expectedId||category!=ITEM_CATEGORY_GLOVE||subcategory!=COMBAT_SUBCATEGORY)
        return false;
    return readMemory(object+OFF_ITEM_PARAMS,paramsOut,sizeof(*paramsOut));
}

enum CombatReconcileResult{
    COMBAT_WAITING,COMBAT_INVALID,COMBAT_READY
};
static bool findCombatItems(
    const GameImage* image,u8** level1,u8** level2
){
    *level1=0;*level2=0;
    u64 systemPointer=0;
    if(!readU64(image->base+RVA_ITEM_SYSTEM_GLOBAL,&systemPointer)||!systemPointer)
        return false;
    u8* system=(u8*)systemPointer;
    if(!hasExactVtable(image,system,RVA_ITEM_SYSTEM_VTABLE))return false;
    RawArray items;
    if(!readMemory(system+OFF_ITEM_SYSTEM_ARRAY,&items,sizeof(items))||
       !items.count||items.count>ITEM_SYSTEM_MAX_COUNT||
       items.capacity<items.count||items.capacity>ITEM_SYSTEM_MAX_COUNT||
       !items.entries)return false;
    for(u32 i=0;i<items.count;i++){
        u64 objectPointer=0;
        if(!readU64((void*)(items.entries+(u64)i*8u),&objectPointer)||!objectPointer)
            return false;
        u8 id=0;
        if(!readU8((u8*)objectPointer+OFF_ITEM_ID,&id))return false;
        if(id==COMBAT_ID_LEVEL1){
            if(*level1)return false;
            *level1=(u8*)objectPointer;
        }else if(id==COMBAT_ID_LEVEL2){
            if(*level2)return false;
            *level2=(u8*)objectPointer;
        }
    }
    return *level1&&*level2;
}

static bool validateCombatNative(
    const GameImage* image,u8* object,u8 id,RawArray* params
){
    if(!inspectCombatIdentity(image,object,id,params))return false;
    u8* shadow=combatShadowForId(id);
    if(params->count==COMBAT_SHADOW_PARAM_COUNT&&params->capacity==COMBAT_SHADOW_PARAM_COUNT&&
       params->entries==(u64)shadow){
        u32 values[COMBAT_SHADOW_PARAM_COUNT];
        if(!readMemory(shadow,values,sizeof(values)))return false;
        u32 expected[COMBAT_NATIVE_PARAM_COUNT];
        combatNativeParams(id,expected);
        for(u32 i=0;i<COMBAT_NATIVE_PARAM_COUNT;i++)
            if(values[i]!=expected[i])return false;
        u32 expectedAngle=floatBits(
            id==COMBAT_ID_LEVEL1?COMBAT_LEVEL1_PICKUP_ANGLE:COMBAT_LEVEL2_PICKUP_ANGLE
        );
        return values[7]==expectedAngle;
    }
    if(params->count!=COMBAT_NATIVE_PARAM_COUNT||
       params->capacity<COMBAT_NATIVE_PARAM_COUNT||params->capacity>32u||
       !params->entries)return false;
    u32 values[COMBAT_NATIVE_PARAM_COUNT],expected[COMBAT_NATIVE_PARAM_COUNT];
    if(!readMemory((void*)params->entries,values,sizeof(values)))return false;
    combatNativeParams(id,expected);
    for(u32 i=0;i<COMBAT_NATIVE_PARAM_COUNT;i++)
        if(values[i]!=expected[i])return false;
    return true;
}

static bool applyCombatShadow(
    const GameImage* image,u8* object,u8 id
){
    RawArray params;
    if(!validateCombatNative(image,object,id,&params))return false;
    u8* shadow=combatShadowForId(id);
    if(!shadow)return false;
    if(params.count==COMBAT_SHADOW_PARAM_COUNT&&params.entries==(u64)shadow)return true;
    u32 values[COMBAT_SHADOW_PARAM_COUNT],expected[COMBAT_NATIVE_PARAM_COUNT];
    combatNativeParams(id,expected);
    for(u32 i=0;i<COMBAT_NATIVE_PARAM_COUNT;i++)values[i]=expected[i];
    values[7]=floatBits(
        id==COMBAT_ID_LEVEL1?COMBAT_LEVEL1_PICKUP_ANGLE:COMBAT_LEVEL2_PICKUP_ANGLE
    );
    if(!isWritableRange(shadow,sizeof(values)))return false;
    memcpy(shadow,values,sizeof(values));
    if(!isWritableRange(object+OFF_ITEM_PARAMS,sizeof(RawArray)))return false;
    RawArray* live=(RawArray*)(object+OFF_ITEM_PARAMS);
    live->entries=(u64)shadow;
    live->capacity=COMBAT_SHADOW_PARAM_COUNT;
    live->count=COMBAT_SHADOW_PARAM_COUNT;
    RawArray verify;
    return inspectCombatIdentity(image,object,id,&verify)&&
           verify.count==COMBAT_SHADOW_PARAM_COUNT&&
           verify.capacity==COMBAT_SHADOW_PARAM_COUNT&&
           verify.entries==(u64)shadow;
}

static CombatReconcileResult reconcileCombatGloves(const GameImage* image){
    if(!image)return COMBAT_INVALID;
    if(!g_combatShadowPage){
        g_combatShadowPage=(u8*)VirtualAlloc(
            0,0x1000u,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE
        );
        if(!g_combatShadowPage)return COMBAT_INVALID;
    }
    u8 *level1=0,*level2=0;
    if(!findCombatItems(image,&level1,&level2))return COMBAT_WAITING;
    RawArray params1,params2;
    if(!validateCombatNative(image,level1,COMBAT_ID_LEVEL1,&params1)||
       !validateCombatNative(image,level2,COMBAT_ID_LEVEL2,&params2))
        return COMBAT_INVALID;
    if(!applyCombatShadow(image,level1,COMBAT_ID_LEVEL1)||
       !applyCombatShadow(image,level2,COMBAT_ID_LEVEL2))
        return COMBAT_INVALID;
    g_combatObject1=level1;
    g_combatObject2=level2;
    return COMBAT_READY;
}

static bool combatShadowLive(u8* item){
    if(!item||!g_combatShadowPage)return false;
    u8 id=0,subcategory=0;
    RawArray params;
    if(!readU8(item+OFF_ITEM_ID,&id)||
       !readU8(item+OFF_ITEM_SUBCATEGORY,&subcategory)||
       subcategory!=COMBAT_SUBCATEGORY||
       (id!=COMBAT_ID_LEVEL1&&id!=COMBAT_ID_LEVEL2)||
       !readMemory(item+OFF_ITEM_PARAMS,&params,sizeof(params)))return false;
    return params.count==COMBAT_SHADOW_PARAM_COUNT&&
           params.capacity==COMBAT_SHADOW_PARAM_COUNT&&
           params.entries==(u64)combatShadowForId(id);
}

static float combatActionCostHook(u64 first,u8* entry,u8 action){
    if(entry&&action==6u){
        u64 itemPointer=0;
        if(readU64(entry+0x20u,&itemPointer)&&combatShadowLive((u8*)itemPointer)){
            RawArray params;
            if(readMemory((u8*)itemPointer+OFF_ITEM_PARAMS,&params,sizeof(params))){
                u32 bits=0;
                if(readU32((void*)(params.entries+5u*4u),&bits)){
                    union FloatBits{u32 bits;float value;};
                    FloatBits converted;converted.bits=bits;
                    return converted.value;
                }
            }
        }
    }
    return g_originalGloveActionCost?
        g_originalGloveActionCost(first,entry,action):3.402823466e38f;
}

static u8 combatActionAvailableHook(u8* state,u8 subcategory,u8 action){
    if(!g_originalGloveActionAvailable)return 0u;
    u8 result=g_originalGloveActionAvailable(state,subcategory,action);
    if(result||subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE||action!=6u)
        return result;
    return g_originalGloveActionAvailable(state,COMBAT_SUBCATEGORY,action);
}

static bool rel32Fits(const u8* instructionNext,const u8* destination){
    s64 delta=(s64)((u64)destination-(u64)instructionNext);
    return delta>=-2147483648LL&&delta<=2147483647LL;
}
static void writeU64LE(u8* output,u64 value){
    for(u32 i=0;i<8u;i++)output[i]=(u8)(value>>(i*8u));
}

static void buildRel32Jump(u8 output[5],u8* site,u8* destination){
    s64 delta=(s64)((u64)destination-(u64)(site+5u));
    s32 displacement=(s32)delta;
    output[0]=0xE9u;
    output[1]=(u8)(displacement&0xFF);
    output[2]=(u8)((displacement>>8)&0xFF);
    output[3]=(u8)((displacement>>16)&0xFF);
    output[4]=(u8)((displacement>>24)&0xFF);
}

static void buildAbsoluteJump(u8* output,u8* destination){
    output[0]=0xFFu;output[1]=0x25u;
    output[2]=0;output[3]=0;output[4]=0;output[5]=0;
    writeU64LE(output+6u,(u64)destination);
}

static bool writeCodeBytes(u8* address,const u8* bytes,u32 count){
    DWORD oldProtection=0;
    if(!VirtualProtect(address,count,PAGE_EXECUTE_READWRITE,&oldProtection))return false;
    memcpy(address,bytes,count);
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),address,count);
    bool verified=memoryEquals(address,bytes,count);
    DWORD ignored=0;
    BOOL restored=VirtualProtect(address,count,oldProtection,&ignored);
    return flushed&&verified&&restored;
}

static u8* allocateCombatHookPage(u8* site){
    const u64 granularity=0x10000ull,maxDistance=0x7FFF0000ull;
    u64 imageBase=(u64)site-RVA_GLOVE_ACTION_AVAILABLE;
    u64 imageEnd=imageBase+(u64)EXPECTED_IMAGE_SIZE;
    u64 aligned=(imageEnd+granularity-1ull)&~(granularity-1ull);
    for(u64 delta=0;delta<=maxDistance;delta+=granularity){
        u64 candidates[2]={
            aligned+delta,
            aligned>delta?aligned-delta:0ull
        };
        for(u32 i=0;i<2u;i++){
            if(!candidates[i])continue;
            u8* memory=(u8*)VirtualAlloc(
                (LPVOID)candidates[i],0x1000u,
                MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE
            );
            if(memory&&rel32Fits(site+5u,memory))return memory;
            if(memory)VirtualFree(memory,0u,MEM_RELEASE);
        }
    }
    return 0;
}

static bool installCombatHooks(const GameImage* image){
    if(g_combatHooksInstalled)return true;
    if(!image||!image->base)return false;
    u8* cost=image->base+RVA_GLOVE_ACTION_COST;
    u8* available=image->base+RVA_GLOVE_ACTION_AVAILABLE;
    if(!memoryEquals(cost,EXPECTED_COST_ENTRY,sizeof(EXPECTED_COST_ENTRY))||
       !memoryEquals(available,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY)))
        return false;
    g_combatHookPage=allocateCombatHookPage(available);
    if(!g_combatHookPage)return false;
    u8* availableRelay=g_combatHookPage;
    u8* costRelay=g_combatHookPage+0x20u;
    u8* availableTrampoline=g_combatHookPage+0x40u;
    u8* costTrampoline=g_combatHookPage+0x80u;
    buildAbsoluteJump(availableRelay,(u8*)&combatActionAvailableHook);
    buildAbsoluteJump(costRelay,(u8*)&combatActionCostHook);
    memcpy(availableTrampoline,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY));
    buildAbsoluteJump(
        availableTrampoline+sizeof(EXPECTED_AVAILABLE_ENTRY),
        available+sizeof(EXPECTED_AVAILABLE_ENTRY)
    );
    memcpy(costTrampoline,EXPECTED_COST_ENTRY,sizeof(EXPECTED_COST_ENTRY));
    buildAbsoluteJump(
        costTrampoline+sizeof(EXPECTED_COST_ENTRY),
        cost+sizeof(EXPECTED_COST_ENTRY)
    );
    DWORD oldProtection=0;
    if(!VirtualProtect(g_combatHookPage,0x1000u,PAGE_EXECUTE_READ,&oldProtection))
        return false;
    FlushInstructionCache(GetCurrentProcess(),g_combatHookPage,0x1000u);
    if(!rel32Fits(available+5u,availableRelay)||
       !rel32Fits(cost+5u,costRelay))
        return false;
    g_originalGloveActionAvailable=(GloveActionAvailableFn)availableTrampoline;
    g_originalGloveActionCost=(GloveActionCostFn)costTrampoline;

    u8 availablePatch[5],costPatch[7];
    buildRel32Jump(availablePatch,available,availableRelay);
    buildRel32Jump(costPatch,cost,costRelay);
    costPatch[5]=0x90u;costPatch[6]=0x90u;
    if(!writeCodeBytes(available,availablePatch,sizeof(availablePatch))){
        writeCodeBytes(available,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY));
        return false;
    }
    if(!writeCodeBytes(cost,costPatch,sizeof(costPatch))){
        writeCodeBytes(cost,EXPECTED_COST_ENTRY,sizeof(EXPECTED_COST_ENTRY));
        writeCodeBytes(available,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY));
        return false;
    }
    g_combatHooksInstalled=true;
    return true;
}

static bool validateCombatHookState(const GameImage* image){
    if(!g_combatHooksInstalled||!image)return false;
    u8* available=image->base+RVA_GLOVE_ACTION_AVAILABLE;
    u8* cost=image->base+RVA_GLOVE_ACTION_COST;
    return available[0]==0xE9u&&
           cost[0]==0xE9u&&cost[5]==0x90u&&cost[6]==0x90u;
}
