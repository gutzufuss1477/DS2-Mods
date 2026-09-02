#include "../src/coffin_board_all_terrain_speed.cpp"

static const SIZE_T TEST_OFF_STEERING_HI_SPEED_SCALE = 0xA4u;
static const SIZE_T TEST_OFF_STEERING_BOOST_SCALE = 0xA8u;
static void* g_syntheticGroupEntries[COFFIN_PHYSICS_OBJECT_INDEX + 1u];
static BYTE g_syntheticPhysicsRtti[0x48];
static void* g_syntheticPhysicsVtable[1];
static const char g_syntheticPhysicsType[] = "DSPhysicsCoffinResource";
static BYTE g_syntheticStreamingSystem[OFF_STREAMING_GROUP_TABLE + sizeof(void*)];
static BYTE g_syntheticGroupTable[
    (COFFIN_PHYSICS_GROUP_ID + 1u) * STREAMING_GROUP_ENTRY_STRIDE
];
static BYTE g_syntheticLoadedGroup[OFF_LOADED_GROUP_OBJECTS + sizeof(RawArray)];
static UINT32 g_syntheticGroupDescriptor = COFFIN_PHYSICS_GROUP_ID;
__declspec(align(4)) static volatile LONG g_syntheticDriveFactorBits = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringSlopeBits = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringSequence = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringSpeedBits = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringPreClampBits = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringFinalBits = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringRawBits = 0;
__declspec(align(4)) static volatile LONG g_syntheticSteeringScaleBits = 0;
static UINT32 g_syntheticListenerAdds = 0;
static UINT32 g_syntheticListenerRemoves = 0;
static bool g_syntheticCallbackActive = false;
static bool g_syntheticRemovedInsideCallback = false;
static void* g_syntheticListenerObject = 0;

static void __fastcall synthetic_add_listener(void*, void* listener) {
    ++g_syntheticListenerAdds;
    g_syntheticListenerObject = listener;
}

static void __fastcall synthetic_remove_listener(void*, void* listener) {
    ++g_syntheticListenerRemoves;
    if (g_syntheticCallbackActive) g_syntheticRemovedInsideCallback = true;
    if (g_syntheticListenerObject == listener) g_syntheticListenerObject = 0;
}

static void* __fastcall synthetic_physics_get_rtti(void*) {
    return g_syntheticPhysicsRtti;
}

extern "C" __declspec(dllexport) UINT32 RunSyntheticCoffinSpeedTest() {
    BYTE resource[0xE0];
    BYTE riderConfig[0x80];
    memset(resource, 0, sizeof(resource));
    memset(riderConfig, 0, sizeof(riderConfig));

    // Production static defaults are the behavior of absent final-INI keys.
    // Prove that all removed test features begin native/inactive before any
    // synthetic scenario changes the globals below.
    if (g_steeringAnglePercent != 100 ||
        g_steeringResponsePercent != 100 || g_wetGripPercent != 100 ||
        g_speedTelemetry != 0 || g_steeringHookInstalled != 0 ||
        g_driveTelemetryBits != 0) return 73u;

    g_normalPercent = 100;
    g_boostPercent = 100;
    g_scaleWaterCaps = 0;
    g_driveForcePercent = 100;
    g_gearRatioPercent = 100;
    g_accelerationPercent = 100;
    g_steeringAnglePercent = 100;
    g_steeringResponsePercent = 100;
    g_wetGripPercent = 100;
    g_patchStandardSpeed = 0;
    g_raiseSlipThreshold = 1;
    g_physicsReady = 0;
    g_standardReady = 0;
    g_complete = 0;
    g_seenPhysicsResource = 0;
    g_seenRideConfig = riderConfig;
    g_steeringBaseline = 0.0f;
    g_lastAppliedSteering = 0.0f;
    g_wetGripBaseline = 0.0f;
    g_lastAppliedWetGrip = 0.0f;
    g_driveHookInstalled = 0;
    g_driveFactorBits = 0;
    g_steeringHookInstalled = 0;
    g_steeringSlopeBits = 0;
    g_steeringSampleSequence = 0;
    g_steeringSampleSpeedBits = 0;
    g_steeringSamplePreClampBits = 0;
    g_steeringSampleFinalBits = 0;
    g_steeringSampleRawBits = 0;
    g_steeringSampleScaleBits = 0;

    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + TEST_OFF_STEERING_HI_SPEED_SCALE) = 1.0f;
    *(float*)(resource + TEST_OFF_STEERING_BOOST_SCALE) = 1.0f;
    *(float*)(resource + OFF_WET_SIDE_GRIP) = 0.09f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    *(float*)(riderConfig + OFF_STANDARD_SPEED) = 40.0f;

    if (!patch_coffin_physics(resource)) return 1u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 60.0f) return 2u;
    if (*(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 80.0f) return 3u;
    if (*(float*)(resource + OFF_SLIP_SPEED) != 60.0f) return 4u;
    if (*(float*)(riderConfig + OFF_STANDARD_SPEED) != 40.0f) return 5u;

    // The rider-balance write remains independently testable, but is
    // intentionally disabled in the packaged safe defaults.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    *(float*)(riderConfig + OFF_STANDARD_SPEED) = 40.0f;
    g_patchStandardSpeed = 1;
    g_raiseSlipThreshold = 1;
    g_standardReady = 0;
    if (!patch_coffin_physics(resource)) return 6u;
    if (*(float*)(resource + OFF_SLIP_SPEED) != 60.0f ||
        *(float*)(riderConfig + OFF_STANDARD_SPEED) != 60.0f) return 7u;

    // A faster compatible value is preserved rather than reduced.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 75.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 95.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 75.0f;
    g_seenRideConfig = 0;
    if (!patch_coffin_physics(resource)) return 8u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 75.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 95.0f) return 9u;

    // A conflicting in-between edit fails closed and remains untouched.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 50.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 70.0f;
    if (patch_coffin_physics(resource)) return 10u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 50.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 70.0f) return 11u;

    // Config multipliers are calculated from the water values.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 150;
    g_boostPercent = 125;
    if (!patch_coffin_physics(resource)) return 12u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 90.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 100.0f ||
        *(float*)(resource + OFF_SLIP_SPEED) != 90.0f) return 13u;

    // During the one-shot listener window, streaming unload resets targeting
    // so a replacement physics resource can still be accepted before removal.
    g_complete = 1;
    memset(g_syntheticGroupEntries, 0, sizeof(g_syntheticGroupEntries));
    g_syntheticGroupEntries[COFFIN_PHYSICS_OBJECT_INDEX] = resource;
    RawArray unloadGroup = {
        COFFIN_PHYSICS_OBJECT_INDEX + 1u,
        COFFIN_PHYSICS_OBJECT_INDEX + 1u,
        g_syntheticGroupEntries
    };
    on_before_unload(0, &unloadGroup);
    if (g_seenPhysicsResource != 0 || g_physicsReady != 0 || g_complete != 0) return 14u;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 100;
    g_boostPercent = 100;
    g_patchStandardSpeed = 0;
    if (!patch_coffin_physics(resource)) return 15u;
    if (g_seenPhysicsResource != resource ||
        *(float*)(resource + OFF_LAND_TOP_SPEED) != 60.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 80.0f) return 16u;

    // The optional rider experiment also follows a replacement physics target
    // instead of staying at the value calculated for the unloaded instance.
    g_patchStandardSpeed = 1;
    g_seenRideConfig = riderConfig;
    g_standardReady = 0;
    g_lastAppliedStandardSpeed = 0.0f;
    *(float*)(riderConfig + OFF_STANDARD_SPEED) = 40.0f;
    if (!patch_standard_speed(riderConfig, 60.0f)) return 17u;
    g_complete = 1;
    on_before_unload(0, &unloadGroup);
    if (g_standardReady != 0 || g_lastAppliedStandardSpeed != 60.0f) return 18u;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 150;
    if (!patch_coffin_physics(resource)) return 19u;
    if (*(float*)(riderConfig + OFF_STANDARD_SPEED) != 90.0f ||
        g_lastAppliedStandardSpeed != 90.0f || g_standardReady != 1) return 20u;

    // The production callback now checks one exact graph position and UUID,
    // then performs RTTI/layout validation only for that candidate. This also
    // verifies the expanded 1000% configuration ceiling (600/800 km/h caps).
    memset(resource, 0, sizeof(resource));
    memset(g_syntheticPhysicsRtti, 0, sizeof(g_syntheticPhysicsRtti));
    memset(g_syntheticGroupEntries, 0, sizeof(g_syntheticGroupEntries));
    g_syntheticPhysicsVtable[0] = (void*)&synthetic_physics_get_rtti;
    *(void***)resource = g_syntheticPhysicsVtable;
    g_syntheticPhysicsRtti[4] = 4u;
    *(const char**)(g_syntheticPhysicsRtti + 0x40) = g_syntheticPhysicsType;
    *(UINT64*)(resource + 0x10) = COFFIN_PHYSICS_UUID_LOW;
    *(UINT64*)(resource + 0x18) = COFFIN_PHYSICS_UUID_HIGH;
    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_WET_SIDE_GRIP) = 0.09f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_syntheticGroupEntries[COFFIN_PHYSICS_OBJECT_INDEX] = resource;
    g_normalPercent = 1000;
    g_boostPercent = 1000;
    g_patchStandardSpeed = 0;
    g_raiseSlipThreshold = 1;
    g_physicsReady = 0;
    g_standardReady = 0;
    g_complete = 0;
    g_seenPhysicsResource = 0;
    g_seenRideConfig = 0;
    inspect_target_positions(&unloadGroup, false);
    if (!g_physicsReady || !g_complete || g_seenPhysicsResource != resource) return 21u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 600.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 800.0f ||
        *(float*)(resource + OFF_SLIP_SPEED) != 600.0f) return 22u;

    // AddListener does not replay an already loaded target. Exercise the exact
    // native group-table backfill layout and its shared SRW lock with a
    // synthetic StreamingSystem.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 300;
    g_boostPercent = 300;
    g_physicsReady = 0;
    g_complete = 0;
    g_seenPhysicsResource = 0;
    memset(g_syntheticStreamingSystem, 0, sizeof(g_syntheticStreamingSystem));
    memset(g_syntheticGroupTable, 0, sizeof(g_syntheticGroupTable));
    memset(g_syntheticLoadedGroup, 0, sizeof(g_syntheticLoadedGroup));
    UINT32* syntheticGroupHeader =
        (UINT32*)(g_syntheticStreamingSystem + OFF_STREAMING_GROUP_COUNT);
    syntheticGroupHeader[0] = COFFIN_PHYSICS_GROUP_ID + 1u;
    syntheticGroupHeader[1] = COFFIN_PHYSICS_GROUP_ID + 1u;
    *(void**)(g_syntheticStreamingSystem + OFF_STREAMING_GROUP_TABLE) =
        g_syntheticGroupTable;
    BYTE* groupEntry = g_syntheticGroupTable +
        (SIZE_T)COFFIN_PHYSICS_GROUP_ID * STREAMING_GROUP_ENTRY_STRIDE;
    *(void**)(groupEntry + 0u) = &g_syntheticGroupDescriptor;
    *(void**)(groupEntry + 8u) = g_syntheticLoadedGroup;
    *(RawArray*)(g_syntheticLoadedGroup + OFF_LOADED_GROUP_OBJECTS) = unloadGroup;
    inspect_already_loaded_group(g_syntheticStreamingSystem, COFFIN_PHYSICS_GROUP_ID);
    if (!g_physicsReady || !g_complete || g_seenPhysicsResource != resource) return 23u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 180.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 240.0f ||
        *(float*)(resource + OFF_SLIP_SPEED) != 180.0f) return 24u;

    // Optional all-surface mode scales all four caps from one preserved native
    // water baseline. Reapplying the callback must not multiply its own values.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 500;
    g_boostPercent = 500;
    g_scaleWaterCaps = 1;
    g_waterBaseline = 0.0f;
    g_waterBoostBaseline = 0.0f;
    g_lastAppliedWater = 0.0f;
    g_lastAppliedWaterBoost = 0.0f;
    if (!patch_coffin_physics(resource)) return 25u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 300.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 400.0f ||
        *(float*)(resource + OFF_WATER_TOP_SPEED) != 300.0f ||
        *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) != 400.0f ||
        *(float*)(resource + OFF_SLIP_SPEED) != 300.0f) return 26u;
    if (!patch_coffin_physics(resource)) return 27u;
    if (*(float*)(resource + OFF_WATER_TOP_SPEED) != 300.0f ||
        *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) != 400.0f) return 28u;

    // The final speed-only profile selects a smaller drive trampoline with no
    // telemetry load/store. Validate every branch/data target and the actual
    // runtime selector before retaining coverage for the dormant diagnostic
    // variant below.
    int leanFactorDisplacement = 0;
    int leanReturnDisplacement = 0;
    memcpy(
        &leanFactorDisplacement,
        DRIVE_HOOK_NO_TELEMETRY_TEMPLATE + 33u,
        sizeof(leanFactorDisplacement)
    );
    memcpy(
        &leanReturnDisplacement,
        DRIVE_HOOK_NO_TELEMETRY_TEMPLATE + 45u,
        sizeof(leanReturnDisplacement)
    );
    UINT32 leanBranchTarget =
        29u + (UINT32)DRIVE_HOOK_NO_TELEMETRY_TEMPLATE[28u];
    UINT32 leanFactorTarget = 37u + (UINT32)leanFactorDisplacement;
    UINT32 leanReturnTarget = 49u + (UINT32)leanReturnDisplacement;
    g_speedTelemetry = 0;
    DriveHookLayout leanLayout = select_drive_hook_layout();
    if (sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE) != 72u ||
        leanBranchTarget != 37u || leanFactorTarget != 68u ||
        leanReturnTarget != 52u || (68u & 3u) != 0u ||
        DRIVE_HOOK_NO_TELEMETRY_TEMPLATE[43u] != 0xFFu ||
        DRIVE_HOOK_NO_TELEMETRY_TEMPLATE[44u] != 0x25u ||
        leanLayout.bytes != DRIVE_HOOK_NO_TELEMETRY_TEMPLATE ||
        leanLayout.size != 72u || leanLayout.returnOffset != 52u ||
        leanLayout.factorOffset != 68u || leanLayout.telemetryOffset != 0u) {
        return 71u;
    }

    // Validate every control/data target in the opt-in diagnostic template.
    int factorDisplacement = 0;
    int telemetryDisplacement = 0;
    memcpy(&factorDisplacement, DRIVE_HOOK_TEMPLATE + 33u, sizeof(factorDisplacement));
    memcpy(&telemetryDisplacement, DRIVE_HOOK_TEMPLATE + 45u, sizeof(telemetryDisplacement));
    UINT32 branchTarget = 29u + (UINT32)DRIVE_HOOK_TEMPLATE[28u];
    UINT32 factorTarget = 37u + (UINT32)factorDisplacement;
    UINT32 telemetryTarget = 49u + (UINT32)telemetryDisplacement;
    if (sizeof(DRIVE_HOOK_TEMPLATE) != 80u || branchTarget != 49u ||
        factorTarget != 72u || telemetryTarget != 76u ||
        (72u & 3u) != 0u || (76u & 3u) != 0u ||
        DRIVE_HOOK_TEMPLATE[55u] != 0xFFu || DRIVE_HOOK_TEMPLATE[56u] != 0x25u) {
        return 29u;
    }
    g_speedTelemetry = 1;
    DriveHookLayout diagnosticLayout = select_drive_hook_layout();
    if (diagnosticLayout.bytes != DRIVE_HOOK_TEMPLATE ||
        diagnosticLayout.size != 80u || diagnosticLayout.returnOffset != 61u ||
        diagnosticLayout.factorOffset != 72u ||
        diagnosticLayout.telemetryOffset != 76u) return 72u;
    g_speedTelemetry = 0;

    // High-speed gearing and requested drive force fail closed together when
    // the exact hook is unavailable. No resource field may change.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 15.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 500;
    g_boostPercent = 500;
    g_scaleWaterCaps = 1;
    g_driveForcePercent = 500;
    g_gearRatioPercent = 20;
    g_gearRatioBaseline = 0.0f;
    g_lastAppliedGearRatio = 0.0f;
    g_driveHookInstalled = 0;
    g_driveFactorBits = 0;
    if (patch_coffin_physics(resource)) return 30u;
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 40.0f ||
        *(float*)(resource + OFF_FINAL_GEAR_RATIO) != 15.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 60.0f ||
        *(float*)(resource + OFF_WATER_TOP_SPEED) != 60.0f ||
        *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) != 80.0f ||
        *(float*)(resource + OFF_SLIP_SPEED) != 39.0f) return 31u;

    // With a neutral, aligned factor slot available, the full resource
    // transaction commits first and only then publishes the requested 5.0x.
    float neutralFactor = 1.0f;
    LONG neutralFactorBits = 0;
    memcpy(&neutralFactorBits, &neutralFactor, sizeof(neutralFactorBits));
    __atomic_store_n(&g_syntheticDriveFactorBits, neutralFactorBits, __ATOMIC_RELAXED);
    g_driveFactorBits = &g_syntheticDriveFactorBits;
    g_driveHookInstalled = 1;
    if (!patch_coffin_physics(resource)) return 32u;
    LONG factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    float factor = 0.0f;
    memcpy(&factor, &factorBits, sizeof(factor));
    if (*(float*)(resource + OFF_FINAL_GEAR_RATIO) != 3.0f || factor != 5.0f) return 33u;
    if (!patch_coffin_physics(resource) ||
        *(float*)(resource + OFF_FINAL_GEAR_RATIO) != 3.0f) return 34u;
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (factor != 5.0f) return 35u;

    // Unloading the exact patched resource neutralizes the hook before the
    // listener can accept a replacement resource.
    g_complete = 1;
    on_before_unload(0, &unloadGroup);
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (factor != 1.0f || g_physicsReady != 0 || g_complete != 0) return 36u;

    // A foreign gearing value must make the whole atomic patch refuse the
    // resource, leaving caps, slip and gearing untouched and factor neutral.
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 10.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    if (patch_coffin_physics(resource)) return 37u;
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 40.0f ||
        *(float*)(resource + OFF_FINAL_GEAR_RATIO) != 10.0f ||
        *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) != 60.0f ||
        *(float*)(resource + OFF_WATER_TOP_SPEED) != 60.0f ||
        *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) != 80.0f ||
        *(float*)(resource + OFF_SLIP_SPEED) != 39.0f ||
        factor != 1.0f) return 38u;

    // Steering response is a guarded, idempotent part of the same resource
    // transaction. Exercise the proven native 50-degree angle at 130% first;
    // the packaged 160% boundary is tested below. Speed scales stay untouched.
    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 15.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + TEST_OFF_STEERING_HI_SPEED_SCALE) = 1.0f;
    *(float*)(resource + TEST_OFF_STEERING_BOOST_SCALE) = 1.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_steeringAnglePercent = 130;
    g_steeringBaseline = 0.0f;
    g_lastAppliedSteering = 0.0f;
    if (!patch_coffin_physics(resource)) return 39u;
    if (*(float*)(resource + OFF_STEERING_DEGREE) != 65.0f ||
        *(float*)(resource + TEST_OFF_STEERING_HI_SPEED_SCALE) != 1.0f ||
        *(float*)(resource + TEST_OFF_STEERING_BOOST_SCALE) != 1.0f) return 40u;
    if (!patch_coffin_physics(resource) ||
        *(float*)(resource + OFF_STEERING_DEGREE) != 65.0f ||
        *(float*)(resource + TEST_OFF_STEERING_HI_SPEED_SCALE) != 1.0f ||
        *(float*)(resource + TEST_OFF_STEERING_BOOST_SCALE) != 1.0f) return 41u;

    // A foreign in-between steering edit is not compounded or overwritten,
    // and the coupled drive factor is returned to neutral before rejection.
    *(float*)(resource + OFF_STEERING_DEGREE) = 55.0f;
    if (patch_coffin_physics(resource)) return 42u;
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (*(float*)(resource + OFF_STEERING_DEGREE) != 55.0f ||
        *(float*)(resource + TEST_OFF_STEERING_HI_SPEED_SCALE) != 1.0f ||
        *(float*)(resource + TEST_OFF_STEERING_BOOST_SCALE) != 1.0f ||
        factor != 1.0f) return 43u;

    // The accepted upper boundary produces 80 degrees, while values just
    // outside either side of the INI range normalize to the native 100%.
    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 15.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_steeringAnglePercent = normalize_steering_angle_percent(160);
    if (!patch_coffin_physics(resource)) return 44u;
    if (*(float*)(resource + OFF_STEERING_DEGREE) != 80.0f ||
        *(float*)(resource + TEST_OFF_STEERING_HI_SPEED_SCALE) != 1.0f ||
        *(float*)(resource + TEST_OFF_STEERING_BOOST_SCALE) != 1.0f) return 45u;
    if (normalize_steering_angle_percent(100) != 100 ||
        normalize_steering_angle_percent(160) != 160 ||
        normalize_steering_angle_percent(99) != 100 ||
        normalize_steering_angle_percent(161) != 100) return 46u;

    // The compact SpeedPercent profile derives every coupled legacy control.
    apply_unified_speed_profile(500);
    if (g_normalPercent != 500 || g_boostPercent != 500 ||
        g_scaleWaterCaps != 1 || g_driveForcePercent != 500 ||
        g_gearRatioPercent != 20) return 47u;
    apply_unified_speed_profile(99);
    if (g_normalPercent != 100 || g_driveForcePercent != 100 ||
        g_gearRatioPercent != 100) return 48u;
    apply_unified_speed_profile(1200);
    if (g_normalPercent != 1000 || g_driveForcePercent != 1000 ||
        g_gearRatioPercent != 10) return 49u;

    // Acceleration is independent of the cap/gear profile. At the packaged
    // 500% speed and 400% acceleration, the hook publishes 20x drive force;
    // together with the 20% gear ratio this is 4x native effective torque.
    apply_unified_speed_profile(500);
    apply_acceleration_profile(400);
    if (g_accelerationPercent != 400 || g_driveForcePercent != 2000 ||
        g_gearRatioPercent != 20 ||
        g_driveForcePercent * g_gearRatioPercent / 10000 != 4) return 50u;
    apply_acceleration_profile(99);
    if (g_accelerationPercent != 100 || g_driveForcePercent != 500) return 51u;
    apply_unified_speed_profile(1000);
    apply_acceleration_profile(500);
    if (g_driveForcePercent != 5000) return 52u;

    // The compact steering control preserves the 160%/80-degree static limit
    // and uses higher values only for the high-speed response gain.
    apply_compact_steering_profile(250);
    if (g_steeringAnglePercent != 160 || g_steeringResponsePercent != 250 ||
        normalize_steering_response_percent(99) != 100 ||
        normalize_steering_response_percent(301) != 100) return 53u;

    // Validate every branch and RIP-relative data target in the second cave.
    int vtableDisplacement = 0;
    int vtableBranchDisplacement = 0;
    int slopeCheckDisplacement = 0;
    int neutralBranchDisplacement = 0;
    int sequenceBeginDisplacement = 0;
    int speedSampleDisplacement = 0;
    int rampStartDisplacement = 0;
    int zeroDisplacement = 0;
    int spanDisplacement = 0;
    int slopeDisplacement = 0;
    int oneDisplacement = 0;
    int preClampSampleDisplacement = 0;
    int negativeClampDisplacement = 0;
    int positiveClampDisplacement = 0;
    int finalSampleDisplacement = 0;
    int rawSampleDisplacement = 0;
    int scaleSampleDisplacement = 0;
    int sequenceEndDisplacement = 0;
    int returnDisplacement = 0;
    memcpy(&vtableDisplacement, STEERING_HOOK_TEMPLATE + 11u, 4u);
    memcpy(&vtableBranchDisplacement, STEERING_HOOK_TEMPLATE + 20u, 4u);
    memcpy(&slopeCheckDisplacement, STEERING_HOOK_TEMPLATE + 26u, 4u);
    memcpy(&neutralBranchDisplacement, STEERING_HOOK_TEMPLATE + 33u, 4u);
    memcpy(&sequenceBeginDisplacement, STEERING_HOOK_TEMPLATE + 39u, 4u);
    memcpy(&speedSampleDisplacement, STEERING_HOOK_TEMPLATE + 55u, 4u);
    memcpy(&rampStartDisplacement, STEERING_HOOK_TEMPLATE + 63u, 4u);
    memcpy(&zeroDisplacement, STEERING_HOOK_TEMPLATE + 71u, 4u);
    memcpy(&spanDisplacement, STEERING_HOOK_TEMPLATE + 79u, 4u);
    memcpy(&slopeDisplacement, STEERING_HOOK_TEMPLATE + 87u, 4u);
    memcpy(&oneDisplacement, STEERING_HOOK_TEMPLATE + 95u, 4u);
    memcpy(&preClampSampleDisplacement, STEERING_HOOK_TEMPLATE + 107u, 4u);
    memcpy(&negativeClampDisplacement, STEERING_HOOK_TEMPLATE + 115u, 4u);
    memcpy(&positiveClampDisplacement, STEERING_HOOK_TEMPLATE + 123u, 4u);
    memcpy(&finalSampleDisplacement, STEERING_HOOK_TEMPLATE + 131u, 4u);
    memcpy(&rawSampleDisplacement, STEERING_HOOK_TEMPLATE + 147u, 4u);
    memcpy(&scaleSampleDisplacement, STEERING_HOOK_TEMPLATE + 163u, 4u);
    memcpy(&sequenceEndDisplacement, STEERING_HOOK_TEMPLATE + 169u, 4u);
    memcpy(&returnDisplacement, STEERING_HOOK_TEMPLATE + 185u, 4u);
    if (sizeof(STEERING_HOOK_TEMPLATE) != 260u ||
        24u + (UINT32)vtableBranchDisplacement != 173u ||
        37u + (UINT32)neutralBranchDisplacement != 173u ||
        15u + (UINT32)vtableDisplacement != 192u ||
        31u + (UINT32)slopeCheckDisplacement != 220u ||
        43u + (UINT32)sequenceBeginDisplacement != 236u ||
        59u + (UINT32)speedSampleDisplacement != 240u ||
        67u + (UINT32)rampStartDisplacement != 208u ||
        75u + (UINT32)zeroDisplacement != 212u ||
        83u + (UINT32)spanDisplacement != 216u ||
        91u + (UINT32)slopeDisplacement != 220u ||
        99u + (UINT32)oneDisplacement != 224u ||
        111u + (UINT32)preClampSampleDisplacement != 244u ||
        119u + (UINT32)negativeClampDisplacement != 228u ||
        127u + (UINT32)positiveClampDisplacement != 232u ||
        135u + (UINT32)finalSampleDisplacement != 248u ||
        151u + (UINT32)rawSampleDisplacement != 252u ||
        167u + (UINT32)scaleSampleDisplacement != 256u ||
        173u + (UINT32)sequenceEndDisplacement != 236u ||
        189u + (UINT32)returnDisplacement != 200u ||
        (220u & 3u) != 0u || (236u & 3u) != 0u ||
        (240u & 3u) != 0u || (244u & 3u) != 0u ||
        (248u & 3u) != 0u || (252u & 3u) != 0u ||
        (256u & 3u) != 0u ||
        STEERING_HOOK_TEMPLATE[173u] != 0x45u ||
        STEERING_HOOK_TEMPLATE[183u] != 0xFFu) return 54u;

    // Publishing 250% produces a 1.0 -> 2.5 linear ramp over 140 km/h;
    // publishing 100% restores the neutral zero slope.
    g_steeringHookInstalled = 1;
    g_steeringSlopeBits = &g_syntheticSteeringSlopeBits;
    if (!publish_steering_response_percent(250)) return 55u;
    LONG steeringSlopeBits = __atomic_load_n(
        &g_syntheticSteeringSlopeBits, __ATOMIC_RELAXED
    );
    float steeringSlope = 0.0f;
    memcpy(&steeringSlope, &steeringSlopeBits, sizeof(steeringSlope));
    if (!f_near(steeringSlope, 1.5f / 140.0f, 0.000001f)) return 56u;
    if (!publish_steering_response_percent(100)) return 57u;
    steeringSlopeBits = __atomic_load_n(
        &g_syntheticSteeringSlopeBits, __ATOMIC_RELAXED
    );
    memcpy(&steeringSlope, &steeringSlopeBits, sizeof(steeringSlope));
    if (steeringSlope != 0.0f) return 58u;

    // The steering diagnostic record is rejected while its sequence is odd,
    // then decoded only after one coherent even publication. Clamp detection
    // compares the amplified and final wheel commands from that same sample.
    g_steeringSampleSequence = &g_syntheticSteeringSequence;
    g_steeringSampleSpeedBits = &g_syntheticSteeringSpeedBits;
    g_steeringSamplePreClampBits = &g_syntheticSteeringPreClampBits;
    g_steeringSampleFinalBits = &g_syntheticSteeringFinalBits;
    g_steeringSampleRawBits = &g_syntheticSteeringRawBits;
    g_steeringSampleScaleBits = &g_syntheticSteeringScaleBits;
    __atomic_store_n(&g_syntheticSteeringSequence, 1, __ATOMIC_RELEASE);
    SteeringTelemetrySample steeringSample;
    if (read_steering_telemetry_sample(&steeringSample)) return 62u;
    float sampleValue = 240.0f;
    LONG sampleBits = 0;
    memcpy(&sampleBits, &sampleValue, sizeof(sampleBits));
    __atomic_store_n(&g_syntheticSteeringSpeedBits, sampleBits, __ATOMIC_RELAXED);
    sampleValue = 2.0f;
    memcpy(&sampleBits, &sampleValue, sizeof(sampleBits));
    __atomic_store_n(&g_syntheticSteeringPreClampBits, sampleBits, __ATOMIC_RELAXED);
    sampleValue = 1.483529806f;
    memcpy(&sampleBits, &sampleValue, sizeof(sampleBits));
    __atomic_store_n(&g_syntheticSteeringFinalBits, sampleBits, __ATOMIC_RELAXED);
    sampleValue = -0.75f;
    memcpy(&sampleBits, &sampleValue, sizeof(sampleBits));
    __atomic_store_n(&g_syntheticSteeringRawBits, sampleBits, __ATOMIC_RELAXED);
    sampleValue = 1.0f;
    memcpy(&sampleBits, &sampleValue, sizeof(sampleBits));
    __atomic_store_n(&g_syntheticSteeringScaleBits, sampleBits, __ATOMIC_RELAXED);
    __atomic_store_n(&g_syntheticSteeringSequence, 2, __ATOMIC_RELEASE);
    if (!read_steering_telemetry_sample(&steeringSample) ||
        steeringSample.sequence != 2 || steeringSample.speedKmh != 240.0f ||
        steeringSample.preClampRadians != 2.0f ||
        !f_near(steeringSample.finalRadians, 1.483529806f, 0.000001f) ||
        steeringSample.rawInput != -0.75f || steeringSample.scale != 1.0f ||
        !steering_sample_hit_clamp(&steeringSample)) return 63u;
    __atomic_store_n(&g_syntheticSteeringSequence, 3, __ATOMIC_RELEASE);
    sampleValue = 1.0f;
    memcpy(&sampleBits, &sampleValue, sizeof(sampleBits));
    __atomic_store_n(&g_syntheticSteeringPreClampBits, sampleBits, __ATOMIC_RELAXED);
    __atomic_store_n(&g_syntheticSteeringFinalBits, sampleBits, __ATOMIC_RELAXED);
    __atomic_store_n(&g_syntheticSteeringSequence, 4, __ATOMIC_RELEASE);
    if (!read_steering_telemetry_sample(&steeringSample) ||
        steering_sample_hit_clamp(&steeringSample)) return 64u;

    // WetGripPercent is an isolated scalar resource edit. It accepts only the
    // exact native 0.09 value or this mod's value on the same live resource,
    // scales idempotently, and fails closed on foreign/non-finite input.
    if (normalize_wet_grip_percent(100) != 100 ||
        normalize_wet_grip_percent(280) != 280 ||
        normalize_wet_grip_percent(555) != 555 ||
        normalize_wet_grip_percent(99) != 100 ||
        normalize_wet_grip_percent(556) != 100) return 65u;
    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 15.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_WET_SIDE_GRIP) = 0.09f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_normalPercent = 100;
    g_boostPercent = 100;
    g_scaleWaterCaps = 0;
    g_driveForcePercent = 100;
    g_gearRatioPercent = 100;
    g_accelerationPercent = 100;
    g_steeringAnglePercent = 100;
    g_steeringResponsePercent = 100;
    g_wetGripPercent = 280;
    g_raiseSlipThreshold = 0;
    g_wetGripBaseline = 0.0f;
    g_lastAppliedWetGrip = 0.0f;
    g_seenPhysicsResource = 0;
    g_physicsReady = 0;
    if (!patch_coffin_physics(resource) ||
        !f_near(*(float*)(resource + OFF_WET_SIDE_GRIP), 0.252f, 0.00001f) ||
        !f_near(g_wetGripBaseline, 0.09f, 0.00001f) ||
        !f_near(g_lastAppliedWetGrip, 0.252f, 0.00001f)) return 66u;
    if (!patch_coffin_physics(resource) ||
        !f_near(*(float*)(resource + OFF_WET_SIDE_GRIP), 0.252f, 0.00001f)) return 67u;

    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 15.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_WET_SIDE_GRIP) = 0.12f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    g_wetGripBaseline = 0.0f;
    g_lastAppliedWetGrip = 0.0f;
    g_seenPhysicsResource = 0;
    g_physicsReady = 0;
    if (patch_coffin_physics(resource) ||
        *(float*)(resource + OFF_LAND_TOP_SPEED) != 40.0f ||
        *(float*)(resource + OFF_WET_SIDE_GRIP) != 0.12f) return 68u;
    UINT32 nanBits = 0x7FC00000u;
    memcpy(resource + OFF_WET_SIDE_GRIP, &nanBits, sizeof(nanBits));
    if (patch_coffin_physics(resource)) return 69u;

    *(float*)(resource + OFF_WET_SIDE_GRIP) = 0.09f;
    g_wetGripPercent = 555;
    if (!patch_coffin_physics(resource) ||
        !f_near(*(float*)(resource + OFF_WET_SIDE_GRIP), 0.4995f, 0.00001f)) return 70u;

    // Full v1.0.0 packaged-profile transaction: only speed/caps/gearing/slip
    // and the required drive factor change. Steering, wet grip and telemetry
    // remain native/inactive, so no steering hook is required.
    *(float*)(resource + OFF_STEERING_DEGREE) = 50.0f;
    *(float*)(resource + OFF_LAND_TOP_SPEED) = 40.0f;
    *(float*)(resource + OFF_FINAL_GEAR_RATIO) = 15.0f;
    *(float*)(resource + OFF_LAND_BOOST_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_TOP_SPEED) = 60.0f;
    *(float*)(resource + OFF_WATER_BOOST_TOP_SPEED) = 80.0f;
    *(float*)(resource + OFF_WET_SIDE_GRIP) = 0.09f;
    *(float*)(resource + OFF_SLIP_SPEED) = 39.0f;
    apply_unified_speed_profile(500);
    apply_acceleration_profile(400);
    g_steeringAnglePercent = 100;
    g_steeringResponsePercent = 100;
    g_wetGripPercent = 100;
    g_speedTelemetry = 0;
    g_patchStandardSpeed = 0;
    g_raiseSlipThreshold = 1;
    g_waterBaseline = 0.0f;
    g_waterBoostBaseline = 0.0f;
    g_gearRatioBaseline = 0.0f;
    g_steeringBaseline = 0.0f;
    g_wetGripBaseline = 0.0f;
    g_lastAppliedWetGrip = 0.0f;
    g_seenPhysicsResource = 0;
    g_physicsReady = 0;
    neutralFactor = 1.0f;
    memcpy(&neutralFactorBits, &neutralFactor, sizeof(neutralFactorBits));
    __atomic_store_n(&g_syntheticDriveFactorBits, neutralFactorBits, __ATOMIC_RELAXED);
    g_driveHookInstalled = 1;
    g_driveFactorBits = &g_syntheticDriveFactorBits;
    g_driveTelemetryBits = 0;
    g_steeringHookInstalled = 0;
    g_steeringSlopeBits = 0;
    if (!patch_coffin_physics(resource)) return 59u;
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (*(float*)(resource + OFF_LAND_TOP_SPEED) != 300.0f ||
        *(float*)(resource + OFF_WATER_TOP_SPEED) != 300.0f ||
        *(float*)(resource + OFF_FINAL_GEAR_RATIO) != 3.0f ||
        *(float*)(resource + OFF_STEERING_DEGREE) != 50.0f ||
        !f_near(*(float*)(resource + OFF_WET_SIDE_GRIP), 0.09f, 0.00001f) ||
        factor != 20.0f || steering_coupling_needed() ||
        g_speedTelemetry != 0 || g_driveTelemetryBits != 0 ||
        g_steeringHookInstalled != 0) return 60u;
    g_complete = 1;
    on_before_unload(0, &unloadGroup);
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (factor != 1.0f ||
        g_wetGripBaseline != 0.0f || g_lastAppliedWetGrip != 0.0f ||
        g_physicsReady != 0 || g_complete != 0) return 61u;

    // The one-shot lifecycle validates both native listener slots before Add,
    // observes callback completion atomically on the worker side, and removes
    // exactly once only after callback code has returned.
    void* syntheticListenerVtable[5] = { 0, 0, 0, 0, 0 };
    void* wrongListenerVtable[5] = { 0, 0, 0, 0, 0 };
    StreamingEvents syntheticListenerSystem = { syntheticListenerVtable };
    syntheticListenerVtable[3] = (void*)&synthetic_add_listener;
    syntheticListenerVtable[4] = (void*)&synthetic_remove_listener;
    wrongListenerVtable[3] = (void*)&synthetic_add_listener;
    wrongListenerVtable[4] = (void*)&synthetic_remove_listener;
    g_syntheticListenerAdds = 0;
    g_syntheticListenerRemoves = 0;
    g_syntheticCallbackActive = false;
    g_syntheticRemovedInsideCallback = false;
    g_syntheticListenerObject = 0;
    g_streamingSystem = 0;
    g_removeStreamingListener = 0;
    __atomic_store_n(&g_listenerRegistered, 0, __ATOMIC_RELEASE);
    publish_patch_complete(false);
    if (register_streaming_listener_exact(
            &syntheticListenerSystem,
            wrongListenerVtable,
            &synthetic_add_listener,
            &synthetic_remove_listener) ||
        g_syntheticListenerAdds != 0 || listener_state_acquire() != 0) return 74u;

    syntheticListenerVtable[3] = (void*)&synthetic_remove_listener;
    if (register_streaming_listener_exact(
            &syntheticListenerSystem,
            syntheticListenerVtable,
            &synthetic_add_listener,
            &synthetic_remove_listener) ||
        g_syntheticListenerAdds != 0 || listener_state_acquire() != 0) return 75u;

    syntheticListenerVtable[3] = (void*)&synthetic_add_listener;
    syntheticListenerVtable[4] = (void*)&synthetic_add_listener;
    if (register_streaming_listener_exact(
            &syntheticListenerSystem,
            syntheticListenerVtable,
            &synthetic_add_listener,
            &synthetic_remove_listener) ||
        g_syntheticListenerAdds != 0 || listener_state_acquire() != 0) return 76u;

    syntheticListenerVtable[4] = (void*)&synthetic_remove_listener;
    if (!register_streaming_listener_exact(
            &syntheticListenerSystem,
            syntheticListenerVtable,
            &synthetic_add_listener,
            &synthetic_remove_listener) ||
        g_syntheticListenerAdds != 1 || listener_state_acquire() != 1 ||
        g_syntheticListenerObject != &g_listener) return 77u;
    if (register_streaming_listener_exact(
            &syntheticListenerSystem,
            syntheticListenerVtable,
            &synthetic_add_listener,
            &synthetic_remove_listener) ||
        g_syntheticListenerAdds != 1) return 78u;
    if (wait_for_patch_completion(1u, 0u)) return 79u;

    g_seenPhysicsResource = 0;
    g_physicsReady = 0;
    publish_patch_complete(false);
    g_syntheticCallbackActive = true;
    on_finish_load(&g_listener, &unloadGroup);
    g_syntheticCallbackActive = false;
    if (!wait_for_patch_completion(1u, 0u) ||
        g_syntheticListenerRemoves != 0 || g_syntheticRemovedInsideCallback) return 80u;
    if (!unregister_streaming_listener() ||
        g_syntheticListenerRemoves != 1 || g_syntheticRemovedInsideCallback ||
        g_syntheticListenerObject != 0 || listener_state_acquire() != 0 ||
        g_streamingSystem != 0 || g_removeStreamingListener != 0) return 81u;
    factorBits = __atomic_load_n(&g_syntheticDriveFactorBits, __ATOMIC_RELAXED);
    memcpy(&factor, &factorBits, sizeof(factor));
    if (factor != 20.0f) return 82u;
    if (!unregister_streaming_listener() || g_syntheticListenerRemoves != 1) return 83u;

    // The bounded timeout path also removes a registered listener even when no
    // callback ever publishes completion. This is bounded release behavior.
    publish_patch_complete(false);
    if (!register_streaming_listener_exact(
            &syntheticListenerSystem,
            syntheticListenerVtable,
            &synthetic_add_listener,
            &synthetic_remove_listener) ||
        wait_for_patch_completion(1u, 0u) || !unregister_streaming_listener() ||
        g_syntheticListenerAdds != 2 || g_syntheticListenerRemoves != 2 ||
        listener_state_acquire() != 0) return 84u;
    return 0u;
}
