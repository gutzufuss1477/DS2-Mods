#define ODRADEK_TEST_BUILD 1
#include "../src/odradek_scan.cpp"

static void writePointer(u8* base,u64 rva,u64 targetRva){
    *(u64*)(base+rva)=(u64)(base+targetRva);
}

static void writeVtable(u8* base,u64 tableRva,const u64* functions){
    for(u32 i=0;i<4u;i++)writePointer(base,tableRva+(u64)i*8u,functions[i]);
}

static u8 syntheticSensorGate(u8*,const double* target,float optionalRadius){
    return target&&target[0]>=0.0&&optionalRadius>=0.0f?1u:0u;
}

static volatile long g_syntheticDetectedCalls=0;
static u8* g_syntheticFreshTarget=0;
static u8* g_syntheticBaggage=0;
static u8* g_syntheticBaggageEntityComponents=0;
static volatile long g_syntheticFoundCalls=0;
static volatile long g_syntheticMarkerCalls=0;
static volatile long g_syntheticFoundFresh=0;
static volatile long g_syntheticMarkerFresh=0;
static volatile long g_syntheticFoundDistanceBits=0;
static volatile long g_syntheticFoundBreakLinks=0;
static volatile long g_syntheticEntityUpdateCalls=0;
static volatile long g_syntheticRealizedInitCalls=0;
static volatile long g_syntheticRealizedUnlinkCalls=0;
static volatile long g_syntheticRealizedUpdateCalls=0;
static u8* g_syntheticEntityManager=0;
static u8* g_syntheticLifecycleBaggage=0;
static u8* g_syntheticBridgeDetected=0;
static volatile long g_syntheticPendingCountAtUnlink=-1;
static volatile long g_syntheticRadiusAfterUpdateBits=0;
static u8* g_syntheticSymbols=0;
static u8* g_syntheticSymbolRecords=0;
static u8* g_syntheticSymbolTemplate=0;
static volatile long g_syntheticSymbolEventCalls=0;
static volatile long g_syntheticSymbolEventEnabled=0;
static volatile long g_syntheticRequestRealizeCalls=0;
static volatile long g_syntheticRequestRealizeDl=0;
static volatile long g_syntheticRequestRealizeR8=0;
static u8* g_syntheticRequestRealizeOwner=0;
static volatile long g_syntheticCoordinatorCalls=0;
static u8* g_syntheticCoordinatorOwner=0;

static void syntheticDetectedUpdate(u8* component,void*){
    _InterlockedIncrement(&g_syntheticDetectedCalls);
    if(component){
        component[0x56u]=1u;
        if(component==g_syntheticFreshTarget){
            component[OFF_DETECTED_FRESH_HIT]=1u;
        }
    }
}

static u8* syntheticFindBaggage(u8* entityComponents,const void*){
    return entityComponents==g_syntheticBaggageEntityComponents?
           g_syntheticBaggage:0;
}

static u8* syntheticFindSymbols(u8* entityComponents,const void* descriptor){
    if(entityComponents!=g_syntheticBaggageEntityComponents||
       descriptor!=(const void*)(g_gameBase+RVA_BAGGAGE_SYMBOLS_RTTI))return 0;
    return g_syntheticSymbols;
}

static void syntheticBaggageSymbolEvent(u8* symbols,u8* message){
    _InterlockedIncrement(&g_syntheticSymbolEventCalls);
    if(!symbols||symbols!=g_syntheticSymbols||!message||
       *(u32*)(message+0x10u)!=BAGGAGE_SYMBOL_EVENT_ID||
       message[0x98u]!=BAGGAGE_SYMBOL_EVENT_CATEGORY||
       _InterlockedCompareExchange(&g_syntheticSymbolEventEnabled,0,0)!=1){
        return;
    }
    message[8u]=1u;
    memset(g_syntheticSymbolRecords,0,BAGGAGE_SYMBOL_RECORD_SIZE);
    g_syntheticSymbolRecords[0]=BAGGAGE_SYMBOL_EVENT_CATEGORY;
    *(u64*)(g_syntheticSymbolRecords+OFF_BAGGAGE_SYMBOL_RECORD_TEMPLATE)=
        (u64)g_syntheticSymbolTemplate;
    *(u32*)(g_syntheticSymbolRecords+OFF_BAGGAGE_SYMBOL_RECORD_REMAINING)=
        *(u32*)(g_syntheticSymbolTemplate+OFF_BAGGAGE_SYMBOL_TEMPLATE_DURATION);
    *(u32*)(symbols+OFF_BAGGAGE_SYMBOLS_RECORD_COUNT)=1u;
    *(u32*)(symbols+OFF_BAGGAGE_SYMBOLS_RECORD_CAPACITY)=1u;
    *(u64*)(symbols+OFF_BAGGAGE_SYMBOLS_RECORDS)=
        (u64)g_syntheticSymbolRecords;
}

static void syntheticRequestEntityRealize(u8* owner,u8 dl,u8 r8){
    _InterlockedIncrement(&g_syntheticRequestRealizeCalls);
    _InterlockedExchange(&g_syntheticRequestRealizeDl,(long)dl);
    _InterlockedExchange(&g_syntheticRequestRealizeR8,(long)r8);
    g_syntheticRequestRealizeOwner=owner;
    if(!owner)return;
    u64* flags=(u64*)(owner+0x98u);
    if((*flags&0x0C00000000ull)!=0ull){
        *flags&=0xFFFFFFF3FFFFFFFFull;
        owner[0x30Fu]=0u;
    }else if((*flags&0x200ull)!=0ull&&(*flags&0x3000000000ull)==0ull){
        *flags|=0x1000000000ull;
        if(r8)owner[0x30Fu]|=4u;
        if(dl)owner[0x30Fu]|=2u;
    }
}

static void syntheticRealizationCoordinator(void*){
    _InterlockedIncrement(&g_syntheticCoordinatorCalls);
    if(!g_syntheticCoordinatorOwner)return;
    // Model the native distance coordinator scheduling this entity for
    // unrealization before the v0.1.10 relay reasserts its exact lease.
    *(u64*)(g_syntheticCoordinatorOwner+0x98u)|=0x0400000000ull;
    g_syntheticCoordinatorOwner[0x30Fu]|=2u;
}

static void syntheticFoundBridge(u8* realized){
    _InterlockedIncrement(&g_syntheticFoundCalls);
    if(g_syntheticBridgeDetected){
        _InterlockedExchange(
            &g_syntheticFoundFresh,
            (long)g_syntheticBridgeDetected[OFF_DETECTED_FRESH_HIT]
        );
    }
    if(realized){
        _InterlockedExchange(
            &g_syntheticFoundDistanceBits,
            (long)*(u32*)(realized+OFF_REALIZED_DISTANCE_SQUARED)
        );
        if(_InterlockedCompareExchange(&g_syntheticFoundBreakLinks,0,0)==1&&
           g_syntheticLifecycleBaggage){
            *(u64*)(g_syntheticLifecycleBaggage+
                OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
        }
    }
}

static void syntheticMarkerBridge(u8*){
    _InterlockedIncrement(&g_syntheticMarkerCalls);
    if(g_syntheticBridgeDetected){
        _InterlockedExchange(
            &g_syntheticMarkerFresh,
            (long)g_syntheticBridgeDetected[OFF_DETECTED_FRESH_HIT]
        );
    }
}

static void syntheticEntityManagerGameUpdate(void*,void*){
    _InterlockedIncrement(&g_syntheticEntityUpdateCalls);
    long radiusBits=_InterlockedCompareExchange(
        &g_syntheticRadiusAfterUpdateBits,0,0
    );
    if(g_syntheticEntityManager&&radiusBits){
        *(u32*)(g_syntheticEntityManager+OFF_MANAGER_GATE_RADIUS)=
            (u32)radiusBits;
    }
}

static void syntheticRealizedBaggageInit(u8* realized,void*){
    _InterlockedIncrement(&g_syntheticRealizedInitCalls);
    if(realized&&g_syntheticLifecycleBaggage){
        *(u64*)(realized+OFF_REALIZED_BAGGAGE_COMPONENT)=
            (u64)g_syntheticLifecycleBaggage;
        *(u64*)(g_syntheticLifecycleBaggage+OFF_BAGGAGE_REALIZED_COMPONENT)=
            (u64)realized;
    }
}

static void syntheticRealizedBaggageUnlink(u8* realized,void*){
    _InterlockedIncrement(&g_syntheticRealizedUnlinkCalls);
    _InterlockedExchange(
        &g_syntheticPendingCountAtUnlink,(long)g_pendingCargoCount
    );
    if(realized&&g_syntheticLifecycleBaggage){
        *(u64*)(g_syntheticLifecycleBaggage+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
        *(u64*)(realized+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    }
}

[[maybe_unused]] static void syntheticRealizedBaggageUpdate(u8*,void*){
    _InterlockedIncrement(&g_syntheticRealizedUpdateCalls);
}

static void writeAbsoluteJump(u8* destination,const void* target){
    destination[0]=0x48u;
    destination[1]=0xB8u;
    *(u64*)(destination+2u)=(u64)target;
    destination[10]=0xFFu;
    destination[11]=0xE0u;
}

#include "scanned_cargo_hud_test.inl"
#include "scanned_cargo_distance_test.inl"
#include "scanned_cargo_sensor_test.inl"
#include "survey_alignment_test.inl"
#include "cargo_meter_label_test.inl"
#include "visual_wave_test.inl"
#include "cargo_recipient_test.inl"

extern "C" __declspec(dllexport) u32 RunPackagedSettingsTest(){
    Settings settings;
    SettingsResult result=loadSettings(&settings);
    closeLog();
    if(result!=SETTINGS_READY)return 1u;
    if(settings.rangeBits!=floatBits(1000.0f)||settings.uiRangeBits!=floatBits(500.0f)||
       settings.rangeMeters!=500.0f)return 2u;
    if(!settings.fullCircle||settings.shapeBits!=0u||
       settings.angleBits!=NATIVE_SURVEY_ANGLE_BITS)return 3u;
    if(!settings.extendCargoMarkers||g_debugLog||g_visualWaveScale!=1.0f)return 4u;
    // A stale installed INI must never reactivate the rejected lifecycle path.
    settings.extendCargoMarkers=true;
    if(!disableRejectedCargoLifecycle(&settings)||settings.extendCargoMarkers||
       settings.rangeBits!=floatBits(1000.0f)||!settings.fullCircle)return 6u;
    if(disableRejectedCargoLifecycle(&settings)||settings.extendCargoMarkers)return 7u;
    // A stalled ACTIVE scan has unchanged counters for many worker ticks.
    // It must remain observable until native completion.
    for(u32 ticks=0;ticks<120u;ticks++){
        if(gateTelemetryMayFinish(true,1u,ticks)||
           gateTelemetryMayFinish(false,0u,ticks))return 8u;
    }
    if(gateTelemetryMayFinish(true,0u,1u)||
       !gateTelemetryMayFinish(true,0u,2u)||
       !gateTelemetryMayFinish(true,0x100u,2u))return 9u;
    float visualParsed=0.0f;
    if(!parseDecimal(L"0.25",0.25f,1.0f,&visualParsed)||visualParsed!=0.25f||
       !parseDecimal(L"1",0.25f,1.0f,&visualParsed)||visualParsed!=1.0f||
       parseDecimal(L"0.249",0.25f,1.0f,&visualParsed)||
       parseDecimal(L"1.001",0.25f,1.0f,&visualParsed)||
       parseDecimal(L"nan",0.25f,1.0f,&visualParsed))return 10u;
    float parsed=0.0f;
    if(!parseDecimal(L"50",MIN_RANGE_METERS,MAX_RANGE_METERS,&parsed)||parsed!=50.0f||
       !parseDecimal(L"500.000",MIN_RANGE_METERS,MAX_RANGE_METERS,&parsed)||parsed!=500.0f||
       parseDecimal(L"49.999",MIN_RANGE_METERS,MAX_RANGE_METERS,&parsed)||
       !parseDecimal(L"1000",MIN_RANGE_METERS,MAX_RANGE_METERS,&parsed)||parsed!=1000.0f||
       parseDecimal(L"1000.001",MIN_RANGE_METERS,MAX_RANGE_METERS,&parsed))return 5u;
    return 0u;
}

extern "C" __declspec(dllexport) u32 RunSyntheticOdradekTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 1u;
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};

    writeVtable(base,RVA_SURVEY_VTABLE,SURVEY_VFUNCS);
    writeVtable(base,RVA_BODY_SECONDARY_VTABLE,BODY_SECONDARY_VFUNCS);
    writeVtable(base,RVA_SENSOR_VTABLE,SENSOR_VFUNCS);
    writeVtable(base,RVA_SENSOR_MANAGER_VTABLE,SENSOR_MANAGER_VFUNCS);
    writeVtable(base,RVA_EFFECT_INSTANCE_VTABLE,EFFECT_INSTANCE_VFUNCS);
    writeVtable(base,RVA_EFFECT_RESOURCE_VTABLE,EFFECT_RESOURCE_VFUNCS);
    writeVtable(base,RVA_UI_RESIDENT_VTABLE,UI_RESIDENT_VFUNCS);
    writeVtable(base,RVA_UI_GAME_PARAMS_VTABLE,UI_GAME_PARAMS_VFUNCS);
    writeVtable(base,RVA_BAGGAGE_MANAGER_VTABLE,BAGGAGE_MANAGER_VFUNCS);
    writeVtable(
        base,RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE,BAGGAGE_MANAGER_SECONDARY_VFUNCS
    );
    writeVtable(base,RVA_BAGGAGE_CONFIG_VTABLE,BAGGAGE_CONFIG_VFUNCS);
    writeVtable(
        base,RVA_PLAYER_ODRADEK_CONFIG_VTABLE,PLAYER_ODRADEK_CONFIG_VFUNCS
    );
    writeVtable(base,RVA_DETECTED_COMPONENT_VTABLE,DETECTED_COMPONENT_VFUNCS);
    writeVtable(base,RVA_BAGGAGE_COMPONENT_VTABLE,BAGGAGE_COMPONENT_VFUNCS);
    writeVtable(base,RVA_BAGGAGE_SYMBOLS_VTABLE,BAGGAGE_SYMBOLS_VFUNCS);
    writeVtable(
        base,RVA_BAGGAGE_SYMBOLS_SECONDARY_VTABLE,
        BAGGAGE_SYMBOLS_SECONDARY_VFUNCS
    );
    writeVtable(base,RVA_REALIZED_BAGGAGE_VTABLE,REALIZED_BAGGAGE_VFUNCS);
    writeVtable(base,RVA_ENTITY_MANAGER_GAME_VTABLE,ENTITY_MANAGER_GAME_VFUNCS);
    writePointer(
        base,RVA_ENTITY_MANAGER_GAME_VTABLE-8u,RVA_ENTITY_MANAGER_GAME_COL
    );
    *(u32*)(base+RVA_ENTITY_MANAGER_GAME_COL+0x0Cu)=
        (u32)RVA_ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR;
    memcpy(
        base+RVA_ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR+0x10u,
        EXPECTED_ENTITY_MANAGER_GAME_TYPE_NAME,
        sizeof(EXPECTED_ENTITY_MANAGER_GAME_TYPE_NAME)
    );
    memcpy(base+RVA_SURVEY_FACTORY,EXPECTED_SURVEY_FACTORY,16u);
    memcpy(
        base+RVA_SURVEY_CONSTRUCTOR,EXPECTED_SURVEY_CONSTRUCTOR,
        sizeof(EXPECTED_SURVEY_CONSTRUCTOR)
    );
    memcpy(
        base+RVA_GAMEPLAY_SCAN_START,EXPECTED_GAMEPLAY_SCAN_START,
        sizeof(EXPECTED_GAMEPLAY_SCAN_START)
    );
    memcpy(
        base+RVA_SENSOR_GATE_RADIUS_LOAD,EXPECTED_SENSOR_GATE_RADIUS_LOAD,
        sizeof(EXPECTED_SENSOR_GATE_RADIUS_LOAD)
    );
    memcpy(
        base+RVA_SENSOR_GATE_CALL_WINDOW,EXPECTED_SENSOR_GATE_CALL_WINDOW,
        sizeof(EXPECTED_SENSOR_GATE_CALL_WINDOW)
    );
    memcpy(
        base+RVA_UI_CACHE_LOAD,EXPECTED_UI_CACHE_LOAD,
        sizeof(EXPECTED_UI_CACHE_LOAD)
    );
    memcpy(
        base+RVA_UI_SEARCHED_DISTANCE_LOAD,EXPECTED_UI_SEARCHED_DISTANCE_LOAD,
        sizeof(EXPECTED_UI_SEARCHED_DISTANCE_LOAD)
    );
    memcpy(
        base+RVA_BAGGAGE_CONFIG_LOAD,EXPECTED_BAGGAGE_CONFIG_LOAD,
        sizeof(EXPECTED_BAGGAGE_CONFIG_LOAD)
    );
    memcpy(
        base+RVA_BAGGAGE_CONFIG_CACHE_STORES,EXPECTED_BAGGAGE_CONFIG_CACHE_STORES,
        sizeof(EXPECTED_BAGGAGE_CONFIG_CACHE_STORES)
    );
    memcpy(
        base+RVA_BAGGAGE_MARKER_UNTOUCHED_LOAD,EXPECTED_BAGGAGE_MARKER_UNTOUCHED_LOAD,
        sizeof(EXPECTED_BAGGAGE_MARKER_UNTOUCHED_LOAD)
    );
    memcpy(
        base+RVA_BAGGAGE_MARKER_TOUCHED_LOAD,EXPECTED_BAGGAGE_MARKER_TOUCHED_LOAD,
        sizeof(EXPECTED_BAGGAGE_MARKER_TOUCHED_LOAD)
    );
    memcpy(
        base+RVA_BAGGAGE_MARKER_MISSION_LOAD,EXPECTED_BAGGAGE_MARKER_MISSION_LOAD,
        sizeof(EXPECTED_BAGGAGE_MARKER_MISSION_LOAD)
    );
    memcpy(
        base+RVA_BAGGAGE_FOUND_DISTANCE_LOAD,EXPECTED_BAGGAGE_FOUND_DISTANCE_LOAD,
        sizeof(EXPECTED_BAGGAGE_FOUND_DISTANCE_LOAD)
    );
    memcpy(
        base+RVA_PLAYER_RESOURCE_ROOT_LOAD,EXPECTED_PLAYER_RESOURCE_ROOT_LOAD,
        sizeof(EXPECTED_PLAYER_RESOURCE_ROOT_LOAD)
    );
    memcpy(
        base+RVA_PLAYER_RESOURCE_ENUMERATION,
        EXPECTED_PLAYER_RESOURCE_ENUMERATION,
        sizeof(EXPECTED_PLAYER_RESOURCE_ENUMERATION)
    );
    memcpy(
        base+RVA_PLAYER_ODRADEK_CONTEXT_PATH,
        EXPECTED_PLAYER_ODRADEK_CONTEXT_PATH,
        sizeof(EXPECTED_PLAYER_ODRADEK_CONTEXT_PATH)
    );
    memcpy(
        base+RVA_PLAYER_ODRADEK_CONFIG_GETTER,
        EXPECTED_PLAYER_ODRADEK_CONFIG_GETTER,
        sizeof(EXPECTED_PLAYER_ODRADEK_CONFIG_GETTER)
    );
    memcpy(
        base+RVA_DETECTED_CONSTANT_UPDATE_THUNK,
        EXPECTED_DETECTED_CONSTANT_UPDATE_THUNK,
        sizeof(EXPECTED_DETECTED_CONSTANT_UPDATE_THUNK)
    );
    memcpy(
        base+RVA_DETECTED_CONSTANT_UPDATE_CORE,
        EXPECTED_DETECTED_CONSTANT_UPDATE_CORE,
        sizeof(EXPECTED_DETECTED_CONSTANT_UPDATE_CORE)
    );
    memcpy(
        base+RVA_FIND_ENTITY_COMPONENT,EXPECTED_FIND_ENTITY_COMPONENT,
        sizeof(EXPECTED_FIND_ENTITY_COMPONENT)
    );
    memcpy(
        base+RVA_BAGGAGE_SYMBOLS_RTTI,EXPECTED_BAGGAGE_SYMBOLS_RTTI,
        sizeof(EXPECTED_BAGGAGE_SYMBOLS_RTTI)
    );
    writePointer(base,RVA_BAGGAGE_SYMBOLS_RTTI+0x18u,0x011FB830ull);
    writePointer(base,RVA_BAGGAGE_SYMBOLS_RTTI+0x20u,0x011FB890ull);
    memcpy(
        base+RVA_BAGGAGE_SYMBOLS_EVENT_CORE,
        EXPECTED_BAGGAGE_SYMBOLS_EVENT_CORE,
        sizeof(EXPECTED_BAGGAGE_SYMBOLS_EVENT_CORE)
    );
    memcpy(
        base+RVA_BAGGAGE_SYMBOLS_EVENT_THUNK,
        EXPECTED_BAGGAGE_SYMBOLS_EVENT_THUNK,
        sizeof(EXPECTED_BAGGAGE_SYMBOLS_EVENT_THUNK)
    );
    memcpy(
        base+RVA_BAGGAGE_SYMBOLS_EVENT_MESSAGE,
        EXPECTED_BAGGAGE_SYMBOLS_EVENT_MESSAGE,
        sizeof(EXPECTED_BAGGAGE_SYMBOLS_EVENT_MESSAGE)
    );
    memcpy(
        base+RVA_REALIZED_FOUND_BRIDGE,EXPECTED_REALIZED_FOUND_BRIDGE,
        sizeof(EXPECTED_REALIZED_FOUND_BRIDGE)
    );
    memcpy(
        base+RVA_REALIZED_MARKER_BRIDGE,EXPECTED_REALIZED_MARKER_BRIDGE,
        sizeof(EXPECTED_REALIZED_MARKER_BRIDGE)
    );
    memcpy(
        base+RVA_REALIZED_BAGGAGE_INIT_THUNK,
        EXPECTED_REALIZED_BAGGAGE_INIT_THUNK,
        sizeof(EXPECTED_REALIZED_BAGGAGE_INIT_THUNK)
    );
    memcpy(
        base+RVA_REALIZED_BAGGAGE_INIT_CORE,
        EXPECTED_REALIZED_BAGGAGE_INIT_CORE,
        sizeof(EXPECTED_REALIZED_BAGGAGE_INIT_CORE)
    );
    memcpy(
        base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK,
        EXPECTED_REALIZED_BAGGAGE_UNLINK_THUNK,
        sizeof(EXPECTED_REALIZED_BAGGAGE_UNLINK_THUNK)
    );
    memcpy(
        base+RVA_REALIZED_BAGGAGE_UNLINK_CORE,
        EXPECTED_REALIZED_BAGGAGE_UNLINK_CORE,
        sizeof(EXPECTED_REALIZED_BAGGAGE_UNLINK_CORE)
    );
    memcpy(
        base+RVA_REALIZED_BAGGAGE_UPDATE_THUNK,
        EXPECTED_REALIZED_BAGGAGE_UPDATE_THUNK,
        sizeof(EXPECTED_REALIZED_BAGGAGE_UPDATE_THUNK)
    );
    memcpy(
        base+RVA_REALIZED_BAGGAGE_UPDATE_CORE,
        EXPECTED_REALIZED_BAGGAGE_UPDATE_CORE,
        sizeof(EXPECTED_REALIZED_BAGGAGE_UPDATE_CORE)
    );
    memcpy(
        base+RVA_ENTITY_MANAGER_GAME_UPDATE,
        EXPECTED_ENTITY_MANAGER_GAME_UPDATE,
        sizeof(EXPECTED_ENTITY_MANAGER_GAME_UPDATE)
    );
    memcpy(
        base+RVA_ENTITY_REQUEST_REALIZE,
        EXPECTED_ENTITY_REQUEST_REALIZE,
        sizeof(EXPECTED_ENTITY_REQUEST_REALIZE)
    );
    memcpy(
        base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW,
        EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW,
        sizeof(EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW)
    );
    writePointer(base,RVA_SURVEY_FACTORY_SLOT,RVA_SURVEY_FACTORY);
    writePointer(base,RVA_GAMEPLAY_SCAN_SLOT,RVA_GAMEPLAY_SCAN_START);
    writePointer(
        base,RVA_DETECTED_CONSTANT_UPDATE_MESSAGE_SLOT,
        RVA_DETECTED_CONSTANT_UPDATE_MESSAGE
    );
    writePointer(
        base,RVA_DETECTED_CONSTANT_UPDATE_HANDLER_SLOT,
        RVA_DETECTED_CONSTANT_UPDATE_THUNK
    );
    writePointer(
        base,RVA_BAGGAGE_SYMBOLS_EVENT_MESSAGE_SLOT,
        RVA_BAGGAGE_SYMBOLS_EVENT_MESSAGE
    );
    writePointer(
        base,RVA_BAGGAGE_SYMBOLS_EVENT_HANDLER_SLOT,
        RVA_BAGGAGE_SYMBOLS_EVENT_THUNK
    );
    writePointer(
        base,RVA_REALIZED_BAGGAGE_INIT_MESSAGE_SLOT,
        RVA_REALIZED_BAGGAGE_INIT_MESSAGE
    );
    writePointer(
        base,RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT,
        RVA_REALIZED_BAGGAGE_INIT_THUNK
    );
    writePointer(
        base,RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT,
        RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE
    );
    writePointer(
        base,RVA_REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT,
        RVA_REALIZED_BAGGAGE_UNLINK_THUNK
    );
    writePointer(
        base,RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT,
        RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE
    );
    writePointer(
        base,RVA_REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT,
        RVA_REALIZED_BAGGAGE_UPDATE_THUNK
    );
    writePointer(
        base,RVA_ENTITY_MANAGER_GAME_UPDATE_SLOT,
        RVA_ENTITY_MANAGER_GAME_UPDATE
    );
    if(!validateStaticTargets(&image,true))return 2u;
    base[RVA_REALIZED_BAGGAGE_INIT_THUNK]^=1u;
    if(validateStaticTargets(&image,true))return 200u;
    base[RVA_REALIZED_BAGGAGE_INIT_THUNK]^=1u;
    base[RVA_REALIZED_BAGGAGE_UNLINK_CORE+15u]^=1u;
    if(validateStaticTargets(&image,true))return 201u;
    base[RVA_REALIZED_BAGGAGE_UNLINK_CORE+15u]^=1u;
    *(u64*)(base+RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT)=
        (u64)(base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK);
    if(validateStaticTargets(&image,true))return 202u;
    writePointer(
        base,RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT,
        RVA_REALIZED_BAGGAGE_INIT_THUNK
    );
    if(!validateStaticTargets(&image,true))return 203u;

    const u64 surveyRva=0x07000000ull;
    memset(base+surveyRva,0,0x2000u);
    writePointer(base,surveyRva,RVA_SURVEY_VTABLE);
    *(u32*)(base+surveyRva+OFF_SURVEY_SHAPE)=NATIVE_SURVEY_SHAPE;
    *(u32*)(base+surveyRva+OFF_SURVEY_ANGLE)=NATIVE_SURVEY_ANGLE_BITS;
    *(u32*)(base+surveyRva+OFF_SURVEY_RANGE)=NATIVE_SURVEY_RANGE_BITS;
    *(u32*)(base+surveyRva+OFF_SURVEY_BASE_TIME)=NATIVE_SURVEY_BASE_TIME_BITS;
    *(u32*)(base+surveyRva+OFF_SURVEY_FADE_TIME)=NATIVE_SURVEY_FADE_TIME_BITS;
    *(u32*)(base+surveyRva+OFF_SURVEY_CIRCLE_TIME)=NATIVE_SURVEY_CIRCLE_TIME_BITS;
    *(u32*)(base+surveyRva+OFF_SURVEY_SIZE_FADE)=NATIVE_SURVEY_SIZE_FADE_BITS;
    *(u32*)(base+surveyRva+OFF_SURVEY_NUM_GRID)=NATIVE_SURVEY_GRID;
    if(!validateSurveyNative(&image,base+surveyRva))return 3u;

    Settings settings;
    settings.rangeMeters=150.0f;
    settings.rangeBits=floatBits(300.0f);
    settings.uiRangeBits=floatBits(150.0f);
    settings.shapeBits=0u;
    settings.angleBits=NATIVE_SURVEY_ANGLE_BITS;
    settings.fullCircle=true;
    settings.extendCargoMarkers=true;
    if(applySurvey(base+surveyRva,&settings)!=APPLY_READY||
       !validateSurveyTarget(&image,base+surveyRva,&settings))return 4u;
    if(applySurvey(base+surveyRva,&settings)!=APPLY_READY)return 5u;

    *(u32*)(base+surveyRva+OFF_SURVEY_SHAPE)=NATIVE_SURVEY_SHAPE;
    *(u32*)(base+surveyRva+OFF_SURVEY_ANGLE)=NATIVE_SURVEY_ANGLE_BITS;
    if(!validateSurveyPatchable(&image,base+surveyRva,&settings)||
       applySurvey(base+surveyRva,&settings)!=APPLY_READY||
       !validateSurveyTarget(&image,base+surveyRva,&settings))return 15u;
    *(u32*)(base+surveyRva+OFF_SURVEY_RANGE)=NATIVE_SURVEY_RANGE_BITS;
    if(!validateSurveyPatchable(&image,base+surveyRva,&settings)||
       applySurvey(base+surveyRva,&settings)!=APPLY_READY||
       !validateSurveyTarget(&image,base+surveyRva,&settings))return 16u;

    *(u32*)(base+surveyRva+OFF_SURVEY_SHAPE)=NATIVE_SURVEY_SHAPE;
    *(u32*)(base+surveyRva+OFF_SURVEY_ANGLE)=floatBits(90.0f);
    *(u32*)(base+surveyRva+OFF_SURVEY_RANGE)=NATIVE_SURVEY_RANGE_BITS;
    if(applySurvey(base+surveyRva,&settings)!=APPLY_CONFLICT)return 9u;
    if(*(u32*)(base+surveyRva+OFF_SURVEY_SHAPE)!=NATIVE_SURVEY_SHAPE||
       *(u32*)(base+surveyRva+OFF_SURVEY_RANGE)!=NATIVE_SURVEY_RANGE_BITS)return 10u;

    __declspec(align(8)) u64 hookSlot=(u64)(base+RVA_SURVEY_FACTORY);
    u64 replacement=(u64)(const void*)&surveyFactoryHook;
    DWORD protection=0;
    bool changed=false;
    if(installPointerHook(
        (u8*)&hookSlot,(u64)(base+RVA_SURVEY_FACTORY),replacement,&protection,&changed
    )!=APPLY_READY||!changed||hookSlot!=replacement)return 11u;
    if(rollbackPointerHook(
        (u8*)&hookSlot,(u64)(base+RVA_SURVEY_FACTORY),replacement,protection
    )!=APPLY_FAILED||hookSlot!=(u64)(base+RVA_SURVEY_FACTORY))return 12u;

    const u8 returnSecondArgumentStub[4]={0x48,0x8B,0xC2,0xC3};
    memcpy(base+RVA_SURVEY_FACTORY,returnSecondArgumentStub,4u);
    g_gameBase=base;
    g_surveyResource=0;
    u8 surveyObject=0;
    if(surveyFactoryHook((void*)1,&surveyObject)!=&surveyObject||
       (u8*)(u64)g_surveyResource!=&surveyObject)return 13u;

    if(protectionAfterOwnedWrite(PAGE_WRITECOPY)!=PAGE_READWRITE||
       protectionAfterOwnedWrite(PAGE_EXECUTE_WRITECOPY)!=PAGE_EXECUTE_READWRITE||
       !protectionValueMatchesAfterOwnedWrite(PAGE_READWRITE,PAGE_WRITECOPY))return 17u;

    const u64 bodyRva=0x07002000ull;
    const u64 sensorRva=0x07003000ull;
    const u64 instanceRva=0x07004000ull;
    const u64 resourceRva=0x07005000ull;
    const u64 managerRva=0x07006000ull;
    memset(base+bodyRva,0,0x5000u);
    writePointer(base,bodyRva,RVA_BODY_SECONDARY_VTABLE);
    writePointer(base,sensorRva,RVA_SENSOR_VTABLE);
    writePointer(base,instanceRva,RVA_EFFECT_INSTANCE_VTABLE);
    writePointer(base,resourceRva,RVA_EFFECT_RESOURCE_VTABLE);
    writePointer(base,managerRva,RVA_SENSOR_MANAGER_VTABLE);
    *(u64*)(base+bodyRva+OFF_BODY_SENSOR)=(u64)(base+sensorRva);
    *(u64*)(base+sensorRva+OFF_SENSOR_EFFECT_INSTANCE)=(u64)(base+instanceRva);
    *(u64*)(base+instanceRva+OFF_EFFECT_RESOURCE)=(u64)(base+resourceRva);
    *(u64*)(base+RVA_SENSOR_MANAGER_GLOBAL)=(u64)(base+managerRva);
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_SCAN_SNAPSHOT)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_CURRENT_GENERATION)=31u;
    *(u32*)(base+managerRva+OFF_MANAGER_SNAPSHOT_GENERATION)=31u;
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(75.0f);
    *(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)=INACTIVE_SIZE_OVERRIDE_BITS;
    *(u32*)(base+resourceRva+OFF_EFFECT_RESOURCE_SIZE)=floatBits(100.0f);
    *(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)=floatBits(1.0f);
    *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)=floatBits(1.0f);
    *(double*)(base+managerRva+0x38u)=0.0;
    *(double*)(base+managerRva+0x40u)=0.0;
    *(double*)(base+managerRva+0x48u)=0.0;
    _InterlockedExchange64(
        &g_originalSensorGate,(long long)(u64)(const void*)&syntheticSensorGate
    );
    resetGateTelemetry();
    double farTarget[3]={250.0,0.0,0.0};
    if(!sensorGateTelemetryHook(base+managerRva,farTarget,0.0f,base+bodyRva)||
       g_gateCalls!=1||g_gatePasses!=1||g_gateRejects!=0||
       g_gateBin200To300!=1||g_gatePassOver200!=1||
       (u32)g_gateMaxDistanceSquaredBits!=floatBits(62500.0f)||
       (u32)g_gateLatestRadiusBits!=floatBits(75.0f))return 41u;
    double nearRejectedTarget[3]={-10.0,0.0,0.0};
    if(sensorGateTelemetryHook(base+managerRva,nearRejectedTarget,0.0f,base+bodyRva)||
       g_gateCalls!=2||g_gatePasses!=1||g_gateRejects!=1||g_gateBinTo50!=1)return 42u;
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=0u;
    sensorGateTelemetryHook(base+managerRva,farTarget,0.0f,base+bodyRva);
    if(g_gateCalls!=2||g_gatePasses!=1||g_gateRejects!=1)return 43u;
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=1u;
    _InterlockedExchange(&g_gateCaptureActive,0);
    if(!sensorGateTelemetryHook(base+managerRva,farTarget,0.0f,base+bodyRva)||
       g_gateCalls!=2||g_gatePasses!=1||g_gateRejects!=1)return 44u;
    base[RVA_GAMEPLAY_SCAN_START]=0xC3u;
    g_targetEffectDiameterBits=floatBits(300.0f);
    g_gameplayFullCircle=true;
    g_hooksReady=0;
    g_gameplayStatus=0;
    g_originalGameplayScan=(long long)(u64)(base+RVA_GAMEPLAY_SCAN_START);
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)!=INACTIVE_SIZE_OVERRIDE_BITS||
       *(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)!=floatBits(1.0f)||
       *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)!=floatBits(1.0f)||
       g_gameplayStatus!=0)return 18u;
    g_hooksReady=1;
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)!=floatBits(300.0f)||
       *(u32*)(base+resourceRva+OFF_EFFECT_RESOURCE_SIZE)!=floatBits(100.0f)||
         *(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)!=FULL_CIRCLE_RADIANS_BITS||
         *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)!=FULL_CIRCLE_RADIANS_BITS||
         *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)!=floatBits(75.0f)||
          g_gameplayStatus!=7||
          (scanPresentationSnapshot(0,0)&1ull)!=0u||
          (u32)g_gameplayBeforeDiameterBits!=floatBits(100.0f)||
         (u32)g_gameplayAfterDiameterBits!=floatBits(300.0f)||
         (u32)g_gameplayBeforeGateRadiusBits!=floatBits(75.0f)||
         (u32)g_gameplayAfterGateRadiusBits!=floatBits(75.0f))return 19u;

    *(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)=INACTIVE_SIZE_OVERRIDE_BITS;
    *(u32*)(base+resourceRva+OFF_EFFECT_RESOURCE_SIZE)=floatBits(400.0f);
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)!=INACTIVE_SIZE_OVERRIDE_BITS||
        *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)!=floatBits(75.0f)||
       (u32)g_gameplayBeforeDiameterBits!=floatBits(400.0f)||
       (u32)g_gameplayAfterDiameterBits!=floatBits(400.0f))return 20u;

    *(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)=INACTIVE_SIZE_OVERRIDE_BITS;
    *(u32*)(base+resourceRva+OFF_EFFECT_RESOURCE_SIZE)=0x7FC00000u;
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(75.0f);
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)!=INACTIVE_SIZE_OVERRIDE_BITS||
       *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)!=floatBits(75.0f)||
       (u32)g_gameplayBeforeDiameterBits!=0u||
       (u32)g_gameplayAfterDiameterBits!=0u||
       g_gameplayStatus!=3)return 40u;
    *(u32*)(base+resourceRva+OFF_EFFECT_RESOURCE_SIZE)=floatBits(400.0f);
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(200.0f);

    *(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)=floatBits(1.0f);
    *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)=floatBits(1.0f);
    DWORD managerProtection=0;
    if(!VirtualProtect(base+managerRva,0x1000u,PAGE_READONLY,&managerProtection))return 21u;
    gameplayScanStartHook(base+bodyRva,0,0,0);
    u32 protectedInstanceAngle=*(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE);
    u32 protectedManagerAngle=*(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE);
    long protectedStatus=g_gameplayStatus;
    DWORD ignoredProtection=0;
    bool managerRestored=VirtualProtect(
        base+managerRva,0x1000u,managerProtection,&ignoredProtection
    )!=FALSE;
    if(!managerRestored||protectedInstanceAngle!=floatBits(1.0f)||
       protectedManagerAngle!=floatBits(1.0f)||
       protectedStatus!=4)return 22u;

    *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)=0x7FC00000u;
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)!=floatBits(1.0f)||
       *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)!=0x7FC00000u||
       g_gameplayStatus!=4)return 22u;

    *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)=floatBits(1.0f);
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=0u;
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(g_gameplayStatus!=2||
       *(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)!=floatBits(1.0f)||
       *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)!=floatBits(1.0f))return 23u;

    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=1u;
    *(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)=INACTIVE_SIZE_OVERRIDE_BITS;
    *(u32*)(base+resourceRva+OFF_EFFECT_RESOURCE_SIZE)=floatBits(100.0f);
    g_gameplayFullCircle=false;
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)!=floatBits(300.0f)||
       *(u32*)(base+instanceRva+OFF_EFFECT_FULL_ANGLE)!=floatBits(1.0f)||
       *(u32*)(base+managerRva+OFF_MANAGER_FULL_ANGLE)!=floatBits(1.0f)||
       g_gameplayStatus!=7)return 24u;

    const u64 additionalRva=0x07007000ull;
    memset(base+additionalRva,0,0x100u);
    *(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)=INACTIVE_SIZE_OVERRIDE_BITS;
    *(u32*)(base+instanceRva+OFF_EFFECT_FLAGS)=0x00000100u;
    *(u64*)(base+resourceRva+OFF_EFFECT_RESOURCE_ADDITIONAL)=(u64)(base+additionalRva);
    *(u32*)(base+additionalRva+OFF_ADDITIONAL_SIZE)=floatBits(50.0f);
    gameplayScanStartHook(base+bodyRva,0,0,0);
    if(*(u32*)(base+instanceRva+OFF_EFFECT_SIZE_OVERRIDE)!=floatBits(300.0f)||
       (u32)g_gameplayBeforeDiameterBits!=floatBits(150.0f)||
       (u32)g_gameplayAfterDiameterBits!=floatBits(300.0f)||
       g_gameplayStatus!=7)return 25u;

    const u64 uiCacheRva=0x07008000ull;
    const u64 uiResidentRva=0x07009000ull;
    const u64 uiParametersRva=0x0700A000ull;
    memset(base+uiCacheRva,0,0x3000u);
    u8* resolvedUi=0;
    if(resolveUiGameDesign(&image,&resolvedUi)!=RESOLVE_NOT_READY||resolvedUi)return 26u;
    writePointer(base,RVA_UI_CACHE_ROOT,uiCacheRva);
    if(resolveUiGameDesign(&image,&resolvedUi)!=RESOLVE_NOT_READY||resolvedUi)return 27u;
    writePointer(base,uiCacheRva+OFF_UI_CACHE_RESIDENT,uiResidentRva);
    if(resolveUiGameDesign(&image,&resolvedUi)!=RESOLVE_INVALID||resolvedUi)return 28u;
    writePointer(base,uiResidentRva,RVA_UI_RESIDENT_VTABLE);
    if(resolveUiGameDesign(&image,&resolvedUi)!=RESOLVE_NOT_READY||resolvedUi)return 29u;
    writePointer(base,uiResidentRva+OFF_UI_RESIDENT_GAME_PARAMS,uiParametersRva);
    if(resolveUiGameDesign(&image,&resolvedUi)!=RESOLVE_INVALID||resolvedUi)return 30u;
    writePointer(base,uiParametersRva,RVA_UI_GAME_PARAMS_VTABLE);
    *(u32*)(base+uiParametersRva+0x2Cu)=floatBits(30.0f);
    *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)=
        floatBits(200.0f);
    *(u32*)(base+uiParametersRva+0x34u)=floatBits(400.0f);
    *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)=
        NATIVE_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE_BITS;
    *(u32*)(base+uiParametersRva+0x8Cu)=floatBits(30.0f);
    *(u32*)(base+uiParametersRva+0x90u)=floatBits(30.0f);
    if(resolveUiGameDesign(&image,&resolvedUi)!=RESOLVE_READY||
       resolvedUi!=base+uiParametersRva)return 31u;

    settings.rangeMeters=150.0f;
    settings.rangeBits=floatBits(300.0f);
    settings.uiRangeBits=floatBits(150.0f);
    settings.extendCargoMarkers=true;
    if(applyHudMarkers(&image,resolvedUi,&settings,true)!=APPLY_READY||
       !validateHudTarget(&image,resolvedUi,&settings,true))return 32u;
    if(*(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(200.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(150.0f)||
       *(u32*)(base+uiParametersRva+0x2Cu)!=floatBits(30.0f)||
       *(u32*)(base+uiParametersRva+0x34u)!=floatBits(400.0f)||
       *(u32*)(base+uiParametersRva+0x8Cu)!=floatBits(30.0f)||
       *(u32*)(base+uiParametersRva+0x90u)!=floatBits(30.0f))return 33u;
    if(applyHudMarkers(&image,resolvedUi,&settings,true)!=APPLY_READY)return 34u;

    *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)=
        floatBits(200.0f);
    *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)=
        NATIVE_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE_BITS;
    settings.rangeMeters=500.0f;
    settings.rangeBits=floatBits(1000.0f);
    settings.uiRangeBits=floatBits(500.0f);
    settings.extendCargoMarkers=false;
    if(applyHudMarkers(&image,resolvedUi,&settings,true)!=APPLY_READY||
       !validateHudTarget(&image,resolvedUi,&settings,true)||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(200.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(25.0f))return 35u;

    settings.extendCargoMarkers=true;
    if(applyHudMarkers(&image,resolvedUi,&settings,true)!=APPLY_READY||
       !validateHudTarget(&image,resolvedUi,&settings,true)||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(200.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(500.0f))return 36u;
    *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)=floatBits(333.0f);
    if(applyHudMarkers(&image,resolvedUi,&settings,true)!=APPLY_READY||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(333.0f))return 360u;

    *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)=
        floatBits(200.0f);
    *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)=floatBits(26.0f);
    if(applyHudMarkers(&image,resolvedUi,&settings,true)!=APPLY_CONFLICT||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(200.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(26.0f))return 37u;

    *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)=
        NATIVE_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE_BITS;
    DWORD uiOriginalProtection=0;
    if(!VirtualProtect(base+uiParametersRva,0x1000u,PAGE_READONLY,&uiOriginalProtection))return 38u;
    ApplyResult protectedHudResult=applyHudMarkers(&image,resolvedUi,&settings,true);
    DWORD uiRestoredProtection=0;
    bool uiProtectionRead=queryProtection(base+uiParametersRva,&uiRestoredProtection);
    DWORD ignoredUiProtection=0;
    bool uiMadeWritable=VirtualProtect(
        base+uiParametersRva,0x1000u,uiOriginalProtection,&ignoredUiProtection
    )!=FALSE;
    if(protectedHudResult!=APPLY_READY||!uiProtectionRead||
       uiRestoredProtection!=PAGE_READONLY||!uiMadeWritable||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(200.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(500.0f))return 39u;
    if(applyHudMarkers(&image,resolvedUi,&settings,false)!=APPLY_READY||
       !validateHudTarget(&image,resolvedUi,&settings,false)||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(200.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(25.0f))return 390u;

    HudMarkerOwnership hudOwnership;
    memset(&hudOwnership,0,sizeof(hudOwnership));
    *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)=floatBits(333.0f);
    publishScanPresentation(70u,true);
    bool hudPresentation=false;
    if(reconcileHudMarkers(
           &image,resolvedUi,&settings,&hudOwnership,&hudPresentation
       )!=APPLY_READY||hudPresentation||hudOwnership.field.owned||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(333.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(25.0f))return 3901u;
    publishScanPresentation(70u,false);
    hudPresentation=true;
    if(reconcileHudMarkers(
           &image,0,&settings,&hudOwnership,&hudPresentation
       )!=APPLY_READY||hudPresentation||hudOwnership.field.owned||
       *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)!=floatBits(333.0f)||
       *(u32*)(base+uiParametersRva+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE)!=floatBits(25.0f))return 3902u;
    *(u32*)(base+uiParametersRva+OFF_UI_SEARCHED_VISIBLE_DISTANCE)=floatBits(200.0f);

    const u64 baggageManagerRva=0x07100000ull;
    const u64 baggageConfigRva=0x07140000ull;
    memset(base+baggageManagerRva,0,0x37000u);
    memset(base+baggageConfigRva,0,0x1000u);
    writePointer(base,baggageManagerRva,RVA_BAGGAGE_MANAGER_VTABLE);
    writePointer(base,baggageManagerRva+8u,RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE);
    writePointer(base,baggageConfigRva,RVA_BAGGAGE_CONFIG_VTABLE);
    *(u64*)(base+baggageManagerRva+OFF_BAGGAGE_MANAGER_CONFIG)=
        (u64)(base+baggageConfigRva);
    *(u64*)(base+RVA_BAGGAGE_MANAGER_GLOBAL)=(u64)(base+baggageManagerRva);
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_REALIZE_DISTANCE)=floatBits(768.0f);
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNREALIZE_DISTANCE)=floatBits(800.0f);
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)=floatBits(25.0f);
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE)=floatBits(25.0f);
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_MISSION_DISTANCE)=floatBits(120.0f);
    *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)=floatBits(625.0f);
    *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_TOUCHED_CACHE_SQUARED)=floatBits(625.0f);
    *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_MISSION_CACHE_SQUARED)=floatBits(14400.0f);
    CargoMarkerObjects cargoMarkers;
    if(resolveCargoMarkerObjects(&image,&cargoMarkers)!=RESOLVE_READY||
       cargoMarkers.manager!=base+baggageManagerRva||
       cargoMarkers.config!=base+baggageConfigRva)return 41u;
    if(applyCargoMarkerRanges(&image,&cargoMarkers,&settings,true)!=APPLY_READY||
       !validateCargoMarkerTarget(&image,&cargoMarkers,&settings,true))return 42u;
    if(*(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
           floatBits(500.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=
           floatBits(250000.0f)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE)!=
           floatBits(25.0f)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_MISSION_DISTANCE)!=
           floatBits(120.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_TOUCHED_CACHE_SQUARED)!=
           floatBits(625.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_MISSION_CACHE_SQUARED)!=
           floatBits(14400.0f))return 43u;
    if(*(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_REALIZE_DISTANCE)!=floatBits(768.0f)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNREALIZE_DISTANCE)!=floatBits(800.0f)){
        return 45u;
    }
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE)=floatBits(333.0f);
    *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_MISSION_CACHE_SQUARED)=floatBits(444.0f);
    if(applyCargoMarkerRanges(&image,&cargoMarkers,&settings,true)!=APPLY_READY||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE)!=
           floatBits(333.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_MISSION_CACHE_SQUARED)!=
           floatBits(444.0f))return 44u;

    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)=floatBits(600.0f);
    if(applyCargoMarkerRanges(&image,&cargoMarkers,&settings,true)!=APPLY_CONFLICT||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
            floatBits(600.0f))return 46u;
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)=floatBits(500.0f);
    if(applyCargoMarkerRanges(&image,&cargoMarkers,&settings,false)!=APPLY_READY||
       !validateCargoMarkerTarget(&image,&cargoMarkers,&settings,false)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=floatBits(25.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=floatBits(625.0f)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE)!=floatBits(333.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_MISSION_CACHE_SQUARED)!=floatBits(444.0f))return 460u;

    CargoMarkerOwnership cargoOwnership;
    memset(&cargoOwnership,0,sizeof(cargoOwnership));
    publishScanPresentation(71u,true);
    bool cargoPresentation=false;
    if(reconcileCargoMarkerRanges(
           &image,&cargoMarkers,&settings,&cargoOwnership,&cargoPresentation
       )!=APPLY_READY||cargoPresentation||
       cargoMarkerOwnershipPresent(&cargoOwnership)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
           floatBits(25.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=
           floatBits(625.0f))return 4601u;
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)=
        floatBits(333.0f);
    publishScanPresentation(71u,false);
    cargoPresentation=true;
    if(reconcileCargoMarkerRanges(
           &image,0,&settings,&cargoOwnership,&cargoPresentation
       )!=APPLY_READY||cargoPresentation||
       cargoMarkerOwnershipPresent(&cargoOwnership)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
           floatBits(333.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=
           floatBits(625.0f))return 4602u;
    *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)=
        floatBits(25.0f);

    const u64 replacementBaggageManagerRva=0x07180000ull;
    const u64 replacementBaggageConfigRva=0x071C0000ull;
    memset(base+replacementBaggageManagerRva,0,0x37000u);
    memset(base+replacementBaggageConfigRva,0,0x1000u);
    writePointer(base,replacementBaggageManagerRva,RVA_BAGGAGE_MANAGER_VTABLE);
    writePointer(
        base,replacementBaggageManagerRva+8u,
        RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE
    );
    writePointer(base,replacementBaggageConfigRva,RVA_BAGGAGE_CONFIG_VTABLE);
    *(u64*)(base+replacementBaggageManagerRva+OFF_BAGGAGE_MANAGER_CONFIG)=
        (u64)(base+replacementBaggageConfigRva);
    *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_REALIZE_DISTANCE)=
        floatBits(768.0f);
    *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_UNREALIZE_DISTANCE)=
        floatBits(800.0f);
    *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)=
        floatBits(25.0f);
    *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE)=
        floatBits(25.0f);
    *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_MISSION_DISTANCE)=
        floatBits(120.0f);
    *(u32*)(base+replacementBaggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)=
        floatBits(625.0f);
    *(u32*)(base+replacementBaggageManagerRva+OFF_BAGGAGE_TOUCHED_CACHE_SQUARED)=
        floatBits(625.0f);
    *(u32*)(base+replacementBaggageManagerRva+OFF_BAGGAGE_MISSION_CACHE_SQUARED)=
        floatBits(14400.0f);

    publishScanPresentation(72u,true);
    if(reconcileCargoMarkerRanges(
           &image,&cargoMarkers,&settings,&cargoOwnership,&cargoPresentation
       )!=APPLY_READY||cargoMarkerOwnershipPresent(&cargoOwnership))return 4603u;
    *(u64*)(base+RVA_BAGGAGE_MANAGER_GLOBAL)=
        (u64)(base+replacementBaggageManagerRva);
    CargoMarkerObjects replacementCargoMarkers;
    if(resolveCargoMarkerObjects(
           &image,&replacementCargoMarkers
       )!=RESOLVE_READY||
       reconcileCargoMarkerRanges(
           &image,&replacementCargoMarkers,&settings,&cargoOwnership,
           &cargoPresentation
       )!=APPLY_READY||cargoPresentation||
       cargoMarkerOwnershipPresent(&cargoOwnership)||
       *(u32*)(base+baggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
           floatBits(25.0f)||
       *(u32*)(base+baggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=
           floatBits(625.0f)||
       *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
           floatBits(25.0f)||
       *(u32*)(base+replacementBaggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=
           floatBits(625.0f))return 4604u;
    publishScanPresentation(72u,false);
    if(reconcileCargoMarkerRanges(
           &image,0,&settings,&cargoOwnership,&cargoPresentation
       )!=APPLY_READY||cargoMarkerOwnershipPresent(&cargoOwnership)||
       *(u32*)(base+replacementBaggageConfigRva+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE)!=
           floatBits(25.0f)||
       *(u32*)(base+replacementBaggageManagerRva+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED)!=
           floatBits(625.0f))return 4605u;
    *(u64*)(base+RVA_BAGGAGE_MANAGER_GLOBAL)=(u64)(base+baggageManagerRva);

    const u64 playerRootRva=0x07150000ull;
    const u64 playerItemsRva=0x07151000ull;
    const u64 skippedEntryRva=0x07152000ull;
    const u64 playerEntryRva=0x07152100ull;
    const u64 playerContainerRva=0x07153000ull;
    const u64 playerContextRva=0x07159000ull;
    const u64 playerResourceRva=0x0715A000ull;
    const u64 playerConfigRva=0x0715B000ull;
    memset(base+playerRootRva,0,0xC000u);
    u8* playerConfig=0;
    if(resolvePlayerOdradekConfig(&image,&playerConfig)!=RESOLVE_NOT_READY||
       playerConfig)return 47u;
    *(u64*)(base+RVA_PLAYER_RESOURCE_ROOT)=(u64)(base+playerRootRva);
    *(u32*)(base+playerRootRva+OFF_PLAYER_ROOT_COUNT)=1u;
    *(u64*)(base+playerRootRva+OFF_PLAYER_ROOT_ITEMS)=(u64)(base+playerItemsRva);
    *(u64*)(base+playerItemsRva)=(u64)(base+skippedEntryRva);
    *(u32*)(base+skippedEntryRva+OFF_PLAYER_ENTRY_KIND)=1u;
    if(resolvePlayerOdradekConfig(&image,&playerConfig)!=RESOLVE_NOT_READY||
       playerConfig)return 48u;

    *(u32*)(base+playerRootRva+OFF_PLAYER_ROOT_COUNT)=2u;
    *(u64*)(base+playerItemsRva+8u)=(u64)(base+playerEntryRva);
    *(u32*)(base+playerEntryRva+OFF_PLAYER_ENTRY_KIND)=0u;
    *(u64*)(base+playerEntryRva+OFF_PLAYER_ENTRY_CONTAINER)=
        (u64)(base+playerContainerRva);
    *(u64*)(base+playerContainerRva+OFF_PLAYER_CONTAINER_CONTEXT)=
        (u64)(base+playerContextRva);
    *(u64*)(base+playerContextRva+OFF_PLAYER_CONTEXT_RESOURCE)=
        (u64)(base+playerResourceRva);
    *(u64*)(base+playerResourceRva+OFF_PLAYER_RESOURCE_ODRADEK_CONFIG)=
        (u64)(base+playerConfigRva);
    writePointer(base,playerConfigRva,RVA_PLAYER_ODRADEK_CONFIG_VTABLE);
    *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)=floatBits(80.0f);
    *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)=floatBits(120.0f);
    if(resolvePlayerOdradekConfig(&image,&playerConfig)!=RESOLVE_READY||
       playerConfig!=base+playerConfigRva)return 49u;
    if(applyPlayerCargoRanges(&image,playerConfig,&settings)!=APPLY_READY||
       !validatePlayerCargoTarget(&image,playerConfig,&settings)||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)!=
           floatBits(500.0f)||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)!=
           floatBits(500.0f))return 50u;

    *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)=floatBits(600.0f);
    *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)=floatBits(700.0f);
    if(applyPlayerCargoRanges(&image,playerConfig,&settings)!=APPLY_READY||
       !validatePlayerCargoTarget(&image,playerConfig,&settings)||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)!=
           floatBits(600.0f)||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)!=
           floatBits(700.0f))return 51u;

    *(u64*)(base+playerConfigRva)=(u64)(base+RVA_BAGGAGE_CONFIG_VTABLE);
    *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)=floatBits(80.0f);
    *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)=floatBits(120.0f);
    playerConfig=(u8*)1;
    if(resolvePlayerOdradekConfig(&image,&playerConfig)!=RESOLVE_INVALID||
       playerConfig||
       applyPlayerCargoRanges(&image,base+playerConfigRva,&settings)!=APPLY_FAILED||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)!=floatBits(80.0f)||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)!=
           floatBits(120.0f))return 52u;

    const u64 replacementPlayerConfigRva=0x0715B800ull;
    writePointer(base,playerConfigRva,RVA_PLAYER_ODRADEK_CONFIG_VTABLE);
    writePointer(
        base,replacementPlayerConfigRva,RVA_PLAYER_ODRADEK_CONFIG_VTABLE
    );
    *(u32*)(base+replacementPlayerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)=
        floatBits(90.0f);
    *(u32*)(base+replacementPlayerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)=
        floatBits(130.0f);
    *(u64*)(base+playerResourceRva+OFF_PLAYER_RESOURCE_ODRADEK_CONFIG)=
        (u64)(base+replacementPlayerConfigRva);
    if(applyPlayerCargoRanges(
           &image,base+playerConfigRva,&settings
       )!=APPLY_NOT_READY||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)!=floatBits(80.0f)||
       *(u32*)(base+playerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)!=floatBits(120.0f)||
       *(u32*)(base+replacementPlayerConfigRva+OFF_PLAYER_CARGO_DETECT_RANGE)!=
           floatBits(90.0f)||
       *(u32*)(base+replacementPlayerConfigRva+OFF_PLAYER_CARGO_SEARCH_RANGE)!=
           floatBits(130.0f))return 53u;

    *(u32*)(base+playerRootRva+OFF_PLAYER_ROOT_COUNT)=MAX_PLAYER_ROOT_ENTRIES+1u;
    playerConfig=(u8*)1;
    if(resolvePlayerOdradekConfig(&image,&playerConfig)!=RESOLVE_INVALID||
       playerConfig)return 54u;

    // v0.1.8 manager-snapshot/fan-out regression suite. Kept as compile-time
    // documentation, but deliberately unreachable: v0.1.9 removed this path
    // after live traces proved it both hitched and skipped every natural
    // component as a duplicate.
    if(false){
    const u64 detectedItemsRva=0x07160000ull;
    const u64 detectedCurrentRva=0x07161000ull;
    const u64 detectedEligibleRva=0x07161100ull;
    const u64 detectedForeignRva=0x07161200ull;
    const u64 detectedDisabledRva=0x07161300ull;
    const u64 detectedOwnersRva=0x07170000ull;
    const u64 detectedResourceRva=0x07174000ull;
    const u64 syntheticBaggageRva=0x07163000ull;
    const u64 syntheticRealizedRva=0x07164000ull;
    const u64 syntheticUpdateContextRva=0x07165000ull;
    memset(base+detectedItemsRva,0,0x2200u);
    memset(base+detectedOwnersRva,0,0x5000u);
    memset(base+syntheticBaggageRva,0,0x2000u);
    memset(base+syntheticUpdateContextRva,0,0x100u);
    u8* detectedComponents[4]={
        base+detectedCurrentRva,base+detectedEligibleRva,
        base+detectedForeignRva,base+detectedDisabledRva
    };
    for(u32 i=0;i<4u;i++){
        u8* component=detectedComponents[i];
        u8* owner=base+detectedOwnersRva+(u64)i*0x400u;
        writePointer(
            base,(u64)(component-base),
            i==2u?RVA_SENSOR_VTABLE:RVA_DETECTED_COMPONENT_VTABLE
        );
        component[0x38u]=1u;
        component[0x50u]=1u;
        component[OFF_DETECTED_ODRADEK_SENSOR]=1u;
        *(u64*)(component+0x30u)=(u64)(base+detectedResourceRva);
        *(u64*)(component+OFF_DETECTED_OWNER)=(u64)owner;
        ((double*)(owner+OFF_ENTITY_WORLD_POSITION))[0]=300.0;
        ((double*)(owner+OFF_ENTITY_WORLD_POSITION))[1]=0.0;
        ((double*)(owner+OFF_ENTITY_WORLD_POSITION))[2]=0.0;
        *(u64*)(base+detectedItemsRva+(u64)i*8u)=(u64)component;
    }
    (base+detectedDisabledRva)[0x50u]=0u;
    writePointer(base,syntheticBaggageRva,RVA_BAGGAGE_COMPONENT_VTABLE);
    writePointer(base,syntheticRealizedRva,RVA_REALIZED_BAGGAGE_VTABLE);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_DETECTED_COMPONENT)=
        (u64)(base+detectedEligibleRva);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=
        (u64)(base+syntheticRealizedRva);
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=
        (u64)(base+syntheticBaggageRva);
    writeAbsoluteJump(
        base+RVA_FIND_ENTITY_COMPONENT,(const void*)&syntheticFindBaggage
    );
    writeAbsoluteJump(
        base+RVA_REALIZED_FOUND_BRIDGE,(const void*)&syntheticFoundBridge
    );
    writeAbsoluteJump(
        base+RVA_REALIZED_MARKER_BRIDGE,(const void*)&syntheticMarkerBridge
    );
    *(u32*)(base+managerRva+OFF_MANAGER_DETECTED_COUNT)=4u;
    *(u32*)(base+managerRva+OFF_MANAGER_DETECTED_CAPACITY)=4u;
    *(u64*)(base+managerRva+OFF_MANAGER_DETECTED_ITEMS)=
        (u64)(base+detectedItemsRva);
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(497.0f);
    (base+syntheticUpdateContextRva)[9u]=1u;
    *(u64*)(base+syntheticUpdateContextRva+0x48u)=77u;
    *(u32*)(base+syntheticUpdateContextRva+0x60u)=floatBits(1.0f/60.0f);
    g_gameBase=base;
    g_targetScanRadiusBits=floatBits(500.0f);
    _InterlockedExchange64(
        &g_originalDetectedConstantUpdate,
        (long long)(u64)(const void*)&syntheticDetectedUpdate
    );
    _InterlockedExchange64(
        &g_originalEntityManagerGameUpdate,
        (long long)(u64)(const void*)&syntheticEntityManagerGameUpdate
    );
    g_syntheticFreshTarget=base+detectedEligibleRva;
    g_syntheticBridgeDetected=base+detectedEligibleRva;
    g_syntheticBaggage=base+syntheticBaggageRva;
    g_syntheticBaggageEntityComponents=
        base+detectedOwnersRva+0x400u+OFF_ENTITY_COMPONENTS;
    g_syntheticEntityManager=base+managerRva;
    _InterlockedExchange(
        &g_syntheticRadiusAfterUpdateBits,(long)floatBits(497.0f)
    );
    _InterlockedExchange(&g_syntheticDetectedCalls,0);
    _InterlockedExchange(&g_syntheticFoundCalls,0);
    _InterlockedExchange(&g_syntheticMarkerCalls,0);
    _InterlockedExchange(&g_syntheticFoundFresh,0);
    _InterlockedExchange(&g_syntheticMarkerFresh,1);
    _InterlockedExchange(&g_syntheticFoundDistanceBits,0);
    _InterlockedExchange(&g_syntheticFoundBreakLinks,0);
    _InterlockedExchange(&g_syntheticEntityUpdateCalls,0);
    resetFarRefreshDiagnostics();
    AcquireSRWLockExclusive(&g_farRefreshTickLock);
    memset(g_farRefreshTickBuckets,0,sizeof(g_farRefreshTickBuckets));
    _InterlockedExchange(&g_farRefreshOutstandingFanout,0);
    ReleaseSRWLockExclusive(&g_farRefreshTickLock);
    _InterlockedExchange(&g_farRefreshReady,1);
    _InterlockedExchange(&g_farRefreshPending,1);
    _InterlockedExchange(&g_farRefreshBusy,0);
    _InterlockedExchange(&g_farRefreshScanSerial,7);
    _InterlockedExchange(&g_scanAttemptEpoch,7);
    _InterlockedExchange(&g_farRefreshScanEpoch,7);
    _InterlockedExchange(&g_farRefreshPendingSerial,7);
    _InterlockedExchange(&g_farRefreshCompletedSerial,0);
    u8 syntheticMessage[0x20]={0};
    *(u64*)(syntheticMessage+0x10u)=0x123456789ABCDEF0ull;
    *(u32*)(syntheticMessage+0x18u)=77u;
    detectedConstantUpdateHook(base+detectedCurrentRva,syntheticMessage);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_syntheticEntityUpdateCalls!=1)return 550u;
    if(g_syntheticDetectedCalls!=2)return 551u;
    if(g_farRefreshPending!=0||g_farRefreshCompletedSerial!=7)return 552u;
    if(g_farRefreshManagerCount!=4||g_farRefreshSnapshotCount!=4)return 553u;
    if(g_farRefreshEligible!=1||g_farRefreshCalls!=1||
       g_farRefreshSkipped!=3)return 554u;
    if(g_farRefreshBridges!=1)return 5550u;
    if(g_farRefreshBridgeFailures!=0)return 5551u;
    if(g_syntheticFoundCalls!=1)return 5552u;
    if(g_syntheticMarkerCalls!=1)return 5553u;
    if(g_farRefreshPostJoinCalls!=1||g_farRefreshContextReady!=1||
       g_farRefreshContextInvalid!=0||g_farRefreshContextInactive!=0||
       g_farRefreshContextDeltaInvalid!=0||
       g_farRefreshContextBucketFailed!=0)return 5554u;
    if(g_farRefreshDetectedValid!=2||g_farRefreshDetectedInvalid!=2||
       g_farRefreshDistanceInvalid!=0||g_farRefreshOutsideRadius!=0||
       g_farRefreshInRadius!=2)return 5555u;
    if(g_farRefreshNoBaggage!=1||g_farRefreshBaggageFound!=1||
       g_farRefreshRealizedFound!=1||g_farRefreshUnrealized!=0||
       g_farRefreshInvalidCargo!=0)return 5556u;
    if(g_farRefreshClaimDuplicate!=0||g_farRefreshClaimFull!=0||
       g_farRefreshFreshHits!=1||g_farRefreshFreshMisses!=0||
       g_farRefreshFoundCalls!=1||g_farRefreshMarkerCalls!=1||
       g_farRefreshPostCoreBridgeFailed!=0)return 5557u;
    if((u32)g_farRefreshMaxDetectedDistanceSquaredBits!=floatBits(90000.0f)||
       (u32)g_farRefreshMaxCargoDistanceSquaredBits!=floatBits(90000.0f)||
       g_farRefreshOutstandingFanout!=0)return 5558u;
    u32 terminalSum=(u32)g_farRefreshDetectedInvalid+
        (u32)g_farRefreshDistanceInvalid+(u32)g_farRefreshOutsideRadius+
        (u32)g_farRefreshNoBaggage+(u32)g_farRefreshDeferredUnrealized+
        (u32)g_farRefreshInvalidCargo+(u32)g_farRefreshClaimDuplicate+
        (u32)g_farRefreshClaimFull+(u32)g_farRefreshFreshMisses+
        (u32)g_farRefreshPostCoreBridgeFailed+(u32)g_farRefreshBridges;
    DetectedTickBucket* releasedBucket=detectedTickBucket(77u,false);
    if(terminalSum!=(u32)g_farRefreshSnapshotCount||!releasedBucket||
       releasedBucket->fanoutCount!=0||releasedBucket->sweepGuard!=0||
       releasedBucket->pinCount!=0)return 5560u;
    if((base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]!=0u||
       *(u32*)(base+syntheticRealizedRva+OFF_REALIZED_DISTANCE_SQUARED)!=
           floatBits(90000.0f))return 556u;
    if((base+detectedCurrentRva)[0x56u]!=1u||
       (base+detectedEligibleRva)[0x56u]!=1u||
       (base+detectedForeignRva)[0x56u]!=0u||
       (base+detectedDisabledRva)[0x56u]!=0u)return 557u;

    CargoBridgeObjects unrealizedCargo;
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    if(resolveCargoBridgeObjects(
           base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,&unrealizedCargo
       )!=CARGO_BRIDGE_UNREALIZED||
       unrealizedCargo.baggage!=base+syntheticBaggageRva||
       unrealizedCargo.realized)return 5559u;
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=
        (u64)(base+syntheticRealizedRva);

    detectedConstantUpdateHook(base+detectedEligibleRva,syntheticMessage);
    if(g_syntheticDetectedCalls!=3||g_farRefreshDuplicateClaims!=0)return 56u;

    _InterlockedExchange(&g_syntheticDetectedCalls,0);
    _InterlockedExchange(&g_farRefreshPending,1);
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(480.0f);
    *(u32*)(syntheticMessage+0x18u)=78u;
    detectedConstantUpdateHook(base+detectedCurrentRva,syntheticMessage);
    tryFarRefresh(syntheticMessage);
    if(g_syntheticDetectedCalls!=1||g_farRefreshPending!=1||
       g_farRefreshBusy!=0)return 57u;

    *(u32*)(syntheticMessage+0x18u)=77u;
    detectedConstantUpdateHook(base+detectedEligibleRva,syntheticMessage);
    if(g_syntheticDetectedCalls!=2||g_farRefreshDuplicateClaims!=1)return 58u;

    _InterlockedExchange(&g_farRefreshPending,0);
    *(u32*)(syntheticMessage+0x18u)=79u;
    detectedConstantUpdateHook(base+detectedEligibleRva,syntheticMessage);
    if(g_syntheticDetectedCalls!=3||g_farRefreshOutstandingFanout!=0)return 59u;

    u8 postJoinMessage[0x20]={0};
    (base+syntheticUpdateContextRva)[9u]=0u;
    if(buildPostJoinConstantUpdateMessage(
           base+syntheticUpdateContextRva,postJoinMessage
       ))return 60u;
    (base+syntheticUpdateContextRva)[9u]=1u;
    *(u64*)(base+syntheticUpdateContextRva+0x48u)=80u;
    if(!buildPostJoinConstantUpdateMessage(
           base+syntheticUpdateContextRva,postJoinMessage
       )||*(u32*)(postJoinMessage+0x18u)!=80u||
       *(u32*)(postJoinMessage+0x1Cu)!=floatBits(1.0f/60.0f))return 60u;

    resetUnownedDetectedTickBuckets();
    const u32 nonMonotonicKeys[7]={1000u,900u,1100u,700u,1200u,600u,800u};
    bool nonMonotonicReady=true;
    AcquireSRWLockExclusive(&g_farRefreshTickLock);
    for(u32 i=0u;i<7u;i++){
        if(!detectedTickBucket(nonMonotonicKeys[i],true)){
            nonMonotonicReady=false;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_farRefreshTickLock);
    if(!nonMonotonicReady)return 600u;

    _InterlockedExchange(&g_farRefreshPending,1);
    *(u32*)(base+managerRva+OFF_MANAGER_SNAPSHOT_GENERATION)=32u;
    *(u32*)(syntheticMessage+0x18u)=81u;
    *(u64*)(base+syntheticUpdateContextRva+0x48u)=81u;
    detectedConstantUpdateHook(base+detectedCurrentRva,syntheticMessage);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_syntheticEntityUpdateCalls!=2||g_syntheticDetectedCalls!=4||
       g_farRefreshPending!=1||
       g_farRefreshBusy!=0||g_farRefreshOutstandingFanout!=0)return 61u;

    _InterlockedExchange(&g_farRefreshPending,0);
    _InterlockedExchange(&g_farRefreshBusy,1);
    *(u32*)(syntheticMessage+0x18u)=82u;
    if(beginNaturalDetectedUpdate(base+detectedEligibleRva,syntheticMessage)||
       !beginNaturalDetectedUpdate(
           base+detectedEligibleRva,syntheticMessage
       ))return 62u;
    _InterlockedExchange(&g_farRefreshBusy,0);

    _InterlockedExchange(&g_farRefreshScanSerial,10);
    _InterlockedExchange(&g_farRefreshPendingSerial,10);
    _InterlockedExchange(&g_farRefreshPending,1);
    _InterlockedExchange(&g_farRefreshCompletedSerial,0);
    publishScanPresentation(10u,true);
    if(cancelFarRefresh(9u,"synthetic stale serial")||
       g_farRefreshPending!=1||
       scanPresentationSnapshot(0,0)!=scanPresentationValue(10u,true))return 63u;
    if(!cancelFarRefresh(10u,"synthetic current serial")||
       g_farRefreshPending!=0||
       scanPresentationSnapshot(0,0)!=scanPresentationValue(10u,false))return 64u;

    publishScanPresentation(11u,true);
    if(clearScanPresentation(10u)||
       scanPresentationSnapshot(0,0)!=scanPresentationValue(11u,true))return 641u;

    *(u64*)(base+RVA_SENSOR_MANAGER_GLOBAL)=0u;
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(scanPresentationSnapshot(0,0)!=scanPresentationValue(11u,false))return 640u;
    *(u64*)(base+RVA_SENSOR_MANAGER_GLOBAL)=(u64)(base+managerRva);

    resetFarRefreshDiagnostics();
    AcquireSRWLockExclusive(&g_farRefreshTickLock);
    memset(g_farRefreshTickBuckets,0,sizeof(g_farRefreshTickBuckets));
    _InterlockedExchange(&g_farRefreshOutstandingFanout,0);
    ReleaseSRWLockExclusive(&g_farRefreshTickLock);
    *(u32*)(base+managerRva+OFF_MANAGER_DETECTED_COUNT)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_DETECTED_CAPACITY)=1u;
    *(u64*)(base+managerRva+OFF_MANAGER_DETECTED_ITEMS)=
        (u64)(base+detectedItemsRva+8u);
    *(u32*)(base+managerRva+OFF_MANAGER_CURRENT_GENERATION)=31u;
    *(u32*)(base+managerRva+OFF_MANAGER_SNAPSHOT_GENERATION)=31u;
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(499.0f);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]=0u;
    (base+detectedEligibleRva)[0x56u]=0u;
    (base+syntheticUpdateContextRva)[9u]=1u;
    *(u64*)(base+syntheticUpdateContextRva+0x48u)=90u;
    _InterlockedExchange(&g_syntheticDetectedCalls,0);
    _InterlockedExchange(&g_syntheticFoundCalls,0);
    _InterlockedExchange(&g_syntheticMarkerCalls,0);
    _InterlockedExchange(&g_syntheticFoundFresh,0);
    _InterlockedExchange(&g_syntheticMarkerFresh,1);
    _InterlockedExchange(&g_syntheticFoundDistanceBits,0);
    _InterlockedExchange(&g_syntheticFoundBreakLinks,0);
    _InterlockedExchange(&g_syntheticEntityUpdateCalls,0);
    _InterlockedExchange(&g_syntheticRealizedInitCalls,0);
    _InterlockedExchange(&g_syntheticRealizedUnlinkCalls,0);
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    memset(g_pendingCargoHits,0,sizeof(g_pendingCargoHits));
    memset(g_realizedInitFlights,0,sizeof(g_realizedInitFlights));
    g_pendingCargoCount=0u;
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    _InterlockedExchange(&g_pendingPublishedCount,0);
    _InterlockedExchange(&g_realizedInitFlightOverflow,0);
    _InterlockedExchange64(&g_pendingTestNowMs,1000);
    g_syntheticLifecycleBaggage=base+syntheticBaggageRva;
    _InterlockedExchange64(
        &g_originalRealizedBaggageInit,
        (long long)(u64)(const void*)&syntheticRealizedBaggageInit
    );
    _InterlockedExchange64(
        &g_originalRealizedBaggageUnlink,
        (long long)(u64)(const void*)&syntheticRealizedBaggageUnlink
    );
    _InterlockedExchange(&g_lateCargoReady,1);
    _InterlockedExchange(
        &g_syntheticRadiusAfterUpdateBits,(long)floatBits(497.0f)
    );
    _InterlockedExchange(&g_farRefreshReady,1);
    _InterlockedExchange(&g_farRefreshPending,1);
    _InterlockedExchange(&g_farRefreshBusy,0);
    _InterlockedExchange(&g_farRefreshScanSerial,20);
    _InterlockedExchange(&g_scanAttemptEpoch,20);
    _InterlockedExchange(&g_farRefreshScanEpoch,20);
    _InterlockedExchange(&g_farRefreshPendingSerial,20);
    _InterlockedExchange(&g_farRefreshCompletedSerial,0);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_syntheticEntityUpdateCalls!=1||g_syntheticDetectedCalls!=1||
       g_farRefreshCompletedSerial!=20||g_farRefreshPending!=0||
       g_farRefreshManagerCount!=1||g_farRefreshSnapshotCount!=1||
       g_farRefreshEligible!=1||g_farRefreshCalls!=1||
       g_farRefreshSkipped!=1)return 650u;
    if(g_farRefreshBaggageFound!=1||g_farRefreshRealizedFound!=0||
       g_farRefreshUnrealized!=1||g_farRefreshDeferredUnrealized!=1||
       g_farRefreshFreshHits!=1||g_farRefreshFreshMisses!=0||
       g_farRefreshBridges!=0||g_farRefreshFoundCalls!=0||
       g_farRefreshMarkerCalls!=0||g_farRefreshPostCoreBridgeFailed!=0){
        return 651u;
    }
    if(g_syntheticFoundCalls!=0||g_syntheticMarkerCalls!=0||
       (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]!=0u||
       (base+detectedEligibleRva)[0x56u]!=1u||
       g_farRefreshOutstandingFanout!=0||g_pendingCargoCount!=1u||
       g_pendingRecorded!=1)return 652u;
    u32 deferredTerminal=(u32)g_farRefreshDetectedInvalid+
        (u32)g_farRefreshDistanceInvalid+(u32)g_farRefreshOutsideRadius+
        (u32)g_farRefreshNoBaggage+(u32)g_farRefreshDeferredUnrealized+
        (u32)g_farRefreshInvalidCargo+(u32)g_farRefreshClaimDuplicate+
        (u32)g_farRefreshClaimFull+(u32)g_farRefreshFreshMisses+
        (u32)g_farRefreshPostCoreBridgeFailed+(u32)g_farRefreshBridges;
    if(deferredTerminal!=(u32)g_farRefreshSnapshotCount)return 653u;

    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    // The lifecycle hook may only publish readiness; it must not re-enter the
    // native Found/Marker routines.
    if(g_syntheticRealizedInitCalls!=1||g_pendingCargoCount!=1u||
       g_pendingCargoHits[0].readyRealized!=base+syntheticRealizedRva||
       g_syntheticFoundCalls!=0||g_syntheticMarkerCalls!=0||
       g_pendingConsumed!=0||g_pendingInitCalls!=1)return 654u;
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_syntheticEntityUpdateCalls!=2||g_pendingCargoCount!=0u||
       g_syntheticFoundCalls!=1||g_syntheticMarkerCalls!=1||
       g_pendingConsumed!=1||g_pendingFoundCalls!=1||
       g_pendingMarkerCalls!=1||g_pendingSyntheticFresh!=1||
       g_syntheticFoundFresh!=1||g_syntheticMarkerFresh!=0||
       (u32)g_syntheticFoundDistanceBits!=floatBits(90000.0f)||
       (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]!=0u||
       *(u32*)(base+syntheticRealizedRva+OFF_REALIZED_DISTANCE_SQUARED)!=
           floatBits(90000.0f))return 654u;
    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_syntheticRealizedInitCalls!=2||g_syntheticFoundCalls!=1||
       g_syntheticMarkerCalls!=1||g_pendingInitNoMatch!=0||
       g_syntheticEntityUpdateCalls!=3)return 655u;

    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    double pendingOrigin[3]={0.0,0.0,0.0};
    _InterlockedExchange64(&g_pendingTestNowMs,2000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,20u,20u,91u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 656u;
    long expiredBeforeBoundary=g_pendingExpired;
    _InterlockedExchange64(&g_pendingTestNowMs,6999);
    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    if(g_pendingCargoCount!=1u||
       g_pendingCargoHits[0].readyRealized!=base+syntheticRealizedRva||
       g_pendingExpired!=expiredBeforeBoundary||
       g_syntheticFoundCalls!=1||g_syntheticMarkerCalls!=1)return 6561u;
    _InterlockedExchange64(&g_pendingTestNowMs,7000);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_pendingCargoCount!=0u||
       g_pendingExpired!=expiredBeforeBoundary+1||
       g_syntheticFoundCalls!=1||g_syntheticMarkerCalls!=1)return 657u;

    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    _InterlockedExchange64(&g_pendingTestNowMs,8000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,20u,20u,92u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 658u;
    _InterlockedExchange(&g_scanAttemptEpoch,21);
    purgePendingCargo(false);
    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    if(g_pendingCargoCount!=0u||g_pendingEpochPurged!=1||
       g_syntheticFoundCalls!=1||g_syntheticMarkerCalls!=1)return 659u;

    // TTL must still hold after an entry has been atomically claimed and
    // removed from the table.
    _InterlockedExchange(&g_scanAttemptEpoch,22);
    _InterlockedExchange(&g_farRefreshScanSerial,22);
    _InterlockedExchange(&g_farRefreshScanEpoch,22);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    _InterlockedExchange64(&g_pendingTestNowMs,9000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,22u,22u,93u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 660u;
    PendingCargoHit claimedHit;
    if(!takePendingCargoHitMatching(
           base+syntheticBaggageRva,0,
           PENDING_CLAIM_UNREADY_OR_SAME,&claimedHit
       )||
       g_pendingCargoCount!=0u)return 661u;
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=
        (u64)(base+syntheticRealizedRva);
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=
        (u64)(base+syntheticBaggageRva);
    _InterlockedExchange64(&g_pendingTestNowMs,14000);
    CargoBridgeObjects claimedObjects;
    float claimedDistance=0.0f;
    if(validatePendingCargoHit(
           &claimedHit,base+syntheticRealizedRva,
           &claimedObjects,&claimedDistance
       )!=PENDING_VALIDATION_EXPIRED)return 662u;

    // Unlink owns cleanup while reciprocal links are still readable.
    _InterlockedExchange(&g_scanAttemptEpoch,23);
    _InterlockedExchange(&g_farRefreshScanSerial,23);
    _InterlockedExchange(&g_farRefreshScanEpoch,23);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    _InterlockedExchange64(&g_pendingTestNowMs,15000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,23u,23u,94u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 663u;
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=
        (u64)(base+syntheticRealizedRva);
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=
        (u64)(base+syntheticBaggageRva);
    if(!markPendingCargoReady(base+syntheticRealizedRva)||
       g_pendingCargoHits[0].readyRealized!=base+syntheticRealizedRva){
        return 6631u;
    }
    long unlinkPurgedBefore=g_pendingUnlinkPurged;
    long unlinkCallsBefore=g_syntheticRealizedUnlinkCalls;
    realizedBaggageUnlinkHook(base+syntheticRealizedRva,0);
    if(g_pendingCargoCount!=0u||
       g_pendingUnlinkPurged!=unlinkPurgedBefore+1||
       g_syntheticRealizedUnlinkCalls!=unlinkCallsBefore+1||
       g_syntheticPendingCountAtUnlink!=0||
       *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)!=0u||
       *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)!=0u){
        return 664u;
    }

    // A cargo that moved outside radius+tolerance is consumed from the table
    // but never presented.
    _InterlockedExchange(&g_scanAttemptEpoch,24);
    _InterlockedExchange(&g_farRefreshScanSerial,24);
    _InterlockedExchange(&g_farRefreshScanEpoch,24);
    _InterlockedExchange64(&g_pendingTestNowMs,16000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,24u,24u,95u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 665u;
    ((double*)(base+detectedOwnersRva+0x400u+OFF_ENTITY_WORLD_POSITION))[0]=600.0;
    long distanceRejectBefore=g_pendingInitDistanceReject;
    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_pendingCargoCount!=0u||
       g_pendingInitDistanceReject!=distanceRejectBefore+1||
       g_syntheticFoundCalls!=1||g_syntheticMarkerCalls!=1)return 666u;
    ((double*)(base+detectedOwnersRva+0x400u+OFF_ENTITY_WORLD_POSITION))[0]=300.0;

    // Reciprocal detected/baggage identity mismatches fail closed.
    _InterlockedExchange(&g_scanAttemptEpoch,25);
    _InterlockedExchange(&g_farRefreshScanSerial,25);
    _InterlockedExchange(&g_farRefreshScanEpoch,25);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    _InterlockedExchange64(&g_pendingTestNowMs,17000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,25u,25u,96u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 667u;
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_DETECTED_COMPONENT)=
        (u64)(base+detectedCurrentRva);
    long identityRejectBefore=g_pendingInitIdentityReject;
    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    if(g_pendingCargoCount!=0u||
       g_pendingInitIdentityReject!=identityRejectBefore+1||
       g_syntheticFoundCalls!=1||g_syntheticMarkerCalls!=1)return 668u;
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_DETECTED_COMPONENT)=
        (u64)(base+detectedEligibleRva);

    // If Found invalidates reciprocal links, Marker must not run and the
    // synthetic one-shot is cleared on the still-validated Detected object.
    _InterlockedExchange(&g_scanAttemptEpoch,27);
    _InterlockedExchange(&g_farRefreshScanSerial,27);
    _InterlockedExchange(&g_farRefreshScanEpoch,27);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    _InterlockedExchange64(&g_pendingTestNowMs,17500);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,27u,27u,961u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 6681u;
    long pendingBridgeFailedBefore=g_pendingBridgeFailed;
    _InterlockedExchange(&g_syntheticFoundBreakLinks,1);
    realizedBaggageInitHook(base+syntheticRealizedRva,0);
    entityManagerGameUpdateHook(0,base+syntheticUpdateContextRva);
    _InterlockedExchange(&g_syntheticFoundBreakLinks,0);
    if(g_pendingCargoCount!=0u||
       g_pendingBridgeFailed!=pendingBridgeFailedBefore+1||
       g_syntheticFoundCalls!=2||g_syntheticMarkerCalls!=1||
       (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]!=0u)return 6682u;

    // Exact duplicates do not extend TTL; ambiguous identity reuse deletes
    // the old record instead of replacing it.
    _InterlockedExchange(&g_scanAttemptEpoch,26);
    _InterlockedExchange(&g_farRefreshScanSerial,26);
    _InterlockedExchange(&g_farRefreshScanEpoch,26);
    _InterlockedExchange64(&g_pendingTestNowMs,18000);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,26u,26u,97u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 669u;
    u64 originalPendingCreated=0u;
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        if(g_pendingCargoHits[i].baggage==base+syntheticBaggageRva){
            originalPendingCreated=g_pendingCargoHits[i].createdMs;
            break;
        }
    }
    _InterlockedExchange64(&g_pendingTestNowMs,18100);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,26u,26u,98u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_DEDUPE||g_pendingCargoCount!=1u||
       !originalPendingCreated)return 670u;
    u64 dedupedCreated=0u;
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        if(g_pendingCargoHits[i].baggage==base+syntheticBaggageRva){
            dedupedCreated=g_pendingCargoHits[i].createdMs;
            break;
        }
    }
    if(dedupedCreated!=originalPendingCreated||
       rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x800u,26u,26u,99u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_REJECTED||g_pendingCargoCount!=0u)return 671u;

    // Capacity is hard-bounded: the 257th record is dropped, never evicted.
    _InterlockedExchange(&g_scanAttemptEpoch,30);
    _InterlockedExchange(&g_farRefreshScanSerial,30);
    _InterlockedExchange(&g_farRefreshScanEpoch,30);
    _InterlockedExchange64(&g_pendingTestNowMs,19000);
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        if(rememberPendingCargoHit(
               base+0x08000000ull+(u64)i*0x100u,
               base+detectedEligibleRva,
               base+detectedOwnersRva+0x400u,30u,30u,100u+i,
               floatBits(500.0f),90000.0f,pendingOrigin
           )!=PENDING_REMEMBER_RECORDED)return 672u;
    }
    long fullBefore=g_pendingFull;
    if(g_pendingCargoCount!=MAX_PENDING_CARGO_HITS||
       rememberPendingCargoHit(
           base+0x08110000ull,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,30u,30u,999u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_FULL||g_pendingFull!=fullBefore+1||
       g_pendingCargoCount!=MAX_PENDING_CARGO_HITS||
       g_pendingHighWater!=MAX_PENDING_CARGO_HITS)return 673u;
    bool firstStillPresent=false,lastStillPresent=false,overflowPresent=false;
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        u8* baggage=g_pendingCargoHits[i].baggage;
        if(baggage==base+0x08000000ull)firstStillPresent=true;
        if(baggage==base+0x0800FF00ull)lastStillPresent=true;
        if(baggage==base+0x08110000ull)overflowPresent=true;
    }
    if(!firstStillPresent||!lastStillPresent||overflowPresent)return 6731u;
    // A frame can bridge at most the declared drain cap even if more entries
    // become ready together. The remainder stays queued for later frames.
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    for(u32 i=0u;i<MAX_PENDING_CARGO_DRAIN_PER_UPDATE+1u;i++){
        g_pendingCargoHits[i].readyRealized=
            base+0x08200000ull+(u64)i*0x100u;
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    ReadyPendingCargoCandidate cappedCandidates[
        MAX_PENDING_CARGO_DRAIN_PER_UPDATE
    ];
    memset(cappedCandidates,0,sizeof(cappedCandidates));
    if(snapshotReadyPendingCargo(
           cappedCandidates,MAX_PENDING_CARGO_DRAIN_PER_UPDATE
       )!=MAX_PENDING_CARGO_DRAIN_PER_UPDATE||
       g_pendingCargoCount!=MAX_PENDING_CARGO_HITS)return 6732u;
    purgePendingCargo(true);
    if(g_pendingCargoCount!=0u)return 674u;

    // The exact init-flight table is bounded. Overflow fails closed for every
    // Realized pointer until the unmatched flight token is retired.
    static RealizedInitFlightToken flightTokens[MAX_PENDING_CARGO_HITS];
    memset(flightTokens,0,sizeof(flightTokens));
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        flightTokens[i]=beginRealizedInitFlight(
            base+0x08300000ull+(u64)i*0x100u
        );
        if(!flightTokens[i].tracked||flightTokens[i].overflow)return 67401u;
    }
    RealizedInitFlightToken overflowFlight=beginRealizedInitFlight(
        base+0x08410000ull
    );
    if(!overflowFlight.overflow||overflowFlight.tracked||
       g_realizedInitFlightOverflow!=1||
       !realizedInitInFlight(base+0x08420000ull))return 67402u;
    endRealizedInitFlight(&overflowFlight);
    if(g_realizedInitFlightOverflow!=0||
       realizedInitInFlight(base+0x08420000ull))return 67403u;
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        endRealizedInitFlight(&flightTokens[i]);
    }
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        if(g_realizedInitFlights[i].realized||
           g_realizedInitFlights[i].references)return 67404u;
    }

    // Deterministic early-link publication: reciprocal pointers already look
    // READY while native Realized initialization is still in flight. Neither
    // the normal nor catch-up consumer may bridge until the init post-hook.
    _InterlockedExchange(&g_scanAttemptEpoch,31);
    _InterlockedExchange(&g_farRefreshScanSerial,31);
    _InterlockedExchange(&g_farRefreshScanEpoch,31);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=
        (u64)(base+syntheticRealizedRva);
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=
        (u64)(base+syntheticBaggageRva);
    long foundBeforeFlight=g_syntheticFoundCalls;
    long markerBeforeFlight=g_syntheticMarkerCalls;
    RealizedInitFlightToken pendingFlight=beginRealizedInitFlight(
        base+syntheticRealizedRva
    );
    if(!pendingFlight.tracked||pendingFlight.overflow||
       !realizedInitInFlight(base+syntheticRealizedRva))return 6741u;
    _InterlockedExchange64(&g_pendingTestNowMs,19500);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,31u,31u,1100u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED||
       consumePendingCargoHitKnown(
           base+syntheticRealizedRva,base+syntheticBaggageRva,false,false
       )||
       g_pendingCargoCount!=1u)return 6742u;
    endRealizedInitFlight(&pendingFlight);
    if(realizedInitInFlight(base+syntheticRealizedRva)||
       !markPendingCargoReady(base+syntheticRealizedRva)||
       g_pendingCargoCount!=1u||
       g_syntheticFoundCalls!=foundBeforeFlight||
       g_syntheticMarkerCalls!=markerBeforeFlight)return 6743u;
    drainReadyPendingCargoHits();
    if(g_pendingCargoCount!=0u||
       g_syntheticFoundCalls!=foundBeforeFlight+1||
       g_syntheticMarkerCalls!=markerBeforeFlight+1||
       (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]!=0u)return 6743u;

    // Init completed before publication: the post-publish catch-up claims an
    // unmarked record, and a later readiness mark cannot duplicate it.
    _InterlockedExchange(&g_scanAttemptEpoch,32);
    _InterlockedExchange(&g_farRefreshScanSerial,32);
    _InterlockedExchange(&g_farRefreshScanEpoch,32);
    _InterlockedExchange64(&g_pendingTestNowMs,19600);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,32u,32u,1101u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED)return 6744u;
    long foundBeforeCatchup=g_syntheticFoundCalls;
    long markerBeforeCatchup=g_syntheticMarkerCalls;
    if(!consumePendingCargoHitKnown(
           base+syntheticRealizedRva,base+syntheticBaggageRva,false,false
       )||markPendingCargoReady(base+syntheticRealizedRva)||
       g_pendingCargoCount!=0u||
       g_syntheticFoundCalls!=foundBeforeCatchup+1||
       g_syntheticMarkerCalls!=markerBeforeCatchup+1)return 6745u;

    // The opposite order is also exactly once: a same-identity catch-up may
    // claim an already marked record, and the later drain finds nothing.
    _InterlockedExchange(&g_scanAttemptEpoch,33);
    _InterlockedExchange(&g_farRefreshScanSerial,33);
    _InterlockedExchange(&g_farRefreshScanEpoch,33);
    _InterlockedExchange64(&g_pendingTestNowMs,19700);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,33u,33u,1102u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED||
       !markPendingCargoReady(base+syntheticRealizedRva))return 6746u;
    long foundBeforeMarkedCatchup=g_syntheticFoundCalls;
    long markerBeforeMarkedCatchup=g_syntheticMarkerCalls;
    if(!consumePendingCargoHitKnown(
           base+syntheticRealizedRva,base+syntheticBaggageRva,false,false
       ))return 6747u;
    drainReadyPendingCargoHits();
    if(g_pendingCargoCount!=0u||
       g_syntheticFoundCalls!=foundBeforeMarkedCatchup+1||
       g_syntheticMarkerCalls!=markerBeforeMarkedCatchup+1)return 6748u;

    // A catch-up using a different Realized identity must discard the marked
    // record rather than presenting cargo through the wrong object.
    _InterlockedExchange(&g_scanAttemptEpoch,34);
    _InterlockedExchange(&g_farRefreshScanSerial,34);
    _InterlockedExchange(&g_farRefreshScanEpoch,34);
    _InterlockedExchange64(&g_pendingTestNowMs,19800);
    if(rememberPendingCargoHit(
           base+syntheticBaggageRva,base+detectedEligibleRva,
           base+detectedOwnersRva+0x400u,34u,34u,1103u,
           floatBits(500.0f),90000.0f,pendingOrigin
       )!=PENDING_REMEMBER_RECORDED||
       !markPendingCargoReady(base+syntheticRealizedRva))return 6749u;
    long conflictBeforeReady=g_pendingIdentityConflicts;
    long foundBeforeConflict=g_syntheticFoundCalls;
    long markerBeforeConflict=g_syntheticMarkerCalls;
    if(consumePendingCargoHitKnown(
           base+syntheticRealizedRva+0x100u,
           base+syntheticBaggageRva,false,false
       )||g_pendingCargoCount!=0u||
       g_pendingIdentityConflicts!=conflictBeforeReady+1||
       g_syntheticFoundCalls!=foundBeforeConflict||
       g_syntheticMarkerCalls!=markerBeforeConflict)return 67491u;

    // Native miss: no +0x84 means no delayed record, even for valid
    // registered-but-unrealized Lost Cargo.
    resetFarRefreshDiagnostics();
    resetUnownedDetectedTickBuckets();
    *(u32*)(base+managerRva+OFF_MANAGER_DETECTED_COUNT)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_DETECTED_CAPACITY)=1u;
    *(u64*)(base+managerRva+OFF_MANAGER_DETECTED_ITEMS)=
        (u64)(base+detectedItemsRva+8u);
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_SCAN_SNAPSHOT)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_CURRENT_GENERATION)=31u;
    *(u32*)(base+managerRva+OFF_MANAGER_SNAPSHOT_GENERATION)=31u;
    *(u32*)(base+managerRva+OFF_MANAGER_GATE_RADIUS)=floatBits(497.0f);
    *(u64*)(base+syntheticBaggageRva+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(base+syntheticRealizedRva+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]=0u;
    g_syntheticFreshTarget=0;
    _InterlockedExchange(&g_scanAttemptEpoch,40);
    _InterlockedExchange(&g_farRefreshScanEpoch,40);
    _InterlockedExchange(&g_farRefreshScanSerial,40);
    _InterlockedExchange(&g_farRefreshPendingSerial,40);
    _InterlockedExchange(&g_farRefreshPending,1);
    _InterlockedExchange(&g_farRefreshBusy,0);
    _InterlockedExchange(&g_farRefreshCompletedSerial,0);
    u8 missMessage[0x20]={0};
    *(u32*)(missMessage+0x18u)=200u;
    u32 recordedBeforeMiss=(u32)g_pendingRecorded;
    tryFarRefresh(missMessage);
    if(g_farRefreshCompletedSerial!=40||g_farRefreshFreshHits!=0||
       g_farRefreshFreshMisses!=1||g_farRefreshDeferredUnrealized!=0||
       g_pendingCargoCount!=0u||(u32)g_pendingRecorded!=recordedBeforeMiss||
       (base+detectedEligibleRva)[OFF_DETECTED_FRESH_HIT]!=0u)return 675u;
    g_syntheticFreshTarget=base+detectedEligibleRva;
    _InterlockedExchange64(&g_pendingTestNowMs,-1);
    }

    // v0.1.10: a genuine READY hit remains native; the wrapper never invokes
    // Found/Marker and never calls the detected original a second time.
    const u64 naturalDetectedRva=0x07161000ull;
    const u64 naturalOwnerRva=0x07170000ull;
    const u64 naturalResourceRva=0x07174000ull;
    const u64 naturalBaggageRva=0x07163000ull;
    const u64 naturalRealizedRva=0x07164000ull;
    const u64 naturalSymbolsRva=0x071A0000ull;
    const u64 naturalSymbolRecordsRva=0x071A1000ull;
    const u64 naturalSymbolTemplateRva=0x071A2000ull;
    memset(base+naturalDetectedRva,0,0x1000u);
    memset(base+naturalOwnerRva,0,0x4000u);
    memset(base+naturalBaggageRva,0,0x2000u);
    memset(base+naturalSymbolsRva,0,0x3000u);
    u8* naturalDetected=base+naturalDetectedRva;
    u8* naturalOwner=base+naturalOwnerRva;
    u8* naturalBaggage=base+naturalBaggageRva;
    u8* naturalRealized=base+naturalRealizedRva;
    u8* naturalSymbols=base+naturalSymbolsRva;
    writePointer(base,naturalDetectedRva,RVA_DETECTED_COMPONENT_VTABLE);
    writePointer(base,naturalBaggageRva,RVA_BAGGAGE_COMPONENT_VTABLE);
    writePointer(base,naturalRealizedRva,RVA_REALIZED_BAGGAGE_VTABLE);
    writePointer(base,naturalSymbolsRva,RVA_BAGGAGE_SYMBOLS_VTABLE);
    writePointer(
        base,naturalSymbolsRva+0x20u,RVA_BAGGAGE_SYMBOLS_SECONDARY_VTABLE
    );
    naturalDetected[0x38u]=1u;
    naturalDetected[0x50u]=1u;
    naturalDetected[OFF_DETECTED_ODRADEK_SENSOR]=1u;
    *(u64*)(naturalDetected+0x30u)=(u64)(base+naturalResourceRva);
    *(u64*)(naturalDetected+OFF_DETECTED_OWNER)=(u64)naturalOwner;
    ((double*)(naturalOwner+OFF_ENTITY_WORLD_POSITION))[0]=300.0;
    *(u64*)(naturalBaggage+OFF_BAGGAGE_DETECTED_COMPONENT)=
        (u64)naturalDetected;
    *(u64*)(naturalBaggage+OFF_BAGGAGE_REALIZED_COMPONENT)=
        (u64)naturalRealized;
    *(u64*)(naturalRealized+OFF_REALIZED_BAGGAGE_COMPONENT)=
        (u64)naturalBaggage;
    *(u64*)(naturalRealized+OFF_DETECTED_OWNER)=(u64)naturalOwner;
    naturalSymbols[0x38u]=1u;
    *(u64*)(naturalSymbols+OFF_BAGGAGE_SYMBOLS_OWNER)=(u64)naturalOwner;
    *(u64*)(naturalSymbols+OFF_BAGGAGE_SYMBOLS_BAGGAGE)=(u64)naturalBaggage;
    (base+naturalSymbolTemplateRva)[0x20u]=BAGGAGE_SYMBOL_EVENT_CATEGORY;
    *(u32*)(base+naturalSymbolTemplateRva+
        OFF_BAGGAGE_SYMBOL_TEMPLATE_DURATION)=floatBits(5.0f);
    writeAbsoluteJump(
        base+RVA_FIND_ENTITY_COMPONENT,(const void*)&syntheticFindBaggage
    );
    g_syntheticBaggage=naturalBaggage;
    g_syntheticBaggageEntityComponents=naturalOwner+OFF_ENTITY_COMPONENTS;
    g_syntheticFreshTarget=naturalDetected;
    g_syntheticLifecycleBaggage=naturalBaggage;
    g_syntheticSymbols=naturalSymbols;
    g_syntheticSymbolRecords=base+naturalSymbolRecordsRva;
    g_syntheticSymbolTemplate=base+naturalSymbolTemplateRva;
    g_testFindEntityComponent=&syntheticFindSymbols;
    g_testBaggageSymbolsEvent=&syntheticBaggageSymbolEvent;
    g_testRequestEntityRealize=&syntheticRequestEntityRealize;
    g_gameBase=base;
    g_targetScanRadiusBits=floatBits(500.0f);
    *(u64*)(base+RVA_SENSOR_MANAGER_GLOBAL)=(u64)(base+managerRva);
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_SCAN_SNAPSHOT)=1u;
    *(u32*)(base+managerRva+OFF_MANAGER_CURRENT_GENERATION)=90u;
    *(u32*)(base+managerRva+OFF_MANAGER_SNAPSHOT_GENERATION)=90u;
    ((double*)(base+managerRva+0x38u))[0]=0.0;
    ((double*)(base+managerRva+0x38u))[1]=0.0;
    ((double*)(base+managerRva+0x38u))[2]=0.0;
    _InterlockedExchange64(
        &g_originalDetectedConstantUpdate,
        (long long)(u64)(const void*)&syntheticDetectedUpdate
    );
    _InterlockedExchange64(
        &g_originalRealizedBaggageInit,
        (long long)(u64)(const void*)&syntheticRealizedBaggageInit
    );
    _InterlockedExchange64(
        &g_originalRealizedBaggageUpdate,
        (long long)(u64)(const void*)&syntheticRealizedBaggageUpdate
    );
    _InterlockedExchange(&g_lateCargoReady,1);
    _InterlockedExchange(&g_farRefreshReady,1);
    _InterlockedExchange(&g_scanAttemptEpoch,90);
    _InterlockedExchange(&g_farRefreshScanEpoch,90);
    _InterlockedExchange(&g_farRefreshScanSerial,90);
    publishScanPresentation(90u,true);
    resetFarRefreshDiagnostics();
    purgePendingCargo(true);
    purgeRealizeLeases(true);
    _InterlockedExchange(&g_realizeLeaseRecorded,0);
    _InterlockedExchange(&g_realizeLeaseDedupe,0);
    _InterlockedExchange(&g_realizeLeaseFull,0);
    _InterlockedExchange(&g_realizeLeaseExpired,0);
    _InterlockedExchange(&g_realizeLeaseEpochPurged,0);
    _InterlockedExchange(&g_realizeLeaseIdentityPurged,0);
    _InterlockedExchange(&g_realizeLeaseUnlinkPurged,0);
    _InterlockedExchange(&g_realizeLeaseHighWater,0);
    _InterlockedExchange(&g_realizeRequestInitial,0);
    _InterlockedExchange(&g_realizeRequestRelay,0);
    _InterlockedExchange(&g_realizeInitialStateRejected,0);
    _InterlockedExchange(&g_realizeCoordinatorCalls,0);
    _InterlockedExchange(&g_realizeCoordinatorSnapshots,0);
    _InterlockedExchange64(&g_pendingTestNowMs,30000);
    _InterlockedExchange(&g_syntheticDetectedCalls,0);
    _InterlockedExchange(&g_syntheticFoundCalls,0);
    _InterlockedExchange(&g_syntheticMarkerCalls,0);
    _InterlockedExchange(&g_syntheticRealizedUpdateCalls,0);
    _InterlockedExchange(&g_pendingConsumed,0);
    _InterlockedExchange(&g_pendingNativeUpdateCalls,0);
    _InterlockedExchange(&g_pendingSyntheticFresh,0);
    _InterlockedExchange(&g_syntheticSymbolEventCalls,0);
    _InterlockedExchange(&g_syntheticSymbolEventEnabled,1);
    _InterlockedExchange(&g_syntheticRequestRealizeCalls,0);
    _InterlockedExchange(&g_syntheticRequestRealizeDl,-1);
    _InterlockedExchange(&g_syntheticRequestRealizeR8,-1);
    g_syntheticRequestRealizeOwner=0;
    u8 naturalMessage[0x20]={0};
    *(u32*)(naturalMessage+0x18u)=900u;
    detectedConstantUpdateHook(naturalDetected,naturalMessage);
    if(g_syntheticDetectedCalls!=1||
       naturalDetected[OFF_DETECTED_FRESH_HIT]!=1u||
       g_farRefreshFreshHits!=1||g_farRefreshBaggageFound!=1||
       g_farRefreshRealizedFound!=1||g_farRefreshUnrealized!=0||
       g_pendingCargoCount!=0u||g_syntheticFoundCalls!=0||
       g_syntheticMarkerCalls!=0)return 680u;

    // An exact UNREALIZED native hit publishes both bounded identities before
    // RequestRealize(owner, false, true). The retired fresh bit is replayed
    // only after the genuine Realized component finishes initialization.
    *(u64*)(naturalBaggage+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(naturalRealized+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    *(u64*)(naturalOwner+0x98u)=0x200ull;
    naturalOwner[0x30Fu]=0u;
    naturalDetected[OFF_DETECTED_FRESH_HIT]=0u;
    long pendingRecordedBeforeNatural=g_pendingRecorded;
    long pendingDedupeBeforeNatural=g_pendingDedupe;
    *(u32*)(naturalMessage+0x18u)=901u;
    detectedConstantUpdateHook(naturalDetected,naturalMessage);
    if(g_syntheticDetectedCalls!=2||
       naturalDetected[OFF_DETECTED_FRESH_HIT]!=0u||
       g_farRefreshUnrealized!=1||g_farRefreshDeferredUnrealized!=1||
       g_symbolRevealAttempts!=0||g_symbolRevealSucceeded!=0||
       g_syntheticSymbolEventCalls!=0||
       g_pendingCargoCount!=1u||
       g_pendingRecorded!=pendingRecordedBeforeNatural+1||
       realizeLeaseCountSnapshot()!=1u||g_realizeLeasePublishedCount!=1||
       g_realizeLeaseRecorded!=1||g_realizeLeaseHighWater!=1||
       g_realizeRequestInitial!=1||g_syntheticRequestRealizeCalls!=1||
       g_syntheticRequestRealizeOwner!=naturalOwner||
       g_syntheticRequestRealizeDl!=0||g_syntheticRequestRealizeR8!=1||
       (*(u64*)(naturalOwner+0x98u)&0x1000000000ull)==0ull||
       (naturalOwner[0x30Fu]&4u)==0u)return 681u;

    // Repeated native delivery for the same scan identity deduplicates both
    // records without extending the lease, but safely reasserts Realize.
    naturalDetected[OFF_DETECTED_FRESH_HIT]=0u;
    *(u32*)(naturalMessage+0x18u)=902u;
    detectedConstantUpdateHook(naturalDetected,naturalMessage);
    if(g_syntheticDetectedCalls!=3||
       naturalDetected[OFF_DETECTED_FRESH_HIT]!=0u||
       g_farRefreshUnrealized!=2||g_farRefreshDeferredUnrealized!=2||
       g_symbolRevealAttempts!=0||g_syntheticSymbolEventCalls!=0||
       g_pendingCargoCount!=1u||
       g_pendingRecorded!=pendingRecordedBeforeNatural+1||
       g_pendingDedupe!=pendingDedupeBeforeNatural+1||
       realizeLeaseCountSnapshot()!=1u||g_realizeLeasePublishedCount!=1||
       g_realizeLeaseRecorded!=1||g_realizeLeaseDedupe!=1||
       g_realizeRequestInitial!=2||g_syntheticRequestRealizeCalls!=2||
       g_syntheticRequestRealizeOwner!=naturalOwner||
       g_syntheticRequestRealizeDl!=0||g_syntheticRequestRealizeR8!=1){
        return 6811u;
    }

    // Model completion of the native realization transition. Consuming the
    // pending fresh hit must not retire the independent 15-second lease.
    *(u64*)(naturalOwner+0x98u)=0u;
    naturalOwner[0x30Fu]=0u;
    realizedBaggageInitHook(naturalRealized,0);
    if(g_pendingCargoCount!=1u||
       g_pendingCargoHits[0].readyRealized!=naturalRealized||
       realizeLeaseCountSnapshot()!=1u||g_realizeLeasePublishedCount!=1||
       g_syntheticFoundCalls!=0||g_syntheticMarkerCalls!=0)return 682u;
    realizedBaggageUpdateHook(naturalRealized,0);
    if(g_pendingCargoCount!=0u||g_syntheticRealizedUpdateCalls!=1||
       g_pendingConsumed!=1||g_pendingNativeUpdateCalls!=1||
       g_pendingSyntheticFresh!=1||
       naturalDetected[OFF_DETECTED_FRESH_HIT]!=1u||
       realizeLeaseCountSnapshot()!=1u||g_realizeLeasePublishedCount!=1||
       g_syntheticFoundCalls!=0||g_syntheticMarkerCalls!=0)return 683u;
    realizedBaggageUpdateHook(naturalRealized,0);
    if(g_syntheticRealizedUpdateCalls!=2||g_pendingConsumed!=1||
       g_pendingNativeUpdateCalls!=1)return 684u;

    // The native coordinator first schedules pending-unrealize. The relay
    // then runs once for the exact validated lease and cancels that state by
    // reissuing the cancellation-only RequestRealize(owner, false, false).
    g_syntheticCoordinatorOwner=naturalOwner;
    _InterlockedExchange(&g_syntheticCoordinatorCalls,0);
    _InterlockedExchange64(
        &g_originalRealizationCoordinator,
        (long long)(u64)(const void*)&syntheticRealizationCoordinator
    );
    realizationCoordinatorHook(naturalOwner);
    if(g_syntheticCoordinatorCalls!=1||g_realizeCoordinatorCalls!=1||
       g_realizeCoordinatorSnapshots!=1||g_realizeRequestRelay!=1||
       g_syntheticRequestRealizeCalls!=3||
       g_syntheticRequestRealizeOwner!=naturalOwner||
       g_syntheticRequestRealizeDl!=0||g_syntheticRequestRealizeR8!=0||
       (*(u64*)(naturalOwner+0x98u)&0x0C00000000ull)!=0ull||
       naturalOwner[0x30Fu]!=0u||realizeLeaseCountSnapshot()!=1u){
        return 6841u;
    }

    // Exact TTL boundary: the lease is valid at 14,999 ms and absent at
    // 15,000 ms. Expiry does not affect the already consumed pending hit.
    long leaseExpiredBefore=g_realizeLeaseExpired;
    _InterlockedExchange64(&g_pendingTestNowMs,44999);
    purgeRealizeLeases(false);
    if(realizeLeaseCountSnapshot()!=1u||
       g_realizeLeaseExpired!=leaseExpiredBefore)return 6842u;
    _InterlockedExchange64(&g_pendingTestNowMs,45000);
    purgeRealizeLeases(false);
    if(realizeLeaseCountSnapshot()!=0u||g_realizeLeasePublishedCount!=0||
       g_realizeLeaseExpired!=leaseExpiredBefore+1||
       g_pendingCargoCount!=0u)return 6843u;
    _InterlockedExchange64(&g_pendingTestNowMs,-1);
    g_syntheticCoordinatorOwner=0;

    // A null Realized link is insufficient on its own. Active owners and
    // owners already entering either unrealize stage must be left completely
    // native: no pending record, no lease, no request, and no retired hit.
    *(u64*)(naturalBaggage+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(naturalRealized+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    const u64 rejectedLifecycleStates[3]={
        0u,
        ENTITY_FLAG_INACTIVE|ENTITY_FLAG_REQUEST_UNREALIZE,
        ENTITY_FLAG_INACTIVE|ENTITY_FLAG_STAGED_UNREALIZE
    };
    long stateRejectBefore=g_realizeInitialStateRejected;
    long requestBeforeReject=g_syntheticRequestRealizeCalls;
    long unrealizedBeforeReject=g_farRefreshUnrealized;
    for(u32 i=0u;i<3u;i++){
        *(u64*)(naturalOwner+OFF_ENTITY_LIFECYCLE_FLAGS)=
            rejectedLifecycleStates[i];
        naturalDetected[OFF_DETECTED_FRESH_HIT]=1u;
        *(u32*)(naturalMessage+0x18u)=904u+i;
        captureNaturalFreshCargoHit(naturalDetected,naturalMessage);
        if(naturalDetected[OFF_DETECTED_FRESH_HIT]!=1u||
           g_pendingCargoCount!=0u||realizeLeaseCountSnapshot()!=0u||
           g_syntheticRequestRealizeCalls!=requestBeforeReject){
            return 6844u+i;
        }
    }
    if(g_realizeInitialStateRejected!=stateRejectBefore+3||
       g_farRefreshUnrealized!=unrealizedBeforeReject+3)return 6847u;

    // A 600 m native hit is never claimed by the 500 m feature.
    _InterlockedExchange(&g_scanAttemptEpoch,91);
    _InterlockedExchange(&g_farRefreshScanEpoch,91);
    _InterlockedExchange(&g_farRefreshScanSerial,91);
    publishScanPresentation(91u,true);
    purgePendingCargo(false);
    ((double*)(naturalOwner+OFF_ENTITY_WORLD_POSITION))[0]=600.0;
    *(u64*)(naturalBaggage+OFF_BAGGAGE_REALIZED_COMPONENT)=0u;
    *(u64*)(naturalRealized+OFF_REALIZED_BAGGAGE_COMPONENT)=0u;
    naturalDetected[OFF_DETECTED_FRESH_HIT]=0u;
    *(u32*)(naturalMessage+0x18u)=903u;
    detectedConstantUpdateHook(naturalDetected,naturalMessage);
    if(naturalDetected[OFF_DETECTED_FRESH_HIT]!=1u||
       g_farRefreshOutsideRadius!=1||g_pendingCargoCount!=0u)return 685u;
    ((double*)(naturalOwner+OFF_ENTITY_WORLD_POSITION))[0]=300.0;

    // The EntityManager hook is now a strict pass-through and cannot perform
    // a component snapshot or second detected update.
    _InterlockedExchange64(
        &g_originalEntityManagerGameUpdate,
        (long long)(u64)(const void*)&syntheticEntityManagerGameUpdate
    );
    _InterlockedExchange(&g_syntheticEntityUpdateCalls,0);
    long detectedBeforePassThrough=g_syntheticDetectedCalls;
    entityManagerGameUpdateHook(0,0);
    if(g_syntheticEntityUpdateCalls!=1||
       g_syntheticDetectedCalls!=detectedBeforePassThrough)return 686u;

    // Scan completion still clears presentation immediately; neither global
    // marker distance nor unrelated mission/construction HUD state is leased.
    publishScanPresentation(92u,true);
    *(u32*)(base+managerRva+OFF_MANAGER_ACTIVE)=0u;
    _InterlockedExchange(&g_farRefreshCompletedSerial,0);
    failClosedPresentationWatchdog(&image);
    if(scanPresentationSnapshot(0,0)!=scanPresentationValue(92u,false)||
       g_farRefreshCompletedSerial!=92||
       g_pendingCargoCount!=0u)return 687u;

    // Repeated watchdog completion remains idempotent.
    publishScanPresentation(93u,true);
    _InterlockedExchange(&g_farRefreshCompletedSerial,0);
    failClosedPresentationWatchdog(&image);
    if(scanPresentationSnapshot(0,0)!=scanPresentationValue(93u,false)||
       g_farRefreshCompletedSerial!=93)return 689u;
    g_testFindEntityComponent=0;
    g_testBaggageSymbolsEvent=0;
    g_testRequestEntityRealize=0;
    _InterlockedExchange64(&g_originalRealizationCoordinator,0);
    _InterlockedExchange64(&g_pendingTestNowMs,-1);

    return 0u;
}

extern "C" __declspec(dllexport) u32 RunMappedImageHookTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 1u;
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateStaticTargets(&image,false))return 2u;
    u8* slot=base+RVA_SURVEY_FACTORY_SLOT;
    u64 expected=(u64)(base+RVA_SURVEY_FACTORY);
    u64 replacement=(u64)(const void*)&surveyFactoryHook;
    DWORD beforeProtection=0;
    if(!queryProtection(slot,&beforeProtection)||beforeProtection!=PAGE_WRITECOPY)return 3u;
    DWORD originalProtection=0;
    bool changed=false;
    if(installPointerHook(slot,expected,replacement,&originalProtection,&changed)!=APPLY_READY||
       !changed||originalProtection!=PAGE_WRITECOPY)return 4u;
    u64 current=0;
    DWORD activeProtection=0;
    if(!readU64(slot,&current)||current!=replacement||
       !queryProtection(slot,&activeProtection)||activeProtection!=PAGE_READWRITE||
       !protectionMatchesAfterOwnedWrite(slot,originalProtection))return 5u;
    if(rollbackPointerHook(slot,expected,replacement,originalProtection)!=APPLY_FAILED)return 6u;
    if(!readU64(slot,&current)||current!=expected||
       !queryProtection(slot,&activeProtection)||activeProtection!=PAGE_READWRITE||
       !protectionMatchesAfterOwnedWrite(slot,originalProtection))return 7u;

    slot=base+RVA_GAMEPLAY_SCAN_SLOT;
    expected=(u64)(base+RVA_GAMEPLAY_SCAN_START);
    replacement=(u64)(const void*)&gameplayScanStartHook;
    beforeProtection=0;
    originalProtection=0;
    changed=false;
    if(!queryProtection(slot,&beforeProtection)||
       (beforeProtection!=PAGE_WRITECOPY&&beforeProtection!=PAGE_READONLY))return 8u;
    if(installPointerHook(slot,expected,replacement,&originalProtection,&changed)!=APPLY_READY||
       !changed||originalProtection!=beforeProtection)return 9u;
    if(!readU64(slot,&current)||current!=replacement||
       !queryProtection(slot,&activeProtection)||
       !protectionValueMatchesAfterOwnedWrite(activeProtection,originalProtection)||
       !protectionMatchesAfterOwnedWrite(slot,originalProtection))return 10u;
    if(rollbackPointerHook(slot,expected,replacement,originalProtection)!=APPLY_FAILED)return 11u;
    if(!readU64(slot,&current)||current!=expected||
       !queryProtection(slot,&activeProtection)||
       !protectionValueMatchesAfterOwnedWrite(activeProtection,originalProtection)||
       !protectionMatchesAfterOwnedWrite(slot,originalProtection))return 12u;

    u8* gateWindow=base+RVA_SENSOR_GATE_CALL_WINDOW;
    DWORD gateProtection=0;
    if(!queryProtection(gateWindow,&gateProtection))return 13u;
    u8* gateRelay=0;
    if(installGateTelemetry(base,EXPECTED_IMAGE_SIZE,&gateRelay)!=APPLY_READY||!gateRelay){
        return 14u;
    }
    u64 patchedWindow=0;
    u64 relayTarget=0;
    u32 patchedDisplacement=0;
    DWORD relayProtection=0;
    static const u8 relayPrefix[5]={0x49,0x89,0xF9,0x48,0xB8};
    static const u8 relaySuffix[2]={0xFF,0xE0};
    if(!readU64(gateWindow,&patchedWindow)||
       patchedWindow==0xC084FF10136AE800ull||
       !readU32(gateWindow+2u,&patchedDisplacement)||
       (u8*)((s64)(u64)(base+RVA_SENSOR_GATE_CALL+5u)+(s64)(s32)patchedDisplacement)!=
            gateRelay||
       !bytesEqual(gateRelay,relayPrefix,(u32)sizeof(relayPrefix))||
       !readU64(gateRelay+5u,&relayTarget)||
       relayTarget!=(u64)(const void*)&sensorGateTelemetryHook||
       !bytesEqual(gateRelay+13u,relaySuffix,(u32)sizeof(relaySuffix))||
       !queryProtection(gateRelay,&relayProtection)||relayProtection!=PAGE_EXECUTE_READ){
        return 15u;
    }
    if(rollbackGateTelemetryPatch(base,patchedWindow,gateProtection)!=APPLY_FAILED){
        return 16u;
    }
    if(!readU64(gateWindow,&patchedWindow)||patchedWindow!=0xC084FF10136AE800ull){
        return 17u;
    }

    u8* detectedSlot=base+RVA_DETECTED_CONSTANT_UPDATE_HANDLER_SLOT;
    DWORD detectedProtection=0;
    if(!queryProtection(detectedSlot,&detectedProtection))return 18u;
    u64 detectedExpected=(u64)(base+RVA_DETECTED_CONSTANT_UPDATE_THUNK);
    u64 detectedReplacement=(u64)(const void*)&detectedConstantUpdateHook;
    DWORD detectedOriginalProtection=0;
    bool detectedChanged=false;
    if(installPointerHook(
           detectedSlot,detectedExpected,detectedReplacement,
           &detectedOriginalProtection,&detectedChanged
       )!=APPLY_READY||!detectedChanged||
       detectedOriginalProtection!=detectedProtection)return 19u;
    u64 detectedPatched=0;
    DWORD detectedActiveProtection=0;
    if(!readU64(detectedSlot,&detectedPatched)||
       detectedPatched!=detectedReplacement||
       !queryProtection(detectedSlot,&detectedActiveProtection)||
       !protectionValueMatchesAfterOwnedWrite(
           detectedActiveProtection,detectedOriginalProtection
       )||!protectionMatchesAfterOwnedWrite(
           detectedSlot,detectedOriginalProtection
       ))return 20u;
    if(rollbackPointerHook(
           detectedSlot,detectedExpected,detectedReplacement,
           detectedOriginalProtection
       )!=APPLY_FAILED)return 21u;
    if(!readU64(detectedSlot,&detectedPatched)||
       detectedPatched!=detectedExpected||
       !queryProtection(detectedSlot,&detectedActiveProtection)||
       !protectionValueMatchesAfterOwnedWrite(
           detectedActiveProtection,detectedOriginalProtection
       )||!protectionMatchesAfterOwnedWrite(
           detectedSlot,detectedOriginalProtection
       ))return 22u;

    u8* entityManagerSlot=base+RVA_ENTITY_MANAGER_GAME_UPDATE_SLOT;
    DWORD entityManagerProtection=0;
    if(!queryProtection(
           entityManagerSlot,&entityManagerProtection
       ))return 23u;
    u64 entityManagerExpected=(u64)(base+RVA_ENTITY_MANAGER_GAME_UPDATE);
    u64 entityManagerReplacement=(u64)(const void*)&entityManagerGameUpdateHook;
    DWORD entityManagerOriginalProtection=0;
    bool entityManagerChanged=false;
    if(installPointerHook(
           entityManagerSlot,entityManagerExpected,entityManagerReplacement,
           &entityManagerOriginalProtection,&entityManagerChanged
       )!=APPLY_READY||!entityManagerChanged||
       entityManagerOriginalProtection!=entityManagerProtection)return 24u;
    u64 entityManagerPatched=0;
    DWORD entityManagerActiveProtection=0;
    if(!readU64(entityManagerSlot,&entityManagerPatched)||
       entityManagerPatched!=entityManagerReplacement||
       !queryProtection(
           entityManagerSlot,&entityManagerActiveProtection
       )||!protectionValueMatchesAfterOwnedWrite(
           entityManagerActiveProtection,entityManagerOriginalProtection
       )||!protectionMatchesAfterOwnedWrite(
           entityManagerSlot,entityManagerOriginalProtection
       ))return 25u;
    if(rollbackPointerHook(
           entityManagerSlot,entityManagerExpected,entityManagerReplacement,
           entityManagerOriginalProtection
       )!=APPLY_FAILED)return 26u;
    if(!readU64(entityManagerSlot,&entityManagerPatched)||
       entityManagerPatched!=entityManagerExpected||
       !queryProtection(
           entityManagerSlot,&entityManagerActiveProtection
       )||!protectionValueMatchesAfterOwnedWrite(
           entityManagerActiveProtection,entityManagerOriginalProtection
       )||!protectionMatchesAfterOwnedWrite(
           entityManagerSlot,entityManagerOriginalProtection
       ))return 27u;

    struct LifecycleHookCase{
        u8* slot;
        u64 expected;
        u64 replacement;
    } lifecycleHooks[2]={
        {
            base+RVA_REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT,
            (u64)(base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK),
            (u64)(const void*)&realizedBaggageUnlinkHook
        },{
            base+RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT,
            (u64)(base+RVA_REALIZED_BAGGAGE_INIT_THUNK),
            (u64)(const void*)&realizedBaggageInitHook
        }
    };
    for(u32 i=0u;i<2u;i++){
        DWORD before=0,ownedProtection=0,after=0;
        bool hookChanged=false;
        u64 value=0;
        if(!queryProtection(lifecycleHooks[i].slot,&before))return 28u+i*4u;
        if(installPointerHook(
               lifecycleHooks[i].slot,lifecycleHooks[i].expected,
               lifecycleHooks[i].replacement,&ownedProtection,&hookChanged
           )!=APPLY_READY||!hookChanged||ownedProtection!=before){
            return 29u+i*4u;
        }
        if(!readU64(lifecycleHooks[i].slot,&value)||
           value!=lifecycleHooks[i].replacement||
           !queryProtection(lifecycleHooks[i].slot,&after)||
           !protectionValueMatchesAfterOwnedWrite(after,ownedProtection)||
           !protectionMatchesAfterOwnedWrite(
               lifecycleHooks[i].slot,ownedProtection
           ))return 30u+i*4u;
        if(rollbackPointerHook(
               lifecycleHooks[i].slot,lifecycleHooks[i].expected,
               lifecycleHooks[i].replacement,ownedProtection
           )!=APPLY_FAILED||
           !readU64(lifecycleHooks[i].slot,&value)||
           value!=lifecycleHooks[i].expected){
            return 31u+i*4u;
        }
    }

    u32 hudResult=testMappedScannedHud(base);
    if(hudResult)return hudResult;
    u32 distanceResult=testMappedHudDistances(base);
    if(distanceResult)return distanceResult;
    u32 alignmentResult=testMappedSurveyAlignment(base);
    if(alignmentResult)return alignmentResult;
    u32 meterResult=testMappedCargoMeter(base);
    if(meterResult)return meterResult;
    u32 visualResult=testMappedVisualWave(base);
    return visualResult?visualResult:testMappedRecipient(base);
}
