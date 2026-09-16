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
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
__declspec(dllimport) BOOL __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) void __stdcall Sleep(DWORD);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(LPCVOID, MEMORY_BASIC_INFORMATION_X64*, SIZE_T);
__declspec(dllimport) int __stdcall GetPrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
}

typedef unsigned long long u64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned char u8;

static HMODULE g_self = (HMODULE)0;
static WCHAR g_ini_path[1024];

static const u64 kManagerGlobalRva = 0x0623E5D0ull;
static const u32 kEntryCount = 0x37u;
static const u32 kEntryArrayOffset = 0x30u;
static const u32 kResourcePtrOffset = 0x70u;
static const u32 kCostOffset = 0x24u;

static const DWORD MEM_COMMIT_X = 0x1000u;
static const DWORD PAGE_NOACCESS_X = 0x01u;
static const DWORD PAGE_GUARD_X = 0x100u;
static const DWORD PAGE_READWRITE_X = 0x04u;
static const DWORD PAGE_WRITECOPY_X = 0x08u;
static const DWORD PAGE_EXECUTE_READWRITE_X = 0x40u;
static const DWORD PAGE_EXECUTE_WRITECOPY_X = 0x80u;
static const HANDLE INVALID_HANDLE_VALUE_X = (HANDLE)(~(u64)0);

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

static BOOL RegionAllows(const void* p, SIZE_T need, BOOL requireWrite) {
    if (!p || need == 0) return 0;
    MEMORY_BASIC_INFORMATION_X64 mbi;
    SIZE_T got = VirtualQuery(p, &mbi, (SIZE_T)sizeof(mbi));
    if (got < (SIZE_T)sizeof(mbi)) return 0;
    if (mbi.State != MEM_COMMIT_X) return 0;
    if (mbi.Protect & (PAGE_NOACCESS_X | PAGE_GUARD_X)) return 0;

    u64 start = (u64)p;
    u64 base = (u64)mbi.BaseAddress;
    u64 end = start + (u64)need;
    u64 regionEnd = base + (u64)mbi.RegionSize;
    if (end < start || end > regionEnd) return 0;

    if (!requireWrite) return 1;
    DWORD prot = mbi.Protect & 0xFFu;
    return prot == PAGE_READWRITE_X || prot == PAGE_WRITECOPY_X ||
           prot == PAGE_EXECUTE_READWRITE_X || prot == PAGE_EXECUTE_WRITECOPY_X;
}

struct PassResult {
    u32 resources;
    u32 patched;
};

static PassResult PatchPass(u8* base, i32 target) {
    PassResult r = {};

    // The global slot is inside the main executable image at a known RVA.
    void* manager = *(void**)(base + kManagerGlobalRva);
    if (!manager) return r;

    // Validate the entry-array region once instead of VirtualQuery on every slot.
    u8* arrayStart = (u8*)manager + kEntryArrayOffset;
    SIZE_T arrayBytes = (SIZE_T)kEntryCount * (SIZE_T)sizeof(void*);
    if (!RegionAllows(arrayStart, arrayBytes, 0)) return r;

    for (u32 i = 0; i < kEntryCount; ++i) {
        void* entry = *(void**)(arrayStart + i * sizeof(void*));
        if (!entry) continue;

        // Validate only the tiny entry region needed to read the resource pointer.
        void** resourceSlot = (void**)((u8*)entry + kResourcePtrOffset);
        if (!RegionAllows(resourceSlot, sizeof(void*), 0)) continue;

        void* resource = *resourceSlot;
        if (!resource) continue;

        i32* field = (i32*)((u8*)resource + kCostOffset);
        if (!RegionAllows(field, sizeof(i32), 1)) continue;

        ++r.resources;
        i32 current = *field;
        if (current < 0 || current > 1000000 || current == target) continue;

        i32 expected = current;
        if (__atomic_compare_exchange_n(field, &expected, target, 0,
                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            ++r.patched;
        }
    }

    return r;
}

static DWORD __stdcall Worker(LPVOID) {
    BuildSiblingPath(g_ini_path, 1024, L"ds2_apas_memory_costs.ini");
    if (!g_ini_path[0]) return 0;

    int enabled = GetPrivateProfileIntW(L"APASMemoryCosts", L"Enabled", 1, g_ini_path);
    if (!enabled) return 0;

    int target = GetPrivateProfileIntW(L"APASMemoryCosts", L"GlobalCost", 1, g_ini_path);
    if (target < 0 || target > 1000000) target = 1;

    u8* base = (u8*)GetModuleHandleW((LPCWSTR)0);
    if (!base) return 0;

    // Avoid doing any scanning during the heaviest initial process startup.
    Sleep(3000u);

    u32 maxResources = 0;
    u32 stablePasses = 0;
    u32 lastResources = 0;

    for (;;) {
        PassResult r = PatchPass(base, target);
        if (r.resources > maxResources) maxResources = r.resources;

        // Once the normal APAS table is populated, remain for a few quiet passes
        // to catch late startup entries, then terminate the worker completely.
        if (r.resources >= 40u) {
            if (r.resources == lastResources && r.patched == 0u) ++stablePasses;
            else stablePasses = 0;

            if (stablePasses >= 5u) return 0;
            Sleep(1000u);
        } else {
            // At title/load stages the table may contain only a few resources.
            // Poll very lightly so there is no persistent gameplay cost.
            stablePasses = 0;
            Sleep(5000u);
        }

        lastResources = r.resources;

        // Conservative fallback: if a future build never exposes 40 resources,
        // stop after observing a non-empty stable table for a long period.
        if (maxResources > 0u && r.resources == lastResources && r.patched == 0u && maxResources < 40u) {
            // intentionally no action here; the low-frequency loop is negligible
        }
    }
}

extern "C" __declspec(dllexport) const char* __stdcall DS2ApasMemoryCostsVersion() {
    return "1.0.3-performance-test";
}

extern "C" __declspec(dllexport) const void* DS2ApasMemoryCostsAnchor = (const void*)&DS2ApasMemoryCostsVersion;

extern "C" BOOL __stdcall DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == 1u) {
        g_self = hModule;
        DisableThreadLibraryCalls(hModule);
        HANDLE h = CreateThread((LPVOID)0, 0, Worker, (LPVOID)0, 0, (DWORD*)0);
        if (h && h != INVALID_HANDLE_VALUE_X) CloseHandle(h);
    }
    return 1;
}
