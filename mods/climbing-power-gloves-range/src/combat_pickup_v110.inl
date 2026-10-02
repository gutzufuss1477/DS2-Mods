// Combat Power Gloves cargo-pickup extension for v1.1.0.
// Keeps all seven native Combat Glove parameters unchanged.
static const u64 RVA_GLOVE_ACTION_COST=0x00E9E400ull;
static const u64 RVA_GLOVE_ACTION_AVAILABLE=0x00E9E6B0ull;
static const u64 RVA_VEHICLE_PICKUP_COLLECTOR=0x01003F00ull;
static const u64 RVA_PICKUP_RIGHT_SELECTOR=0x0102C330ull;
static const u64 RVA_PICKUP_RIGHT_GATE=0x0102C500ull;
static const u64 RVA_PICKUP_LEFT_SELECTOR=0x0102C880ull;
static const u64 RVA_PICKUP_LEFT_GATE=0x0102CB70ull;
static const u64 RVA_PICKUP_SHARED_GATE=0x011216A0ull;
static const u64 RVA_PICKUP_PRIMARY_INNER=0x01083EE0ull;
static const u64 RVA_PICKUP_FALLBACK_INNER=0x010899F0ull;
static const u64 RVA_PICKUP_RIGHT_START=0x0102C430ull;
static const u64 RVA_PICKUP_RIGHT_END=0x0102C4A0ull;
static const u64 RVA_PICKUP_LEFT_START=0x0102C9F0ull;
static const u64 RVA_PICKUP_LEFT_END=0x0102CB10ull;
static const u64 RVA_CLIMBING_ONLY_OBSERVER=0x00F46000ull;
static const u64 RVA_CATCH_RIGHT_SELECTOR=0x010298E0ull;
static const u64 RVA_CATCH_RIGHT_START=0x01029B70ull;
static const u64 RVA_CATCH_RIGHT_END=0x01029BB0ull;
static const u64 RVA_CATCH_LEFT_SELECTOR=0x0102A6E0ull;
static const u64 RVA_CATCH_LEFT_START=0x0102AD10ull;
static const u64 RVA_CATCH_LEFT_END=0x0102AD50ull;
static const u64 RVA_VTABLE_PICK_FROM_VEHICLE=0x0323CA70ull;
static const u64 RVA_VTABLE_PUT_TO_VEHICLE=0x0323F718ull;
static const u64 RVA_LOCAL_CLIMBING_GATE_STATUS=0x00DC0E70ull;
static const u64 RVA_LOCAL_GLOVE_FLAG_UPDATE=0x00E4AA80ull;
static const u64 RVA_ENABLE_PLAYER_ACTION_FLAG=0x0104DC40ull;
static const u64 RVA_DEFERRED_CANDIDATE_WRITER=0x01087490ull;
static const u64 RVA_INTERACTION_READER=0x00ED9B30ull;
static const u64 RVA_EQUIPPED_SUBCATEGORY_CHECK=0x00E9DC00ull;
static const u64 RVA_PICKUP_PROVIDER_SUBCAT_RETURN=0x01004025ull;
static const u64 RVA_F7_READER_RETURN=0x01009E4Aull;
static const u64 DEFERRED_RETURN_RVAS[7]={
    0x00F5A0C1ull,0x00FE298Full,0x00FF7752ull,0x00FF7879ull,
    0x0100A0F1ull,0x0109AD4Dull,0x0109ADA3ull
};
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
static const u8 EXPECTED_VEHICLE_COLLECTOR_ENTRY[5]={
    0x48,0x89,0x5C,0x24,0x08
};
static const u8 EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY[9]={
    0x40,0x53,0x48,0x81,0xEC,0x90,0x00,0x00,0x00
};
static const u8 EXPECTED_PICKUP_RIGHT_GATE_ENTRY[6]={
    0x40,0x53,0x48,0x83,0xEC,0x20
};
static const u8 EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY[9]={
    0x40,0x53,0x48,0x81,0xEC,0x90,0x00,0x00,0x00
};
static const u8 EXPECTED_PICKUP_LEFT_GATE_ENTRY[6]={
    0x40,0x53,0x48,0x83,0xEC,0x20
};
static const u8 EXPECTED_PICKUP_SHARED_GATE_ENTRY[5]={
    0x48,0x89,0x5C,0x24,0x08
};
static const u8 EXPECTED_PICKUP_PRIMARY_INNER_ENTRY[5]={
    0x48,0x89,0x5C,0x24,0x10
};
static const u8 EXPECTED_PICKUP_FALLBACK_INNER_ENTRY[5]={
    0x48,0x89,0x5C,0x24,0x08
};
static const u8 EXPECTED_PICKUP_RIGHT_START_ENTRY[7]={
    0x80,0xA1,0xB8,0x01,0x00,0x00,0xE3
};
static const u8 EXPECTED_PICKUP_RIGHT_END_ENTRY[6]={
    0x40,0x53,0x48,0x83,0xEC,0x20
};
static const u8 EXPECTED_PICKUP_LEFT_START_ENTRY[5]={
    0x48,0x89,0x5C,0x24,0x08
};
static const u8 EXPECTED_PICKUP_LEFT_END_ENTRY[6]={
    0x40,0x53,0x48,0x83,0xEC,0x20
};
static const u8 EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY[6]={
    0x40,0x57,0x48,0x83,0xEC,0x30
};
static const u8 EXPECTED_CATCH_SELECTOR_ENTRY[5]={0x48,0x89,0x5C,0x24,0x10};
static const u8 EXPECTED_CATCH_START_ENTRY[6]={0x40,0x53,0x48,0x83,0xEC,0x20};
static const u8 EXPECTED_CATCH_END_ENTRY[5]={0x48,0x89,0x5C,0x24,0x08};
static const u8 EXPECTED_LOCAL_CLIMBING_GATE_ENTRY[5]={0x48,0x89,0x5C,0x24,0x08};
static const u8 EXPECTED_LOCAL_GLOVE_FLAG_ENTRY[7]={0x40,0x53,0x55,0x41,0x56,0x41,0x57};
static const u8 EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY[5]={0x48,0x89,0x54,0x24,0x10};
static const u8 EXPECTED_INTERACTION_READER_ENTRY[10]={0x4D,0x63,0xD0,0x4D,0x69,0xC2,0xD0,0x04,0x00,0x00};
static const u8 EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY[6]={0x40,0x53,0x48,0x83,0xEC,0x20};

typedef float (*GloveActionCostFn)(u64,u8*,u8);
typedef u8 (*GloveActionAvailableFn)(u8*,u8,u8);
typedef u64 (*VehiclePickupCollectorFn)(u64,u64);
typedef u64 (*PickupGate1Fn)(u64);
typedef u32 (*PickupSharedGateFn)(u64,u8);
typedef u64 (*PickupInnerFn)(u64,u64,u64);
typedef void (*PickupActionFn)(u64);
typedef u64 (*LocalClimbingGateFn)(u64);
typedef void (*LocalGloveFlagUpdateFn)(u64,u64,u64);
typedef void (*EnablePlayerActionFlagFn)(u64,u8);
typedef u64 (*DeferredCandidateWriterFn)(u64,u64*,u8,u32);
typedef u64 (*InteractionReaderFn)(u64,u64,int,u32*);
typedef u64 (*EquippedSubcategoryCheckFn)(u64,u8,u8);

static GloveActionCostFn g_originalGloveActionCost=0;
static GloveActionAvailableFn g_originalGloveActionAvailable=0;
static VehiclePickupCollectorFn g_originalVehiclePickupCollector=0;
static PickupGate1Fn g_originalPickupRightSelector=0;
static PickupGate1Fn g_originalPickupRightGate=0;
static PickupGate1Fn g_originalPickupLeftSelector=0;
static PickupGate1Fn g_originalPickupLeftGate=0;
static PickupSharedGateFn g_originalPickupSharedGate=0;
static PickupInnerFn g_originalPickupPrimaryInner=0;
static PickupInnerFn g_originalPickupFallbackInner=0;
static PickupActionFn g_originalPickupRightStart=0;
static PickupActionFn g_originalPickupRightEnd=0;
static PickupActionFn g_originalPickupLeftStart=0;
static PickupActionFn g_originalPickupLeftEnd=0;
static PickupActionFn g_originalClimbingOnlyObserver=0;
static PickupGate1Fn g_originalCatchRightSelector=0;
static PickupActionFn g_originalCatchRightStart=0;
static PickupActionFn g_originalCatchRightEnd=0;
static PickupGate1Fn g_originalCatchLeftSelector=0;
static PickupActionFn g_originalCatchLeftStart=0;
static PickupActionFn g_originalCatchLeftEnd=0;
static u64 g_pickFromVehicleVtable=0;
static u64 g_putToVehicleVtable=0;
static LocalClimbingGateFn g_originalLocalClimbingGate=0;
static LocalGloveFlagUpdateFn g_originalLocalGloveFlagUpdate=0;
static EnablePlayerActionFlagFn g_enablePlayerActionFlag=0;
static DeferredCandidateWriterFn g_originalDeferredCandidateWriter=0;
static InteractionReaderFn g_originalInteractionReader=0;
static EquippedSubcategoryCheckFn g_originalEquippedSubcategoryCheck=0;
static u64 g_combatImageBase=0;
static u8* g_combatHookPage=0;
static u8* g_combatShadowPage=0;
static u8* g_combatObject1=0;
static u8* g_combatObject2=0;
static bool g_combatHooksInstalled=false;
static volatile long g_diagAvailabilityCalls[8][8]={};
static volatile long g_diagAvailabilityTrue[8][8]={};
static volatile long g_diagCombatRetryCalls=0;
static volatile long g_diagCombatRetryTrue=0;
static volatile long g_diagClimbingCostCalls[8]={};
static volatile long g_diagCombatCostCalls[8]={};
static volatile long g_diagVehicleCollectorCalls=0;
static volatile long g_diagVehicleCollectorAccepted=0;
static volatile long g_diagVehicleCollectorRejected=0;
static volatile long g_diagRightSelectorCalls=0;
static volatile long g_diagRightSelectorTrue=0;
static volatile long g_diagRightGateCalls=0;
static volatile long g_diagRightGateTrue=0;
static volatile long g_diagLeftSelectorCalls=0;
static volatile long g_diagLeftSelectorTrue=0;
static volatile long g_diagLeftGateCalls=0;
static volatile long g_diagLeftGateTrue=0;
static volatile long g_diagSharedGateCalls=0;
static volatile long g_diagSharedGateTrue=0;
static volatile long g_diagSharedGateForced=0;
static volatile long g_recentGloveSubcategory=0;
static volatile long g_vehicleSubcategorySpoofActive=0;
static volatile long g_diagVehicleSubcategoryToClimbing=0;
static volatile long g_diagVehicleSubcategoryToCombat=0;
static volatile long g_combatPickupExecuting=0;
static volatile long g_diagPickupActionStarts=0;
static volatile long g_diagPickupActionEnds=0;
static volatile long g_diagClimbingObserverBypass=0;
static volatile long g_diagCollectorPickCalls=0;
static volatile long g_diagCollectorPickZero=0;
static volatile long g_diagCollectorPutCalls=0;
static volatile long g_diagCollectorPutZero=0;
static volatile long g_diagCatchRightSelectorCalls=0;
static volatile long g_diagCatchRightSelectorTrue=0;
static volatile long g_diagCatchRightStarts=0;
static volatile long g_diagCatchRightEnds=0;
static volatile long g_diagCatchLeftSelectorCalls=0;
static volatile long g_diagCatchLeftSelectorTrue=0;
static volatile long g_diagCatchLeftStarts=0;
static volatile long g_diagCatchLeftEnds=0;
static volatile long g_diagLocalCombatGateTrue=0;
static volatile long g_diagLocalCombatFlag22Set=0;
static volatile long g_diagScopedUpdateSpoof=0;
static volatile long g_diagPrimaryInnerCalls=0;
static volatile long g_diagPrimaryInnerTrue=0;
static volatile long g_diagFallbackInnerCalls=0;
static volatile long g_diagFallbackInnerTrue=0;
static volatile long g_diagPrimaryBusy151=0;
static volatile long g_diagPrimaryFlag40=0;
static volatile long g_diagPrimaryId80=0;
static volatile long g_diagPrimaryId108=0;
static volatile long g_diagPrimaryId118=0;
static volatile long g_diagPrimaryId128=0;
static volatile long g_diagPrimaryId138=0;
static volatile long g_diagDeferredWriterCalls[8]={};
static volatile long g_diagDeferredWriterTrue[8]={};
static volatile long g_combatVehicleActive=0;
static volatile long g_diagF7FallbackCalls=0;
static volatile long g_diagF7B6True=0;
static volatile long g_diagF7B7True=0;
static volatile long g_diagF7B2True=0;
static volatile long g_diagInteractionScanHits[0xF9]={};
static volatile long g_liveDebugEnabled=1;
static volatile long g_liveScanGeneration=1;
static volatile long g_liveScanSeen=0;
static volatile long g_liveFallbackAction=-1;
static volatile long g_liveRootSubcategoryFallback=1;
static volatile long g_liveMapRightWriterToLeft=0;
static volatile long g_diagLiveFallbackCalls=0;
static volatile long g_diagLiveFallbackTrue=0;
static volatile long g_diagLiveFallbackAction=0;
static volatile long g_diagRootSubcatRetryCalls=0;
static volatile long g_diagRootSubcatRetryTrue=0;
static volatile long g_diagRightNativeCatch=0;
static volatile long g_diagDeferredSide8=0;
static volatile long g_diagDeferredSide9=0;
static volatile long g_diagDeferredSideOther=0;
static volatile long g_diagDeferredSide8To9=0;

static bool parseLiveInt(const WCHAR* value,int minimum,int maximum,int* out){
    if(!value||!out)return false;
    u32 begin=0,end=0;trimmedBounds(value,&begin,&end);if(begin==end)return false;
    bool negative=false;if(value[begin]==L'-'){negative=true;begin++;if(begin==end)return false;}
    int base=10;if(begin+1u<end&&value[begin]==L'0'&&(value[begin+1u]==L'x'||value[begin+1u]==L'X')){base=16;begin+=2u;if(begin==end)return false;}
    int parsed=0;
    for(u32 i=begin;i<end;i++){
        WCHAR c=value[i];int digit=-1;
        if(c>=L'0'&&c<=L'9')digit=(int)(c-L'0');
        else if(c>=L'a'&&c<=L'f')digit=10+(int)(c-L'a');
        else if(c>=L'A'&&c<=L'F')digit=10+(int)(c-L'A');
        else return false;
        if(digit<0||digit>=base)return false;
        if(parsed>1000000)return false;
        parsed=parsed*base+digit;
    }
    if(negative)parsed=-parsed;
    if(parsed<minimum||parsed>maximum)return false;
    *out=parsed;return true;
}
static void reloadCombatLiveSettings(){
    WCHAR path[520],value[64];modulePath(path,L"ds2_climbing_gloves_range.ini");if(!path[0])return;
    bool enabled=true;
    DWORD n=GetPrivateProfileStringW(L"CombatDebug",L"LiveDebug",L"1",value,64u,path);
    if(n&&n<63u&&parseBool(value,&enabled))_InterlockedExchange(&g_liveDebugEnabled,enabled?1:0);
    int generation=1;
    n=GetPrivateProfileStringW(L"CombatDebug",L"ScanGeneration",L"1",value,64u,path);
    if(n&&n<63u&&parseLiveInt(value,0,1000000,&generation))_InterlockedExchange(&g_liveScanGeneration,generation);
    int fallback=-1;
    n=GetPrivateProfileStringW(L"CombatDebug",L"FallbackAction",L"-1",value,64u,path);
    if(n&&n<63u&&parseLiveInt(value,-1,0xF8,&fallback))_InterlockedExchange(&g_liveFallbackAction,fallback);
    bool rootFallback=true;
    n=GetPrivateProfileStringW(L"CombatDebug",L"RootSubcategoryFallback",L"1",value,64u,path);
    if(n&&n<63u&&parseBool(value,&rootFallback))_InterlockedExchange(&g_liveRootSubcategoryFallback,rootFallback?1:0);
    bool mapRightWriter=false;
    n=GetPrivateProfileStringW(L"CombatDebug",L"MapRightWriterToLeft",L"0",value,64u,path);
    if(n&&n<63u&&parseBool(value,&mapRightWriter))_InterlockedExchange(&g_liveMapRightWriterToLeft,mapRightWriter?1:0);
}

static void diagIncrement(volatile long* value){
    long current=0;
    do{
        current=_InterlockedCompareExchange(value,0,0);
    }while(_InterlockedCompareExchange(value,current+1,current)!=current);
}
static u32 diagRead(volatile long* value){
    return (u32)_InterlockedCompareExchange(value,0,0);
}
static u32 diagAppendText(char* out,u32 pos,const char* text){
    if(!out||!text)return pos;
    while(*text&&pos<500u)out[pos++]=*text++;
    return pos;
}
static u32 diagAppendU32(char* out,u32 pos,u32 value){
    char tmp[16];u32 count=0;
    if(value==0u)tmp[count++]='0';
    while(value&&count<16u){
        tmp[count++]=(char)('0'+(value%10u));
        value/=10u;
    }
    while(count&&pos<500u)out[pos++]=tmp[--count];
    return pos;
}
static void diagLogCounter(
    const char* kind,u32 first,u32 second,u32 calls,u32 positives
){
    char line[512];u32 p=0;
    p=diagAppendText(line,p,"TRACE ");
    p=diagAppendText(line,p,kind);
    p=diagAppendText(line,p," first=");
    p=diagAppendU32(line,p,first);
    p=diagAppendText(line,p," second=");
    p=diagAppendU32(line,p,second);
    p=diagAppendText(line,p," calls=");
    p=diagAppendU32(line,p,calls);
    p=diagAppendText(line,p," true=");
    p=diagAppendU32(line,p,positives);
    if(p<510u){line[p++]='\r';line[p++]='\n';}
    logRaw(line,p);
}
static void logVehiclePickupDiagnostics(){
    static u32 lastAvail[8][8]={};
    static u32 lastTrue[8][8]={};
    static u32 lastClimbCost[8]={};
    static u32 lastCombatCost[8]={};
    static u32 lastRetryCalls=0,lastRetryTrue=0;
    static u32 lastVehicleCalls=0,lastVehicleAccepted=0,lastVehicleRejected=0;
    static u32 lastRightSelectorCalls=0,lastRightSelectorTrue=0;
    static u32 lastRightGateCalls=0,lastRightGateTrue=0;
    static u32 lastLeftSelectorCalls=0,lastLeftSelectorTrue=0;
    static u32 lastLeftGateCalls=0,lastLeftGateTrue=0;
    static u32 lastSharedGateCalls=0,lastSharedGateTrue=0,lastSharedGateForced=0;
    static u32 lastSubcatToClimb=0,lastSubcatToCombat=0;
    static u32 lastActionStarts=0,lastActionEnds=0,lastObserverBypass=0;
    static u32 lastCollectorPick=0,lastCollectorPickZero=0,lastCollectorPut=0,lastCollectorPutZero=0;
    static u32 lastCatchRS=0,lastCatchRST=0,lastCatchRStart=0,lastCatchREnd=0;
    static u32 lastCatchLS=0,lastCatchLST=0,lastCatchLStart=0,lastCatchLEnd=0;
    static u32 lastLocalGate=0,lastFlag22=0,lastScopedUpdate=0;
    static u32 lastPrimaryCalls=0,lastPrimaryTrue=0;
    static u32 lastFallbackCalls=0,lastFallbackTrue=0;
    static u32 lastBusy151=0,lastFlag40=0;
    static u32 lastId80=0,lastId108=0,lastId118=0,lastId128=0,lastId138=0;
    static u32 lastDeferredCalls[8]={};
    static u32 lastDeferredTrue[8]={};
    static u32 lastF7Fallback=0,lastF7B6=0,lastF7B7=0,lastF7B2=0;
    static u32 lastInteractionScanHits[0xF9]={};
    static u32 lastLiveFallbackCalls=0,lastLiveFallbackTrue=0,lastLiveFallbackAction=0;
    static u32 lastRootRetryCalls=0,lastRootRetryTrue=0;
    static u32 lastDeferredSide8=0,lastDeferredSide9=0,lastDeferredSideOther=0,lastDeferredSide8To9=0;

    for(u32 sub=6u;sub<=7u;sub++){
        for(u32 action=0u;action<8u;action++){
            u32 calls=diagRead(&g_diagAvailabilityCalls[sub][action]);
            u32 yes=diagRead(&g_diagAvailabilityTrue[sub][action]);
            if(calls!=lastAvail[sub][action]||yes!=lastTrue[sub][action]){
                diagLogCounter("availability",sub,action,calls,yes);
                lastAvail[sub][action]=calls;
                lastTrue[sub][action]=yes;
            }
        }
    }
    for(u32 action=0u;action<8u;action++){
        u32 climb=diagRead(&g_diagClimbingCostCalls[action]);
        if(climb!=lastClimbCost[action]){
            diagLogCounter("cost-climbing",6u,action,climb,0u);
            lastClimbCost[action]=climb;
        }
        u32 combat=diagRead(&g_diagCombatCostCalls[action]);
        if(combat!=lastCombatCost[action]){
            diagLogCounter("cost-combat",7u,action,combat,0u);
            lastCombatCost[action]=combat;
        }
    }
    u32 retryCalls=diagRead(&g_diagCombatRetryCalls);
    u32 retryTrue=diagRead(&g_diagCombatRetryTrue);
    if(retryCalls!=lastRetryCalls||retryTrue!=lastRetryTrue){
        diagLogCounter("combat-retry-action6",7u,6u,retryCalls,retryTrue);
        lastRetryCalls=retryCalls;lastRetryTrue=retryTrue;
    }
    u32 vehicleCalls=diagRead(&g_diagVehicleCollectorCalls);
    u32 vehicleAccepted=diagRead(&g_diagVehicleCollectorAccepted);
    u32 vehicleRejected=diagRead(&g_diagVehicleCollectorRejected);
    if(vehicleCalls!=lastVehicleCalls||vehicleAccepted!=lastVehicleAccepted||
       vehicleRejected!=lastVehicleRejected){
        char line[512];u32 p=0;
        p=diagAppendText(line,p,"TRACE vehicle-collector calls=");
        p=diagAppendU32(line,p,vehicleCalls);
        p=diagAppendText(line,p," accepted=");
        p=diagAppendU32(line,p,vehicleAccepted);
        p=diagAppendText(line,p," rejected=");
        p=diagAppendU32(line,p,vehicleRejected);
        if(p<510u){line[p++]='\r';line[p++]='\n';}
        logRaw(line,p);
        lastVehicleCalls=vehicleCalls;
        lastVehicleAccepted=vehicleAccepted;
        lastVehicleRejected=vehicleRejected;
    }

    u32 rightSelectorCalls=diagRead(&g_diagRightSelectorCalls);
    u32 rightSelectorTrue=diagRead(&g_diagRightSelectorTrue);
    if(rightSelectorCalls!=lastRightSelectorCalls||rightSelectorTrue!=lastRightSelectorTrue){
        diagLogCounter("pickup-right-selector",0u,0u,rightSelectorCalls,rightSelectorTrue);
        lastRightSelectorCalls=rightSelectorCalls;lastRightSelectorTrue=rightSelectorTrue;
    }
    u32 rightGateCalls=diagRead(&g_diagRightGateCalls);
    u32 rightGateTrue=diagRead(&g_diagRightGateTrue);
    if(rightGateCalls!=lastRightGateCalls||rightGateTrue!=lastRightGateTrue){
        diagLogCounter("pickup-right-gate",0u,0u,rightGateCalls,rightGateTrue);
        lastRightGateCalls=rightGateCalls;lastRightGateTrue=rightGateTrue;
    }
    u32 leftSelectorCalls=diagRead(&g_diagLeftSelectorCalls);
    u32 leftSelectorTrue=diagRead(&g_diagLeftSelectorTrue);
    if(leftSelectorCalls!=lastLeftSelectorCalls||leftSelectorTrue!=lastLeftSelectorTrue){
        diagLogCounter("pickup-left-selector",0u,0u,leftSelectorCalls,leftSelectorTrue);
        lastLeftSelectorCalls=leftSelectorCalls;lastLeftSelectorTrue=leftSelectorTrue;
    }
    u32 leftGateCalls=diagRead(&g_diagLeftGateCalls);
    u32 leftGateTrue=diagRead(&g_diagLeftGateTrue);
    if(leftGateCalls!=lastLeftGateCalls||leftGateTrue!=lastLeftGateTrue){
        diagLogCounter("pickup-left-gate",0u,0u,leftGateCalls,leftGateTrue);
        lastLeftGateCalls=leftGateCalls;lastLeftGateTrue=leftGateTrue;
    }
    u32 sharedGateCalls=diagRead(&g_diagSharedGateCalls);
    u32 sharedGateTrue=diagRead(&g_diagSharedGateTrue);
    if(sharedGateCalls!=lastSharedGateCalls||sharedGateTrue!=lastSharedGateTrue){
        diagLogCounter("pickup-shared-gate",0u,0u,sharedGateCalls,sharedGateTrue);
        lastSharedGateCalls=sharedGateCalls;lastSharedGateTrue=sharedGateTrue;
    }
    u32 sharedGateForced=diagRead(&g_diagSharedGateForced);
    if(sharedGateForced!=lastSharedGateForced){
        diagLogCounter("pickup-shared-gate-combat-vehicle-force",7u,0xEu,sharedGateForced,sharedGateForced);
        lastSharedGateForced=sharedGateForced;
    }

    u32 subcatToClimb=diagRead(&g_diagVehicleSubcategoryToClimbing);
    u32 subcatToCombat=diagRead(&g_diagVehicleSubcategoryToCombat);
    if(subcatToClimb!=lastSubcatToClimb||subcatToCombat!=lastSubcatToCombat){
        char line[512];u32 p=0;
        p=diagAppendText(line,p,"TRACE vehicle-subcategory climbing=");
        p=diagAppendU32(line,p,subcatToClimb);
        p=diagAppendText(line,p," combat=");
        p=diagAppendU32(line,p,subcatToCombat);
        if(p<510u){line[p++]='\r';line[p++]='\n';}
        logRaw(line,p);
        lastSubcatToClimb=subcatToClimb;lastSubcatToCombat=subcatToCombat;
    }
    u32 actionStarts=diagRead(&g_diagPickupActionStarts);
    u32 actionEnds=diagRead(&g_diagPickupActionEnds);
    if(actionStarts!=lastActionStarts||actionEnds!=lastActionEnds){
        char line[512];u32 p=0;
        p=diagAppendText(line,p,"TRACE pickup-action starts=");
        p=diagAppendU32(line,p,actionStarts);
        p=diagAppendText(line,p," ends=");
        p=diagAppendU32(line,p,actionEnds);
        if(p<510u){line[p++]='\r';line[p++]='\n';}
        logRaw(line,p);
        lastActionStarts=actionStarts;lastActionEnds=actionEnds;
    }
    u32 observerBypass=diagRead(&g_diagClimbingObserverBypass);
    if(observerBypass!=lastObserverBypass){
        diagLogCounter("climbing-observer-combat-bypass",7u,6u,observerBypass,observerBypass);
        lastObserverBypass=observerBypass;
    }

    u32 cp=diagRead(&g_diagCollectorPickCalls), cpz=diagRead(&g_diagCollectorPickZero);
    u32 cu=diagRead(&g_diagCollectorPutCalls), cuz=diagRead(&g_diagCollectorPutZero);
    if(cp!=lastCollectorPick||cpz!=lastCollectorPickZero){diagLogCounter("collector-pick-vehicle",0u,0u,cp,cpz);lastCollectorPick=cp;lastCollectorPickZero=cpz;}
    if(cu!=lastCollectorPut||cuz!=lastCollectorPutZero){diagLogCounter("collector-put-vehicle",0u,0u,cu,cuz);lastCollectorPut=cu;lastCollectorPutZero=cuz;}
    u32 crs=diagRead(&g_diagCatchRightSelectorCalls), crst=diagRead(&g_diagCatchRightSelectorTrue);
    u32 crb=diagRead(&g_diagCatchRightStarts), cre=diagRead(&g_diagCatchRightEnds);
    if(crs!=lastCatchRS||crst!=lastCatchRST){diagLogCounter("catch-right-selector",0u,0u,crs,crst);lastCatchRS=crs;lastCatchRST=crst;}
    if(crb!=lastCatchRStart||cre!=lastCatchREnd){diagLogCounter("catch-right-action",0u,0u,crb,cre);lastCatchRStart=crb;lastCatchREnd=cre;}
    u32 cls=diagRead(&g_diagCatchLeftSelectorCalls), clst=diagRead(&g_diagCatchLeftSelectorTrue);
    u32 clb=diagRead(&g_diagCatchLeftStarts), cle=diagRead(&g_diagCatchLeftEnds);
    if(cls!=lastCatchLS||clst!=lastCatchLST){diagLogCounter("catch-left-selector",0u,0u,cls,clst);lastCatchLS=cls;lastCatchLST=clst;}
    if(clb!=lastCatchLStart||cle!=lastCatchLEnd){diagLogCounter("catch-left-action",0u,0u,clb,cle);lastCatchLStart=clb;lastCatchLEnd=cle;}

    u32 localGate=diagRead(&g_diagLocalCombatGateTrue), flag22=diagRead(&g_diagLocalCombatFlag22Set);
    if(localGate!=lastLocalGate){diagLogCounter("local-combat-climbing-gate",7u,6u,localGate,localGate);lastLocalGate=localGate;}
    if(flag22!=lastFlag22){diagLogCounter("local-combat-flag22",7u,0x22u,flag22,flag22);lastFlag22=flag22;}

    u32 scopedUpdate=diagRead(&g_diagScopedUpdateSpoof);
    if(scopedUpdate!=lastScopedUpdate){diagLogCounter("scoped-update-combat-as-climbing",7u,6u,scopedUpdate,scopedUpdate);lastScopedUpdate=scopedUpdate;}

    for(u32 action=0u;action<=0xF8u;action++){
        u32 hits=diagRead(&g_diagInteractionScanHits[action]);
        if(hits!=lastInteractionScanHits[action]){
            diagLogCounter("interaction-scan-hit",action,0u,hits,hits);
            lastInteractionScanHits[action]=hits;
        }
    }

    static u32 lastRightNativeCatch=0;
    u32 rightNativeCatch=diagRead(&g_diagRightNativeCatch);
    if(rightNativeCatch!=lastRightNativeCatch){
        diagLogCounter("right-native-vehicle-catch",3u,0u,rightNativeCatch,rightNativeCatch);
        lastRightNativeCatch=rightNativeCatch;
    }

    u32 rootRetryCalls=diagRead(&g_diagRootSubcatRetryCalls);
    u32 rootRetryTrue=diagRead(&g_diagRootSubcatRetryTrue);
    if(rootRetryCalls!=lastRootRetryCalls||rootRetryTrue!=lastRootRetryTrue){
        diagLogCounter("root-subcat-6-to-7",6u,7u,rootRetryCalls,rootRetryTrue);
        lastRootRetryCalls=rootRetryCalls;lastRootRetryTrue=rootRetryTrue;
    }

    u32 liveFallbackCalls=diagRead(&g_diagLiveFallbackCalls);
    u32 liveFallbackTrue=diagRead(&g_diagLiveFallbackTrue);
    u32 liveFallbackAction=diagRead(&g_diagLiveFallbackAction);
    if(liveFallbackCalls!=lastLiveFallbackCalls||liveFallbackTrue!=lastLiveFallbackTrue||liveFallbackAction!=lastLiveFallbackAction){
        diagLogCounter("live-fallback",liveFallbackAction,0u,liveFallbackCalls,liveFallbackTrue);
        lastLiveFallbackCalls=liveFallbackCalls;lastLiveFallbackTrue=liveFallbackTrue;lastLiveFallbackAction=liveFallbackAction;
    }

    u32 f7Fallback=diagRead(&g_diagF7FallbackCalls);
    u32 f7b6=diagRead(&g_diagF7B6True),f7b7=diagRead(&g_diagF7B7True),f7b2=diagRead(&g_diagF7B2True);
    if(f7Fallback!=lastF7Fallback||f7b6!=lastF7B6||f7b7!=lastF7B7||f7b2!=lastF7B2){
        char line[512];u32 p=0;
        p=diagAppendText(line,p,"TRACE f7-fallback calls=");p=diagAppendU32(line,p,f7Fallback);
        p=diagAppendText(line,p," b6=");p=diagAppendU32(line,p,f7b6);
        p=diagAppendText(line,p," b7=");p=diagAppendU32(line,p,f7b7);
        p=diagAppendText(line,p," b2=");p=diagAppendU32(line,p,f7b2);
        if(p<510u){line[p++]='\r';line[p++]='\n';}logRaw(line,p);
        lastF7Fallback=f7Fallback;lastF7B6=f7b6;lastF7B7=f7b7;lastF7B2=f7b2;
    }

    u32 ds8=diagRead(&g_diagDeferredSide8),ds9=diagRead(&g_diagDeferredSide9),dso=diagRead(&g_diagDeferredSideOther),dsm=diagRead(&g_diagDeferredSide8To9);
    if(ds8!=lastDeferredSide8||ds9!=lastDeferredSide9||dso!=lastDeferredSideOther||dsm!=lastDeferredSide8To9){
        char line[512];u32 p=0;
        p=diagAppendText(line,p,"TRACE deferred-side side8=");p=diagAppendU32(line,p,ds8);
        p=diagAppendText(line,p," side9=");p=diagAppendU32(line,p,ds9);
        p=diagAppendText(line,p," other=");p=diagAppendU32(line,p,dso);
        p=diagAppendText(line,p," mapped8to9=");p=diagAppendU32(line,p,dsm);
        if(p<510u){line[p++]='\r';line[p++]='\n';}logRaw(line,p);
        lastDeferredSide8=ds8;lastDeferredSide9=ds9;lastDeferredSideOther=dso;lastDeferredSide8To9=dsm;
    }

    for(u32 i=0u;i<8u;i++){
        u32 calls=diagRead(&g_diagDeferredWriterCalls[i]);
        u32 yes=diagRead(&g_diagDeferredWriterTrue[i]);
        if(calls!=lastDeferredCalls[i]||yes!=lastDeferredTrue[i]){
            diagLogCounter("deferred-candidate-caller",i,(i<7u)?(u32)(DEFERRED_RETURN_RVAS[i]&0xFFFFFFFFu):0u,calls,yes);
            lastDeferredCalls[i]=calls;lastDeferredTrue[i]=yes;
        }
    }

    u32 primaryCalls=diagRead(&g_diagPrimaryInnerCalls);
    u32 primaryTrue=diagRead(&g_diagPrimaryInnerTrue);
    if(primaryCalls!=lastPrimaryCalls||primaryTrue!=lastPrimaryTrue){
        diagLogCounter("pickup-primary-inner",0u,0u,primaryCalls,primaryTrue);
        lastPrimaryCalls=primaryCalls;lastPrimaryTrue=primaryTrue;
    }
    u32 fallbackCalls=diagRead(&g_diagFallbackInnerCalls);
    u32 fallbackTrue=diagRead(&g_diagFallbackInnerTrue);
    if(fallbackCalls!=lastFallbackCalls||fallbackTrue!=lastFallbackTrue){
        diagLogCounter("pickup-fallback-inner",0u,0u,fallbackCalls,fallbackTrue);
        lastFallbackCalls=fallbackCalls;lastFallbackTrue=fallbackTrue;
    }
    u32 busy151=diagRead(&g_diagPrimaryBusy151);
    u32 flag40=diagRead(&g_diagPrimaryFlag40);
    u32 id80=diagRead(&g_diagPrimaryId80);
    u32 id108=diagRead(&g_diagPrimaryId108);
    u32 id118=diagRead(&g_diagPrimaryId118);
    u32 id128=diagRead(&g_diagPrimaryId128);
    u32 id138=diagRead(&g_diagPrimaryId138);
    if(busy151!=lastBusy151||flag40!=lastFlag40||id80!=lastId80||
       id108!=lastId108||id118!=lastId118||id128!=lastId128||id138!=lastId138){
        char line[512];u32 p=0;
        p=diagAppendText(line,p,"TRACE pickup-primary-state busy151=");
        p=diagAppendU32(line,p,busy151);
        p=diagAppendText(line,p," flag40=");
        p=diagAppendU32(line,p,flag40);
        p=diagAppendText(line,p," id80=");
        p=diagAppendU32(line,p,id80);
        p=diagAppendText(line,p," id108=");
        p=diagAppendU32(line,p,id108);
        p=diagAppendText(line,p," id118=");
        p=diagAppendU32(line,p,id118);
        p=diagAppendText(line,p," id128=");
        p=diagAppendU32(line,p,id128);
        p=diagAppendText(line,p," id138=");
        p=diagAppendU32(line,p,id138);
        if(p<510u){line[p++]='\r';line[p++]='\n';}
        logRaw(line,p);
        lastBusy151=busy151;lastFlag40=flag40;
        lastId80=id80;lastId108=id108;lastId118=id118;lastId128=id128;lastId138=id138;
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
       (subcategory!=COMBAT_SUBCATEGORY&&
        subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE)||
       (id!=COMBAT_ID_LEVEL1&&id!=COMBAT_ID_LEVEL2)||
       !readMemory(item+OFF_ITEM_PARAMS,&params,sizeof(params)))return false;
    return params.count==COMBAT_SHADOW_PARAM_COUNT&&
           params.capacity==COMBAT_SHADOW_PARAM_COUNT&&
           params.entries==(u64)combatShadowForId(id);
}

static float combatActionCostHook(u64 first,u8* entry,u8 action){
    if(entry&&action<8u){
        u64 itemPointer=0;
        if(readU64(entry+0x20u,&itemPointer)&&itemPointer){
            u8 subcategory=0;
            if(readU8((u8*)itemPointer+OFF_ITEM_SUBCATEGORY,&subcategory)){
                u8 itemId=0;readU8((u8*)itemPointer+OFF_ITEM_ID,&itemId);
                bool combatId=itemId==COMBAT_ID_LEVEL1||itemId==COMBAT_ID_LEVEL2;
                if(combatId)
                    _InterlockedExchange(&g_recentGloveSubcategory,(long)COMBAT_SUBCATEGORY);
                else if(subcategory==ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE)
                    _InterlockedExchange(&g_recentGloveSubcategory,(long)subcategory);
                if(combatId)diagIncrement(&g_diagCombatCostCalls[action]);
                else if(subcategory==ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE)
                    diagIncrement(&g_diagClimbingCostCalls[action]);
            }
        }
    }
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
    if(subcategory<8u&&action<8u){
        diagIncrement(&g_diagAvailabilityCalls[subcategory][action]);
        if(result)diagIncrement(&g_diagAvailabilityTrue[subcategory][action]);
    }
    if(result||subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE||action!=6u)
        return result;
    diagIncrement(&g_diagCombatRetryCalls);
    u8 retry=g_originalGloveActionAvailable(state,COMBAT_SUBCATEGORY,action);
    if(retry)diagIncrement(&g_diagCombatRetryTrue);
    return retry;
}

static bool recentCombatGlovesActive(){
    return _InterlockedCompareExchange(&g_recentGloveSubcategory,0,0)==COMBAT_SUBCATEGORY;
}
static u64 localClimbingGateHook(u64 context){
    return g_originalLocalClimbingGate?g_originalLocalClimbingGate(context):0ull;
}
static void localGloveFlagUpdateHook(u64 self,u64 p2,u64 p3){
    if(g_originalLocalGloveFlagUpdate)g_originalLocalGloveFlagUpdate(self,p2,p3);
}

static u64 equippedSubcategoryCheckHook(u64 state,u8 subcategory,u8 bypassDurability){
    u64 result=g_originalEquippedSubcategoryCheck?
        g_originalEquippedSubcategoryCheck(state,subcategory,bypassDurability):0ull;
    if((u8)result||subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE||bypassDurability!=0u||
       !recentCombatGlovesActive()||
       _InterlockedCompareExchange(&g_combatVehicleActive,0,0)==0||
       _InterlockedCompareExchange(&g_liveRootSubcategoryFallback,0,0)==0)return result;
    u64 returnAddress=(u64)__builtin_return_address(0);
    if(!g_combatImageBase||returnAddress-g_combatImageBase!=RVA_PICKUP_PROVIDER_SUBCAT_RETURN)return result;
    diagIncrement(&g_diagRootSubcatRetryCalls);
    u64 retry=g_originalEquippedSubcategoryCheck(state,COMBAT_SUBCATEGORY,0u);
    if((u8)retry)diagIncrement(&g_diagRootSubcatRetryTrue);
    return retry;
}

static u64 interactionReaderHook(u64 state,u64 owner,int action,u32* out){
    u64 result=g_originalInteractionReader?g_originalInteractionReader(state,owner,action,out):0ull;
    if((u8)result||action!=0xF7||!recentCombatGlovesActive()||
       _InterlockedCompareExchange(&g_combatVehicleActive,0,0)==0||
       _InterlockedCompareExchange(&g_liveDebugEnabled,0,0)==0)return result;
    u64 returnAddress=(u64)__builtin_return_address(0);
    if(!g_combatImageBase||returnAddress-g_combatImageBase!=RVA_F7_READER_RETURN)return result;
    diagIncrement(&g_diagF7FallbackCalls);

    long generation=_InterlockedCompareExchange(&g_liveScanGeneration,0,0);
    long seen=_InterlockedCompareExchange(&g_liveScanSeen,0,0);
    if(generation!=seen){
        for(int probe=0;probe<=0xF8;probe++){
            if(probe==0xF7)continue;
            u32 scratch[25];memset(scratch,0,sizeof(scratch));
            u64 hit=g_originalInteractionReader(state,owner,probe,scratch);
            if((u8)hit)diagIncrement(&g_diagInteractionScanHits[probe]);
        }
        _InterlockedExchange(&g_liveScanSeen,generation);
    }

    int fallback=(int)_InterlockedCompareExchange(&g_liveFallbackAction,0,0);
    if(fallback>=0&&fallback<=0xF8&&fallback!=0xF7){
        _InterlockedExchange(&g_diagLiveFallbackAction,fallback);
        diagIncrement(&g_diagLiveFallbackCalls);
        result=g_originalInteractionReader(state,owner,fallback,out);
        if((u8)result)diagIncrement(&g_diagLiveFallbackTrue);
        return result;
    }
    return result;
}

static u32 deferredCallerBucket(u64 returnAddress){
    if(!g_combatImageBase||returnAddress<g_combatImageBase)return 7u;
    u64 rva=returnAddress-g_combatImageBase;
    for(u32 i=0u;i<7u;i++)if(rva==DEFERRED_RETURN_RVAS[i])return i;
    return 7u;
}
static u64 deferredCandidateWriterHook(u64 work,u64* candidate,u8 side,u32 ttl){
    u64 returnAddress=(u64)__builtin_return_address(0);
    u32 bucket=deferredCallerBucket(returnAddress);
    diagIncrement(&g_diagDeferredWriterCalls[bucket]);
    if(bucket==4u&&recentCombatGlovesActive()&&_InterlockedCompareExchange(&g_combatVehicleActive,0,0)!=0){
        if(side==8u)diagIncrement(&g_diagDeferredSide8);
        else if(side==9u)diagIncrement(&g_diagDeferredSide9);
        else diagIncrement(&g_diagDeferredSideOther);
        if(side==8u&&_InterlockedCompareExchange(&g_liveMapRightWriterToLeft,0,0)!=0){
            side=9u;
            diagIncrement(&g_diagDeferredSide8To9);
        }
    }
    u64 result=g_originalDeferredCandidateWriter?
        g_originalDeferredCandidateWriter(work,candidate,side,ttl):0ull;
    if((u8)result)diagIncrement(&g_diagDeferredWriterTrue[bucket]);
    return result;
}

static u64 vehiclePickupCollectorHook(u64 self,u64 candidate){
    diagIncrement(&g_diagVehicleCollectorCalls);
    u64 vt=0; if(self)readU64((void*)self,&vt);
    u64 result=g_originalVehiclePickupCollector?g_originalVehiclePickupCollector(self,candidate):1ull;
    if(result)diagIncrement(&g_diagVehicleCollectorAccepted); else diagIncrement(&g_diagVehicleCollectorRejected);
    if(vt==g_pickFromVehicleVtable){diagIncrement(&g_diagCollectorPickCalls);if(result==0ull)diagIncrement(&g_diagCollectorPickZero);}
    if(vt==g_putToVehicleVtable){diagIncrement(&g_diagCollectorPutCalls);if(result==0ull)diagIncrement(&g_diagCollectorPutZero);}
    return result;
}

static bool combatVehicleState(u64 self);
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
    diagIncrement(&g_diagCatchRightSelectorCalls);
    if(prepareRightVehicleCatch(self)){
        diagIncrement(&g_diagRightNativeCatch);
        diagIncrement(&g_diagCatchRightSelectorTrue);
        return 1ull;
    }
    u64 r=g_originalCatchRightSelector?g_originalCatchRightSelector(self):0ull;
    if((u8)r)diagIncrement(&g_diagCatchRightSelectorTrue);
    return r;
}

static void catchRightStartHook(u64 self){
    diagIncrement(&g_diagCatchRightStarts);
    if(g_originalCatchRightStart)g_originalCatchRightStart(self);
}
static void catchRightEndHook(u64 self){if(g_originalCatchRightEnd)g_originalCatchRightEnd(self);diagIncrement(&g_diagCatchRightEnds);}
static u64 catchLeftSelectorHook(u64 self){diagIncrement(&g_diagCatchLeftSelectorCalls);u64 r=g_originalCatchLeftSelector?g_originalCatchLeftSelector(self):0ull;if((u8)r)diagIncrement(&g_diagCatchLeftSelectorTrue);return r;}
static void catchLeftStartHook(u64 self){diagIncrement(&g_diagCatchLeftStarts);if(g_originalCatchLeftStart)g_originalCatchLeftStart(self);}
static void catchLeftEndHook(u64 self){if(g_originalCatchLeftEnd)g_originalCatchLeftEnd(self);diagIncrement(&g_diagCatchLeftEnds);}

static u64 pickupRightSelectorHook(u64 self){
    diagIncrement(&g_diagRightSelectorCalls);
    u64 result=g_originalPickupRightSelector?g_originalPickupRightSelector(self):0ull;
    if((u8)result)diagIncrement(&g_diagRightSelectorTrue);
    return result;
}
static bool combatVehicleState(u64 self){
    if(!self||!recentCombatGlovesActive())return false;
    u64 playerState=0;u32 movementState=0;
    return readU64((void*)(self+0x28u),&playerState)&&playerState&&
           readU32((void*)(playerState+0x7548u),&movementState)&&movementState==0xEu;
}
static u64 pickupRightGateHook(u64 self){
    diagIncrement(&g_diagRightGateCalls);
    u64 result=g_originalPickupRightGate?g_originalPickupRightGate(self):0ull;
    if(!(u8)result&&combatVehicleState(self))result=1ull;
    if((u8)result)diagIncrement(&g_diagRightGateTrue);
    return result;
}
static u64 pickupLeftSelectorHook(u64 self){
    diagIncrement(&g_diagLeftSelectorCalls);
    u64 result=g_originalPickupLeftSelector?g_originalPickupLeftSelector(self):0ull;
    if((u8)result)diagIncrement(&g_diagLeftSelectorTrue);
    return result;
}
static u64 pickupLeftGateHook(u64 self){
    diagIncrement(&g_diagLeftGateCalls);
    u64 result=g_originalPickupLeftGate?g_originalPickupLeftGate(self):0ull;
    if(!(u8)result&&combatVehicleState(self))result=1ull;
    if((u8)result)diagIncrement(&g_diagLeftGateTrue);
    return result;
}
static u32 pickupSharedGateHook(u64 self,u8 mode){
    diagIncrement(&g_diagSharedGateCalls);
    u64 playerState=0;u32 movementState=0;
    bool stateKnown=self&&readU64((void*)(self+0x28u),&playerState)&&playerState&&
        readU32((void*)(playerState+0x7548u),&movementState);
    long recent=_InterlockedCompareExchange(&g_recentGloveSubcategory,0,0);
    bool combatEquipped=recent==COMBAT_SUBCATEGORY;
    if(combatEquipped&&stateKnown)_InterlockedExchange(&g_combatVehicleActive,movementState==0xEu?1:0);
    else if(stateKnown)_InterlockedExchange(&g_combatVehicleActive,0);
    u32 result=g_originalPickupSharedGate?g_originalPickupSharedGate(self,mode):0u;
    if((u8)result){
        diagIncrement(&g_diagSharedGateTrue);
        return result;
    }
    if(!combatEquipped||!stateKnown||movementState!=0xEu)return result;
    diagIncrement(&g_diagSharedGateForced);
    diagIncrement(&g_diagSharedGateTrue);
    return 1u;
}

static void pickupRightStartHook(u64 self){
    if(recentCombatGlovesActive()){
        diagIncrement(&g_diagPickupActionStarts);
    }
    if(g_originalPickupRightStart)g_originalPickupRightStart(self);
}
static void pickupLeftStartHook(u64 self){
    if(recentCombatGlovesActive()){diagIncrement(&g_diagPickupActionStarts);}
    if(g_originalPickupLeftStart)g_originalPickupLeftStart(self);
}
static void pickupRightEndHook(u64 self){
    if(g_originalPickupRightEnd)g_originalPickupRightEnd(self);
    if(recentCombatGlovesActive())diagIncrement(&g_diagPickupActionEnds);
}
static void pickupLeftEndHook(u64 self){
    if(g_originalPickupLeftEnd)g_originalPickupLeftEnd(self);
    if(recentCombatGlovesActive())diagIncrement(&g_diagPickupActionEnds);
}
static void climbingOnlyObserverHook(u64 self){
    u64 itemPointer=0;u8 id=0,subcategory=0;
    if(self&&readU64((void*)(self+0x58u),&itemPointer)&&itemPointer&&
       readU8((void*)(itemPointer+OFF_ITEM_ID),&id)&&
       readU8((void*)(itemPointer+OFF_ITEM_SUBCATEGORY),&subcategory)&&
       (id==COMBAT_ID_LEVEL1||id==COMBAT_ID_LEVEL2)&&
       subcategory==ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE){
        diagIncrement(&g_diagClimbingObserverBypass);
        return;
    }
    if(g_originalClimbingOnlyObserver)g_originalClimbingOnlyObserver(self);
}

static bool diagQwordPresent(u64 address){
    u64 value=0;
    return readU64((void*)address,&value)&&value!=0xffffffffffffffffull&&value!=0ull;
}
static u64 pickupPrimaryInnerHook(u64 work,u64 owner,u64 resultData){
    diagIncrement(&g_diagPrimaryInnerCalls);
    if(work){
        u8 b=0;
        if(readU8((void*)(work+0x151u),&b)&&b)diagIncrement(&g_diagPrimaryBusy151);
        if(readU8((void*)(work+0x69u),&b)&&(b&0x40u))diagIncrement(&g_diagPrimaryFlag40);
        if(diagQwordPresent(work+0x80u))diagIncrement(&g_diagPrimaryId80);
        if(diagQwordPresent(work+0x108u))diagIncrement(&g_diagPrimaryId108);
        if(diagQwordPresent(work+0x118u))diagIncrement(&g_diagPrimaryId118);
        if(diagQwordPresent(work+0x128u))diagIncrement(&g_diagPrimaryId128);
        if(diagQwordPresent(work+0x138u))diagIncrement(&g_diagPrimaryId138);
    }
    u64 result=g_originalPickupPrimaryInner?
        g_originalPickupPrimaryInner(work,owner,resultData):0ull;
    if((u8)result)diagIncrement(&g_diagPrimaryInnerTrue);
    return result;
}
static u64 pickupFallbackInnerHook(u64 work,u64 owner,u64 resultData){
    diagIncrement(&g_diagFallbackInnerCalls);
    u64 result=g_originalPickupFallbackInner?
        g_originalPickupFallbackInner(work,owner,resultData):0ull;
    if((u8)result)diagIncrement(&g_diagFallbackInnerTrue);
    return result;
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
    u8* vehicleCollector=image->base+RVA_VEHICLE_PICKUP_COLLECTOR;
    if(!memoryEquals(cost,EXPECTED_COST_ENTRY,sizeof(EXPECTED_COST_ENTRY))||
       !memoryEquals(available,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY))||
       !memoryEquals(
           vehicleCollector,EXPECTED_VEHICLE_COLLECTOR_ENTRY,
           sizeof(EXPECTED_VEHICLE_COLLECTOR_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_RIGHT_SELECTOR,EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY,
           sizeof(EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_RIGHT_GATE,EXPECTED_PICKUP_RIGHT_GATE_ENTRY,
           sizeof(EXPECTED_PICKUP_RIGHT_GATE_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_LEFT_SELECTOR,EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY,
           sizeof(EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_LEFT_GATE,EXPECTED_PICKUP_LEFT_GATE_ENTRY,
           sizeof(EXPECTED_PICKUP_LEFT_GATE_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_SHARED_GATE,EXPECTED_PICKUP_SHARED_GATE_ENTRY,
           sizeof(EXPECTED_PICKUP_SHARED_GATE_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_PRIMARY_INNER,EXPECTED_PICKUP_PRIMARY_INNER_ENTRY,
           sizeof(EXPECTED_PICKUP_PRIMARY_INNER_ENTRY)
       )||
       !memoryEquals(
           image->base+RVA_PICKUP_FALLBACK_INNER,EXPECTED_PICKUP_FALLBACK_INNER_ENTRY,
           sizeof(EXPECTED_PICKUP_FALLBACK_INNER_ENTRY)
       )||
       !memoryEquals(image->base+RVA_PICKUP_RIGHT_START,EXPECTED_PICKUP_RIGHT_START_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_START_ENTRY))||
       !memoryEquals(image->base+RVA_PICKUP_RIGHT_END,EXPECTED_PICKUP_RIGHT_END_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_END_ENTRY))||
       !memoryEquals(image->base+RVA_PICKUP_LEFT_START,EXPECTED_PICKUP_LEFT_START_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_START_ENTRY))||
       !memoryEquals(image->base+RVA_PICKUP_LEFT_END,EXPECTED_PICKUP_LEFT_END_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_END_ENTRY))||
       !memoryEquals(image->base+RVA_CLIMBING_ONLY_OBSERVER,EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY,sizeof(EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY))||
       !memoryEquals(image->base+RVA_CATCH_RIGHT_SELECTOR,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY))||
       !memoryEquals(image->base+RVA_CATCH_RIGHT_START,EXPECTED_CATCH_START_ENTRY,sizeof(EXPECTED_CATCH_START_ENTRY))||
       !memoryEquals(image->base+RVA_CATCH_RIGHT_END,EXPECTED_CATCH_END_ENTRY,sizeof(EXPECTED_CATCH_END_ENTRY))||
       !memoryEquals(image->base+RVA_CATCH_LEFT_SELECTOR,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY))||
       !memoryEquals(image->base+RVA_CATCH_LEFT_START,EXPECTED_CATCH_START_ENTRY,sizeof(EXPECTED_CATCH_START_ENTRY))||
       !memoryEquals(image->base+RVA_CATCH_LEFT_END,EXPECTED_CATCH_END_ENTRY,sizeof(EXPECTED_CATCH_END_ENTRY))||
       !memoryEquals(image->base+RVA_LOCAL_CLIMBING_GATE_STATUS,EXPECTED_LOCAL_CLIMBING_GATE_ENTRY,sizeof(EXPECTED_LOCAL_CLIMBING_GATE_ENTRY))||
       !memoryEquals(image->base+RVA_LOCAL_GLOVE_FLAG_UPDATE,EXPECTED_LOCAL_GLOVE_FLAG_ENTRY,sizeof(EXPECTED_LOCAL_GLOVE_FLAG_ENTRY))||
       !memoryEquals(image->base+RVA_DEFERRED_CANDIDATE_WRITER,EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY,sizeof(EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY))||
       !memoryEquals(image->base+RVA_INTERACTION_READER,EXPECTED_INTERACTION_READER_ENTRY,sizeof(EXPECTED_INTERACTION_READER_ENTRY))||
       !memoryEquals(image->base+RVA_EQUIPPED_SUBCATEGORY_CHECK,EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY,sizeof(EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY)))
        return false;
    g_combatHookPage=allocateCombatHookPage(available);
    if(!g_combatHookPage)return false;
    u8* availableRelay=g_combatHookPage;
    u8* costRelay=g_combatHookPage+0x20u;
    u8* availableTrampoline=g_combatHookPage+0x40u;
    u8* costTrampoline=g_combatHookPage+0x80u;
    u8* vehicleRelay=g_combatHookPage+0xC0u;
    u8* vehicleTrampoline=g_combatHookPage+0xE0u;
    u8* rightSelectorRelay=g_combatHookPage+0x120u;
    u8* rightSelectorTrampoline=g_combatHookPage+0x140u;
    u8* rightGateRelay=g_combatHookPage+0x180u;
    u8* rightGateTrampoline=g_combatHookPage+0x1A0u;
    u8* leftSelectorRelay=g_combatHookPage+0x1E0u;
    u8* leftSelectorTrampoline=g_combatHookPage+0x200u;
    u8* leftGateRelay=g_combatHookPage+0x240u;
    u8* leftGateTrampoline=g_combatHookPage+0x260u;
    u8* sharedGateRelay=g_combatHookPage+0x2A0u;
    u8* sharedGateTrampoline=g_combatHookPage+0x2C0u;
    u8* primaryInnerRelay=g_combatHookPage+0x300u;
    u8* primaryInnerTrampoline=g_combatHookPage+0x320u;
    u8* fallbackInnerRelay=g_combatHookPage+0x360u;
    u8* fallbackInnerTrampoline=g_combatHookPage+0x380u;
    u8* rightStartRelay=g_combatHookPage+0x3C0u;
    u8* rightStartTrampoline=g_combatHookPage+0x3E0u;
    u8* rightEndRelay=g_combatHookPage+0x420u;
    u8* rightEndTrampoline=g_combatHookPage+0x440u;
    u8* leftStartRelay=g_combatHookPage+0x480u;
    u8* leftStartTrampoline=g_combatHookPage+0x4A0u;
    u8* leftEndRelay=g_combatHookPage+0x4E0u;
    u8* leftEndTrampoline=g_combatHookPage+0x500u;
    u8* observerRelay=g_combatHookPage+0x540u;
    u8* observerTrampoline=g_combatHookPage+0x560u;
    u8* catchRSRelay=g_combatHookPage+0x5A0u; u8* catchRSTramp=g_combatHookPage+0x5C0u;
    u8* catchRStartRelay=g_combatHookPage+0x600u; u8* catchRStartTramp=g_combatHookPage+0x620u;
    u8* catchREndRelay=g_combatHookPage+0x660u; u8* catchREndTramp=g_combatHookPage+0x680u;
    u8* catchLSRelay=g_combatHookPage+0x6C0u; u8* catchLSTramp=g_combatHookPage+0x6E0u;
    u8* catchLStartRelay=g_combatHookPage+0x720u; u8* catchLStartTramp=g_combatHookPage+0x740u;
    u8* catchLEndRelay=g_combatHookPage+0x780u; u8* catchLEndTramp=g_combatHookPage+0x7A0u;
    u8* localGateRelay=g_combatHookPage+0x800u; u8* localGateTramp=g_combatHookPage+0x820u;
    u8* localFlagRelay=g_combatHookPage+0x860u; u8* localFlagTramp=g_combatHookPage+0x880u;
    u8* deferredWriterRelay=g_combatHookPage+0x8C0u; u8* deferredWriterTramp=g_combatHookPage+0x8E0u;
    u8* interactionReaderRelay=g_combatHookPage+0x920u; u8* interactionReaderTramp=g_combatHookPage+0x940u;
    u8* subcatCheckRelay=g_combatHookPage+0x980u; u8* subcatCheckTramp=g_combatHookPage+0x9A0u;

    buildAbsoluteJump(availableRelay,(u8*)&combatActionAvailableHook);
    buildAbsoluteJump(costRelay,(u8*)&combatActionCostHook);
    buildAbsoluteJump(vehicleRelay,(u8*)&vehiclePickupCollectorHook);
    buildAbsoluteJump(rightSelectorRelay,(u8*)&pickupRightSelectorHook);
    buildAbsoluteJump(rightGateRelay,(u8*)&pickupRightGateHook);
    buildAbsoluteJump(leftSelectorRelay,(u8*)&pickupLeftSelectorHook);
    buildAbsoluteJump(leftGateRelay,(u8*)&pickupLeftGateHook);
    buildAbsoluteJump(sharedGateRelay,(u8*)&pickupSharedGateHook);
    buildAbsoluteJump(primaryInnerRelay,(u8*)&pickupPrimaryInnerHook);
    buildAbsoluteJump(fallbackInnerRelay,(u8*)&pickupFallbackInnerHook);
    buildAbsoluteJump(rightStartRelay,(u8*)&pickupRightStartHook);
    buildAbsoluteJump(rightEndRelay,(u8*)&pickupRightEndHook);
    buildAbsoluteJump(leftStartRelay,(u8*)&pickupLeftStartHook);
    buildAbsoluteJump(leftEndRelay,(u8*)&pickupLeftEndHook);
    buildAbsoluteJump(observerRelay,(u8*)&climbingOnlyObserverHook);
    buildAbsoluteJump(catchRSRelay,(u8*)&catchRightSelectorHook); buildAbsoluteJump(catchRStartRelay,(u8*)&catchRightStartHook); buildAbsoluteJump(catchREndRelay,(u8*)&catchRightEndHook);
    buildAbsoluteJump(catchLSRelay,(u8*)&catchLeftSelectorHook); buildAbsoluteJump(catchLStartRelay,(u8*)&catchLeftStartHook); buildAbsoluteJump(catchLEndRelay,(u8*)&catchLeftEndHook);
    buildAbsoluteJump(localGateRelay,(u8*)&localClimbingGateHook);
    buildAbsoluteJump(localFlagRelay,(u8*)&localGloveFlagUpdateHook);
    buildAbsoluteJump(deferredWriterRelay,(u8*)&deferredCandidateWriterHook);
    buildAbsoluteJump(interactionReaderRelay,(u8*)&interactionReaderHook);
    buildAbsoluteJump(subcatCheckRelay,(u8*)&equippedSubcategoryCheckHook);
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
    memcpy(
        vehicleTrampoline,EXPECTED_VEHICLE_COLLECTOR_ENTRY,
        sizeof(EXPECTED_VEHICLE_COLLECTOR_ENTRY)
    );
    buildAbsoluteJump(
        vehicleTrampoline+sizeof(EXPECTED_VEHICLE_COLLECTOR_ENTRY),
        vehicleCollector+sizeof(EXPECTED_VEHICLE_COLLECTOR_ENTRY)
    );

    u8* rightSelector=image->base+RVA_PICKUP_RIGHT_SELECTOR;
    u8* rightGate=image->base+RVA_PICKUP_RIGHT_GATE;
    u8* leftSelector=image->base+RVA_PICKUP_LEFT_SELECTOR;
    u8* leftGate=image->base+RVA_PICKUP_LEFT_GATE;
    u8* sharedGate=image->base+RVA_PICKUP_SHARED_GATE;

    memcpy(
        rightSelectorTrampoline,EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY,
        sizeof(EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY)
    );
    buildAbsoluteJump(
        rightSelectorTrampoline+sizeof(EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY),
        rightSelector+sizeof(EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY)
    );
    memcpy(
        rightGateTrampoline,EXPECTED_PICKUP_RIGHT_GATE_ENTRY,
        sizeof(EXPECTED_PICKUP_RIGHT_GATE_ENTRY)
    );
    buildAbsoluteJump(
        rightGateTrampoline+sizeof(EXPECTED_PICKUP_RIGHT_GATE_ENTRY),
        rightGate+sizeof(EXPECTED_PICKUP_RIGHT_GATE_ENTRY)
    );
    memcpy(
        leftSelectorTrampoline,EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY,
        sizeof(EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY)
    );
    buildAbsoluteJump(
        leftSelectorTrampoline+sizeof(EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY),
        leftSelector+sizeof(EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY)
    );
    memcpy(
        leftGateTrampoline,EXPECTED_PICKUP_LEFT_GATE_ENTRY,
        sizeof(EXPECTED_PICKUP_LEFT_GATE_ENTRY)
    );
    buildAbsoluteJump(
        leftGateTrampoline+sizeof(EXPECTED_PICKUP_LEFT_GATE_ENTRY),
        leftGate+sizeof(EXPECTED_PICKUP_LEFT_GATE_ENTRY)
    );
    memcpy(
        sharedGateTrampoline,EXPECTED_PICKUP_SHARED_GATE_ENTRY,
        sizeof(EXPECTED_PICKUP_SHARED_GATE_ENTRY)
    );
    buildAbsoluteJump(
        sharedGateTrampoline+sizeof(EXPECTED_PICKUP_SHARED_GATE_ENTRY),
        sharedGate+sizeof(EXPECTED_PICKUP_SHARED_GATE_ENTRY)
    );

    u8* primaryInner=image->base+RVA_PICKUP_PRIMARY_INNER;
    u8* fallbackInner=image->base+RVA_PICKUP_FALLBACK_INNER;
    memcpy(
        primaryInnerTrampoline,EXPECTED_PICKUP_PRIMARY_INNER_ENTRY,
        sizeof(EXPECTED_PICKUP_PRIMARY_INNER_ENTRY)
    );
    buildAbsoluteJump(
        primaryInnerTrampoline+sizeof(EXPECTED_PICKUP_PRIMARY_INNER_ENTRY),
        primaryInner+sizeof(EXPECTED_PICKUP_PRIMARY_INNER_ENTRY)
    );
    memcpy(
        fallbackInnerTrampoline,EXPECTED_PICKUP_FALLBACK_INNER_ENTRY,
        sizeof(EXPECTED_PICKUP_FALLBACK_INNER_ENTRY)
    );
    buildAbsoluteJump(
        fallbackInnerTrampoline+sizeof(EXPECTED_PICKUP_FALLBACK_INNER_ENTRY),
        fallbackInner+sizeof(EXPECTED_PICKUP_FALLBACK_INNER_ENTRY)
    );
    u8* rightStart=image->base+RVA_PICKUP_RIGHT_START;
    u8* rightEnd=image->base+RVA_PICKUP_RIGHT_END;
    u8* leftStart=image->base+RVA_PICKUP_LEFT_START;
    u8* leftEnd=image->base+RVA_PICKUP_LEFT_END;
    memcpy(rightStartTrampoline,EXPECTED_PICKUP_RIGHT_START_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_START_ENTRY));
    buildAbsoluteJump(rightStartTrampoline+sizeof(EXPECTED_PICKUP_RIGHT_START_ENTRY),rightStart+sizeof(EXPECTED_PICKUP_RIGHT_START_ENTRY));
    memcpy(rightEndTrampoline,EXPECTED_PICKUP_RIGHT_END_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_END_ENTRY));
    buildAbsoluteJump(rightEndTrampoline+sizeof(EXPECTED_PICKUP_RIGHT_END_ENTRY),rightEnd+sizeof(EXPECTED_PICKUP_RIGHT_END_ENTRY));
    memcpy(leftStartTrampoline,EXPECTED_PICKUP_LEFT_START_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_START_ENTRY));
    buildAbsoluteJump(leftStartTrampoline+sizeof(EXPECTED_PICKUP_LEFT_START_ENTRY),leftStart+sizeof(EXPECTED_PICKUP_LEFT_START_ENTRY));
    memcpy(leftEndTrampoline,EXPECTED_PICKUP_LEFT_END_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_END_ENTRY));
    buildAbsoluteJump(leftEndTrampoline+sizeof(EXPECTED_PICKUP_LEFT_END_ENTRY),leftEnd+sizeof(EXPECTED_PICKUP_LEFT_END_ENTRY));
    u8* climbingObserver=image->base+RVA_CLIMBING_ONLY_OBSERVER;
    memcpy(observerTrampoline,EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY,sizeof(EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY));
    buildAbsoluteJump(observerTrampoline+sizeof(EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY),climbingObserver+sizeof(EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY));
    u8* catchRS=image->base+RVA_CATCH_RIGHT_SELECTOR;u8* catchRStart=image->base+RVA_CATCH_RIGHT_START;u8* catchREnd=image->base+RVA_CATCH_RIGHT_END;
    u8* catchLS=image->base+RVA_CATCH_LEFT_SELECTOR;u8* catchLStart=image->base+RVA_CATCH_LEFT_START;u8* catchLEnd=image->base+RVA_CATCH_LEFT_END;
    memcpy(catchRSTramp,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY));buildAbsoluteJump(catchRSTramp+sizeof(EXPECTED_CATCH_SELECTOR_ENTRY),catchRS+sizeof(EXPECTED_CATCH_SELECTOR_ENTRY));
    memcpy(catchRStartTramp,EXPECTED_CATCH_START_ENTRY,sizeof(EXPECTED_CATCH_START_ENTRY));buildAbsoluteJump(catchRStartTramp+sizeof(EXPECTED_CATCH_START_ENTRY),catchRStart+sizeof(EXPECTED_CATCH_START_ENTRY));
    memcpy(catchREndTramp,EXPECTED_CATCH_END_ENTRY,sizeof(EXPECTED_CATCH_END_ENTRY));buildAbsoluteJump(catchREndTramp+sizeof(EXPECTED_CATCH_END_ENTRY),catchREnd+sizeof(EXPECTED_CATCH_END_ENTRY));
    memcpy(catchLSTramp,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY));buildAbsoluteJump(catchLSTramp+sizeof(EXPECTED_CATCH_SELECTOR_ENTRY),catchLS+sizeof(EXPECTED_CATCH_SELECTOR_ENTRY));
    memcpy(catchLStartTramp,EXPECTED_CATCH_START_ENTRY,sizeof(EXPECTED_CATCH_START_ENTRY));buildAbsoluteJump(catchLStartTramp+sizeof(EXPECTED_CATCH_START_ENTRY),catchLStart+sizeof(EXPECTED_CATCH_START_ENTRY));
    memcpy(catchLEndTramp,EXPECTED_CATCH_END_ENTRY,sizeof(EXPECTED_CATCH_END_ENTRY));buildAbsoluteJump(catchLEndTramp+sizeof(EXPECTED_CATCH_END_ENTRY),catchLEnd+sizeof(EXPECTED_CATCH_END_ENTRY));
    g_pickFromVehicleVtable=(u64)(image->base+RVA_VTABLE_PICK_FROM_VEHICLE);g_putToVehicleVtable=(u64)(image->base+RVA_VTABLE_PUT_TO_VEHICLE);
    u8* localGate=image->base+RVA_LOCAL_CLIMBING_GATE_STATUS;u8* localFlag=image->base+RVA_LOCAL_GLOVE_FLAG_UPDATE;
    memcpy(localGateTramp,EXPECTED_LOCAL_CLIMBING_GATE_ENTRY,sizeof(EXPECTED_LOCAL_CLIMBING_GATE_ENTRY));buildAbsoluteJump(localGateTramp+sizeof(EXPECTED_LOCAL_CLIMBING_GATE_ENTRY),localGate+sizeof(EXPECTED_LOCAL_CLIMBING_GATE_ENTRY));
    memcpy(localFlagTramp,EXPECTED_LOCAL_GLOVE_FLAG_ENTRY,sizeof(EXPECTED_LOCAL_GLOVE_FLAG_ENTRY));buildAbsoluteJump(localFlagTramp+sizeof(EXPECTED_LOCAL_GLOVE_FLAG_ENTRY),localFlag+sizeof(EXPECTED_LOCAL_GLOVE_FLAG_ENTRY));
    u8* deferredWriter=image->base+RVA_DEFERRED_CANDIDATE_WRITER;
    memcpy(deferredWriterTramp,EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY,sizeof(EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY));
    buildAbsoluteJump(deferredWriterTramp+sizeof(EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY),deferredWriter+sizeof(EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY));
    u8* interactionReader=image->base+RVA_INTERACTION_READER;
    memcpy(interactionReaderTramp,EXPECTED_INTERACTION_READER_ENTRY,sizeof(EXPECTED_INTERACTION_READER_ENTRY));
    buildAbsoluteJump(interactionReaderTramp+sizeof(EXPECTED_INTERACTION_READER_ENTRY),interactionReader+sizeof(EXPECTED_INTERACTION_READER_ENTRY));
    u8* subcatCheck=image->base+RVA_EQUIPPED_SUBCATEGORY_CHECK;
    memcpy(subcatCheckTramp,EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY,sizeof(EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY));
    buildAbsoluteJump(subcatCheckTramp+sizeof(EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY),subcatCheck+sizeof(EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY));
    g_enablePlayerActionFlag=(EnablePlayerActionFlagFn)(image->base+RVA_ENABLE_PLAYER_ACTION_FLAG);
    g_combatImageBase=(u64)image->base;
    DWORD oldProtection=0;
    if(!VirtualProtect(g_combatHookPage,0x1000u,PAGE_EXECUTE_READ,&oldProtection))
        return false;
    FlushInstructionCache(GetCurrentProcess(),g_combatHookPage,0x1000u);
    if(!rel32Fits(available+5u,availableRelay)||
       !rel32Fits(cost+5u,costRelay)||
       !rel32Fits(vehicleCollector+5u,vehicleRelay)||
       !rel32Fits(rightSelector+5u,rightSelectorRelay)||
       !rel32Fits(rightGate+5u,rightGateRelay)||
       !rel32Fits(leftSelector+5u,leftSelectorRelay)||
       !rel32Fits(leftGate+5u,leftGateRelay)||
       !rel32Fits(sharedGate+5u,sharedGateRelay)||
       !rel32Fits(primaryInner+5u,primaryInnerRelay)||
       !rel32Fits(fallbackInner+5u,fallbackInnerRelay)||
       !rel32Fits(rightStart+5u,rightStartRelay)||
       !rel32Fits(rightEnd+5u,rightEndRelay)||
       !rel32Fits(leftStart+5u,leftStartRelay)||
       !rel32Fits(leftEnd+5u,leftEndRelay)||
       !rel32Fits(climbingObserver+5u,observerRelay)||!rel32Fits(catchRS+5u,catchRSRelay)||!rel32Fits(catchRStart+5u,catchRStartRelay)||!rel32Fits(catchREnd+5u,catchREndRelay)||!rel32Fits(catchLS+5u,catchLSRelay)||!rel32Fits(catchLStart+5u,catchLStartRelay)||!rel32Fits(catchLEnd+5u,catchLEndRelay)||!rel32Fits(localGate+5u,localGateRelay)||!rel32Fits(localFlag+5u,localFlagRelay)||!rel32Fits(deferredWriter+5u,deferredWriterRelay)||!rel32Fits(interactionReader+5u,interactionReaderRelay)||!rel32Fits(subcatCheck+5u,subcatCheckRelay))
        return false;
    g_originalGloveActionAvailable=(GloveActionAvailableFn)availableTrampoline;
    g_originalGloveActionCost=(GloveActionCostFn)costTrampoline;
    g_originalVehiclePickupCollector=(VehiclePickupCollectorFn)vehicleTrampoline;
    g_originalPickupRightSelector=(PickupGate1Fn)rightSelectorTrampoline;
    g_originalPickupRightGate=(PickupGate1Fn)rightGateTrampoline;
    g_originalPickupLeftSelector=(PickupGate1Fn)leftSelectorTrampoline;
    g_originalPickupLeftGate=(PickupGate1Fn)leftGateTrampoline;
    g_originalPickupSharedGate=(PickupSharedGateFn)sharedGateTrampoline;
    g_originalPickupPrimaryInner=(PickupInnerFn)primaryInnerTrampoline;
    g_originalPickupFallbackInner=(PickupInnerFn)fallbackInnerTrampoline;
    g_originalPickupRightStart=(PickupActionFn)rightStartTrampoline;
    g_originalPickupRightEnd=(PickupActionFn)rightEndTrampoline;
    g_originalPickupLeftStart=(PickupActionFn)leftStartTrampoline;
    g_originalPickupLeftEnd=(PickupActionFn)leftEndTrampoline;
    g_originalClimbingOnlyObserver=(PickupActionFn)observerTrampoline;
    g_originalCatchRightSelector=(PickupGate1Fn)catchRSTramp;g_originalCatchRightStart=(PickupActionFn)catchRStartTramp;g_originalCatchRightEnd=(PickupActionFn)catchREndTramp;
    g_originalCatchLeftSelector=(PickupGate1Fn)catchLSTramp;g_originalCatchLeftStart=(PickupActionFn)catchLStartTramp;g_originalCatchLeftEnd=(PickupActionFn)catchLEndTramp;
    g_originalLocalClimbingGate=(LocalClimbingGateFn)localGateTramp;
    g_originalLocalGloveFlagUpdate=(LocalGloveFlagUpdateFn)localFlagTramp;
    g_originalDeferredCandidateWriter=(DeferredCandidateWriterFn)deferredWriterTramp;
    g_originalInteractionReader=(InteractionReaderFn)interactionReaderTramp;
    g_originalEquippedSubcategoryCheck=(EquippedSubcategoryCheckFn)subcatCheckTramp;

    u8 availablePatch[5],costPatch[7],vehiclePatch[5];
    u8 rightSelectorPatch[9],rightGatePatch[6],leftSelectorPatch[9],leftGatePatch[6];
    u8 sharedGatePatch[5],primaryInnerPatch[5],fallbackInnerPatch[5];
    u8 rightStartPatch[7],rightEndPatch[6],leftStartPatch[5],leftEndPatch[6];
    u8 observerPatch[6];u8 catchRSPatch[5],catchRStartPatch[6],catchREndPatch[5],catchLSPatch[5],catchLStartPatch[6],catchLEndPatch[5];
    u8 localGatePatch[5],localFlagPatch[7];
    u8 deferredWriterPatch[5];
    u8 interactionReaderPatch[10];
    u8 subcatCheckPatch[6];
    memset(rightSelectorPatch,0x90,sizeof(rightSelectorPatch));
    memset(rightGatePatch,0x90,sizeof(rightGatePatch));
    memset(leftSelectorPatch,0x90,sizeof(leftSelectorPatch));
    memset(leftGatePatch,0x90,sizeof(leftGatePatch));

    buildRel32Jump(availablePatch,available,availableRelay);
    buildRel32Jump(costPatch,cost,costRelay);
    buildRel32Jump(vehiclePatch,vehicleCollector,vehicleRelay);
    buildRel32Jump(rightSelectorPatch,rightSelector,rightSelectorRelay);
    buildRel32Jump(rightGatePatch,rightGate,rightGateRelay);
    buildRel32Jump(leftSelectorPatch,leftSelector,leftSelectorRelay);
    buildRel32Jump(leftGatePatch,leftGate,leftGateRelay);
    buildRel32Jump(sharedGatePatch,sharedGate,sharedGateRelay);
    buildRel32Jump(primaryInnerPatch,primaryInner,primaryInnerRelay);
    buildRel32Jump(fallbackInnerPatch,fallbackInner,fallbackInnerRelay);
    memset(rightStartPatch,0x90,sizeof(rightStartPatch));
    memset(rightEndPatch,0x90,sizeof(rightEndPatch));
    memset(leftStartPatch,0x90,sizeof(leftStartPatch));
    memset(leftEndPatch,0x90,sizeof(leftEndPatch));
    memset(observerPatch,0x90,sizeof(observerPatch));
    buildRel32Jump(rightStartPatch,rightStart,rightStartRelay);
    buildRel32Jump(rightEndPatch,rightEnd,rightEndRelay);
    buildRel32Jump(leftStartPatch,leftStart,leftStartRelay);
    buildRel32Jump(leftEndPatch,leftEnd,leftEndRelay);
    buildRel32Jump(observerPatch,climbingObserver,observerRelay);
    buildRel32Jump(catchRSPatch,catchRS,catchRSRelay);memset(catchRStartPatch,0x90,sizeof(catchRStartPatch));buildRel32Jump(catchRStartPatch,catchRStart,catchRStartRelay);buildRel32Jump(catchREndPatch,catchREnd,catchREndRelay);
    buildRel32Jump(catchLSPatch,catchLS,catchLSRelay);memset(catchLStartPatch,0x90,sizeof(catchLStartPatch));buildRel32Jump(catchLStartPatch,catchLStart,catchLStartRelay);buildRel32Jump(catchLEndPatch,catchLEnd,catchLEndRelay);
    buildRel32Jump(localGatePatch,localGate,localGateRelay);memset(localFlagPatch,0x90,sizeof(localFlagPatch));buildRel32Jump(localFlagPatch,localFlag,localFlagRelay);
    buildRel32Jump(deferredWriterPatch,deferredWriter,deferredWriterRelay);
    memset(interactionReaderPatch,0x90,sizeof(interactionReaderPatch));buildRel32Jump(interactionReaderPatch,interactionReader,interactionReaderRelay);
    memset(subcatCheckPatch,0x90,sizeof(subcatCheckPatch));buildRel32Jump(subcatCheckPatch,subcatCheck,subcatCheckRelay);
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
    if(!writeCodeBytes(vehicleCollector,vehiclePatch,sizeof(vehiclePatch))||
       !writeCodeBytes(rightSelector,rightSelectorPatch,sizeof(rightSelectorPatch))||
       !writeCodeBytes(rightGate,rightGatePatch,sizeof(rightGatePatch))||
       !writeCodeBytes(leftSelector,leftSelectorPatch,sizeof(leftSelectorPatch))||
       !writeCodeBytes(leftGate,leftGatePatch,sizeof(leftGatePatch))||
       !writeCodeBytes(sharedGate,sharedGatePatch,sizeof(sharedGatePatch))||
       !writeCodeBytes(primaryInner,primaryInnerPatch,sizeof(primaryInnerPatch))||
       !writeCodeBytes(fallbackInner,fallbackInnerPatch,sizeof(fallbackInnerPatch))||
       !writeCodeBytes(rightStart,rightStartPatch,sizeof(rightStartPatch))||
       !writeCodeBytes(rightEnd,rightEndPatch,sizeof(rightEndPatch))||
       !writeCodeBytes(leftStart,leftStartPatch,sizeof(leftStartPatch))||
       !writeCodeBytes(leftEnd,leftEndPatch,sizeof(leftEndPatch))||
       !writeCodeBytes(climbingObserver,observerPatch,sizeof(observerPatch))||!writeCodeBytes(catchRS,catchRSPatch,sizeof(catchRSPatch))||!writeCodeBytes(catchRStart,catchRStartPatch,sizeof(catchRStartPatch))||!writeCodeBytes(catchREnd,catchREndPatch,sizeof(catchREndPatch))||!writeCodeBytes(catchLS,catchLSPatch,sizeof(catchLSPatch))||!writeCodeBytes(catchLStart,catchLStartPatch,sizeof(catchLStartPatch))||!writeCodeBytes(catchLEnd,catchLEndPatch,sizeof(catchLEndPatch))||!writeCodeBytes(localGate,localGatePatch,sizeof(localGatePatch))||!writeCodeBytes(localFlag,localFlagPatch,sizeof(localFlagPatch))||!writeCodeBytes(deferredWriter,deferredWriterPatch,sizeof(deferredWriterPatch))||!writeCodeBytes(interactionReader,interactionReaderPatch,sizeof(interactionReaderPatch))||!writeCodeBytes(subcatCheck,subcatCheckPatch,sizeof(subcatCheckPatch))){
        writeCodeBytes(cost,EXPECTED_COST_ENTRY,sizeof(EXPECTED_COST_ENTRY));
        writeCodeBytes(available,EXPECTED_AVAILABLE_ENTRY,sizeof(EXPECTED_AVAILABLE_ENTRY));
        writeCodeBytes(
            vehicleCollector,EXPECTED_VEHICLE_COLLECTOR_ENTRY,
            sizeof(EXPECTED_VEHICLE_COLLECTOR_ENTRY)
        );
        writeCodeBytes(
            rightSelector,EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY,
            sizeof(EXPECTED_PICKUP_RIGHT_SELECTOR_ENTRY)
        );
        writeCodeBytes(
            rightGate,EXPECTED_PICKUP_RIGHT_GATE_ENTRY,
            sizeof(EXPECTED_PICKUP_RIGHT_GATE_ENTRY)
        );
        writeCodeBytes(
            leftSelector,EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY,
            sizeof(EXPECTED_PICKUP_LEFT_SELECTOR_ENTRY)
        );
        writeCodeBytes(
            leftGate,EXPECTED_PICKUP_LEFT_GATE_ENTRY,
            sizeof(EXPECTED_PICKUP_LEFT_GATE_ENTRY)
        );
        writeCodeBytes(
            sharedGate,EXPECTED_PICKUP_SHARED_GATE_ENTRY,
            sizeof(EXPECTED_PICKUP_SHARED_GATE_ENTRY)
        );
        writeCodeBytes(
            primaryInner,EXPECTED_PICKUP_PRIMARY_INNER_ENTRY,
            sizeof(EXPECTED_PICKUP_PRIMARY_INNER_ENTRY)
        );
        writeCodeBytes(
            fallbackInner,EXPECTED_PICKUP_FALLBACK_INNER_ENTRY,
            sizeof(EXPECTED_PICKUP_FALLBACK_INNER_ENTRY)
        );
        writeCodeBytes(rightStart,EXPECTED_PICKUP_RIGHT_START_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_START_ENTRY));
        writeCodeBytes(rightEnd,EXPECTED_PICKUP_RIGHT_END_ENTRY,sizeof(EXPECTED_PICKUP_RIGHT_END_ENTRY));
        writeCodeBytes(leftStart,EXPECTED_PICKUP_LEFT_START_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_START_ENTRY));
        writeCodeBytes(leftEnd,EXPECTED_PICKUP_LEFT_END_ENTRY,sizeof(EXPECTED_PICKUP_LEFT_END_ENTRY));
        writeCodeBytes(climbingObserver,EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY,sizeof(EXPECTED_CLIMBING_ONLY_OBSERVER_ENTRY));
        writeCodeBytes(catchRS,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY));writeCodeBytes(catchRStart,EXPECTED_CATCH_START_ENTRY,sizeof(EXPECTED_CATCH_START_ENTRY));writeCodeBytes(catchREnd,EXPECTED_CATCH_END_ENTRY,sizeof(EXPECTED_CATCH_END_ENTRY));
        writeCodeBytes(catchLS,EXPECTED_CATCH_SELECTOR_ENTRY,sizeof(EXPECTED_CATCH_SELECTOR_ENTRY));writeCodeBytes(catchLStart,EXPECTED_CATCH_START_ENTRY,sizeof(EXPECTED_CATCH_START_ENTRY));writeCodeBytes(catchLEnd,EXPECTED_CATCH_END_ENTRY,sizeof(EXPECTED_CATCH_END_ENTRY));
        writeCodeBytes(localGate,EXPECTED_LOCAL_CLIMBING_GATE_ENTRY,sizeof(EXPECTED_LOCAL_CLIMBING_GATE_ENTRY));writeCodeBytes(localFlag,EXPECTED_LOCAL_GLOVE_FLAG_ENTRY,sizeof(EXPECTED_LOCAL_GLOVE_FLAG_ENTRY));
        writeCodeBytes(deferredWriter,EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY,sizeof(EXPECTED_DEFERRED_CANDIDATE_WRITER_ENTRY));
        writeCodeBytes(interactionReader,EXPECTED_INTERACTION_READER_ENTRY,sizeof(EXPECTED_INTERACTION_READER_ENTRY));
        writeCodeBytes(subcatCheck,EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY,sizeof(EXPECTED_EQUIPPED_SUBCATEGORY_CHECK_ENTRY));
        return false;
    }
    g_combatHooksInstalled=true;
    return true;
}

static bool validateCombatHookState(const GameImage* image){
    if(!g_combatHooksInstalled||!image)return false;
    u8* available=image->base+RVA_GLOVE_ACTION_AVAILABLE;
    u8* cost=image->base+RVA_GLOVE_ACTION_COST;
    u8* vehicleCollector=image->base+RVA_VEHICLE_PICKUP_COLLECTOR;
    return available[0]==0xE9u&&
           cost[0]==0xE9u&&cost[5]==0x90u&&cost[6]==0x90u&&
           vehicleCollector[0]==0xE9u&&
           image->base[RVA_PICKUP_RIGHT_SELECTOR]==0xE9u&&
           image->base[RVA_PICKUP_RIGHT_GATE]==0xE9u&&
           image->base[RVA_PICKUP_LEFT_SELECTOR]==0xE9u&&
           image->base[RVA_PICKUP_LEFT_GATE]==0xE9u&&
           image->base[RVA_PICKUP_SHARED_GATE]==0xE9u&&
           image->base[RVA_PICKUP_PRIMARY_INNER]==0xE9u&&
           image->base[RVA_PICKUP_FALLBACK_INNER]==0xE9u&&
           image->base[RVA_PICKUP_RIGHT_START]==0xE9u&&
           image->base[RVA_PICKUP_RIGHT_END]==0xE9u&&
           image->base[RVA_PICKUP_LEFT_START]==0xE9u&&
           image->base[RVA_PICKUP_LEFT_END]==0xE9u&&
           image->base[RVA_CLIMBING_ONLY_OBSERVER]==0xE9u&&image->base[RVA_CATCH_RIGHT_SELECTOR]==0xE9u&&image->base[RVA_CATCH_RIGHT_START]==0xE9u&&image->base[RVA_CATCH_RIGHT_END]==0xE9u&&image->base[RVA_CATCH_LEFT_SELECTOR]==0xE9u&&image->base[RVA_CATCH_LEFT_START]==0xE9u&&image->base[RVA_CATCH_LEFT_END]==0xE9u&&image->base[RVA_LOCAL_CLIMBING_GATE_STATUS]==0xE9u&&image->base[RVA_LOCAL_GLOVE_FLAG_UPDATE]==0xE9u&&image->base[RVA_DEFERRED_CANDIDATE_WRITER]==0xE9u&&image->base[RVA_INTERACTION_READER]==0xE9u&&image->base[RVA_EQUIPPED_SUBCATEGORY_CHECK]==0xE9u;
}
