// DS2 Coffin Board All-Terrain Speed v1.1.7-native-replay
// Target: DEATH STRANDING 2: ON THE BEACH, Steam 1.10.89.0
//
// Production test build based on the feature-complete v1.1.2 carrier source.
// The RideVehicle carrier-preserve shim now replays the original resolver block
// before any Coffin-only inspection so non-Coffin vehicles retain native call semantics.
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
#define FILE_APPEND_DATA 0x00000004u
#define FILE_SHARE_READ 0x00000001u
#define FILE_SHARE_WRITE 0x00000002u
#define OPEN_ALWAYS 4u
#define FILE_ATTRIBUTE_NORMAL 0x80u
#define FILE_END 2u
#define INVALID_HANDLE_VALUE ((HANDLE)(INT64)-1)

static const UINT32 EXPECTED_TIMESTAMP = 0x6A3DAE46u;
static const UINT32 EXPECTED_IMAGE_SIZE = 0x0B292000u;
static const SIZE_T OFF_LAND_TOP_SPEED = 0x44u;
static const SIZE_T OFF_FINAL_GEAR_RATIO = 0x48u;
static const SIZE_T OFF_LAND_BOOST_TOP_SPEED = 0x94u;
static const SIZE_T OFF_WATER_TOP_SPEED = 0x98u;
static const SIZE_T OFF_WATER_BOOST_TOP_SPEED = 0x9Cu;
static const SIZE_T OFF_SLIP_SPEED = 0xD8u;
static const UINT32 COFFIN_PHYSICS_GROUP_ID = 499u;
static const UINT32 COFFIN_PHYSICS_OBJECT_INDEX = 71288u;
static const UINT64 COFFIN_PHYSICS_UUID_LOW = 0x6B4E9446DCF0C387ull;
static const UINT64 COFFIN_PHYSICS_UUID_HIGH = 0x26DBAD5945C463BAull;
static const UINT32 RVA_STREAMING_SIGNATURE = 0x00693674u;
static const UINT32 RVA_STREAMING_MANAGER_GLOBAL = 0x06266938u;
static const UINT32 RVA_STREAMING_SYSTEM_VTABLE = 0x034532A0u;
static const UINT32 RVA_STREAMING_ADD_LISTENER = 0x026F6E40u;
static const UINT32 RVA_STREAMING_REMOVE_LISTENER = 0x026F6EE0u;
static const UINT32 RVA_DRIVE_TORQUE_HOOK = 0x0247A431u;
static const UINT32 RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT = 0x01007D94u;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_HOOK = 0x00F9A807u;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_CONTINUE = 0x00F9A81Cu;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_SKIP = 0x00F9AC6Bu;
static const UINT32 RVA_RESOLVE_LINKED_CARRIER = 0x00E462F0u;
static const UINT32 RVA_CARRIER_DETACH_EVENT_GATE = 0x00E5C7AAu;
static const UINT32 RVA_CARRIER_DETACH_OTHER_EVENT = 0x00E5C7C2u;
static const UINT32 RVA_CARRIER_DETACH_NATIVE = 0x00E5CDD3u;
static const UINT32 RVA_CARRIER_DETACH_SUPPRESSED = 0x00E5D0A7u;
static const UINT32 RVA_CARRIER_WARNING_NOTIFICATION_GATE = 0x0121D839u;
static const UINT32 RVA_CARRIER_WARNING_NOTIFICATION_CONTINUE = 0x0121D84Au;
static const UINT32 RVA_ENTITY_MESSAGE_DISPATCH = 0x00130C60u;
static const UINT32 RVA_COFFIN_PHYSICS_VTABLE = 0x033AD0B0u;
static const UINT32 RVA_COFFIN_VFUNC_0 = 0x01F430B0u;
static const UINT32 RVA_COFFIN_VFUNC_1 = 0x01F4E160u;
static const UINT32 RVA_COFFIN_VFUNC_2 = 0x01F480A0u;
static const UINT32 RVA_COFFIN_VFUNC_3 = 0x01F49350u;
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
static int g_speedPercent = 500;
static int g_accelerationPercent = 400;
static int g_driveForcePercent = 2000;
static int g_gearRatioPercent = 20;
static int g_allowFloatingCarrier = 1;
static UINT64 g_callbackGroups = 0;
static UINT64 g_callbackObjects = 0;
static UINT64 g_rttiLookups = 0;
static UINT64 g_targetChecks = 0;
static volatile LONG g_listenerRegistered = 0;
static void* g_streamingSystem = 0;
static void (__fastcall* g_removeStreamingListener)(void*, void*) = 0;
static volatile LONG g_complete = 0;
static int g_physicsReady = 0;
static float g_waterBaseline = 0.0f;
static float g_waterBoostBaseline = 0.0f;
static float g_lastAppliedWater = 0.0f;
static float g_lastAppliedWaterBoost = 0.0f;
static float g_gearRatioBaseline = 0.0f;
static float g_lastAppliedGearRatio = 0.0f;
static void* g_seenPhysicsResource = 0;
static int g_driveHookInstalled = 0;
static volatile LONG* g_driveFactorBits = 0;
static int g_carrierGateInstalled = 0;
static int g_carrierLinkPreserveInstalled = 0;
static int g_carrierDetachGuardInstalled = 0;
static int g_carrierWarningGuardInstalled = 0;
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
};
__declspec(align(16)) static CarrierRideLinkState g_carrierRideLinkState = {};
static volatile LONG* g_carrierWarningProducerHits = 0;
static volatile LONG* g_carrierWarningSuppressedHits = 0;
static UINT64 g_logLock = 0;
static UINT64 g_targetStateLock = 0;
static HANDLE g_suspendedThreadHandles[512];
static DWORD g_suspendedThreadIds[512];
static UINT32 g_suspendedThreadCount = 0;
__declspec(align(16)) static BYTE g_threadContext[0x4D0];

static float f_abs(float value) { return value < 0.0f ? -value : value; }
static bool f_near(float value, float expected, float tolerance) {
    return f_abs(value - expected) <= tolerance;
}
static float f_max(float a, float b) { return a > b ? a : b; }
static bool patch_complete_acquire() {
    return __atomic_load_n(&g_complete, __ATOMIC_ACQUIRE) != 0;
}
static void publish_patch_complete(bool complete) {
    __atomic_store_n(&g_complete, complete ? 1 : 0, __ATOMIC_RELEASE);
}
static LONG listener_state_acquire() {
    return __atomic_load_n(&g_listenerRegistered, __ATOMIC_ACQUIRE);
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
        ++a; ++b;
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
    char temporary[32]; int count = 0;
    if (value == 0) { log_char(buffer, '0'); return; }
    while (value && count < 31) {
        temporary[count++] = (char)('0' + value % 10); value /= 10;
    }
    while (count) log_char(buffer, temporary[--count]);
}
static void log_float2(LogBuffer* buffer, float value) {
    if (value < 0.0f) { log_char(buffer, '-'); value = -value; }
    UINT64 whole = (UINT64)value;
    UINT32 fraction = (UINT32)((value - (float)whole) * 100.0f + 0.5f);
    if (fraction >= 100) { ++whole; fraction -= 100; }
    log_uint(buffer, whole); log_char(buffer, '.');
    log_char(buffer, (char)('0' + (fraction / 10) % 10));
    log_char(buffer, (char)('0' + fraction % 10));
}
static void log_prefix(LogBuffer* buffer) {
    SYSTEMTIME_X time; GetLocalTime(&time);
    log_char(buffer, '['); log_uint(buffer, time.wYear); log_char(buffer, '-');
    if (time.wMonth < 10) log_char(buffer, '0'); log_uint(buffer, time.wMonth); log_char(buffer, '-');
    if (time.wDay < 10) log_char(buffer, '0'); log_uint(buffer, time.wDay); log_char(buffer, ' ');
    if (time.wHour < 10) log_char(buffer, '0'); log_uint(buffer, time.wHour); log_char(buffer, ':');
    if (time.wMinute < 10) log_char(buffer, '0'); log_uint(buffer, time.wMinute); log_char(buffer, ':');
    if (time.wSecond < 10) log_char(buffer, '0'); log_uint(buffer, time.wSecond); log_text(buffer, "] ");
}
static void append_log(LogBuffer* buffer) {
    AcquireSRWLockExclusive(&g_logLock);
    HANDLE file = CreateFileW(g_logPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (!file || file == INVALID_HANDLE_VALUE) {
        ReleaseSRWLockExclusive(&g_logLock); return;
    }
    SetFilePointer(file, 0, 0, FILE_END);
    DWORD written = 0; WriteFile(file, buffer->data, (DWORD)buffer->length, &written, 0);
    CloseHandle(file); ReleaseSRWLockExclusive(&g_logLock);
}
static void log_line(const char* text) {
    LogBuffer buffer; log_init(&buffer); log_prefix(&buffer); log_text(&buffer, text);
    log_text(&buffer, "\r\n"); append_log(&buffer);
}
static void make_sibling_path(WCHAR* destination, int capacity, const WCHAR* fileName) {
    WCHAR modulePath[512]; DWORD length = GetModuleFileNameW(g_module, modulePath, 511);
    if (length == 0 || length >= 511) { wide_copy(destination, capacity, fileName); return; }
    modulePath[length] = 0; int separator = -1;
    for (int i = 0; modulePath[i]; ++i) if (modulePath[i] == L'\\' || modulePath[i] == L'/') separator = i;
    int output = 0;
    for (int i = 0; i <= separator && output < capacity - 1; ++i) destination[output++] = modulePath[i];
    for (int i = 0; fileName[i] && output < capacity - 1; ++i) destination[output++] = fileName[i];
    destination[output] = 0;
}
static int clamp_speed(int value) { if (value < 100) return 100; if (value > 1000) return 1000; return value; }
static int clamp_accel(int value) { if (value < 100) return 100; if (value > 500) return 500; return value; }
static void read_configuration() {
    make_sibling_path(g_iniPath, 512, L"ds2_coffin_board_all_terrain_speed.ini");
    make_sibling_path(g_logPath, 512, L"ds2_coffin_board_all_terrain_speed.log");
    g_enabled = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"Enabled", 1, g_iniPath);
    g_speedPercent = clamp_speed((int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"SpeedPercent", 500, g_iniPath));
    g_accelerationPercent = clamp_accel((int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"AccelerationPercent", 400, g_iniPath));
    g_allowFloatingCarrier = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"AllowFloatingCarrier", 1, g_iniPath) != 0;
    UINT64 derived = (UINT64)g_speedPercent * (UINT64)g_accelerationPercent / 100u;
    if (derived < 100u) derived = 100u; if (derived > 5000u) derived = 5000u;
    g_driveForcePercent = (int)derived;
    g_gearRatioPercent = 10000 / g_speedPercent;
    if (g_gearRatioPercent < 10) g_gearRatioPercent = 10;
    if (g_gearRatioPercent > 100) g_gearRatioPercent = 100;
}
static bool readable_range(const void* address, SIZE_T bytes) {
    if (!address || bytes == 0) return false;
    MEMORY_BASIC_INFORMATION_X64 information;
    if (!VirtualQuery(address, &information, sizeof(information))) return false;
    if (information.State != MEM_COMMIT || (information.Protect & PAGE_GUARD) ||
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
static int write_float_checked(float* field, float target) {
    if (!field || !readable_range(field, sizeof(float))) return 0;
    if (f_near(*field, target, 0.01f)) return 1;
    DWORD oldProtection = 0; int changedProtection = FALSE;
    if (!writable_range(field, sizeof(float))) {
        changedProtection = VirtualProtect(field, sizeof(float), PAGE_READWRITE, &oldProtection);
        if (!changedProtection) return 0;
    }
    *field = target; bool verified = f_near(*field, target, 0.01f);
    if (changedProtection) { DWORD ignored = 0; VirtualProtect(field, sizeof(float), oldProtection, &ignored); }
    return verified ? 2 : 0;
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
    for (SIZE_T index = 0; index < size; ++index) if (left[index] != right[index]) return false;
    return true;
}
static bool tracked_thread_id(const DWORD* ids, UINT32 count, DWORD id) {
    for (UINT32 index = 0; index < count; ++index) if (ids[index] == id) return true;
    return false;
}
static void resume_suspended_threads(HANDLE* handles, UINT32 count) {
    while (count) { HANDLE thread = handles[--count]; ResumeThread(thread); CloseHandle(thread); }
}
static bool suspend_other_threads(HANDLE* handles, UINT32* suspendedCount, const BYTE* patchTarget, SIZE_T patchSize) {
    if (!handles || !suspendedCount || !patchTarget || patchSize == 0u) return false;
    *suspendedCount = 0;
    DWORD processId = GetCurrentProcessId(); DWORD currentThreadId = GetCurrentThreadId();
    bool stable = false, unsafeInstructionPointer = false, contextFailure = false;
    for (UINT32 pass = 0; pass < 4u && !stable && !unsafeInstructionPointer && !contextFailure; ++pass) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) break;
        THREADENTRY32_X entry; memset(&entry, 0, sizeof(entry)); entry.dwSize = sizeof(entry);
        bool unresolved = false; UINT32 added = 0;
        if (Thread32First(snapshot, &entry)) {
            do {
                DWORD id = entry.th32ThreadID;
                if (entry.th32OwnerProcessID != processId || id == currentThreadId ||
                    tracked_thread_id(g_suspendedThreadIds, *suspendedCount, id)) continue;
                if (*suspendedCount >= 512u) { unresolved = true; break; }
                HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, id);
                if (!thread) { unresolved = true; continue; }
                if (SuspendThread(thread) == 0xFFFFFFFFu) { CloseHandle(thread); unresolved = true; continue; }
                g_suspendedThreadIds[*suspendedCount] = id; handles[*suspendedCount] = thread;
                ++*suspendedCount; ++added;
                memset(g_threadContext, 0, sizeof(g_threadContext));
                *(DWORD*)(g_threadContext + 48u) = CONTEXT_CONTROL_X64;
                if (!GetThreadContext(thread, g_threadContext)) { contextFailure = true; break; }
                UINT64 instructionPointer = *(UINT64*)(g_threadContext + 248u);
                UINT64 patchStart = (UINT64)patchTarget, patchEnd = patchStart + (UINT64)patchSize;
                if (instructionPointer >= patchStart && instructionPointer < patchEnd) { unsafeInstructionPointer = true; break; }
            } while (Thread32Next(snapshot, &entry));
        } else unresolved = true;
        CloseHandle(snapshot);
        stable = !unresolved && !unsafeInstructionPointer && !contextFailure && added == 0u;
    }
    if (!stable) { resume_suspended_threads(handles, *suspendedCount); *suspendedCount = 0; }
    return stable;
}
static bool freeze_for_patch(const BYTE* target, SIZE_T size) {
    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(g_suspendedThreadHandles, &g_suspendedThreadCount, target, size);
        if (!threadsFrozen) Sleep(1);
    }
    return threadsFrozen;
}
static void unfreeze_after_patch() {
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
}

// v1.1.7 FIX: replay the original RideVehicle resolver block *before* any
// Coffin-only inspection. The native direct call did not clobber RAX on entry;
// older carrier shims used "mov rax,resolver / call rax", which changed the
// resolver's incoming register state for every vehicle. That is the key
// behavioural difference removed here.
//
// For non-Coffin vehicles the observable state at RVA 0xF9A81C is now native:
//   - mov rcx,[rdi+0xA8] is replayed first
//   - resolver is called exactly once with no GPR used as an address scratch
//   - RAX is the real resolver return value
//   - R10/R11/RSP are restored after the Coffin discriminator
//   - flags are restored to the native "test rax,rax" fall-through state
//
// Runtime qwords: resolver +19, native-skip +38, state +81,
// coffin-skip +143, native-continuation +164.
static const BYTE CARRIER_LINK_PRESERVE_TEMPLATE[172] = {
    0xF3,0x0F,0x1E,0xFA,                         // endbr64

    // Exact native pre-call instruction.
    0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,          // mov rcx,[rdi+A8h]

    // Absolute indirect call without clobbering RAX or any other GPR.
    // call [rip+2] returns to the following short jump, which skips the qword.
    0xFF,0x15,0x02,0x00,0x00,0x00,               // call qword ptr [rip+2]
    0xEB,0x08,                                    // jmp +8
    0,0,0,0,0,0,0,0,                             // resolver address (+19)

    // Exact native zero-result behaviour.
    0x48,0x85,0xC0,                               // test rax,rax
    0x75,0x0E,                                    // jne resolver_nonzero
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [rip+0]
    0,0,0,0,0,0,0,0,                             // native skip (+38)

    // Only after the native resolver has completed do we inspect Coffin state.
    0x41,0x52,                                    // push r10
    0x41,0x53,                                    // push r11
    0x4C,0x8B,0x97,0x90,0x01,0x00,0x00,          // mov r10,[rdi+190h]
    0x4D,0x85,0xD2,                               // test r10,r10
    0x74,0x59,                                    // je native_non_coffin
    0x41,0x83,0xBA,0xA0,0x02,0x00,0x00,0x03,     // cmp dword [r10+2A0h],3
    0x75,0x4F,                                    // jne native_non_coffin

    // Coffin-only: capture the already-resolved linked carrier and preserve it.
    0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,          // mov rcx,[rdi+A8h]
    0x49,0xBA, 0,0,0,0,0,0,0,0,                  // mov r10,state (+81)
    0x49,0xC7,0x02,0x00,0x00,0x00,0x00,          // carrierEntity = 0
    0x49,0xC7,0x42,0x18,0x00,0x00,0x00,0x00,     // carrierEntityHandle = 0
    0x49,0x89,0x4A,0x08,                          // playerState = rcx
    0x4C,0x8B,0x9E,0x20,0x03,0x00,0x00,          // r11=[rsi+320h]
    0x4D,0x89,0x5A,0x10,                          // coffinVehicleHandle = r11
    0x4C,0x8B,0x98,0x20,0x03,0x00,0x00,          // r11=[rax+320h]
    0x4D,0x89,0x5A,0x18,                          // carrierEntityHandle = r11
    0x49,0x89,0x02,                               // publish carrierEntity = rax last
    0x41,0x5B,                                    // pop r11
    0x41,0x5A,                                    // pop r10
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [rip+0]
    0,0,0,0,0,0,0,0,                             // Coffin skip disconnect (+143)

    // Non-Coffin: restore post-resolver register state and native flags.
    0x41,0x5B,                                    // pop r11
    0x41,0x5A,                                    // pop r10
    0x48,0x85,0xC0,                               // native test rax,rax (ZF=0 here)
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [rip+0]
    0,0,0,0,0,0,0,0                              // native continuation (+164)
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
        0, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE),
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    );
    if (!cave) return false;
    memcpy(cave, CARRIER_LINK_PRESERVE_TEMPLATE,
           sizeof(CARRIER_LINK_PRESERVE_TEMPLATE));

    UINT64 resolver = (UINT64)(base + RVA_RESOLVE_LINKED_CARRIER);
    UINT64 nativeSkip = (UINT64)(base + RVA_COFFIN_PRESERVE_CARRIER_SKIP);
    UINT64 stateAddress = (UINT64)&g_carrierRideLinkState;
    UINT64 continuation = (UINT64)(base + RVA_COFFIN_PRESERVE_CARRIER_CONTINUE);

    memcpy(cave + 19u, &resolver, sizeof(resolver));
    memcpy(cave + 38u, &nativeSkip, sizeof(nativeSkip));
    memcpy(cave + 81u, &stateAddress, sizeof(stateAddress));
    memcpy(cave + 143u, &nativeSkip, sizeof(nativeSkip));
    memcpy(cave + 164u, &continuation, sizeof(continuation));

    FlushInstructionCache(
        (HANDLE)(INT64)-1, cave, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE)
    );

    BYTE replacement[21] = {
        0xFF,0x25,0,0,0,0, 0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));

    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(
            target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection
        )) {
        unfreeze_after_patch();
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
    unfreeze_after_patch();
    if (!installed) return false;
    g_carrierLinkPreserveInstalled = 1;
    return true;
}

static const BYTE CARRIER_DETACH_GUARD_TEMPLATE[124] = {
    0xF3,0x0F,0x1E,0xFA, 0x8B,0x42,0x10, 0x49,0x89,0xD5, 0x4C,0x8B,0x61,0x48,
    0x48,0x89,0xCE, 0x3D,0xE0,0x4F,0x75,0x13, 0x75,0x46,
    0x49,0xBA, 0,0,0,0,0,0,0,0, 0x49,0x39,0x72,0x08, 0x75,0x30,
    0x4D,0x8B,0x5A,0x18, 0x4D,0x85,0xDB, 0x74,0x27, 0x49,0x83,0xFB,0xFF, 0x74,0x21,
    0x4C,0x39,0x9E,0x60,0x0C,0x00,0x00, 0x75,0x18, 0x49,0x8B,0x02, 0x48,0x85,0xC0,
    0x74,0x10, 0x48,0x83,0xBE,0x68,0x0C,0x00,0x00,0xFF, 0x74,0x06,
    0xFF,0x25,0x0C,0x00,0x00,0x00, 0xFF,0x25,0x0E,0x00,0x00,0x00, 0xFF,0x25,0x10,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};
static bool install_floating_carrier_detach_guard(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable; BYTE* target = base + RVA_CARRIER_DETACH_EVENT_GATE;
    static const BYTE expected[24] = {0x8B,0x42,0x10,0x4C,0x8B,0xEA,0x4C,0x8B,0x61,0x48,0x48,0x8B,0xF1,0x3D,0xE0,0x4F,0x75,0x13,0x0F,0x84,0x11,0x06,0x00,0x00};
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, sizeof(CARRIER_DETACH_GUARD_TEMPLATE), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    memcpy(cave, CARRIER_DETACH_GUARD_TEMPLATE, sizeof(CARRIER_DETACH_GUARD_TEMPLATE));
    UINT64 stateAddress=(UINT64)&g_carrierRideLinkState, suppressed=(UINT64)(base+RVA_CARRIER_DETACH_SUPPRESSED), nativeDetach=(UINT64)(base+RVA_CARRIER_DETACH_NATIVE), otherEvent=(UINT64)(base+RVA_CARRIER_DETACH_OTHER_EVENT);
    memcpy(cave+26u,&stateAddress,8); memcpy(cave+100u,&suppressed,8); memcpy(cave+108u,&nativeDetach,8); memcpy(cave+116u,&otherEvent,8);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(CARRIER_DETACH_GUARD_TEMPLATE));
    BYTE replacement[24]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90};
    UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6u,&caveAddress,8);
    if (!freeze_for_patch(target,sizeof(replacement))) return false;
    DWORD oldProtection=0; if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement)); FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement)); if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0; VirtualProtect(target,sizeof(replacement),oldProtection,&ignored); unfreeze_after_patch();
    if(!installed)return false; g_carrierDetachGuardInstalled=1; return true;
}

static const BYTE CARRIER_WARNING_GUARD_TEMPLATE[170] = {
    0xF3,0x0F,0x1E,0xFA,0x50,0x41,0x52,0x41,0x53,0xF0,0xFF,0x05,0x92,0x00,0x00,0x00,
    0x49,0xBA,0,0,0,0,0,0,0,0,0x49,0x8B,0x02,0x48,0x85,0xC0,0x74,0x54,
    0x49,0x8B,0x42,0x08,0x48,0x85,0xC0,0x74,0x4B,0x4D,0x8B,0x5A,0x18,0x4D,0x85,0xDB,0x74,0x42,
    0x49,0x83,0xFB,0xFF,0x74,0x3C,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
    0x4C,0x39,0x98,0x60,0x0C,0x00,0x00,0x75,0x2A,0x4D,0x8B,0x5A,0x10,0x4D,0x85,0xDB,0x74,0x21,
    0x49,0x83,0xFB,0xFF,0x74,0x1B,0x4C,0x39,0x98,0x68,0x0C,0x00,0x00,0x75,0x12,
    0xF0,0xFF,0x05,0x3B,0x00,0x00,0x00,0x41,0x5B,0x41,0x5A,0x58,0xFF,0x25,0x24,0x00,0x00,0x00,
    0x41,0x5B,0x41,0x5A,0x58,0xC7,0x44,0x24,0x50,0xB0,0x9B,0x66,0x16,0x89,0x44,0x24,0x58,
    0x49,0xBB,0,0,0,0,0,0,0,0,0x41,0xFF,0xD3,0xFF,0x25,0x00,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
static bool install_floating_carrier_warning_guard(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base=(BYTE*)executable; BYTE* target=base+RVA_CARRIER_WARNING_NOTIFICATION_GATE;
    static const BYTE expected[17]={0xC7,0x44,0x24,0x50,0xB0,0x9B,0x66,0x16,0x89,0x44,0x24,0x58,0xE8,0x16,0x34,0xF1,0xFE};
    if(!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(CARRIER_WARNING_GUARD_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); if(!cave)return false;
    memcpy(cave,CARRIER_WARNING_GUARD_TEMPLATE,sizeof(CARRIER_WARNING_GUARD_TEMPLATE));
    UINT64 stateAddress=(UINT64)&g_carrierRideLinkState, dispatcher=(UINT64)(base+RVA_ENTITY_MESSAGE_DISPATCH), continuation=(UINT64)(base+RVA_CARRIER_WARNING_NOTIFICATION_CONTINUE);
    memcpy(cave+18u,&stateAddress,8); memcpy(cave+137u,&dispatcher,8); memcpy(cave+154u,&continuation,8);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(CARRIER_WARNING_GUARD_TEMPLATE));
    BYTE replacement[17]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90,0x90}; UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false; DWORD oldProtection=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement)); bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch(); if(!installed)return false;
    g_carrierWarningProducerHits=(volatile LONG*)(cave+162u);g_carrierWarningSuppressedHits=(volatile LONG*)(cave+166u);g_carrierWarningGuardInstalled=1;return true;
}
static bool install_floating_carrier_gate(HMODULE executable) {
    if(!g_allowFloatingCarrier)return true; BYTE* base=(BYTE*)executable; BYTE* context=base+RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT-6u; BYTE* immediate=base+RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT;
    static const BYTE expectedContext[9]={0x41,0x8D,0x45,0xFF,0x83,0xF8,0x01,0x76,0x49};
    if(!readable_range(context,sizeof(expectedContext))||!bytes_equal(context,expectedContext,sizeof(expectedContext)))return false;
    if(!freeze_for_patch(immediate,1u))return false; DWORD oldProtection=0;
    if(!VirtualProtect(immediate,1u,PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    *immediate=0x02;FlushInstructionCache((HANDLE)(INT64)-1,immediate,1u);bool installed=*immediate==0x02;
    if(!installed){*immediate=0x01;FlushInstructionCache((HANDLE)(INT64)-1,immediate,1u);}DWORD ignored=0;VirtualProtect(immediate,1u,oldProtection,&ignored);unfreeze_after_patch();
    if(!installed)return false;g_carrierGateInstalled=1;return true;
}

static const BYTE DRIVE_HOOK_NO_TELEMETRY_TEMPLATE[72] = {
    0xF3,0x0F,0x1E,0xFA,0xC4,0xC1,0x4A,0x59,0xCC,0xC5,0xFA,0x59,0xC1,0x50,
    0x48,0xB8,0,0,0,0,0,0,0,0,0x48,0x39,0x03,0x75,0x08,
    0xC5,0xFA,0x59,0x05,0x1F,0,0,0,0x58,0xC5,0xFA,0x11,0x47,0x18,0xFF,0x25,0x03,0,0,0,
    0xCC,0xCC,0xCC,0,0,0,0,0,0,0,0,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0,0,0,0
};
static bool install_drive_force_hook(HMODULE executable) {
    BYTE* base=(BYTE*)executable; BYTE* target=base+RVA_DRIVE_TORQUE_HOOK; void** v=(void**)(base+RVA_COFFIN_PHYSICS_VTABLE);
    static const BYTE expected[18]={0xC4,0xC1,0x4A,0x59,0xCC,0xC5,0xFA,0x59,0xC1,0xC5,0xFA,0x11,0x47,0x18,0xC5,0xE2,0x5C,0xC6};
    if(!readable_range(v,4u*sizeof(void*))||v[0]!=(void*)(base+RVA_COFFIN_VFUNC_0)||v[1]!=(void*)(base+RVA_COFFIN_VFUNC_1)||v[2]!=(void*)(base+RVA_COFFIN_VFUNC_2)||v[3]!=(void*)(base+RVA_COFFIN_VFUNC_3)||!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);if(!cave)return false;
    memcpy(cave,DRIVE_HOOK_NO_TELEMETRY_TEMPLATE,sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE)); UINT64 coffinVtable=(UINT64)(base+RVA_COFFIN_PHYSICS_VTABLE),returnAddress=(UINT64)(target+14u);float factor=1.0f;
    memcpy(cave+16u,&coffinVtable,8);memcpy(cave+52u,&returnAddress,8);memcpy(cave+68u,&factor,4);FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE));
    BYTE replacement[14]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0};UINT64 caveAddress=(UINT64)cave;memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false;DWORD oldProtection=0;if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,14u);FlushInstructionCache((HANDLE)(INT64)-1,target,14u);}DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();if(!installed)return false;
    g_driveFactorBits=(volatile LONG*)(cave+68u);g_driveHookInstalled=1;return true;
}
static bool publish_drive_factor_percent(int percent) {
    if(!g_driveHookInstalled||!g_driveFactorBits)return false;float factor=(float)percent/100.0f;LONG bits=0;memcpy(&bits,&factor,4);__atomic_store_n(g_driveFactorBits,bits,__ATOMIC_RELEASE);return __atomic_load_n(g_driveFactorBits,__ATOMIC_ACQUIRE)==bits;
}
static int string_compare(const char* a,const char* b){if(!a||!b)return a==b?0:1;while(*a&&*b&&*a==*b){++a;++b;}return(unsigned char)*a-(unsigned char)*b;}
static const char* rtti_type_name(void* object,void** vtable){if(!object||!vtable||!readable_range(vtable,sizeof(void*)))return 0;typedef void*(__fastcall* GetRttiFn)(void*);GetRttiFn getRtti=(GetRttiFn)vtable[0];if(!getRtti||!readable_range((void*)getRtti,1))return 0;void* rtti=getRtti(object);if(!readable_range(rtti,0x48))return 0;if(*((BYTE*)rtti+4)!=4)return 0;const char* typeName=*(const char**)((BYTE*)rtti+0x40);return readable_range(typeName,1)?typeName:0;}
static bool object_has_uuid(void* object,UINT64 low,UINT64 high){if(!object||!readable_range((BYTE*)object+0x10u,16u))return false;const UINT64* uuid=(const UINT64*)((BYTE*)object+0x10u);return uuid[0]==low&&uuid[1]==high;}
static bool object_has_exact_type(void* object,const char* expectedType){if(!object||!readable_range(object,sizeof(void*)))return false;void** vtable=*(void***)object;if(!vtable)return false;++g_rttiLookups;const char* typeName=rtti_type_name(object,vtable);return typeName&&string_compare(typeName,expectedType)==0;}
static bool patch_coffin_physics(void* resource) {
    if(!resource||!readable_range((BYTE*)resource+OFF_SLIP_SPEED,sizeof(float))||!publish_drive_factor_percent(100))return false;
    float* land=(float*)((BYTE*)resource+OFF_LAND_TOP_SPEED);float* gear=(float*)((BYTE*)resource+OFF_FINAL_GEAR_RATIO);float* boost=(float*)((BYTE*)resource+OFF_LAND_BOOST_TOP_SPEED);float* water=(float*)((BYTE*)resource+OFF_WATER_TOP_SPEED);float* waterBoost=(float*)((BYTE*)resource+OFF_WATER_BOOST_TOP_SPEED);float* slip=(float*)((BYTE*)resource+OFF_SLIP_SPEED);
    float oldLand=*land,oldGear=*gear,oldBoost=*boost,oldWater=*water,oldWaterBoost=*waterBoost,oldSlip=*slip;
    bool ownWater=g_waterBaseline>0.0f&&g_waterBoostBaseline>0.0f&&f_near(oldWater,g_lastAppliedWater,0.05f)&&f_near(oldWaterBoost,g_lastAppliedWaterBoost,0.05f);float baseWater=ownWater?g_waterBaseline:oldWater,baseWaterBoost=ownWater?g_waterBoostBaseline:oldWaterBoost;
    bool ownGear=g_gearRatioBaseline>0.0f&&f_near(oldGear,g_lastAppliedGearRatio,0.05f);float baseGear=ownGear?g_gearRatioBaseline:oldGear;
    if(baseWater<10.0f||baseWater>250.0f||baseWaterBoost<10.0f||baseWaterBoost>300.0f||!f_near(baseGear,15.0f,0.05f)){log_line("WARNING: Coffin physics layout/value guard failed; resource skipped.");return false;}
    float finalLand=baseWater*(float)g_speedPercent/100.0f,finalBoost=baseWaterBoost*(float)g_speedPercent/100.0f,finalWater=f_max(oldWater,finalLand),finalWaterBoost=f_max(oldWaterBoost,finalBoost),finalGear=baseGear*(float)g_gearRatioPercent/100.0f,finalSlip=f_max(oldSlip,finalLand);
    if(finalLand>650.0f||finalBoost>850.0f||finalWater>650.0f||finalWaterBoost>850.0f||finalGear<1.5f||finalGear>30.0f||!(f_near(oldSlip,39.0f,0.05f)||oldSlip>=finalLand)||!value_is_native_or_compatible(oldLand,40.0f,finalLand)||!value_is_native_or_compatible(oldBoost,60.0f,finalBoost)){log_line("WARNING: Coffin physics safety/conflict guard failed; resource skipped.");return false;}
    int a=write_float_checked(land,finalLand);int b=a?write_float_checked(boost,finalBoost):0;int c=b?write_float_checked(water,finalWater):0;int d=c?write_float_checked(waterBoost,finalWaterBoost):0;int e=d?write_float_checked(gear,finalGear):0;int f=e?write_float_checked(slip,finalSlip):0;bool drive=f&&publish_drive_factor_percent(g_driveForcePercent);
    if(!drive){publish_drive_factor_percent(100);if(f==2)write_float_checked(slip,oldSlip);if(e==2)write_float_checked(gear,oldGear);if(d==2)write_float_checked(waterBoost,oldWaterBoost);if(c==2)write_float_checked(water,oldWater);if(b==2)write_float_checked(boost,oldBoost);if(a==2)write_float_checked(land,oldLand);log_line("ERROR: Coffin physics transaction failed and was rolled back.");return false;}
    g_waterBaseline=baseWater;g_waterBoostBaseline=baseWaterBoost;g_lastAppliedWater=finalWater;g_lastAppliedWaterBoost=finalWaterBoost;g_gearRatioBaseline=baseGear;g_lastAppliedGearRatio=finalGear;g_seenPhysicsResource=resource;g_physicsReady=1;
    LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Coffin physics land ");log_float2(&m,oldLand);log_text(&m," -> ");log_float2(&m,finalLand);log_text(&m,", boost ");log_float2(&m,oldBoost);log_text(&m," -> ");log_float2(&m,finalBoost);log_text(&m,", water ");log_float2(&m,oldWater);log_text(&m," -> ");log_float2(&m,finalWater);log_text(&m,", water boost ");log_float2(&m,oldWaterBoost);log_text(&m," -> ");log_float2(&m,finalWaterBoost);log_text(&m,", slip ");log_float2(&m,oldSlip);log_text(&m," -> ");log_float2(&m,finalSlip);log_text(&m,", gear ratio ");log_float2(&m,oldGear);log_text(&m," -> ");log_float2(&m,finalGear);log_text(&m,", drive factor ");log_uint(&m,(UINT64)g_driveForcePercent);log_text(&m,"%.\r\n");append_log(&m);return true;
}
struct RawArray{UINT32 count;UINT32 capacity;void* entries;};struct StreamingEvents{void** vtable;};
static bool validate_group_array(const RawArray* objects){if(!objects||!readable_range(objects,sizeof(RawArray)))return false;if(objects->count<1u||objects->count>1000000u||objects->capacity<objects->count||objects->capacity>1000000u||!objects->entries)return false;return readable_range(objects->entries,sizeof(void*));}
static bool group_object_at(const RawArray* objects,UINT32 index,void** object){if(!objects||!object||objects->count<=index)return false;void** slot=(void**)((BYTE*)objects->entries+(SIZE_T)index*sizeof(void*));if(!readable_range(slot,sizeof(void*)))return false;*object=*slot;return true;}
static void inspect_target_positions(const RawArray* objects,bool countTelemetry){if(!validate_group_array(objects))return;AcquireSRWLockExclusive(&g_targetStateLock);if(countTelemetry){++g_callbackGroups;g_callbackObjects+=objects->count;}void* candidate=0;if(!g_physicsReady&&group_object_at(objects,COFFIN_PHYSICS_OBJECT_INDEX,&candidate)){++g_targetChecks;if(object_has_uuid(candidate,COFFIN_PHYSICS_UUID_LOW,COFFIN_PHYSICS_UUID_HIGH)&&object_has_exact_type(candidate,"DSPhysicsCoffinResource")){if(patch_coffin_physics(candidate)){publish_patch_complete(true);LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Patch complete. groups=");log_uint(&m,g_callbackGroups);log_text(&m," objects=");log_uint(&m,g_callbackObjects);log_text(&m," rttiLookups=");log_uint(&m,g_rttiLookups);log_text(&m," targetChecks=");log_uint(&m,g_targetChecks);log_text(&m,".\r\n");append_log(&m);}}}ReleaseSRWLockExclusive(&g_targetStateLock);}
static void inspect_target_unload_positions(const RawArray* objects){if(!validate_group_array(objects)||!g_seenPhysicsResource)return;AcquireSRWLockExclusive(&g_targetStateLock);void* candidate=0;if(group_object_at(objects,COFFIN_PHYSICS_OBJECT_INDEX,&candidate)&&candidate==g_seenPhysicsResource){publish_drive_factor_percent(100);g_seenPhysicsResource=0;g_physicsReady=0;publish_patch_complete(false);log_line("STATE: Coffin physics resource unloaded; drive factor neutralized.");}ReleaseSRWLockExclusive(&g_targetStateLock);}
static void __fastcall on_finish_load(StreamingEvents*,const RawArray* objects){if(!g_enabled||patch_complete_acquire())return;inspect_target_positions(objects,true);}static void __fastcall on_before_unload(StreamingEvents*,const RawArray* objects){inspect_target_unload_positions(objects);}static void __fastcall on_load_asset(StreamingEvents*,const RawArray*){}
static void* g_listenerVtable[3]={(void*)&on_finish_load,(void*)&on_before_unload,(void*)&on_load_asset};static StreamingEvents g_listener={g_listenerVtable};
static UINT64 resolve_rip(UINT64 instruction,UINT32 displacementOffset){int displacement=*(int*)(instruction+displacementOffset);return instruction+displacementOffset+4u+(INT64)displacement;}
static UINT64 find_streaming_manager_global(HMODULE executable){BYTE* base=(BYTE*)executable;BYTE* candidate=base+RVA_STREAMING_SIGNATURE;if(!readable_range(candidate,28u))return 0;static const BYTE signature[28]={0x48,0x89,0x05,0,0,0,0,0xE8,0,0,0,0,0x33,0xD2,0x41,0xB8,0xF8,0x0A,0x00,0x00,0x48,0x8B,0xC8,0x48,0x8B,0xD8,0xE8,0};static const BYTE mask[28]={1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};for(UINT32 i=0;i<28u;++i)if(mask[i]&&candidate[i]!=signature[i])return 0;UINT64 resolved=resolve_rip((UINT64)candidate,3u);UINT64 expected=(UINT64)(base+RVA_STREAMING_MANAGER_GLOBAL);return resolved==expected?resolved:0;}
typedef void(__fastcall* AddListenerFn)(void*,void*);typedef void(__fastcall* RemoveListenerFn)(void*,void*);
static bool register_streaming_listener(void* manager){if(!manager||!readable_range((BYTE*)manager+0x578,sizeof(void*)))return false;void* system=*(void**)((BYTE*)manager+0x578);BYTE* exe=(BYTE*)GetModuleHandleW(0);if(!system||!exe||!readable_range(system,sizeof(void*)))return false;void** vtable=*(void***)system;if(vtable!=(void**)(exe+RVA_STREAMING_SYSTEM_VTABLE)||!readable_range(vtable,5u*sizeof(void*)))return false;AddListenerFn add=(AddListenerFn)vtable[3];RemoveListenerFn remove=(RemoveListenerFn)vtable[4];if(add!=(AddListenerFn)(exe+RVA_STREAMING_ADD_LISTENER)||remove!=(RemoveListenerFn)(exe+RVA_STREAMING_REMOVE_LISTENER)||listener_state_acquire()!=0)return false;add(system,&g_listener);g_streamingSystem=system;g_removeStreamingListener=remove;__atomic_store_n(&g_listenerRegistered,1,__ATOMIC_RELEASE);return true;}
static bool unregister_streaming_listener(){LONG state=listener_state_acquire();if(state==0)return true;if(state!=1||!g_streamingSystem||!g_removeStreamingListener)return false;LONG expected=1;if(!__atomic_compare_exchange_n(&g_listenerRegistered,&expected,2,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return expected==0;g_removeStreamingListener(g_streamingSystem,&g_listener);g_removeStreamingListener=0;g_streamingSystem=0;__atomic_store_n(&g_listenerRegistered,0,__ATOMIC_RELEASE);return true;}
static void inspect_already_loaded_group(void* streamingSystem,UINT32 groupId){if(!streamingSystem||!readable_range((BYTE*)streamingSystem+OFF_STREAMING_GROUP_LOCK,sizeof(void*)))return;void* lock=(BYTE*)streamingSystem+OFF_STREAMING_GROUP_LOCK;AcquireSRWLockShared(lock);UINT32* arrayHeader=(UINT32*)((BYTE*)streamingSystem+OFF_STREAMING_GROUP_COUNT);void*** tableField=(void***)((BYTE*)streamingSystem+OFF_STREAMING_GROUP_TABLE);if(readable_range(arrayHeader,2u*sizeof(UINT32))&&readable_range(tableField,sizeof(void*))&&arrayHeader[0]>groupId&&arrayHeader[0]<=arrayHeader[1]&&arrayHeader[1]<1000000u&&*tableField){BYTE* entry=(BYTE*)(*tableField)+(SIZE_T)groupId*STREAMING_GROUP_ENTRY_STRIDE;if(readable_range(entry,16u)){void* descriptor=*(void**)(entry+0u);void* loadedGroup=*(void**)(entry+8u);if(descriptor&&loadedGroup&&readable_range(descriptor,sizeof(UINT32))&&*(UINT32*)descriptor==groupId&&readable_range((BYTE*)loadedGroup+OFF_LOADED_GROUP_OBJECTS,sizeof(RawArray)))inspect_target_positions((const RawArray*)((BYTE*)loadedGroup+OFF_LOADED_GROUP_OBJECTS),false);}}ReleaseSRWLockShared(lock);}
static DWORD __stdcall worker(LPVOID){read_configuration();log_line("DS2 Coffin Board All-Terrain Speed v1.1.7-native-replay loaded.");if(!g_enabled){log_line("Disabled in INI; no game memory was changed.");return 0;}HMODULE executable=GetModuleHandleW(0);if(!executable||!validate_target_build(executable)){log_line("ERROR: Unsupported DS2.exe. Expected Steam build 1.10.89.0; no patch applied.");return 0;}UINT64 globalAddress=find_streaming_manager_global(executable);if(!globalAddress||!readable_range((void*)globalAddress,sizeof(void*))){log_line("ERROR: Exact StreamingManager anchor did not match; no patch applied.");return 0;}void** managerGlobal=(void**)globalAddress;void* manager=0;for(int attempt=0;attempt<1200;++attempt){if(readable_range(managerGlobal,sizeof(void*)))manager=*managerGlobal;if(manager)break;Sleep(50);}if(!manager){log_line("ERROR: StreamingManager did not initialize within 60 seconds.");return 0;}
    if(!install_floating_carrier_link_preserve(executable)){log_line("ERROR: Exact Coffin/Floating Carrier transition anchor did not match.");return 0;}
    if(!install_floating_carrier_detach_guard(executable)){log_line("ERROR: Exact Floating Carrier detach-event anchor did not match.");return 0;}
    if(!install_floating_carrier_warning_guard(executable)){log_line("ERROR: Exact Floating Carrier notification anchor did not match.");return 0;}
    if(!install_floating_carrier_gate(executable)){log_line("ERROR: Exact Coffin/Floating Carrier gate anchor did not match.");return 0;}
    if(g_carrierGateInstalled)log_line("Coffin-only Floating Carrier mount gate installed; the active loaded carrier remains linked during the ride.");
    if(g_carrierLinkPreserveInstalled)log_line("Coffin-only RideVehicle carrier-disconnect transaction bypass installed (v1.1.7 native resolver replay + post-call Coffin guard active).");
    if(g_carrierDetachGuardInstalled)log_line("Coffin-only Floating Carrier jump-detach event guard installed.");
    if(g_carrierWarningGuardInstalled)log_line("Coffin-only Floating Carrier overextension HUD/voice notification guard installed.");
    if(!install_drive_force_hook(executable)){log_line("ERROR: Exact Coffin drive-force hook anchor/vtable did not match; no speed patch applied.");return 0;}
    {LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Coffin-only drive-force hook installed neutral at 100%; requested ");log_uint(&m,(UINT64)g_driveForcePercent);log_text(&m,"% activates only after the resource patch.\r\n");append_log(&m);}
    if(!register_streaming_listener(manager)){log_line("ERROR: Could not register resource listener; no speed patch was applied.");return 0;}
    {LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Listener registered. speed=");log_uint(&m,(UINT64)g_speedPercent);log_text(&m,"%, acceleration=");log_uint(&m,(UINT64)g_accelerationPercent);log_text(&m,"%, drive=");log_uint(&m,(UINT64)g_driveForcePercent);log_text(&m,"%, gearRatioPercent=");log_uint(&m,(UINT64)g_gearRatioPercent);log_text(&m,"%, AllowFloatingCarrier=");log_uint(&m,(UINT64)(g_allowFloatingCarrier!=0));log_text(&m,".\r\n");append_log(&m);}
    for(UINT32 poll=0;poll<1200u&&!patch_complete_acquire();++poll){if((poll%10u)==0u)inspect_already_loaded_group(g_streamingSystem,COFFIN_PHYSICS_GROUP_ID);if(!patch_complete_acquire())Sleep(50);}bool completed=patch_complete_acquire();if(!unregister_streaming_listener()){log_line("LIFECYCLE ERROR: exact native listener removal failed; restart DS2 before changing this ASI.");return 0;}if(completed)log_line("LIFECYCLE: patch completion observed; one-shot listener removed outside callbacks.");else log_line("LIFECYCLE WARNING: 60-second resource window expired; listener removed without a completed speed patch.");return 0;}
extern "C" __declspec(dllexport) void InitializeASI() {}
extern "C" int __stdcall DllMain(HINSTANCE instance,DWORD reason,LPVOID){if(reason==DLL_PROCESS_ATTACH){if(!is_main_game_process())return TRUE;g_module=(HMODULE)instance;DisableThreadLibraryCalls(g_module);g_mutex=CreateMutexW(0,FALSE,L"Local\\DS2_CoffinBoardAllTerrainSpeed_v0_1");if(!g_mutex)return TRUE;if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(g_mutex);g_mutex=0;return TRUE;}HANDLE thread=CreateThread(0,0,worker,0,0,0);if(thread)CloseHandle(thread);}else if(reason==DLL_PROCESS_DETACH){if(g_mutex){CloseHandle(g_mutex);g_mutex=0;}}return TRUE;}
