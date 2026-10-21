// In-process fixture only: no game process, files or saves are touched.
#include "../src/DS2VehicleCore.cpp"

static BYTE testRtti[0x48];
static void* TestRtti(void*) { return testRtti; }

extern "C" __declspec(dllexport) int TestVehiclePhysics() {
    alignas(8) BYTE truck[0xE0] = {};
    alignas(8) BYTE bike[0xE0] = {};
    alignas(8) BYTE invalid[0xE0] = {};
    g_speedEnabled = 1;
    g_speedPatchedResourceCount = g_speedProfilesPatched = g_speedFieldsChanged = 0;
    g_truckNormalSpeedMultiplier = g_bikeNormalSpeedMultiplier = 1.0f;
    g_truckBoostTopSpeedMultiplier = g_bikeBoostTopSpeedMultiplier = 2.0f;
    *(float*)(truck + 0x40) = *(float*)(truck + 0x44) = 30.0f;
    *(float*)(truck + 0x94) = 45.0f;
    if (patch_speed_resource(truck, false) != 1) return 1;
    if (*(float*)(truck + 0x40) != 30.0f || *(float*)(truck + 0x94) != 90.0f) return 2;
    if (patch_speed_resource(truck, false) != 0 || *(float*)(truck + 0x94) != 90.0f) return 3;
    *(float*)(bike + 0x40) = *(float*)(bike + 0x44) = 40.0f;
    *(float*)(bike + 0x94) = 60.0f;
    g_bikeNormalSpeedMultiplier = 1.5f;
    if (patch_speed_resource(bike, true) != 3) return 4;
    if (*(float*)(bike + 0x40) != 60.0f || *(float*)(bike + 0x94) != 120.0f) return 5;
    if (patch_speed_resource(invalid, false) != 0) return 6;
    g_speedEnabled = 0;
    if (patch_speed_resource(invalid, true) != 0) return 7;

    alignas(8) BYTE hill[0x40] = {};
    float slopes[5][2] = {{0.0f,1.0f},{45.0f,0.30f},{40.0f,0.65f},{30.0f,0.75f},{15.0f,0.80f}};
    RawArray* arr = (RawArray*)(hill + 0x28);
    arr->count = arr->capacity = 5;
    arr->entries = slopes;
    g_enabled = 1;
    g_patchedResourceCount = g_totalEntriesChanged = g_patchComplete = 0;
    if (identify_exact_slope_table(arr) != 1) return 8;
    if (patch_slope_resource(hill, "test") != 4) return 9;
    if (!f_near(slopes[1][1], 1.5f, 0.0001f) || slopes[0][1] != 1.0f) return 10;
    if (patch_slope_resource(hill, "test") != 0 || g_totalEntriesChanged != 4) return 11;
    arr->capacity = 4;
    if (identify_exact_slope_table(arr) != 0) return 12;

    // The real streaming path calls an RTTI function through a vtable.
    // Exercise that indirect call with CFG enabled in this fixture.
    memset(testRtti, 0, sizeof(testRtti));
    testRtti[4] = 4;
    *(const char**)(testRtti + 0x40) = "DSPhysicsVehicleResource";
    void* vtable[] = {(void*)&TestRtti};
    void** object = vtable;
    memset(g_typeCache, 0, sizeof(g_typeCache));
    if (classify_stream_object(&object) != TYPE_VEHICLE_RESOURCE) return 13;
    return 0;
}
