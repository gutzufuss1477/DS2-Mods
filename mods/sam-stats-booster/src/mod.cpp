extern "C" void* get_peb();
extern "C" void SamStatsU32Hook();
extern "C" void SamStatsQ64Hook();
extern "C" void SamStatsDoubleHook();
extern "C" int _fltused = 0;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i32 = int;
using usize = unsigned long long;
using BOOL = int;
using DWORD = unsigned long;
using HANDLE = void*;
using HMODULE = void*;

extern "C" {
__declspec(dllexport) void* g_u32Continue = nullptr;
__declspec(dllexport) void* g_q64Continue = nullptr;
__declspec(dllexport) void* g_doubleContinue = nullptr;
__declspec(dllexport) void* g_managerSlot = nullptr;
__declspec(dllexport) u32 g_enabledMask = 0x00000FFFu;
__declspec(dllexport) double g_multiplier = 5.0;
__declspec(dllexport) double g_half = 0.5;
__declspec(dllexport) double g_doubleCap = 999999999999.99;
__declspec(dllexport) __declspec(align(8)) volatile u64 g_eventCounts[13] = {};
}

using U32Getter_t = u32(*)(void*, u16);
using U32Mutator_t = void(*)(void*, u16, u32);
using Q64Getter_t = u64(*)(void*, u16);
using Q64Mutator_t = void(*)(void*, u16, u64, u64);
using DoubleGetter_t = double(*)(void*, u16);
using DoubleMutator_t = void(*)(void*, u16, double, double);
static U32Getter_t pU32Getter = nullptr;
static U32Mutator_t pU32Mutator = nullptr;
static Q64Getter_t pQ64Getter = nullptr;
static Q64Mutator_t pQ64Mutator = nullptr;
static DoubleGetter_t pDoubleGetter = nullptr;
static DoubleMutator_t pDoubleMutator = nullptr;

// Runtime diagnostics for integer-backed Sam Stats. Indices match the
// public 12-stat order plus the disabled internal StealthTakedown skill.
static volatile u64 g_integerRawSum[13] = {};
static volatile u64 g_integerRequestedSum[13] = {};
static volatile u64 g_integerStoredSum[13] = {};
static volatile u64 g_integerLastRaw[13] = {};
static volatile u64 g_integerLastRequested[13] = {};
static volatile u64 g_integerLastStored[13] = {};
static volatile u64 g_integerLastBefore[13] = {};
static volatile u64 g_integerLastAfter[13] = {};
static volatile u64 g_integerWriteCalls[13] = {};

static int u32_index_from_record(u32 recordId) {
    if (recordId == 0x4Bu) return 5;   // Strand Skills
    if (recordId == 0x3Fu) return 6;   // Melee Combat
    if (recordId == 0x49u) return 7;   // Handguns
    if (recordId == 0x43u) return 8;   // Assault Rifles
    if (recordId == 0x47u) return 9;   // Sniper Rifles
    if (recordId == 0x41u) return 10;  // Machine Guns
    if (recordId == 0x45u) return 11;  // Shotguns
    if (recordId == 0x0Bu) return 12;  // Internal Stealth Takedown
    return -1;
}

static u64 scale_integer_gain(u64 raw, u64 cap) {
    if (!raw) return 0;
    double scaled = (double)raw * g_multiplier + 0.5;
    if (scaled >= (double)cap) return cap;
    return (u64)scaled;
}

static void record_integer_write(u32 index, u64 raw, u64 requested, u64 before, u64 after) {
    const u64 stored = after >= before ? (after - before) : 0u;
    __atomic_fetch_add((u64*)&g_integerRawSum[index], raw, __ATOMIC_RELAXED);
    __atomic_fetch_add((u64*)&g_integerRequestedSum[index], requested, __ATOMIC_RELAXED);
    __atomic_fetch_add((u64*)&g_integerStoredSum[index], stored, __ATOMIC_RELAXED);
    g_integerLastRaw[index] = raw;
    g_integerLastRequested[index] = requested;
    g_integerLastStored[index] = stored;
    g_integerLastBefore[index] = before;
    g_integerLastAfter[index] = after;
    __atomic_fetch_add((u64*)&g_integerWriteCalls[index], 1u, __ATOMIC_RELAXED);
}

extern "C" __declspec(dllexport) void ProcessSamStatsU32(void* manager, u32 recordId, u32 rawDelta) {
    if (!pU32Mutator) return;
    const int index = u32_index_from_record(recordId);
    if (index < 0 || !manager || !pU32Getter) {
        pU32Mutator(manager, (u16)recordId, rawDelta);
        return;
    }

    if ((i32)rawDelta <= 0) {
        pU32Mutator(manager, (u16)recordId, rawDelta);
        return;
    }

    __atomic_fetch_add((u64*)&g_eventCounts[index], 1u, __ATOMIC_RELAXED);
    u32 requested = rawDelta;
    if ((g_enabledMask & (1u << (u32)index)) != 0u)
        requested = (u32)scale_integer_gain(rawDelta, 999999999u);

    const u32 before = pU32Getter(manager, (u16)recordId);
    pU32Mutator(manager, (u16)recordId, requested);
    const u32 after = pU32Getter(manager, (u16)recordId);
    record_integer_write((u32)index, rawDelta, requested, before, after);
}

extern "C" __declspec(dllexport) void ProcessSamStatsQ64(void* manager, u32 recordId,
                                                             u64 rawDelta, u64 cap) {
    if (!pQ64Mutator) return;
    const u32 index = 4u; // Lung Capacity is the only Sam Stats Q64 record.
    if (!manager || !pQ64Getter || recordId != 6u) {
        pQ64Mutator(manager, (u16)recordId, rawDelta, cap);
        return;
    }

    if ((long long)rawDelta <= 0) {
        pQ64Mutator(manager, (u16)recordId, rawDelta, cap);
        return;
    }

    __atomic_fetch_add((u64*)&g_eventCounts[index], 1u, __ATOMIC_RELAXED);
    u64 requested = rawDelta;
    if ((g_enabledMask & (1u << index)) != 0u)
        requested = scale_integer_gain(rawDelta, cap);

    const u64 before = pQ64Getter(manager, (u16)recordId);
    pQ64Mutator(manager, (u16)recordId, requested, cap);
    const u64 after = pQ64Getter(manager, (u16)recordId);
    record_integer_write(index, rawDelta, requested, before, after);
}

// Runtime diagnostics for the four physical Sam Stats. These values prove
// whether the boosted delta reaches DS2's encrypted play-record storage.
static volatile double g_physicalRawSum[4] = {};
static volatile double g_physicalRequestedSum[4] = {};
static volatile double g_physicalStoredSum[4] = {};
static volatile double g_physicalLastRaw[4] = {};
static volatile double g_physicalLastRequested[4] = {};
static volatile double g_physicalLastStored[4] = {};
static volatile double g_physicalLastBefore[4] = {};
static volatile double g_physicalLastAfter[4] = {};
static volatile u64 g_physicalWriteCalls[4] = {};

static int physical_index_from_record(u32 recordId) {
    if (recordId == 8u) return 0;   // Terrain Traversal
    if (recordId == 10u) return 1;  // Carrying Capacity
    if (recordId == 14u) return 2;  // Endurance
    if (recordId == 12u) return 3;  // KO Resistance
    return -1;
}

extern "C" __declspec(dllexport) void ProcessSamStatsDouble(void* manager, u32 recordId,
                                                               double rawDelta, double cap) {
    if (!pDoubleMutator) return;
    const int index = physical_index_from_record(recordId);
    if (index < 0 || !manager || !pDoubleGetter) {
        pDoubleMutator(manager, (u16)recordId, rawDelta, cap);
        return;
    }

    double requested = rawDelta;
    if (rawDelta > 0.0) {
        __atomic_fetch_add((u64*)&g_eventCounts[index], 1u, __ATOMIC_RELAXED);
        const u32 mask = g_enabledMask;
        if ((mask & (1u << (u32)index)) != 0u) {
            requested = rawDelta * g_multiplier;
            if (requested > cap) requested = cap;
        }
    }

    const double before = pDoubleGetter(manager, (u16)recordId);
    pDoubleMutator(manager, (u16)recordId, requested, cap);
    const double after = pDoubleGetter(manager, (u16)recordId);
    const double stored = after - before;

    g_physicalRawSum[index] = g_physicalRawSum[index] + rawDelta;
    g_physicalRequestedSum[index] = g_physicalRequestedSum[index] + requested;
    g_physicalStoredSum[index] = g_physicalStoredSum[index] + stored;
    g_physicalLastRaw[index] = rawDelta;
    g_physicalLastRequested[index] = requested;
    g_physicalLastStored[index] = stored;
    g_physicalLastBefore[index] = before;
    g_physicalLastAfter[index] = after;
    __atomic_fetch_add((u64*)&g_physicalWriteCalls[index], 1u, __ATOMIC_RELAXED);
}

static int g_enabled = 1;
static volatile u32 g_stopWorker = 0;
static u32 g_workerStarted = 0;
static u32 g_status = 0;
static u32 g_error = 0;
static u32 g_hookFlags = 0;
static u32 g_resolutionFlags = 0;
static u32 g_runtimeTimestamp = 0;
static u32 g_runtimeImageSize = 0;
static u32 g_u32Rva = 0;
static u32 g_q64Rva = 0;
static u32 g_doubleRva = 0;
static char g_iniBuf[8192];
static char g_iniPath[1024];
static char g_logPath[1024];

struct LIST_ENTRY { LIST_ENTRY* Flink; LIST_ENTRY* Blink; };
struct UNICODE_STRING { u16 Length; u16 MaximumLength; wchar_t* Buffer; };

static u8 lower8(u8 c) { return (c >= 'A' && c <= 'Z') ? (u8)(c + 32) : c; }
static wchar_t lowerw(wchar_t c) { return (c >= L'A' && c <= L'Z') ? (wchar_t)(c + 32) : c; }

static bool wends_eq(const wchar_t* s, u16 byteLen, const wchar_t* lit) {
    if (!s || !lit) return false;
    usize n = byteLen / 2;
    usize m = 0;
    while (lit[m]) ++m;
    if (n != m) return false;
    for (usize i = 0; i < n; ++i) if (lowerw(s[i]) != lowerw(lit[i])) return false;
    return true;
}

static void* find_module(const wchar_t* name) {
    u8* peb = (u8*)get_peb();
    if (!peb) return nullptr;
    u8* ldr = *(u8**)(peb + 0x18);
    if (!ldr) return nullptr;
    LIST_ENTRY* head = (LIST_ENTRY*)(ldr + 0x20);
    for (LIST_ENTRY* n = head->Flink; n && n != head; n = n->Flink) {
        u8* e = (u8*)n - 0x10;
        UNICODE_STRING* baseName = (UNICODE_STRING*)(e + 0x58);
        if (wends_eq(baseName->Buffer, baseName->Length, name)) return *(void**)(e + 0x30);
    }
    return nullptr;
}

static int streqi(const char* a, const char* b) {
    while (*a && *b) {
        if (lower8((u8)*a) != lower8((u8)*b)) return 0;
        ++a;
        ++b;
    }
    return *a == *b;
}

static void* resolve_export_recursive(void* module, const char* wanted, int depth) {
    if (!module || !wanted || depth > 4) return nullptr;
    u8* b = (u8*)module;
    u32 peoff = *(u32*)(b + 0x3C);
    u8* nt = b + peoff;
    if (*(u32*)nt != 0x00004550u) return nullptr;
    u32 expRva = *(u32*)(nt + 0x88);
    u32 expSize = *(u32*)(nt + 0x8C);
    if (!expRva) return nullptr;
    u8* e = b + expRva;
    u32 nNames = *(u32*)(e + 0x18);
    u32 funcsRva = *(u32*)(e + 0x1C);
    u32 namesRva = *(u32*)(e + 0x20);
    u32 ordRva = *(u32*)(e + 0x24);
    u32* names = (u32*)(b + namesRva);
    u16* ords = (u16*)(b + ordRva);
    u32* funcs = (u32*)(b + funcsRva);
    for (u32 i = 0; i < nNames; ++i) {
        const char* nm = (const char*)(b + names[i]);
        if (!streqi(nm, wanted)) continue;
        u32 rva = funcs[ords[i]];
        if (!(rva >= expRva && rva < expRva + expSize)) return b + rva;

        const char* fw = (const char*)(b + rva);
        char mod[96];
        char fn[128];
        usize mi = 0, fi = 0;
        while (*fw && *fw != '.' && mi < 88) mod[mi++] = *fw++;
        if (*fw != '.') return nullptr;
        ++fw;
        while (*fw && fi < 120) fn[fi++] = *fw++;
        mod[mi] = 0;
        fn[fi] = 0;
        if (fn[0] == '#') return nullptr;
        const char dll[] = ".dll";
        if (mi + 4 < 95) for (int j = 0; j < 5; ++j) mod[mi + j] = dll[j];
        wchar_t wmod[96];
        usize wn = 0;
        while (mod[wn] && wn < 95) {
            wmod[wn] = (wchar_t)(u8)mod[wn];
            ++wn;
        }
        wmod[wn] = 0;
        void* next = find_module(wmod);
        if (!next) next = find_module(L"kernelbase.dll");
        return resolve_export_recursive(next, fn, depth + 1);
    }
    return nullptr;
}

static void* resolve_export(void* module, const char* wanted) {
    return resolve_export_recursive(module, wanted, 0);
}

using VirtualProtect_t = BOOL(*)(void*, usize, DWORD, DWORD*);
using FlushInstructionCache_t = BOOL(*)(HANDLE, const void*, usize);
using GetModuleFileNameA_t = DWORD(*)(HMODULE, char*, DWORD);
using CreateFileA_t = HANDLE(*)(const char*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
using ReadFile_t = BOOL(*)(HANDLE, void*, DWORD, DWORD*, void*);
using WriteFile_t = BOOL(*)(HANDLE, const void*, DWORD, DWORD*, void*);
using CloseHandle_t = BOOL(*)(HANDLE);
using ThreadProc_t = DWORD(*)(void*);
using CreateThread_t = HANDLE(*)(void*, usize, ThreadProc_t, void*, DWORD, DWORD*);
using Sleep_t = void(*)(DWORD);

static VirtualProtect_t pVirtualProtect;
static FlushInstructionCache_t pFlushInstructionCache;
static GetModuleFileNameA_t pGetModuleFileNameA;
static CreateFileA_t pCreateFileA;
static ReadFile_t pReadFile;
static WriteFile_t pWriteFile;
static CloseHandle_t pCloseHandle;
static CreateThread_t pCreateThread;
static Sleep_t pSleep;

static bool resolve_api() {
    void* k32 = find_module(L"kernel32.dll");
    void* kb = find_module(L"kernelbase.dll");
    void* root = k32 ? k32 : kb;
    if (!root) return false;
    pVirtualProtect = (VirtualProtect_t)resolve_export(root, "VirtualProtect");
    pFlushInstructionCache = (FlushInstructionCache_t)resolve_export(root, "FlushInstructionCache");
    pGetModuleFileNameA = (GetModuleFileNameA_t)resolve_export(root, "GetModuleFileNameA");
    pCreateFileA = (CreateFileA_t)resolve_export(root, "CreateFileA");
    pReadFile = (ReadFile_t)resolve_export(root, "ReadFile");
    pWriteFile = (WriteFile_t)resolve_export(root, "WriteFile");
    pCloseHandle = (CloseHandle_t)resolve_export(root, "CloseHandle");
    pCreateThread = (CreateThread_t)resolve_export(root, "CreateThread");
    pSleep = (Sleep_t)resolve_export(root, "Sleep");
    return pVirtualProtect && pFlushInstructionCache && pGetModuleFileNameA &&
           pCreateFileA && pReadFile && pWriteFile && pCloseHandle && pCreateThread && pSleep;
}

static char* trim(char* s) {
    while (*s == ' ' || *s == '\t' || *s == '\r') ++s;
    char* e = s;
    while (*e && *e != '\n') ++e;
    while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r')) --e;
    *e = 0;
    return s;
}

static char* find_char(char* s, char c) {
    while (*s) {
        if (*s == c) return s;
        ++s;
    }
    return nullptr;
}

static int parse_bool(const char* s, int defv) {
    if (streqi(s, "1") || streqi(s, "true") || streqi(s, "yes") || streqi(s, "on")) return 1;
    if (streqi(s, "0") || streqi(s, "false") || streqi(s, "no") || streqi(s, "off")) return 0;
    return defv;
}

static double parse_double(const char* s, double defv) {
    while (*s == ' ' || *s == '\t') ++s;
    double v = 0.0;
    int any = 0;
    while (*s >= '0' && *s <= '9') {
        any = 1;
        v = v * 10.0 + (double)(*s - '0');
        ++s;
    }
    if (*s == '.' || *s == ',') {
        ++s;
        double place = 0.1;
        while (*s >= '0' && *s <= '9') {
            any = 1;
            v += (double)(*s - '0') * place;
            place *= 0.1;
            ++s;
        }
    }
    if (!any || v < 0.0 || v > 1000.0) return defv;
    return v;
}

static void set_skill_bit(u32 bit, int on) {
    if (on) g_enabledMask |= (1u << bit);
    else g_enabledMask &= ~(1u << bit);
}

static void parse_ini(char* buf) {
    char* p = buf;
    while (*p) {
        char* line = p;
        while (*p && *p != '\n') ++p;
        if (*p == '\n') {
            *p = 0;
            ++p;
        }
        line = trim(line);
        if (!*line || *line == ';' || *line == '#' || *line == '[') continue;
        char* eq = find_char(line, '=');
        if (!eq) continue;
        *eq = 0;
        char* key = trim(line);
        char* val = trim(eq + 1);
        if (streqi(key, "Enabled")) g_enabled = parse_bool(val, g_enabled);
        else if (streqi(key, "Multiplier")) g_multiplier = parse_double(val, g_multiplier);
        else if (streqi(key, "TerrainTraversal")) set_skill_bit(0, parse_bool(val, 1));
        else if (streqi(key, "CarryingCapacity")) set_skill_bit(1, parse_bool(val, 1));
        else if (streqi(key, "Endurance")) set_skill_bit(2, parse_bool(val, 1));
        else if (streqi(key, "KOResistance")) set_skill_bit(3, parse_bool(val, 1));
        else if (streqi(key, "LungCapacity")) set_skill_bit(4, parse_bool(val, 1));
        else if (streqi(key, "StrandSkills")) set_skill_bit(5, parse_bool(val, 1));
        else if (streqi(key, "MeleeCombat")) set_skill_bit(6, parse_bool(val, 1));
        else if (streqi(key, "Handguns")) set_skill_bit(7, parse_bool(val, 1));
        else if (streqi(key, "AssaultRifles")) set_skill_bit(8, parse_bool(val, 1));
        else if (streqi(key, "SniperRifles")) set_skill_bit(9, parse_bool(val, 1));
        else if (streqi(key, "MachineGuns")) set_skill_bit(10, parse_bool(val, 1));
        else if (streqi(key, "Shotguns")) set_skill_bit(11, parse_bool(val, 1));
        else if (streqi(key, "StealthTakedown")) set_skill_bit(12, parse_bool(val, 0));
    }
    if (g_multiplier < 1.0) g_multiplier = 1.0;
    if (g_multiplier > 100.0) g_multiplier = 100.0;
}

static bool make_sibling_path(HMODULE self, const char* ext, char* out, usize cap) {
    if (!pGetModuleFileNameA || !out || cap < 8) return false;
    DWORD got = pGetModuleFileNameA(self, out, (DWORD)(cap - 1));
    if (!got || got >= cap - 1) return false;
    out[got] = 0;
    int len = (int)got;
    int dot = -1;
    for (int i = len - 1; i >= 0; --i) {
        if (out[i] == '.') {
            dot = i;
            break;
        }
        if (out[i] == '\\' || out[i] == '/') break;
    }
    if (dot < 0) dot = len;
    usize e = 0;
    while (ext[e]) ++e;
    if ((usize)dot + e + 1 >= cap) return false;
    for (usize i = 0; i <= e; ++i) out[dot + i] = ext[i];
    return true;
}

static void load_ini() {
    if (!pCreateFileA || !pReadFile || !pCloseHandle || !g_iniPath[0]) return;
    HANDLE h = pCreateFileA(g_iniPath, 0x80000000u, 1u, nullptr, 3u, 0x80u, nullptr);
    if ((usize)h == ~(usize)0) return;
    DWORD got = 0;
    if (pReadFile(h, g_iniBuf, 8191, &got, nullptr) && got < 8192) {
        g_iniBuf[got] = 0;
        parse_ini(g_iniBuf);
    }
    pCloseHandle(h);
}

static char g_logBufferStorage[32768];

struct LogBuffer {
    char* data;
    usize pos;
    usize cap;
};

static void log_char(LogBuffer& b, char c) {
    if (b.pos + 1 < b.cap) b.data[b.pos++] = c;
}

static void log_text(LogBuffer& b, const char* s) {
    if (!s) return;
    while (*s && b.pos + 1 < b.cap) b.data[b.pos++] = *s++;
}

static void log_u64(LogBuffer& b, u64 v) {
    char tmp[32];
    int n = 0;
    if (!v) {
        log_char(b, '0');
        return;
    }
    while (v && n < 31) {
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (n) log_char(b, tmp[--n]);
}

static void log_hex32(LogBuffer& b, u32 v) {
    const char* h = "0123456789ABCDEF";
    log_text(b, "0x");
    for (int shift = 28; shift >= 0; shift -= 4) log_char(b, h[(v >> shift) & 0xF]);
}

static void log_fixed2(LogBuffer& b, double v) {
    if (v < 0.0) {
        log_char(b, '-');
        v = -v;
    }
    u64 scaled = (u64)(v * 100.0 + 0.5);
    log_u64(b, scaled / 100);
    log_char(b, '.');
    u32 f = (u32)(scaled % 100);
    log_char(b, (char)('0' + (f / 10)));
    log_char(b, (char)('0' + (f % 10)));
}

static void log_fixed6(LogBuffer& b, double v) {
    if (v < 0.0) {
        log_char(b, '-');
        v = -v;
    }
    // All relevant native values are below 1e12, so six decimals remain
    // representable in u64 and are sufficient to expose tiny frame deltas.
    u64 scaled = (u64)(v * 1000000.0 + 0.5);
    log_u64(b, scaled / 1000000u);
    log_char(b, '.');
    u32 f = (u32)(scaled % 1000000u);
    u32 div = 100000u;
    for (int i = 0; i < 6; ++i) {
        log_char(b, (char)('0' + ((f / div) % 10u)));
        div /= 10u;
    }
}

static const char* status_text() {
    if (g_status == 100) return "ACTIVE";
    if (g_status == 101) return "DISABLED_BY_INI";
    if (g_status == 102) return "ACTIVE_WITHOUT_PERIODIC_LOG_THREAD";
    return "INACTIVE";
}

static const char* error_text() {
    switch (g_error) {
        case 0: return "none";
        case 1: return "Windows API resolution failed";
        case 2: return "DS2.exe module not found";
        case 3: return "Unsupported DS2.exe build header";
        case 10: return "U32 skill producer signature not unique";
        case 11: return "Q64 lung producer signature not unique";
        case 12: return "Double physical-skill producer signature not unique";
        case 20: return "U32 patch-site validation failed";
        case 21: return "Q64 patch-site validation failed";
        case 22: return "Double patch-site validation failed";
        case 23: return "Decoded manager slot is invalid";
        case 24: return "Decoded double cap is invalid";
        case 25: return "Double getter validation failed";
        case 26: return "Double mutator validation failed";
        case 27: return "U32 getter validation failed";
        case 28: return "U32 mutator validation failed";
        case 29: return "Q64 getter or mutator validation failed";
        case 30: return "Failed to install U32 hook";
        case 31: return "Failed to install Q64 hook";
        case 32: return "Failed to install double hook";
        default: return "unknown";
    }
}

static void append_counter(LogBuffer& b, const char* name, u32 index) {
    log_text(b, "  ");
    log_text(b, name);
    log_text(b, ": ");
    log_u64(b, g_eventCounts[index]);
    log_char(b, '\n');
}

static void append_physical_verification(LogBuffer& b, const char* name, u32 index) {
    log_text(b, "  ");
    log_text(b, name);
    log_text(b, ":\n");
    log_text(b, "    Native write calls: ");
    log_u64(b, g_physicalWriteCalls[index]);
    log_char(b, '\n');
    log_text(b, "    Raw gain sum: ");
    log_fixed6(b, g_physicalRawSum[index]);
    log_char(b, '\n');
    log_text(b, "    Requested boosted sum: ");
    log_fixed6(b, g_physicalRequestedSum[index]);
    log_char(b, '\n');
    log_text(b, "    Actual stored gain sum: ");
    log_fixed6(b, g_physicalStoredSum[index]);
    log_char(b, '\n');
    log_text(b, "    Last raw -> requested -> stored: ");
    log_fixed6(b, g_physicalLastRaw[index]);
    log_text(b, " -> ");
    log_fixed6(b, g_physicalLastRequested[index]);
    log_text(b, " -> ");
    log_fixed6(b, g_physicalLastStored[index]);
    log_char(b, '\n');
    log_text(b, "    Last total before -> after: ");
    log_fixed6(b, g_physicalLastBefore[index]);
    log_text(b, " -> ");
    log_fixed6(b, g_physicalLastAfter[index]);
    log_char(b, '\n');
}

static void append_integer_verification(LogBuffer& b, const char* name, u32 index) {
    log_text(b, "  ");
    log_text(b, name);
    log_text(b, ":\n");
    log_text(b, "    Native write calls: ");
    log_u64(b, g_integerWriteCalls[index]);
    log_char(b, '\n');
    log_text(b, "    Raw gain sum: ");
    log_u64(b, g_integerRawSum[index]);
    log_char(b, '\n');
    log_text(b, "    Requested boosted sum: ");
    log_u64(b, g_integerRequestedSum[index]);
    log_char(b, '\n');
    log_text(b, "    Actual stored gain sum: ");
    log_u64(b, g_integerStoredSum[index]);
    log_char(b, '\n');
    log_text(b, "    Last raw -> requested -> stored: ");
    log_u64(b, g_integerLastRaw[index]);
    log_text(b, " -> ");
    log_u64(b, g_integerLastRequested[index]);
    log_text(b, " -> ");
    log_u64(b, g_integerLastStored[index]);
    log_char(b, '\n');
    log_text(b, "    Last total before -> after: ");
    log_u64(b, g_integerLastBefore[index]);
    log_text(b, " -> ");
    log_u64(b, g_integerLastAfter[index]);
    log_char(b, '\n');
}

static void write_log() {
    if (!pCreateFileA || !pWriteFile || !pCloseHandle || !g_logPath[0]) return;
    LogBuffer b;
    b.data = g_logBufferStorage;
    b.pos = 0;
    b.cap = sizeof(g_logBufferStorage);
    log_text(b, "DS2 Sam Stats Booster v1.0.0 STABLE\n");
    log_text(b, "State: ");
    log_text(b, status_text());
    log_char(b, '\n');
    log_text(b, "Error: ");
    log_text(b, error_text());
    log_char(b, '\n');
    log_text(b, "Supported target: DS2.exe 1.10.89.0\n");
    log_text(b, "Runtime PE timestamp: ");
    log_hex32(b, g_runtimeTimestamp);
    log_char(b, '\n');
    log_text(b, "Runtime image size: ");
    log_hex32(b, g_runtimeImageSize);
    log_char(b, '\n');
    log_text(b, "Multiplier: ");
    log_fixed2(b, g_multiplier);
    log_char(b, '\n');
    log_text(b, "Enabled mask: ");
    log_hex32(b, g_enabledMask);
    log_char(b, '\n');
    log_text(b, "Periodic logger: ");
    log_text(b, g_workerStarted ? "running" : "not running");
    log_char(b, '\n');
    log_text(b, "\nHook resolution:\n");
    log_text(b, "  U32 combat/strand producer: ");
    log_text(b, (g_hookFlags & 1u) ? "installed at DS2.exe+" : "not installed; candidate DS2.exe+");
    log_hex32(b, g_u32Rva);
    log_text(b, (g_resolutionFlags & 1u) ? " (known RVA)\n" : " (AOB fallback)\n");
    log_text(b, "  Q64 lung producer: ");
    log_text(b, (g_hookFlags & 2u) ? "installed at DS2.exe+" : "not installed; candidate DS2.exe+");
    log_hex32(b, g_q64Rva);
    log_text(b, (g_resolutionFlags & 2u) ? " (known RVA)\n" : " (AOB fallback)\n");
    log_text(b, "  Double physical producer: ");
    log_text(b, (g_hookFlags & 4u) ? "installed at DS2.exe+" : "not installed; candidate DS2.exe+");
    log_hex32(b, g_doubleRva);
    log_text(b, (g_resolutionFlags & 4u) ? " (known RVA)\n" : " (AOB fallback)\n");
    log_text(b, "\nNative positive gain events observed:\n");
    append_counter(b, "TerrainTraversal", 0);
    append_counter(b, "CarryingCapacity", 1);
    append_counter(b, "Endurance", 2);
    append_counter(b, "KOResistance", 3);
    append_counter(b, "LungCapacity", 4);
    append_counter(b, "StrandSkills", 5);
    append_counter(b, "MeleeCombat", 6);
    append_counter(b, "Handguns", 7);
    append_counter(b, "AssaultRifles", 8);
    append_counter(b, "SniperRifles", 9);
    append_counter(b, "MachineGuns", 10);
    append_counter(b, "Shotguns", 11);
    append_counter(b, "StealthTakedownInternal", 12);
    log_text(b, "\nPhysical write verification (getter before/after native mutator):\n");
    append_physical_verification(b, "TerrainTraversal", 0);
    append_physical_verification(b, "CarryingCapacity", 1);
    append_physical_verification(b, "Endurance", 2);
    append_physical_verification(b, "KOResistance", 3);
    log_text(b, "\nInteger write verification (getter before/after native mutator):\n");
    append_integer_verification(b, "LungCapacity", 4);
    append_integer_verification(b, "StrandSkills", 5);
    append_integer_verification(b, "MeleeCombat", 6);
    append_integer_verification(b, "Handguns", 7);
    append_integer_verification(b, "AssaultRifles", 8);
    append_integer_verification(b, "SniperRifles", 9);
    append_integer_verification(b, "MachineGuns", 10);
    append_integer_verification(b, "Shotguns", 11);
    append_integer_verification(b, "StealthTakedownInternal", 12);
    log_text(b, "\nInterpretation: if Requested boosted sum is larger than Raw gain sum and Actual stored gain sum follows it, the multiplier reached DS2 storage. A stored gain of 0 usually means the native record is already capped.\n");
    log_text(b, "\nCounters are diagnostic. They increment only after DS2's native skill eligibility checks and when the native gain is positive.\n");
    log_text(b, "StealthTakedownInternal is disabled by default because it is not one of the 12 visible radar-chart stats.\n");
    b.data[b.pos] = 0;

    HANDLE h = pCreateFileA(g_logPath, 0x40000000u, 3u, nullptr, 2u, 0x80u, nullptr);
    if ((usize)h == ~(usize)0) return;
    DWORD wrote = 0;
    pWriteFile(h, b.data, (DWORD)b.pos, &wrote, nullptr);
    pCloseHandle(h);
}

static DWORD log_worker(void*) {
    pSleep(2000);
    while (!g_stopWorker) {
        write_log();
        pSleep(5000);
    }
    return 0;
}

struct TextSection {
    u8* base;
    usize size;
};

static bool get_pe_info(void* module, u32& timestamp, u32& imageSize, TextSection& text) {
    if (!module) return false;
    u8* b = (u8*)module;
    if (*(u16*)b != 0x5A4D) return false;
    u32 peoff = *(u32*)(b + 0x3C);
    u8* nt = b + peoff;
    if (*(u32*)nt != 0x00004550u) return false;
    u8* coff = nt + 4;
    u16 sections = *(u16*)(coff + 2);
    timestamp = *(u32*)(coff + 4);
    u16 optSize = *(u16*)(coff + 16);
    u8* opt = coff + 20;
    if (*(u16*)opt != 0x20B) return false;
    imageSize = *(u32*)(opt + 56);
    u8* sh = opt + optSize;
    for (u16 i = 0; i < sections; ++i) {
        u8* s = sh + (usize)i * 40;
        if (s[0] == '.' && s[1] == 't' && s[2] == 'e' && s[3] == 'x' && s[4] == 't' && s[5] == 0) {
            u32 vsize = *(u32*)(s + 8);
            u32 vaddr = *(u32*)(s + 12);
            text.base = b + vaddr;
            text.size = vsize;
            return true;
        }
    }
    return false;
}

static bool match_at(const u8* p, const u8* pat, const char* mask, usize n) {
    for (usize i = 0; i < n; ++i) if (mask[i] == 'x' && p[i] != pat[i]) return false;
    return true;
}

static u8* find_unique(const TextSection& text, const u8* pat, const char* mask, usize n, u32& count) {
    count = 0;
    u8* found = nullptr;
    if (!text.base || text.size < n) return nullptr;
    for (usize i = 0; i + n <= text.size; ++i) {
        if (!match_at(text.base + i, pat, mask, n)) continue;
        ++count;
        found = text.base + i;
        if (count > 1) return nullptr;
    }
    return count == 1 ? found : nullptr;
}

static u8* resolve_anchor(u8* exe, const TextSection& text, u32 knownRva,
                          const u8* pat, const char* mask, usize n,
                          u32 resolutionBit, u32 errorCode) {
    u8* known = exe + knownRva;
    if (known >= text.base && known + n <= text.base + text.size && match_at(known, pat, mask, n)) {
        g_resolutionFlags |= resolutionBit;
        return known;
    }
    u32 count = 0;
    u8* found = find_unique(text, pat, mask, n, count);
    if (!found) g_error = errorCode;
    return found;
}

struct PatchRecord {
    u8* target;
    usize length;
    u8 original[32];
    int applied;
};

static bool apply_abs_jump(PatchRecord& rec, void* hook) {
    if (!rec.target || rec.length < 12 || rec.length > sizeof(rec.original)) return false;
    for (usize i = 0; i < rec.length; ++i) rec.original[i] = rec.target[i];
    DWORD oldp = 0;
    if (!pVirtualProtect(rec.target, rec.length, 0x40u, &oldp)) return false;
    rec.target[0] = 0x48;
    rec.target[1] = 0xB8;
    *(u64*)(rec.target + 2) = (u64)hook;
    rec.target[10] = 0xFF;
    rec.target[11] = 0xE0;
    for (usize i = 12; i < rec.length; ++i) rec.target[i] = 0x90;
    pFlushInstructionCache((HANDLE)(usize)-1, rec.target, rec.length);
    DWORD dummy = 0;
    pVirtualProtect(rec.target, rec.length, oldp, &dummy);
    rec.applied = 1;
    return true;
}

static void rollback_patch(PatchRecord& rec) {
    if (!rec.applied) return;
    DWORD oldp = 0;
    if (!pVirtualProtect(rec.target, rec.length, 0x40u, &oldp)) return;
    for (usize i = 0; i < rec.length; ++i) rec.target[i] = rec.original[i];
    pFlushInstructionCache((HANDLE)(usize)-1, rec.target, rec.length);
    DWORD dummy = 0;
    pVirtualProtect(rec.target, rec.length, oldp, &dummy);
    rec.applied = 0;
}

static i32 read_i32(const u8* p) { return *(const i32*)p; }

static bool install_hooks(void* exeBase) {
    u8* exe = (u8*)exeBase;
    TextSection text{};
    if (!get_pe_info(exeBase, g_runtimeTimestamp, g_runtimeImageSize, text)) {
        g_error = 3;
        return false;
    }
    if (g_runtimeTimestamp != 0x6A3DAE46u || g_runtimeImageSize != 0x0B292000u) {
        g_error = 3;
        return false;
    }

    static const u8 u32Pat[] = {
        0x40,0x57,0x48,0x83,0xEC,0x20,0x80,0x7A,0x02,0x01,0x48,0x8B,0xFA,0x0F,0x85,
        0,0,0,0,0x0F,0xB7,0x02,0x48,0x89,0x5C,0x24,0x30,0x66,0x83,0xF8,0x3E
    };
    static const char u32Mask[] = "xxxxxxxxxxxxxxx????xxxxxxxxxxxx";
    static const u8 q64Pat[] = {
        0x40,0x53,0x48,0x83,0xEC,0x20,0x80,0x7A,0x02,0x01,0x48,0x8B,0xDA,0x75,0x2E,
        0x66,0x83,0x3A,0x05,0x75,0x28,0xB2,0x0B,0xE8,0,0,0,0,0x84,0xC0,0x74,0x1D
    };
    static const char q64Mask[] = "xxxxxxxxxxxxxxxxxxxxxxxx????xxxx";
    static const u8 dblPat[] = {
        0x40,0x57,0x48,0x83,0xEC,0x20,0x80,0x7A,0x02,0x01,0x48,0x8B,0xFA,0x75,0x6C,
        0x0F,0xB7,0x02,0x48,0x89,0x5C,0x24,0x30,0x66,0x83,0xF8,0x07,0x75,0x09,0xBB,
        0x08,0x00,0x00,0x00
    };
    static const char dblMask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    u8* u32Anchor = resolve_anchor(exe, text, 0x0D92F30u, u32Pat, u32Mask, sizeof(u32Pat), 1u, 10u);
    if (!u32Anchor) return false;
    u8* q64Anchor = resolve_anchor(exe, text, 0x0D92FF0u, q64Pat, q64Mask, sizeof(q64Pat), 2u, 11u);
    if (!q64Anchor) return false;
    u8* dblAnchor = resolve_anchor(exe, text, 0x0D93040u, dblPat, dblMask, sizeof(dblPat), 4u, 12u);
    if (!dblAnchor) return false;

    g_u32Rva = (u32)(u32Anchor - exe);
    g_q64Rva = (u32)(q64Anchor - exe);
    g_doubleRva = (u32)(dblAnchor - exe);

    u8* u32Site = u32Anchor + 0x9A;
    u8* q64Site = q64Anchor + 0x20;
    u8* dblSite = dblAnchor + 0x5A;

    static const u8 u32SitePat[] = {
        0x44,0x8B,0x47,0x04,0x0F,0xB7,0xD3,0x48,0x8B,0x0D,0,0,0,0,
        0xE8,0,0,0,0
    };
    static const char u32SiteMask[] = "xxxxxxxxxx????x????";
    static const u8 q64SitePat[] = {
        0x4C,0x8B,0x43,0x08,0xBA,0x06,0,0,0,0x49,0xB9,0xFF,0x0F,0xA5,0xD4,0xE8,0,0,0,
        0x48,0x83,0xC4,0x20,0x5B,0xE9,0x33,0xDE,0xE6,0xFF
    };
    static const char q64SiteMask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    static const u8 dblSitePat[] = {
        0xC5,0xFB,0x10,0x1D,0,0,0,0,
        0xC5,0xFB,0x10,0x57,0x08,
        0x48,0x8B,0x0D,0,0,0,0,
        0x0F,0xB7,0xD3,
        0xE8,0,0,0,0
    };
    static const char dblSiteMask[] = "xxxx????xxxxxxxx????xxxx????";

    if (!match_at(u32Site, u32SitePat, u32SiteMask, sizeof(u32SitePat))) {
        g_error = 20;
        return false;
    }
    if (!match_at(q64Site, q64SitePat, q64SiteMask, sizeof(q64SitePat))) {
        g_error = 21;
        return false;
    }
    if (!match_at(dblSite, dblSitePat, dblSiteMask, sizeof(dblSitePat))) {
        g_error = 22;
        return false;
    }

    i32 managerDisp = read_i32(u32Site + 10);
    u8* managerSlot = u32Site + 14 + managerDisp;
    if (managerSlot < exe || managerSlot + 8 > exe + g_runtimeImageSize ||
        (u32)(managerSlot - exe) != 0x0623E5E8u) {
        g_error = 23;
        return false;
    }
    g_managerSlot = managerSlot;

    i32 capDisp = read_i32(dblSite + 4);
    u8* capAddress = dblSite + 8 + capDisp;
    if (capAddress < exe || capAddress + 8 > exe + g_runtimeImageSize ||
        (u32)(capAddress - exe) != 0x03462020u) {
        g_error = 24;
        return false;
    }
    g_doubleCap = *(double*)capAddress;
    if (!(g_doubleCap > 1.0 && g_doubleCap < 1000000000000000.0)) {
        g_error = 24;
        return false;
    }

    u8* u32Getter = exe + 0x00C01120u;
    static const u8 u32GetterPat[] = {0x40,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xF9,0x66,0x83,0xFA,0x79};
    static const char u32GetterMask[] = "xxxxxxxxxxxxx";
    if (!match_at(u32Getter, u32GetterPat, u32GetterMask, sizeof(u32GetterPat))) {
        g_error = 27;
        return false;
    }
    u8* u32Mutator = exe + 0x00C00D70u;
    static const u8 u32MutatorPat[] = {0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xEC,0x20};
    static const char u32MutatorMask[] = "xxxxxxxxxxxxxxx";
    if (!match_at(u32Mutator, u32MutatorPat, u32MutatorMask, sizeof(u32MutatorPat))) {
        g_error = 28;
        return false;
    }

    u8* q64Getter = exe + 0x00C011D0u;
    static const u8 q64GetterPat[] = {0x40,0x57,0x48,0x83,0xEC,0x20,0x33,0xC0,0x48,0x8B,0xF9,0x66,0x83,0xFA,0x09};
    static const char q64GetterMask[] = "xxxxxxxxxxxxxxx";
    u8* q64Mutator = exe + 0x00C00E60u;
    static const u8 q64MutatorPat[] = {0x48,0x89,0x6C,0x24,0x18,0x56,0x57,0x41,0x56,0x48,0x83,0xEC,0x20};
    static const char q64MutatorMask[] = "xxxxxxxxxxxxx";
    const bool q64MutatorOriginal = match_at(q64Mutator, q64MutatorPat, q64MutatorMask, sizeof(q64MutatorPat));
    const bool q64MutatorDetoured = q64Mutator[0] == 0x48 && q64Mutator[1] == 0xB8 &&
                                      q64Mutator[10] == 0xFF && q64Mutator[11] == 0xE0;
    if (!match_at(q64Getter, q64GetterPat, q64GetterMask, sizeof(q64GetterPat)) ||
        (!q64MutatorOriginal && !q64MutatorDetoured)) {
        g_error = 29;
        return false;
    }
    pU32Getter = (U32Getter_t)u32Getter;
    pU32Mutator = (U32Mutator_t)u32Mutator;
    pQ64Getter = (Q64Getter_t)q64Getter;
    pQ64Mutator = (Q64Mutator_t)q64Mutator;

    u8* doubleGetter = exe + 0x00C012B0u;
    static const u8 doubleGetterPat[] = {0x40,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xF9,0x66,0x83,0xFA,0x2F};
    static const char doubleGetterMask[] = "xxxxxxxxxxxxx";
    if (!match_at(doubleGetter, doubleGetterPat, doubleGetterMask, sizeof(doubleGetterPat))) {
        g_error = 25;
        return false;
    }

    i32 mutatorDisp = read_i32(dblSite + 24);
    u8* doubleMutator = dblSite + 28 + mutatorDisp;
    static const u8 doubleMutatorPat[] = {0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xEC,0x50};
    static const char doubleMutatorMask[] = "xxxxxxxxxx";
    if ((u32)(doubleMutator - exe) != 0x00C00FA0u ||
        !match_at(doubleMutator, doubleMutatorPat, doubleMutatorMask, sizeof(doubleMutatorPat))) {
        g_error = 26;
        return false;
    }
    pDoubleGetter = (DoubleGetter_t)doubleGetter;
    pDoubleMutator = (DoubleMutator_t)doubleMutator;

    g_u32Continue = u32Site + 19;
    g_q64Continue = q64Site + 29;
    g_doubleContinue = dblSite + 28;

    PatchRecord patches[3]{};
    patches[0].target = u32Site;
    patches[0].length = 19;
    patches[1].target = q64Site;
    patches[1].length = 29;
    patches[2].target = dblSite;
    patches[2].length = 28;

    if (!apply_abs_jump(patches[0], (void*)&SamStatsU32Hook)) {
        g_error = 30;
        return false;
    }
    g_hookFlags |= 1u;
    if (!apply_abs_jump(patches[1], (void*)&SamStatsQ64Hook)) {
        g_error = 31;
        rollback_patch(patches[0]);
        g_hookFlags = 0;
        return false;
    }
    g_hookFlags |= 2u;
    if (!apply_abs_jump(patches[2], (void*)&SamStatsDoubleHook)) {
        g_error = 32;
        rollback_patch(patches[1]);
        rollback_patch(patches[0]);
        g_hookFlags = 0;
        return false;
    }
    g_hookFlags |= 4u;
    return true;
}

extern "C" BOOL DllMain(HMODULE self, DWORD reason, void*) {
    if (reason == 0) {
        g_stopWorker = 1;
        return 1;
    }
    if (reason != 1) return 1;

    if (!resolve_api()) {
        g_status = 0;
        g_error = 1;
        return 1;
    }
    make_sibling_path(self, ".ini", g_iniPath, sizeof(g_iniPath));
    make_sibling_path(self, ".log", g_logPath, sizeof(g_logPath));
    load_ini();

    if (!g_enabled) {
        g_status = 101;
        write_log();
        return 1;
    }

    void* exe = find_module(L"ds2.exe");
    if (!exe) {
        g_status = 0;
        g_error = 2;
        write_log();
        return 1;
    }

    if (!install_hooks(exe)) {
        g_status = 0;
        write_log();
        return 1;
    }

    g_status = 100;
    write_log();
    HANDLE thread = pCreateThread(nullptr, 0, &log_worker, nullptr, 0, nullptr);
    if (thread && (usize)thread != ~(usize)0) {
        g_workerStarted = 1;
        pCloseHandle(thread);
        write_log();
    } else {
        g_status = 102;
        write_log();
    }
    return 1;
}
