#define DS2_TESTING 1
#include "../src/construction_max_level_on_build.cpp"

static u32 g_setSequence[64];
static u32 g_setSequenceCount=0;
static u32 g_initCount=0;
static u32 g_updateCount=0;
static u8 g_initObservedLevel=0;
static u8 g_lastUpdateFlags=0;
static bool g_reenterUpdateFromSetLevel=false;
static bool g_reenterMaterialSetLevelFromSetLevel=false;
static bool g_reenterFoundationRefreshFromUpdate=false;
static u8 object[0x500];
static u8 secondObject[0x500];
static u8 descriptor[0x400];
static u8 config[0x800];

static void __fastcall fakeSetLevel(void* object,u32 level){
    if(g_setSequenceCount<64u)g_setSequence[g_setSequenceCount++]=level;
    *(u8*)((u8*)object+OFF_OBJECT_LEVEL)=(u8)level;
    *(u8*)((u8*)object+OFF_OBJECT_LEVEL_DIRTY)=1u;
    if(g_reenterMaterialSetLevelFromSetLevel){
        g_reenterMaterialSetLevelFromSetLevel=false;
        hookedSetLevel(object,1u);
    }
    if(g_reenterUpdateFromSetLevel){
        g_reenterUpdateFromSetLevel=false;
        hookedConstructionUpdate(object,0.016f,1u);
    }
}
static void __fastcall fakeConstructionInit(void* object,void* descriptor){
    g_initCount++;
    g_initObservedLevel=*(u8*)((u8*)descriptor+OFF_DESCRIPTOR_LEVEL);
    *(u8*)((u8*)object+OFF_OBJECT_SUBTYPE)=
        *(u8*)((u8*)descriptor+OFF_DESCRIPTOR_SUBTYPE);
    *(u32*)((u8*)object+OFF_OBJECT_CONSTRUCTION_ID)=
        *(u32*)((u8*)descriptor+OFF_DESCRIPTOR_CONSTRUCTION_ID);
    *(u8*)((u8*)object+OFF_OBJECT_NET_TYPE)=
        *(u8*)((u8*)descriptor+OFF_DESCRIPTOR_NET_TYPE);
    *(u8*)((u8*)object+OFF_OBJECT_LEVEL)=g_initObservedLevel;
    *(u8*)((u8*)object+OFF_OBJECT_APPLIED_LEVEL)=g_initObservedLevel;
}
static void __fastcall fakeConstructionUpdate(void* object,float,u8 updateFlags){
    g_updateCount++;
    g_lastUpdateFlags=updateFlags;
    if(g_reenterFoundationRefreshFromUpdate){
        g_reenterFoundationRefreshFromUpdate=false;
        hookedSetLevel(object,1u);
    }
}
static u32 __fastcall fakeEffectiveMaxLevel(void* config){
    return *(u32*)((u8*)config+OFF_MAX_LEVEL);
}

static void resetHarness(){
    g_debugLog=true;
    memset(g_setSequence,0,sizeof(g_setSequence));
    g_setSequenceCount=0;
    g_initCount=0;
    g_updateCount=0;
    g_initObservedLevel=0;
    g_lastUpdateFlags=0;
    g_reenterUpdateFromSetLevel=false;
    g_reenterMaterialSetLevelFromSetLevel=false;
    g_reenterFoundationRefreshFromUpdate=false;
    memset(g_giftMarkers,0,sizeof(g_giftMarkers));
    memset(g_foundationDiagnostics,0,sizeof(g_foundationDiagnostics));
    clearAllowedConfigs();
    atomicStoreLong(&g_configBatchPublished,1);
    memset((void*)g_candidatesTracked,0,sizeof(g_candidatesTracked));
    memset((void*)g_levelStepsStarted,0,sizeof(g_levelStepsStarted));
    memset((void*)g_foundationStepsStarted,0,sizeof(g_foundationStepsStarted));
    memset((void*)g_promotions,0,sizeof(g_promotions));
    memset((void*)g_downgradesPrevented,0,sizeof(g_downgradesPrevented));
    memset((void*)g_promotionFailures,0,sizeof(g_promotionFailures));
    atomicStoreLong(&g_initHookObserved,0);
    atomicStoreLong(&g_updateHookObserved,0);
    g_testMarkerLockAttempts=0u;
    g_testReadableChecks=0u;
    g_testResolvedConfig=0;
    g_testSetLevel=&fakeSetLevel;
    g_testConstructionInit=&fakeConstructionInit;
    g_testConstructionUpdate=&fakeConstructionUpdate;
    g_testEffectiveMaxLevel=&fakeEffectiveMaxLevel;
    g_testHookClass=0;
    g_testMaterialCaller=true;
    g_gameBase=(u8*)1;
}
static void setMetadata(u8* config,u32 maxLevel,u32 completionLevel,u32 customizableLevel){
    *(u32*)(config+OFF_MAX_LEVEL)=maxLevel;
    *(u32*)(config+OFF_COMPLETION_LEVEL)=completionLevel;
    *(u32*)(config+OFF_CUSTOMIZABLE_LEVEL)=customizableLevel;
}
static void prepareDescriptor(
    u8* descriptor,u8 initialLevel,u8 netType,u32 constructionId
){
    memset(descriptor,0,0x400u);
    descriptor[OFF_DESCRIPTOR_TYPE]=0u;
    descriptor[OFF_DESCRIPTOR_SUBTYPE]=0x0Eu;
    *(u32*)(descriptor+OFF_DESCRIPTOR_CONSTRUCTION_ID)=constructionId;
    descriptor[OFF_DESCRIPTOR_NET_TYPE]=netType;
    descriptor[OFF_DESCRIPTOR_LEVEL]=initialLevel;
}
static void makeStable(void* object,u8 level){
    *(u32*)((u8*)object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT)=0u;
    *(u8*)((u8*)object+OFF_OBJECT_LIFECYCLE_STATE)=3u;
    *(u8*)((u8*)object+OFF_OBJECT_LEVEL)=level;
    *(u8*)((u8*)object+OFF_OBJECT_APPLIED_LEVEL)=level;
    *(u8*)((u8*)object+OFF_OBJECT_SECONDARY_UPDATE_BLOCKED)=0u;
    // Native Init/SetLevel set +0x3B7 to 1 and no proven acknowledgement path
    // clears it. Keeping it set verifies that it is not treated as pending.
    *(u8*)((u8*)object+0x3B7u)=1u;
    *(u8*)((u8*)object+OFF_OBJECT_MATERIAL_DIRTY)=0u;
    *(u8*)((u8*)object+OFF_OBJECT_BUILD_PENDING)=0u;
    *(u8*)((u8*)object+OFF_OBJECT_LEVEL_DIRTY)=0u;
    *(u8*)((u8*)object+OFF_OBJECT_BUILD_AUX_PENDING)=0u;
    *(u32*)((u8*)object+OFF_OBJECT_FOUNDATION_TIMER)=0u;
}
static void makeFoundationStable(void* object,u8 level){
    makeStable(object,level);
    *(u8*)((u8*)object+OFF_OBJECT_LIFECYCLE_STATE)=2u;
    *(u8*)((u8*)object+OFF_OBJECT_BUILD_PENDING)=1u;
    *(u32*)((u8*)object+OFF_OBJECT_FOUNDATION_TIMER)=FOUNDATION_DWELL_BITS;
}
static u32 markerPhase(void* object,u32 constructionId,u32 targetIndex){
    GiftMarker* marker=lockMatchingGiftMarker(object,constructionId,targetIndex);
    if(!marker)return GIFT_PHASE_NONE;
    u32 phase=marker->phase;
    unlockGiftMarker(marker);
    return phase;
}

extern "C" __declspec(dllexport) u32 RunSyntheticConstructionLevelTest(){
    memset(object,0,sizeof(object));
    memset(secondObject,0,sizeof(secondObject));
    memset(descriptor,0,sizeof(descriptor));
    memset(config,0,sizeof(config));

    // Init remains native at completion level 1 and only records a candidate.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,0x11223344u);
    hookedConstructionInit(object,descriptor);
    if(g_initCount!=1u||g_initObservedLevel!=1u||
       descriptor[OFF_DESCRIPTOR_LEVEL]!=1u||object[OFF_OBJECT_LEVEL]!=1u||
       object[OFF_OBJECT_APPLIED_LEVEL]!=1u||g_initHookObserved!=1)return 1u;
    if(markerPhase(object,0x11223344u,0u)!=GIFT_PHASE_CANDIDATE)return 2u;

    // Lifecycle state 1 is still inside the native build transaction: no step.
    object[OFF_OBJECT_LIFECYCLE_STATE]=1u;
    hookedConstructionUpdate(object,0.016f,0xA5u);
    if(g_updateCount!=1u||g_lastUpdateFlags!=0xA5u||g_updateHookObserved!=1||
       g_setSequenceCount!=0u)return 3u;

    // For an ordinary tuple the secondary native dispatch (flags=0) is
    // forwarded but cannot advance the gift phase.
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=0u||g_lastUpdateFlags!=0u||
       markerPhase(object,0x11223344u,0u)!=GIFT_PHASE_CANDIDATE)return 35u;

    // One primary stable update arms, the next uses the native material-path
    // shape to request effective Max in one acknowledged transition.
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=0u||
       markerPhase(object,0x11223344u,0u)!=GIFT_PHASE_ARMED)return 4u;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=3u||
       object[OFF_OBJECT_LEVEL]!=3u||object[OFF_OBJECT_APPLIED_LEVEL]!=1u||
       markerPhase(object,0x11223344u,0u)!=GIFT_PHASE_WAIT_ACK)return 5u;

    // No second transition before AppliedLevel acknowledges Max.
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u)return 6u;

    makeStable(object,3u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(markerPhase(object,0x11223344u,0u)!=GIFT_PHASE_COMPLETE||
       g_promotions[0]!=1||g_setSequenceCount!=1u)return 10u;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_promotions[0]!=1)return 11u;

    // Gifted max is protected only on the exact material caller.
    *(u32*)(object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT)=1u;
    object[OFF_OBJECT_BUILD_PENDING]=1u;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=1u||object[OFF_OBJECT_LEVEL]!=3u||
       g_downgradesPrevented[0]!=1)return 12u;
    g_testMaterialCaller=false;
    hookedSetLevel(object,2u);
    if(g_setSequenceCount!=2u||g_setSequence[1]!=2u||
       object[OFF_OBJECT_LEVEL]!=2u)return 13u;

    // Removal below Completion remains native and clears gift tracking.
    makeStable(object,3u);
    g_testMaterialCaller=true;
    hookedSetLevel(object,0u);
    if(g_setSequenceCount!=3u||g_setSequence[2]!=0u||
       markerPhase(object,0x11223344u,0u)!=GIFT_PHASE_NONE)return 14u;

    // Unmarked max structures are never shielded.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    object[OFF_OBJECT_NET_TYPE]=CONSTRUCTION_NET_TYPE_PLAYER;
    *(u32*)(object+OFF_OBJECT_CONSTRUCTION_ID)=7u;
    makeStable(object,3u);
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=1u||
       g_downgradesPrevented[0]!=0)return 16u;

    // Net/Stage descriptors initialize natively but are never tracked.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,2u,8u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_initCount!=1u||g_setSequenceCount!=0u||
       markerPhase(object,8u,0u)!=GIFT_PHASE_NONE)return 17u;

    // Unpublished config initializes natively without tracking.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,9u);
    hookedConstructionInit(object,descriptor);
    if(g_initCount!=1u||markerPhase(object,9u,0u)!=GIFT_PHASE_NONE)return 18u;

    // Metadata and class mismatches fail closed.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,9u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,10u);
    hookedConstructionInit(object,descriptor);
    if(markerPhase(object,10u,0u)!=GIFT_PHASE_NONE||g_initCount!=1u)return 19u;
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,11u);
    hookedConstructionInit(object,descriptor);
    if(markerPhase(object,11u,0u)!=GIFT_PHASE_NONE)return 20u;

    // SafeHouse/bridge foundations use only the exact secondary state-2
    // dispatch while still in state 2. Their native +0x3D1 pending byte starts
    // at 1, so the native +0x224 timer must reach 0.1 seconds before two
    // stable polls arm and invoke the class-specific Completion transition.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,4u,2u,3u);
    g_testResolvedConfig=config;
    g_allowedConfigs[1]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,12u);
    hookedConstructionInit(object,descriptor);
    makeFoundationStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=0u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 21u;
    if(g_foundationDiagnostics[1].used==0u)return 48u;
    *(u32*)(object+OFF_OBJECT_FOUNDATION_TIMER)=FOUNDATION_DWELL_BITS-1u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=0u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 47u;
    makeFoundationStable(object,1u);
    *(u32*)(object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT)=1u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 52u;
    *(u32*)(object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT)=0u;
    object[OFF_OBJECT_SECONDARY_UPDATE_BLOCKED]=1u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 53u;
    object[OFF_OBJECT_SECONDARY_UPDATE_BLOCKED]=0u;
    object[OFF_OBJECT_MATERIAL_DIRTY]=1u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 54u;
    object[OFF_OBJECT_MATERIAL_DIRTY]=0u;
    object[OFF_OBJECT_LEVEL_DIRTY]=1u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 55u;
    object[OFF_OBJECT_LEVEL_DIRTY]=0u;
    object[OFF_OBJECT_BUILD_AUX_PENDING]=1u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=0u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 56u;
    makeFoundationStable(object,1u);
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=0u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_ARMED)return 22u;
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=2u||
       object[OFF_OBJECT_BUILD_PENDING]!=1u||
       g_foundationStepsStarted[1]!=1||
       markerPhase(object,12u,1u)!=GIFT_PHASE_WAIT_ACK)return 23u;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=1u||object[OFF_OBJECT_LEVEL]!=2u||
       g_downgradesPrevented[1]!=1||
       markerPhase(object,12u,1u)!=GIFT_PHASE_WAIT_ACK)return 41u;

    // Repeated secondary updates and even a level ACK in state 2 cannot start
    // Max. The native lifecycle must first promote the object to Active.
    hookedConstructionUpdate(object,0.016f,0u);
    makeFoundationStable(object,2u);
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=1u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_WAIT_ACK)return 24u;
    makeStable(object,2u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(markerPhase(object,12u,1u)!=GIFT_PHASE_CANDIDATE)return 37u;
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=2u||g_setSequence[1]!=4u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_WAIT_ACK)return 38u;
    makeStable(object,4u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_promotions[1]!=1||markerPhase(object,12u,1u)!=GIFT_PHASE_COMPLETE)return 39u;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=2u||object[OFF_OBJECT_LEVEL]!=4u||
       g_downgradesPrevented[1]!=2)return 42u;
    hookedSetLevel(object,0u);
    if(g_setSequenceCount!=3u||g_setSequence[2]!=0u||
       markerPhase(object,12u,1u)!=GIFT_PHASE_NONE)return 43u;

    // Native foundation Update may legitimately refresh SetLevel(1) while the
    // object is already at 1. That is not a downgrade or teardown and must not
    // erase the CANDIDATE marker before the post-Update foundation dwell.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,4u,2u,3u);
    g_testResolvedConfig=config;
    g_allowedConfigs[1]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,124u);
    hookedConstructionInit(object,descriptor);
    makeFoundationStable(object,1u);
    g_reenterFoundationRefreshFromUpdate=true;
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=1u||
       markerPhase(object,124u,1u)!=GIFT_PHASE_CANDIDATE)return 58u;
    bool sawEntryDiagnostic=false;
    bool sawPostDiagnostic=false;
    for(u32 i=0;i<g_foundationDiagnostics[1].used;i++){
        const FoundationDiagnosticSample* sample=&g_foundationDiagnostics[1].samples[i];
        if(sample->constructionId!=124u)continue;
        if(sample->samplePoint==0u)sawEntryDiagnostic=true;
        if(sample->samplePoint==1u)sawPostDiagnostic=true;
    }
    if(!sawEntryDiagnostic||!sawPostDiagnostic)return 60u;
    makeFoundationStable(object,1u);
    hookedConstructionUpdate(object,0.016f,0u);
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=2u||g_setSequence[1]!=2u||
       markerPhase(object,124u,1u)!=GIFT_PHASE_WAIT_ACK)return 59u;

    // A state-2 material reconciliation may also lower a transient internal 2
    // back to foundation level 1. It stays native, but it must not be mistaken
    // for teardown and erase the still-CANDIDATE foundation marker.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,4u,2u,3u);
    g_testResolvedConfig=config;
    g_allowedConfigs[1]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,125u);
    hookedConstructionInit(object,descriptor);
    makeFoundationStable(object,2u);
    g_testMaterialCaller=true;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=1u||
       markerPhase(object,125u,1u)!=GIFT_PHASE_CANDIDATE)return 61u;
    makeFoundationStable(object,1u);
    hookedConstructionUpdate(object,0.016f,0u);
    hookedConstructionUpdate(object,0.016f,0u);
    if(g_setSequenceCount!=2u||g_setSequence[1]!=2u||
       markerPhase(object,125u,1u)!=GIFT_PHASE_WAIT_ACK)return 62u;

    // All three normal bridge target indices share the same proven
    // class-specific foundation path and accept pending=1 after 0.1 seconds.
    for(u32 bridgeTarget=7u;bridgeTarget<=9u;bridgeTarget++){
        resetHarness();
        memset(object,0,sizeof(object));
        setMetadata(config,4u,2u,3u);
        g_testResolvedConfig=config;
        g_allowedConfigs[bridgeTarget]=(u64)config;
        g_testHookClass=7;
        u32 constructionId=200u+bridgeTarget;
        prepareDescriptor(
            descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,constructionId
        );
        hookedConstructionInit(object,descriptor);
        makeFoundationStable(object,1u);
        hookedConstructionUpdate(object,0.016f,0u);
        hookedConstructionUpdate(object,0.016f,0u);
        if(g_setSequenceCount!=1u||g_setSequence[0]!=2u||
           object[OFF_OBJECT_BUILD_PENDING]!=1u||
           g_foundationStepsStarted[bridgeTarget]!=1||
           markerPhase(object,constructionId,bridgeTarget)!=GIFT_PHASE_WAIT_ACK){
            return 49u+bridgeTarget-7u;
        }
    }

    // A native state-2 -> state-3 change between the two stable polls does not
    // lose the marker; the matching primary dispatch completes the same step.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,4u,2u,3u);
    g_testResolvedConfig=config;
    g_allowedConfigs[1]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,120u);
    hookedConstructionInit(object,descriptor);
    makeFoundationStable(object,1u);
    hookedConstructionUpdate(object,0.016f,0u);
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=2u||
       markerPhase(object,120u,1u)!=GIFT_PHASE_WAIT_ACK)return 44u;

    // A foundation first observed in Active state has the same two-poll path.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,4u,2u,3u);
    g_testResolvedConfig=config;
    g_allowedConfigs[1]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,121u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=2u||
       markerPhase(object,121u,1u)!=GIFT_PHASE_WAIT_ACK)return 45u;

    // Reloading a donated SafeHouse/bridge level keeps the narrow foundation
    // material shield even though its original material ledger may still say 1.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,4u,2u,3u);
    g_testResolvedConfig=config;
    g_allowedConfigs[1]=(u64)config;
    g_testHookClass=1;
    prepareDescriptor(descriptor,4u,CONSTRUCTION_NET_TYPE_PLAYER,122u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,4u);
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=0u||object[OFF_OBJECT_LEVEL]!=4u||
       g_downgradesPrevented[1]!=1||
       markerPhase(object,122u,1u)!=GIFT_PHASE_COMPLETE)return 46u;

    // Every proven pending flag blocks arming and therefore blocks SetLevel.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,13u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    *(u32*)(object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT)=1u;
    hookedConstructionUpdate(object,0.016f,1u);
    *(u32*)(object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT)=0u;
    object[OFF_OBJECT_MATERIAL_DIRTY]=1u;
    hookedConstructionUpdate(object,0.016f,1u);
    object[OFF_OBJECT_MATERIAL_DIRTY]=0u;
    object[OFF_OBJECT_BUILD_PENDING]=1u;
    hookedConstructionUpdate(object,0.016f,1u);
    object[OFF_OBJECT_BUILD_PENDING]=0u;
    object[OFF_OBJECT_LEVEL_DIRTY]=1u;
    hookedConstructionUpdate(object,0.016f,1u);
    object[OFF_OBJECT_LEVEL_DIRTY]=0u;
    object[OFF_OBJECT_BUILD_AUX_PENDING]=1u;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=0u||markerPhase(object,13u,0u)!=GIFT_PHASE_CANDIDATE)return 25u;

    // Reinitializing a reused pointer clears its old marker.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,14u);
    hookedConstructionInit(object,descriptor);
    if(markerPhase(object,14u,0u)!=GIFT_PHASE_CANDIDATE)return 26u;
    prepareDescriptor(descriptor,4u,CONSTRUCTION_NET_TYPE_PLAYER,15u);
    hookedConstructionInit(object,descriptor);
    if(markerPhase(object,14u,0u)!=GIFT_PHASE_NONE||
       markerPhase(object,15u,0u)!=GIFT_PHASE_NONE||g_initCount!=2u)return 27u;

    // Independent objects maintain independent acknowledged transitions.
    resetHarness();
    memset(object,0,sizeof(object));
    memset(secondObject,0,sizeof(secondObject));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,16u);
    hookedConstructionInit(object,descriptor);
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,17u);
    hookedConstructionInit(secondObject,descriptor);
    makeStable(object,1u);
    makeStable(secondObject,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(secondObject,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(secondObject,0.016f,1u);
    if(g_setSequenceCount!=2u||g_setSequence[0]!=3u||g_setSequence[1]!=3u||
       markerPhase(object,16u,0u)!=GIFT_PHASE_WAIT_ACK||
       markerPhase(secondObject,17u,0u)!=GIFT_PHASE_WAIT_ACK)return 28u;

    // A material downgrade during WAIT_ACK remains native. The following
    // update notices the divergence and abandons the gift instead of forcing
    // or overlapping another transition.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,18u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=3u||
       markerPhase(object,18u,0u)!=GIFT_PHASE_WAIT_ACK)return 29u;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=2u||g_setSequence[1]!=1u||
       g_downgradesPrevented[0]!=0)return 30u;
    hookedConstructionUpdate(object,0.016f,1u);
    if(markerPhase(object,18u,0u)!=GIFT_PHASE_NONE||
       g_promotionFailures[0]!=1)return 31u;

    // A saved canonical Player object that reinitializes at donated Max is
    // restored as COMPLETE and regains only the narrow stable material shield.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,3u,CONSTRUCTION_NET_TYPE_PLAYER,19u);
    hookedConstructionInit(object,descriptor);
    if(markerPhase(object,19u,0u)!=GIFT_PHASE_COMPLETE)return 32u;
    makeStable(object,3u);
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=0u||g_downgradesPrevented[0]!=1||
       object[OFF_OBJECT_LEVEL]!=3u)return 33u;

    // A saved intermediate level resumes through the same acknowledged path.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,2u,CONSTRUCTION_NET_TYPE_PLAYER,20u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,2u);
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=3u||
       markerPhase(object,20u,0u)!=GIFT_PHASE_WAIT_ACK)return 34u;

    // A synchronous Update callback from inside native SetLevel cannot advance
    // the marker while its one native transition is still on the stack.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,21u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    g_reenterUpdateFromSetLevel=true;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=3u||
       markerPhase(object,21u,0u)!=GIFT_PHASE_WAIT_ACK)return 36u;

    // A material SetLevel callback nested inside the original promotion sees
    // the lock-free CALL_IN_PROGRESS reservation and cannot roll Max back to 1.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,22u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    g_reenterMaterialSetLevelFromSetLevel=true;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=3u||
       object[OFF_OBJECT_LEVEL]!=3u||g_downgradesPrevented[0]!=1||
       markerPhase(object,22u,0u)!=GIFT_PHASE_WAIT_ACK)return 57u;

    // The global Update hook must remain effectively free for unrelated
    // constructions, even while a different object has an active marker.
    // This is the steady-state path whose old eight-slot locking scan scaled
    // with every loaded construction in mature saves.
    resetHarness();
    memset(object,0,sizeof(object));
    memset(secondObject,0,sizeof(secondObject));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,23u);
    hookedConstructionInit(secondObject,descriptor);
    if(markerPhase(secondObject,23u,0u)!=GIFT_PHASE_CANDIDATE)return 63u;
    g_debugLog=false;
    u32 locksBefore=g_testMarkerLockAttempts;
    u32 readsBefore=g_testReadableChecks;
    static const u32 HOT_UPDATE_ITERATIONS=4096u;
    for(u32 i=0;i<HOT_UPDATE_ITERATIONS;i++){
        hookedConstructionUpdate(object,0.016f,1u);
    }
    if(g_updateCount!=HOT_UPDATE_ITERATIONS||
       g_testMarkerLockAttempts!=locksBefore||
       g_testReadableChecks!=readsBefore)return 64u;
    if(markerPhase(secondObject,23u,0u)!=GIFT_PHASE_CANDIDATE)return 65u;

    // A max-level construction restored from a save keeps its COMPLETE marker
    // for the rare SetLevel material guard, but its per-frame Update path must
    // reject that marker before locking it or validating runtime memory.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,3u,CONSTRUCTION_NET_TYPE_PLAYER,24u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,3u);
    if(markerPhase(object,24u,0u)!=GIFT_PHASE_COMPLETE)return 66u;
    g_debugLog=false;
    locksBefore=g_testMarkerLockAttempts;
    readsBefore=g_testReadableChecks;
    for(u32 i=0;i<HOT_UPDATE_ITERATIONS;i++){
        hookedConstructionUpdate(object,0.016f,1u);
    }
    if(g_updateCount!=HOT_UPDATE_ITERATIONS||
       g_testMarkerLockAttempts!=locksBefore||
       g_testReadableChecks!=readsBefore)return 67u;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=0u||object[OFF_OBJECT_LEVEL]!=3u||
       g_downgradesPrevented[0]!=1||
       markerPhase(object,24u,0u)!=GIFT_PHASE_COMPLETE)return 68u;

    // The active state machine still takes the validated slow path and reaches
    // COMPLETE exactly once. Once complete, subsequent Updates use the same
    // zero-lock/zero-readable-check fast path and the downgrade guard remains.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,25u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,1u);
    g_debugLog=false;
    locksBefore=g_testMarkerLockAttempts;
    readsBefore=g_testReadableChecks;
    hookedConstructionUpdate(object,0.016f,1u);
    hookedConstructionUpdate(object,0.016f,1u);
    makeStable(object,3u);
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_updateCount!=3u||g_setSequenceCount!=1u||g_setSequence[0]!=3u||
       g_promotions[0]!=1||
       g_testMarkerLockAttempts<=locksBefore||
       g_testReadableChecks<=readsBefore)return 69u;
    if(markerPhase(object,25u,0u)!=GIFT_PHASE_COMPLETE)return 72u;
    locksBefore=g_testMarkerLockAttempts;
    readsBefore=g_testReadableChecks;
    for(u32 i=0;i<HOT_UPDATE_ITERATIONS;i++){
        hookedConstructionUpdate(object,0.016f,1u);
    }
    if(g_updateCount!=HOT_UPDATE_ITERATIONS+3u||g_promotions[0]!=1||
       g_setSequenceCount!=1u||
       g_testMarkerLockAttempts!=locksBefore||
       g_testReadableChecks!=readsBefore)return 70u;
    hookedSetLevel(object,1u);
    if(g_setSequenceCount!=1u||object[OFF_OBJECT_LEVEL]!=3u||
       g_downgradesPrevented[0]!=1||
       markerPhase(object,25u,0u)!=GIFT_PHASE_COMPLETE)return 71u;

    // COMPLETE is a fast return only while the live level still matches the
    // marker's expected Max. A direct external divergence must take the slow
    // path once and discard the stale downgrade-protection marker.
    resetHarness();
    memset(object,0,sizeof(object));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,3u,CONSTRUCTION_NET_TYPE_PLAYER,26u);
    hookedConstructionInit(object,descriptor);
    makeStable(object,3u);
    if(markerPhase(object,26u,0u)!=GIFT_PHASE_COMPLETE)return 73u;
    makeStable(object,2u);
    g_debugLog=false;
    locksBefore=g_testMarkerLockAttempts;
    readsBefore=g_testReadableChecks;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_updateCount!=1u||g_testMarkerLockAttempts<=locksBefore||
       g_testReadableChecks<=readsBefore)return 74u;
    if(markerPhase(object,26u,0u)!=GIFT_PHASE_NONE)return 75u;
    locksBefore=g_testMarkerLockAttempts;
    readsBefore=g_testReadableChecks;
    hookedConstructionUpdate(object,0.016f,1u);
    if(g_updateCount!=2u||g_testMarkerLockAttempts!=locksBefore||
       g_testReadableChecks!=readsBefore)return 76u;

    // SetLevel for an unmarked object is an equally important global hook hot
    // path. An unrelated active marker must not trigger a lock or metadata
    // validation, while the original SetLevel call remains fully native.
    resetHarness();
    memset(object,0,sizeof(object));
    memset(secondObject,0,sizeof(secondObject));
    setMetadata(config,3u,1u,2u);
    g_testResolvedConfig=config;
    g_allowedConfigs[0]=(u64)config;
    prepareDescriptor(descriptor,1u,CONSTRUCTION_NET_TYPE_PLAYER,27u);
    hookedConstructionInit(secondObject,descriptor);
    if(markerPhase(secondObject,27u,0u)!=GIFT_PHASE_CANDIDATE)return 77u;
    locksBefore=g_testMarkerLockAttempts;
    readsBefore=g_testReadableChecks;
    hookedSetLevel(object,2u);
    if(g_setSequenceCount!=1u||g_setSequence[0]!=2u||
       object[OFF_OBJECT_LEVEL]!=2u||
       g_testMarkerLockAttempts!=locksBefore||
       g_testReadableChecks!=readsBefore)return 78u;
    if(markerPhase(secondObject,27u,0u)!=GIFT_PHASE_CANDIDATE)return 79u;

    return 0u;
}
