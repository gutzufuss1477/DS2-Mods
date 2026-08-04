// Pickup Truck Hill Assist v1.2.0
// DEATH STRANDING 2: ON THE BEACH - Steam 1.10.89.0
// Public source corresponding to the validated targeted release.
// Uses exact slope-table signatures and a measured late streaming window.

extern "C" {
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long SIZE_T;
typedef long long INT64;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
typedef WCHAR* LPWSTR;
typedef const char* LPCSTR;
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

__declspec(dllimport) BOOL __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HANDLE __stdcall CreateThread(LPVOID, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, DWORD*);
__declspec(dllimport) void __stdcall Sleep(DWORD);
__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE, LPWSTR, DWORD);
__declspec(dllimport) HANDLE __stdcall CreateMutexW(LPVOID, BOOL, LPCWSTR);
__declspec(dllimport) DWORD __stdcall GetLastError();
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
__declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, LPVOID);
__declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE, long, long*, DWORD);
__declspec(dllimport) void __stdcall GetLocalTime(SYSTEMTIME_X*);
__declspec(dllimport) unsigned int __stdcall GetPrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(LPCVOID, MEMORY_BASIC_INFORMATION_X64*, SIZE_T);
__declspec(dllimport) BOOL __stdcall VirtualProtect(LPVOID, SIZE_T, DWORD, DWORD*);
}

extern "C" int _fltused = 0;
extern "C" void* memset(void* dst, int value, SIZE_T size) {
    BYTE* p = (BYTE*)dst;
    for (SIZE_T i = 0; i < size; ++i) p[i] = (BYTE)value;
    return dst;
}
extern "C" void* memcpy(void* dst, const void* src, SIZE_T size) {
    BYTE* d = (BYTE*)dst;
    const BYTE* s = (const BYTE*)src;
    for (SIZE_T i = 0; i < size; ++i) d[i] = s[i];
    return dst;
}

#define TRUE 1
#define FALSE 0
#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
#define ERROR_ALREADY_EXISTS 183
#define GENERIC_WRITE 0x40000000
#define FILE_APPEND_DATA 0x00000004
#define FILE_SHARE_READ 0x00000001
#define OPEN_ALWAYS 4
#define FILE_ATTRIBUTE_NORMAL 0x80
#define FILE_END 2
#define INVALID_HANDLE_VALUE ((HANDLE)(INT64)-1)
#define MEM_COMMIT 0x1000
#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD 0x100

static HMODULE g_module = 0;
static HANDLE g_mutex = 0;
static WCHAR g_iniPath[512];
static WCHAR g_logPath[512];
static WCHAR g_modulePath[512];
static int g_enabled = 1;
static float g_gripMultiplier = 3.5f;
static float g_minFrictionFull = 1.5f;
static float g_startSlope = 6.0f;
static float g_fullSlope = 22.0f;
static float g_maxFriction = 8.0f;
static void* g_patchedResources[1024];
static int g_patchedResourceCount = 0;
static int g_totalEntriesChanged = 0;
static int g_listenerRegistered = 0;
static int g_stopAfterChangedEntries = 14;
static int g_patchComplete = 0;
static UINT64 g_scanStartGroup = 4300;
static UINT64 g_scanEndGroup = 4800;
static UINT64 g_windowObjects = 0;
static int g_scanWindowLogged = 0;
static int g_scanWindowEndedLogged = 0;

#define TYPE_CACHE_SIZE 4096
#define TYPE_CACHE_PROBES 8
#define TYPE_OTHER 1
#define TYPE_SLOPE_RESOURCE 2
#define TYPE_VEHICLE_RESOURCE 3

struct TypeCacheEntry {
    void* vtable;
    BYTE kind;
    BYTE padding[7];
};

static TypeCacheEntry g_typeCache[TYPE_CACHE_SIZE];
static UINT64 g_callbackGroups = 0;
static UINT64 g_callbackObjects = 0;
static UINT64 g_rttiLookups = 0;
static UINT64 g_typeCacheHits = 0;

static int scmp(const char* a, const char* b) {
    if (!a || !b) return a == b ? 0 : 1;
    while (*a && *b && *a == *b) { ++a; ++b; }
    return (unsigned char)*a - (unsigned char)*b;
}
static void wcopy(WCHAR* dst, int cap, const WCHAR* src) {
    if (!dst || cap <= 0) return;
    int i = 0;
    while (src && src[i] && i < cap - 1) { dst[i] = src[i]; ++i; }
    dst[i] = 0;
}
static WCHAR wlower_ascii(WCHAR c) {
    return (c >= L'A' && c <= L'Z') ? (WCHAR)(c + (L'a' - L'A')) : c;
}
static bool weq_icase_ascii(const WCHAR* a, const WCHAR* b) {
    if (!a || !b) return a == b;
    while (*a && *b) {
        if (wlower_ascii(*a) != wlower_ascii(*b)) return false;
        ++a; ++b;
    }
    return *a == 0 && *b == 0;
}
static bool is_main_game_process() {
    WCHAR path[512];
    DWORD n = GetModuleFileNameW(0, path, 511);
    if (n == 0 || n >= 511) return false;
    path[n] = 0;
    const WCHAR* base = path;
    for (DWORD i = 0; i < n; ++i) {
        if (path[i] == L'\\' || path[i] == L'/') base = &path[i + 1];
    }
    return weq_icase_ascii(base, L"DS2.exe");
}

struct LogBuffer { char data[1024]; int len; };
static void lb_init(LogBuffer* b) { b->len = 0; }
static void lb_char(LogBuffer* b, char c) { if (b->len < 1023) b->data[b->len++] = c; }
static void lb_text(LogBuffer* b, const char* s) { if (!s) return; while (*s && b->len < 1023) b->data[b->len++] = *s++; }
static void lb_uint(LogBuffer* b, UINT64 value) {
    char tmp[32]; int n = 0;
    if (value == 0) { lb_char(b, '0'); return; }
    while (value && n < 31) { tmp[n++] = (char)('0' + value % 10); value /= 10; }
    while (n) lb_char(b, tmp[--n]);
}
static void lb_hex(LogBuffer* b, UINT64 value) {
    static const char* h = "0123456789ABCDEF";
    lb_text(b, "0x");
    for (int i = 15; i >= 0; --i) lb_char(b, h[(value >> (i * 4)) & 0xF]);
}
static void lb_float2(LogBuffer* b, float value) {
    if (value < 0.0f) { lb_char(b, '-'); value = -value; }
    UINT64 whole = (UINT64)value;
    unsigned int frac = (unsigned int)((value - (float)whole) * 100.0f + 0.5f);
    if (frac >= 100) { ++whole; frac -= 100; }
    lb_uint(b, whole); lb_char(b, '.');
    lb_char(b, (char)('0' + (frac / 10) % 10));
    lb_char(b, (char)('0' + frac % 10));
}
static void append_log(LogBuffer* b) {
    HANDLE h = CreateFileW(g_logPath, FILE_APPEND_DATA, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (!h || h == INVALID_HANDLE_VALUE) return;
    SetFilePointer(h, 0, 0, FILE_END);
    DWORD written = 0;
    WriteFile(h, b->data, (DWORD)b->len, &written, 0);
    CloseHandle(h);
}
static void log_prefix(LogBuffer* b) {
    SYSTEMTIME_X st; GetLocalTime(&st);
    lb_char(b, '['); lb_uint(b, st.wYear); lb_char(b, '-');
    if (st.wMonth < 10) lb_char(b, '0'); lb_uint(b, st.wMonth); lb_char(b, '-');
    if (st.wDay < 10) lb_char(b, '0'); lb_uint(b, st.wDay); lb_char(b, ' ');
    if (st.wHour < 10) lb_char(b, '0'); lb_uint(b, st.wHour); lb_char(b, ':');
    if (st.wMinute < 10) lb_char(b, '0'); lb_uint(b, st.wMinute); lb_char(b, ':');
    if (st.wSecond < 10) lb_char(b, '0'); lb_uint(b, st.wSecond); lb_text(b, "] ");
}
static void log_line(const char* s) {
    LogBuffer b; lb_init(&b); log_prefix(&b); lb_text(&b, s); lb_text(&b, "\r\n"); append_log(&b);
}

static void make_paths() {
    g_modulePath[0] = 0;
    DWORD n = GetModuleFileNameW(g_module, g_modulePath, 511);
    if (n == 0 || n >= 511) {
        wcopy(g_iniPath, 512, L"PickupTruckHillAssist.ini");
        wcopy(g_logPath, 512, L"PickupTruckHillAssist.log");
        return;
    }
    int last = -1;
    for (int i = 0; g_modulePath[i]; ++i) if (g_modulePath[i] == L'\\' || g_modulePath[i] == L'/') last = i;
    int pos = 0;
    for (int i = 0; i <= last && pos < 430; ++i) {
        g_iniPath[pos] = g_modulePath[i]; g_logPath[pos] = g_modulePath[i]; ++pos;
    }
    const WCHAR* ini = L"PickupTruckHillAssist.ini";
    const WCHAR* log = L"PickupTruckHillAssist.log";
    int j = 0; while (ini[j] && pos + j < 511) { g_iniPath[pos + j] = ini[j]; ++j; } g_iniPath[pos + j] = 0;
    j = 0; while (log[j] && pos + j < 511) { g_logPath[pos + j] = log[j]; ++j; } g_logPath[pos + j] = 0;
}
static void read_config() {
    g_enabled = (int)GetPrivateProfileIntW(L"HillAssist", L"Enabled", 1, g_iniPath);
    int grip = (int)GetPrivateProfileIntW(L"HillAssist", L"GripPercent", 350, g_iniPath);
    int minF = (int)GetPrivateProfileIntW(L"HillAssist", L"MinimumFrictionPercent", 150, g_iniPath);
    int start = (int)GetPrivateProfileIntW(L"HillAssist", L"AssistStartSlopeDegrees", 6, g_iniPath);
    int full = (int)GetPrivateProfileIntW(L"HillAssist", L"FullAssistSlopeDegrees", 22, g_iniPath);
    int maxF = (int)GetPrivateProfileIntW(L"HillAssist", L"MaximumFrictionPercent", 800, g_iniPath);
    int stopAfter = (int)GetPrivateProfileIntW(L"HillAssist", L"StopAfterChangedEntries", 14, g_iniPath);
    int scanStart = (int)GetPrivateProfileIntW(L"HillAssist", L"ScanStartGroup", 4300, g_iniPath);
    int scanEnd = (int)GetPrivateProfileIntW(L"HillAssist", L"ScanEndGroup", 4800, g_iniPath);
    if (grip < 100) grip = 100; if (grip > 600) grip = 600;
    if (minF < 100) minF = 100; if (minF > 400) minF = 400;
    if (start < 0) start = 0; if (start > 60) start = 60;
    if (full <= start) full = start + 1; if (full > 80) full = 80;
    if (maxF < 100) maxF = 100; if (maxF > 2000) maxF = 2000;
    g_gripMultiplier = (float)grip / 100.0f;
    g_minFrictionFull = (float)minF / 100.0f;
    g_startSlope = (float)start;
    if (stopAfter < 0) stopAfter = 0;
    if (stopAfter > 256) stopAfter = 256;
    if (scanStart < 0) scanStart = 0;
    if (scanStart > 20000) scanStart = 20000;
    if (scanEnd <= scanStart) scanEnd = scanStart + 500;
    if (scanEnd > 30000) scanEnd = 30000;
    g_fullSlope = (float)full;
    g_maxFriction = (float)maxF / 100.0f;
    g_stopAfterChangedEntries = stopAfter;
    g_scanStartGroup = (UINT64)scanStart;
    g_scanEndGroup = (UINT64)scanEnd;
}

static bool readable_range(const void* ptr, SIZE_T bytes) {
    if (!ptr || bytes == 0) return false;
    MEMORY_BASIC_INFORMATION_X64 mbi;
    if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD) || (mbi.Protect & PAGE_NOACCESS)) return false;
    UINT64 start = (UINT64)mbi.BaseAddress;
    UINT64 end = start + (UINT64)mbi.RegionSize;
    UINT64 p = (UINT64)ptr;
    return p >= start && p + bytes >= p && p + bytes <= end;
}
static bool writable_range(const void* ptr, SIZE_T bytes) {
    if (!ptr || bytes == 0) return false;
    MEMORY_BASIC_INFORMATION_X64 mbi;
    if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD) || (mbi.Protect & PAGE_NOACCESS)) return false;
    UINT64 start = (UINT64)mbi.BaseAddress;
    UINT64 end = start + (UINT64)mbi.RegionSize;
    UINT64 p = (UINT64)ptr;
    if (!(p >= start && p + bytes >= p && p + bytes <= end)) return false;
    DWORD prot = mbi.Protect & 0xFF;
    return prot == PAGE_READWRITE || prot == PAGE_WRITECOPY ||
           prot == PAGE_EXECUTE_READWRITE || prot == PAGE_EXECUTE_WRITECOPY;
}

static const char* rtti_type_name_from_vtable(void* obj, void** vt) {
    if (!obj || !vt || !readable_range(vt, 8)) return 0;
    typedef void* (__fastcall* GetRTTIFn)(void*);
    GetRTTIFn fn = (GetRTTIFn)vt[0];
    if (!readable_range((void*)fn, 1)) return 0;
    void* rtti = fn(obj);
    if (!readable_range(rtti, 0x48)) return 0;
    if (*((BYTE*)rtti + 4) != 4) return 0;
    const char* name = *(const char**)((BYTE*)rtti + 0x40);
    if (!readable_range(name, 1)) return 0;
    return name;
}

static BYTE classify_stream_object(void* obj) {
    if (!obj || !readable_range(obj, 8)) return TYPE_OTHER;
    void** vt = *(void***)obj;
    if (!vt) return TYPE_OTHER;

    UINT64 hash = ((UINT64)vt >> 4) ^ ((UINT64)vt >> 17);
    UINT32 first = (UINT32)(hash & (TYPE_CACHE_SIZE - 1));
    TypeCacheEntry* empty = 0;

    for (UINT32 probe = 0; probe < TYPE_CACHE_PROBES; ++probe) {
        TypeCacheEntry* entry = &g_typeCache[(first + probe) & (TYPE_CACHE_SIZE - 1)];
        void* cachedVtable = entry->vtable;
        if (cachedVtable == vt) {
            ++g_typeCacheHits;
            return entry->kind ? entry->kind : TYPE_OTHER;
        }
        if (!cachedVtable && !empty) empty = entry;
    }

    ++g_rttiLookups;
    BYTE kind = TYPE_OTHER;
    const char* type = rtti_type_name_from_vtable(obj, vt);
    if (type) {
        if (scmp(type, "DSPhysicsVehicleSlopeFrictionResource") == 0) {
            kind = TYPE_SLOPE_RESOURCE;
        } else if (scmp(type, "DSPhysicsVehicleResource") == 0) {
            kind = TYPE_VEHICLE_RESOURCE;
        }
    }

    TypeCacheEntry* target = empty ? empty : &g_typeCache[first];
    target->kind = kind;
    target->vtable = vt;
    return kind;
}

struct RawArray {
    UINT32 count;
    UINT32 capacity;
    void* entries;
};

static bool already_patched(void* resource) {
    for (int i = 0; i < g_patchedResourceCount; ++i) if (g_patchedResources[i] == resource) return true;
    return false;
}
static void remember_patched(void* resource) {
    if (g_patchedResourceCount < 1024) g_patchedResources[g_patchedResourceCount++] = resource;
}
static float f_abs(float v) { return v < 0.0f ? -v : v; }
static float f_max(float a, float b) { return a > b ? a : b; }
static float f_min(float a, float b) { return a < b ? a : b; }

static bool f_near(float value, float expected, float tolerance) {
    return f_abs(value - expected) <= tolerance;
}

static bool table_entry_matches(BYTE* entries, UINT32 index, float slope, float friction) {
    float* item = (float*)(entries + (SIZE_T)index * 8);
    return f_near(item[0], slope, 0.06f) && f_near(item[1], friction, 0.02f);
}

static int identify_exact_slope_table(RawArray* arr) {
    if (!arr || !readable_range(arr, sizeof(RawArray))) return 0;
    if (!arr->entries || arr->capacity < arr->count || arr->capacity > 256) return 0;
    if (!(arr->count == 2 || arr->count == 5 || arr->count == 6)) return 0;
    SIZE_T bytes = (SIZE_T)arr->count * 8;
    if (!readable_range(arr->entries, bytes)) return 0;
    BYTE* e = (BYTE*)arr->entries;

    // Exact tables validated in repeated DS2 Steam 1.10.89.0 test runs.
    if (arr->count == 5 &&
        table_entry_matches(e, 1, 45.0f, 0.30f) &&
        table_entry_matches(e, 2, 40.0f, 0.65f) &&
        table_entry_matches(e, 3, 30.0f, 0.75f) &&
        table_entry_matches(e, 4, 15.0f, 0.80f)) return 1;

    if (arr->count == 6 &&
        table_entry_matches(e, 1, 55.0f, 0.30f) &&
        table_entry_matches(e, 2, 52.0f, 0.60f) &&
        table_entry_matches(e, 3, 50.0f, 0.70f) &&
        table_entry_matches(e, 4, 28.0f, 0.80f) &&
        table_entry_matches(e, 5, 20.0f, 0.90f)) return 2;

    if (arr->count == 6 &&
        table_entry_matches(e, 1, 57.0f, 0.30f) &&
        table_entry_matches(e, 2, 55.0f, 0.50f) &&
        table_entry_matches(e, 3, 45.0f, 0.70f) &&
        table_entry_matches(e, 4, 35.0f, 0.80f)) return 3;

    if (arr->count == 2 &&
        table_entry_matches(e, 1, 40.0f, 0.25f)) return 4;

    return 0;
}

static int patch_slope_resource(void* resource, const char* source) {
    if (!g_enabled || !resource || already_patched(resource)) return 0;
    RawArray* arr = (RawArray*)((BYTE*)resource + 0x28);
    int signature = identify_exact_slope_table(arr);
    if (!signature) return 0;

    SIZE_T bytes = (SIZE_T)arr->count * 8;
    DWORD oldProt = 0;
    BOOL changedProtection = FALSE;
    if (!writable_range(arr->entries, bytes)) {
        changedProtection = VirtualProtect(arr->entries, bytes, PAGE_READWRITE, &oldProt);
        if (!changedProtection) {
            log_line("WARNING: exact slope table is not writable; resource skipped.");
            return 0;
        }
    }

    int changed = 0;
    BYTE* entries = (BYTE*)arr->entries;

    LogBuffer b; lb_init(&b); log_prefix(&b);
    lb_text(&b, "patch exact resource="); lb_hex(&b, (UINT64)resource);
    lb_text(&b, " signature="); lb_uint(&b, signature);
    lb_text(&b, " source="); lb_text(&b, source ? source : "direct");
    lb_text(&b, " entries="); lb_uint(&b, arr->count); lb_text(&b, "\r\n");
    append_log(&b);

    for (UINT32 i = 0; i < arr->count; ++i) {
        float* item = (float*)(entries + (SIZE_T)i * 8);
        float slope = item[0];
        float oldFriction = item[1];
        if (!(slope >= -90.0f && slope <= 90.0f)) continue;
        if (!(oldFriction > 0.0001f && oldFriction < 20.0f)) continue;
        float absSlope = f_abs(slope);
        if (absSlope < g_startSlope) continue;
        float blend = (absSlope - g_startSlope) / (g_fullSlope - g_startSlope);
        blend = f_max(0.0f, f_min(1.0f, blend));
        float factor = 1.0f + (g_gripMultiplier - 1.0f) * blend;
        float floorValue = 1.0f + (g_minFrictionFull - 1.0f) * blend;
        float newFriction = f_max(oldFriction * factor, floorValue);
        newFriction = f_min(newFriction, g_maxFriction);
        if (newFriction <= oldFriction + 0.001f) continue;
        item[1] = newFriction;
        ++changed;
        ++g_totalEntriesChanged;

        LogBuffer e; lb_init(&e); log_prefix(&e);
        lb_text(&e, "  slope["); lb_uint(&e, i); lb_text(&e, "] deg="); lb_float2(&e, slope);
        lb_text(&e, " friction "); lb_float2(&e, oldFriction); lb_text(&e, " -> "); lb_float2(&e, newFriction);
        lb_text(&e, "\r\n"); append_log(&e);
    }

    if (changedProtection) { DWORD dummy = 0; VirtualProtect(arr->entries, bytes, oldProt, &dummy); }
    remember_patched(resource);

    LogBuffer finish; lb_init(&finish); log_prefix(&finish);
    lb_text(&finish, "resource finished changed="); lb_uint(&finish, changed);
    lb_text(&finish, " totalChanged="); lb_uint(&finish, g_totalEntriesChanged); lb_text(&finish, "\r\n"); append_log(&finish);

    if (!g_patchComplete && g_stopAfterChangedEntries > 0 &&
        g_totalEntriesChanged >= g_stopAfterChangedEntries) {
        g_patchComplete = 1;
        log_line("Target entry count reached. Callback switched to fast bypass mode.");
    }
    return changed;
}

static int patch_vehicle_resource(void* vehicle) {
    if (!vehicle) return 0;
    RawArray* refs = (RawArray*)((BYTE*)vehicle + 0x70);
    if (!readable_range(refs, sizeof(RawArray))) return 0;
    if (refs->count < 1 || refs->count > 64 || refs->capacity < refs->count || refs->capacity > 256 || !refs->entries) return 0;
    if (!readable_range(refs->entries, (SIZE_T)refs->count * 8)) return 0;
    void** resources = (void**)refs->entries;
    int total = 0;
    for (UINT32 i = 0; i < refs->count; ++i) total += patch_slope_resource(resources[i], "DSPhysicsVehicleResource+0x70");
    return total;
}

struct StreamingEvents { void** vtable; };
static void __fastcall on_finish_load(StreamingEvents*, const RawArray* objects);
static void __fastcall on_before_unload(StreamingEvents*, const RawArray*) {}
static void __fastcall on_load_asset(StreamingEvents*, const RawArray*) {}
static void* g_listenerVtable[3] = { (void*)&on_finish_load, (void*)&on_before_unload, (void*)&on_load_asset };
static StreamingEvents g_listener = { g_listenerVtable };

static void __fastcall on_finish_load(StreamingEvents*, const RawArray* objects) {
    if (!g_enabled || g_patchComplete) return;

    ++g_callbackGroups;
    if (g_callbackGroups < g_scanStartGroup) return;

    if (g_callbackGroups > g_scanEndGroup) {
        if (!g_scanWindowEndedLogged) {
            g_scanWindowEndedLogged = 1;
            log_line("WARNING: scan window ended before all target tables were found. Adjust ScanStartGroup/ScanEndGroup in the INI.");
        }
        return;
    }

    if (!g_scanWindowLogged) {
        g_scanWindowLogged = 1;
        LogBuffer begin; lb_init(&begin); log_prefix(&begin);
        lb_text(&begin, "Targeted scan window started at group="); lb_uint(&begin, g_callbackGroups);
        lb_text(&begin, " endGroup="); lb_uint(&begin, g_scanEndGroup); lb_text(&begin, "\r\n"); append_log(&begin);
    }

    if (!objects || !readable_range(objects, sizeof(RawArray))) return;
    if (objects->count < 1 || objects->count > 65536 || !objects->entries) return;
    if (!readable_range(objects->entries, (SIZE_T)objects->count * 8)) return;

    g_callbackObjects += objects->count;
    g_windowObjects += objects->count;

    void** entries = (void**)objects->entries;
    int changed = 0;
    for (UINT32 i = 0; i < objects->count; ++i) {
        void* obj = entries[i];
        if (!obj) continue;

        int direct = patch_slope_resource(obj, "ExactSlopeSignature");
        changed += direct;
        if (!direct) changed += patch_vehicle_resource(obj);
        if (g_patchComplete) break;
    }

    if (changed > 0) {
        LogBuffer b; lb_init(&b); log_prefix(&b);
        lb_text(&b, "targeted group patched entries="); lb_uint(&b, changed);
        lb_text(&b, " group="); lb_uint(&b, g_callbackGroups);
        lb_text(&b, " windowObjects="); lb_uint(&b, g_windowObjects);
        lb_text(&b, "\r\n"); append_log(&b);
    }
}

static UINT64 resolve_rip(UINT64 instruction, unsigned int operandOffset) {
    int rel = *(int*)(instruction + operandOffset);
    return instruction + operandOffset + 4 + (INT64)rel;
}

static bool name_is_text(const BYTE* name) {
    return name[0] == '.' && name[1] == 't' && name[2] == 'e' &&
           name[3] == 'x' && name[4] == 't';
}

static UINT64 find_streaming_global(HMODULE exe) {
    BYTE* base = (BYTE*)exe;
    if (!readable_range(base, 0x1000)) return 0;
    UINT32 peOffset = *(UINT32*)(base + 0x3C);
    if (peOffset > 0x1000 || !readable_range(base + peOffset, 0x108)) return 0;
    BYTE* nt = base + peOffset;
    if (*(UINT32*)nt != 0x00004550) return 0; // PE\0\0
    WORD sectionCount = *(WORD*)(nt + 6);
    WORD optionalSize = *(WORD*)(nt + 20);
    if (sectionCount < 1 || sectionCount > 96 || optionalSize < 0x70 || optionalSize > 0x400) return 0;
    BYTE* section = nt + 24 + optionalSize;
    BYTE* textStart = 0;
    UINT32 textSize = 0;
    for (WORD i = 0; i < sectionCount; ++i, section += 40) {
        if (!readable_range(section, 40)) return 0;
        if (name_is_text(section)) {
            UINT32 virtualSize = *(UINT32*)(section + 8);
            UINT32 virtualAddress = *(UINT32*)(section + 12);
            if (virtualSize < 0x1000 || virtualSize > 0x40000000) return 0;
            textStart = base + virtualAddress;
            textSize = virtualSize;
            break;
        }
    }
    if (!textStart || !textSize || !readable_range(textStart, textSize)) return 0;

    static const BYTE sig[28] = {
        0x48,0x89,0x05,0,0,0,0,0xE8,0,0,0,0,0x33,0xD2,0x41,0xB8,
        0xF8,0x0A,0x00,0x00,0x48,0x8B,0xC8,0x48,0x8B,0xD8,0xE8,0
    };
    static const BYTE mask[28] = {
        1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0
    };
    for (UINT32 i = 0; i + 28 <= textSize; ++i) {
        bool ok = true;
        for (int j = 0; j < 28; ++j) {
            if (mask[j] && textStart[i + j] != sig[j]) { ok = false; break; }
        }
        if (ok) return resolve_rip((UINT64)(textStart + i), 3);
    }
    return 0;
}

static bool register_listener(void* manager) {
    if (!manager || !readable_range((BYTE*)manager + 0x578, 8)) return false;
    void* streamSys = *(void**)((BYTE*)manager + 0x578);
    if (!streamSys || !readable_range(streamSys, 8)) return false;
    void** vt = *(void***)streamSys;
    if (!vt || !readable_range(vt, 5 * 8)) return false;
    typedef void (__fastcall* AddListenerFn)(void*, void*);
    AddListenerFn add = (AddListenerFn)vt[3];
    if (!add || !readable_range((void*)add, 1)) return false;
    add(streamSys, &g_listener);
    g_listenerRegistered = 1;
    return true;
}

static DWORD __stdcall worker(LPVOID) {
    make_paths(); read_config();
    log_line("Pickup Truck Hill Assist v1.2 LATE TARGETED loaded (exact signatures, delayed scan window).");
    if (!g_enabled) { log_line("Disabled in INI."); return 0; }
    LogBuffer cfg; lb_init(&cfg); log_prefix(&cfg);
    lb_text(&cfg, "Config: grip="); lb_float2(&cfg, g_gripMultiplier);
    lb_text(&cfg, " minFriction="); lb_float2(&cfg, g_minFrictionFull);
    lb_text(&cfg, " startSlope="); lb_float2(&cfg, g_startSlope);
    lb_text(&cfg, " fullSlope="); lb_float2(&cfg, g_fullSlope);
    lb_text(&cfg, " maxFriction="); lb_float2(&cfg, g_maxFriction);
    lb_text(&cfg, " stopAfterEntries="); lb_uint(&cfg, g_stopAfterChangedEntries);
    lb_text(&cfg, " scanStartGroup="); lb_uint(&cfg, g_scanStartGroup);
    lb_text(&cfg, " scanEndGroup="); lb_uint(&cfg, g_scanEndGroup);
    lb_text(&cfg, "\r\n"); append_log(&cfg);

    HMODULE exe = GetModuleHandleW(0);
    if (!exe) { log_line("ERROR: DS2.exe module not found."); return 0; }
    UINT64 globalAddr = find_streaming_global(exe);
    if (!globalAddr || !readable_range((void*)globalAddr, 8)) {
        log_line("ERROR: StreamingManager signature not found for this game build.");
        return 0;
    }

    LogBuffer s; lb_init(&s); log_prefix(&s); lb_text(&s, "StreamingManager global="); lb_hex(&s, globalAddr); lb_text(&s, "\r\n"); append_log(&s);
    void** managerGlobal = (void**)globalAddr;
    void* manager = 0;
    for (int i = 0; i < 1200; ++i) {
        if (readable_range(managerGlobal, 8)) manager = *managerGlobal;
        if (manager) break;
        Sleep(50);
    }
    if (!manager) { log_line("ERROR: StreamingManager was not created within 60 seconds."); return 0; }
    if (!register_listener(manager)) { log_line("ERROR: Could not register streaming listener."); return 0; }
    log_line("Streaming listener registered. Early groups are bypassed until the targeted scan window.");
    return 0;
}

extern "C" BOOL __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        // version.dll can also be loaded by DS2 helper executables. Only the
        // actual DS2.exe process may own the listener and the instance mutex.
        if (!is_main_game_process()) return TRUE;
        g_module = (HMODULE)instance;
        DisableThreadLibraryCalls(g_module);
        g_mutex = CreateMutexW(0, FALSE, L"Local\\DS2_PickupTruckHillAssist_v1_2");
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
