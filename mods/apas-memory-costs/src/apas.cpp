#include "platform.h"
#include "target.h"

// One binary, one immutable startup configuration. No timer or gameplay worker.
#define APAS_VERSION "3.0.0-rc.6"
constexpr u32 kConstructorRva = 0xBE0270;
constexpr u32 kUnlockBlockRva = 0xBE39A0;
constexpr u32 kResourceVtableRva = 0x32088B8;

extern "C" void* memcpy(void* dst, const void* src, SIZE_T size) {
    for (SIZE_T i = 0; i < size; ++i) ((u8*)dst)[i] = ((const u8*)src)[i];
    return dst;
}
extern "C" void* memset(void* dst, int value, SIZE_T size) {
    for (SIZE_T i = 0; i < size; ++i) ((u8*)dst)[i] = (u8)value;
    return dst;
}
static bool Equal(const void* a, const void* b, SIZE_T size) {
    for (SIZE_T i = 0; i < size; ++i) if (((const u8*)a)[i] != ((const u8*)b)[i]) return false;
    return true;
}
static bool g_usedProtectedRead = false;
static u32 g_protectedReadFailure = 0;
static bool Read(const void* src, void* dst, SIZE_T size) {
    SIZE_T got = 0;
    if (ReadProcessMemory(GetCurrentProcess(), src, dst, size, &got) && got == size) return true;
    // Ultimate ASI Loader marks this game's executable pages execute-only. Do
    // not weaken the anchor check: permit reads from just the queried 4 KiB page,
    // retry, and restore the exact original protection before returning.
    MemoryInfo info;
    u64 address = (u64)src;
    u64 page = address & ~0xfffull;
    if (!VirtualQuery(src, &info, sizeof(info))) {
        g_protectedReadFailure = 1;
        return false;
    }
    if (info.state != kCommit) {
        g_protectedReadFailure = 2;
        return false;
    }
    // Every anchor read is deliberately bounded to its own 4 KiB page. Do not
    // depend on a loader's coalesced VirtualQuery region boundaries.
    if (size > 0x1000 - (address & 0xfffull)) {
        g_protectedReadFailure = 3;
        return false;
    }
    DWORD previous, ignored;
    if (!VirtualProtect((void*)page, 0x1000, kExecuteRead, &previous)) {
        g_protectedReadFailure = 4;
        return false;
    }
    // The caller is now permitted to read this bounded page. Some loaders still
    // reject a second ReadProcessMemory request, so copy directly in-process.
    memcpy(dst, src, size);
    bool restored = VirtualProtect((void*)page, 0x1000, previous, &ignored) != 0;
    if (!restored) {
        g_protectedReadFailure = 5;
        return false;
    }
    g_usedProtectedRead = true;
    return true;
}

struct Settings { bool enabled; bool unlockAll; i32 cost; };
static Settings g_settings = {true, false, 1};
static u8* g_base = nullptr;
static HMODULE g_self = nullptr;
static HANDLE g_log = (HANDLE)(i64)-1;
static wchar_t g_ini[1024];
using Constructor = void* (*)(void*, u64, void*, i32);
static Constructor g_original = nullptr;
static volatile long g_started = 0;

static void Log(const char* message) {
    if (g_log == (HANDLE)(i64)-1) return;
    DWORD length = 0, written = 0;
    while (message[length]) ++length;
    WriteFile(g_log, message, length, &written, nullptr);
    WriteFile(g_log, "\r\n", 2, &written, nullptr);
}
static void LogNumber(const char* label, u32 number) {
    char buffer[96]; u32 n = 0;
    while (label[n] && n < 64) { buffer[n] = label[n]; ++n; }
    char digits[10]; u32 count = 0;
    do { digits[count++] = (char)('0' + number % 10); number /= 10; } while (number);
    while (count) buffer[n++] = digits[--count];
    buffer[n] = 0;
    Log(buffer);
}
static void LogHexNumber(const char* label, u64 number) {
    char buffer[96]; u32 n = 0;
    while (label[n] && n < 64) { buffer[n] = label[n]; ++n; }
    buffer[n++] = '0'; buffer[n++] = 'x';
    bool emitted = false;
    for (i32 shift = 60; shift >= 0; shift -= 4) {
        u8 digit = (u8)((number >> shift) & 15);
        if (digit || emitted || shift == 0) {
            buffer[n++] = (char)(digit < 10 ? '0' + digit : 'A' + digit - 10);
            emitted = true;
        }
    }
    buffer[n] = 0;
    Log(buffer);
}
static bool Sibling(wchar_t* out, const wchar_t* name) {
    DWORD n = GetModuleFileNameW(g_self, out, 1024);
    if (!n || n >= 1024) return false;
    while (n && out[n - 1] != L'\\' && out[n - 1] != L'/') --n;
    if (!n) return false;
    for (u32 i = 0; name[i]; ++i) {
        if (n >= 1023) return false;
        out[n++] = name[i];
    }
    out[n] = 0;
    return true;
}

// Reject malformed/negative/overflowing values instead of silently enabling cheats.
static bool ParseNumber(const wchar_t* value, u32 maximum, u32* result) {
    while (*value == L' ' || *value == L'\t') ++value;
    if (*value < L'0' || *value > L'9') return false;
    u32 n = 0;
    while (*value >= L'0' && *value <= L'9') {
        u32 digit = (u32)(*value++ - L'0');
        if (n > maximum / 10 || (n == maximum / 10 && digit > maximum % 10)) return false;
        n = n * 10 + digit;
    }
    while (*value == L' ' || *value == L'\t') ++value;
    if (*value) return false;
    *result = n;
    return true;
}
static bool Setting(const wchar_t* section, const wchar_t* key, const wchar_t* fallback,
                    u32 maximum, u32* value) {
    wchar_t text[64];
    DWORD n = GetPrivateProfileStringW(section, key, fallback, text, 64, g_ini);
    return n < 63 && ParseNumber(text, maximum, value);
}
static bool LoadSettings() {
    u32 enabled, cost, unlock;
    if (!Setting(L"APASMemoryCosts", L"Enabled", L"1", 1, &enabled)) {
        Log("ERROR: Enabled must be 0 or 1. No changes installed."); return false;
    }
    if (!Setting(L"APASMemoryCosts", L"GlobalCost", L"1", 1000000, &cost)) {
        Log("ERROR: GlobalCost must be an integer from 0 to 1000000. No changes installed."); return false;
    }
    if (!Setting(L"APASUnlocks", L"UnlockAll", L"0", 1, &unlock)) {
        Log("ERROR: UnlockAll must be 0 or 1. No changes installed."); return false;
    }
    g_settings = {enabled != 0, unlock != 0, (i32)cost};
    LogNumber("Enabled=", enabled);
    LogNumber("GlobalCost=", cost);
    LogNumber("UnlockAll=", unlock);
    return true;
}

static bool ValidateImage(u8* base) {
    g_usedProtectedRead = false;
    g_protectedReadFailure = 0;
    u16 magic = 0, machine = 0;
    u32 pe = 0, signature = 0, timestamp = 0, imageSize = 0;
    if (!Read(base, &magic, 2) || magic != 0x5A4D ||
        !Read(base + 0x3c, &pe, 4) || pe < 0x40 || pe > 0x1000 ||
        !Read(base + pe, &signature, 4) || signature != 0x4550 ||
        !Read(base + pe + 4, &machine, 2) || machine != 0x8664 ||
        !Read(base + pe + 24, &magic, 2) || magic != 0x20b) {
        Log("TARGET_IMAGE_FORMAT_MISMATCH");
        return false;
    }
    if (!Read(base + pe + 8, &timestamp, 4) ||
        !Read(base + pe + 24 + 56, &imageSize, 4)) {
        Log("TARGET_METADATA_READ_FAILURE");
        return false;
    }
    // The user's ASI loader changes these two PE fields in memory. They are not
    // patch targets. The image format plus every exact code/vtable anchor below
    // remain mandatory, so a metadata-only change cannot widen a patch site.
    if (timestamp != kTimestamp || imageSize != kImageSize)
        Log("TARGET_METADATA_CHANGED: exact code anchors still required.");
    for (u32 anchorIndex = 0; anchorIndex < sizeof(kAnchors) / sizeof(kAnchors[0]); ++anchorIndex) {
        const auto& anchor = kAnchors[anchorIndex];
        u8 bytes[128];
        if (!Read(base + anchor.rva, bytes, anchor.length)) {
            Log("TARGET_ANCHOR_READ_FAILURE");
            LogHexNumber("AnchorRva=", anchor.rva);
            if (g_protectedReadFailure) LogNumber("ProtectedReadFailure=", g_protectedReadFailure);
            return false;
        }
        if (anchor.relocatedPointers) {
            for (u32 i = 0; i < anchor.length; i += 8) {
                u64 actual, native;
                memcpy(&actual, bytes + i, 8); memcpy(&native, anchor.bytes + i, 8);
                if (actual != (u64)base + native - 0x140000000ull) {
                    Log("TARGET_RELOCATED_POINTER_MISMATCH");
                    LogHexNumber("AnchorRva=", anchor.rva);
                    LogNumber("PointerIndex=", i / 8);
                    LogHexNumber("Found=", actual);
                    return false;
                }
            }
        } else if (!Equal(bytes, anchor.bytes, anchor.length)) {
            Log("TARGET_ANCHOR_BYTES_MISMATCH");
            LogHexNumber("AnchorRva=", anchor.rva);
            LogNumber("AnchorIndex=", anchorIndex);
            return false;
        }
    }
    if (g_usedProtectedRead)
        Log("TARGET_EXECUTE_ONLY_READ: exact anchors verified; protection restored.");
    return true;
}

// This is called only on the game's native node-creation path, with its live
// resource argument. Set the source cost BEFORE the native copy into entry+0x50.
// Preserve the four special base IDs and all natively free resources.
static void* CostConstructor(void* manager, u64 position, void* resource, i32 order) {
    if (g_settings.enabled && resource && *(u64*)resource == (u64)(g_base + kResourceVtableRva)) {
        u8 id = *((u8*)resource + 0x20);
        volatile i32* cost = (volatile i32*)((u8*)resource + 0x24);
        i32 native = *cost;
        if (id >= 4 && id < 55 && native > 0 && native <= 1000000 && native != g_settings.cost)
            *cost = g_settings.cost;
    }
    return g_original(manager, position, resource, order);
}

struct alignas(16) Block { u8 bytes[16]; };
struct Patch {
    u8* address;
    Block before;
    Block after;
};
static bool Exchange(const Patch& patch, bool install) {
    if ((u64)patch.address & 15) return false;
    alignas(16) i64 expected[2];
    i64 replacement[2];
    memcpy(expected, install ? patch.before.bytes : patch.after.bytes, 16);
    memcpy(replacement, install ? patch.after.bytes : patch.before.bytes, 16);
    return _InterlockedCompareExchange128((volatile i64*)patch.address,
        replacement[1], replacement[0], expected) != 0;
}
static void AbsoluteJump(u8* at, const void* destination) {
    at[0] = 0xff; at[1] = 0x25;
    memset(at + 2, 0, 4);
    u64 address = (u64)destination;
    memcpy(at + 6, &address, 8);
}
static void* AllocateRange(u64 start, u64 limit) {
    u64 address = (start + 0xffff) & ~0xffffull;
    while (address < limit && limit - address >= 4096) {
        MemoryInfo info;
        if (!VirtualQuery((void*)address, &info, sizeof(info))) break;
        u64 end = (u64)info.base + info.size;
        if (end <= address) break;
        if (info.state == 0x10000 && end - address >= 4096) {
            void* page = VirtualAlloc((void*)address, 4096, kReserve | kCommit, kReadWrite);
            if (page) return page;
        }
        address = (end + 0xffff) & ~0xffffull;
    }
    return nullptr;
}
static u8* AllocateRelay(u8* base) {
    u64 address = (u64)base;
    u8* page = (u8*)AllocateRange(address + kImageSize, address + 0x70000000);
    if (!page) page = (u8*)AllocateRange(address > 0x70000000 ? address - 0x70000000 : 0x10000, address);
    return page;
}

static bool PrepareCost(u8* base, u8* relay, Patch* patch) {
    patch->address = base + kConstructorRva;
    memcpy(patch->before.bytes, kConstructorBytes, 16);
    patch->after = patch->before;
    i64 displacement = (i64)relay - (i64)(patch->address + 5);
    if (displacement < -2147483648ll || displacement > 2147483647ll) return false;
    patch->after.bytes[0] = 0xe9;
    i32 relative = (i32)displacement;
    memcpy(patch->after.bytes + 1, &relative, 4);
    // The replaced instruction is exactly five bytes and has no relative operand:
    // mov [rsp+0x10],rdx. All following original instruction boundaries survive.
    AbsoluteJump(relay, (void*)&CostConstructor);
    memcpy(relay + 32, kConstructorBytes, 5);
    AbsoluteJump(relay + 37, patch->address + 5);
    return true;
}
static void PrepareUnlock(u8* base, Patch* patch) {
    patch->address = base + kUnlockBlockRva;
    memcpy(patch->before.bytes, kUnlockBytes, 16);
    patch->after = patch->before;
    // Replace one complete four-byte instruction at BE39A9. Skip only this
    // APAS updater's fact/grade prerequisites, retaining the native system guards
    // and native node creation. EB 43 lands exactly at BE39EE.
    patch->after.bytes[9] = 0xeb; patch->after.bytes[10] = 0x43;
    patch->after.bytes[11] = 0x90; patch->after.bytes[12] = 0x90;
}

static bool Install(u8* base) {
    if (!g_settings.enabled && !g_settings.unlockAll) { Log("OFF: both features disabled."); return true; }
    if (!ValidateImage(base)) {
        Log("UNSUPPORTED_OR_CONFLICT: executable/anchors differ. Nothing patched."); return false;
    }
    // ASI startup only. Avoid partially changing an already loaded manager/save.
    void* manager = nullptr;
    if (!Read(base + kManagerRva, &manager, sizeof(manager)) || manager) {
        Log("LATE_LOAD: APAS manager already exists. Restart using an ASI loader; nothing patched."); return false;
    }
    g_base = base;
    Patch patches[2]; u32 count = 0;
    u8* relay = nullptr;
    if (g_settings.enabled) {
        relay = AllocateRelay(base);
        if (!relay || !PrepareCost(base, relay, &patches[count])) {
            if (relay) VirtualFree(relay, 0, kRelease);
            Log("ERROR: could not prepare cost hook. Nothing patched."); return false;
        }
        DWORD previous;
        if (!VirtualProtect(relay, 4096, kExecuteRead, &previous) ||
            !FlushInstructionCache(GetCurrentProcess(), relay, 64)) {
            VirtualFree(relay, 0, kRelease);
            Log("ERROR: could not finalize executable relay. Nothing patched."); return false;
        }
        g_original = (Constructor)(relay + 32);
        ++count;
    }
    if (g_settings.unlockAll) PrepareUnlock(base, &patches[count++]);
    DWORD protection[2]; u32 opened = 0;
    for (; opened < count; ++opened) {
        if (!VirtualProtect(patches[opened].address, 16, kExecuteReadWrite, &protection[opened])) break;
    }
    u32 applied = 0;
    if (opened == count) {
        for (; applied < count; ++applied) if (!Exchange(patches[applied], true)) break;
    }
    bool success = applied == count;
    if (!success) {
        while (applied) {
            --applied;
            if (!Exchange(patches[applied], false)) Log("ERROR: rollback encountered another code modification.");
        }
    }
    bool restored = true;
    for (u32 i = 0; i < opened; ++i) {
        DWORD ignored;
        if (!VirtualProtect(patches[i].address, 16, protection[i], &ignored)) restored = false;
        if (!FlushInstructionCache(GetCurrentProcess(), patches[i].address, 16)) restored = false;
    }
    // Keep the tiny RX relay alive for the process lifetime, including rollback:
    // another thread may already have entered it. Never free an in-flight target.
    if (!restored) Log("ERROR: protection/cache finalization failed; restart the game.");
    if (!success) Log("CONFLICT: installation aborted and owned changes rolled back.");
    if (success && restored) {
        Log("READY: native APAS creation hook installed. No gameplay polling.");
        if (g_settings.unlockAll) Log("UnlockAll active: APAS prerequisite bypass; saved unlocks may persist.");
    }
    return success && restored;
}

#ifndef APAS_TEST
static DWORD __stdcall Worker(void*) {
    wchar_t path[1024];
    if (Sibling(path, L"ds2_apas_memory_costs.log"))
        g_log = CreateFileW(path, 0x40000000, 1, nullptr, 2, 0x80, nullptr);
    Log("DS2 APAS Memory Costs " APAS_VERSION);
    if (Sibling(g_ini, L"ds2_apas_memory_costs.ini") && LoadSettings()) {
        HMODULE pinned = nullptr;
        // PIN | FROM_ADDRESS: a loader must not unload callback code while a
        // native constructor jump still points into this module.
        if (GetModuleHandleExW(0x5, (const wchar_t*)&Worker, &pinned))
            Install((u8*)GetModuleHandleW(nullptr));
        else Log("ERROR: could not retain callback module. Nothing patched.");
    } else Log("ERROR: configuration could not be loaded. Nothing patched.");
    if (g_log != (HANDLE)(i64)-1) { CloseHandle(g_log); g_log = (HANDLE)(i64)-1; }
    return 0;
}
extern "C" __declspec(dllexport) const char* DS2ApasMemoryCostsVersion() { return APAS_VERSION; }
extern "C" BOOL __stdcall DllMain(HMODULE module, DWORD reason, void*) {
    if (reason == 1 && _InterlockedCompareExchange(&g_started, 1, 0) == 0) {
        g_self = module;
        DisableThreadLibraryCalls(module);
        HANDLE thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return 1;
}
#endif
