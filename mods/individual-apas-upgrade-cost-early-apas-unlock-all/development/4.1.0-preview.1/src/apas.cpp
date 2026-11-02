#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <bcrypt.h>
#include <intrin.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cwchar>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i32 = int32_t;
#include "target.h"

namespace apas {
constexpr char Version[] = "4.1.0-preview.1";
constexpr u32 ConstructorRva = 0xBE0270, ResourceVtable = 0x32088B8;
constexpr u32 ManagerRva = 0x623E5D0, RootRva = 0x623E310, GateRva = 0x623E5E8;
constexpr u32 RingRva = 0x16E5120, RingSlotRva = 0x32FBE20, LocateRva = 0xBE1640;
struct Settings { bool costs = true, unlock = false, early = false; i32 cost = 1; };
static Settings config;
static u8* image;
static HMODULE self;
static wchar_t iniPath[MAX_PATH * 4], logPath[MAX_PATH * 4];
static volatile LONG ready = 0, inUnlock = 0, ringReported = 0, unlockReported = 0;
using Constructor = void* (*)(void*, u64, void*, i32);
using Ring = void (*)(void*);
using Locate = u8 (*)(void*, u8);
static Constructor constructor;
static Ring ring;
static Locate locate;

static void Log(const char* text) {
    if (!logPath[0]) return;
    HANDLE f = CreateFileW(logPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD n;
    WriteFile(f, text, static_cast<DWORD>(strlen(text)), &n, nullptr);
    WriteFile(f, "\r\n", 2, &n, nullptr);
    CloseHandle(f);
}

static bool Setting(const wchar_t* section, const wchar_t* key, const wchar_t* fallback,
                    u32 limit, u32& out) {
    wchar_t value[64];
    DWORD n = GetPrivateProfileStringW(section, key, fallback, value, 64, iniPath);
    if (!n || n >= 63) return false;
    u32 v = 0;
    for (DWORD i = 0; i < n; ++i) {
        if (value[i] < L'0' || value[i] > L'9') return false;
        u32 digit = value[i] - L'0';
        if (v > limit / 10 || (v == limit / 10 && digit > limit % 10)) return false;
        v = v * 10 + digit;
    }
    out = v;
    return true;
}
static bool LoadSettings() {
    u32 cost, enabled, unlock, early;
    if (!Setting(L"APASMemoryCosts", L"Enabled", L"1", 1, enabled) ||
        !Setting(L"APASMemoryCosts", L"GlobalCost", L"1", 1000000, cost) ||
        !Setting(L"APASUnlocks", L"UnlockAll", L"0", 1, unlock) ||
        !Setting(L"APASUnlocks", L"EarlyAccess", L"0", 1, early)) return false;
    config = {enabled != 0, unlock != 0, early != 0, static_cast<i32>(cost)};
    return true;
}

enum class Check { Good, Different, TemporarilyUnreadable };
// Respect guard/no-access pages. The only protection adjustment permitted here
// is adding read access to an existing execute-only loader page, then restoring it.
static Check Read(const void* source, void* dest, size_t length) {
    auto src = static_cast<const u8*>(source);
    auto dst = static_cast<u8*>(dest);
    while (length) {
        size_t chunk = 4096 - (reinterpret_cast<uintptr_t>(src) & 4095);
        if (chunk > length) chunk = length;
        MEMORY_BASIC_INFORMATION info{};
        if (VirtualQuery(src, &info, sizeof(info)) != sizeof(info) ||
            info.State != MEM_COMMIT || (info.Protect & PAGE_GUARD) ||
            (info.Protect & 0xff) == PAGE_NOACCESS) return Check::Different;
        SIZE_T got = 0;
        if (!ReadProcessMemory(GetCurrentProcess(), src, dst, chunk, &got) || got != chunk) {
            if ((info.Protect & 0xff) != PAGE_EXECUTE) return Check::Different;
            DWORD old = 0, ignored = 0;
            DWORD readable = (info.Protect & ~0xffu) | PAGE_EXECUTE_READ;
            if (!VirtualProtect(const_cast<u8*>(src), chunk, readable, &old))
                return Check::TemporarilyUnreadable;
            got = 0;
            bool copied = ReadProcessMemory(GetCurrentProcess(), src, dst, chunk, &got) && got == chunk;
            if (!VirtualProtect(const_cast<u8*>(src), chunk, old, &ignored)) return Check::Different;
            if (!copied) return Check::Different;
        }
        src += chunk; dst += chunk; length -= chunk;
    }
    return Check::Good;
}
static Check Validate(u8* base) {
    IMAGE_DOS_HEADER dos{};
    IMAGE_NT_HEADERS64 pe{};
    if (Read(base, &dos, sizeof(dos)) != Check::Good || dos.e_magic != IMAGE_DOS_SIGNATURE ||
        dos.e_lfanew < 64 || dos.e_lfanew > 4096 ||
        Read(base + dos.e_lfanew, &pe, sizeof(pe)) != Check::Good ||
        pe.Signature != IMAGE_NT_SIGNATURE || pe.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        pe.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) return Check::Different;
    // Some ASI loaders rewrite timestamp/SizeOfImage. Complete function hashes
    // and relocated vtable hashes identify the supported executable instead.
    for (const auto& anchor : Anchors) {
        u8 buffer[4096], digest[32];
        if (anchor.length > sizeof(buffer)) return Check::Different;
        Check read = Read(base + anchor.rva, buffer, anchor.length);
        if (read != Check::Good) return read;
        if (anchor.pointers) {
            for (u32 i = 0; i < anchor.length; i += 8) {
                u64 address;
                memcpy(&address, buffer + i, 8);
                address = address - reinterpret_cast<u64>(base) + 0x140000000ull;
                memcpy(buffer + i, &address, 8);
            }
        }
        if (BCryptHash(BCRYPT_SHA256_ALG_HANDLE, nullptr, 0, buffer, anchor.length,
                       digest, sizeof(digest)) < 0 || memcmp(digest, anchor.sha, 32))
            return Check::Different;
    }
    return Check::Good;
}

static bool ValidResource(void* resource, u32 id) {
    return resource && *static_cast<void**>(resource) == image + ResourceVtable &&
           static_cast<u8*>(resource)[0x20] == id;
}
static void AdjustCost(void* resource) {
    if (!config.costs || !resource || *static_cast<void**>(resource) != image + ResourceVtable) return;
    auto p = static_cast<u8*>(resource);
    u32 id = p[0x20];
    auto cost = reinterpret_cast<i32*>(p + 0x24);
    // Free/base/sentinel nodes are part of the game's accounting contract.
    if (id >= 4 && id < 55 && *cost > 0 && *cost <= 1000000) *cost = config.cost;
}
static void* CreateNode(void* manager, u64 position, void* resource, i32 order) {
    // A callback already in flight must remain a pass-through if setup failed.
    if (InterlockedCompareExchange(&ready, 0, 0))
        AdjustCost(resource); // before native code copies resource+24 to entry+50
    return constructor(manager, position, resource, order);
}

static u32 UnlockNodes() {
    if (!config.unlock || !locate || InterlockedCompareExchange(&inUnlock, 1, 0)) return 0;
    auto manager = *reinterpret_cast<u8**>(image + ManagerRva);
    auto root = *reinterpret_cast<u8**>(image + RootRva);
    auto system = root ? *reinterpret_cast<u8**>(root + 0x2C0) : nullptr;
    u32 created = 0;
    bool available = manager && system && *reinterpret_cast<u32*>(image + GateRva) != 0;
    // The four native base nodes must exist before locating paid enhancements.
    for (u32 id = 0; available && id < 4; ++id)
        available = *reinterpret_cast<void**>(manager + 0x30 + id * 8) != nullptr;
    if (available) {
        for (u32 id = 4; id < 55; ++id) {
            void* resource = *reinterpret_cast<void**>(system + 0x30 + id * 8);
            if (!ValidResource(resource, id) || *reinterpret_cast<void**>(manager + 0x30 + id * 8)) continue;
            // Native allocation, indexing and side effects; no fabricated entries
            // and no global story facts. Retry missing nodes on the next menu open.
            if (locate(manager, static_cast<u8>(id))) ++created;
        }
    }
    InterlockedExchange(&inUnlock, 0);
    return created;
}
static void BuildRing(void* state) {
    ring(state); // unchanged original function, no menu trampoline
    if (!InterlockedCompareExchange(&ready, 0, 0) || !state) return;
    auto s = static_cast<u8*>(state);
    if (config.early) {
        s[0x2F] = 1;
        s[0x38] = s[0x43] == 0 ? 1 : 0; // retain native contextual restrictions
    }
    if (config.unlock && s[0x43] == 0 && UnlockNodes() && !InterlockedExchange(&unlockReported, 1))
        Log("UNLOCK_RUNTIME: native APAS enhancements created. Reloads are checked independently.");
    if (!InterlockedExchange(&ringReported, 1))
        Log("RING_RUNTIME: virtual callback reached; original menu builder completed.");
}

// Tail padding keeps both snapshots aligned for the atomic 16-byte exchange.
#pragma warning(push)
#pragma warning(disable:4324)
struct alignas(16) CodePatch { u8 before[16], after[16]; u8* address; };
#pragma warning(pop)
static CodePatch costPatch{};
#ifdef APAS_REBUILD_TEST
static void (*installationRace)(u8*) = nullptr;
#endif
extern "C" void* ApasResume = nullptr;
extern "C" void* ApasNativeConstructor(void*, u64, void*, i32);
static bool ExchangeCode(const CodePatch& p, bool install) {
    alignas(16) LONG64 expected[2];
    LONG64 next[2];
    memcpy(expected, install ? p.before : p.after, 16);
    memcpy(next, install ? p.after : p.before, 16);
    return InterlockedCompareExchange128(reinterpret_cast<volatile LONG64*>(p.address),
                                        next[1], next[0], expected) != 0;
}
static bool NoPaidEntries(u8* base) {
    u8* manager = nullptr;
    if (Read(base + ManagerRva, &manager, 8) != Check::Good) return false;
    if (!manager) return true;
    for (u32 id = 4; id < 55; ++id) {
        void* entry = nullptr;
        if (Read(manager + 0x30 + id * 8, &entry, 8) != Check::Good || entry) return false;
    }
    return true;
}
static bool Install(u8* base) {
    if (!config.costs && !config.unlock && !config.early) { Log("OFF: no changes requested."); return true; }
    if (Validate(base) != Check::Good) { Log("REFUSED: unsupported executable or conflicting APAS modification."); return false; }
    if (config.costs && !NoPaidEntries(base)) { Log("REFUSED: APAS was already loaded. Restart the game."); return false; }
    image = base;
    ring = reinterpret_cast<Ring>(base + RingRva);
    locate = reinterpret_cast<Locate>(base + LocateRva);
    constructor = ApasNativeConstructor;
    ApasResume = base + ConstructorRva + 14;
    bool needRing = config.unlock || config.early;
    auto slot = reinterpret_cast<void* volatile*>(base + RingSlotRva);
    DWORD codeOld = 0, slotOld = 0, ignored = 0;
    bool codeOpen = false, slotOpen = false, codeApplied = false, slotApplied = false;
    InterlockedExchange(&ready, 0);
    if (config.costs) {
        costPatch.address = base + ConstructorRva;
        if (Read(costPatch.address, costPatch.before, 16) != Check::Good) return false;
        memcpy(costPatch.after, costPatch.before, 16);
        // Complete 14-byte prologue, preserved by the checked static ABI bridge.
        costPatch.after[0] = 0xFF; costPatch.after[1] = 0x25;
        memset(costPatch.after + 2, 0, 4);
        void* target = reinterpret_cast<void*>(&CreateNode);
        memcpy(costPatch.after + 6, &target, 8);
        codeOpen = VirtualProtect(costPatch.address, 16, PAGE_EXECUTE_READWRITE, &codeOld) != 0;
        if (!codeOpen) return false;
    }
    if (needRing) slotOpen = VirtualProtect(base + RingSlotRva, 8, PAGE_READWRITE, &slotOld) != 0;
    bool success = !needRing || slotOpen;
    // Check again after opening pages. Never adopt changed code as the baseline.
    if (success) success = Validate(base) == Check::Good;
    if (success && config.costs) success = codeApplied = ExchangeCode(costPatch, true);
#ifdef APAS_REBUILD_TEST
    if (success && installationRace) installationRace(base);
#endif
    if (success && needRing) {
        slotApplied = InterlockedCompareExchangePointer(slot, reinterpret_cast<void*>(&BuildRing),
                        reinterpret_cast<void*>(ring)) == reinterpret_cast<void*>(ring);
        success = slotApplied;
    }
    // Track current writability separately from the original protection. A failed
    // restore still needs cleanup; a successful restore needs reopening to undo.
    bool finalized = true;
    if (success) {
        if (slotOpen) {
            if (VirtualProtect(base + RingSlotRva, 8, slotOld, &ignored)) slotOpen = false;
            else finalized = false;
        }
        if (codeOpen) {
            if (VirtualProtect(costPatch.address, 16, codeOld, &ignored)) codeOpen = false;
            else finalized = false;
            if (!FlushInstructionCache(GetCurrentProcess(), costPatch.address, 16)) finalized = false;
        }
    }
    if (success && finalized) {
        InterlockedExchange(&ready, 1);
        Log("READY: APAS rebuild. At most one code hook and one virtual-function slot.");
        return true;
    }
    // Finalization errors follow the same rollback path as a conflicting hook.
    // Compare/exchange only our own changes; never overwrite another mod's data.
    bool recovered = true;
    if (slotApplied) {
        if (!slotOpen) slotOpen = VirtualProtect(base + RingSlotRva, 8, PAGE_READWRITE, &ignored) != 0;
        if (!slotOpen || InterlockedCompareExchangePointer(slot, reinterpret_cast<void*>(ring),
            reinterpret_cast<void*>(&BuildRing)) != reinterpret_cast<void*>(&BuildRing)) recovered = false;
    }
    if (codeApplied) {
        if (!codeOpen) codeOpen = VirtualProtect(costPatch.address, 16, PAGE_EXECUTE_READWRITE, &ignored) != 0;
        if (!codeOpen || !ExchangeCode(costPatch, false)) recovered = false;
    }
    if (slotOpen && !VirtualProtect(base + RingSlotRva, 8, slotOld, &ignored)) recovered = false;
    if (codeOpen && !VirtualProtect(costPatch.address, 16, codeOld, &ignored)) recovered = false;
    if (codeApplied && !FlushInstructionCache(GetCurrentProcess(), costPatch.address, 16)) recovered = false;
    Log(recovered ? "ERROR: installation failed; our changes were rolled back. APAS is inactive."
                  : "ERROR: installation failed and recovery was incomplete; restart before playing.");
    return false;
}

#ifndef APAS_REBUILD_TEST
static DWORD WINAPI Start(void*) {
    wchar_t modulePath[MAX_PATH * 4], gamePath[MAX_PATH * 4];
    DWORD n = GetModuleFileNameW(self, modulePath, _countof(modulePath));
    if (!n || n >= _countof(modulePath)) return 0;
    wchar_t* slash = wcsrchr(modulePath, L'\\');
    if (!slash) return 0;
    slash[1] = 0;
    if (swprintf_s(iniPath, L"%sds2_apas_memory_costs.ini", modulePath) < 0 ||
        swprintf_s(logPath, L"%sds2_apas_memory_costs.log", modulePath) < 0) return 0;
    Log("--- DS2 APAS 4.1.0-preview.1 ---");
    n = GetModuleFileNameW(nullptr, gamePath, _countof(gamePath));
    if (!n || n >= _countof(gamePath)) { Log("REFUSED: process path unavailable."); return 0; }
    slash = wcsrchr(gamePath, L'\\');
    if (!slash || _wcsicmp(slash + 1, L"DS2.exe")) {
        Log("REFUSED: process is not DS2.exe."); return 0;
    }
    if (!LoadSettings()) { Log("REFUSED: invalid INI value. No changes installed."); return 0; }
    char settings[160];
    sprintf_s(settings, "Enabled=%d GlobalCost=%d UnlockAll=%d EarlyAccess=%d",
              config.costs, config.cost, config.unlock, config.early);
    Log(settings);
    if (!config.costs && !config.unlock && !config.early) { Log("OFF: no changes requested."); return 0; }
    auto base = reinterpret_cast<u8*>(GetModuleHandleW(nullptr));
    for (u32 attempt = 0; attempt < 300; ++attempt) {
        Check result = Validate(base);
        if (result == Check::Good) { Install(base); return 0; }
        if (result != Check::TemporarilyUnreadable) {
            Log("REFUSED: executable hashes differ, or another APAS mod is installed."); return 0;
        }
        Sleep(100);
    }
    Log("REFUSED: loader did not expose the supported executable within 30 seconds.");
    return 0;
}
static INIT_ONCE initializeOnce = INIT_ONCE_STATIC_INIT;
static BOOL CALLBACK Initialize(PINIT_ONCE, PVOID, PVOID*) {
    // The loader invokes InitializeASI after LoadLibrary has completed. Pin
    // before starting work so a delayed worker cannot outlive the loaded module.
    HMODULE pin = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&Start), &pin)) return FALSE;
    HANDLE worker = CreateThread(nullptr, 0, Start, nullptr, 0, nullptr);
    if (!worker) return FALSE; // InitOnce permits a later explicit retry.
    CloseHandle(worker);
    return TRUE;
}
#endif
} // namespace apas

#ifndef APAS_REBUILD_TEST
extern "C" __declspec(dllexport) const char* DS2ApasMemoryCostsVersion() { return apas::Version; }
extern "C" __declspec(dllexport) void InitializeASI() {
    InitOnceExecuteOnce(&apas::initializeOnce, apas::Initialize, nullptr, nullptr);
}
extern "C" BOOL WINAPI DllMain(HMODULE module, DWORD reason, void*) {
    if (reason == DLL_PROCESS_ATTACH) apas::self = module;
    return TRUE;
}
#endif
