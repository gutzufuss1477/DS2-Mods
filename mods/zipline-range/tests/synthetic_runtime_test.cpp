#include "../src/zipline_range.cpp"

static void writePointer(u8* base,u64 rva,u64 targetRva){
    *(u64*)(base+rva)=(u64)(base+targetRva);
}

static void writeVtable(u8* base,u64 tableRva,const u64* functions){
    for(u32 i=0;i<4u;i++)*(u64*)(base+tableRva+(u64)i*8u)=(u64)(base+functions[i]);
}

extern "C" __declspec(dllexport) u32 RunPackagedSettingsTest(){
    Settings settings;
    SettingsResult result=loadSettings(&settings);
    closeLog();
    if(result!=SETTINGS_READY)return 1u;
    if(settings.targetBits[0]!=floatBits(601.0f)||
       settings.targetBits[1]!=floatBits(601.0f)||
       settings.targetBits[2]!=floatBits(601.0f)||
       settings.targetBits[3]!=floatBits(601.0f)||
       settings.targetBits[4]!=floatBits(601.0f))return 2u;
    if(settings.distanceRealizeBits!=floatBits(650.0f)||
       settings.distanceUnrealizeBits!=floatBits(666.0f)||
       settings.distanceOpenBits!=floatBits(1000.0f)||
       settings.forceRealizeRadiusBits!=floatBits(650.0f)||
       settings.uiVisibleDistanceBits!=floatBits(650.0f)||
       settings.speedMultiplier!=2.0f||
       settings.travelSpeedBits!=floatBits(32.0f)||
       settings.ignoreConnectionObstacles)return 3u;
    float parsed=0.0f;
    if(!parseDecimal(L"0.25",MIN_SPEED_MULTIPLIER,MAX_SPEED_MULTIPLIER,&parsed)||
       parsed!=0.25f||
       !parseDecimal(L"10.0",MIN_SPEED_MULTIPLIER,MAX_SPEED_MULTIPLIER,&parsed)||
       parsed!=10.0f||
       parseDecimal(L"0.249",MIN_SPEED_MULTIPLIER,MAX_SPEED_MULTIPLIER,&parsed)||
       parseDecimal(L"10.001",MIN_SPEED_MULTIPLIER,MAX_SPEED_MULTIPLIER,&parsed))return 4u;
    Settings unequal=settings;
    unequal.displayedRange[0]=500.0f;
    unequal.displayedRange[1]=600.0f;
    if(!deriveRangeSettings(&unequal)||
       unequal.targetBits[0]!=floatBits(501.0f)||
       unequal.targetBits[1]!=floatBits(601.0f)||
       unequal.targetBits[2]!=floatBits(601.0f)||
       unequal.targetBits[3]!=floatBits(601.0f)||
       unequal.targetBits[4]!=floatBits(601.0f))return 5u;
    unequal.displayedRange[0]=700.0f;
    if(deriveRangeSettings(&unequal))return 6u;
    return 0u;
}

extern "C" __declspec(dllexport) u32 RunSyntheticZiplineTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 1u;

    memcpy(base+RVA_RANGE_GETTER,EXPECTED_RANGE_GETTER,sizeof(EXPECTED_RANGE_GETTER));
    memcpy(base+RVA_CANDIDATE_RANGE,EXPECTED_CANDIDATE_RANGE,sizeof(EXPECTED_CANDIDATE_RANGE));
    memcpy(base+RVA_FINAL_RANGE,EXPECTED_FINAL_RANGE,sizeof(EXPECTED_FINAL_RANGE));
    memcpy(base+RVA_CONSTRUCTOR_DEFAULTS,EXPECTED_CONSTRUCTOR_DEFAULTS,sizeof(EXPECTED_CONSTRUCTOR_DEFAULTS));
    memcpy(base+RVA_REALIZATION_USAGE,EXPECTED_REALIZATION_USAGE,sizeof(EXPECTED_REALIZATION_USAGE));
    memcpy(base+RVA_DISTANCE_OPEN_DEFAULT,EXPECTED_DISTANCE_OPEN_DEFAULT,sizeof(EXPECTED_DISTANCE_OPEN_DEFAULT));
    memcpy(base+RVA_UI_DISTANCE_USAGE,EXPECTED_UI_DISTANCE_USAGE,sizeof(EXPECTED_UI_DISTANCE_USAGE));
    memcpy(
        base+RVA_TRAVEL_SPEED_CONTEXT,
        EXPECTED_TRAVEL_SPEED_CONTEXT,
        sizeof(EXPECTED_TRAVEL_SPEED_CONTEXT)
    );
    *(u32*)(base+RVA_NATIVE_TRAVEL_SPEED)=floatBits(NATIVE_TRAVEL_SPEED);
    memcpy(
        base+RVA_FIXED_RIDEABLE_RANGE_CONTEXT,
        EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT,
        sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT)
    );
    memcpy(
        base+RVA_RIDEABLE_FINAL_RANGE_GATE,
        EXPECTED_RIDEABLE_FINAL_RANGE_GATE,
        sizeof(EXPECTED_RIDEABLE_FINAL_RANGE_GATE)
    );
    memcpy(
        base+RVA_CONNECTION_OBSTACLE_RESULT,
        EXPECTED_CONNECTION_OBSTACLE_RESULT,
        sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)
    );
    if(!preflightCode(base,capacity,true,true))return 2u;
    base[RVA_TRAVEL_SPEED_CONTEXT]^=0x01u;
    if(!preflightCode(base,capacity,true,false))return 2u;
    base[RVA_TRAVEL_SPEED_CONTEXT]^=0x01u;

    writeVtable(base,RVA_MANAGER_VTABLE,MANAGER_VFUNCS);
    writeVtable(base,RVA_FACILITY_VTABLE,FACILITY_VFUNCS);
    writeVtable(base,RVA_ZIPLINE_CONFIG_VTABLE,ZIPLINE_CONFIG_VFUNCS);
    writeVtable(base,RVA_ENTITY_RESOURCE_VTABLE,ENTITY_RESOURCE_VFUNCS);
    writeVtable(base,RVA_IMAGINARY_COMPONENT_VTABLE,IMAGINARY_COMPONENT_VFUNCS);
    writeVtable(base,RVA_UI_RESIDENT_VTABLE,UI_RESIDENT_VFUNCS);
    writeVtable(base,RVA_UI_GAME_PARAMS_VTABLE,UI_GAME_PARAMS_VFUNCS);
    if(!validateVtableEntries(base,RVA_MANAGER_VTABLE,MANAGER_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_FACILITY_VTABLE,FACILITY_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_ZIPLINE_CONFIG_VTABLE,ZIPLINE_CONFIG_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_ENTITY_RESOURCE_VTABLE,ENTITY_RESOURCE_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_IMAGINARY_COMPONENT_VTABLE,IMAGINARY_COMPONENT_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_UI_RESIDENT_VTABLE,UI_RESIDENT_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_UI_GAME_PARAMS_VTABLE,UI_GAME_PARAMS_VFUNCS,4u))return 3u;

    const u64 rootRva=0x07000000ull;
    const u64 managerRva=0x07001000ull;
    const u64 facilityRva=0x07002000ull;
    const u64 configRva=0x07003000ull;
    const u64 entityRva=0x07005000ull;
    const u64 entriesRva=0x07006000ull;
    const u64 componentRva=0x07007000ull;
    const u64 uiCacheRva=0x07008000ull;
    const u64 uiResidentRva=0x07009000ull;
    const u64 uiParametersRva=0x0700A000ull;

    memset(base+rootRva,0,0xB000u);
    writePointer(base,RVA_GLOBAL_ROOT,rootRva);
    writePointer(base,rootRva+OFF_ROOT_MANAGER_CONFIG,managerRva);
    writePointer(base,managerRva,RVA_MANAGER_VTABLE);
    writePointer(base,managerRva+OFF_MANAGER_ZIPLINE,facilityRva);
    writePointer(base,facilityRva,RVA_FACILITY_VTABLE);
    writePointer(base,facilityRva+OFF_FACILITY_CONFIG,configRva);
    writePointer(base,facilityRva+OFF_FACILITY_IMAGINARY_ENTITY,entityRva);
    writePointer(base,configRva,RVA_ZIPLINE_CONFIG_VTABLE);
    *(u32*)(base+configRva+OFF_MAX_LEVEL)=SERIALIZED_MAX_LEVEL;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        *(u32*)(base+configRva+OFF_PASSABLE_LEVEL1+(u64)i*4u)=floatBits(CONSTRUCTOR_LEVELS[i]);
    }
    *(u32*)(base+configRva+OFF_DISTANCE_REALIZE)=floatBits(CONSTRUCTOR_DISTANCE_REALIZE);
    *(u32*)(base+configRva+OFF_DISTANCE_UNREALIZE)=floatBits(CONSTRUCTOR_DISTANCE_UNREALIZE);
    *(u32*)(base+configRva+OFF_DISTANCE_OPEN)=floatBits(NATIVE_DISTANCE_OPEN);

    writePointer(base,entityRva,RVA_ENTITY_RESOURCE_VTABLE);
    *(u32*)(base+entityRva+OFF_ENTITY_COMPONENT_ARRAY)=1u;
    *(u32*)(base+entityRva+OFF_ENTITY_COMPONENT_ARRAY+4u)=1u;
    writePointer(base,entityRva+OFF_ENTITY_COMPONENT_ARRAY+8u,entriesRva);
    writePointer(base,entriesRva,componentRva);
    writePointer(base,componentRva,RVA_IMAGINARY_COMPONENT_VTABLE);
    *(u32*)(base+componentRva+OFF_FORCE_REALIZE_RADIUS)=floatBits(NATIVE_FORCE_REALIZE_RADIUS);

    writePointer(base,RVA_UI_CACHE_ROOT,uiCacheRva);
    writePointer(base,uiCacheRva+OFF_UI_CACHE_RESIDENT,uiResidentRva);
    writePointer(base,uiResidentRva,RVA_UI_RESIDENT_VTABLE);
    writePointer(base,uiResidentRva+OFF_UI_RESIDENT_GAME_PARAMS,uiParametersRva);
    writePointer(base,uiParametersRva,RVA_UI_GAME_PARAMS_VTABLE);
    *(u32*)(base+uiParametersRva+OFF_UI_ZIPLINE_VISIBLE_DISTANCE)=floatBits(NATIVE_UI_ZIPLINE_VISIBLE_DISTANCE);

    u8* config=0;
    u8* facility=0;
    u8* component=0;
    u8* canonicalFacility=0;
    u8* canonicalComponent=0;
    u8* uiParameters=0;
    if(resolveZiplineConfig(base,&config,&facility)!=RESOLVE_READY||config!=base+configRva||facility!=base+facilityRva)return 4u;
    if(resolveImaginaryComponent(base,facility,&component)!=RESOLVE_READY||component!=base+componentRva)return 5u;
    if(resolveCanonicalImaginaryComponent(base,&canonicalFacility,&canonicalComponent)!=RESOLVE_READY||
       canonicalFacility!=facility||canonicalComponent!=component)return 6u;
    if(resolveUiGameDesign(base,&uiParameters)!=RESOLVE_READY||uiParameters!=base+uiParametersRva)return 7u;

    Settings settings;
    settings.displayedRange[0]=600.0f;
    settings.displayedRange[1]=600.0f;
    settings.targetBits[0]=floatBits(601.0f);
    settings.targetBits[1]=floatBits(601.0f);
    settings.targetBits[2]=floatBits(601.0f);
    settings.targetBits[3]=floatBits(601.0f);
    settings.targetBits[4]=floatBits(601.0f);
    settings.distanceRealizeBits=floatBits(650.0f);
    settings.distanceUnrealizeBits=floatBits(666.0f);
    settings.distanceOpenBits=floatBits(1000.0f);
    settings.forceRealizeRadiusBits=floatBits(650.0f);
    settings.uiVisibleDistanceBits=floatBits(650.0f);
    settings.speedMultiplier=2.0f;
    settings.travelSpeedBits=floatBits(32.0f);
    settings.ignoreConnectionObstacles=false;

    *(u32*)(base+componentRva+OFF_FORCE_REALIZE_RADIUS)=0u;
    if(applyForceRadius(base,facility,component,&settings)!=APPLY_NOT_READY)return 8u;
    *(u32*)(base+componentRva+OFF_FORCE_REALIZE_RADIUS)=floatBits(NATIVE_FORCE_REALIZE_RADIUS);
    *(u32*)(base+uiParametersRva+OFF_UI_ZIPLINE_VISIBLE_DISTANCE)=0u;
    if(applyUiVisibleDistance(base,uiParameters,&settings)!=APPLY_NOT_READY)return 9u;
    *(u32*)(base+uiParametersRva+OFF_UI_ZIPLINE_VISIBLE_DISTANCE)=floatBits(NATIVE_UI_ZIPLINE_VISIBLE_DISTANCE);

    *(u32*)(base+configRva+OFF_MAX_LEVEL)=2u;
    if(applyRanges(base,config,&settings)!=APPLY_CONFLICT)return 10u;
    *(u32*)(base+configRva+OFF_MAX_LEVEL)=4u;
    if(applyRanges(base,config,&settings)!=APPLY_CONFLICT)return 10u;
    *(u32*)(base+configRva+OFF_MAX_LEVEL)=1u;
    if(applyRanges(base,config,&settings)!=APPLY_NOT_READY)return 10u;
    *(u32*)(base+configRva+OFF_MAX_LEVEL)=SERIALIZED_MAX_LEVEL;
    if(applyRanges(base,config,&settings)!=APPLY_NOT_READY)return 10u;
    *(u32*)(base+configRva+OFF_DISTANCE_REALIZE)=floatBits(NATIVE_DISTANCE_REALIZE);
    *(u32*)(base+configRva+OFF_DISTANCE_UNREALIZE)=floatBits(NATIVE_DISTANCE_UNREALIZE);
    if(applyRanges(base,config,&settings)!=APPLY_READY)return 11u;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        if(*(u32*)(base+configRva+OFF_PASSABLE_LEVEL1+(u64)i*4u)!=settings.targetBits[i])return 12u;
    }

    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        *(u32*)(base+configRva+OFF_PASSABLE_LEVEL1+(u64)i*4u)=floatBits(NATIVE_LEVELS[i]);
    }
    *(u32*)(base+configRva+OFF_DISTANCE_REALIZE)=floatBits(NATIVE_DISTANCE_REALIZE);
    *(u32*)(base+configRva+OFF_DISTANCE_UNREALIZE)=floatBits(NATIVE_DISTANCE_UNREALIZE);
    if(applyRanges(base,config,&settings)!=APPLY_READY)return 13u;
    if(applyRanges(base,config,&settings)!=APPLY_READY)return 14u;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        if(*(u32*)(base+configRva+OFF_PASSABLE_LEVEL1+(u64)i*4u)!=settings.targetBits[i])return 15u;
    }
    if(*(u32*)(base+configRva+OFF_DISTANCE_REALIZE)!=settings.distanceRealizeBits||
       *(u32*)(base+configRva+OFF_DISTANCE_UNREALIZE)!=settings.distanceUnrealizeBits||
       *(u32*)(base+configRva+OFF_DISTANCE_OPEN)!=settings.distanceOpenBits)return 16u;

    if(applyForceRadius(base,facility,component,&settings)!=APPLY_READY||
       applyForceRadius(base,facility,component,&settings)!=APPLY_READY)return 17u;
    if(*(u32*)(base+componentRva+OFF_FORCE_REALIZE_RADIUS)!=settings.forceRealizeRadiusBits)return 18u;

    const u64 secondFacilityRva=0x0700B000ull;
    const u64 secondConfigRva=0x0700C000ull;
    const u64 secondEntityRva=0x0700E000ull;
    const u64 secondEntriesRva=0x0700F000ull;
    const u64 secondComponentRva=0x07010000ull;
    memset(base+secondFacilityRva,0,0x6000u);
    writePointer(base,secondFacilityRva,RVA_FACILITY_VTABLE);
    writePointer(base,secondFacilityRva+OFF_FACILITY_CONFIG,secondConfigRva);
    writePointer(base,secondFacilityRva+OFF_FACILITY_IMAGINARY_ENTITY,secondEntityRva);
    writePointer(base,secondConfigRva,RVA_ZIPLINE_CONFIG_VTABLE);
    writePointer(base,secondEntityRva,RVA_ENTITY_RESOURCE_VTABLE);
    *(u32*)(base+secondEntityRva+OFF_ENTITY_COMPONENT_ARRAY)=1u;
    *(u32*)(base+secondEntityRva+OFF_ENTITY_COMPONENT_ARRAY+4u)=1u;
    writePointer(base,secondEntityRva+OFF_ENTITY_COMPONENT_ARRAY+8u,secondEntriesRva);
    writePointer(base,secondEntriesRva,secondComponentRva);
    writePointer(base,secondComponentRva,RVA_IMAGINARY_COMPONENT_VTABLE);
    *(u32*)(base+secondComponentRva+OFF_FORCE_REALIZE_RADIUS)=floatBits(NATIVE_FORCE_REALIZE_RADIUS);
    writePointer(base,managerRva+OFF_MANAGER_ZIPLINE,secondFacilityRva);
    if(applyForceRadius(base,facility,component,&settings)!=APPLY_NOT_READY)return 19u;
    writePointer(base,managerRva+OFF_MANAGER_ZIPLINE,facilityRva);

    if(applyUiVisibleDistance(base,uiParameters,&settings)!=APPLY_READY||
       applyUiVisibleDistance(base,uiParameters,&settings)!=APPLY_READY)return 20u;
    if(*(u32*)(base+uiParametersRva+OFF_UI_ZIPLINE_VISIBLE_DISTANCE)!=settings.uiVisibleDistanceBits)return 21u;

    *(u32*)(base+uiParametersRva+OFF_UI_ZIPLINE_VISIBLE_DISTANCE)=floatBits(777.0f);
    if(applyUiVisibleDistance(base,uiParameters,&settings)!=APPLY_CONFLICT)return 22u;
    if(applyFixedRideableRangePatch(base)!=APPLY_READY||
       applyFixedRideableRangePatch(base)!=APPLY_READY)return 23u;
    if(!fixedRideableRangePatchMatches(base)||
       !bytesEqual(
           base+RVA_FIXED_RIDEABLE_RANGE_BLOCK,
           PATCHED_FIXED_RIDEABLE_RANGE_BLOCK,
           (u32)sizeof(PATCHED_FIXED_RIDEABLE_RANGE_BLOCK)
       ))return 24u;
    Settings obstacleSettings=settings;
    obstacleSettings.ignoreConnectionObstacles=true;
    if(applyConnectionObstacleSetting(base,&obstacleSettings)!=APPLY_READY||
       applyConnectionObstacleSetting(base,&obstacleSettings)!=APPLY_READY)return 25u;
    if(!connectionObstaclePatchMatches(base+RVA_CONNECTION_OBSTACLE_RESULT))return 26u;

    const u64 scratchSpeedCellRva=0x07011000ull;
    *(u32*)(base+scratchSpeedCellRva)=settings.travelSpeedBits;
    Settings nativeSpeedSettings=settings;
    nativeSpeedSettings.speedMultiplier=1.0f;
    nativeSpeedSettings.travelSpeedBits=floatBits(NATIVE_TRAVEL_SPEED);
    if(installTravelSpeedPatch(base,capacity,&nativeSpeedSettings)!=APPLY_READY||
       g_travelSpeedCell)return 27u;

    *(u32*)(base+RVA_NATIVE_TRAVEL_SPEED)=floatBits(20.0f);
    if(applyTravelSpeedPatchWithCell(base,base+scratchSpeedCellRva,settings.travelSpeedBits)!=APPLY_CONFLICT)return 27u;
    *(u32*)(base+RVA_NATIVE_TRAVEL_SPEED)=floatBits(NATIVE_TRAVEL_SPEED);

    if(installTravelSpeedPatch(base,capacity,&settings)!=APPLY_READY||
       installTravelSpeedPatch(base,capacity,&settings)!=APPLY_READY)return 27u;
    if(!g_travelSpeedCell||
       !travelSpeedPatchMatches(base,g_travelSpeedCell,settings.travelSpeedBits))return 28u;
    u32 patchedDisplacement=0;
    if(!readU32(base+RVA_TRAVEL_SPEED_DISPLACEMENT,&patchedDisplacement))return 29u;
    const u8* resolvedSpeedCell=base+RVA_TRAVEL_SPEED_LOAD+8u+(s32)patchedDisplacement;
    if(resolvedSpeedCell!=g_travelSpeedCell)return 30u;
    if(rollbackTravelSpeedPatch(base,patchedDisplacement,PAGE_READWRITE)!=APPLY_FAILED)return 31u;
    if(!VirtualFree(g_travelSpeedCell,0,MEM_RELEASE))return 32u;
    g_travelSpeedCell=0;
    return 0u;
}
