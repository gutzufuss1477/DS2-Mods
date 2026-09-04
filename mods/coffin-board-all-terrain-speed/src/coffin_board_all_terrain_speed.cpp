// DS2 Coffin Board All-Terrain Speed v1.1.0
// Target: DEATH STRANDING 2: ON THE BEACH, Steam 1.10.89.0
//
// Event-driven resource patch. The normal-land limits are derived from the
// resource's own water limits, so the default remains meaningful if another
// compatible mod deliberately raises the water speed.

extern "C" {
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long SIZE_T;
typedef long long INT64;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
typedef DWORD (__stdcall *LPTHREAD_START_ROUTINE)(LPVOID);

struct MEMORY_BASIC_INFORMATION_X64 {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    WORD PartitionId;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
};

struct SYSTEMTIME_X {
    WORD wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds;
};

struct THREADENTRY32_X {
    DWORD dwSize;
    DWORD cntUsage;
    DWORD th32ThreadID;
    DWORD th32OwnerProcessID;
    LONG tpBasePri;
    LONG tpDeltaPri;
    DWORD dwFlags;
};

__declspec(dllimport) int __stdcall CloseHandle(HANDLE);
__declspec(dllimport) void __stdcall AcquireSRWLockExclusive(void*);
__declspec(dllimport) void __stdcall AcquireSRWLockShared(void*);
__declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
__declspec(dllimport) HANDLE __stdcall CreateMutexW(LPVOID, int, LPCWSTR);
__declspec(dllimport) HANDLE __stdcall CreateToolhelp32Snapshot(DWORD, DWORD);
__declspec(dllimport) HANDLE __stdcall CreateThread(LPVOID, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, DWORD*);
__declspec(dllimport) int __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) DWORD __stdcall GetLastError();
__declspec(dllimport) void __stdcall GetLocalTime(SYSTEMTIME_X*);
__declspec(dllimport) DWORD __stdcall GetCurrentProcessId();
__declspec(dllimport) DWORD __stdcall GetCurrentThreadId();
__declspec(dllimport) int __stdcall GetThreadContext(HANDLE, void*);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE, WCHAR*, DWORD);
__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
__declspec(dllimport) unsigned int __stdcall GetPrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
__declspec(dllimport) HANDLE __stdcall OpenThread(DWORD, int, DWORD);
__declspec(dllimport) void __stdcall ReleaseSRWLockShared(void*);
__declspec(dllimport) void __stdcall ReleaseSRWLockExclusive(void*);
__declspec(dllimport) DWORD __stdcall ResumeThread(HANDLE);
__declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE, long, long*, DWORD);
__declspec(dllimport) void __stdcall Sleep(DWORD);
__declspec(dllimport) DWORD __stdcall SuspendThread(HANDLE);
__declspec(dllimport) int __stdcall Thread32First(HANDLE, THREADENTRY32_X*);
__declspec(dllimport) int __stdcall Thread32Next(HANDLE, THREADENTRY32_X*);
__declspec(dllimport) LPVOID __stdcall VirtualAlloc(LPVOID, SIZE_T, DWORD, DWORD);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(LPCVOID, MEMORY_BASIC_INFORMATION_X64*, SIZE_T);
__declspec(dllimport) int __stdcall VirtualProtect(LPVOID, SIZE_T, DWORD, DWORD*);
__declspec(dllimport) int __stdcall FlushInstructionCache(HANDLE, LPCVOID, SIZE_T);
__declspec(dllimport) int __stdcall WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, LPVOID);
}

extern "C" int _fltused = 0;
extern "C" void* memset(void* destination, int value, SIZE_T size) {
    BYTE* output = (BYTE*)destination;
    for (SIZE_T i = 0; i < size; ++i) output[i] = (BYTE)value;
    return destination;
}
extern "C" void* memcpy(void* destination, const void* source, SIZE_T size) {
    BYTE* output = (BYTE*)destination;
    const BYTE* input = (const BYTE*)source;
    for (SIZE_T i = 0; i < size; ++i) output[i] = input[i];
    return destination;
}

#define TRUE 1
#define FALSE 0
#define DLL_PROCESS_ATTACH 1u
#define DLL_PROCESS_DETACH 0u
#define ERROR_ALREADY_EXISTS 183u
#define TH32CS_SNAPTHREAD 0x00000004u
#define THREAD_SUSPEND_RESUME 0x00000002u
#define THREAD_GET_CONTEXT 0x00000008u
#define CONTEXT_CONTROL_X64 0x00100001u
#define MEM_COMMIT 0x1000u
#define MEM_RESERVE 0x2000u
#define PAGE_NOACCESS 0x01u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define GENERIC_WRITE 0x40000000u
#define FILE_APPEND_DATA 0x00000004u
#define FILE_SHARE_READ 0x00000001u
#define FILE_SHARE_WRITE 0x00000002u
#define OPEN_ALWAYS 4u
#define FILE_ATTRIBUTE_NORMAL 0x80u
#define FILE_END 2u
#define INVALID_HANDLE_VALUE ((HANDLE)(INT64)-1)

static const UINT32 EXPECTED_TIMESTAMP = 0x6A3DAE46u;
static const UINT32 EXPECTED_IMAGE_SIZE = 0x0B292000u;
static const UINT32 CONFIG_MISSING = 0x80000000u;

static const SIZE_T OFF_STEERING_DEGREE = 0x40u;
static const SIZE_T OFF_LAND_TOP_SPEED = 0x44u;
static const SIZE_T OFF_FINAL_GEAR_RATIO = 0x48u;
static const SIZE_T OFF_LAND_BOOST_TOP_SPEED = 0x94u;
static const SIZE_T OFF_WATER_TOP_SPEED = 0x98u;
static const SIZE_T OFF_WATER_BOOST_TOP_SPEED = 0x9Cu;
static const SIZE_T OFF_WET_SIDE_GRIP = 0xC8u;
static const SIZE_T OFF_SLIP_SPEED = 0xD8u;
static const SIZE_T OFF_STANDARD_SPEED = 0x34u;

// Odradek 0.1 graph checksum 3b11a80e0c3b582e808cf07fc8266ddc.
// These are stable object positions and UUIDs for the supported DS2 build.
static const UINT32 COFFIN_PHYSICS_GROUP_ID = 499u;
static const UINT32 COFFIN_PHYSICS_OBJECT_INDEX = 71288u;
static const UINT32 RIDE_COFFIN_GROUP_ID = 31126u;
static const UINT32 RIDE_COFFIN_OBJECT_INDEX = 19173u;
static const UINT64 COFFIN_PHYSICS_UUID_LOW = 0x6B4E9446DCF0C387ull;
static const UINT64 COFFIN_PHYSICS_UUID_HIGH = 0x26DBAD5945C463BAull;
static const UINT64 RIDE_COFFIN_UUID_LOW = 0x0248EA66A879EA57ull;
static const UINT64 RIDE_COFFIN_UUID_HIGH = 0x21C573467A45A1A1ull;

static const UINT32 RVA_STREAMING_SIGNATURE = 0x00693674u;
static const UINT32 RVA_STREAMING_MANAGER_GLOBAL = 0x06266938u;
static const UINT32 RVA_STREAMING_SYSTEM_VTABLE = 0x034532A0u;
static const UINT32 RVA_STREAMING_ADD_LISTENER = 0x026F6E40u;
static const UINT32 RVA_STREAMING_REMOVE_LISTENER = 0x026F6EE0u;
static const UINT32 RVA_DRIVE_TORQUE_HOOK = 0x0247A431u;
static const UINT32 RVA_STEERING_OUTPUT_HOOK = 0x01F46AF6u;
// Interaction 0xA8 builds Coffin Board ride action 0x158. Its linked-object
// guard treats native vehicle types 1 and 2 as compatible, but excludes the
// Coffin Board (type 3). Raising this immediate from 1 to 2 adds type 3 to
// that one exception without disabling any later mount/safety validation.
static const UINT32 RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT = 0x01007D94u;
// RideVehicle normally disconnects the player's active Floating Carrier while
// entering any vehicle.  The Coffin Board exception below keeps the existing,
// fully loaded carrier linked instead of invoking that disconnect transaction.
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_HOOK = 0x00F9A807u;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_CONTINUE = 0x00F9A81Cu;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_SKIP = 0x00F9AC6Bu;
static const UINT32 RVA_RESOLVE_LINKED_CARRIER = 0x00E462F0u;
// Player baggage event 0x13754FE0 is the confirmed detach path used when a
// high-speed jump exceeds the native Floating Carrier boundary.  Its whole
// transaction must be skipped while the exact captured carrier is still linked
// to an active Coffin Board; preserving only player+0xC60 is too late because
// the transaction has already notified the carrier entity by then.
static const UINT32 RVA_CARRIER_DETACH_EVENT_GATE = 0x00E5C7AAu;
static const UINT32 RVA_CARRIER_DETACH_OTHER_EVENT = 0x00E5C7C2u;
static const UINT32 RVA_CARRIER_DETACH_NATIVE = 0x00E5CDD3u;
static const UINT32 RVA_CARRIER_DETACH_SUPPRESSED = 0x00E5D0A7u;
// In the native bit-4 overextension/break branch, DSHoveringCartComponent sends
// MsgDsNotify 0x16669BB0 to the player immediately before the detach event. Its
// player consumer selects the warning/dialog text, starts Sam's reaction, and
// emits MsgMissionPlayerNotify 0x68F85019. Filter only this dispatch for the
// exact carrier/player/Coffin identity captured during mounting.
static const UINT32 RVA_CARRIER_WARNING_NOTIFICATION_GATE = 0x0121D839u;
static const UINT32 RVA_CARRIER_WARNING_NOTIFICATION_CONTINUE = 0x0121D84Au;
static const UINT32 RVA_ENTITY_MESSAGE_DISPATCH = 0x00130C60u;
static const UINT32 RVA_RIDE_VEHICLE_CARRIER_GATE = 0x01011B6Fu;
static const UINT32 RVA_RIDE_VEHICLE_RESULT_GATE = 0x010049B0u;
static const UINT32 RVA_RIDE_VEHICLE_FALLBACK_GATE = 0x01004A8Au;
static const UINT32 RVA_COFFIN_PHYSICS_VTABLE = 0x033AD0B0u;
static const UINT32 RVA_COFFIN_VFUNC_0 = 0x01F430B0u;
static const UINT32 RVA_COFFIN_VFUNC_1 = 0x01F4E160u;
static const UINT32 RVA_COFFIN_VFUNC_2 = 0x01F480A0u;
static const UINT32 RVA_COFFIN_VFUNC_3 = 0x01F49350u;
static const UINT32 RVA_COFFIN_VFUNC_6 = 0x01F46AB0u;
#if defined(COFFIN_CARRIER_TRACE_BUILD)
static const UINT32 RVA_RIDE_VEHICLE_VTABLE = 0x0325E168u;
static const UINT32 RVA_DROP_CART_VTABLE = 0x03267358u;
static const UINT32 RVA_GET_CART_VTABLE = 0x03266868u;
static const UINT32 RVA_ATTACH_CART_VTABLE = 0x03268B48u;
// The three exact cargo-state event paths that replace the live Floating
// Carrier/current-vehicle handle block at player+0xC60.
static const UINT32 RVA_PLAYER_CARRIER_WRITE_1 = 0x00F6CB2Au;
static const UINT32 RVA_PLAYER_CARRIER_WRITE_2 = 0x00F6CDCAu;
static const UINT32 RVA_PLAYER_CARRIER_WRITE_3 = 0x00F6D184u;
static const UINT32 RVA_PLAYER_CARRIER_DIRECT_EVENT = 0x00E18D78u;
static const UINT32 RVA_PLAYER_CARRIER_ATTACH = 0x00E5CB0Au;
static const UINT32 RVA_PLAYER_CARRIER_ATTACH_NULL = 0x00E5CB2Du;
static const UINT32 RVA_PLAYER_CARRIER_DETACH_EVENT = 0x00E5D09Cu;
static const UINT32 RVA_RIDE_CARRIER_DIRECT_CLEAR = 0x00F9A9BFu;
#endif
static const SIZE_T OFF_STREAMING_GROUP_LOCK = 0xA8u;
static const SIZE_T OFF_STREAMING_GROUP_COUNT = 0x150288u;
static const SIZE_T OFF_STREAMING_GROUP_TABLE = 0x150290u;
static const SIZE_T STREAMING_GROUP_ENTRY_STRIDE = 0x21u;
static const SIZE_T OFF_LOADED_GROUP_OBJECTS = 0x20u;

static HMODULE g_module = 0;
static HANDLE g_mutex = 0;
static WCHAR g_iniPath[512];
static WCHAR g_logPath[512];
static int g_enabled = 1;
static int g_normalPercent = 100;
static int g_boostPercent = 100;
static int g_scaleWaterCaps = 0;
static int g_driveForcePercent = 100;
static int g_gearRatioPercent = 100;
static int g_accelerationPercent = 100;
static int g_steeringAnglePercent = 100;
static int g_steeringResponsePercent = 100;
static int g_wetGripPercent = 100;
static int g_speedTelemetry = 0;
static int g_patchStandardSpeed = 0;
static int g_raiseSlipThreshold = 1;
static int g_debugLog = 0;
static int g_simpleProfile = 0;
static int g_allowFloatingCarrier = 1;
static UINT64 g_maxScanGroups = 0;
static UINT64 g_callbackGroups = 0;
static UINT64 g_callbackObjects = 0;
static UINT64 g_rttiLookups = 0;
static UINT64 g_targetChecks = 0;
static volatile LONG g_listenerRegistered = 0;
static void* g_streamingSystem = 0;
static void (__fastcall* g_removeStreamingListener)(void*, void*) = 0;
static int g_physicsReady = 0;
static int g_standardReady = 0;
static volatile LONG g_complete = 0;
static float g_desiredNormalSpeed = 60.0f;
static float g_lastAppliedStandardSpeed = 0.0f;
static float g_waterBaseline = 0.0f;
static float g_waterBoostBaseline = 0.0f;
static float g_lastAppliedWater = 0.0f;
static float g_lastAppliedWaterBoost = 0.0f;
static float g_gearRatioBaseline = 0.0f;
static float g_lastAppliedGearRatio = 0.0f;
static float g_steeringBaseline = 0.0f;
static float g_lastAppliedSteering = 0.0f;
static float g_wetGripBaseline = 0.0f;
static float g_lastAppliedWetGrip = 0.0f;
static void* g_seenPhysicsResource = 0;
static void* g_seenRideConfig = 0;
static int g_driveHookInstalled = 0;
static volatile LONG* g_driveFactorBits = 0;
static volatile LONG* g_driveTelemetryBits = 0;
static int g_steeringHookInstalled = 0;
static int g_carrierGateInstalled = 0;
static int g_carrierLinkPreserveInstalled = 0;
static int g_carrierDetachGuardInstalled = 0;
static int g_carrierWarningGuardInstalled = 0;

// Written only from native gameplay threads. The Coffin transition publishes
// the carrier entity last; the event hook reads it first. Aligned qword stores
// are atomic on x64 and the platform's store ordering makes the snapshot safe
// without introducing an OS call or lock on the simulation thread.
struct CarrierRideLinkState {
    volatile UINT64 carrierEntity;
    volatile UINT64 playerState;
    volatile UINT64 coffinVehicleHandle;
    volatile UINT64 carrierEntityHandle;
    volatile LONG followUpdateCalls;
    volatile LONG publishedCarrierCalls;
    volatile LONG activeCoffinCalls;
    volatile LONG scaledFollowCalls;
    volatile UINT64 lastComponentHandle;
#if defined(COFFIN_CARRIER_TRACE_BUILD)
    volatile LONG playerEventCalls;
    volatile LONG cartCodeBreakCalls;
    volatile LONG carrierHandleChanges;
    volatile UINT32 lastPlayerEventId;
    volatile UINT32 lastCarrierChangeEventId;
    volatile UINT64 carrierBeforeChange;
    volatile UINT64 carrierAfterChange;
    volatile UINT64 vehicleBeforeChange;
    volatile UINT64 vehicleAfterChange;
#endif
};
__declspec(align(16)) static CarrierRideLinkState g_carrierRideLinkState = {};
static volatile LONG* g_carrierHelperCount = 0;
static volatile LONG* g_carrierHelperStatus = 0;
static volatile LONG* g_carrierResultCount = 0;
static volatile LONG* g_carrierResultCode = 0;
static volatile LONG* g_carrierFallbackCount = 0;
static volatile LONG* g_carrierWarningProducerHits = 0;
static volatile LONG* g_carrierWarningSuppressedHits = 0;
static volatile LONG* g_steeringSlopeBits = 0;
static volatile LONG* g_steeringSampleSequence = 0;
static volatile LONG* g_steeringSampleSpeedBits = 0;
static volatile LONG* g_steeringSamplePreClampBits = 0;
static volatile LONG* g_steeringSampleFinalBits = 0;
static volatile LONG* g_steeringSampleRawBits = 0;
static volatile LONG* g_steeringSampleScaleBits = 0;

#if defined(COFFIN_CARRIER_TRACE_BUILD)
static volatile UINT64* g_carrierTracePlayerState = 0;

struct CarrierTraceProbe {
    const char* name;
    UINT32 vtableRva;
    UINT32 slot;
    UINT32 expectedFunctionRva;
    volatile LONG count;
};

static CarrierTraceProbe g_carrierTraceProbes[] = {
    { "RideVehicle.Enter", RVA_RIDE_VEHICLE_VTABLE, 6u, 0x01004F20u, 0 },
    { "DropCart.CanEnter", RVA_DROP_CART_VTABLE, 4u, 0x010F2900u, 0 },
    { "DropCart.Enter", RVA_DROP_CART_VTABLE, 11u, 0x010F29D0u, 0 },
    { "DropCart.Exit", RVA_DROP_CART_VTABLE, 12u, 0x010F2D70u, 0 },
    { "DropCart.Event", RVA_DROP_CART_VTABLE, 16u, 0x010F3740u, 0 },
    { "GetCart.Enter", RVA_GET_CART_VTABLE, 11u, 0x010F4330u, 0 },
    { "GetCart.Event", RVA_GET_CART_VTABLE, 27u, 0x010F4D30u, 0 },
    { "AttachCart.Event", RVA_ATTACH_CART_VTABLE, 27u, 0x0112F750u, 0 },
};
#endif

static bool patch_complete_acquire() {
    return __atomic_load_n(&g_complete, __ATOMIC_ACQUIRE) != 0;
}

static void publish_patch_complete(bool complete) {
    __atomic_store_n(&g_complete, complete ? 1 : 0, __ATOMIC_RELEASE);
}

static LONG listener_state_acquire() {
    return __atomic_load_n(&g_listenerRegistered, __ATOMIC_ACQUIRE);
}
static UINT64 g_logLock = 0;
// Serializes callback-driven discovery with the worker's loaded-group
// backfill. A zero-initialized SRW lock is valid on Windows.
static UINT64 g_targetStateLock = 0;
static HANDLE g_suspendedThreadHandles[512];
static DWORD g_suspendedThreadIds[512];
static UINT32 g_suspendedThreadCount = 0;
__declspec(align(16)) static BYTE g_threadContext[0x4D0];

static float f_abs(float value) { return value < 0.0f ? -value : value; }
static bool f_near(float value, float expected, float tolerance) {
    return f_abs(value - expected) <= tolerance;
}
static bool f_in_range(float value, float minimum, float maximum) {
    return value >= minimum && value <= maximum;
}
static float f_max(float a, float b) { return a > b ? a : b; }
static int normalize_steering_angle_percent(int value) {
    return value >= 100 && value <= 160 ? value : 100;
}
static int normalize_steering_response_percent(int value) {
    return value >= 100 && value <= 300 ? value : 100;
}
static int normalize_acceleration_percent(int value) {
    return value >= 100 && value <= 500 ? value : 100;
}
static int normalize_wet_grip_percent(int value) {
    return value >= 100 && value <= 555 ? value : 100;
}
static int normalize_speed_percent(int value) {
    if (value < 100) return 100;
    if (value > 1000) return 1000;
    return value;
}
static void apply_unified_speed_profile(int value) {
    int percent = normalize_speed_percent(value);
    g_normalPercent = percent;
    g_boostPercent = percent;
    g_scaleWaterCaps = 1;
    g_driveForcePercent = percent;
    g_gearRatioPercent = 10000 / percent;
    if (g_gearRatioPercent < 10) g_gearRatioPercent = 10;
    if (g_gearRatioPercent > 100) g_gearRatioPercent = 100;
}
static void apply_acceleration_profile(int value) {
    g_accelerationPercent = normalize_acceleration_percent(value);
    UINT64 derivedDrivePercent =
        (UINT64)g_normalPercent * (UINT64)g_accelerationPercent / 100u;
    if (derivedDrivePercent < 100u) derivedDrivePercent = 100u;
    if (derivedDrivePercent > 5000u) derivedDrivePercent = 5000u;
    g_driveForcePercent = (int)derivedDrivePercent;
}
static void apply_compact_steering_profile(int value) {
    g_steeringResponsePercent = normalize_steering_response_percent(value);
    g_steeringAnglePercent = g_steeringResponsePercent > 160 ?
        160 : g_steeringResponsePercent;
}

static int string_compare(const char* a, const char* b) {
    if (!a || !b) return a == b ? 0 : 1;
    while (*a && *b && *a == *b) { ++a; ++b; }
    return (unsigned char)*a - (unsigned char)*b;
}
static void wide_copy(WCHAR* destination, int capacity, const WCHAR* source) {
    if (!destination || capacity <= 0) return;
    int index = 0;
    while (source && source[index] && index < capacity - 1) {
        destination[index] = source[index];
        ++index;
    }
    destination[index] = 0;
}
static WCHAR wide_lower_ascii(WCHAR value) {
    return value >= L'A' && value <= L'Z' ? (WCHAR)(value + (L'a' - L'A')) : value;
}
static bool wide_equal_ignore_case_ascii(const WCHAR* a, const WCHAR* b) {
    if (!a || !b) return a == b;
    while (*a && *b) {
        if (wide_lower_ascii(*a) != wide_lower_ascii(*b)) return false;
        ++a;
        ++b;
    }
    return *a == 0 && *b == 0;
}
static bool is_main_game_process() {
    WCHAR path[512];
    DWORD length = GetModuleFileNameW(0, path, 511);
    if (length == 0 || length >= 511) return false;
    path[length] = 0;
    const WCHAR* baseName = path;
    for (DWORD i = 0; i < length; ++i) {
        if (path[i] == L'\\' || path[i] == L'/') baseName = &path[i + 1];
    }
    return wide_equal_ignore_case_ascii(baseName, L"DS2.exe");
}

struct LogBuffer { char data[1024]; int length; };
static void log_init(LogBuffer* buffer) { buffer->length = 0; }
static void log_char(LogBuffer* buffer, char value) {
    if (buffer->length < 1023) buffer->data[buffer->length++] = value;
}
static void log_text(LogBuffer* buffer, const char* text) {
    if (!text) return;
    while (*text && buffer->length < 1023) buffer->data[buffer->length++] = *text++;
}
static void log_uint(LogBuffer* buffer, UINT64 value) {
    char temporary[32];
    int count = 0;
    if (value == 0) { log_char(buffer, '0'); return; }
    while (value && count < 31) {
        temporary[count++] = (char)('0' + value % 10);
        value /= 10;
    }
    while (count) log_char(buffer, temporary[--count]);
}
static void log_float2(LogBuffer* buffer, float value) {
    if (value < 0.0f) { log_char(buffer, '-'); value = -value; }
    UINT64 whole = (UINT64)value;
    UINT32 fraction = (UINT32)((value - (float)whole) * 100.0f + 0.5f);
    if (fraction >= 100) { ++whole; fraction -= 100; }
    log_uint(buffer, whole);
    log_char(buffer, '.');
    log_char(buffer, (char)('0' + (fraction / 10) % 10));
    log_char(buffer, (char)('0' + fraction % 10));
}
static void log_prefix(LogBuffer* buffer) {
    SYSTEMTIME_X time;
    GetLocalTime(&time);
    log_char(buffer, '['); log_uint(buffer, time.wYear); log_char(buffer, '-');
    if (time.wMonth < 10) log_char(buffer, '0'); log_uint(buffer, time.wMonth); log_char(buffer, '-');
    if (time.wDay < 10) log_char(buffer, '0'); log_uint(buffer, time.wDay); log_char(buffer, ' ');
    if (time.wHour < 10) log_char(buffer, '0'); log_uint(buffer, time.wHour); log_char(buffer, ':');
    if (time.wMinute < 10) log_char(buffer, '0'); log_uint(buffer, time.wMinute); log_char(buffer, ':');
    if (time.wSecond < 10) log_char(buffer, '0'); log_uint(buffer, time.wSecond); log_text(buffer, "] ");
}
static void append_log(LogBuffer* buffer) {
    AcquireSRWLockExclusive(&g_logLock);
    HANDLE file = CreateFileW(
        g_logPath,
        FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    if (!file || file == INVALID_HANDLE_VALUE) {
        ReleaseSRWLockExclusive(&g_logLock);
        return;
    }
    SetFilePointer(file, 0, 0, FILE_END);
    DWORD written = 0;
    WriteFile(file, buffer->data, (DWORD)buffer->length, &written, 0);
    CloseHandle(file);
    ReleaseSRWLockExclusive(&g_logLock);
}
static void log_line(const char* text, bool always) {
    if (!always && !g_debugLog) return;
    LogBuffer buffer;
    log_init(&buffer);
    log_prefix(&buffer);
    log_text(&buffer, text);
    log_text(&buffer, "\r\n");
    append_log(&buffer);
}

static void make_sibling_path(WCHAR* destination, int capacity, const WCHAR* fileName) {
    WCHAR modulePath[512];
    DWORD length = GetModuleFileNameW(g_module, modulePath, 511);
    if (length == 0 || length >= 511) {
        wide_copy(destination, capacity, fileName);
        return;
    }
    modulePath[length] = 0;
    int separator = -1;
    for (int i = 0; modulePath[i]; ++i) {
        if (modulePath[i] == L'\\' || modulePath[i] == L'/') separator = i;
    }
    int output = 0;
    for (int i = 0; i <= separator && output < capacity - 1; ++i) {
        destination[output++] = modulePath[i];
    }
    for (int i = 0; fileName[i] && output < capacity - 1; ++i) {
        destination[output++] = fileName[i];
    }
    destination[output] = 0;
}
static void read_configuration() {
    make_sibling_path(g_iniPath, 512, L"ds2_coffin_board_all_terrain_speed.ini");
    make_sibling_path(g_logPath, 512, L"ds2_coffin_board_all_terrain_speed.log");
    g_enabled = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"Enabled", 1, g_iniPath);
    UINT32 rawUnifiedSpeedPercent = GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"SpeedPercent", -2147483647 - 1, g_iniPath
    );
    if (rawUnifiedSpeedPercent != CONFIG_MISSING) {
        g_simpleProfile = 1;
        apply_unified_speed_profile((int)rawUnifiedSpeedPercent);
        apply_acceleration_profile((int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"AccelerationPercent", 100, g_iniPath
        ));
    } else {
        // Backward compatibility for v0.2.2 and hand-tuned advanced profiles.
        g_normalPercent = (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"LandSpeedPercentOfWater", 100, g_iniPath
        );
        g_boostPercent = (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"LandBoostPercentOfWater", 100, g_iniPath
        );
        g_scaleWaterCaps = (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"ScaleWaterCapsWithPercent", 0, g_iniPath
        );
        g_driveForcePercent = (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"DriveForcePercent", 100, g_iniPath
        );
        g_gearRatioPercent = (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"GearRatioPercent", 100, g_iniPath
        );
    }
    UINT32 rawSteeringPercent = GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"SteeringPercent", -2147483647 - 1, g_iniPath
    );
    if (rawSteeringPercent != CONFIG_MISSING) {
        // Keep the proven static angle increase from v0.2.3, while values over
        // 160 now add only a speed-dependent response gain in the output hook.
        apply_compact_steering_profile((int)rawSteeringPercent);
    } else {
        // Backward-compatible advanced profile: the old angle-only key does
        // not opt into the new runtime steering-response hook.
        g_steeringResponsePercent = 100;
        g_steeringAnglePercent = (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"SteeringAnglePercent", 100, g_iniPath
        );
    }
    g_wetGripPercent = normalize_wet_grip_percent((int)GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"WetGripPercent", 100, g_iniPath
    ));
    UINT32 rawTelemetry = GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"Telemetry", -2147483647 - 1, g_iniPath
    );
    g_speedTelemetry = rawTelemetry != CONFIG_MISSING ? (rawTelemetry != 0u) :
        (int)GetPrivateProfileIntW(
            L"CoffinBoardAllTerrainSpeed", L"SpeedTelemetry", 0, g_iniPath
        );
    g_patchStandardSpeed = (int)GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"PatchStandardSpeed", 0, g_iniPath
    );
    g_raiseSlipThreshold = (int)GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"RaiseSlipThreshold", 1, g_iniPath
    );
    g_debugLog = (int)GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"DebugLog", 0, g_iniPath
    );
    g_allowFloatingCarrier = (int)GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"AllowFloatingCarrier", 1, g_iniPath
    ) != 0;
    int maxGroups = (int)GetPrivateProfileIntW(
        L"CoffinBoardAllTerrainSpeed", L"MaxScanGroups", 0, g_iniPath
    );
    if (g_normalPercent < 100) g_normalPercent = 100;
    if (g_normalPercent > 1000) g_normalPercent = 1000;
    if (g_boostPercent < 100) g_boostPercent = 100;
    if (g_boostPercent > 1000) g_boostPercent = 1000;
    if (g_driveForcePercent < 100) g_driveForcePercent = 100;
    if (g_driveForcePercent > 5000) g_driveForcePercent = 5000;
    // Invalid gearing input fails back to native. In particular, treating 0
    // as the most aggressive 10% value would be an unsafe surprise.
    if (g_gearRatioPercent < 10 || g_gearRatioPercent > 100) {
        g_gearRatioPercent = 100;
    }
    // 100 preserves the native 50-degree steering angle. The upper bound is
    // deliberately conservative because the packaged profile can exceed
    // 200 km/h and a larger wheel angle can cause snap steering or rollovers.
    g_steeringAnglePercent = normalize_steering_angle_percent(g_steeringAnglePercent);
    g_steeringResponsePercent = normalize_steering_response_percent(
        g_steeringResponsePercent
    );
    if (maxGroups < 0) maxGroups = 0;
    if (maxGroups > 1000000) maxGroups = 1000000;
    g_maxScanGroups = (UINT64)maxGroups;
}

static bool readable_range(const void* address, SIZE_T bytes) {
    if (!address || bytes == 0) return false;
    MEMORY_BASIC_INFORMATION_X64 information;
    if (!VirtualQuery(address, &information, sizeof(information))) return false;
    if (information.State != MEM_COMMIT ||
        (information.Protect & PAGE_GUARD) ||
        (information.Protect & PAGE_NOACCESS)) return false;
    UINT64 regionStart = (UINT64)information.BaseAddress;
    UINT64 regionEnd = regionStart + (UINT64)information.RegionSize;
    UINT64 requestedStart = (UINT64)address;
    UINT64 requestedEnd = requestedStart + (UINT64)bytes;
    return requestedStart >= regionStart && requestedEnd >= requestedStart && requestedEnd <= regionEnd;
}
static bool writable_range(const void* address, SIZE_T bytes) {
    if (!readable_range(address, bytes)) return false;
    MEMORY_BASIC_INFORMATION_X64 information;
    if (!VirtualQuery(address, &information, sizeof(information))) return false;
    DWORD protection = information.Protect & 0xFFu;
    return protection == PAGE_READWRITE || protection == PAGE_WRITECOPY ||
           protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY;
}

// Returns 0 on failure, 1 when already equal and 2 when a write occurred.
static int write_float_checked_tolerance(float* field, float target, float tolerance) {
    if (!field || !readable_range(field, sizeof(float))) return 0;
    if (f_near(*field, target, tolerance)) return 1;
    DWORD oldProtection = 0;
    int changedProtection = FALSE;
    if (!writable_range(field, sizeof(float))) {
        changedProtection = VirtualProtect(field, sizeof(float), PAGE_READWRITE, &oldProtection);
        if (!changedProtection) return 0;
    }
    *field = target;
    bool verified = f_near(*field, target, tolerance);
    if (changedProtection) {
        DWORD ignored = 0;
        VirtualProtect(field, sizeof(float), oldProtection, &ignored);
    }
    return verified ? 2 : 0;
}
static int write_float_checked(float* field, float target) {
    return write_float_checked_tolerance(field, target, 0.01f);
}

static bool value_is_native_or_compatible(float value, float nativeValue, float target) {
    return f_near(value, nativeValue, 0.05f) || f_near(value, target, 0.05f) || value > target;
}

static bool validate_target_build(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    if (!readable_range(base, 0x1000)) return false;
    UINT32 peOffset = *(UINT32*)(base + 0x3C);
    if (peOffset > 0x1000 || !readable_range(base + peOffset, 0x108)) return false;
    BYTE* nt = base + peOffset;
    if (*(UINT32*)nt != 0x00004550u) return false;
    if (*(UINT32*)(nt + 8) != EXPECTED_TIMESTAMP) return false;
    BYTE* optional = nt + 24;
    if (*(WORD*)optional != 0x020Bu) return false;
    return *(UINT32*)(optional + 56) == EXPECTED_IMAGE_SIZE;
}

static bool bytes_equal(const BYTE* left, const BYTE* right, SIZE_T size) {
    if (!left || !right) return false;
    for (SIZE_T index = 0; index < size; ++index) {
        if (left[index] != right[index]) return false;
    }
    return true;
}

static bool tracked_thread_id(const DWORD* ids, UINT32 count, DWORD id) {
    for (UINT32 index = 0; index < count; ++index) {
        if (ids[index] == id) return true;
    }
    return false;
}

static void resume_suspended_threads(HANDLE* handles, UINT32 count) {
    while (count) {
        HANDLE thread = handles[--count];
        ResumeThread(thread);
        CloseHandle(thread);
    }
}

// A multi-byte x64 detour cannot be published atomically. Freeze all other
// threads and require a stable second enumeration before touching executable
// bytes. The hook is installed during startup, before Coffin gameplay exists.
static bool suspend_other_threads(
    HANDLE* handles, UINT32* suspendedCount, const BYTE* patchTarget, SIZE_T patchSize
) {
    if (!handles || !suspendedCount || !patchTarget || patchSize == 0u) return false;
    *suspendedCount = 0;
    DWORD processId = GetCurrentProcessId();
    DWORD currentThreadId = GetCurrentThreadId();
    bool stable = false;
    bool unsafeInstructionPointer = false;
    bool contextFailure = false;
    for (UINT32 pass = 0;
         pass < 4u && !stable && !unsafeInstructionPointer && !contextFailure;
         ++pass) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) break;
        THREADENTRY32_X entry;
        memset(&entry, 0, sizeof(entry));
        entry.dwSize = sizeof(entry);
        bool unresolved = false;
        UINT32 added = 0;
        if (Thread32First(snapshot, &entry)) {
            do {
                DWORD id = entry.th32ThreadID;
                if (entry.th32OwnerProcessID != processId || id == currentThreadId ||
                    tracked_thread_id(g_suspendedThreadIds, *suspendedCount, id)) continue;
                if (*suspendedCount >= 512u) { unresolved = true; break; }
                HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, id);
                if (!thread) { unresolved = true; continue; }
                if (SuspendThread(thread) == 0xFFFFFFFFu) {
                    CloseHandle(thread);
                    unresolved = true;
                    continue;
                }
                g_suspendedThreadIds[*suspendedCount] = id;
                handles[*suspendedCount] = thread;
                ++*suspendedCount;
                ++added;
                memset(g_threadContext, 0, sizeof(g_threadContext));
                *(DWORD*)(g_threadContext + 48u) = CONTEXT_CONTROL_X64;
                if (!GetThreadContext(thread, g_threadContext)) {
                    contextFailure = true;
                    break;
                } else {
                    UINT64 instructionPointer = *(UINT64*)(g_threadContext + 248u);
                    UINT64 patchStart = (UINT64)patchTarget;
                    UINT64 patchEnd = patchStart + (UINT64)patchSize;
                    if (instructionPointer >= patchStart && instructionPointer < patchEnd) {
                        unsafeInstructionPointer = true;
                        break;
                    }
                }
            } while (Thread32Next(snapshot, &entry));
        } else {
            unresolved = true;
        }
        CloseHandle(snapshot);
        stable = !unresolved && !unsafeInstructionPointer && !contextFailure && added == 0u;
    }
    if (!stable) {
        resume_suspended_threads(handles, *suspendedCount);
        *suspendedCount = 0;
    }
    return stable;
}

// The first native gate runs before DS2 has identified the nearby vehicle.
// Let the normal query run, then keep the exception only when that query
// returns interaction result 0x158. The Coffin interaction generator emits
// 0x158 exclusively for its ride action; other vehicle results still branch
// to the original failure path while carrier status bit 12 is set.
//
// Runtime-patched qwords: continuation +99, generic success +113, failure +127.
// Trace cells: hit count +135, last result code +139.
static const BYTE CARRIER_RESULT_GATE_TEMPLATE[143] = {
    0xF3,0x0F,0x1E,0xFA,                         // endbr64
    0xF0,0xFF,0x05,0x7C,0x00,0x00,0x00,          // lock inc dword [hit count]
    0x8B,0x84,0x24,0x80,0x00,0x00,0x00,          // mov eax,[rsp+80h]
    0x89,0x05,0x73,0x00,0x00,0x00,               // mov [last result],eax
    0x48,0x8B,0x43,0x28,                          // mov rax,[rbx+28h]
    0x8B,0x90,0x90,0x73,0x00,0x00,               // mov edx,[rax+7390h]
    0x0B,0x90,0x88,0x73,0x00,0x00,               // or edx,[rax+7388h]
    0xF7,0xC2,0x00,0x10,0x00,0x00,               // test edx,1000h
    0x0F,0x84,0x11,0x00,0x00,0x00,               // je native
    0x81,0xBC,0x24,0x80,0x00,0x00,0x00,
        0x58,0x01,0x00,0x00,                      // cmp dword [rsp+80h],158h
    0x0F,0x85,0x34,0x00,0x00,0x00,               // jne fail
    0x8B,0x4C,0x24,0x24,                          // native: mov ecx,[rsp+24h]
    0x83,0xF9,0x01,                               // cmp ecx,1
    0x0F,0x85,0x19,0x00,0x00,0x00,               // jne generic success
    0x81,0xBC,0x24,0x80,0x00,0x00,0x00,
        0x58,0x01,0x00,0x00,                      // cmp dword [rsp+80h],158h
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [continuation]
    0,0,0,0,0,0,0,0,
    0xFF,0x25,0x00,0x00,0x00,0x00,               // generic: jmp [native success]
    0,0,0,0,0,0,0,0,
    0xFF,0x25,0x00,0x00,0x00,0x00,               // fail: jmp [native failure]
    0,0,0,0,0,0,0,0,
    0,0,0,0,                                      // hit count
    0,0,0,0                                       // last result code
};

// Runtime-patched qwords: continuation +65, failure +79. Trace count +87.
static const BYTE CARRIER_FALLBACK_GATE_TEMPLATE[91] = {
    0xF3,0x0F,0x1E,0xFA,                         // endbr64
    0xF0,0xFF,0x05,0x4C,0x00,0x00,0x00,          // lock inc dword [hit count]
    0x84,0xC0,                                    // test al,al
    0x0F,0x84,0x36,0x00,0x00,0x00,               // je fail
    0x48,0x8B,0x43,0x28,                          // mov rax,[rbx+28h]
    0x8B,0x90,0x90,0x73,0x00,0x00,               // mov edx,[rax+7390h]
    0x0B,0x90,0x88,0x73,0x00,0x00,               // or edx,[rax+7388h]
    0xF7,0xC2,0x00,0x10,0x00,0x00,               // test edx,1000h
    0x0F,0x85,0x1A,0x00,0x00,0x00,               // jne fail
    0x48,0x8B,0x44,0x24,0x30,                     // mov rax,[rsp+30h]
    0x48,0x89,0x83,0x20,0x02,0x00,0x00,          // mov [rbx+220h],rax
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [continuation]
    0,0,0,0,0,0,0,0,
    0xFF,0x25,0x00,0x00,0x00,0x00,               // fail: jmp [native failure]
    0,0,0,0,0,0,0,0,
    0,0,0,0                                       // hit count
};

// The initial gate trace captures the combined shifted player status in ECX,
// then always continues with the native checks that follow the carrier bit.
// Runtime-patched continuation qword +34. Trace cells: count +42, status +46.
static const BYTE CARRIER_INITIAL_GATE_TEMPLATE[50] = {
    0xF3,0x0F,0x1E,0xFA,                         // endbr64
    0xF0,0xFF,0x05,0x1F,0x00,0x00,0x00,          // lock inc dword [hit count]
    0x89,0x0D,0x1D,0x00,0x00,0x00,               // mov [last status],ecx
    0x48,0x8B,0x43,0x28,                          // mov rax,[rbx+28h]
    0x83,0xB8,0x48,0x75,0x00,0x00,0x01,          // cmp dword [rax+7548h],1
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [continuation]
    0,0,0,0,0,0,0,0,
    0,0,0,0,                                      // hit count
    0,0,0,0                                       // last shifted status
};

// Coffin-only RideVehicle transition shim. The Coffin branch performs the
// native read-only linked-carrier lookup, snapshots that exact entity plus the
// player/vehicle handles, and then bypasses the disconnect transaction. For
// every other vehicle it replays the original lookup and branch unchanged.
// The carrier handle is cleared before lookup and the entity pointer is
// published last, after the matching handle is captured. Runtime-patched
// qwords: state address +29/+81, resolver +69/+123,
// native continuation +150, Coffin/no-carrier skip +158.
static const BYTE CARRIER_LINK_PRESERVE_TEMPLATE[166] = {
    0xF3,0x0F,0x1E,0xFA,
    0x48,0x8B,0x87,0x90,0x01,0x00,0x00,
    0x83,0xB8,0xA0,0x02,0x00,0x00,0x03,
    0x75,0x5E,
    0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0xC7,0x02,0x00,0x00,0x00,0x00,
    0x49,0xC7,0x42,0x18,0x00,0x00,0x00,0x00,
    0x49,0x89,0x4A,0x08,
    // RSI is the selected VehicleEntity. This matches the later native ride
    // commit at RVA 0xF9ADB6, which copies [rsi+0x320] to playerState+0xC68.
    0x4C,0x8B,0x9E,0x20,0x03,0x00,0x00,
    0x4D,0x89,0x5A,0x10,
    0x48,0xB8, 0,0,0,0,0,0,0,0,
    0xFF,0xD0,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x48,0x85,0xC0,
    0x74,0x0E,
    0x4C,0x8B,0x98,0x20,0x03,0x00,0x00,
    0x4D,0x89,0x5A,0x18,
    0x49,0x89,0x02,
    0xFF,0x25,0x2C,0x00,0x00,0x00,
    0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,
    0x48,0xB8, 0,0,0,0,0,0,0,0,
    0xFF,0xD0,
    0x48,0x85,0xC0,
    0x74,0x0E,
    0xFF,0x25,0x06,0x00,0x00,0x00,
    0xFF,0x25,0x08,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

static bool install_floating_carrier_link_preserve(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_COFFIN_PRESERVE_CARRIER_HOOK;
    static const BYTE expected[21] = {
        0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,
        0xE8,0xDD,0xBA,0xEA,0xFF,
        0x48,0x85,0xC0,
        0x0F,0x84,0x4F,0x04,0x00,0x00
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* cave = (BYTE*)VirtualAlloc(
        0, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE), MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    if (!cave) return false;
    memcpy(cave, CARRIER_LINK_PRESERVE_TEMPLATE, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE));
    UINT64 stateAddress = (UINT64)&g_carrierRideLinkState;
    UINT64 resolver = (UINT64)(base + RVA_RESOLVE_LINKED_CARRIER);
    UINT64 continuation = (UINT64)(base + RVA_COFFIN_PRESERVE_CARRIER_CONTINUE);
    UINT64 skipDisconnect = (UINT64)(base + RVA_COFFIN_PRESERVE_CARRIER_SKIP);
    memcpy(cave + 29u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + 69u, &resolver, sizeof(resolver));
    memcpy(cave + 81u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + 123u, &resolver, sizeof(resolver));
    memcpy(cave + 150u, &continuation, sizeof(continuation));
    memcpy(cave + 158u, &skipDisconnect, sizeof(skipDisconnect));
    FlushInstructionCache(
        (HANDLE)(INT64)-1, cave, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE)
    );

    BYTE replacement[21] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));

    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount,
            target, sizeof(replacement)
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;

    DWORD oldProtection = 0;
    if (!VirtualProtect(
            target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection
        )) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_carrierLinkPreserveInstalled = 1;
    return true;
}

// Event-dispatch shim for the confirmed Floating Carrier detach event.  The
// first four instructions are the native function setup overwritten by the
// hook. Runtime-patched qwords: state address +26, suppressed epilogue +100,
// native detach branch +108, next event comparison +116.
static const BYTE CARRIER_DETACH_GUARD_TEMPLATE[124] = {
    0xF3,0x0F,0x1E,0xFA,
    0x8B,0x42,0x10,
    0x49,0x89,0xD5,
    0x4C,0x8B,0x61,0x48,
    0x48,0x89,0xCE,
    0x3D,0xE0,0x4F,0x75,0x13,
    0x75,0x46,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x39,0x72,0x08,
    0x75,0x30,
    0x4D,0x8B,0x5A,0x18,
    0x4D,0x85,0xDB,
    0x74,0x27,
    0x49,0x83,0xFB,0xFF,
    0x74,0x21,
    0x4C,0x39,0x9E,0x60,0x0C,0x00,0x00,
    0x75,0x18,
    0x49,0x8B,0x02,
    0x48,0x85,0xC0,
    0x74,0x10,
    0x48,0x83,0xBE,0x68,0x0C,0x00,0x00,0xFF,
    0x74,0x06,
    0xFF,0x25,0x0C,0x00,0x00,0x00,
    0xFF,0x25,0x0E,0x00,0x00,0x00,
    0xFF,0x25,0x10,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

static bool install_floating_carrier_detach_guard(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_CARRIER_DETACH_EVENT_GATE;
    static const BYTE expected[24] = {
        0x8B,0x42,0x10,
        0x4C,0x8B,0xEA,
        0x4C,0x8B,0x61,0x48,
        0x48,0x8B,0xF1,
        0x3D,0xE0,0x4F,0x75,0x13,
        0x0F,0x84,0x11,0x06,0x00,0x00
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* cave = (BYTE*)VirtualAlloc(
        0, sizeof(CARRIER_DETACH_GUARD_TEMPLATE),
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    );
    if (!cave) return false;
    memcpy(cave, CARRIER_DETACH_GUARD_TEMPLATE,
           sizeof(CARRIER_DETACH_GUARD_TEMPLATE));
    UINT64 stateAddress = (UINT64)&g_carrierRideLinkState;
    UINT64 suppressed = (UINT64)(base + RVA_CARRIER_DETACH_SUPPRESSED);
    UINT64 nativeDetach = (UINT64)(base + RVA_CARRIER_DETACH_NATIVE);
    UINT64 otherEvent = (UINT64)(base + RVA_CARRIER_DETACH_OTHER_EVENT);
    memcpy(cave + 26u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + 100u, &suppressed, sizeof(suppressed));
    memcpy(cave + 108u, &nativeDetach, sizeof(nativeDetach));
    memcpy(cave + 116u, &otherEvent, sizeof(otherEvent));
    FlushInstructionCache(
        (HANDLE)(INT64)-1, cave, sizeof(CARRIER_DETACH_GUARD_TEMPLATE)
    );

    BYTE replacement[24] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));

    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount,
            target, sizeof(replacement)
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;

    DWORD oldProtection = 0;
    if (!VirtualProtect(
            target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection
        )) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_carrierDetachGuardInstalled = 1;
    return true;
}

// Inline filter for the proven HUD/Sam overextension notification. The native
// path rebuilds MsgDsNotify 0x16669BB0 and calls the original entity dispatcher;
// the Coffin-only path resumes immediately after that call. The captured
// carrier must still be the player's active carrier and the captured Coffin
// must still be the active vehicle. DSHoveringCartComponent+0x650 is an
// internal reference used by its transform lookup, not the player-facing
// carrier handle, so it is intentionally not compared with Entity+0x320.
// Runtime fields: shared state +18, dispatcher +137, continuation +154. Trace
// counters are in the cave at +162 and +166.
static const BYTE CARRIER_WARNING_GUARD_TEMPLATE[170] = {
    0xF3,0x0F,0x1E,0xFA,
    0x50,
    0x41,0x52,
    0x41,0x53,
    0xF0,0xFF,0x05,0x92,0x00,0x00,0x00,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x8B,0x02,
    0x48,0x85,0xC0,
    0x74,0x54,
    0x49,0x8B,0x42,0x08,
    0x48,0x85,0xC0,
    0x74,0x4B,
    0x4D,0x8B,0x5A,0x18,
    0x4D,0x85,0xDB,
    0x74,0x42,
    0x49,0x83,0xFB,0xFF,
    0x74,0x3C,
    0x90,0x90,0x90,0x90,0x90,0x90,0x90,
    0x90,0x90,
    0x4C,0x39,0x98,0x60,0x0C,0x00,0x00,
    0x75,0x2A,
    0x4D,0x8B,0x5A,0x10,
    0x4D,0x85,0xDB,
    0x74,0x21,
    0x49,0x83,0xFB,0xFF,
    0x74,0x1B,
    0x4C,0x39,0x98,0x68,0x0C,0x00,0x00,
    0x75,0x12,
    0xF0,0xFF,0x05,0x3B,0x00,0x00,0x00,
    0x41,0x5B,
    0x41,0x5A,
    0x58,
    0xFF,0x25,0x24,0x00,0x00,0x00,
    0x41,0x5B,
    0x41,0x5A,
    0x58,
    0xC7,0x44,0x24,0x50,0xB0,0x9B,0x66,0x16,
    0x89,0x44,0x24,0x58,
    0x49,0xBB, 0,0,0,0,0,0,0,0,
    0x41,0xFF,0xD3,
    0xFF,0x25,0x00,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

static bool install_floating_carrier_warning_guard(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_CARRIER_WARNING_NOTIFICATION_GATE;
    static const BYTE expected[17] = {
        0xC7,0x44,0x24,0x50,0xB0,0x9B,0x66,0x16,
        0x89,0x44,0x24,0x58,
        0xE8,0x16,0x34,0xF1,0xFE
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* cave = (BYTE*)VirtualAlloc(
        0, sizeof(CARRIER_WARNING_GUARD_TEMPLATE),
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    );
    if (!cave) return false;
    memcpy(cave, CARRIER_WARNING_GUARD_TEMPLATE,
           sizeof(CARRIER_WARNING_GUARD_TEMPLATE));
    UINT64 stateAddress = (UINT64)&g_carrierRideLinkState;
    UINT64 dispatcher = (UINT64)(base + RVA_ENTITY_MESSAGE_DISPATCH);
    UINT64 continuation =
        (UINT64)(base + RVA_CARRIER_WARNING_NOTIFICATION_CONTINUE);
    memcpy(cave + 18u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + 137u, &dispatcher, sizeof(dispatcher));
    memcpy(cave + 154u, &continuation, sizeof(continuation));
    FlushInstructionCache(
        (HANDLE)(INT64)-1, cave, sizeof(CARRIER_WARNING_GUARD_TEMPLATE)
    );

    BYTE replacement[17] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));

    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount,
            target, sizeof(replacement)
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;

    DWORD oldProtection = 0;
    if (!VirtualProtect(
            target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection
        )) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_carrierWarningProducerHits = (volatile LONG*)(cave + 162u);
    g_carrierWarningSuppressedHits = (volatile LONG*)(cave + 166u);
    g_carrierWarningGuardInstalled = 1;
    return true;
}

static bool install_floating_carrier_gate(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* context = base + RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT - 6u;
    BYTE* immediate = base + RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT;
    static const BYTE expectedContext[9] = {
        0x41,0x8D,0x45,0xFF,                    // lea eax,[r13-1]
        0x83,0xF8,0x01,                         // cmp eax,1
        0x76,0x49                                // jbe native-compatible path
    };
    if (!readable_range(context, sizeof(expectedContext)) ||
        !bytes_equal(context, expectedContext, sizeof(expectedContext))) return false;

    // Publishing a single aligned byte is atomic on x64, but changing page
    // protection still happens while other game threads are frozen so this
    // follows the same conservative patch lifecycle as the larger hooks.
    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount, immediate, 1u
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;

    DWORD oldProtection = 0;
    if (!VirtualProtect(immediate, 1u, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    *immediate = 0x02;
    FlushInstructionCache((HANDLE)(INT64)-1, immediate, 1u);
    bool installed = *immediate == 0x02;
    if (!installed) {
        *immediate = 0x01;
        FlushInstructionCache((HANDLE)(INT64)-1, immediate, 1u);
    }
    DWORD ignored = 0;
    VirtualProtect(immediate, 1u, oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_carrierGateInstalled = 1;
    return true;
}

// Retained temporarily as an uncalled reference implementation while the
// targeted interaction patch is gameplay-tested. It is never installed.
bool install_floating_carrier_gate_legacy(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* initialGate = base + RVA_RIDE_VEHICLE_CARRIER_GATE;
    BYTE* resultGate = base + RVA_RIDE_VEHICLE_RESULT_GATE;
    BYTE* fallbackGate = base + RVA_RIDE_VEHICLE_FALLBACK_GATE;
    static const BYTE expectedInitial[17] = {
        0x0F,0x85,0x87,0x00,0x00,0x00,
        0x48,0x8B,0x43,0x28,
        0x83,0xB8,0x48,0x75,0x00,0x00,0x01
    };
    static const BYTE expectedResult[24] = {
        0x8B,0x4C,0x24,0x24, 0x83,0xF9,0x01,
        0x0F,0x85,0x93,0x00,0x00,0x00,
        0x81,0xBC,0x24,0x80,0x00,0x00,0x00,0x58,0x01,0x00,0x00
    };
    static const BYTE expectedFallback[20] = {
        0x84,0xC0, 0x0F,0x84,0xAC,0xFE,0xFF,0xFF,
        0x48,0x8B,0x44,0x24,0x30,
        0x48,0x89,0x83,0x20,0x02,0x00,0x00
    };
    if (!readable_range(initialGate, sizeof(expectedInitial)) ||
        !bytes_equal(initialGate, expectedInitial, sizeof(expectedInitial)) ||
        !readable_range(resultGate, sizeof(expectedResult)) ||
        !bytes_equal(resultGate, expectedResult, sizeof(expectedResult)) ||
        !readable_range(fallbackGate, sizeof(expectedFallback)) ||
        !bytes_equal(fallbackGate, expectedFallback, sizeof(expectedFallback))) return false;

    BYTE* initialCave = (BYTE*)VirtualAlloc(
        0, sizeof(CARRIER_INITIAL_GATE_TEMPLATE), MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    BYTE* resultCave = (BYTE*)VirtualAlloc(
        0, sizeof(CARRIER_RESULT_GATE_TEMPLATE), MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    BYTE* fallbackCave = (BYTE*)VirtualAlloc(
        0, sizeof(CARRIER_FALLBACK_GATE_TEMPLATE), MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    if (!initialCave || !resultCave || !fallbackCave) return false;
    memcpy(initialCave, CARRIER_INITIAL_GATE_TEMPLATE, sizeof(CARRIER_INITIAL_GATE_TEMPLATE));
    memcpy(resultCave, CARRIER_RESULT_GATE_TEMPLATE, sizeof(CARRIER_RESULT_GATE_TEMPLATE));
    memcpy(fallbackCave, CARRIER_FALLBACK_GATE_TEMPLATE, sizeof(CARRIER_FALLBACK_GATE_TEMPLATE));

    UINT64 canEnterFailure = (UINT64)(base + 0x01004936u);
    UINT64 resultContinuation = (UINT64)(base + 0x010049C8u);
    UINT64 genericSuccess = (UINT64)(base + 0x01004A50u);
    UINT64 fallbackContinuation = (UINT64)(base + 0x01004A9Eu);
    UINT64 helperContinuation = (UINT64)(base + 0x01011B80u);
    memcpy(initialCave + 34u, &helperContinuation, sizeof(helperContinuation));
    memcpy(resultCave + 99u, &resultContinuation, sizeof(resultContinuation));
    memcpy(resultCave + 113u, &genericSuccess, sizeof(genericSuccess));
    memcpy(resultCave + 127u, &canEnterFailure, sizeof(canEnterFailure));
    memcpy(fallbackCave + 65u, &fallbackContinuation, sizeof(fallbackContinuation));
    memcpy(fallbackCave + 79u, &canEnterFailure, sizeof(canEnterFailure));
    FlushInstructionCache((HANDLE)(INT64)-1, initialCave, sizeof(CARRIER_INITIAL_GATE_TEMPLATE));
    FlushInstructionCache((HANDLE)(INT64)-1, resultCave, sizeof(CARRIER_RESULT_GATE_TEMPLATE));
    FlushInstructionCache((HANDLE)(INT64)-1, fallbackCave, sizeof(CARRIER_FALLBACK_GATE_TEMPLATE));

    BYTE initialReplacement[17] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90
    };
    BYTE resultReplacement[24] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90
    };
    BYTE fallbackReplacement[20] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90,0x90
    };
    UINT64 resultCaveAddress = (UINT64)resultCave;
    UINT64 fallbackCaveAddress = (UINT64)fallbackCave;
    UINT64 initialCaveAddress = (UINT64)initialCave;
    memcpy(initialReplacement + 6u, &initialCaveAddress, sizeof(initialCaveAddress));
    memcpy(resultReplacement + 6u, &resultCaveAddress, sizeof(resultCaveAddress));
    memcpy(fallbackReplacement + 6u, &fallbackCaveAddress, sizeof(fallbackCaveAddress));

    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount,
            base + 0x010047D0u, 0x0000D3B0u
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;
    DWORD initialProtection = 0;
    DWORD resultProtection = 0;
    DWORD fallbackProtection = 0;
    bool initialWritable = VirtualProtect(
        initialGate, sizeof(expectedInitial), PAGE_EXECUTE_READWRITE, &initialProtection
    ) != 0;
    bool resultWritable = initialWritable && VirtualProtect(
        resultGate, sizeof(expectedResult), PAGE_EXECUTE_READWRITE, &resultProtection
    ) != 0;
    bool fallbackWritable = resultWritable && VirtualProtect(
        fallbackGate, sizeof(expectedFallback), PAGE_EXECUTE_READWRITE, &fallbackProtection
    ) != 0;
    if (!fallbackWritable) {
        DWORD ignored = 0;
        if (resultWritable) VirtualProtect(resultGate, sizeof(expectedResult), resultProtection, &ignored);
        if (initialWritable) VirtualProtect(initialGate, sizeof(expectedInitial), initialProtection, &ignored);
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }

    memcpy(initialGate, initialReplacement, sizeof(initialReplacement));
    memcpy(resultGate, resultReplacement, sizeof(resultReplacement));
    memcpy(fallbackGate, fallbackReplacement, sizeof(fallbackReplacement));
    FlushInstructionCache((HANDLE)(INT64)-1, initialGate, sizeof(initialReplacement));
    FlushInstructionCache((HANDLE)(INT64)-1, resultGate, sizeof(resultReplacement));
    FlushInstructionCache((HANDLE)(INT64)-1, fallbackGate, sizeof(fallbackReplacement));
    bool installed =
        bytes_equal(initialGate, initialReplacement, sizeof(initialReplacement)) &&
        bytes_equal(resultGate, resultReplacement, sizeof(resultReplacement)) &&
        bytes_equal(fallbackGate, fallbackReplacement, sizeof(fallbackReplacement));
    if (!installed) {
        memcpy(initialGate, expectedInitial, sizeof(expectedInitial));
        memcpy(resultGate, expectedResult, sizeof(expectedResult));
        memcpy(fallbackGate, expectedFallback, sizeof(expectedFallback));
        FlushInstructionCache((HANDLE)(INT64)-1, initialGate, sizeof(expectedInitial));
        FlushInstructionCache((HANDLE)(INT64)-1, resultGate, sizeof(expectedResult));
        FlushInstructionCache((HANDLE)(INT64)-1, fallbackGate, sizeof(expectedFallback));
    }
    DWORD ignored = 0;
    VirtualProtect(fallbackGate, sizeof(expectedFallback), fallbackProtection, &ignored);
    VirtualProtect(resultGate, sizeof(expectedResult), resultProtection, &ignored);
    VirtualProtect(initialGate, sizeof(expectedInitial), initialProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_carrierHelperCount = (volatile LONG*)(initialCave + 42u);
    g_carrierHelperStatus = (volatile LONG*)(initialCave + 46u);
    g_carrierResultCount = (volatile LONG*)(resultCave + 135u);
    g_carrierResultCode = (volatile LONG*)(resultCave + 139u);
    g_carrierFallbackCount = (volatile LONG*)(fallbackCave + 87u);
    g_carrierGateInstalled = 1;
    return true;
}

// The final package does not enable telemetry. Use this smaller trampoline in
// that mode so the physics thread performs no telemetry load/store at all.
// Runtime-patched fields: vtable qword +16, return qword +52 and aligned
// factor bits +68.
static const BYTE DRIVE_HOOK_NO_TELEMETRY_TEMPLATE[72] = {
    0xF3,0x0F,0x1E,0xFA,
    0xC4,0xC1,0x4A,0x59,0xCC,
    0xC5,0xFA,0x59,0xC1,
    0x50,
    0x48,0xB8, 0,0,0,0,0,0,0,0,
    0x48,0x39,0x03,
    0x75,0x08,
    0xC5,0xFA,0x59,0x05, 0x1F,0,0,0,
    0x58,
    0xC5,0xFA,0x11,0x47,0x18,
    0xFF,0x25,0x03,0,0,0,
    0xCC,0xCC,0xCC,
    0,0,0,0,0,0,0,0,
    0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,
    0,0,0,0
};

// Legacy diagnostic variant. It is selected only when an older custom INI
// explicitly opts into telemetry; the v1.0.0 INI has no such key.
// Runtime-patched fields: vtable qword +16, return qword +61, aligned factor
// bits +72 and aligned telemetry bits +76.
static const BYTE DRIVE_HOOK_TEMPLATE[80] = {
    0xF3,0x0F,0x1E,0xFA,
    0xC4,0xC1,0x4A,0x59,0xCC,
    0xC5,0xFA,0x59,0xC1,
    0x50,
    0x48,0xB8, 0,0,0,0,0,0,0,0,
    0x48,0x39,0x03,
    0x75,0x14,
    0xC5,0xFA,0x59,0x05, 0x23,0,0,0,
    0x8B,0x83,0x20,0x01,0,0,
    0x89,0x05, 0x1B,0,0,0,
    0x58,
    0xC5,0xFA,0x11,0x47,0x18,
    0xFF,0x25,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0xCC,0xCC,0xCC,
    0,0,0,0,
    0,0,0,0
};

struct DriveHookLayout {
    const BYTE* bytes;
    SIZE_T size;
    SIZE_T returnOffset;
    SIZE_T factorOffset;
    SIZE_T telemetryOffset;
};

static DriveHookLayout select_drive_hook_layout() {
    DriveHookLayout layout;
    if (g_speedTelemetry) {
        layout.bytes = DRIVE_HOOK_TEMPLATE;
        layout.size = sizeof(DRIVE_HOOK_TEMPLATE);
        layout.returnOffset = 61u;
        layout.factorOffset = 72u;
        layout.telemetryOffset = 76u;
    } else {
        layout.bytes = DRIVE_HOOK_NO_TELEMETRY_TEMPLATE;
        layout.size = sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE);
        layout.returnOffset = 52u;
        layout.factorOffset = 68u;
        layout.telemetryOffset = 0u;
    }
    return layout;
}

// Hooks the proven generic drive-moment store, but changes xmm0 only when the
// current component has the exact Coffin physics vtable. The optional legacy
// diagnostic variant also copies prior-frame 3D speed to a private slot. No
// file I/O or locks run on the physics thread.
static bool install_drive_force_hook(HMODULE executable) {
    bool hookNeeded = g_driveForcePercent != 100 ||
        g_gearRatioPercent < 100 || g_speedTelemetry;
    if (!hookNeeded) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_DRIVE_TORQUE_HOOK;
    void** coffinVtableAddress = (void**)(base + RVA_COFFIN_PHYSICS_VTABLE);
    static const BYTE expected[18] = {
        0xC4,0xC1,0x4A,0x59,0xCC,             // vmulss xmm1,xmm6,xmm12
        0xC5,0xFA,0x59,0xC1,                  // vmulss xmm0,xmm0,xmm1
        0xC5,0xFA,0x11,0x47,0x18,             // vmovss [rdi+18h],xmm0
        0xC5,0xE2,0x5C,0xC6                   // next untouched instruction
    };
    if (!readable_range(coffinVtableAddress, 4u * sizeof(void*)) ||
        coffinVtableAddress[0] != (void*)(base + RVA_COFFIN_VFUNC_0) ||
        coffinVtableAddress[1] != (void*)(base + RVA_COFFIN_VFUNC_1) ||
        coffinVtableAddress[2] != (void*)(base + RVA_COFFIN_VFUNC_2) ||
        coffinVtableAddress[3] != (void*)(base + RVA_COFFIN_VFUNC_3) ||
        !readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    DriveHookLayout layout = select_drive_hook_layout();
    BYTE* cave = (BYTE*)VirtualAlloc(
        0, layout.size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    );
    if (!cave) return false;
    memcpy(cave, layout.bytes, layout.size);
    UINT64 coffinVtable = (UINT64)(base + RVA_COFFIN_PHYSICS_VTABLE);
    UINT64 returnAddress = (UINT64)(target + 14u);
    // The hook is published in a neutral state. The configured multiplier is
    // exposed only after the resource caps/gearing transaction commits.
    float driveFactor = 1.0f;
    memcpy(cave + 16u, &coffinVtable, sizeof(coffinVtable));
    memcpy(cave + layout.returnOffset, &returnAddress, sizeof(returnAddress));
    memcpy(cave + layout.factorOffset, &driveFactor, sizeof(driveFactor));
    if (layout.telemetryOffset) *(float*)(cave + layout.telemetryOffset) = 0.0f;
    FlushInstructionCache((HANDLE)(INT64)-1, cave, layout.size);

    BYTE replacement[14] = { 0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0 };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    BYTE original[14];
    memcpy(original, target, sizeof(original));
    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount, target, sizeof(replacement)
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, original, sizeof(original));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(original));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_driveFactorBits = (volatile LONG*)(cave + layout.factorOffset);
    g_driveTelemetryBits = layout.telemetryOffset ?
        (volatile LONG*)(cave + layout.telemetryOffset) : 0;
    g_driveHookInstalled = 1;
    return true;
}

// Runtime-patched fields: vtable qword +192, return qword +200, aligned float
// constants +208..+232, then an aligned steering telemetry record at
// +236..+256. The odd/even sequence brackets every record publication so the
// worker can reject torn samples; the physics thread only performs aligned
// scalar stores and never does file I/O. Slope zero bypasses both the response
// path and the sampler, keeping the initially published detour neutral.
static const BYTE STEERING_HOOK_TEMPLATE[260] = {
    0xF3,0x0F,0x1E,0xFA,
    0xC5,0xE2,0x59,0xE0,
    0x4C,0x8B,0x1D,0xB1,0x00,0x00,0x00,
    0x4C,0x39,0x19,
    0x0F,0x85,0x95,0x00,0x00,0x00,
    0x83,0x3D,0xBD,0x00,0x00,0x00,0x00,
    0x0F,0x84,0x88,0x00,0x00,0x00,
    0xFF,0x05,0xC1,0x00,0x00,0x00,
    0xC5,0xFA,0x10,0x91,0x20,0x01,0x00,0x00,
    0xC5,0xFA,0x11,0x15,0xB5,0x00,0x00,0x00,
    0xC5,0xEA,0x5C,0x15,0x8D,0x00,0x00,0x00,
    0xC5,0xEA,0x5F,0x15,0x89,0x00,0x00,0x00,
    0xC5,0xEA,0x5D,0x15,0x85,0x00,0x00,0x00,
    0xC5,0xEA,0x59,0x15,0x81,0x00,0x00,0x00,
    0xC5,0xEA,0x58,0x15,0x7D,0x00,0x00,0x00,
    0xC5,0xDA,0x59,0xE2,
    0xC5,0xFA,0x11,0x25,0x85,0x00,0x00,0x00,
    0xC5,0xDA,0x5F,0x25,0x6D,0x00,0x00,0x00,
    0xC5,0xDA,0x5D,0x25,0x69,0x00,0x00,0x00,
    0xC5,0xFA,0x11,0x25,0x71,0x00,0x00,0x00,
    0xC5,0xFA,0x10,0x91,0xF4,0x00,0x00,0x00,
    0xC5,0xFA,0x11,0x15,0x65,0x00,0x00,0x00,
    0xC5,0xFA,0x10,0x91,0x10,0x01,0x00,0x00,
    0xC5,0xFA,0x11,0x15,0x59,0x00,0x00,0x00,
    0xFF,0x05,0x3F,0x00,0x00,0x00,
    0x45,0x89,0xC8,
    0x4C,0x63,0x10,
    0x49,0x83,0xFA,0x04,
    0xFF,0x25,0x0B,0x00,0x00,0x00,
    0x0F,0x1F,0x00
};

// The normal game's high-speed scale is capped at 1.0. This Coffin-only hook
// therefore remaps the final wheel command itself: native response through
// 80 km/h, a linear gain ramp through 220 km/h, and a hard +/-85-degree clamp.
static bool install_steering_response_hook(HMODULE executable) {
    if (g_steeringResponsePercent == 100) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_STEERING_OUTPUT_HOOK;
    void** coffinVtableAddress = (void**)(base + RVA_COFFIN_PHYSICS_VTABLE);
    static const BYTE expected[16] = {
        0xC5,0xE2,0x59,0xE0,                  // vmulss xmm4,xmm3,xmm0
        0x45,0x8B,0xC1,                       // mov r8d,r9d
        0x4C,0x63,0x10,                       // movsxd r10,dword ptr [rax]
        0x49,0x83,0xFA,0x04,                  // cmp r10,4
        0x7C,0x72                              // next untouched jl
    };
    if (!readable_range(coffinVtableAddress, 7u * sizeof(void*)) ||
        coffinVtableAddress[0] != (void*)(base + RVA_COFFIN_VFUNC_0) ||
        coffinVtableAddress[6] != (void*)(base + RVA_COFFIN_VFUNC_6) ||
        !readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* cave = (BYTE*)VirtualAlloc(
        0, sizeof(STEERING_HOOK_TEMPLATE), MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    if (!cave) return false;
    memcpy(cave, STEERING_HOOK_TEMPLATE, sizeof(STEERING_HOOK_TEMPLATE));
    UINT64 coffinVtable = (UINT64)(base + RVA_COFFIN_PHYSICS_VTABLE);
    UINT64 returnAddress = (UINT64)(target + 14u);
    float rampStartKmh = 80.0f;
    float zero = 0.0f;
    float rampSpanKmh = 140.0f;
    float neutralSlope = 0.0f;
    float one = 1.0f;
    float negativeClampRadians = -1.483529806f;
    float positiveClampRadians = 1.483529806f;
    memcpy(cave + 192u, &coffinVtable, sizeof(coffinVtable));
    memcpy(cave + 200u, &returnAddress, sizeof(returnAddress));
    memcpy(cave + 208u, &rampStartKmh, sizeof(rampStartKmh));
    memcpy(cave + 212u, &zero, sizeof(zero));
    memcpy(cave + 216u, &rampSpanKmh, sizeof(rampSpanKmh));
    memcpy(cave + 220u, &neutralSlope, sizeof(neutralSlope));
    memcpy(cave + 224u, &one, sizeof(one));
    memcpy(cave + 228u, &negativeClampRadians, sizeof(negativeClampRadians));
    memcpy(cave + 232u, &positiveClampRadians, sizeof(positiveClampRadians));
    FlushInstructionCache((HANDLE)(INT64)-1, cave, sizeof(STEERING_HOOK_TEMPLATE));

    BYTE replacement[14] = { 0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0 };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    BYTE original[14];
    memcpy(original, target, sizeof(original));
    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount, target, sizeof(replacement)
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, original, sizeof(original));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(original));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    if (!installed) return false;
    g_steeringSlopeBits = (volatile LONG*)(cave + 220u);
    g_steeringSampleSequence = (volatile LONG*)(cave + 236u);
    g_steeringSampleSpeedBits = (volatile LONG*)(cave + 240u);
    g_steeringSamplePreClampBits = (volatile LONG*)(cave + 244u);
    g_steeringSampleFinalBits = (volatile LONG*)(cave + 248u);
    g_steeringSampleRawBits = (volatile LONG*)(cave + 252u);
    g_steeringSampleScaleBits = (volatile LONG*)(cave + 256u);
    g_steeringHookInstalled = 1;
    return true;
}

#if defined(COFFIN_CARRIER_TRACE_BUILD)
// Diagnostic-only vtable shim. It preserves registers and flags, increments a
// private counter without calling into the OS, then tail-jumps to the exact
// original virtual method. No logging or locking occurs on the game thread.
static const BYTE CARRIER_TRACE_SHIM_TEMPLATE[35] = {
    0xF3,0x0F,0x1E,0xFA,                    // endbr64
    0x9C,                                   // pushfq
    0x50,                                   // push rax
    0x48,0xB8, 0,0,0,0,0,0,0,0,            // mov rax,counter
    0xF0,0xFF,0x00,                         // lock inc dword ptr [rax]
    0x58,                                   // pop rax
    0x9D,                                   // popfq
    0xFF,0x25,0,0,0,0,                     // jmp qword ptr [rip]
    0,0,0,0,0,0,0,0                        // original virtual method
};

// RideVehicle.Enter uses the same counter shim but also snapshots the player
// state pointer from action+0xA8. The worker only reads that pointer and the
// two native link handles; no game function is called from the worker thread.
// Runtime-patched qwords: counter +8, original method +41. State cell +49.
static const BYTE CARRIER_RIDE_ENTER_TRACE_SHIM_TEMPLATE[57] = {
    0xF3,0x0F,0x1E,0xFA,                    // endbr64
    0x9C,                                   // pushfq
    0x50,                                   // push rax
    0x48,0xB8, 0,0,0,0,0,0,0,0,            // mov rax,counter
    0xF0,0xFF,0x00,                         // lock inc dword ptr [rax]
    0x48,0x8B,0x81,0xA8,0x00,0x00,0x00,     // mov rax,[rcx+A8h]
    0x48,0x89,0x05,0x10,0x00,0x00,0x00,     // mov [state cell],rax
    0x58,                                   // pop rax
    0x9D,                                   // popfq
    0xFF,0x25,0x00,0x00,0x00,               // jmp qword ptr [rip]
    0,0,0,0,0,0,0,0,                       // original virtual method
    0,0,0,0,0,0,0,0                        // captured player state
};

// Non-calling inline trace for the three exact 32-byte handle-block stores.
// Runtime-patched values: shared state +10, event id +132, native continuation
// +42 and trace continuation +151. The cave preserves flags, RAX and R10,
// replays the original AVX store exactly once, and publishes only real changes.
static const BYTE PLAYER_HANDLE_WRITE_TRACE_TEMPLATE[159] = {
    0xF3,0x0F,0x1E,0xFA, 0x9C,0x50,0x41,0x52,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x39,0x7A,0x08, 0x74,0x1A,
    0xC5,0xFC,0x11,0x8F,0x60,0x0C,0x00,0x00,
    0x41,0x5A,0x58,0x9D,
    0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
    0x48,0x8B,0x87,0x60,0x0C,0x00,0x00,
    0x49,0x89,0x42,0x50,
    0x48,0x8B,0x87,0x68,0x0C,0x00,0x00,
    0x49,0x89,0x42,0x60,
    0xC5,0xFC,0x11,0x8F,0x60,0x0C,0x00,0x00,
    0x48,0x8B,0x87,0x60,0x0C,0x00,0x00,
    0x49,0x89,0x42,0x58,
    0x48,0x8B,0x87,0x68,0x0C,0x00,0x00,
    0x49,0x89,0x42,0x68,
    0x48,0x8B,0x87,0x60,0x0C,0x00,0x00,
    0x49,0x3B,0x42,0x50, 0x75,0x0D,
    0x48,0x8B,0x87,0x68,0x0C,0x00,0x00,
    0x49,0x3B,0x42,0x60, 0x74,0x0D,
    0x41,0xC7,0x42,0x48, 0,0,0,0,
    0xF0,0x41,0xFF,0x42,0x40,
    0x41,0x5A,0x58,0x9D,
    0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0
};

static BYTE* allocate_trace_caves_near(BYTE* executableBase, SIZE_T size) {
    UINT64 start = ((UINT64)executableBase + 0x0C000000ull) & ~0xFFFFull;
    UINT64 end = (UINT64)executableBase + 0x70000000ull;
    for (UINT64 candidate = start; candidate < end; candidate += 0x10000ull) {
        BYTE* allocation = (BYTE*)VirtualAlloc(
            (void*)candidate, size, MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE
        );
        if (allocation) return allocation;
    }
    return 0;
}

static bool install_one_player_handle_write_trace(
    BYTE* target, BYTE* cave, UINT32 eventId
) {
    static const BYTE expected[8] = {
        0xC5,0xFC,0x11,0x8F,0x60,0x0C,0x00,0x00
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;
    memcpy(
        cave, PLAYER_HANDLE_WRITE_TRACE_TEMPLATE,
        sizeof(PLAYER_HANDLE_WRITE_TRACE_TEMPLATE)
    );
    UINT64 stateAddress = (UINT64)&g_carrierRideLinkState;
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(cave + 10u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + 42u, &continuation, sizeof(continuation));
    memcpy(cave + 132u, &eventId, sizeof(eventId));
    memcpy(cave + 151u, &continuation, sizeof(continuation));
    FlushInstructionCache(
        (HANDLE)(INT64)-1, cave, sizeof(PLAYER_HANDLE_WRITE_TRACE_TEMPLATE)
    );

    INT64 displacement = (INT64)cave - (INT64)(target + 5u);
    if (displacement < (INT64)-0x80000000ll ||
        displacement > (INT64)0x7FFFFFFFll) return false;
    BYTE replacement[8] = { 0xE9,0,0,0,0, 0x90,0x90,0x90 };
    LONG relative = (LONG)displacement;
    memcpy(replacement + 1u, &relative, sizeof(relative));

    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount,
            target, sizeof(replacement)
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(
            target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection
        )) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    return installed;
}

static bool install_player_handle_write_traces(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    const UINT32 sites[3] = {
        RVA_PLAYER_CARRIER_WRITE_1,
        RVA_PLAYER_CARRIER_WRITE_2,
        RVA_PLAYER_CARRIER_WRITE_3
    };
    const UINT32 eventIds[3] = {
        0x1F5976C4u,
        0x2CEFDD76u,
        0x35A87951u
    };
    static const BYTE expected[8] = {
        0xC5,0xFC,0x11,0x8F,0x60,0x0C,0x00,0x00
    };
    for (UINT32 index = 0; index < 3u; ++index) {
        BYTE* target = base + sites[index];
        if (!readable_range(target, sizeof(expected)) ||
            !bytes_equal(target, expected, sizeof(expected))) return false;
    }
    SIZE_T stride = 192u;
    BYTE* caves = allocate_trace_caves_near(base, stride * 3u);
    if (!caves) return false;
    for (UINT32 index = 0; index < 3u; ++index) {
        if (!install_one_player_handle_write_trace(
                base + sites[index], caves + stride * index, eventIds[index]
            )) return false;
    }
    return true;
}

// Direct qword writers missed by the 32-byte event-block trace above. Each
// cave records the exact site only when its base equals the Coffin transition's
// captured player state, then replays the native store without calling out.
static const BYTE DIRECT_WRITE_RCX_RAX_TEMPLATE[94] = {
    0xF3,0x0F,0x1E,0xFA, 0x9C,0x41,0x52,0x41,0x53,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x39,0x4A,0x08, 0x75,0x2B,
    0x4C,0x8B,0x99,0x60,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x50, 0x49,0x89,0x42,0x58,
    0x4C,0x8B,0x99,0x68,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x60, 0x4D,0x89,0x5A,0x68,
    0x41,0xC7,0x42,0x48, 0,0,0,0,
    0xF0,0x41,0xFF,0x42,0x40,
    0x48,0x89,0x81,0x60,0x0C,0x00,0x00,
    0x41,0x5B,0x41,0x5A,0x9D,
    0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0
};

static const BYTE DIRECT_WRITE_RSI_RAX_TEMPLATE[94] = {
    0xF3,0x0F,0x1E,0xFA, 0x9C,0x41,0x52,0x41,0x53,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x39,0x72,0x08, 0x75,0x2B,
    0x4C,0x8B,0x9E,0x60,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x50, 0x49,0x89,0x42,0x58,
    0x4C,0x8B,0x9E,0x68,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x60, 0x4D,0x89,0x5A,0x68,
    0x41,0xC7,0x42,0x48, 0,0,0,0,
    0xF0,0x41,0xFF,0x42,0x40,
    0x48,0x89,0x86,0x60,0x0C,0x00,0x00,
    0x41,0x5B,0x41,0x5A,0x9D,
    0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0
};

static const BYTE DIRECT_WRITE_RSI_MINUS_ONE_TEMPLATE[105] = {
    0xF3,0x0F,0x1E,0xFA, 0x9C,0x41,0x52,0x41,0x53,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x39,0x72,0x08, 0x75,0x32,
    0x4C,0x8B,0x9E,0x60,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x50,
    0x49,0xC7,0xC3,0xFF,0xFF,0xFF,0xFF,
    0x4D,0x89,0x5A,0x58,
    0x4C,0x8B,0x9E,0x68,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x60, 0x4D,0x89,0x5A,0x68,
    0x41,0xC7,0x42,0x48, 0,0,0,0,
    0xF0,0x41,0xFF,0x42,0x40,
    0x48,0xC7,0x86,0x60,0x0C,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
    0x41,0x5B,0x41,0x5A,0x9D,
    0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0
};

static const BYTE DIRECT_WRITE_RAX_MINUS_ONE_TEMPLATE[105] = {
    0xF3,0x0F,0x1E,0xFA, 0x9C,0x41,0x52,0x41,0x53,
    0x49,0xBA, 0,0,0,0,0,0,0,0,
    0x49,0x39,0x42,0x08, 0x75,0x32,
    0x4C,0x8B,0x98,0x60,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x50,
    0x49,0xC7,0xC3,0xFF,0xFF,0xFF,0xFF,
    0x4D,0x89,0x5A,0x58,
    0x4C,0x8B,0x98,0x68,0x0C,0x00,0x00,
    0x4D,0x89,0x5A,0x60, 0x4D,0x89,0x5A,0x68,
    0x41,0xC7,0x42,0x48, 0,0,0,0,
    0xF0,0x41,0xFF,0x42,0x40,
    0x48,0xC7,0x80,0x60,0x0C,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
    0x41,0x5B,0x41,0x5A,0x9D,
    0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0
};

static bool install_direct_carrier_write_trace(
    BYTE* target, BYTE* cave, const BYTE* expected, SIZE_T instructionSize,
    const BYTE* traceTemplate, SIZE_T templateSize, UINT32 siteId,
    SIZE_T eventOffset, SIZE_T continuationOffset
) {
    if (!readable_range(target, instructionSize) ||
        !bytes_equal(target, expected, instructionSize)) return false;
    memcpy(cave, traceTemplate, templateSize);
    UINT64 stateAddress = (UINT64)&g_carrierRideLinkState;
    UINT64 continuation = (UINT64)(target + instructionSize);
    memcpy(cave + 11u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + eventOffset, &siteId, sizeof(siteId));
    memcpy(cave + continuationOffset, &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, cave, templateSize);

    INT64 displacement = (INT64)cave - (INT64)(target + 5u);
    if (displacement < (INT64)-0x80000000ll ||
        displacement > (INT64)0x7FFFFFFFll) return false;
    BYTE replacement[11] = {
        0xE9,0,0,0,0, 0x90,0x90,0x90,0x90,0x90,0x90
    };
    LONG relative = (LONG)displacement;
    memcpy(replacement + 1u, &relative, sizeof(relative));

    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(
            g_suspendedThreadHandles, &g_suspendedThreadCount,
            target, instructionSize
        );
        if (!threadsFrozen) Sleep(1);
    }
    if (!threadsFrozen) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(
            target, instructionSize, PAGE_EXECUTE_READWRITE, &oldProtection
        )) {
        resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
        g_suspendedThreadCount = 0;
        return false;
    }
    memcpy(target, replacement, instructionSize);
    FlushInstructionCache((HANDLE)(INT64)-1, target, instructionSize);
    bool installed = bytes_equal(target, replacement, instructionSize);
    if (!installed) {
        memcpy(target, expected, instructionSize);
        FlushInstructionCache((HANDLE)(INT64)-1, target, instructionSize);
    }
    DWORD ignored = 0;
    VirtualProtect(target, instructionSize, oldProtection, &ignored);
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
    return installed;
}

static bool install_direct_carrier_write_traces(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    static const BYTE writeRcxRax[7] = {
        0x48,0x89,0x81,0x60,0x0C,0x00,0x00
    };
    static const BYTE writeRsiRax[7] = {
        0x48,0x89,0x86,0x60,0x0C,0x00,0x00
    };
    static const BYTE writeRsiMinusOne[11] = {
        0x48,0xC7,0x86,0x60,0x0C,0x00,0x00,0xFF,0xFF,0xFF,0xFF
    };
    static const BYTE writeRaxMinusOne[11] = {
        0x48,0xC7,0x80,0x60,0x0C,0x00,0x00,0xFF,0xFF,0xFF,0xFF
    };
    struct DirectTraceSite {
        UINT32 rva;
        const BYTE* expected;
        SIZE_T instructionSize;
        const BYTE* traceTemplate;
        SIZE_T templateSize;
        UINT32 siteId;
        SIZE_T eventOffset;
        SIZE_T continuationOffset;
    };
    const DirectTraceSite sites[5] = {
        { RVA_PLAYER_CARRIER_DIRECT_EVENT, writeRcxRax, 7u,
          DIRECT_WRITE_RCX_RAX_TEMPLATE, 94u, 0x073C6B2Eu, 59u, 86u },
        { RVA_PLAYER_CARRIER_ATTACH, writeRsiRax, 7u,
          DIRECT_WRITE_RSI_RAX_TEMPLATE, 94u, 0x15783361u, 59u, 86u },
        { RVA_PLAYER_CARRIER_ATTACH_NULL, writeRsiMinusOne, 11u,
          DIRECT_WRITE_RSI_MINUS_ONE_TEMPLATE, 105u, 0x15783362u, 66u, 97u },
        { RVA_PLAYER_CARRIER_DETACH_EVENT, writeRsiMinusOne, 11u,
          DIRECT_WRITE_RSI_MINUS_ONE_TEMPLATE, 105u, 0x13754FE0u, 66u, 97u },
        { RVA_RIDE_CARRIER_DIRECT_CLEAR, writeRaxMinusOne, 11u,
          DIRECT_WRITE_RAX_MINUS_ONE_TEMPLATE, 105u, 0x00F9A9BFu, 66u, 97u }
    };
    for (UINT32 index = 0; index < 5u; ++index) {
        BYTE* target = base + sites[index].rva;
        if (!readable_range(target, sites[index].instructionSize) ||
            !bytes_equal(target, sites[index].expected, sites[index].instructionSize)) {
            return false;
        }
    }
    SIZE_T stride = 128u;
    BYTE* caves = allocate_trace_caves_near(base, stride * 5u);
    if (!caves) return false;
    for (UINT32 index = 0; index < 5u; ++index) {
        const DirectTraceSite* site = &sites[index];
        if (!install_direct_carrier_write_trace(
                base + site->rva, caves + stride * index,
                site->expected, site->instructionSize,
                site->traceTemplate, site->templateSize, site->siteId,
                site->eventOffset, site->continuationOffset
            )) return false;
    }
    return true;
}

static bool install_carrier_trace_probes(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    const UINT32 probeCount =
        (UINT32)(sizeof(g_carrierTraceProbes) / sizeof(g_carrierTraceProbes[0]));

    // Validate every owner table and slot before publishing any shim.
    for (UINT32 index = 0; index < probeCount; ++index) {
        CarrierTraceProbe* probe = &g_carrierTraceProbes[index];
        void** entry = (void**)(base + probe->vtableRva) + probe->slot;
        if (!readable_range(entry, sizeof(void*)) ||
            *entry != (void*)(base + probe->expectedFunctionRva)) return false;
    }

    for (UINT32 index = 0; index < probeCount; ++index) {
        CarrierTraceProbe* probe = &g_carrierTraceProbes[index];
        if (index == 0u) {
            BYTE* shim = (BYTE*)VirtualAlloc(
                0, sizeof(CARRIER_RIDE_ENTER_TRACE_SHIM_TEMPLATE),
                MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
            );
            if (!shim) return false;
            memcpy(
                shim, CARRIER_RIDE_ENTER_TRACE_SHIM_TEMPLATE,
                sizeof(CARRIER_RIDE_ENTER_TRACE_SHIM_TEMPLATE)
            );
            UINT64 counterAddress = (UINT64)&probe->count;
            UINT64 originalAddress = (UINT64)(base + probe->expectedFunctionRva);
            memcpy(shim + 8u, &counterAddress, sizeof(counterAddress));
            memcpy(shim + 41u, &originalAddress, sizeof(originalAddress));
            FlushInstructionCache(
                (HANDLE)(INT64)-1, shim,
                sizeof(CARRIER_RIDE_ENTER_TRACE_SHIM_TEMPLATE)
            );

            void** entry = (void**)(base + probe->vtableRva) + probe->slot;
            DWORD oldProtection = 0;
            if (!VirtualProtect(
                    entry, sizeof(void*), PAGE_READWRITE, &oldProtection
                )) return false;
            __atomic_store_n((UINT64*)entry, (UINT64)shim, __ATOMIC_RELEASE);
            DWORD ignored = 0;
            VirtualProtect(entry, sizeof(void*), oldProtection, &ignored);
            if (__atomic_load_n((UINT64*)entry, __ATOMIC_ACQUIRE) !=
                (UINT64)shim) return false;
            g_carrierTracePlayerState = (volatile UINT64*)(shim + 49u);
            continue;
        }
        BYTE* shim = (BYTE*)VirtualAlloc(
            0, sizeof(CARRIER_TRACE_SHIM_TEMPLATE), MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE
        );
        if (!shim) return false;
        memcpy(shim, CARRIER_TRACE_SHIM_TEMPLATE, sizeof(CARRIER_TRACE_SHIM_TEMPLATE));
        UINT64 counterAddress = (UINT64)&probe->count;
        UINT64 originalAddress = (UINT64)(base + probe->expectedFunctionRva);
        memcpy(shim + 8u, &counterAddress, sizeof(counterAddress));
        memcpy(shim + 27u, &originalAddress, sizeof(originalAddress));
        FlushInstructionCache((HANDLE)(INT64)-1, shim, sizeof(CARRIER_TRACE_SHIM_TEMPLATE));

        void** entry = (void**)(base + probe->vtableRva) + probe->slot;
        DWORD oldProtection = 0;
        if (!VirtualProtect(entry, sizeof(void*), PAGE_READWRITE, &oldProtection)) return false;
        __atomic_store_n((UINT64*)entry, (UINT64)shim, __ATOMIC_RELEASE);
        DWORD ignored = 0;
        VirtualProtect(entry, sizeof(void*), oldProtection, &ignored);
        if (__atomic_load_n((UINT64*)entry, __ATOMIC_ACQUIRE) != (UINT64)shim) return false;
    }
    return true;
}

#endif

static bool drive_coupling_needed() {
    return g_driveForcePercent != 100 || g_gearRatioPercent < 100;
}

static bool steering_coupling_needed() {
    return g_steeringResponsePercent != 100;
}

static bool publish_drive_factor_percent(int percent) {
    if (!g_driveHookInstalled || !g_driveFactorBits) return false;
    float factor = (float)percent / 100.0f;
    LONG bits = 0;
    memcpy(&bits, &factor, sizeof(bits));
    __atomic_store_n(g_driveFactorBits, bits, __ATOMIC_RELEASE);
    return __atomic_load_n(g_driveFactorBits, __ATOMIC_ACQUIRE) == bits;
}

static bool publish_steering_response_percent(int percent) {
    if (!g_steeringHookInstalled || !g_steeringSlopeBits) return false;
    float slope = (float)(percent - 100) / 14000.0f;
    LONG bits = 0;
    memcpy(&bits, &slope, sizeof(bits));
    __atomic_store_n(g_steeringSlopeBits, bits, __ATOMIC_RELEASE);
    return __atomic_load_n(g_steeringSlopeBits, __ATOMIC_ACQUIRE) == bits;
}

static const char* rtti_type_name(void* object, void** vtable) {
    if (!object || !vtable || !readable_range(vtable, sizeof(void*))) return 0;
    typedef void* (__fastcall* GetRttiFn)(void*);
    GetRttiFn getRtti = (GetRttiFn)vtable[0];
    if (!getRtti || !readable_range((void*)getRtti, 1)) return 0;
    void* rtti = getRtti(object);
    if (!readable_range(rtti, 0x48)) return 0;
    if (*((BYTE*)rtti + 4) != 4) return 0;
    const char* typeName = *(const char**)((BYTE*)rtti + 0x40);
    return readable_range(typeName, 1) ? typeName : 0;
}

static bool object_has_uuid(void* object, UINT64 low, UINT64 high) {
    if (!object || !readable_range((BYTE*)object + 0x10u, 16u)) return false;
    const UINT64* uuid = (const UINT64*)((BYTE*)object + 0x10u);
    return uuid[0] == low && uuid[1] == high;
}

static bool object_has_exact_type(void* object, const char* expectedType) {
    if (!object || !readable_range(object, sizeof(void*))) return false;
    void** vtable = *(void***)object;
    if (!vtable) return false;
    ++g_rttiLookups;
    const char* typeName = rtti_type_name(object, vtable);
    return typeName && string_compare(typeName, expectedType) == 0;
}

static bool patch_standard_speed(void* config, float desiredNormal) {
    if (!g_patchStandardSpeed) { g_standardReady = 1; return true; }
    if (!config || !readable_range((BYTE*)config + OFF_STANDARD_SPEED, sizeof(float))) return false;
    float* standardSpeed = (float*)((BYTE*)config + OFF_STANDARD_SPEED);
    float oldValue = *standardSpeed;
    bool wasOurPreviousValue=config==g_seenRideConfig&&g_lastAppliedStandardSpeed>0.0f&&
        f_near(oldValue,g_lastAppliedStandardSpeed,0.05f);
    if (!value_is_native_or_compatible(oldValue, 40.0f, desiredNormal)&&
        !wasOurPreviousValue) {
        log_line("WARNING: DSPlayerRideCoffinConfig StandardSpeed is unexpected; field left unchanged.", true);
        return false;
    }
    float finalValue = f_max(oldValue, desiredNormal);
    int result = write_float_checked(standardSpeed, finalValue);
    if (!result) {
        log_line("ERROR: Could not write DSPlayerRideCoffinConfig StandardSpeed.", true);
        return false;
    }
    g_standardReady = 1;
    g_lastAppliedStandardSpeed = finalValue;
    LogBuffer message;
    log_init(&message); log_prefix(&message);
    log_text(&message, "Coffin rider StandardSpeed "); log_float2(&message, oldValue);
    log_text(&message, " -> "); log_float2(&message, finalValue); log_text(&message, " km/h\r\n");
    append_log(&message);
    return true;
}

static bool patch_coffin_physics(void* resource) {
    if (!resource || !readable_range((BYTE*)resource + OFF_SLIP_SPEED, sizeof(float))) return false;
    bool driveCoupling = drive_coupling_needed();
    bool steeringCoupling = steering_coupling_needed();
    if (driveCoupling &&
        (!g_driveHookInstalled || !g_driveFactorBits ||
         !publish_drive_factor_percent(100))) {
        return false;
    }
    if (steeringCoupling &&
        (!g_steeringHookInstalled || !g_steeringSlopeBits ||
         !publish_steering_response_percent(100))) {
        if (driveCoupling) publish_drive_factor_percent(100);
        return false;
    }
    float* steering = (float*)((BYTE*)resource + OFF_STEERING_DEGREE);
    float* land = (float*)((BYTE*)resource + OFF_LAND_TOP_SPEED);
    float* gearRatio = (float*)((BYTE*)resource + OFF_FINAL_GEAR_RATIO);
    float* landBoost = (float*)((BYTE*)resource + OFF_LAND_BOOST_TOP_SPEED);
    float* water = (float*)((BYTE*)resource + OFF_WATER_TOP_SPEED);
    float* waterBoost = (float*)((BYTE*)resource + OFF_WATER_BOOST_TOP_SPEED);
    float* wetGrip = (float*)((BYTE*)resource + OFF_WET_SIDE_GRIP);
    float* slip = (float*)((BYTE*)resource + OFF_SLIP_SPEED);

    float oldSteering = *steering;
    float oldLand = *land;
    float oldGearRatio = *gearRatio;
    float oldLandBoost = *landBoost;
    float oldWater = *water;
    float oldWaterBoost = *waterBoost;
    float oldWetGrip = *wetGrip;
    float oldSlip = *slip;

    bool isOurScaledWater = g_scaleWaterCaps &&
        g_waterBaseline > 0.0f && g_waterBoostBaseline > 0.0f &&
        f_near(oldWater, g_lastAppliedWater, 0.05f) &&
        f_near(oldWaterBoost, g_lastAppliedWaterBoost, 0.05f);
    float baseWater = isOurScaledWater ? g_waterBaseline : oldWater;
    float baseWaterBoost = isOurScaledWater ? g_waterBoostBaseline : oldWaterBoost;
    bool isOurScaledGear = g_gearRatioPercent < 100 &&
        g_gearRatioBaseline > 0.0f &&
        f_near(oldGearRatio, g_lastAppliedGearRatio, 0.05f);
    float baseGearRatio = isOurScaledGear ? g_gearRatioBaseline : oldGearRatio;
    bool isOurScaledSteering = g_steeringAnglePercent > 100 &&
        g_steeringBaseline > 0.0f &&
        f_near(oldSteering, g_lastAppliedSteering, 0.05f);
    float baseSteering = isOurScaledSteering ? g_steeringBaseline : oldSteering;
    bool patchWetGrip = g_wetGripPercent > 100;
    bool isOurScaledWetGrip = patchWetGrip && resource == g_seenPhysicsResource &&
        f_in_range(g_wetGripBaseline, 0.08f, 0.10f) &&
        f_in_range(g_lastAppliedWetGrip, 0.09f, 0.50f) &&
        f_near(oldWetGrip, g_lastAppliedWetGrip, 0.0005f);
    float baseWetGrip = isOurScaledWetGrip ? g_wetGripBaseline : oldWetGrip;

    // Exact native steering anchor (or our idempotent prior value) plus broad
    // plausibility for water values modified by another compatible speed mod.
    if (!f_near(baseSteering, 50.0f, 0.05f) ||
        baseWater < 10.0f || baseWater > 250.0f ||
        baseWaterBoost < 10.0f || baseWaterBoost > 300.0f) {
        log_line("WARNING: DSPhysicsCoffinResource layout/value guard failed; resource skipped.", true);
        return false;
    }
    if (g_gearRatioPercent < 100 && !f_near(baseGearRatio, 15.0f, 0.05f)) {
        log_line("WARNING: Coffin Board FinalGearRatio conflicts with another mod; resource skipped.", true);
        return false;
    }
    if (patchWetGrip &&
        (!f_in_range(oldWetGrip, 0.0f, 1.0f) ||
         !f_near(baseWetGrip, 0.09f, 0.0005f))) {
        log_line("WARNING: Coffin Board wet side-grip is non-finite, implausible, or conflicts with another mod; resource skipped.", true);
        return false;
    }

    float desiredLand = baseWater * (float)g_normalPercent / 100.0f;
    float desiredBoost = baseWaterBoost * (float)g_boostPercent / 100.0f;
    if (desiredLand > 650.0f || desiredBoost > 850.0f) {
        log_line("WARNING: calculated Coffin Board speed is outside the safety range.", true);
        return false;
    }
    if (!value_is_native_or_compatible(oldLand, 40.0f, desiredLand) ||
        !value_is_native_or_compatible(oldLandBoost, 60.0f, desiredBoost)) {
        log_line("WARNING: Coffin Board land-speed fields conflict with another mod; resource skipped.", true);
        return false;
    }
    float finalLand = f_max(oldLand, desiredLand);
    float finalBoost = f_max(oldLandBoost, desiredBoost);
    float finalWater = g_scaleWaterCaps ? f_max(oldWater, desiredLand) : oldWater;
    float finalWaterBoost = g_scaleWaterCaps ? f_max(oldWaterBoost, desiredBoost) : oldWaterBoost;
    float finalGearRatio = g_gearRatioPercent < 100 ?
        baseGearRatio * (float)g_gearRatioPercent / 100.0f : oldGearRatio;
    float finalSteering = g_steeringAnglePercent > 100 ?
        baseSteering * (float)g_steeringAnglePercent / 100.0f : oldSteering;
    float finalWetGrip = patchWetGrip ?
        baseWetGrip * (float)g_wetGripPercent / 100.0f : oldWetGrip;
    if (finalLand > 650.0f || finalWater > 650.0f ||
        finalBoost > 850.0f || finalWaterBoost > 850.0f) {
        log_line("WARNING: existing Coffin Board cap is outside the safety range; resource skipped.", true);
        return false;
    }
    if (g_gearRatioPercent < 100 && (finalGearRatio < 1.5f || finalGearRatio > 30.0f)) {
        log_line("WARNING: calculated Coffin Board gear ratio is outside the safety range.", true);
        return false;
    }
    if (finalSteering < 50.0f || finalSteering > 80.0f) {
        log_line("WARNING: calculated Coffin Board steering angle is outside the safety range.", true);
        return false;
    }
    if (patchWetGrip && !f_in_range(finalWetGrip, 0.09f, 0.50f)) {
        log_line("WARNING: calculated Coffin Board wet side-grip is outside the safety range.", true);
        return false;
    }
    float finalSlip = oldSlip;
    if (g_raiseSlipThreshold) {
        if (!(f_near(oldSlip, 39.0f, 0.05f) || oldSlip >= finalLand)) {
            log_line("WARNING: Coffin Board SlipSpeed is unexpected; resource skipped atomically.", true);
            return false;
        }
        finalSlip = f_max(oldSlip, finalLand);
    }

    int steeringResult = g_steeringAnglePercent > 100 ?
        write_float_checked(steering, finalSteering) : 1;
    int landResult = steeringResult ? write_float_checked(land, finalLand) : 0;
    int boostResult = landResult ? write_float_checked(landBoost, finalBoost) : 0;
    int waterResult = boostResult ?
        (g_scaleWaterCaps ? write_float_checked(water, finalWater) : 1) : 0;
    int waterBoostResult = waterResult ?
        (g_scaleWaterCaps ? write_float_checked(waterBoost, finalWaterBoost) : 1) : 0;
    int gearResult = waterBoostResult ?
        (g_gearRatioPercent < 100 ? write_float_checked(gearRatio, finalGearRatio) : 1) : 0;
    int slipResult = gearResult ?
        (g_raiseSlipThreshold ? write_float_checked(slip, finalSlip) : 1) : 0;
    int wetGripResult = slipResult ?
        (patchWetGrip ?
            write_float_checked_tolerance(wetGrip, finalWetGrip, 0.00005f) : 1) : 0;
    bool driveFactorResult = wetGripResult &&
        (!driveCoupling || publish_drive_factor_percent(g_driveForcePercent));
    bool steeringFactorResult = driveFactorResult &&
        (!steeringCoupling ||
         publish_steering_response_percent(g_steeringResponsePercent));
    if (!steeringResult || !landResult || !boostResult || !waterResult || !waterBoostResult ||
        !gearResult || !slipResult || !wetGripResult ||
        !driveFactorResult || !steeringFactorResult) {
        bool rolledBack = true;
        if (driveCoupling && !publish_drive_factor_percent(100)) rolledBack = false;
        if (steeringCoupling && !publish_steering_response_percent(100)) rolledBack = false;
        if (wetGripResult == 2 &&
            !write_float_checked_tolerance(wetGrip, oldWetGrip, 0.00005f)) rolledBack = false;
        if (slipResult == 2 && !write_float_checked(slip, oldSlip)) rolledBack = false;
        if (gearResult == 2 && !write_float_checked(gearRatio, oldGearRatio)) rolledBack = false;
        if (waterBoostResult == 2 && !write_float_checked(waterBoost, oldWaterBoost)) rolledBack = false;
        if (waterResult == 2 && !write_float_checked(water, oldWater)) rolledBack = false;
        if (boostResult == 2 && !write_float_checked(landBoost, oldLandBoost)) rolledBack = false;
        if (landResult == 2 && !write_float_checked(land, oldLand)) rolledBack = false;
        if (steeringResult == 2 && !write_float_checked(steering, oldSteering)) rolledBack = false;
        log_line(
            rolledBack ?
                "ERROR: Coffin Board physics-value transaction failed; completed writes were rolled back." :
                "FATAL: Coffin Board physics-value transaction and rollback were incomplete; restart the game.",
            true
        );
        return false;
    }

    if (g_scaleWaterCaps) {
        g_waterBaseline = baseWater;
        g_waterBoostBaseline = baseWaterBoost;
        g_lastAppliedWater = finalWater;
        g_lastAppliedWaterBoost = finalWaterBoost;
    }
    if (g_gearRatioPercent < 100) {
        g_gearRatioBaseline = baseGearRatio;
        g_lastAppliedGearRatio = finalGearRatio;
    }
    if (g_steeringAnglePercent > 100) {
        g_steeringBaseline = baseSteering;
        g_lastAppliedSteering = finalSteering;
    }
    if (patchWetGrip) {
        g_wetGripBaseline = baseWetGrip;
        g_lastAppliedWetGrip = finalWetGrip;
    }
    g_desiredNormalSpeed = finalLand;
    g_seenPhysicsResource = resource;
    g_physicsReady = 1;
    LogBuffer message;
    log_init(&message); log_prefix(&message);
    log_text(&message, "Coffin physics land "); log_float2(&message, oldLand);
    log_text(&message, " -> "); log_float2(&message, finalLand);
    log_text(&message, ", boost "); log_float2(&message, oldLandBoost);
    log_text(&message, " -> "); log_float2(&message, finalBoost);
    log_text(&message, ", water "); log_float2(&message, oldWater);
    log_text(&message, " -> "); log_float2(&message, finalWater);
    log_text(&message, ", water boost "); log_float2(&message, oldWaterBoost);
    log_text(&message, " -> "); log_float2(&message, finalWaterBoost);
    log_text(&message, ", slip "); log_float2(&message, oldSlip);
    log_text(&message, " -> "); log_float2(&message, finalSlip); log_text(&message, " km/h");
    log_text(&message, ", gear ratio "); log_float2(&message, oldGearRatio);
    log_text(&message, " -> "); log_float2(&message, finalGearRatio);
    if (g_steeringAnglePercent > 100) {
        log_text(&message, ", steering "); log_float2(&message, oldSteering);
        log_text(&message, " -> "); log_float2(&message, finalSteering);
        log_text(&message, " deg");
    } else {
        log_text(&message, ", steering native");
    }
    if (patchWetGrip) {
        log_text(&message, ", wet side-grip "); log_float2(&message, oldWetGrip);
        log_text(&message, " -> "); log_float2(&message, finalWetGrip);
    } else {
        log_text(&message, ", wet side-grip native");
    }
    log_text(&message, ", drive factor ");
    log_uint(&message, (UINT64)g_driveForcePercent); log_text(&message, "%");
    if (steeringCoupling) {
        log_text(&message, ", steering response ");
        log_uint(&message, (UINT64)g_steeringResponsePercent);
        log_text(&message, "% active");
    }
    log_text(&message, ", telemetry ");
    log_text(&message, g_speedTelemetry ? "active" : "off");
    log_text(&message, ".\r\n");
    append_log(&message);

    if (g_seenRideConfig && !g_standardReady) {
        patch_standard_speed(g_seenRideConfig, g_desiredNormalSpeed);
    }
    return true;
}

struct RawArray { UINT32 count; UINT32 capacity; void* entries; };
struct StreamingEvents { void** vtable; };
static void mark_complete_if_ready() {
    bool standardDone = !g_patchStandardSpeed || g_standardReady;
    if (!patch_complete_acquire() && g_physicsReady && standardDone) {
        publish_patch_complete(true);
        LogBuffer message;
        log_init(&message); log_prefix(&message);
        log_text(&message, "Patch complete. groups="); log_uint(&message, g_callbackGroups);
        log_text(&message, " objects="); log_uint(&message, g_callbackObjects);
        log_text(&message, " rttiLookups="); log_uint(&message, g_rttiLookups);
        log_text(&message, " targetChecks="); log_uint(&message, g_targetChecks);
        log_text(&message, ". One-shot listener removal is now pending on the worker.\r\n");
        append_log(&message);
    }
}

static bool validate_group_array(const RawArray* objects) {
    if (!objects || !readable_range(objects, sizeof(RawArray))) return false;
    if (objects->count < 1u || objects->count > 1000000u ||
        objects->capacity < objects->count || objects->capacity > 1000000u ||
        !objects->entries) return false;
    return readable_range(objects->entries, sizeof(void*));
}

static bool group_object_at(const RawArray* objects, UINT32 index, void** object) {
    if (!objects || !object || objects->count <= index) return false;
    void** slot = (void**)((BYTE*)objects->entries + (SIZE_T)index * sizeof(void*));
    if (!readable_range(slot, sizeof(void*))) return false;
    *object = *slot;
    return true;
}

static void inspect_target_positions(const RawArray* objects, bool countTelemetry) {
    if (!validate_group_array(objects)) return;
    AcquireSRWLockExclusive(&g_targetStateLock);
    if (countTelemetry) {
        ++g_callbackGroups;
        g_callbackObjects += objects->count;
        if (g_maxScanGroups && g_callbackGroups > g_maxScanGroups) {
            publish_patch_complete(true);
            log_line("WARNING: MaxScanGroups reached before all Coffin Board resources were found.", true);
            ReleaseSRWLockExclusive(&g_targetStateLock);
            return;
        }
    }
    // Group 499 has at least 87,603 objects. v0.1.0 rejected the whole
    // callback because of its 65,536-object guard and then scanned every later
    // group forever. The exact graph index turns the callback into O(1).
    void* candidate = 0;
    if (!g_physicsReady &&
        group_object_at(objects, COFFIN_PHYSICS_OBJECT_INDEX, &candidate)) {
        ++g_targetChecks;
        if (object_has_uuid(candidate, COFFIN_PHYSICS_UUID_LOW, COFFIN_PHYSICS_UUID_HIGH) &&
            object_has_exact_type(candidate, "DSPhysicsCoffinResource")) {
            patch_coffin_physics(candidate);
        }
    }

    candidate = 0;
    if (g_patchStandardSpeed && !g_standardReady &&
        group_object_at(objects, RIDE_COFFIN_OBJECT_INDEX, &candidate)) {
        ++g_targetChecks;
        if (object_has_uuid(candidate, RIDE_COFFIN_UUID_LOW, RIDE_COFFIN_UUID_HIGH) &&
            object_has_exact_type(candidate, "DSPlayerRideCoffinConfig")) {
            g_seenRideConfig = candidate;
            if (g_physicsReady) patch_standard_speed(candidate, g_desiredNormalSpeed);
        }
    }
    mark_complete_if_ready();
    ReleaseSRWLockExclusive(&g_targetStateLock);
}

static void inspect_target_unload_positions(const RawArray* objects) {
    if (!validate_group_array(objects)) return;
    AcquireSRWLockExclusive(&g_targetStateLock);
    if (!g_seenPhysicsResource && !g_seenRideConfig) {
        ReleaseSRWLockExclusive(&g_targetStateLock);
        return;
    }
    void* candidate = 0;
    if (g_seenPhysicsResource &&
        group_object_at(objects, COFFIN_PHYSICS_OBJECT_INDEX, &candidate) &&
        candidate == g_seenPhysicsResource) {
        bool driveNeutral = !drive_coupling_needed() || publish_drive_factor_percent(100);
        bool steeringNeutral = !steering_coupling_needed() ||
            publish_steering_response_percent(100);
        bool factorsNeutral = driveNeutral && steeringNeutral;
        g_seenPhysicsResource = 0;
        g_wetGripBaseline = 0.0f;
        g_lastAppliedWetGrip = 0.0f;
        g_physicsReady = 0;
        publish_patch_complete(false);
        if (g_patchStandardSpeed && g_seenRideConfig) g_standardReady = 0;
        log_line(
            factorsNeutral ?
                "STATE: Coffin physics resource unloaded; runtime factors neutralized and target state reset within the one-shot window." :
                "FATAL: Coffin physics resource unloaded but a runtime factor could not be neutralized; restart the game.",
            true
        );
    }
    candidate = 0;
    if (g_seenRideConfig &&
        group_object_at(objects, RIDE_COFFIN_OBJECT_INDEX, &candidate) &&
        candidate == g_seenRideConfig) {
        g_seenRideConfig = 0;
        g_lastAppliedStandardSpeed = 0.0f;
        if (g_patchStandardSpeed) {
            g_standardReady = 0;
            publish_patch_complete(false);
            log_line("STATE: optional Coffin rider config unloaded; target state reset within the one-shot window.", true);
        }
    }
    ReleaseSRWLockExclusive(&g_targetStateLock);
}

static void __fastcall on_finish_load(StreamingEvents*, const RawArray* objects) {
    if (!g_enabled || patch_complete_acquire()) return;
    inspect_target_positions(objects, true);
}

static void __fastcall on_before_unload(StreamingEvents*, const RawArray* objects) {
    inspect_target_unload_positions(objects);
}
static void __fastcall on_load_asset(StreamingEvents*, const RawArray*) {}
static void* g_listenerVtable[3] = {
    (void*)&on_finish_load,
    (void*)&on_before_unload,
    (void*)&on_load_asset
};
static StreamingEvents g_listener = { g_listenerVtable };

static UINT64 resolve_rip(UINT64 instruction, UINT32 displacementOffset) {
    int displacement = *(int*)(instruction + displacementOffset);
    return instruction + displacementOffset + 4u + (INT64)displacement;
}
static UINT64 find_streaming_manager_global(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    BYTE* candidate = base + RVA_STREAMING_SIGNATURE;
    if (!readable_range(candidate, 28u)) return 0;
    static const BYTE signature[28] = {
        0x48,0x89,0x05,0,0,0,0,0xE8,0,0,0,0,0x33,0xD2,0x41,0xB8,
        0xF8,0x0A,0x00,0x00,0x48,0x8B,0xC8,0x48,0x8B,0xD8,0xE8,0
    };
    static const BYTE mask[28] = {
        1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0
    };
    for (UINT32 index = 0; index < 28u; ++index) {
        if (mask[index] && candidate[index] != signature[index]) return 0;
    }
    UINT64 resolved = resolve_rip((UINT64)candidate, 3u);
    UINT64 expected = (UINT64)(base + RVA_STREAMING_MANAGER_GLOBAL);
    return resolved == expected ? resolved : 0;
}

typedef void (__fastcall* AddListenerFn)(void*, void*);
typedef void (__fastcall* RemoveListenerFn)(void*, void*);

static bool register_streaming_listener_exact(
    void* streamingSystem,
    void** expectedVtable,
    AddListenerFn expectedAddListener,
    RemoveListenerFn expectedRemoveListener
) {
    if (!streamingSystem || !readable_range(streamingSystem, sizeof(void*))) return false;
    void** vtable = *(void***)streamingSystem;
    if (!vtable || !readable_range(vtable, 5u * sizeof(void*))) return false;
    if (vtable != expectedVtable) return false;
    AddListenerFn addListener = (AddListenerFn)vtable[3];
    RemoveListenerFn removeListener = (RemoveListenerFn)vtable[4];
    if (!addListener || addListener != expectedAddListener ||
        !removeListener || removeListener != expectedRemoveListener ||
        listener_state_acquire() != 0) return false;

    addListener(streamingSystem, &g_listener);
    g_streamingSystem = streamingSystem;
    g_removeStreamingListener = removeListener;
    __atomic_store_n(&g_listenerRegistered, 1, __ATOMIC_RELEASE);
    return true;
}

static bool register_streaming_listener(void* manager) {
    if (!manager || !readable_range((BYTE*)manager + 0x578, sizeof(void*))) return false;
    void* streamingSystem = *(void**)((BYTE*)manager + 0x578);
    BYTE* executable = (BYTE*)GetModuleHandleW(0);
    if (!executable) return false;
    return register_streaming_listener_exact(
        streamingSystem,
        (void**)(executable + RVA_STREAMING_SYSTEM_VTABLE),
        (AddListenerFn)(executable + RVA_STREAMING_ADD_LISTENER),
        (RemoveListenerFn)(executable + RVA_STREAMING_REMOVE_LISTENER)
    );
}

// RemoveListener takes the engine's exclusive listener lock. This function is
// called only by the worker, never from a callback (dispatch holds that lock
// shared while invoking callbacks). The CAS makes repeated cleanup attempts a
// no-op and publishes state changes across the worker/streaming threads.
static bool unregister_streaming_listener() {
    LONG state = listener_state_acquire();
    if (state == 0) return true;
    if (state != 1 || !g_streamingSystem || !g_removeStreamingListener) return false;

    LONG expected = 1;
    if (!__atomic_compare_exchange_n(
            &g_listenerRegistered, &expected, 2, false,
            __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        return expected == 0;
    }

    g_removeStreamingListener(g_streamingSystem, &g_listener);
    g_removeStreamingListener = 0;
    g_streamingSystem = 0;
    __atomic_store_n(&g_listenerRegistered, 0, __ATOMIC_RELEASE);
    return true;
}

static DWORD stop_worker_after_listener_error(const char* message) {
    log_line(message, true);
    if (!unregister_streaming_listener()) {
        log_line("LIFECYCLE ERROR: listener cleanup failed after hook installation error; restart DS2 before changing this ASI.", true);
    }
    return 0;
}

static void inspect_already_loaded_group(void* streamingSystem, UINT32 groupId) {
    if (!streamingSystem || !readable_range((BYTE*)streamingSystem + OFF_STREAMING_GROUP_LOCK, sizeof(void*))) return;
    void* lock = (BYTE*)streamingSystem + OFF_STREAMING_GROUP_LOCK;
    AcquireSRWLockShared(lock);

    bool inspected = false;
    UINT32* arrayHeader = (UINT32*)((BYTE*)streamingSystem + OFF_STREAMING_GROUP_COUNT);
    void*** tableField = (void***)((BYTE*)streamingSystem + OFF_STREAMING_GROUP_TABLE);
    if (readable_range(arrayHeader, 2u * sizeof(UINT32)) &&
        readable_range(tableField, sizeof(void*)) &&
        arrayHeader[0] > groupId && arrayHeader[0] <= arrayHeader[1] &&
        arrayHeader[1] < 1000000u && *tableField) {
        BYTE* entry = (BYTE*)(*tableField) + (SIZE_T)groupId * STREAMING_GROUP_ENTRY_STRIDE;
        if (readable_range(entry, 16u)) {
            void* descriptor = *(void**)(entry + 0u);
            void* loadedGroup = *(void**)(entry + 8u);
            if (descriptor && loadedGroup && readable_range(descriptor, sizeof(UINT32)) &&
                *(UINT32*)descriptor == groupId &&
                readable_range((BYTE*)loadedGroup + OFF_LOADED_GROUP_OBJECTS, sizeof(RawArray))) {
                inspect_target_positions(
                    (const RawArray*)((BYTE*)loadedGroup + OFF_LOADED_GROUP_OBJECTS), false
                );
                inspected = true;
            }
        }
    }
    ReleaseSRWLockShared(lock);

    if (inspected && g_debugLog) {
        LogBuffer message;
        log_init(&message); log_prefix(&message);
        log_text(&message, "Backfill inspected loaded streaming group ");
        log_uint(&message, groupId); log_text(&message, ".\r\n");
        append_log(&message);
    }
}

static bool wait_for_patch_completion_with_backfill(
    void* streamingSystem,
    UINT32 maxPolls,
    DWORD pollMilliseconds,
    UINT32 backfillEveryPolls
) {
    if (backfillEveryPolls == 0u) backfillEveryPolls = 1u;
    for (UINT32 poll = 0; poll < maxPolls; ++poll) {
        if (patch_complete_acquire()) return true;
        if ((poll % backfillEveryPolls) == 0u) {
            // AddListener does not replay a group whose listener snapshot was
            // taken just before registration. Recheck the native resident
            // table throughout the bounded discovery window so that startup
            // timing cannot silently leave the speed patch inactive.
            inspect_already_loaded_group(streamingSystem, COFFIN_PHYSICS_GROUP_ID);
            if (g_patchStandardSpeed) {
                inspect_already_loaded_group(streamingSystem, RIDE_COFFIN_GROUP_ID);
            }
            if (patch_complete_acquire()) return true;
        }
        if (pollMilliseconds) Sleep(pollMilliseconds);
    }
    return patch_complete_acquire();
}

struct SteeringTelemetrySample {
    LONG sequence;
    float speedKmh;
    float preClampRadians;
    float finalRadians;
    float rawInput;
    float scale;
};

static bool steering_sample_value_in_range(float value, float minimum, float maximum) {
    return value >= minimum && value <= maximum;
}

static bool read_steering_telemetry_sample(SteeringTelemetrySample* sample) {
    if (!sample || !g_steeringHookInstalled || !g_steeringSampleSequence ||
        !g_steeringSampleSpeedBits || !g_steeringSamplePreClampBits ||
        !g_steeringSampleFinalBits || !g_steeringSampleRawBits ||
        !g_steeringSampleScaleBits) return false;
    for (int attempt = 0; attempt < 4; ++attempt) {
        LONG sequenceBefore = __atomic_load_n(
            g_steeringSampleSequence, __ATOMIC_ACQUIRE
        );
        if (sequenceBefore == 0 || (sequenceBefore & 1) != 0) continue;
        LONG speedBits = __atomic_load_n(
            g_steeringSampleSpeedBits, __ATOMIC_RELAXED
        );
        LONG preClampBits = __atomic_load_n(
            g_steeringSamplePreClampBits, __ATOMIC_RELAXED
        );
        LONG finalBits = __atomic_load_n(
            g_steeringSampleFinalBits, __ATOMIC_RELAXED
        );
        LONG rawBits = __atomic_load_n(
            g_steeringSampleRawBits, __ATOMIC_RELAXED
        );
        LONG scaleBits = __atomic_load_n(
            g_steeringSampleScaleBits, __ATOMIC_RELAXED
        );
        LONG sequenceAfter = __atomic_load_n(
            g_steeringSampleSequence, __ATOMIC_ACQUIRE
        );
        if (sequenceBefore != sequenceAfter || (sequenceAfter & 1) != 0) continue;
        sample->sequence = sequenceAfter;
        memcpy(&sample->speedKmh, &speedBits, sizeof(speedBits));
        memcpy(&sample->preClampRadians, &preClampBits, sizeof(preClampBits));
        memcpy(&sample->finalRadians, &finalBits, sizeof(finalBits));
        memcpy(&sample->rawInput, &rawBits, sizeof(rawBits));
        memcpy(&sample->scale, &scaleBits, sizeof(scaleBits));
        return steering_sample_value_in_range(sample->speedKmh, 0.0f, 1000.0f) &&
            steering_sample_value_in_range(sample->preClampRadians, -16.0f, 16.0f) &&
            steering_sample_value_in_range(sample->finalRadians, -4.0f, 4.0f) &&
            steering_sample_value_in_range(sample->rawInput, -4.0f, 4.0f) &&
            steering_sample_value_in_range(sample->scale, -4.0f, 4.0f);
    }
    return false;
}

static bool steering_sample_hit_clamp(const SteeringTelemetrySample* sample) {
    if (!sample) return false;
    const float clampRadians = 1.483529806f;
    return f_abs(sample->preClampRadians) > clampRadians + 0.0005f &&
        f_near(f_abs(sample->finalRadians), clampRadians, 0.0005f);
}

static void run_speed_telemetry() {
    if (!g_speedTelemetry || !g_driveHookInstalled || !g_driveTelemetryBits) return;
    float runMaximum = 0.0f;
    float lastLoggedMaximum = 0.0f;
    LONG lastSteeringSequence = 0;
    float lastLoggedRaw = 0.0f;
    float lastLoggedScale = 0.0f;
    float lastLoggedPreClamp = 0.0f;
    float lastLoggedFinal = 0.0f;
    bool lastSteeringActive = false;
    bool lastLoggedClamp = false;
    bool steeringHasLogged = false;
    UINT32 steeringPollsSinceLog = 5u;
    for (;;) {
        Sleep(200);
        LONG currentBits = __atomic_load_n(g_driveTelemetryBits, __ATOMIC_RELAXED);
        float current = 0.0f;
        memcpy(&current, &currentBits, sizeof(current));
        if (current >= 0.0f && current < 1000.0f) {
            if (current > runMaximum) runMaximum = current;
            bool firstReading = lastLoggedMaximum == 0.0f && runMaximum >= 1.0f;
            bool advanced = runMaximum >= lastLoggedMaximum + 10.0f;
            if (firstReading || advanced) {
                lastLoggedMaximum = runMaximum;
                LogBuffer message;
                log_init(&message); log_prefix(&message);
                log_text(&message, "Coffin 3D-speed telemetry: current=");
                log_float2(&message, current);
                log_text(&message, " km/h, run max="); log_float2(&message, runMaximum);
                log_text(&message, " km/h.\r\n"); append_log(&message);
            }
        }

        SteeringTelemetrySample steering;
        if (!read_steering_telemetry_sample(&steering) ||
            steering.sequence == lastSteeringSequence) continue;
        lastSteeringSequence = steering.sequence;
        bool active = f_abs(steering.rawInput) >= 0.05f;
        bool hitClamp = steering_sample_hit_clamp(&steering);
        if (steeringPollsSinceLog < 1000u) ++steeringPollsSinceLog;
        bool changed = !steeringHasLogged ||
            f_abs(steering.rawInput - lastLoggedRaw) >= 0.05f ||
            f_abs(steering.scale - lastLoggedScale) >= 0.05f ||
            f_abs(steering.preClampRadians - lastLoggedPreClamp) >= 0.087266463f ||
            f_abs(steering.finalRadians - lastLoggedFinal) >= 0.043633231f ||
            hitClamp != lastLoggedClamp;
        bool released = !active && lastSteeringActive;
        bool heartbeat = active && steeringPollsSinceLog >= 5u;
        lastSteeringActive = active;
        if ((!active && !released) || (!changed && !released && !heartbeat)) continue;

        const float radiansToDegrees = 57.295779513f;
        LogBuffer steeringMessage;
        log_init(&steeringMessage); log_prefix(&steeringMessage);
        log_text(&steeringMessage, "Coffin steering telemetry: speed=");
        log_float2(&steeringMessage, steering.speedKmh);
        log_text(&steeringMessage, " km/h, raw=");
        log_float2(&steeringMessage, steering.rawInput);
        log_text(&steeringMessage, ", scale=");
        log_float2(&steeringMessage, steering.scale);
        log_text(&steeringMessage, ", pre=");
        log_float2(&steeringMessage, steering.preClampRadians * radiansToDegrees);
        log_text(&steeringMessage, " deg, final=");
        log_float2(&steeringMessage, steering.finalRadians * radiansToDegrees);
        log_text(&steeringMessage, " deg, clamp=");
        log_text(&steeringMessage, hitClamp ? "yes" : "no");
        log_text(&steeringMessage, ".\r\n");
        append_log(&steeringMessage);
        steeringHasLogged = true;
        steeringPollsSinceLog = 0u;
        lastLoggedRaw = steering.rawInput;
        lastLoggedScale = steering.scale;
        lastLoggedPreClamp = steering.preClampRadians;
        lastLoggedFinal = steering.finalRadians;
        lastLoggedClamp = hitClamp;
    }
}

static void run_carrier_gate_trace() {
    if (!g_allowFloatingCarrier || !g_carrierGateInstalled ||
        !g_carrierHelperCount || !g_carrierHelperStatus ||
        !g_carrierResultCount || !g_carrierResultCode ||
        !g_carrierFallbackCount) return;
    LONG lastHelper = -1;
    LONG lastStatus = -1;
    LONG lastResult = -1;
    LONG lastCode = -1;
    LONG lastFallback = -1;
    log_line("CARRIER PATH TRACE: armed for five minutes; approach the Coffin Board with a connected Floating Carrier.", true);
    for (UINT32 poll = 0; poll < 300u; ++poll) {
        Sleep(1000);
        LONG helper = __atomic_load_n(g_carrierHelperCount, __ATOMIC_ACQUIRE);
        LONG status = __atomic_load_n(g_carrierHelperStatus, __ATOMIC_ACQUIRE);
        LONG result = __atomic_load_n(g_carrierResultCount, __ATOMIC_ACQUIRE);
        LONG code = __atomic_load_n(g_carrierResultCode, __ATOMIC_ACQUIRE);
        LONG fallback = __atomic_load_n(g_carrierFallbackCount, __ATOMIC_ACQUIRE);
        bool meaningful = helper != lastHelper || status != lastStatus || result != lastResult ||
            code != lastCode || fallback != lastFallback;
        bool heartbeat = (poll % 5u) == 0u;
        lastHelper = helper;
        lastStatus = status;
        lastResult = result;
        lastCode = code;
        lastFallback = fallback;
        if (!meaningful && !heartbeat) continue;
        LogBuffer message;
        log_init(&message); log_prefix(&message);
        log_text(&message, "CARRIER PATH TRACE: helper=");
        log_uint(&message, (UINT64)(UINT32)helper);
        log_text(&message, ", shiftedStatus=");
        log_uint(&message, (UINT64)(UINT32)status);
        log_text(&message, ", resultGate=");
        log_uint(&message, (UINT64)(UINT32)result);
        log_text(&message, ", resultCode=");
        log_uint(&message, (UINT64)(UINT32)code);
        log_text(&message, ", fallbackGate=");
        log_uint(&message, (UINT64)(UINT32)fallback);
        log_text(&message, ".\r\n");
        append_log(&message);
    }
    log_line("CARRIER PATH TRACE: five-minute window finished.", true);
}

#if defined(COFFIN_CARRIER_TRACE_BUILD)
static void run_carrier_trace() {
    const UINT32 probeCount =
        (UINT32)(sizeof(g_carrierTraceProbes) / sizeof(g_carrierTraceProbes[0]));
    LONG lastCounts[sizeof(g_carrierTraceProbes) / sizeof(g_carrierTraceProbes[0])];
    for (UINT32 index = 0; index < probeCount; ++index) lastCounts[index] = 0;
    UINT64 lastPlayerState = 0;
    UINT64 lastCarrierHandle = 0;
    UINT64 lastVehicleHandle = 0;
    bool handlesSeen = false;
    UINT64 lastCapturedCarrier = 0;
    UINT64 lastCapturedCarrierHandle = 0;
    LONG lastPlayerEventCalls = 0;
    LONG lastCartCodeBreakCalls = 0;
    LONG lastCarrierHandleChanges = 0;
    LONG lastWarningProducerHits = 0;
    LONG lastWarningSuppressedHits = 0;
    log_line(
        "CARRIER BREAK TRACE: recording for five minutes. Mount with a loaded Floating Carrier, then reproduce one rope break or collision disappearance.",
        true
    );
    for (UINT32 poll = 0; poll < 3000u; ++poll) {
        Sleep(100);
        UINT64 playerState = __atomic_load_n(
            &g_carrierRideLinkState.playerState, __ATOMIC_ACQUIRE
        );
        if (!playerState && g_carrierTracePlayerState) playerState = __atomic_load_n(
            g_carrierTracePlayerState, __ATOMIC_ACQUIRE
        );
        if (playerState && readable_range((void*)playerState, 0xC70u)) {
            UINT64 carrierHandle = __atomic_load_n(
                (volatile UINT64*)(playerState + 0xC60u), __ATOMIC_ACQUIRE
            );
            UINT64 vehicleHandle = __atomic_load_n(
                (volatile UINT64*)(playerState + 0xC68u), __ATOMIC_ACQUIRE
            );
            if (!handlesSeen || playerState != lastPlayerState ||
                carrierHandle != lastCarrierHandle ||
                vehicleHandle != lastVehicleHandle) {
                handlesSeen = true;
                lastPlayerState = playerState;
                lastCarrierHandle = carrierHandle;
                lastVehicleHandle = vehicleHandle;
                LogBuffer message;
                log_init(&message); log_prefix(&message);
                log_text(&message, "CARRIER BREAK TRACE handles: playerState=");
                log_uint(&message, playerState);
                log_text(&message, ", carrier=");
                log_uint(&message, carrierHandle);
                log_text(&message, ", vehicle=");
                log_uint(&message, vehicleHandle);
                log_text(&message, ".\r\n");
                append_log(&message);
            }
        }
        UINT64 capturedCarrier = __atomic_load_n(
            &g_carrierRideLinkState.carrierEntity, __ATOMIC_ACQUIRE
        );
        UINT64 capturedCarrierHandle = __atomic_load_n(
            &g_carrierRideLinkState.carrierEntityHandle, __ATOMIC_ACQUIRE
        );
        UINT64 coffinHandle = __atomic_load_n(
            &g_carrierRideLinkState.coffinVehicleHandle, __ATOMIC_ACQUIRE
        );
        bool captureChanged = capturedCarrier != lastCapturedCarrier ||
            capturedCarrierHandle != lastCapturedCarrierHandle;
        if (captureChanged) {
            lastCapturedCarrier = capturedCarrier;
            lastCapturedCarrierHandle = capturedCarrierHandle;
            LogBuffer message;
            log_init(&message); log_prefix(&message);
            log_text(&message, "CARRIER LINK TRACE: entity=");
            log_uint(&message, capturedCarrier);
            log_text(&message, ", handle=");
            log_uint(&message, capturedCarrierHandle);
            log_text(&message, ", coffinHandle=");
            log_uint(&message, coffinHandle);
            log_text(&message, ".\r\n");
            append_log(&message);
        }
        LONG warningProducerHits = g_carrierWarningProducerHits ?
            __atomic_load_n(g_carrierWarningProducerHits, __ATOMIC_ACQUIRE) : 0;
        LONG warningSuppressedHits = g_carrierWarningSuppressedHits ?
            __atomic_load_n(g_carrierWarningSuppressedHits, __ATOMIC_ACQUIRE) : 0;
        if (warningProducerHits != lastWarningProducerHits ||
            warningSuppressedHits != lastWarningSuppressedHits) {
            lastWarningProducerHits = warningProducerHits;
            lastWarningSuppressedHits = warningSuppressedHits;
            LogBuffer message;
            log_init(&message); log_prefix(&message);
            log_text(&message, "CARRIER WARNING TRACE: producer=");
            log_uint(&message, (UINT64)(UINT32)warningProducerHits);
            log_text(&message, ", Coffin-suppressed=");
            log_uint(&message, (UINT64)(UINT32)warningSuppressedHits);
            log_text(&message, ".\r\n");
            append_log(&message);
        }
        LONG playerEventCalls = __atomic_load_n(
            &g_carrierRideLinkState.playerEventCalls, __ATOMIC_ACQUIRE
        );
        LONG cartCodeBreakCalls = __atomic_load_n(
            &g_carrierRideLinkState.cartCodeBreakCalls, __ATOMIC_ACQUIRE
        );
        LONG carrierHandleChanges = __atomic_load_n(
            &g_carrierRideLinkState.carrierHandleChanges, __ATOMIC_ACQUIRE
        );
        if (carrierHandleChanges != lastCarrierHandleChanges ||
            cartCodeBreakCalls != lastCartCodeBreakCalls ||
            (playerEventCalls != lastPlayerEventCalls && (poll % 10u) == 0u)) {
            lastPlayerEventCalls = playerEventCalls;
            lastCartCodeBreakCalls = cartCodeBreakCalls;
            lastCarrierHandleChanges = carrierHandleChanges;
            UINT32 lastEvent = __atomic_load_n(
                &g_carrierRideLinkState.lastPlayerEventId, __ATOMIC_ACQUIRE
            );
            UINT32 changeEvent = __atomic_load_n(
                &g_carrierRideLinkState.lastCarrierChangeEventId, __ATOMIC_ACQUIRE
            );
            LogBuffer message;
            log_init(&message); log_prefix(&message);
            log_text(&message, "PLAYER CARGO EVENT TRACE: calls=");
            log_uint(&message, (UINT64)(UINT32)playerEventCalls);
            log_text(&message, ", CartCodeBreak=");
            log_uint(&message, (UINT64)(UINT32)cartCodeBreakCalls);
            log_text(&message, ", handleChanges=");
            log_uint(&message, (UINT64)(UINT32)carrierHandleChanges);
            log_text(&message, ", lastEvent=");
            log_uint(&message, lastEvent);
            log_text(&message, ", changeEvent=");
            log_uint(&message, changeEvent);
            log_text(&message, ", carrier=");
            log_uint(&message, __atomic_load_n(
                &g_carrierRideLinkState.carrierBeforeChange, __ATOMIC_ACQUIRE
            ));
            log_text(&message, "->");
            log_uint(&message, __atomic_load_n(
                &g_carrierRideLinkState.carrierAfterChange, __ATOMIC_ACQUIRE
            ));
            log_text(&message, ", vehicle=");
            log_uint(&message, __atomic_load_n(
                &g_carrierRideLinkState.vehicleBeforeChange, __ATOMIC_ACQUIRE
            ));
            log_text(&message, "->");
            log_uint(&message, __atomic_load_n(
                &g_carrierRideLinkState.vehicleAfterChange, __ATOMIC_ACQUIRE
            ));
            log_text(&message, ".\r\n");
            append_log(&message);
        }
        for (UINT32 index = 0; index < probeCount; ++index) {
            LONG count = __atomic_load_n(
                &g_carrierTraceProbes[index].count, __ATOMIC_ACQUIRE
            );
            if (count == lastCounts[index]) continue;
            lastCounts[index] = count;
            if (index == 1u) continue;
            LogBuffer message;
            log_init(&message); log_prefix(&message);
            log_text(&message, "CARRIER TRACE event: ");
            log_text(&message, g_carrierTraceProbes[index].name);
            log_text(&message, " count=");
            log_uint(&message, (UINT64)(UINT32)count);
            log_text(&message, ".\r\n");
            append_log(&message);
        }
    }
    log_line("CARRIER BREAK TRACE: five-minute recording window finished.", true);
}
#endif

static DWORD __stdcall worker(LPVOID) {
    read_configuration();
#if defined(COFFIN_CARRIER_TRACE_BUILD)
    log_line("DS2 Coffin Board All-Terrain Speed v1.1.0 carrier TRACE loaded.", true);
#else
    log_line("DS2 Coffin Board All-Terrain Speed v1.1.0 loaded.", true);
#endif
    if (!g_enabled) {
        log_line("Disabled in INI; no game memory was changed.", true);
        return 0;
    }
    HMODULE executable = GetModuleHandleW(0);
    if (!executable || !validate_target_build(executable)) {
        log_line("ERROR: Unsupported DS2.exe. Expected Steam build 1.10.89.0; no patch applied.", true);
        return 0;
    }
    UINT64 globalAddress = find_streaming_manager_global(executable);
    if (!globalAddress || !readable_range((void*)globalAddress, sizeof(void*))) {
        log_line("ERROR: Exact StreamingManager anchor did not match; no patch applied.", true);
        return 0;
    }
    void** managerGlobal = (void**)globalAddress;
    void* manager = 0;
    for (int attempt = 0; attempt < 1200; ++attempt) {
        if (readable_range(managerGlobal, sizeof(void*))) manager = *managerGlobal;
        if (manager) break;
        Sleep(50);
    }
    if (!manager) {
        log_line("ERROR: StreamingManager did not initialize within 60 seconds.", true);
        return 0;
    }

    // Install only the two hooks required by the speed transaction first, then
    // subscribe immediately. Cached starts can finish loading the Coffin
    // resource while the independent carrier/trace hooks are still being
    // installed, so delaying registration until after those hooks creates a
    // real startup race.
    if (!install_drive_force_hook(executable)) {
        if (drive_coupling_needed()) {
            log_line("ERROR: Exact Coffin drive-force hook anchor/vtable did not match; coupled caps/gearing/force patch was not applied.", true);
            return 0;
        }
        log_line("WARNING: Exact Coffin telemetry hook anchor/vtable did not match; cap patch will continue without telemetry.", true);
    } else if (g_driveHookInstalled) {
        LogBuffer hookMessage;
        log_init(&hookMessage); log_prefix(&hookMessage);
        log_text(&hookMessage, "Coffin-only drive-force hook installed neutral at 100%; requested ");
        log_uint(&hookMessage, (UINT64)g_driveForcePercent);
        log_text(&hookMessage, "% activates only after the resource patch, telemetry=");
        log_uint(&hookMessage, (UINT64)(g_speedTelemetry != 0));
        log_text(&hookMessage, ".\r\n"); append_log(&hookMessage);
    }

    if (!install_steering_response_hook(executable)) {
        if (steering_coupling_needed()) {
            log_line("ERROR: Exact Coffin steering-response hook anchor/vtable did not match; no resource patch was applied.", true);
            return 0;
        }
    } else if (g_steeringHookInstalled) {
        LogBuffer hookMessage;
        log_init(&hookMessage); log_prefix(&hookMessage);
        log_text(&hookMessage, "Coffin-only steering-response hook installed neutral at 100%; requested ");
        log_uint(&hookMessage, (UINT64)g_steeringResponsePercent);
        log_text(&hookMessage, "% activates from 80 to 220 km/h after the resource patch, steering telemetry=");
        log_uint(&hookMessage, (UINT64)(g_speedTelemetry != 0));
        log_text(&hookMessage, ".\r\n");
        append_log(&hookMessage);
    }

    if (!register_streaming_listener(manager)) {
        log_line("ERROR: Could not register resource listener; no speed patch was applied.", true);
        return 0;
    }
    LogBuffer message;
    log_init(&message); log_prefix(&message);
    log_text(&message, "Listener registered early. profile=");
    log_text(&message, g_simpleProfile ? "simple" : "legacy");
    log_text(&message, ", land="); log_uint(&message, (UINT64)g_normalPercent);
    log_text(&message, "% of water, boost="); log_uint(&message, (UINT64)g_boostPercent);
    log_text(&message, "%, PatchStandardSpeed="); log_uint(&message, (UINT64)(g_patchStandardSpeed != 0));
    log_text(&message, ", ScaleWaterCapsWithPercent="); log_uint(&message, (UINT64)(g_scaleWaterCaps != 0));
    log_text(&message, ", DriveForcePercent="); log_uint(&message, (UINT64)g_driveForcePercent);
    log_text(&message, ", GearRatioPercent="); log_uint(&message, (UINT64)g_gearRatioPercent);
    log_text(&message, ", AccelerationPercent="); log_uint(&message, (UINT64)g_accelerationPercent);
    log_text(&message, ", SteeringAnglePercent="); log_uint(&message, (UINT64)g_steeringAnglePercent);
    log_text(&message, ", SteeringResponsePercent="); log_uint(&message, (UINT64)g_steeringResponsePercent);
    log_text(&message, ", WetGripPercent="); log_uint(&message, (UINT64)g_wetGripPercent);
    log_text(&message, ", RaiseSlipThreshold="); log_uint(&message, (UINT64)(g_raiseSlipThreshold != 0));
    log_text(&message, ", targeted group/index lookup enabled.\r\n"); append_log(&message);

    if (!install_floating_carrier_link_preserve(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact Coffin/Floating Carrier transition anchor did not match; no mount patch was applied."
        );
    }
    if (!install_floating_carrier_detach_guard(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact Floating Carrier detach-event anchor did not match; no jump-preservation patch was applied."
        );
    }
    if (!install_floating_carrier_warning_guard(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact Floating Carrier overextension-notification anchor did not match; no warning filter was applied."
        );
    }
    if (!install_floating_carrier_gate(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact Coffin/Floating Carrier gate anchor did not match; no mount patch was applied."
        );
    }
    if (g_carrierGateInstalled) {
        log_line("Coffin-only Floating Carrier mount gate installed; the active loaded carrier remains linked during the ride.", true);
    }
    if (g_carrierLinkPreserveInstalled) {
        log_line("Coffin-only RideVehicle carrier-disconnect transaction bypass installed.", true);
    }
    if (g_carrierDetachGuardInstalled) {
        log_line("Coffin-only Floating Carrier jump-detach event guard installed.", true);
    }
    if (g_carrierWarningGuardInstalled) {
        log_line("Coffin-only Floating Carrier overextension HUD/voice notification guard installed.", true);
    }

#if defined(COFFIN_CARRIER_TRACE_BUILD)
    if (!install_direct_carrier_write_traces(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact direct player carrier-handle write anchors did not match; trace build stopped."
        );
    }
    if (!install_player_handle_write_traces(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact player carrier-handle write anchors did not match; trace build stopped."
        );
    }
    if (!install_carrier_trace_probes(executable)) {
        return stop_worker_after_listener_error(
            "ERROR: Exact carrier trace vtable anchors did not match; trace build stopped without publishing a behavior patch."
        );
    }
    log_line("CARRIER TRACE: five direct carrier writes, three block stores, and action/component probes installed without callbacks.", true);
#endif

    // Keep this one-shot listener only for a bounded discovery window. A
    // completion published by a callback is observed with acquire semantics,
    // while a 500-ms backfill closes the listener-registration timing gap.
    // Removal runs only on this worker; if the callback has not returned yet,
    // native RemoveListener's exclusive lock waits for the dispatcher's shared
    // listener lock and provides the final synchronization.
    bool completed = wait_for_patch_completion_with_backfill(
        g_streamingSystem, 1200u, 50u, 10u
    );
    if (!unregister_streaming_listener()) {
        log_line("LIFECYCLE ERROR: exact native listener removal failed; restart DS2 before changing this ASI.", true);
        return 0;
    }
    bool completedAfterRemoval = patch_complete_acquire();
    if (completed && completedAfterRemoval) {
        log_line("LIFECYCLE: patch completion observed; one-shot listener removed outside callbacks.", true);
    } else if (completedAfterRemoval) {
        log_line("LIFECYCLE: patch completed at the discovery-window boundary; one-shot listener removed outside callbacks.", true);
    } else if (completed) {
        log_line("LIFECYCLE WARNING: target unloaded while listener removal was pending; one-shot listener is removed and will not patch a replacement in this session.", true);
    } else {
        log_line("LIFECYCLE WARNING: 60-second resource window expired; one-shot listener removed without a completed patch.", true);
    }
    log_line("LIFECYCLE: worker no longer participates in streaming shutdown.", true);

    run_speed_telemetry();
    run_carrier_gate_trace();
#if defined(COFFIN_CARRIER_TRACE_BUILD)
    run_carrier_trace();
#endif
    return 0;
}

extern "C" int __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        if (!is_main_game_process()) return TRUE;
        g_module = (HMODULE)instance;
        DisableThreadLibraryCalls(g_module);
        g_mutex = CreateMutexW(0, FALSE, L"Local\\DS2_CoffinBoardAllTerrainSpeed_v0_1");
        if (!g_mutex) return TRUE;
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            CloseHandle(g_mutex);
            g_mutex = 0;
            return TRUE;
        }
        HANDLE thread = CreateThread(0, 0, worker, 0, 0, 0);
        if (thread) CloseHandle(thread);
    } else if (reason == DLL_PROCESS_DETACH) {
        if (g_mutex) { CloseHandle(g_mutex); g_mutex = 0; }
    }
    return TRUE;
}
