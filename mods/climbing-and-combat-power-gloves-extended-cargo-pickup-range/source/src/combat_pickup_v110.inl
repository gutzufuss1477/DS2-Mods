// Combat Power Gloves cargo pickup, v1.1.3.
// Only functional hooks are installed; development probes are not included.
static const u64 RVA_GLOVE_ACTION_COST=0x00E9E400ull;
static const u64 RVA_GLOVE_ACTION_AVAILABLE=0x00E9E6B0ull;
static const u64 RVA_PICKUP_RIGHT_GATE=0x0102C500ull;
static const u64 RVA_PICKUP_LEFT_GATE=0x0102CB70ull;
static const u64 RVA_PICKUP_SHARED_GATE=0x011216A0ull;
static const u64 RVA_CATCH_RIGHT_SELECTOR=0x010298E0ull;
static const u64 RVA_EQUIPPED_SUBCATEGORY_CHECK=0x00E9DC00ull;
static const u64 RVA_PICKUP_PROVIDER_SUBCAT_RETURN=0x01004025ull;
static const u8 COMBAT_ID_LEVEL1=55u,COMBAT_ID_LEVEL2=56u,COMBAT_SUBCATEGORY=7u;
static const u32 COMBAT_NATIVE_PARAM_COUNT=7u,COMBAT_SHADOW_PARAM_COUNT=8u;
static const float COMBAT_LEVEL1_PICKUP_ANGLE=100.0f,COMBAT_LEVEL2_PICKUP_ANGLE=120.0f;

static const u8 EXPECTED_COST_ENTRY[7]={0x48,0x8B,0x42,0x20,0x4C,0x8B,0xCA};
static const u8 EXPECTED_AVAILABLE_ENTRY[5]={0x48,0x89,0x5C,0x24,0x08};
static const u8 EXPECTED_PICKUP_RIGHT_GATE_ENTRY[6]={0x40,0x53,0x48,0x83,0xEC,0x20};
static const u8 EXPECTED_PICKUP_LEFT_GATE_ENTRY[6]={0x40,0x53,0x48,0x83,0xEC,0x20};
static const u8 EXPECTED_PICKUP_SHARED_GATE_ENTRY[5]={0x48,0x89,0x5C,0x24,0x08};
static const u8 EXPECTED_CATCH_SELECTOR_ENTRY[5]={0x48,0x89,0x5C,0x24,0x10};
static const u8 EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY[6]={0x40,0x53,0x48,0x83,0xEC,0x20};

typedef float (*GloveActionCostFn)(u64,u8*,u8);
typedef u8 (*GloveActionAvailableFn)(u8*,u8,u8);
typedef u64 (*PickupGate1Fn)(u64);
typedef u32 (*PickupSharedGateFn)(u64,u8);
typedef u64 (*EquippedSubcategoryCheckFn)(u64,u8,u8);
static GloveActionCostFn g_originalGloveActionCost=0;
static GloveActionAvailableFn g_originalGloveActionAvailable=0;
static PickupGate1Fn g_originalPickupRightGate=0,g_originalPickupLeftGate=0;
static PickupGate1Fn g_originalCatchRightSelector=0;
static PickupSharedGateFn g_originalPickupSharedGate=0;
static EquippedSubcategoryCheckFn g_originalEquippedSubcategoryCheck=0;
static u64 g_combatImageBase=0;
static u8* g_combatHookPage=0;
static u8* g_combatShadowPage=0;
static bool g_combatHooksInstalled=false,g_combatHookInstallFailed=false;
static volatile long g_recentGloveSubcategory=0,g_combatVehicleActive=0;
static volatile long g_nativeVehicleCatchCount=0;

static void logCombatPickupStatus(){
    if(!g_debugLog)return;
    static long reported=0;
    long count=_InterlockedCompareExchange(&g_nativeVehicleCatchCount,0,0);
    if(count!=reported){
        const char message[]="STATE: native right-hand vehicle cargo catch prepared.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1u));
        reported=count;
    }
}

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
       id!=expectedId||category!=ITEM_CATEGORY_GLOVE||
       (subcategory!=COMBAT_SUBCATEGORY&&
        subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE))
        return false;
    return readMemory(object+OFF_ITEM_PARAMS,paramsOut,sizeof(*paramsOut));
}

enum CombatReconcileResult{
    COMBAT_WAITING,COMBAT_INVALID,COMBAT_HOOK_ERROR,COMBAT_READY
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

// The pickup queries read Params[6] for distance (e.g. RVA 0xFFCFEB).
// Combat action 8 also reads it for cost (RVA 0xE9E46B). Keep that cost
// privately after the eight game-visible floats.
struct CombatPickupParams {
    u32 values[COMBAT_SHADOW_PARAM_COUNT];
    u32 nativeAction8Cost;
};

static CombatPickupParams combatPickupParams(u8 id,u32 rangeBits){
    CombatPickupParams params;
    combatNativeParams(id,params.values);
    params.nativeAction8Cost=params.values[6];
    params.values[6]=rangeBits;
    params.values[7]=floatBits(
        id==COMBAT_ID_LEVEL1?COMBAT_LEVEL1_PICKUP_ANGLE:COMBAT_LEVEL2_PICKUP_ANGLE
    );
    return params;
}

static bool prepareCombatTier(const GameImage* image,u8* object,u8 id,
                              const CombatPickupParams& expected,RawArray** attach){
    RawArray params;
    *attach=0;
    if(!inspectCombatIdentity(image,object,id,&params))return false;
    u8* shadow=combatShadowForId(id);
    if(!memoryEquals(shadow,(const u8*)&expected,sizeof(expected)))return false;
    if(params.entries==(u64)shadow)
        return params.count==COMBAT_SHADOW_PARAM_COUNT&&params.capacity==COMBAT_SHADOW_PARAM_COUNT;
    u32 native[COMBAT_NATIVE_PARAM_COUNT];
    combatNativeParams(id,native);
    if(params.count!=COMBAT_NATIVE_PARAM_COUNT||params.capacity<params.count||params.capacity>32u||
       !memoryEquals((void*)params.entries,(const u8*)native,sizeof(native))||
       !isWritableRange(object+OFF_ITEM_PARAMS,sizeof(RawArray)))return false;
    *attach=(RawArray*)(object+OFF_ITEM_PARAMS);
    return true;
}

static void attachCombatTier(volatile RawArray* target,u8 id){
    if(!target)return;
    // The game can read the pointer before it sees the extended length.
    target->entries=(u64)combatShadowForId(id);
    target->capacity=COMBAT_SHADOW_PARAM_COUNT;
    target->count=COMBAT_SHADOW_PARAM_COUNT;
}

static bool installCombatHooks(const GameImage* image);
static bool validateCombatHookState(const GameImage* image);

static CombatReconcileResult reconcileCombatGloves(const GameImage* image,const Settings& settings){
    u8 *level1=0,*level2=0;
    if(!image||!findCombatItems(image,&level1,&level2))return COMBAT_WAITING;
    CombatPickupParams first=combatPickupParams(COMBAT_ID_LEVEL1,settings.level1Bits);
    CombatPickupParams second=combatPickupParams(COMBAT_ID_LEVEL2,settings.level2Bits);
    if(!g_combatShadowPage){
        g_combatShadowPage=(u8*)VirtualAlloc(
            0,0x1000u,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE
        );
        if(!g_combatShadowPage)return COMBAT_INVALID;
        memcpy(combatShadowForId(COMBAT_ID_LEVEL1),&first,sizeof(first));
        memcpy(combatShadowForId(COMBAT_ID_LEVEL2),&second,sizeof(second));
    }
    RawArray *attach1,*attach2;
    if(!prepareCombatTier(image,level1,COMBAT_ID_LEVEL1,first,&attach1)||
       !prepareCombatTier(image,level2,COMBAT_ID_LEVEL2,second,&attach2))return COMBAT_INVALID;
    // Both resource descriptors must be usable and the cost hook must be
    // active before either item can expose a pickup radius in slot 6.
    if(!installCombatHooks(image)||!validateCombatHookState(image))return COMBAT_HOOK_ERROR;
    attachCombatTier(attach1,COMBAT_ID_LEVEL1);
    attachCombatTier(attach2,COMBAT_ID_LEVEL2);
    return COMBAT_READY;
}

static bool combatShadowLive(u8* item){
    if(!item||!g_combatShadowPage)return false;
    u8 id=0,subcategory=0;
    RawArray params;
    if(!readU8(item+OFF_ITEM_ID,&id)||
       !readU8(item+OFF_ITEM_SUBCATEGORY,&subcategory)||
       (subcategory!=COMBAT_SUBCATEGORY&&
        subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE)||
       (id!=COMBAT_ID_LEVEL1&&id!=COMBAT_ID_LEVEL2)||
       !readMemory(item+OFF_ITEM_PARAMS,&params,sizeof(params)))return false;
    return params.count==COMBAT_SHADOW_PARAM_COUNT&&
           params.capacity==COMBAT_SHADOW_PARAM_COUNT&&
           params.entries==(u64)combatShadowForId(id);
}

static float combatActionCostHook(u64 first,u8* entry,u8 action){
    u64 itemPointer=0;
    u8 id=0,subcategory=0;
    if(entry&&action<=8u&&readU64(entry+0x20u,&itemPointer)&&itemPointer&&
       readU8((u8*)itemPointer+OFF_ITEM_ID,&id)&&
       readU8((u8*)itemPointer+OFF_ITEM_SUBCATEGORY,&subcategory)){
        bool combatId=id==COMBAT_ID_LEVEL1||id==COMBAT_ID_LEVEL2;
        if(action<8u&&(combatId||subcategory==ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE))
            _InterlockedExchange(&g_recentGloveSubcategory,combatId?COMBAT_SUBCATEGORY:subcategory);
        if(combatId&&action==8u&&subcategory==COMBAT_SUBCATEGORY){
            u64 entries=0;
            float cost;
            if(readU64(&((RawArray*)(itemPointer+OFF_ITEM_PARAMS))->entries,&entries)&&entries){
                // Snapshot the entries pointer once, including during a resource reload.
                const void* source=entries==(u64)combatShadowForId(id)
                    ? (const void*)&((const CombatPickupParams*)entries)->nativeAction8Cost
                    : (const void*)((const u32*)entries+6);
                if(readMemory(source,&cost,sizeof(cost)))return cost;
            }
        }
        if(action==6u&&combatId&&combatShadowLive((u8*)itemPointer)){
            float cost;
            if(readMemory(combatShadowForId(id)+5u*sizeof(float),&cost,sizeof(cost)))return cost;
        }
    }
    return g_originalGloveActionCost?
        g_originalGloveActionCost(first,entry,action):3.402823466e38f;
}

static u8 combatActionAvailableHook(u8* state,u8 subcategory,u8 action){
    if(!g_originalGloveActionAvailable)return 0u;
    u8 result=g_originalGloveActionAvailable(state,subcategory,action);
    if(result||subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE||action!=6u)return result;
    return g_originalGloveActionAvailable(state,COMBAT_SUBCATEGORY,action);
}
static bool recentCombatGlovesActive(){
    return _InterlockedCompareExchange(&g_recentGloveSubcategory,0,0)==COMBAT_SUBCATEGORY;
}
static bool combatVehicleState(u64 self){
    if(!self||!recentCombatGlovesActive())return false;
    u64 playerState=0;u32 movementState=0;
    return readU64((void*)(self+0x28u),&playerState)&&playerState&&
           readU32((void*)(playerState+0x7548u),&movementState)&&movementState==0xEu;
}
static u64 equippedSubcategoryCheckHook(u64 state,u8 subcategory,u8 bypassDurability){
    u64 result=g_originalEquippedSubcategoryCheck?
        g_originalEquippedSubcategoryCheck(state,subcategory,bypassDurability):0ull;
    if((u8)result||subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE||bypassDurability!=0u||
       !recentCombatGlovesActive()||_InterlockedCompareExchange(&g_combatVehicleActive,0,0)==0)
        return result;
    u64 returnAddress=(u64)__builtin_return_address(0);
    if(!g_combatImageBase||returnAddress-g_combatImageBase!=RVA_PICKUP_PROVIDER_SUBCAT_RETURN)return result;
    return g_originalEquippedSubcategoryCheck(state,COMBAT_SUBCATEGORY,0u);
}
static u64 pickupRightGateHook(u64 self){
    u64 result=g_originalPickupRightGate?g_originalPickupRightGate(self):0ull;
    return !(u8)result&&combatVehicleState(self)?1ull:result;
}
static u64 pickupLeftGateHook(u64 self){
    u64 result=g_originalPickupLeftGate?g_originalPickupLeftGate(self):0ull;
    return !(u8)result&&combatVehicleState(self)?1ull:result;
}
static u32 pickupSharedGateHook(u64 self,u8 mode){
    u64 playerState=0;u32 movementState=0;
    bool known=self&&readU64((void*)(self+0x28u),&playerState)&&playerState&&
        readU32((void*)(playerState+0x7548u),&movementState);
    bool vehicle=known&&recentCombatGlovesActive()&&movementState==0xEu;
    if(known)_InterlockedExchange(&g_combatVehicleActive,vehicle?1:0);
    u32 result=g_originalPickupSharedGate?g_originalPickupSharedGate(self,mode):0u;
    return !(u8)result&&vehicle?1u:result;
}
static bool prepareRightVehicleCatch(u64 self){
    // The pickup work already deposits the real cargo ID and a one-second TTL
    // in CatchBaggageWork. State 3 consumes its candidate through native code;
    // states 1 and 2 are guarding / holding breath, not cargo reception.
    if(!combatVehicleState(self))return false;
    u64 player=0,pending=0,candidate=0,held=0;
    u32 rightAction=0,ttlBits=0;
    u8 active=0;
    if(!readU8((void*)(self+8u),&active)||active||
       !readU64((void*)(self+0x28u),&player)||!player||
       !readU32((void*)(player+0x754Cu),&rightAction)||rightAction!=0u||
       !readU64((void*)(self+0x700u),&pending)||pending==~0ull||
       !readU32((void*)(self+0x70Cu),&ttlBits)||ttlBits==0u||ttlBits>0x3F800000u||
       !readU64((void*)(self+0x6F8u),&candidate)||candidate!=~0ull||
       !readU64((void*)(self+0x7C0u),&held)||held!=~0ull||
       !isWritableRange((void*)(self+0x6F8u),8u)||
       !isWritableRange((void*)(self+0x11Au),2u))return false;
    *(u64*)(self+0x6F8u)=pending;
    *(u16*)(self+0x11Au)=3u;
    return true;
}

static u64 catchRightSelectorHook(u64 self){
    if(prepareRightVehicleCatch(self)){
        long previous=0;
        do{previous=_InterlockedCompareExchange(&g_nativeVehicleCatchCount,0,0);}
        while(_InterlockedCompareExchange(&g_nativeVehicleCatchCount,previous+1,previous)!=previous);
        return 1ull;
    }
    return g_originalCatchRightSelector?g_originalCatchRightSelector(self):0ull;
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

struct CombatHookSpec{
    u64 rva;
    const u8* expected;
    u32 length;
    u8* replacement;
};
static const CombatHookSpec COMBAT_HOOKS[]={
    {RVA_GLOVE_ACTION_AVAILABLE,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY),(u8*)&combatActionAvailableHook},
    {RVA_GLOVE_ACTION_COST,EXPECTED_COST_ENTRY,sizeof(EXPECTED_COST_ENTRY),(u8*)&combatActionCostHook},
    {RVA_PICKUP_RIGHT_GATE,EXPECTED_PICKUP_RIGHT_GATE_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_GATE_ENTRY),(u8*)&pickupRightGateHook},
    {RVA_PICKUP_LEFT_GATE,EXPECTED_PICKUP_LEFT_GATE_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_GATE_ENTRY),(u8*)&pickupLeftGateHook},
    {RVA_PICKUP_SHARED_GATE,EXPECTED_PICKUP_SHARED_GATE_ENTRY,sizeof(EXPECTED_PICKUP_SHARED_GATE_ENTRY),(u8*)&pickupSharedGateHook},
    {RVA_CATCH_RIGHT_SELECTOR,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY),(u8*)&catchRightSelectorHook},
    {RVA_EQUIPPED_SUBCATEGORY_CHECK,EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY,sizeof(EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY),(u8*)&equippedSubcategoryCheckHook}
};
static const u32 COMBAT_HOOK_COUNT=(u32)(sizeof(COMBAT_HOOKS)/sizeof(COMBAT_HOOKS[0]));
static const u32 COMBAT_HOOK_STRIDE=0x60u;

static void buildCombatPatch(u8* patch,const GameImage* image,u32 index){
    const CombatHookSpec& hook=COMBAT_HOOKS[index];
    memset(patch,0x90,hook.length);
    buildRel32Jump(patch,image->base+hook.rva,g_combatHookPage+index*COMBAT_HOOK_STRIDE);
}
static bool installCombatHooks(const GameImage* image){
    if(g_combatHooksInstalled)return true;
    if(!image||!image->base||g_combatHookInstallFailed)return false;
    // Validate the whole transaction before altering any game code.
    for(u32 i=0;i<COMBAT_HOOK_COUNT;i++){
        const CombatHookSpec& hook=COMBAT_HOOKS[i];
        if(hook.length<5u||hook.length>8u||
           !memoryEquals(image->base+hook.rva,hook.expected,hook.length))return false;
    }
    g_combatHookPage=allocateCombatHookPage(image->base+RVA_GLOVE_ACTION_AVAILABLE);
    if(!g_combatHookPage)return false;
    for(u32 i=0;i<COMBAT_HOOK_COUNT;i++){
        const CombatHookSpec& hook=COMBAT_HOOKS[i];
        u8* relay=g_combatHookPage+i*COMBAT_HOOK_STRIDE;
        u8* trampoline=relay+0x20u;
        if(!rel32Fits(image->base+hook.rva+5u,relay)){
            VirtualFree(g_combatHookPage,0u,MEM_RELEASE);g_combatHookPage=0;return false;
        }
        buildAbsoluteJump(relay,hook.replacement);
        memcpy(trampoline,hook.expected,hook.length);
        buildAbsoluteJump(trampoline+hook.length,image->base+hook.rva+hook.length);
    }
    DWORD oldProtection=0;
    if(!VirtualProtect(g_combatHookPage,0x1000u,PAGE_EXECUTE_READ,&oldProtection)||
       !FlushInstructionCache(GetCurrentProcess(),g_combatHookPage,0x1000u)){
        VirtualFree(g_combatHookPage,0u,MEM_RELEASE);g_combatHookPage=0;return false;
    }
    g_combatImageBase=(u64)image->base;
    g_originalGloveActionAvailable=(GloveActionAvailableFn)(g_combatHookPage+0x20u);
    g_originalGloveActionCost=(GloveActionCostFn)(g_combatHookPage+COMBAT_HOOK_STRIDE+0x20u);
    g_originalPickupRightGate=(PickupGate1Fn)(g_combatHookPage+2u*COMBAT_HOOK_STRIDE+0x20u);
    g_originalPickupLeftGate=(PickupGate1Fn)(g_combatHookPage+3u*COMBAT_HOOK_STRIDE+0x20u);
    g_originalPickupSharedGate=(PickupSharedGateFn)(g_combatHookPage+4u*COMBAT_HOOK_STRIDE+0x20u);
    g_originalCatchRightSelector=(PickupGate1Fn)(g_combatHookPage+5u*COMBAT_HOOK_STRIDE+0x20u);
    g_originalEquippedSubcategoryCheck=(EquippedSubcategoryCheckFn)(g_combatHookPage+6u*COMBAT_HOOK_STRIDE+0x20u);
    for(u32 i=0;i<COMBAT_HOOK_COUNT;i++){
        u8 patch[8];buildCombatPatch(patch,image,i);
        const CombatHookSpec& hook=COMBAT_HOOKS[i];
        if(!writeCodeBytes(image->base+hook.rva,patch,hook.length)){
            // Restore only this transaction's sites. Retain executable storage
            // for any already-running callback and never retry a partial install.
            g_combatHookInstallFailed=true;
            bool restored=true;
            for(u32 j=i+1u;j>0u;j--){
                const CombatHookSpec& undo=COMBAT_HOOKS[j-1u];
                if(!writeCodeBytes(image->base+undo.rva,undo.expected,undo.length))restored=false;
            }
            if(!restored){
                const char message[]="ERROR: combat hook rollback incomplete; restart DS2.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
            }
            return false;
        }
    }
    g_combatHooksInstalled=true;
    return true;
}
static bool validateCombatHookState(const GameImage* image){
    if(!g_combatHooksInstalled||!image||!image->base||!g_combatHookPage)return false;
    for(u32 i=0;i<COMBAT_HOOK_COUNT;i++){
        u8 patch[8];buildCombatPatch(patch,image,i);
        if(!memoryEquals(image->base+COMBAT_HOOKS[i].rva,patch,COMBAT_HOOKS[i].length))return false;
    }
    return true;
}
