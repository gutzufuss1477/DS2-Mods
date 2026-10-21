// Beach Jump with Cargo 1.0.0
// DEATH STRANDING 2: ON THE BEACH - PC Steam 1.10.89.0
#include "windows.h"

#ifndef FASTCALL
#define FASTCALL __fastcall
#endif

#define MOD_VERSION "1.0.0"

static const u32 EXPECTED_PE_TIMESTAMP=0x6A3DAE46u;
static const u32 EXPECTED_SIZE_OF_IMAGE=0x0B292000u;
static const u32 EXPECTED_PE_SIGNATURE=0x00004550u;
static const u32 BAGGAGE_FAST_TRAVEL_RVA=0x011E34A0u;
static const u32 HOOK_BYTES=8u;
static const u8 EXPECTED_PROLOGUE[HOOK_BYTES]={
    0x40u,0x55u,0x41u,0x55u,0x41u,0x56u,0x41u,0x57u
};

static HMODULE g_self=0;
static u8* g_imageBase=0;
static u8* g_relay=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;

static wchar_t lower_ascii(wchar_t c) {
    return c>=L'A' && c<=L'Z' ? (wchar_t)(c+(L'a'-L'A')) : c;
}

static bool is_ds2_process() {
    wchar_t path[1024];
    DWORD n=GetModuleFileNameW((HMODULE)0,path,1024u);
    if (!n || n>=1024u) return false;
    u32 start=0u;
    for (u32 i=0u;i<n;++i) if (path[i]==L'\\' || path[i]==L'/') start=i+1u;
    static const wchar_t wanted[]=L"ds2.exe";
    u32 j=0u;
    while (wanted[j]) {
        if (start+j>=n || lower_ascii(path[start+j])!=wanted[j]) return false;
        ++j;
    }
    return start+j==n;
}

static bool bytes_equal(const void* a0,const void* b0,SIZE_T count) {
    const u8* a=(const u8*)a0;
    const u8* b=(const u8*)b0;
    for (SIZE_T i=0;i<count;++i) if (a[i]!=b[i]) return false;
    return true;
}

static void copy_bytes(void* d0,const void* s0,SIZE_T count) {
    u8* d=(u8*)d0;
    const u8* s=(const u8*)s0;
    for (SIZE_T i=0;i<count;++i) d[i]=s[i];
}

static u32 ascii_length(const char* text) {
    if (!text) return 0u;
    u32 n=0u;
    while (text[n]) ++n;
    return n;
}

static void log_text(const char* text) {
    if (g_log==INVALID_HANDLE_VALUE || !text) return;
    DWORD written=0u;
    const u32 n=ascii_length(text);
    if (n) WriteFile(g_log,text,n,&written,0);
}

static bool build_sibling_path(wchar_t* out,u32 capacity,const wchar_t* name) {
    DWORD n=GetModuleFileNameW(g_self,out,capacity);
    if (!n || n>=capacity) return false;
    u32 slash=0u;
    for (u32 i=0u;i<n;++i) if (out[i]==L'\\' || out[i]==L'/') slash=i+1u;
    u32 j=0u;
    while (name[j]) {
        if (slash+j+1u>=capacity) return false;
        out[slash+j]=name[j];
        ++j;
    }
    out[slash+j]=0;
    return true;
}

static u32 read_enabled() {
    wchar_t path[1024];
    wchar_t value[8];
    if (!build_sibling_path(path,1024u,L"ds2_beach_jump_with_cargo.ini")) return 2u;
    value[0]=0;
    GetPrivateProfileStringW(
        L"BeachJumpWithCargo",L"Enabled",L"1",value,8u,path
    );
    if (value[0]==L'0' && value[1]==0) return 0u;
    if (value[0]==L'1' && value[1]==0) return 1u;
    return 2u;
}

static bool validate_build() {
    if (!g_imageBase || *(volatile const u16*)g_imageBase!=0x5A4Du) return false;
    const u32 peOffset=*(volatile const u32*)(g_imageBase+0x3Cu);
    if (peOffset<0x40u || peOffset>0x1000u) return false;
    u8* nt=g_imageBase+peOffset;
    if (*(volatile const u32*)nt!=EXPECTED_PE_SIGNATURE) return false;
    if (*(volatile const u16*)(nt+24u)!=0x020Bu) return false;
    return *(volatile const u32*)(nt+8u)==EXPECTED_PE_TIMESTAMP &&
           *(volatile const u32*)(nt+24u+56u)==EXPECTED_SIZE_OF_IMAGE;
}

static bool rel32_fits(const u8* instructionNext,const u8* destination) {
    const s64 delta=(s64)((u64)destination-(u64)instructionNext);
    return delta>=(s64)-2147483648LL && delta<=(s64)2147483647LL;
}

static void write_u64_le(u8* output,u64 value) {
    for (u32 i=0u;i<8u;++i) output[i]=(u8)(value>>(i*8u));
}

static u8* finalize_relay(u8* memory,u8* patchSite,u8* destination) {
    if (!memory) return 0;
    if (!rel32_fits(patchSite+5u,memory)) {
        VirtualFree(memory,0u,MEM_RELEASE);
        return 0;
    }
    memory[0]=0x48u;
    memory[1]=0xB8u;
    write_u64_le(memory+2u,(u64)destination);
    memory[10]=0xFFu;
    memory[11]=0xE0u;
    FlushInstructionCache(GetCurrentProcess(),memory,12u);
    DWORD oldProtect=0u;
    if (!VirtualProtect(memory,0x1000u,PAGE_EXECUTE_READ,&oldProtect)) {
        VirtualFree(memory,0u,MEM_RELEASE);
        return 0;
    }
    return memory;
}

static u8* allocate_near_relay(u8* patchSite,u8* destination) {
    const u64 granularity=0x10000ull;
    const u64 maxDistance=0x7FFF0000ull;
    const u64 imageEnd=(u64)g_imageBase+(u64)EXPECTED_SIZE_OF_IMAGE;
    const u64 first=(imageEnd+granularity-1ull)&~(granularity-1ull);

    u8* memory=(u8*)VirtualAlloc(
        (LPVOID)first,0x1000u,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE
    );
    memory=finalize_relay(memory,patchSite,destination);
    if (memory) return memory;

    const u64 aligned=((u64)patchSite)&~(granularity-1ull);
    for (u64 delta=granularity;delta<=maxDistance;delta+=granularity) {
        const u64 candidates[2]={
            aligned+delta,
            aligned>delta ? aligned-delta : 0ull
        };
        for (u32 i=0u;i<2u;++i) {
            if (!candidates[i] || candidates[i]==first) continue;
            memory=(u8*)VirtualAlloc(
                (LPVOID)candidates[i],0x1000u,
                MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE
            );
            memory=finalize_relay(memory,patchSite,destination);
            if (memory) return memory;
        }
    }
    return 0;
}

static bool write_entry_jump(u8* target,u8* relay) {
    if (!rel32_fits(target+5u,relay)) return false;
    const s32 relative=(s32)((s64)((u64)relay-(u64)(target+5u)));
    u8 patch[HOOK_BYTES];
    patch[0]=0xE9u;
    patch[1]=(u8)(relative&0xFF);
    patch[2]=(u8)((relative>>8)&0xFF);
    patch[3]=(u8)((relative>>16)&0xFF);
    patch[4]=(u8)((relative>>24)&0xFF);
    patch[5]=0x90u;
    patch[6]=0x90u;
    patch[7]=0x90u;

    DWORD oldProtect=0u;
    if (!VirtualProtect(target,HOOK_BYTES,PAGE_EXECUTE_READWRITE,&oldProtect)) return false;
    copy_bytes(target,patch,HOOK_BYTES);
    FlushInstructionCache(GetCurrentProcess(),target,HOOK_BYTES);
    const bool ok=bytes_equal(target,patch,HOOK_BYTES);
    DWORD ignored=0u;
    const bool restored=VirtualProtect(target,HOOK_BYTES,oldProtect,&ignored)!=0;
    return ok && restored;
}

static void FASTCALL keep_fast_travel_cargo(void*,u8) {
    // Intentionally skip DSBaggageManager::HandlingBaggagesOnFastTravel(bool).
    // Sam keeps the cargo already attached to his body/backpack through the jump.
}

static bool install_hook() {
    u8* target=g_imageBase+BAGGAGE_FAST_TRAVEL_RVA;
    if (!bytes_equal(target,EXPECTED_PROLOGUE,HOOK_BYTES)) {
        log_text("status=TARGET_BYTES_REJECTED\r\n");
        return false;
    }
    g_relay=allocate_near_relay(target,(u8*)&keep_fast_travel_cargo);
    if (!g_relay) {
        log_text("status=NEAR_RELAY_ALLOCATION_FAILED\r\n");
        return false;
    }
    if (!write_entry_jump(target,g_relay)) {
        log_text("status=ENTRY_PATCH_FAILED\r\n");
        return false;
    }
    log_text("status=ACTIVE\r\n");
    log_text("hook=DSBaggageManager::HandlingBaggagesOnFastTravel(bool)\r\n");
    return true;
}

static DWORD WINAPI worker_thread(LPVOID) {
    Sleep(1200u);
    if (!is_ds2_process()) return 0u;

    g_imageBase=(u8*)GetModuleHandleW((const wchar_t*)0);
    wchar_t logPath[1024];
    if (!build_sibling_path(logPath,1024u,L"ds2_beach_jump_with_cargo.log")) return 1u;
    g_log=CreateFileW(
        logPath,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
    );
    if (g_log==INVALID_HANDLE_VALUE) return 2u;

    log_text("Beach Jump with Cargo " MOD_VERSION "\r\n");
    log_text("target=DS2.exe Steam 1.10.89.0\r\n");

    if (!validate_build()) {
        log_text("status=UNSUPPORTED_GAME_BUILD\r\n");
        CloseHandle(g_log);
        g_log=INVALID_HANDLE_VALUE;
        return 3u;
    }

    const u32 enabled=read_enabled();
    if (enabled==0u) {
        log_text("status=DISABLED\r\n");
    } else if (enabled==1u) {
        install_hook();
    } else {
        log_text("status=INVALID_CONFIG\r\n");
    }

    CloseHandle(g_log);
    g_log=INVALID_HANDLE_VALUE;
    return 0u;
}

extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID) {
    if (reason==DLL_PROCESS_ATTACH) {
        g_self=module;
        DisableThreadLibraryCalls(module);
        HANDLE thread=CreateThread(0,0,worker_thread,0,0,0);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
