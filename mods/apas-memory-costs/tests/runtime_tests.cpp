#define APAS_TEST
#include "../src/apas.cpp"

static u8 g_entry[0x80];
static u64 g_position;
static i32 g_order;
static void* g_manager;
static void* CaptureConstructor(void* manager, u64 position, void* resource, i32 order) {
    g_manager = manager; g_position = position; g_order = order;
    if (resource) memcpy(g_entry + 0x50, (u8*)resource + 0x24, 4);
    return g_entry;
}

extern "C" __declspec(dllexport) u32 TestConfiguration(const wchar_t* path, u32 expected) {
    u32 i = 0;
    while (path[i] && i < 1023) { g_ini[i] = path[i]; ++i; }
    g_ini[i] = 0;
    bool loaded = LoadSettings();
    if (expected == 0xffffffff) return loaded ? 1 : 0;
    if (!loaded) return 2;
    u32 actual = (u32)g_settings.cost * 4 + (g_settings.enabled ? 1u : 0u) + (g_settings.unlockAll ? 2u : 0u);
    return actual == expected ? 0 : 3;
}

extern "C" __declspec(dllexport) u32 TestCostRules(void* image) {
    g_base = (u8*)image; g_original = CaptureConstructor;
    alignas(16) u8 resource[0x60];
    for (u32 mode = 0; mode < 4; ++mode) {
        const i32 targets[] = {1, 0, 25, 1000000};
        g_settings = {true, false, targets[mode]};
        for (u32 id = 0; id < 56; ++id) {
            memset(resource, 0x7a, sizeof(resource));
            *(u64*)resource = (u64)g_base + kResourceVtableRva;
            resource[0x20] = (u8)id;
            *(i32*)(resource + 0x24) = 40;
            void* result = CostConstructor(g_entry, 0x123456789abcdef0, resource, -1);
            i32 expected = id >= 4 && id < 55 ? targets[mode] : 40;
            if (*(i32*)(resource + 0x24) != expected || *(i32*)(g_entry + 0x50) != expected) return 10 + id;
            if (resource[0x23] != 0x7a || resource[0x28] != 0x7a || resource[0x20] != id) return 80;
            if (result != g_entry || g_manager != g_entry || g_position != 0x123456789abcdef0 || g_order != -1) return 81;
        }
        const i32 special[] = {0, -1, 1000001, 0x7fffffff};
        for (i32 native : special) {
            resource[0x20] = 4; *(i32*)(resource + 0x24) = native;
            CostConstructor(nullptr, 0, resource, 0);
            if (*(i32*)(resource + 0x24) != native) return 82;
        }
    }
    g_settings = {false, false, 1};
    *(i32*)(resource + 0x24) = 40;
    CostConstructor(nullptr, 0, resource, 0);
    if (*(i32*)(resource + 0x24) != 40) return 83;
    g_settings.enabled = true;
    *(u64*)resource = 0;
    CostConstructor(nullptr, 0, resource, 0);
    if (*(i32*)(resource + 0x24) != 40) return 84;
    if (CostConstructor(nullptr, 0, nullptr, 0) != g_entry) return 85;
    return 0;
}

// Execute the real generated relay/trampoline against a native ABI probe with
// the exact five-byte prologue and original register-save/stack layout.
extern "C" __declspec(dllexport) u32 TestRelay(void* image) {
    u8* page = (u8*)VirtualAlloc(nullptr, 4096, kReserve | kCommit, kReadWrite);
    if (!page) return 1;
    u8* native = page + 0x100;
    memcpy(native, kConstructorBytes, 18); // includes sub rsp,68h
    const u8 tail[] = {
        0x41,0x8b,0x40,0x24,         // mov eax,[r8+24]
        0x89,0x41,0x50,              // mov [rcx+50],eax
        0x48,0x89,0x51,0x08,         // mov [rcx+8],rdx
        0x4c,0x89,0x41,0x10,         // mov [rcx+10],r8
        0x44,0x89,0x49,0x18,         // mov [rcx+18],r9d
        0x48,0x8b,0xc1,              // mov rax,rcx
        0x48,0x83,0xc4,0x68,
        0x41,0x5f,0x41,0x5d,0x41,0x5c,0x5e,0x5d,0x5b,0xc3
    };
    memcpy(native + 18, tail, sizeof(tail));
    Patch patch;
    if (!PrepareCost((u8*)((u64)native - kConstructorRva), page + 0x200, &patch)) return 2;
    DWORD old;
    if (!VirtualProtect(page, 4096, kExecuteReadWrite, &old)) return 3;
    g_base = (u8*)image; g_settings = {true, false, 1};
    g_original = (Constructor)(page + 0x220);
    if (!Exchange(patch, true)) return 4;
    if (!Equal(native + 5, kConstructorBytes + 5, 11)) return 5;
    if (Exchange(patch, true)) return 6; // never overwrite an already changed site
    FlushInstructionCache(GetCurrentProcess(), page, 4096);
    alignas(16) u8 resource[0x60] = {};
    *(u64*)resource = (u64)g_base + kResourceVtableRva;
    resource[0x20] = 4; *(i32*)(resource + 0x24) = 40;
    void* result = ((Constructor)native)(g_entry, 0x1122334455667788, resource, -7);
    if (result != g_entry || *(i32*)(resource + 0x24) != 1 || *(i32*)(g_entry + 0x50) != 1) return 7;
    if (*(u64*)(g_entry + 8) != 0x1122334455667788 || *(void**)(g_entry + 16) != resource ||
        *(i32*)(g_entry + 24) != -7) return 8;
    // New entry/new resource after the first construction: no count threshold or timer.
    alignas(16) u8 later[0x60]; memcpy(later, resource, sizeof(later));
    later[0x20] = 54; *(i32*)(later + 0x24) = 80;
    ((Constructor)native)(g_entry, 0, later, 55);
    if (*(i32*)(later + 0x24) != 1 || *(i32*)(g_entry + 0x50) != 1) return 9;
    if (!Exchange(patch, false) || !Equal(native, kConstructorBytes, 16)) return 10;
    FlushInstructionCache(GetCurrentProcess(), page, 4096);
    *(i32*)(resource + 0x24) = 40;
    ((Constructor)native)(g_entry, 0, resource, 0);
    if (*(i32*)(resource + 0x24) != 40 || *(i32*)(g_entry + 0x50) != 40) return 11;
    VirtualFree(page, 0, kRelease);
    g_original = nullptr;
    return 0;
}

static bool RestoreBlock(u8* address, const u8* original) {
    Patch patch; patch.address = address;
    memcpy(patch.before.bytes, original, 16);
    memcpy(patch.after.bytes, address, 16);
    DWORD old, ignored;
    if (!VirtualProtect(address, 16, kExecuteReadWrite, &old)) return false;
    bool result = Exchange(patch, false);
    VirtualProtect(address, 16, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), address, 16);
    return result;
}

extern "C" __declspec(dllexport) u32 TestMappedInstall(void* image) {
    u8* base = (u8*)image;
    if (!ValidateImage(base)) return 1;
    // Four independent INI combinations; all return to the exact native bytes.
    for (u32 flags = 0; flags < 4; ++flags) {
        g_settings = {(flags & 1) != 0, (flags & 2) != 0, 1};
        if (!Install(base)) return 10 + flags;
        if ((base[kConstructorRva] == 0xe9) != g_settings.enabled) return 20 + flags;
        if ((base[kUnlockBlockRva + 9] == 0xeb) != g_settings.unlockAll) return 30 + flags;
        if (g_settings.unlockAll && kUnlockBlockRva + 9 + 2 + base[kUnlockBlockRva + 10] != 0xBE39EE) return 40;
        if (!RestoreBlock(base + kConstructorRva, kConstructorBytes) ||
            !RestoreBlock(base + kUnlockBlockRva, kUnlockBytes) || !ValidateImage(base)) return 41;
    }
    g_settings = {true, true, 1};
    *(void**)(base + kManagerRva) = g_entry;
    bool lateInstalled = Install(base);
    *(void**)(base + kManagerRva) = nullptr;
    if (lateInstalled || !ValidateImage(base)) return 42;
    // An older Unlock All patch must be detected even when the new patch is at
    // an earlier site. A failed preflight must leave the cost hook untouched.
    DWORD old, ignored;
    VirtualProtect(base + 0xBE39C0, 16, kExecuteReadWrite, &old);
    base[0xBE39C9] = 0xeb; base[0xBE39CA] = 0x23;
    bool conflictInstalled = Install(base);
    base[0xBE39C9] = 0x33; base[0xBE39CA] = 0xdb;
    VirtualProtect(base + 0xBE39C0, 16, old, &ignored);
    if (conflictInstalled || !ValidateImage(base)) return 43;
    // Different PE metadata, despite all local code signatures still matching.
    u32 pe = *(u32*)(base + 0x3c);
    VirtualProtect(base, 4096, kReadWrite, &old);
    *(u32*)(base + pe + 8) ^= 1;
    bool unknownInstalled = Install(base);
    *(u32*)(base + pe + 8) ^= 1;
    VirtualProtect(base, 4096, old, &ignored);
    if (unknownInstalled || !ValidateImage(base)) return 44;
    MemoryInfo info;
    VirtualQuery(base + kConstructorRva, &info, sizeof(info));
    if (info.protect & (0x04 | 0x08 | 0x40 | 0x80)) return 45;
    return 0;
}

// Calls the actual game accounting and activation instructions in an isolated
// mapped image. No game entry point, save, Steam API or game imports are invoked.
extern "C" __declspec(dllexport) u32 TestNativeCosts(void* image) {
    u8* base = (u8*)image;
    alignas(16) u8 manager[0x400] = {};
    alignas(16) u8 entry[0x80] = {};
    alignas(16) u8 resource[0x60] = {};
    *(void**)(manager + 0x30) = entry;
    *(void**)(entry + 0x70) = resource;
    entry[0x78] = 1; // already active: bypass effect callbacks and external imports
    auto occupied = (i32 (*)(void*))(base + 0xBE3D00);
    auto activate = (u8 (*)(void*, u8, u8))(base + 0xBE12E0);
    // Old unconditional cost writes can make a zero-capacity base activation fail.
    *(i32*)(resource + 0x24) = 1;
    if (occupied(manager) != 1 || activate(manager, 0, 1) != 0) return 1;
    // Preserving a natively free base lets the same actual native check succeed.
    *(i32*)(resource + 0x24) = 0;
    if (occupied(manager) != 0 || activate(manager, 0, 1) != 1) return 2;
    *(void**)(manager + 0x30) = nullptr;
    *(void**)(manager + 0x30 + 4 * 8) = entry;
    const i32 costs[] = {0, 1, 25, 1000000};
    for (i32 cost : costs) {
        *(i32*)(resource + 0x24) = cost;
        *(i32*)(manager + 8) = cost * 2;
        if (occupied(manager) != cost || activate(manager, 4, 1) != 1) return 3;
        if (cost) {
            *(i32*)(manager + 8) = cost * 2 - 1;
            if (activate(manager, 4, 1) != 0) return 4;
        }
    }
    return 0;
}

extern "C" BOOL __stdcall DllMain(HMODULE, DWORD, void*) { return 1; }
