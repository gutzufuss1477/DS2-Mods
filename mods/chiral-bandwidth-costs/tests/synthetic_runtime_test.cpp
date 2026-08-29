#include "../src/chiral_bandwidth_costs.cpp"

static void writePointer(u8* base,u64 rva,u64 targetRva){
    *(u64*)(base+rva)=(u64)(base+targetRva);
}

static void writeVtable(u8* base,u64 tableRva,const u64* functions){
    for(u32 i=0;i<4u;i++)*(u64*)(base+tableRva+(u64)i*8u)=(u64)(base+functions[i]);
}

static u32 multiStringLength(const WCHAR* value){
    u32 position=0;
    while(value[position]||value[position+1u])position++;
    return position;
}

static bool allNative(const Settings* settings){
    if(!settings||!settings->enabled||settings->debugLog)return false;
    for(u32 i=0;i<SETTING_COUNT;i++)if(settings->costs[i].custom)return false;
    return true;
}

extern "C" __declspec(dllexport) u32 RunPackagedSettingsTest(){
    Settings settings;
    SettingsResult result=loadSettings(&settings);
    closeLog();
    if(result!=SETTINGS_READY||!allNative(&settings))return 2u;

    static const WCHAR valid[]=
        L"Enabled=on\0DebugLog=YES\0Postbox=0\0Bridge=999999\0HotSpringDigger=1000000\0";
    if(!parseSettingsSection(valid,multiStringLength(valid),&settings)||
       !settings.enabled||!settings.debugLog||
       !settings.costs[0].custom||settings.costs[0].value!=0u||
       !settings.costs[7].custom||settings.costs[7].value!=999999u||
       !settings.costs[10].custom||settings.costs[10].value!=1000000u)return 3u;

    static const WCHAR nativeCase[]=
        L"enabled=1\0debuglog=0\0postbox=nAtIvE\0";
    if(!parseSettingsSection(nativeCase,multiStringLength(nativeCase),&settings)||
       settings.costs[0].custom)return 4u;

    static const WCHAR duplicate[] = L"Enabled=1\0Enabled=0\0";
    static const WCHAR unknown[] = L"Enabled=1\0Generatr=10\0";
    static const WCHAR negative[] = L"Enabled=1\0Generator=-1\0";
    static const WCHAR overflow[] = L"Enabled=1\0Generator=1000001\0";
    static const WCHAR decimal[] = L"Enabled=1\0Generator=1.5\0";
    static const WCHAR plus[] = L"Enabled=1\0Generator=+1\0";
    static const WCHAR hexadecimal[] = L"Enabled=1\0Generator=0x10\0";
    static const WCHAR trailing[] = L"Enabled=1\0Generator=10 ; comment\0";
    static const WCHAR empty[] = L"Enabled=1\0Generator=\0";
    static const WCHAR missingEnabled[] = L"Generator=10\0";
    if(parseSettingsSection(duplicate,multiStringLength(duplicate),&settings)||
       parseSettingsSection(unknown,multiStringLength(unknown),&settings)||
       parseSettingsSection(negative,multiStringLength(negative),&settings)||
       parseSettingsSection(overflow,multiStringLength(overflow),&settings)||
       parseSettingsSection(decimal,multiStringLength(decimal),&settings)||
       parseSettingsSection(plus,multiStringLength(plus),&settings)||
       parseSettingsSection(hexadecimal,multiStringLength(hexadecimal),&settings)||
       parseSettingsSection(trailing,multiStringLength(trailing),&settings)||
       parseSettingsSection(empty,multiStringLength(empty),&settings)||
       parseSettingsSection(missingEnabled,multiStringLength(missingEnabled),&settings))return 5u;
    return 1u;
}

extern "C" __declspec(dllexport) u32 RunSyntheticBandwidthTest(u8* base,u32 capacity){
    if(!base||capacity<EXPECTED_IMAGE_SIZE)return 2u;

    *(u16*)base=0x5A4Du;
    *(u32*)(base+0x3Cu)=0x100u;
    *(u32*)(base+0x100u)=0x00004550u;
    *(u16*)(base+0x104u)=0x8664u;
    *(u32*)(base+0x108u)=EXPECTED_TIMESTAMP;
    *(u16*)(base+0x114u)=0x00F0u;
    *(u16*)(base+0x118u)=0x020Bu;
    *(u32*)(base+0x150u)=EXPECTED_IMAGE_SIZE;
    u32 timestamp=0,imageSize=0;
    if(!readBuildMetadata(base,&timestamp,&imageSize)||
       timestamp!=EXPECTED_TIMESTAMP||imageSize!=EXPECTED_IMAGE_SIZE)return 3u;

    memcpy(base+RVA_CAPACITY_CHECK,EXPECTED_CAPACITY_CHECK,sizeof(EXPECTED_CAPACITY_CHECK));
    memcpy(base+RVA_ACCOUNTING_ADD,EXPECTED_ACCOUNTING_ADD,sizeof(EXPECTED_ACCOUNTING_ADD));
    memcpy(base+RVA_ACCOUNTING_VALUE,EXPECTED_ACCOUNTING_VALUE,sizeof(EXPECTED_ACCOUNTING_VALUE));
    memcpy(base+RVA_ACCOUNTING_SUBTRACT,EXPECTED_ACCOUNTING_SUBTRACT,sizeof(EXPECTED_ACCOUNTING_SUBTRACT));
    memcpy(base+RVA_UI_COST,EXPECTED_UI_COST,sizeof(EXPECTED_UI_COST));
    if(!preflightCostCode(base,capacity))return 4u;
    base[RVA_ACCOUNTING_SUBTRACT]^=1u;
    if(preflightCostCode(base,capacity))return 5u;
    base[RVA_ACCOUNTING_SUBTRACT]^=1u;

    writeVtable(base,RVA_MANAGER_VTABLE,MANAGER_VFUNCS);
    writeVtable(base,RVA_FACILITY_VTABLE,FACILITY_VFUNCS);
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        writeVtable(base,TARGETS[i].configVtableRva,TARGETS[i].vfuncs);
    }
    if(!validateAllVtables(base))return 6u;

    const u64 rootRva=0x07000000ull;
    const u64 managerRva=0x07001000ull;
    const u64 objectBaseRva=0x07002000ull;
    const u64 objectStride=0x2000ull;
    memset(base+rootRva,0,0x2000u+PATCH_TARGET_COUNT*objectStride);
    writePointer(base,RVA_GLOBAL_ROOT,rootRva);
    writePointer(base,rootRva+OFF_ROOT_MANAGER,managerRva);
    writePointer(base,managerRva,RVA_MANAGER_VTABLE);

    Observation observations[sizeof(TARGETS)/sizeof(TARGETS[0])];
    memset(observations,0,sizeof(observations));
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u64 facilityRva=objectBaseRva+(u64)i*objectStride;
        u64 configRva=facilityRva+0x1000u;
        writePointer(base,managerRva+TARGETS[i].managerOffset,facilityRva);
        writePointer(base,facilityRva,RVA_FACILITY_VTABLE);
        writePointer(base,facilityRva+OFF_FACILITY_CONFIG,configRva);
        writePointer(base,configRva,TARGETS[i].configVtableRva);
        u32 settingIndex=TARGETS[i].settingIndex;
        *(u32*)(base+configRva+OFF_QPID_COST)=SETTING_DEFINITIONS[settingIndex].nativeCost;
        observations[i].manager=base+managerRva;
        observations[i].facility=base+facilityRva;
        observations[i].config=base+configRva;
        observations[i].stablePolls=2u;
    }

    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u8* facility=0;
        u8* config=0;
        if(resolveTarget(base,i,&facility,&config)!=RESOLVE_READY||
           facility!=observations[i].facility||config!=observations[i].config)return 7u;
    }
    if(observations[7].config==observations[8].config)return 30u;

    Settings settings;
    memset(&settings,0,sizeof(settings));
    settings.enabled=true;
    for(u32 i=0;i<SETTING_COUNT;i++){
        settings.costs[i].custom=true;
        settings.costs[i].value=10000u+i*137u;
    }
    if(!hasRequestedChanges(&settings))return 8u;
    u32 failedTarget=PATCH_TARGET_COUNT;
    observations[1].stablePolls=1u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_NOT_READY||failedTarget!=1u)return 25u;
    if(*(u32*)(observations[0].config+OFF_QPID_COST)!=SETTING_DEFINITIONS[0].nativeCost)return 26u;
    observations[1].stablePolls=2u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_READY)return 9u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_READY)return 10u;
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u32 settingIndex=TARGETS[i].settingIndex;
        if(*(u32*)(observations[i].config+OFF_QPID_COST)!=settings.costs[settingIndex].value)return 11u;
    }

    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u32 settingIndex=TARGETS[i].settingIndex;
        *(u32*)(observations[i].config+OFF_QPID_COST)=SETTING_DEFINITIONS[settingIndex].nativeCost;
    }
    *(u32*)(observations[0].config+OFF_QPID_COST)=777777u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_CONFLICT||failedTarget!=0u)return 12u;
    if(*(u32*)(observations[1].config+OFF_QPID_COST)!=SETTING_DEFINITIONS[1].nativeCost)return 13u;
    *(u32*)(observations[0].config+OFF_QPID_COST)=0u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_NOT_READY||failedTarget!=0u)return 14u;

    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u32 settingIndex=TARGETS[i].settingIndex;
        *(u32*)(observations[i].config+OFF_QPID_COST)=SETTING_DEFINITIONS[settingIndex].nativeCost;
    }
    const u64 secondManagerRva=0x07100000ull;
    writePointer(base,secondManagerRva,RVA_MANAGER_VTABLE);
    g_testManagerSlot=(u64*)(base+rootRva+OFF_ROOT_MANAGER);
    g_testManagerReplacement=(u64)(base+secondManagerRva);
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_NOT_READY)return 34u;
    if(*(u32*)(observations[0].config+OFF_QPID_COST)!=SETTING_DEFINITIONS[0].nativeCost)return 35u;
    writePointer(base,rootRva+OFF_ROOT_MANAGER,managerRva);

    g_testFieldRace=observations[0].config+OFF_QPID_COST;
    g_testFieldRaceValue=777777u;
    ApplyResult fieldRaceResult=applyBatch(base,&settings,observations,&failedTarget);
    if(fieldRaceResult!=APPLY_CONFLICT)return 45u;
    if(failedTarget!=0u)return 49u;
    if(*(u32*)(observations[0].config+OFF_QPID_COST)!=777777u)return 46u;
    *(u32*)(observations[0].config+OFF_QPID_COST)=SETTING_DEFINITIONS[0].nativeCost;

    g_testFailAfterWrites=1;
    g_testRollbackRace=observations[0].config+OFF_QPID_COST;
    g_testRollbackRaceValue=777777u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_CONFLICT)return 47u;
    g_testFailAfterWrites=-1;
    if(*(u32*)(observations[0].config+OFF_QPID_COST)!=777777u)return 48u;
    *(u32*)(observations[0].config+OFF_QPID_COST)=SETTING_DEFINITIONS[0].nativeCost;

    DWORD faultPrevious=0;
    if(!VirtualProtect(observations[0].config,0x1000u,PAGE_READONLY,&faultPrevious))return 27u;
    g_testFailAfterWrites=1;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_FAILED)return 15u;
    g_testFailAfterWrites=-1;
    MEMORY_BASIC_INFORMATION_X64 faultInfo;
    if(VirtualQuery(observations[0].config,&faultInfo,sizeof(faultInfo))<sizeof(faultInfo)||
       faultInfo.Protect!=PAGE_READONLY)return 28u;
    DWORD faultIgnored=0;
    if(!VirtualProtect(observations[0].config,0x1000u,PAGE_READWRITE,&faultIgnored))return 29u;
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u32 settingIndex=TARGETS[i].settingIndex;
        if(*(u32*)(observations[i].config+OFF_QPID_COST)!=SETTING_DEFINITIONS[settingIndex].nativeCost)return 16u;
    }

    g_testFailRestoreCalls=2;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_CRITICAL)return 36u;
    if(g_testFailRestoreCalls!=0)return 37u;
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u32 settingIndex=TARGETS[i].settingIndex;
        if(*(u32*)(observations[i].config+OFF_QPID_COST)!=SETTING_DEFINITIONS[settingIndex].nativeCost)return 38u;
    }

    ProtectionPage protectionRace[1];
    u32 protectionRaceCount=0,pageSize=0;
    if(!getPageSize(&pageSize)||!addProtectionPage(
        protectionRace,&protectionRaceCount,1u,observations[0].config+OFF_QPID_COST,pageSize
    ))return 39u;
    g_testProtectionRacePage=protectionRace[0].address;
    g_testProtectionRaceValue=PAGE_READONLY;
    if(makePagesWritable(protectionRace,protectionRaceCount,pageSize))return 41u;
    if(restorePages(protectionRace,protectionRaceCount,pageSize))return 42u;
    MEMORY_BASIC_INFORMATION_X64 raceInfo;
    if(VirtualQuery(observations[0].config,&raceInfo,sizeof(raceInfo))<sizeof(raceInfo)||
       raceInfo.Protect!=PAGE_READONLY)return 43u;
    DWORD raceIgnored=0;
    if(!VirtualProtect(observations[0].config,0x1000u,PAGE_READWRITE,&raceIgnored))return 44u;

    u8* protectedConfig=observations[4].config;
    DWORD previous=0;
    if(!VirtualProtect(protectedConfig,0x1000u,PAGE_READONLY,&previous))return 17u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_READY)return 18u;
    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(protectedConfig,&info,sizeof(info))<sizeof(info)||info.Protect!=PAGE_READONLY)return 19u;
    DWORD ignored=0;
    if(!VirtualProtect(protectedConfig,0x1000u,PAGE_READWRITE,&ignored))return 20u;

    const u32 swapIndex=2u;
    u64 replacementFacilityRva=objectBaseRva+(u64)PATCH_TARGET_COUNT*objectStride;
    u64 replacementConfigRva=replacementFacilityRva+0x1000u;
    writePointer(base,replacementFacilityRva,RVA_FACILITY_VTABLE);
    writePointer(base,replacementFacilityRva+OFF_FACILITY_CONFIG,replacementConfigRva);
    writePointer(base,replacementConfigRva,TARGETS[swapIndex].configVtableRva);
    *(u32*)(base+replacementConfigRva+OFF_QPID_COST)=SETTING_DEFINITIONS[TARGETS[swapIndex].settingIndex].nativeCost;
    writePointer(base,managerRva+TARGETS[swapIndex].managerOffset,replacementFacilityRva);
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_NOT_READY||failedTarget!=swapIndex)return 21u;
    observations[swapIndex].facility=base+replacementFacilityRva;
    observations[swapIndex].config=base+replacementConfigRva;
    observations[swapIndex].stablePolls=2u;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_READY||
       *(u32*)(base+replacementConfigRva+OFF_QPID_COST)!=settings.costs[TARGETS[swapIndex].settingIndex].value)return 22u;

    const u32 bridge30Index=7u;
    const u32 bridge30AliasIndex=8u;
    writePointer(base,managerRva+TARGETS[bridge30Index].managerOffset,
        (u64)(observations[bridge30Index].facility-base));
    writePointer(base,managerRva+TARGETS[bridge30Index].alternateManagerOffset,
        (u64)(observations[bridge30Index].facility-base));
    observations[bridge30AliasIndex]=observations[bridge30Index];
    *(u32*)(observations[bridge30Index].config+OFF_QPID_COST)=SETTING_DEFINITIONS[7].nativeCost;
    g_testFailAfterWrites=1;
    if(applyBatch(base,&settings,observations,&failedTarget)!=APPLY_READY)return 31u;
    g_testFailAfterWrites=-1;
    if(*(u32*)(observations[bridge30Index].config+OFF_QPID_COST)!=settings.costs[7].value)return 32u;

    writePointer(base,managerRva+TARGETS[bridge30Index].managerOffset,replacementFacilityRva);
    u8* fallbackFacility=0;
    u8* fallbackConfig=0;
    if(resolveTarget(base,bridge30Index,&fallbackFacility,&fallbackConfig)!=RESOLVE_INVALID)return 33u;

    *(u64*)(base+managerRva+TARGETS[bridge30Index].managerOffset)=0u;
    fallbackFacility=0;
    fallbackConfig=0;
    if(resolveTarget(base,bridge30Index,&fallbackFacility,&fallbackConfig)!=RESOLVE_READY||
       fallbackFacility!=observations[bridge30Index].facility||
       fallbackConfig!=observations[bridge30Index].config)return 23u;

    *(u64*)observations[0].config=0u;
    u8* invalidFacility=0;
    u8* invalidConfig=0;
    if(resolveTarget(base,0u,&invalidFacility,&invalidConfig)!=RESOLVE_INVALID)return 24u;
    return 1u;
}
