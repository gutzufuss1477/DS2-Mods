// Runs in a standalone test process. Never opens DS2 or accesses game memory.
#define COFFIN_MOUNT_TRACE 1
#include "../src/coffin_board_all_terrain_speed.cpp"
#include "native_availability_bytes.inc"
extern "C" __declspec(dllimport) void __stdcall ExitProcess(UINT32);
extern "C" __declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD);

static int failures = 0;
static UINT32 calls = 0;
static void* seen[4] = {};
static BYTE seenAcquire = 0, seenFifth = 0;
static BYTE resourceUnavailable = 0;
static BYTE __fastcall mock_resource(void*) { return resourceUnavailable; }
static BYTE __fastcall mock_frontend(void* p) { seen[0] = p; ++calls; return 0xA5; }
static void __fastcall mock_void(void* p) { seen[0] = p; ++calls; }
static UINT64 __fastcall mock_reserve(void* v, void* o, BYTE a, void* c, BYTE f) {
    seen[0] = v; seen[1] = o; seen[2] = c;
    seenAcquire = a; seenFifth = f; ++calls;
    return 0xA123456789ABCDEFuLL;
}
static void check(bool result, const char* label) {
    if (result) return;
    ++failures;
    DWORD written = 0; SIZE_T length = 0;
    while (label[length]) ++length;
    WriteFile(GetStdHandle((DWORD)-11), label, (DWORD)length, &written, 0);
}
static BYTE* alloc(SIZE_T size) {
    return (BYTE*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

extern "C" void test_main() {
    // Enable actual logger work between wrapper entry and its native call.
    static const WCHAR logPath[] = L"mount-trace-abi.log";
    memcpy(g_logPath, logPath, sizeof(logPath));
    BYTE* plugin = alloc(0x500), *state = alloc(0xAC0), *vehicle = alloc(0x1300);
    BYTE* entries = alloc(42 * sizeof(void*));
    check(plugin && state && vehicle && entries, "allocation failed\n");
    if (failures) ExitProcess(1);
    *(void**)(state + 0xAA8) = entries;
    ((void**)entries)[1] = plugin;
    *(UINT64*)plugin = RVA_RIDEVEHICLE_PLUGIN_VTABLE;
    *(void**)(state + 0x190) = plugin;

    g_mtFrontend = mock_frontend; calls = 0;
    check(mt_frontend(plugin) == 0xA5 && calls == 1 && seen[0] == plugin,
          "frontend argument/AL forwarding failed\n");
    g_mtActivate = mock_void; calls = 0;
    mt_activate(plugin);
    check(calls == 1 && seen[0] == plugin, "activation forwarding failed\n");
    g_mtDeactivate = mock_void; calls = 0;
    mt_deactivate(plugin);
    check(calls == 1 && seen[0] == plugin, "deactivation forwarding failed\n");
    g_mtPhase5 = mock_void; calls = 0;
    mt_phase5(state);
    check(calls == 1 && seen[0] == state, "phase5 forwarding failed\n");
    g_mtRideOn = mock_void; calls = 0;
    mt_rideon(state);
    check(calls == 1 && seen[0] == state, "RideOn forwarding failed\n");
    g_mtReserve = mock_reserve; calls = 0;
    UINT64 result = mt_reserve(vehicle, plugin, 0x73, state, 0xB9);
    check(result == 0xA123456789ABCDEFuLL && calls == 1 && seen[0] == vehicle &&
          seen[1] == plugin && seen[2] == state && seenAcquire == 0x73 && seenFifth == 0xB9,
          "reservation five arguments/full RAX forwarding failed\n");

    // Execute the production-generated frontend cave against a mock image.
    // It has the real function's stack layout and branch locations, with tiny
    // allow/reject returns instead of the game's body.
    BYTE* fake = alloc(0x6240000u), *world = alloc(0x2264C0u);
    check(fake && world, "mock image allocation failed\n");
    if (failures) ExitProcess(1);
    g_executable = (HMODULE)fake;
    g_enableOutsideNetworkMount = 1;
    g_v151QpidHits = 0;
    *(void**)(fake + RVA_CHIRAL_WORLD_MANAGER_GLOBAL) = world;
    *(void**)plugin = fake + RVA_RIDEVEHICLE_PLUGIN_VTABLE;
    *(void**)vehicle = fake + RVA_V161_COFFIN_VEHICLE_VTABLE;
    *(UINT32*)(vehicle + 0x490) = 3;
    BYTE* entry = fake + 0x10047D0u;
    const BYTE prologue[] = {
        0x40,0x53, 0x48,0x81,0xEC,0x90,0,0,0,
        0x48,0x8B,0x05,0,0,0,0, 0x48,0x8B,0xD9,
        0x80,0xB8,0xBB,0x64,0x22,0,0, 0x0F,0x85,0x4E,0x01,0,0
    };
    memcpy(entry, prologue, sizeof(prologue));
    int globalDisp = (int)((fake + RVA_CHIRAL_WORLD_MANAGER_GLOBAL) - (entry + 16));
    memcpy(entry + 12, &globalDisp, 4);
    const BYTE allow[] = {0xB0,1,0x48,0x81,0xC4,0x90,0,0,0,0x5B,0xC3};
    const BYTE reject[] = {0xB0,0,0x48,0x81,0xC4,0x90,0,0,0,0x5B,0xC3};
    memcpy(fake + RVA_COFFIN_RIDEVEHICLE_NETWORK_CONTINUE, allow, sizeof(allow));
    memcpy(fake + RVA_COFFIN_RIDEVEHICLE_NETWORK_REJECT, reject, sizeof(reject));
    FlushInstructionCache((HANDLE)(INT64)-1, entry, 0x180);
    check(install_v176_ridevehicle_root((HMODULE)fake), "cave installation failed\n");
    if (failures) ExitProcess(1);
    MtFrontendFn testEntry = (MtFrontendFn)entry;
    g_v167CoffinVehicle = 0;
    world[0x2264BB] = 0;
    check(testEntry(plugin) == 1, "native uninhibited branch failed\n");
    world[0x2264BB] = 1;
    check(testEntry(plugin) == 0, "missing Coffin was admitted\n");
    g_v167CoffinVehicle = (UINT64)vehicle;
    check(testEntry(plugin) == 1, "live Coffin with no QPID history rejected\n");
    *(UINT32*)(vehicle + 0x490) = 2;
    check(testEntry(plugin) == 0, "non-Coffin type admitted\n");
    *(UINT32*)(vehicle + 0x490) = 3;
    *(void**)vehicle = fake;
    check(testEntry(plugin) == 0, "wrong vehicle vtable admitted\n");
    *(void**)vehicle = fake + RVA_V161_COFFIN_VEHICLE_VTABLE;
    g_v167CoffinVehicle = 1;
    check(testEntry(plugin) == 0, "unreadable vehicle admitted\n");
    g_v167CoffinVehicle = (UINT64)vehicle;
    *(void**)plugin = fake;
    check(testEntry(plugin) == 0, "wrong plugin vtable admitted\n");
    *(void**)plugin = fake + RVA_RIDEVEHICLE_PLUGIN_VTABLE;
    for (UINT32 i = 0; i < 2000; ++i) {
        check(testEntry(plugin) == 1, "repeated helper call/stack corruption\n");
    }

    // Execute the exact network-veto tail, then the production patch against it.
    BYTE* tail = fake + RVA_V183_AVAILABILITY_TAIL;
    memcpy(tail, V183_AVAILABILITY_TAIL, sizeof(V183_AVAILABILITY_TAIL));
    *(void**)(fake + RVA_V161_COFFIN_VEHICLE_VTABLE + 0x178u) = fake + 0x1F794F0u;
    MtVoidFn veto = (MtVoidFn)tail;
    *(UINT32*)(vehicle+0x4E8) = 0x11; vehicle[0x126E] = 0;
    veto(vehicle);
    check(*(UINT32*)(vehicle+0x4E8) == 0x10, "native outside veto did not reproduce\n");
    *(UINT32*)(vehicle+0x4E8) = 0x11; vehicle[0x126E] = 1;
    veto(vehicle);
    check(*(UINT32*)(vehicle+0x4E8) == 0x11, "native inside availability failed\n");
    g_enableOutsideNetworkMount = 0;
    check(install_v183_coffin_availability((HMODULE)fake) &&
          bytes_equal(tail,V183_AVAILABILITY_TAIL,sizeof(V183_AVAILABILITY_TAIL)),
          "disabled option modified the veto\n");
    g_enableOutsideNetworkMount = 1;
    tail[10] ^= 1;
    check(!install_v183_coffin_availability((HMODULE)fake), "wrong code anchor accepted\n");
    tail[10] ^= 1;
    *(void**)(fake + RVA_V161_COFFIN_VEHICLE_VTABLE + 0x178u) = fake;
    check(!install_v183_coffin_availability((HMODULE)fake), "wrong Coffin vtable accepted\n");
    *(void**)(fake + RVA_V161_COFFIN_VEHICLE_VTABLE + 0x178u) = fake + 0x1F794F0u;
    check(install_v183_coffin_availability((HMODULE)fake), "availability patch failed\n");
    check(bytes_equal(tail,V183_AVAILABILITY_TAIL,9) && tail[16] == 0xC3,
          "patch modified neighboring instructions\n");
    for (UINT32 coverage = 0; coverage < 2; ++coverage) {
        for (UINT32 flags = 0; flags < 1024; ++flags) {
            vehicle[0x126E] = (BYTE)coverage;
            *(UINT32*)(vehicle+0x4E8) = flags;
            veto(vehicle);
            check(*(UINT32*)(vehicle+0x4E8) == flags && vehicle[0x126E] == coverage,
                  "patched veto forced availability or changed coverage\n");
        }
    }

    // Actual native eligibility instructions, with only its resource vcall mocked.
    BYTE* readiness = alloc(sizeof(NATIVE_AVAILABILITY_BYTES));
    memcpy(readiness,NATIVE_AVAILABILITY_BYTES,sizeof(NATIVE_AVAILABILITY_BYTES));
    FlushInstructionCache((HANDLE)(INT64)-1,readiness,sizeof(NATIVE_AVAILABILITY_BYTES));
    MtVoidFn nativeReady = (MtVoidFn)readiness;
    BYTE* handle = alloc(0x10), *holder = alloc(0x30), *resource = alloc(0x60), *resourceVt = alloc(0x60);
    *(void**)handle = holder;
    *(UINT64*)(handle+8) = 0x0800000000000000ull;
    *(void**)(holder+0x20) = resource;
    *(void**)resource = resourceVt;
    *(void**)(resourceVt+0x50) = (void*)&mock_resource;
    for (UINT32 coverage = 0; coverage < 2; ++coverage) {
        for (UINT32 condition = 0; condition < 17; ++condition) {
            memset(vehicle,0,0x1300);
            *(void**)(vehicle+0x68) = handle;
            *(float*)(vehicle+0x56C) = -1.0f;
            *(UINT32*)(vehicle+0x4E8) = 0x10011; // unrelated bit must survive
            vehicle[0x126E] = (BYTE)coverage;
            resourceUnavailable = 0;
            switch (condition) {
                case 1: *(float*)(vehicle+0xB78) = -0.25f; break;
                case 2: vehicle[0xB94] = 4; break;
                case 3: vehicle[0xB98] = 1; break;
                case 4: vehicle[0xB95] = 2; break;
                case 5: *(UINT32*)(vehicle+0x4E4) = 5; break;
                case 6: *(UINT32*)(vehicle+0x4E4) = 6; break;
                case 7: *(UINT32*)(vehicle+0x4E4) = 7; break;
                case 8: *(float*)(vehicle+0x924) = 1.0f; break;
                case 9: vehicle[0x4E8] |= 4; break;
                case 10: *(float*)(vehicle+0x56C) = 0.0f; break;
                case 11: vehicle[0xCE4] = 0x10; break;
                case 12: *(float*)(vehicle+0xCE8) = 1.0f; break;
                case 13: vehicle[0xCE4] = 4; break;
                case 14: *(UINT32*)(vehicle+0x4FC) = 1; break;
                case 15: *(UINT32*)(vehicle+0x4E4) = 4; break;
                case 16: resourceUnavailable = 1; break;
            }
            nativeReady(vehicle);
            UINT32 beforeVeto = *(UINT32*)(vehicle+0x4E8);
            veto(vehicle);
            check((beforeVeto & 1) == (condition == 0 ? 1u : 0u), "native condition result wrong\n");
            check(*(UINT32*)(vehicle+0x4E8) == beforeVeto && (beforeVeto & 0x10000),
                  "network patch overrode native condition or unrelated flags\n");
        }
    }
    DWORD written = 0;
    const char success[] = "PASS: wrapper ABI, frontend cave (2000 calls), exact network veto (2048 flag cases), and actual native eligibility (34 gate cases). DS2 was not started.\n";
    if (!failures) WriteFile(GetStdHandle((DWORD)-11), success, sizeof(success)-1, &written, 0);
    ExitProcess(failures ? 1 : 0);
}
