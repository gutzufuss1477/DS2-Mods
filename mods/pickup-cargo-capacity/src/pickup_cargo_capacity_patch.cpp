// DS2 Pickup Cargo Capacity v1.0.0
// Validated current-build patch for the pickup cargo pack-area limit.
// Applies four mutually dependent instruction patches only after exact-byte validation.

extern "C" {
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef const wchar_t* LPCWSTR;
typedef wchar_t* LPWSTR;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef unsigned long long QWORD;
typedef unsigned long long SIZE_T;
typedef int BOOL;

__declspec(dllimport) BOOL DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HANDLE CreateThread(LPVOID, SIZE_T, DWORD (*)(LPVOID), LPVOID, DWORD, DWORD*);
__declspec(dllimport) BOOL CloseHandle(HANDLE);
__declspec(dllimport) HMODULE GetModuleHandleW(LPCWSTR);
__declspec(dllimport) DWORD GetModuleFileNameW(HMODULE, LPWSTR, DWORD);
__declspec(dllimport) UINT GetPrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
__declspec(dllimport) BOOL VirtualProtect(LPVOID, SIZE_T, DWORD, DWORD*);
__declspec(dllimport) BOOL FlushInstructionCache(HANDLE, LPCVOID, SIZE_T);
__declspec(dllimport) HANDLE GetCurrentProcess(void);
__declspec(dllimport) HANDLE CreateFileW(LPCWSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, LPVOID);
}

#define TRUE 1
#define DLL_PROCESS_ATTACH 1
#define MAX_PATH_W 1024
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001u
#define FILE_SHARE_WRITE 0x00000002u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define INVALID_HANDLE_VALUE ((HANDLE)(QWORD)-1)
#define PAGE_EXECUTE_READWRITE 0x40u

extern "C" int _fltused = 0;
extern "C" void* memcpy(void* dst, const void* src, SIZE_T n) {
    BYTE* d = (BYTE*)dst;
    const BYTE* s = (const BYTE*)src;
    for (SIZE_T i = 0; i < n; ++i) d[i] = s[i];
    return dst;
}

static HMODULE g_self = 0;
static wchar_t g_dir[MAX_PATH_W];
static wchar_t g_ini[MAX_PATH_W];
static wchar_t g_status[MAX_PATH_W];

static const QWORD RVA_AREA_COUNT = 0x01187D34ull;
static const QWORD RVA_INDEX_LIMIT_A = 0x01187FB8ull;
static const QWORD RVA_INDEX_LIMIT_B = 0x0118809Bull;
static const QWORD RVA_AREA_LIMIT = 0x011881D1ull;

static SIZE_T wlen2(const wchar_t* s) {
    SIZE_T n = 0;
    while (s && s[n]) ++n;
    return n;
}

static void wcopy(wchar_t* dst, const wchar_t* src, SIZE_T cap) {
    SIZE_T i = 0;
    if (!cap) return;
    while (src && src[i] && i + 1 < cap) {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = 0;
}

static void wappend(wchar_t* dst, const wchar_t* src, SIZE_T cap) {
    SIZE_T i = wlen2(dst), j = 0;
    while (src && src[j] && i + 1 < cap) dst[i++] = src[j++];
    dst[i] = 0;
}

struct TextBuffer {
    char data[2048];
    DWORD len;
};

static void tb_init(TextBuffer* t) { t->len = 0; t->data[0] = 0; }
static void tb_char(TextBuffer* t, char c) {
    if (t->len + 1 < sizeof(t->data)) {
        t->data[t->len++] = c;
        t->data[t->len] = 0;
    }
}
static void tb_str(TextBuffer* t, const char* s) { while (s && *s) tb_char(t, *s++); }
static void tb_uint(TextBuffer* t, QWORD v) {
    char tmp[32];
    int n = 0;
    if (!v) { tb_char(t, '0'); return; }
    while (v && n < 31) { tmp[n++] = (char)('0' + (v % 10)); v /= 10; }
    while (n) tb_char(t, tmp[--n]);
}
static void tb_hex(TextBuffer* t, QWORD v, int digits) {
    static const char* H = "0123456789ABCDEF";
    tb_str(t, "0x");
    for (int i = digits - 1; i >= 0; --i) tb_char(t, H[(v >> (i * 4)) & 0xF]);
}

static void set_paths() {
    DWORD n = GetModuleFileNameW(g_self, g_dir, MAX_PATH_W);
    if (!n || n >= MAX_PATH_W) {
        g_dir[0] = 0;
    } else {
        while (n && g_dir[n - 1] != L'\\' && g_dir[n - 1] != L'/') --n;
        g_dir[n] = 0;
    }
    wcopy(g_ini, g_dir, MAX_PATH_W);
    wappend(g_ini, L"PickupCargoCapacity.ini", MAX_PATH_W);
    wcopy(g_status, g_dir, MAX_PATH_W);
    wappend(g_status, L"PickupCargoCapacity_STATUS.txt", MAX_PATH_W);
}

static void write_status(const char* state, const char* detail, int configured, int effective, int areas, int index_bytes, QWORD base) {
    HANDLE h = CreateFileW(g_status, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (h == INVALID_HANDLE_VALUE) return;
    TextBuffer t;
    tb_init(&t);
    tb_str(&t, "DS2 Pickup Cargo Capacity v1.0.0\r\nSTATE=");
    tb_str(&t, state);
    tb_str(&t, "\r\nDETAIL=");
    tb_str(&t, detail);
    tb_str(&t, "\r\nCONFIGURED_CAPACITY_UNITS=");
    tb_uint(&t, (QWORD)configured);
    tb_str(&t, "\r\nEFFECTIVE_CAPACITY_UNITS=");
    tb_uint(&t, (QWORD)effective);
    tb_str(&t, "\r\nPACK_AREA_COUNT=");
    tb_uint(&t, (QWORD)areas);
    tb_str(&t, "\r\nINDEX_LIMIT_BYTES=");
    tb_uint(&t, (QWORD)index_bytes);
    tb_str(&t, "\r\nDS2_MODULE_BASE=");
    tb_hex(&t, base, 16);
    tb_str(&t, "\r\nPATCH_RVA_AREA_COUNT=");
    tb_hex(&t, RVA_AREA_COUNT, 8);
    tb_str(&t, "\r\nPATCH_RVA_INDEX_LIMIT_A=");
    tb_hex(&t, RVA_INDEX_LIMIT_A, 8);
    tb_str(&t, "\r\nPATCH_RVA_INDEX_LIMIT_B=");
    tb_hex(&t, RVA_INDEX_LIMIT_B, 8);
    tb_str(&t, "\r\nPATCH_RVA_AREA_LIMIT=");
    tb_hex(&t, RVA_AREA_LIMIT, 8);
    tb_str(&t, "\r\n");
    DWORD written = 0;
    WriteFile(h, t.data, t.len, &written, 0);
    CloseHandle(h);
}

static BOOL bytes_equal(const BYTE* p, const BYTE* expected, SIZE_T n) {
    for (SIZE_T i = 0; i < n; ++i) if (p[i] != expected[i]) return 0;
    return 1;
}

static BOOL patch_bytes(BYTE* target, const BYTE* replacement, SIZE_T n) {
    DWORD old_protect = 0;
    if (!VirtualProtect(target, n, PAGE_EXECUTE_READWRITE, &old_protect)) return 0;
    memcpy(target, replacement, n);
    FlushInstructionCache(GetCurrentProcess(), target, n);
    DWORD ignored = 0;
    VirtualProtect(target, n, old_protect, &ignored);
    return 1;
}

static DWORD worker(LPVOID) {
    set_paths();

    int enabled = (int)GetPrivateProfileIntW(L"PickupCargoCapacity", L"Enabled", 1, g_ini);
    int configured = (int)GetPrivateProfileIntW(L"PickupCargoCapacity", L"CapacityUnits", 320, g_ini);

    if (!enabled) {
        write_status("DISABLED", "Enabled=0; no code was modified", configured, 160, 11, 44, 0);
        return 0;
    }

    // The direct four-site patch uses signed imm8 comparisons. Derived values must stay <= 127.
    int clamped = configured;
    if (clamped < 160) clamped = 160;
    if (clamped > 480) clamped = 480;
    int effective = (clamped / 16) * 16;
    if (effective < 160) effective = 160;

    int area_count = (effective / 16) + 1; // area 0 is reserved
    int index_limit_bytes = area_count * 4;

    if (area_count > 31 || index_limit_bytes > 127) {
        write_status("ERROR", "Configured capacity exceeds safe direct-patch encoding", configured, effective, area_count, index_limit_bytes, 0);
        return 0;
    }

    BYTE* base = (BYTE*)GetModuleHandleW(0);
    if (!base) {
        write_status("ERROR", "DS2.exe module was not found", configured, effective, area_count, index_limit_bytes, 0);
        return 0;
    }

    BYTE* p1 = base + RVA_AREA_COUNT;
    BYTE* p2 = base + RVA_INDEX_LIMIT_A;
    BYTE* p3 = base + RVA_INDEX_LIMIT_B;
    BYTE* p4 = base + RVA_AREA_LIMIT;

    const BYTE original1[5] = {0xBA, 0x0B, 0x00, 0x00, 0x00};
    const BYTE original2[4] = {0x49, 0x83, 0xFF, 0x2C};
    const BYTE original3[4] = {0x49, 0x83, 0xFE, 0x2C};
    const BYTE original4[4] = {0x48, 0x83, 0xFB, 0x0B};

    if (!bytes_equal(p1, original1, sizeof(original1)) ||
        !bytes_equal(p2, original2, sizeof(original2)) ||
        !bytes_equal(p3, original3, sizeof(original3)) ||
        !bytes_equal(p4, original4, sizeof(original4))) {
        write_status("ERROR", "Exact original bytes do not match this supported DS2 build; no code was modified", configured, effective, area_count, index_limit_bytes, (QWORD)base);
        return 0;
    }

    BYTE replacement1[5] = {0xBA, (BYTE)area_count, 0x00, 0x00, 0x00};
    BYTE replacement2[4] = {0x49, 0x83, 0xFF, (BYTE)index_limit_bytes};
    BYTE replacement3[4] = {0x49, 0x83, 0xFE, (BYTE)index_limit_bytes};
    BYTE replacement4[4] = {0x48, 0x83, 0xFB, (BYTE)area_count};

    if (!patch_bytes(p1, replacement1, sizeof(replacement1)) ||
        !patch_bytes(p2, replacement2, sizeof(replacement2)) ||
        !patch_bytes(p3, replacement3, sizeof(replacement3)) ||
        !patch_bytes(p4, replacement4, sizeof(replacement4))) {
        patch_bytes(p1, original1, sizeof(original1));
        patch_bytes(p2, original2, sizeof(original2));
        patch_bytes(p3, original3, sizeof(original3));
        patch_bytes(p4, original4, sizeof(original4));
        write_status("ERROR", "A code write failed; original bytes were restored where possible", configured, effective, area_count, index_limit_bytes, (QWORD)base);
        return 0;
    }

    write_status("READY", "All four validated pickup pack-limit patches are active", configured, effective, area_count, index_limit_bytes, (QWORD)base);
    return 0;
}

extern "C" BOOL DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_self = module;
        DisableThreadLibraryCalls(module);
        HANDLE thread = CreateThread(0, 0, worker, 0, 0, 0);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
