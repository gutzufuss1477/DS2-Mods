#define APAS_REBUILD_TEST
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <initializer_list>
// Faults apply only to calls made by the included production implementation.
// Fixtures below still use the real Win32 APIs for their isolated allocations.
static unsigned protectCall = 0, failProtectCall = 0;
static bool failFlushOnce = false;
static BOOL WINAPI FixtureProtect(LPVOID p, SIZE_T n, DWORD protection, PDWORD old) {
    if (failProtectCall && ++protectCall == failProtectCall) {
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    return VirtualProtect(p, n, protection, old);
}
static BOOL WINAPI FixtureFlush(HANDLE process, LPCVOID p, SIZE_T n) {
    if (failFlushOnce) { failFlushOnce = false; SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    return FlushInstructionCache(process, p, n);
}
#define VirtualProtect FixtureProtect
#define FlushInstructionCache FixtureFlush
#include "../src/apas.cpp"
#undef VirtualProtect
#undef FlushInstructionCache
using namespace apas;
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
#define TEST(name) extern "C" __declspec(dllexport) u32 name

TEST(MemoryReadProtections)(void*) {
    struct Pages {
        u8* p = static_cast<u8*>(VirtualAlloc(nullptr, 8192, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        ~Pages() { if (p) VirtualFree(p, 0, MEM_RELEASE); }
    } pages;
    CHECK(pages.p);
    memset(pages.p, 0x5a, 8192);
    DWORD old;
    MEMORY_BASIC_INFORMATION info{};
    u8 copy[32];
    for (DWORD protection : {PAGE_READONLY, PAGE_READWRITE, PAGE_EXECUTE_READ, PAGE_EXECUTE}) {
        CHECK(VirtualProtect(pages.p, 4096, protection, &old));
        memset(copy, 0, sizeof(copy));
        CHECK(Read(pages.p, copy, sizeof(copy)) == Check::Good);
        for (u8 value : copy) CHECK(value == 0x5a);
        CHECK(VirtualQuery(pages.p, &info, sizeof(info)) == sizeof(info));
        CHECK(info.Protect == protection);
    }
    for (DWORD protection : {PAGE_NOACCESS, PAGE_READWRITE | PAGE_GUARD}) {
        CHECK(VirtualProtect(pages.p, 4096, protection, &old));
        memset(copy, 0x7c, sizeof(copy));
        CHECK(Read(pages.p, copy, sizeof(copy)) == Check::Different);
        for (u8 value : copy) CHECK(value == 0x7c);
        CHECK(VirtualQuery(pages.p, &info, sizeof(info)) == sizeof(info));
        CHECK(info.Protect == protection); // guard/no-access must never be cleared
    }
    CHECK(VirtualProtect(pages.p, 8192, PAGE_READONLY, &old));
    CHECK(Read(pages.p + 4096 - 16, copy, sizeof(copy)) == Check::Good);
    for (u8 value : copy) CHECK(value == 0x5a);
    CHECK(VirtualProtect(pages.p + 4096, 4096, PAGE_NOACCESS, &old));
    CHECK(Read(pages.p + 4096 - 16, copy, sizeof(copy)) == Check::Different);
    CHECK(VirtualQuery(pages.p + 4096, &info, sizeof(info)) == sizeof(info));
    CHECK(info.Protect == PAGE_NOACCESS);
    CHECK(VirtualFree(pages.p + 4096, 4096, MEM_DECOMMIT));
    CHECK(Read(pages.p + 4096, copy, sizeof(copy)) == Check::Different);
    CHECK(Read(nullptr, copy, sizeof(copy)) == Check::Different);
    CHECK(Read(nullptr, copy, 0) == Check::Good);
    return 0;
}

TEST(Configuration)(const wchar_t* path, u32 expected) {
    wcscpy_s(iniPath, path);
    bool ok = LoadSettings();
    if (expected == 0xffffffff) { CHECK(!ok); return 0; }
    CHECK(ok);
    CHECK(static_cast<u32>(config.cost) * 8 + config.costs + config.unlock * 2 + config.early * 4 == expected);
    return 0;
}

TEST(CostRules)(void* base) {
    image = static_cast<u8*>(base);
    for (i32 target : {0, 1, 25, 1000000}) {
        config = {true, false, false, target};
        for (u32 id = 0; id < 56; ++id) {
            alignas(16) u8 resource[0x60], expected[0x60];
            memset(resource, 0x7a, sizeof(resource));
            *reinterpret_cast<void**>(resource) = image + ResourceVtable;
            resource[0x20] = static_cast<u8>(id);
            *reinterpret_cast<i32*>(resource + 0x24) = 40;
            memcpy(expected, resource, sizeof(resource));
            if (id >= 4 && id < 55) *reinterpret_cast<i32*>(expected + 0x24) = target;
            AdjustCost(resource);
            CHECK(!memcmp(resource, expected, sizeof(resource)));
        }
        alignas(16) u8 resource[0x60]{};
        *reinterpret_cast<void**>(resource) = image + ResourceVtable;
        resource[0x20] = 4;
        for (i32 native : {0, -1, 1000001, INT32_MAX}) {
            *reinterpret_cast<i32*>(resource + 0x24) = native;
            AdjustCost(resource); CHECK(*reinterpret_cast<i32*>(resource + 0x24) == native);
        }
        *reinterpret_cast<i32*>(resource + 0x24) = 40;
        config.costs = false; AdjustCost(resource); CHECK(*reinterpret_cast<i32*>(resource + 0x24) == 40);
        config.costs = true; *reinterpret_cast<void**>(resource) = nullptr;
        AdjustCost(resource); CHECK(*reinterpret_cast<i32*>(resource + 0x24) == 40);
        AdjustCost(nullptr);
    }
    return 0;
}

static u8 resources[55][0x60], entries[55][0x80];
static u32 calls = 0, failId = 55, ringCalls = 0;
static bool recursiveFailure = false;
static u8 LocateProbe(void* manager, u8 id) {
    ++calls;
    if (id == failId) return 0;
    *reinterpret_cast<void**>(static_cast<u8*>(manager) + 0x30 + id * 8) = entries[id];
    if (UnlockNodes() != 0) recursiveFailure = true;
    return 1;
}
static void RingProbe(void* state) {
    ++ringCalls;
    static_cast<u8*>(state)[0x2F] = 0;
    static_cast<u8*>(state)[0x38] = 0;
}
TEST(MenuAndReload)(void* base) {
    image = static_cast<u8*>(base);
    alignas(16) u8 manager[0x200]{}, root[0x2D0]{}, system[0x200]{};
    void* savedManager = *reinterpret_cast<void**>(image + ManagerRva);
    void* savedRoot = *reinterpret_cast<void**>(image + RootRva);
    u32 savedGate = *reinterpret_cast<u32*>(image + GateRva);
    *reinterpret_cast<void**>(image + ManagerRva) = manager;
    *reinterpret_cast<void**>(image + RootRva) = root;
    *reinterpret_cast<void**>(root + 0x2C0) = system;
    locate = LocateProbe; ring = RingProbe; ready = 1; inUnlock = 0;
    for (u32 id = 0; id < 55; ++id) {
        *reinterpret_cast<void**>(resources[id]) = image + ResourceVtable;
        resources[id][0x20] = static_cast<u8>(id);
        *reinterpret_cast<void**>(system + 0x30 + id * 8) = resources[id];
    }
    for (u32 flags = 0; flags < 8; ++flags) {
        config = {(flags & 1) != 0, (flags & 2) != 0, (flags & 4) != 0, 1};
        memset(manager, 0, sizeof(manager)); calls = ringCalls = 0;
        *reinterpret_cast<u32*>(image + GateRva) = 1;
        CHECK(UnlockNodes() == 0 && calls == 0); // bases not yet constructed
        for (u32 id = 0; id < 4; ++id) *reinterpret_cast<void**>(manager + 0x30 + id * 8) = entries[id];
        *reinterpret_cast<u32*>(image + GateRva) = 0;
        CHECK(UnlockNodes() == 0 && calls == 0);
        *reinterpret_cast<u32*>(image + GateRva) = 1;
        u8 state[128], expected[128]; memset(state, 0x5a, sizeof(state)); state[0x43] = 1;
        memcpy(expected, state, sizeof(state)); expected[0x2F] = config.early ? 1 : 0; expected[0x38] = 0;
        BuildRing(state); CHECK(!memcmp(state, expected, sizeof(state)) && calls == 0 && ringCalls == 1);
        state[0x43] = expected[0x43] = 0; expected[0x38] = config.early ? 1 : 0;
        failId = 54; BuildRing(state);
        CHECK(!memcmp(state, expected, sizeof(state)) && calls == (config.unlock ? 51u : 0u));
        failId = 55; BuildRing(state);
        CHECK(calls == (config.unlock ? 52u : 0u));
        BuildRing(state); CHECK(calls == (config.unlock ? 52u : 0u));
        // Same manager address reused by a save reload: no stale 'done' flag.
        memset(manager + 0x50, 0, 51 * 8); BuildRing(state);
        CHECK(calls == (config.unlock ? 103u : 0u));
    }
    *reinterpret_cast<void**>(image + ManagerRva) = savedManager;
    *reinterpret_cast<void**>(image + RootRva) = savedRoot;
    *reinterpret_cast<u32*>(image + GateRva) = savedGate;
    ready = 0; CHECK(!recursiveFailure); return 0;
}

static bool Undo(u8* base) {
    DWORD old, ignored;
    if (config.costs) {
        if (!VirtualProtect(costPatch.address, 16, PAGE_EXECUTE_READWRITE, &old)) return false;
        bool ok = ExchangeCode(costPatch, false);
        VirtualProtect(costPatch.address, 16, old, &ignored);
        FlushInstructionCache(GetCurrentProcess(), costPatch.address, 16);
        if (!ok) return false;
    }
    if (config.unlock || config.early) {
        if (!VirtualProtect(base + RingSlotRva, 8, PAGE_READWRITE, &old)) return false;
        void* previous = InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(base + RingSlotRva),
            base + RingRva, reinterpret_cast<void*>(&BuildRing));
        VirtualProtect(base + RingSlotRva, 8, old, &ignored);
        if (previous != reinterpret_cast<void*>(&BuildRing)) return false;
    }
    ready = 0; return true;
}
static void ConflictAfterCost(u8* base) {
    InterlockedExchangePointer(reinterpret_cast<void* volatile*>(base + RingSlotRva), reinterpret_cast<void*>(&RingProbe));
}
TEST(Installation)(void* mapped) {
    auto base = static_cast<u8*>(mapped);
    CHECK(Validate(base) == Check::Good);
    u8 originalCtor[16], originalRing[32], originalUnlock[32];
    memcpy(originalCtor, base + ConstructorRva, 16);
    memcpy(originalRing, base + RingRva, 32);
    memcpy(originalUnlock, base + 0xBE39A0, 32);
    for (u32 flags = 0; flags < 8; ++flags) {
        config = {(flags & 1) != 0, (flags & 2) != 0, (flags & 4) != 0, 1};
        CHECK(Install(base));
        CHECK((memcmp(originalCtor, base + ConstructorRva, 16) != 0) == config.costs);
        CHECK((*reinterpret_cast<void**>(base + RingSlotRva) != base + RingRva) == (config.unlock || config.early));
        CHECK(!memcmp(originalRing, base + RingRva, 32));
        CHECK(!memcmp(originalUnlock, base + 0xBE39A0, 32));
        MEMORY_BASIC_INFORMATION info{};
        VirtualQuery(base + ConstructorRva, &info, sizeof(info));
        CHECK(!(info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)));
        VirtualQuery(base + RingSlotRva, &info, sizeof(info));
        CHECK(!(info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)));
        CHECK(Undo(base)); CHECK(Validate(base) == Check::Good);
    }
    config = {true, true, true, 1};
    installationRace = ConflictAfterCost;
    CHECK(!Install(base));
    CHECK(!memcmp(originalCtor, base + ConstructorRva, 16));
    CHECK(*reinterpret_cast<void**>(base + RingSlotRva) == reinterpret_cast<void*>(&RingProbe));
    CHECK(ready == 0);
    DWORD raceOld, raceIgnored;
    CHECK(VirtualProtect(base + RingSlotRva, 8, PAGE_READWRITE, &raceOld));
    *reinterpret_cast<void**>(base + RingSlotRva) = base + RingRva;
    CHECK(VirtualProtect(base + RingSlotRva, 8, raceOld, &raceIgnored));
    installationRace = nullptr; CHECK(Validate(base) == Check::Good);
    for (const auto& anchor : Anchors) {
        u8* target = base + anchor.rva + anchor.length - 1;
        DWORD old, ignored; CHECK(VirtualProtect(target, 1, PAGE_EXECUTE_READWRITE, &old));
        *target ^= 1;
        CHECK(Validate(base) == Check::Different);
        CHECK(!Install(base)); CHECK(!memcmp(originalCtor, base + ConstructorRva, 16));
        *target ^= 1; CHECK(VirtualProtect(target, 1, old, &ignored));
        CHECK(Validate(base) == Check::Good);
    }
    // Every exact byte remains mandatory even if loader metadata is rewritten.
    DWORD old, ignored;
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
    CHECK(VirtualProtect(base, 4096, PAGE_READWRITE, &old));
    nt->FileHeader.TimeDateStamp ^= 1; nt->OptionalHeader.SizeOfImage ^= 4096;
    CHECK(Install(base)); CHECK(Undo(base));
    nt->FileHeader.TimeDateStamp ^= 1; nt->OptionalHeader.SizeOfImage ^= 4096;
    CHECK(VirtualProtect(base, 4096, old, &ignored));
    // Execute-only loader pages regain their exact original protection.
    CHECK(VirtualProtect(base + ConstructorRva, 4096, PAGE_EXECUTE, &old));
    CHECK(Install(base)); CHECK(Undo(base));
    MEMORY_BASIC_INFORMATION info{}; VirtualQuery(base + ConstructorRva, &info, sizeof(info));
    CHECK(info.Protect == PAGE_EXECUTE);
    CHECK(VirtualProtect(base + ConstructorRva, 4096, old, &ignored));
    alignas(16) u8 manager[0x200]{};
    auto managerSlot = reinterpret_cast<void**>(base + ManagerRva); void* saved = *managerSlot;
    *managerSlot = manager; *reinterpret_cast<void**>(manager + 0x50) = manager;
    CHECK(!Install(base)); CHECK(Validate(base) == Check::Good);
    *managerSlot = saved;
    return 0;
}

TEST(FinalizationFailures)(void* mapped) {
    auto base = static_cast<u8*>(mapped);
    u8 before[16]; memcpy(before, base + ConstructorRva, sizeof(before));
    void* oldSlot = *reinterpret_cast<void**>(base + RingSlotRva);
    MEMORY_BASIC_INFORMATION code{}, slot{}, after{};
    CHECK(VirtualQuery(base + ConstructorRva, &code, sizeof(code)));
    CHECK(VirtualQuery(base + RingSlotRva, &slot, sizeof(slot)));
    for (unsigned failure = 1; failure <= 5; ++failure) {
        CHECK(Validate(base) == Check::Good);
        config = {true, true, true, 1}; ready = 0;
        protectCall = 0; failProtectCall = failure <= 4 ? failure : 0;
        failFlushOnce = failure == 5;
        bool installed = Install(base);
        failProtectCall = 0; failFlushOnce = false;
        CHECK(!installed && ready == 0);
        CHECK(!memcmp(before, base + ConstructorRva, sizeof(before)));
        CHECK(*reinterpret_cast<void**>(base + RingSlotRva) == oldSlot);
        CHECK(VirtualQuery(base + ConstructorRva, &after, sizeof(after)) && after.Protect == code.Protect);
        CHECK(VirtualQuery(base + RingSlotRva, &after, sizeof(after)) && after.Protect == slot.Protect);
        CHECK(Validate(base) == Check::Good);
    }
    return 0;
}
static unsigned constructorCalls = 0;
static void* ConstructorProbe(void* manager, u64, void*, i32) {
    ++constructorCalls; return manager;
}
TEST(InactiveCallback)(void* mapped) {
    image = static_cast<u8*>(mapped); constructor = ConstructorProbe;
    alignas(16) u8 resource[0x60]{};
    *reinterpret_cast<void**>(resource) = image + ResourceVtable;
    resource[0x20] = 4;
    auto cost = reinterpret_cast<i32*>(resource + 0x24);
    config = {true, false, false, 1}; *cost = 40; ready = 0; constructorCalls = 0;
    CHECK(CreateNode(resource, 0, resource, 0) == resource);
    CHECK(*cost == 40 && constructorCalls == 1);
    ready = 1;
    CHECK(CreateNode(resource, 0, resource, 0) == resource);
    CHECK(*cost == 1 && constructorCalls == 2);
    ready = 0; return 0;
}
TEST(NativeAccounting)(void* mapped) {
    auto base = static_cast<u8*>(mapped);
    alignas(16) u8 manager[0x400]{}, entry[0x80]{}, resource[0x60]{};
    image = base;
    *reinterpret_cast<void**>(resource) = base + ResourceVtable; resource[0x20] = 4;
    *reinterpret_cast<void**>(manager + 0x50) = entry;
    *reinterpret_cast<void**>(entry + 0x70) = resource;
    entry[0x78] = 1; // avoids external effect callbacks in this isolated fixture
    auto occupied = reinterpret_cast<i32 (*)(void*)>(base + 0xBE3D00);
    auto activate = reinterpret_cast<u8 (*)(void*, u8, u8)>(base + 0xBE12E0);
    for (i32 cost : {0, 1, 25, 1000000}) {
        config = {true, false, false, cost}; *reinterpret_cast<i32*>(resource + 0x24) = 40;
        AdjustCost(resource); CHECK(occupied(manager) == cost);
        *reinterpret_cast<i32*>(manager + 8) = cost * 2;
        CHECK(activate(manager, 4, 1) == 1);
        if (cost) { *reinterpret_cast<i32*>(manager + 8) = cost * 2 - 1; CHECK(activate(manager, 4, 1) == 0); }
    }
    return 0;
}

TEST(ConstructorAbi)(void* mapped) {
    auto base = static_cast<u8*>(mapped);
    auto page = static_cast<u8*>(VirtualAlloc(nullptr, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    CHECK(page);
    u8* native = page + 0x100;
    memcpy(native, base + ConstructorRva, 18);
    const u8 tail[] = {0x41,0x8b,0x40,0x24,0x89,0x41,0x50,0x48,0x89,0x51,0x08,
        0x4c,0x89,0x41,0x10,0x44,0x89,0x49,0x18,0x48,0x8b,0xc1,0x48,0x83,0xc4,0x68,
        0x41,0x5f,0x41,0x5d,0x41,0x5c,0x5e,0x5d,0x5b,0xc3};
    memcpy(native + 18, tail, sizeof(tail));
    CodePatch p{}; p.address = native; memcpy(p.before, native, 16); memcpy(p.after, native, 16);
    p.after[0] = 0xff; p.after[1] = 0x25; memset(p.after + 2, 0, 4);
    void* target = reinterpret_cast<void*>(&CreateNode); memcpy(p.after + 6, &target, 8);
    ApasResume = native + 14; constructor = ApasNativeConstructor; image = base; ready = 1;
    DWORD old; CHECK(VirtualProtect(page, 4096, PAGE_EXECUTE_READWRITE, &old));
    CHECK(ExchangeCode(p, true)); CHECK(!ExchangeCode(p, true));
    FlushInstructionCache(GetCurrentProcess(), page, 4096);
    alignas(16) u8 resource[0x60]{}, entry[0x80]{};
    *reinterpret_cast<void**>(resource) = base + ResourceVtable;
    for (i32 cost : {0, 1, 25, 1000000}) {
        config = {true, false, false, cost};
        for (u32 id = 0; id < 55; ++id) {
            resource[0x20] = static_cast<u8>(id); *reinterpret_cast<i32*>(resource + 0x24) = 40;
            CHECK(reinterpret_cast<Constructor>(native)(entry, 0x1122334455667788, resource, -7) == entry);
            CHECK(*reinterpret_cast<i32*>(entry + 0x50) == (id >= 4 ? cost : 40));
            CHECK(*reinterpret_cast<u64*>(entry + 8) == 0x1122334455667788);
            CHECK(*reinterpret_cast<void**>(entry + 16) == resource && *reinterpret_cast<i32*>(entry + 24) == -7);
        }
    }
    CHECK(ExchangeCode(p, false)); CHECK(!memcmp(native, base + ConstructorRva, 18));
    CHECK(VirtualFree(page, 0, MEM_RELEASE));
    DWORD64 bridgeBase = 0;
    auto unwind = RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(&ApasNativeConstructor), &bridgeBase, nullptr);
    CHECK(unwind);
    const u32 offsets[] = {0,5,6,7,8,10,12,14,21};
    for (u32 j = 0; j < _countof(offsets); ++j) {
        alignas(16) DWORD64 stack[64]{}; DWORD64* incoming = stack + 32; incoming[0] = 0x123456789abcdef0;
        CONTEXT c{}; c.ContextFlags = CONTEXT_FULL;
        const DWORD64 values[] = {0x11,0x22,0x33,0x44,0x55,0x66};
        DWORD64* regs[] = {&c.Rbx,&c.Rbp,&c.Rsi,&c.R12,&c.R13,&c.R15};
        u32 pushed = j > 7 ? 6 : j > 1 ? j - 1 : 0;
        for (u32 k = 0; k < 6; ++k) { *regs[k] = values[k]; if (k < pushed) { incoming[-1-static_cast<i32>(k)] = values[k]; *regs[k] = 0; } }
        c.Rip = reinterpret_cast<DWORD64>(&ApasNativeConstructor) + offsets[j];
        c.Rsp = reinterpret_cast<DWORD64>(incoming - pushed);
        void* handler = nullptr; DWORD64 frame = 0;
        RtlVirtualUnwind(UNW_FLAG_NHANDLER, bridgeBase, c.Rip, unwind, &c, &handler, &frame, nullptr);
        CHECK(c.Rip == incoming[0] && c.Rsp == reinterpret_cast<DWORD64>(incoming + 1));
        for (u32 k = 0; k < 6; ++k) CHECK(*regs[k] == values[k]);
    }
    return 0;
}
extern "C" BOOL WINAPI DllMain(HMODULE, DWORD, void*) { return TRUE; }
