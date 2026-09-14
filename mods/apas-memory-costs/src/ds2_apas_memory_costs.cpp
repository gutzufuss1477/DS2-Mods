// DS2 APAS Memory Costs v1.0.0
// Runtime APAS Enhancement memory-cost override for DEATH STRANDING 2: ON THE BEACH.
// No hard game-version gate. Offsets were identified on Steam DS2.exe 1.10.89.0.
// Freestanding x64 Windows ASI; no CRT dependency.

extern "C" {
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long long SIZE_T;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
typedef WCHAR* LPWSTR;
typedef DWORD (__stdcall *LPTHREAD_START_ROUTINE)(LPVOID);

struct MEMORY_BASIC_INFORMATION_X64 {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    DWORD Alignment1;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
    DWORD Alignment2;
};

__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE, LPWSTR, DWORD);
__declspec(dllimport) HANDLE __stdcall CreateThread(LPVOID, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, DWORD*);
__declspec(dllimport) BOOL __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) void __stdcall Sleep(DWORD);
__declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, LPVOID);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(LPCVOID, MEMORY_BASIC_INFORMATION_X64*, SIZE_T);
__declspec(dllimport) BOOL __stdcall VirtualProtect(LPVOID, SIZE_T, DWORD, DWORD*);
__declspec(dllimport) DWORD __stdcall GetPrivateProfileStringW(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, DWORD, LPCWSTR);
__declspec(dllimport) HANDLE __stdcall CreateMutexW(LPVOID, BOOL, LPCWSTR);
__declspec(dllimport) DWORD __stdcall GetLastError(void);
}

typedef unsigned long long u64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned char u8;

static HMODULE g_self = (HMODULE)0;
static WCHAR g_ini_path[1024];
static WCHAR g_log_path[1024];
static BOOL g_debug = 0;
static HANDLE g_instance_mutex = (HANDLE)0;

// Runtime layout discovered/verified on Steam DS2.exe 1.10.89.0.
static const u64 kManagerGlobalRva = 0x0623E5D0ull;
static const u32 kEntryCount = 0x37u;
static const u32 kEntryArrayOffset = 0x30u;
static const u32 kResourcePtrOffset = 0x70u;
static const u32 kCostOffset = 0x24u; // DSApasEnhancementResource::EnhancementPoint

static const DWORD FILE_APPEND_DATA_X = 0x00000004u;
static const DWORD GENERIC_WRITE_X = 0x40000000u;
static const DWORD FILE_SHARE_READ_X = 0x00000001u;
static const DWORD FILE_SHARE_WRITE_X = 0x00000002u;
static const DWORD FILE_SHARE_DELETE_X = 0x00000004u;
static const DWORD OPEN_ALWAYS_X = 4u;
static const DWORD CREATE_ALWAYS_X = 2u;
static const DWORD FILE_ATTRIBUTE_NORMAL_X = 0x00000080u;
static const DWORD PAGE_READWRITE_X = 0x04u;
static const DWORD MEM_COMMIT_X = 0x1000u;
static const DWORD PAGE_NOACCESS_X = 0x01u;
static const DWORD PAGE_GUARD_X = 0x100u;
static const DWORD ERROR_ALREADY_EXISTS_X = 183u;
static const HANDLE INVALID_HANDLE_VALUE_X = (HANDLE)(~(u64)0);

static u32 StrLenA(const char* s) {
    u32 n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static u32 StrLenW(const WCHAR* s) {
    u32 n = 0;
    if (!s) return 0;
    while (s[n]) ++n;
    return n;
}

static void CopyW(WCHAR* dst, u32 cap, const WCHAR* src) {
    if (!dst || cap == 0) return;
    u32 i = 0;
    if (src) {
        while (src[i] && i + 1 < cap) {
            dst[i] = src[i];
            ++i;
        }
    }
    dst[i] = 0;
}

static void AppendW(WCHAR* dst, u32 cap, const WCHAR* src) {
    if (!dst || !src || cap == 0) return;
    u32 d = StrLenW(dst);
    u32 s = 0;
    while (src[s] && d + 1 < cap) dst[d++] = src[s++];
    dst[d] = 0;
}

static void BuildSiblingPath(WCHAR* out, u32 cap, const WCHAR* filename) {
    if (!out || cap == 0) return;
    out[0] = 0;
    WCHAR full[1024];
    full[0] = 0;
    DWORD n = GetModuleFileNameW(g_self, full, 1023);
    if (n == 0 || n >= 1023) return;
    full[n] = 0;
    i32 slash = -1;
    for (u32 i = 0; full[i]; ++i) {
        if (full[i] == L'\\' || full[i] == L'/') slash = (i32)i;
    }
    if (slash >= 0) full[(u32)slash + 1] = 0;
    else full[0] = 0;
    CopyW(out, cap, full);
    AppendW(out, cap, filename);
}

static u32 AppendA(char* dst, u32 cap, u32 pos, const char* src) {
    if (!dst || !src || cap == 0) return pos;
    while (*src && pos + 1 < cap) dst[pos++] = *src++;
    dst[pos] = 0;
    return pos;
}

static u32 AppendUInt(char* dst, u32 cap, u32 pos, u32 value) {
    char tmp[16];
    u32 n = 0;
    do {
        tmp[n++] = (char)('0' + (value % 10));
        value /= 10;
    } while (value && n < 15);
    while (n && pos + 1 < cap) dst[pos++] = tmp[--n];
    dst[pos] = 0;
    return pos;
}

static void ResetLog() {
    if (!g_log_path[0]) return;
    HANDLE h = CreateFileW(g_log_path, GENERIC_WRITE_X,
        FILE_SHARE_READ_X | FILE_SHARE_WRITE_X | FILE_SHARE_DELETE_X,
        (LPVOID)0, CREATE_ALWAYS_X, FILE_ATTRIBUTE_NORMAL_X, (HANDLE)0);
    if (h != INVALID_HANDLE_VALUE_X && h != (HANDLE)0) CloseHandle(h);
}

static void WriteLogRaw(const char* line) {
    if (!g_log_path[0] || !line) return;
    HANDLE h = CreateFileW(g_log_path, FILE_APPEND_DATA_X,
        FILE_SHARE_READ_X | FILE_SHARE_WRITE_X | FILE_SHARE_DELETE_X,
        (LPVOID)0, OPEN_ALWAYS_X, FILE_ATTRIBUTE_NORMAL_X, (HANDLE)0);
    if (h == INVALID_HANDLE_VALUE_X || h == (HANDLE)0) return;
    DWORD written = 0;
    WriteFile(h, line, StrLenA(line), &written, (LPVOID)0);
    static const char crlf[2] = {'\r','\n'};
    WriteFile(h, crlf, 2, &written, (LPVOID)0);
    CloseHandle(h);
}

static void LogInfo(const char* line) {
    if (g_debug) WriteLogRaw(line);
}

static void LogAlways(const char* line) {
    WriteLogRaw(line);
}

static BOOL ParseBoolSetting(const WCHAR* key, BOOL fallback, BOOL* out) {
    WCHAR value[64];
    value[0] = 0;
    DWORD n = GetPrivateProfileStringW(L"APASMemoryCosts", key, fallback ? L"1" : L"0", value, 63, g_ini_path);
    if (n == 1 && value[0] == L'0') { *out = 0; return 1; }
    if (n == 1 && value[0] == L'1') { *out = 1; return 1; }
    return 0;
}

static BOOL ParseCostSetting(i32* out) {
    WCHAR value[64];
    value[0] = 0;
    DWORD n = GetPrivateProfileStringW(L"APASMemoryCosts", L"GlobalCost", L"1", value, 63, g_ini_path);
    if (n == 0 || n > 7) return 0;
    u32 v = 0;
    for (DWORD i = 0; i < n; ++i) {
        WCHAR c = value[i];
        if (c < L'0' || c > L'9') return 0;
        v = v * 10u + (u32)(c - L'0');
        if (v > 1000000u) return 0;
    }
    *out = (i32)v;
    return 1;
}

static BOOL IsReadable(const void* p, SIZE_T need) {
    if (!p || need == 0) return 0;
    MEMORY_BASIC_INFORMATION_X64 mbi;
    SIZE_T got = VirtualQuery(p, &mbi, (SIZE_T)sizeof(mbi));
    if (got < (SIZE_T)sizeof(mbi)) return 0;
    if (mbi.State != MEM_COMMIT_X) return 0;
    if (mbi.Protect & PAGE_NOACCESS_X) return 0;
    if (mbi.Protect & PAGE_GUARD_X) return 0;
    u64 start = (u64)p;
    u64 base = (u64)mbi.BaseAddress;
    u64 end = start + (u64)need;
    u64 regionEnd = base + (u64)mbi.RegionSize;
    if (end < start) return 0;
    return end <= regionEnd;
}

static BOOL WriteCost(i32* field, i32 expected, i32 target) {
    DWORD oldProtect = 0;
    if (!VirtualProtect((LPVOID)field, (SIZE_T)sizeof(i32), PAGE_READWRITE_X, &oldProtect)) return 0;
    i32 exp = expected;
    BOOL exchanged = __atomic_compare_exchange_n(field, &exp, target, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST) ? 1 : 0;
    DWORD ignored = 0;
    BOOL restored = VirtualProtect((LPVOID)field, (SIZE_T)sizeof(i32), oldProtect, &ignored);
    return exchanged && restored && (*field == target);
}

struct PassResult {
    u32 entries;
    u32 resources;
    u32 already;
    u32 patched;
    u32 invalid;
    u32 failed;
};

static PassResult PatchPass(u8* base, i32 target) {
    PassResult r = {};

    void** globalSlot = (void**)(base + kManagerGlobalRva);
    if (!IsReadable(globalSlot, sizeof(void*))) return r;

    void* manager = *globalSlot;
    if (!IsReadable(manager, 0x1F0)) return r;

    for (u32 i = 0; i < kEntryCount; ++i) {
        void** entrySlot = (void**)((u8*)manager + kEntryArrayOffset + i * 8u);
        if (!IsReadable(entrySlot, sizeof(void*))) { ++r.invalid; continue; }

        void* entry = *entrySlot;
        if (!entry) continue;
        ++r.entries;
        if (!IsReadable(entry, 0x80)) { ++r.invalid; continue; }

        void** resourceSlot = (void**)((u8*)entry + kResourcePtrOffset);
        if (!IsReadable(resourceSlot, sizeof(void*))) { ++r.invalid; continue; }

        void* resource = *resourceSlot;
        if (!resource) continue;
        ++r.resources;
        if (!IsReadable(resource, 0x28)) { ++r.invalid; continue; }

        i32* field = (i32*)((u8*)resource + kCostOffset);
        i32 current = *field;
        if (current < 0 || current > 1000000) { ++r.invalid; continue; }
        if (current == target) { ++r.already; continue; }

        if (WriteCost(field, current, target)) ++r.patched;
        else ++r.failed;
    }

    return r;
}

static void LogConfig(i32 cost, BOOL enabled) {
    char line[256];
    u32 p = 0;
    p = AppendA(line, sizeof(line), p, "[INFO] DS2 APAS Memory Costs v1.0.0");
    p = AppendA(line, sizeof(line), p, " | Enabled=");
    p = AppendUInt(line, sizeof(line), p, enabled ? 1u : 0u);
    p = AppendA(line, sizeof(line), p, " GlobalCost=");
    p = AppendUInt(line, sizeof(line), p, (u32)cost);
    p = AppendA(line, sizeof(line), p, " DebugLog=");
    p = AppendUInt(line, sizeof(line), p, g_debug ? 1u : 0u);
    WriteLogRaw(line);
}

static void LogPass(const PassResult& r, i32 target) {
    char line[384];
    u32 p = 0;
    p = AppendA(line, sizeof(line), p, "[INFO] APAS resources: entries=");
    p = AppendUInt(line, sizeof(line), p, r.entries);
    p = AppendA(line, sizeof(line), p, " resources=");
    p = AppendUInt(line, sizeof(line), p, r.resources);
    p = AppendA(line, sizeof(line), p, " patched_now=");
    p = AppendUInt(line, sizeof(line), p, r.patched);
    p = AppendA(line, sizeof(line), p, " already_target=");
    p = AppendUInt(line, sizeof(line), p, r.already);
    p = AppendA(line, sizeof(line), p, " invalid=");
    p = AppendUInt(line, sizeof(line), p, r.invalid);
    p = AppendA(line, sizeof(line), p, " failed=");
    p = AppendUInt(line, sizeof(line), p, r.failed);
    p = AppendA(line, sizeof(line), p, " target=");
    p = AppendUInt(line, sizeof(line), p, (u32)target);
    WriteLogRaw(line);
}

static DWORD __stdcall Worker(LPVOID) {
    BuildSiblingPath(g_ini_path, 1024, L"ds2_apas_memory_costs.ini");
    BuildSiblingPath(g_log_path, 1024, L"ds2_apas_memory_costs.log");

    // Process-local single-instance guard; no persistent lock file is created.
    g_instance_mutex = CreateMutexW((LPVOID)0, 0, L"Local\\DS2_APAS_Memory_Costs_v1");
    if (!g_instance_mutex || g_instance_mutex == INVALID_HANDLE_VALUE_X) return 0;
    if (GetLastError() == ERROR_ALREADY_EXISTS_X) return 0;

    BOOL enabled = 1;
    BOOL debug = 0;
    i32 cost = 1;

    if (!ParseBoolSetting(L"DebugLog", 0, &debug)) {
        LogAlways("[ERROR] Invalid DebugLog. Expected 0 or 1.");
        return 0;
    }
    g_debug = debug;
    if (g_debug) ResetLog();

    if (!ParseBoolSetting(L"Enabled", 1, &enabled)) {
        LogAlways("[ERROR] Invalid Enabled. Expected 0 or 1.");
        return 0;
    }
    if (!ParseCostSetting(&cost)) {
        LogAlways("[ERROR] Invalid GlobalCost. Expected decimal 0..1000000.");
        return 0;
    }

    if (g_debug) LogConfig(cost, enabled);
    if (!enabled) {
        LogInfo("[INFO] Mod disabled by INI.");
        return 0;
    }

    u8* base = (u8*)GetModuleHandleW((LPCWSTR)0);
    if (!base) {
        LogAlways("[ERROR] Could not resolve DS2.exe module base.");
        return 0;
    }

    LogInfo("[INFO] No hard game-version verification is used.");
    LogInfo("[INFO] APAS path: manager+0x30[index] -> entry+0x70 -> resource+0x24 (EnhancementPoint).");

    u32 lastResources = 0xFFFFFFFFu;
    u32 lastInvalid = 0xFFFFFFFFu;
    u32 lastFailed = 0xFFFFFFFFu;
    BOOL managerSeen = 0;

    for (;;) {
        PassResult r = PatchPass(base, cost);
        if (r.entries || r.resources) managerSeen = 1;

        if (g_debug && (r.patched || r.resources != lastResources || r.invalid != lastInvalid || r.failed != lastFailed)) {
            LogPass(r, cost);
        }
        if (r.failed) LogAlways("[ERROR] One or more APAS cost writes failed; retrying on later passes.");

        lastResources = r.resources;
        lastInvalid = r.invalid;
        lastFailed = r.failed;
        Sleep(managerSeen ? 1000u : 250u);
    }
}

extern "C" BOOL __stdcall DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == 1u) {
        g_self = hModule;
        DisableThreadLibraryCalls(hModule);
        HANDLE h = CreateThread((LPVOID)0, 0, Worker, (LPVOID)0, 0, (DWORD*)0);
        if (h && h != INVALID_HANDLE_VALUE_X) CloseHandle(h);
    }
    return 1;
}
