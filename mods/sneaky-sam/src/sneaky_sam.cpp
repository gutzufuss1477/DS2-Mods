// DS2 Sneaky Sam v1.0.0
// Target: DEATH STRANDING 2 v1.10.89.0
// Hides only baggage attached to RightArm, LeftArm, RightWaist and LeftWaist.
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef unsigned long DWORD;
typedef unsigned long long SIZE_T;
typedef int BOOL;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef FASTCALL
#define FASTCALL __fastcall
#endif
typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE_X64)(LPVOID);
__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(const wchar_t*);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE,wchar_t*,DWORD);
__declspec(dllimport) BOOL WINAPI GetModuleHandleExW(DWORD,const wchar_t*,HMODULE*);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HANDLE WINAPI CreateThread(LPVOID,SIZE_T,LPTHREAD_START_ROUTINE_X64,LPVOID,DWORD,DWORD*);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE);
__declspec(dllimport) void WINAPI Sleep(DWORD);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
__declspec(dllimport) BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,DWORD*);
__declspec(dllimport) LPVOID WINAPI VirtualAlloc(LPVOID,SIZE_T,DWORD,DWORD);
__declspec(dllimport) BOOL WINAPI VirtualFree(LPVOID,SIZE_T,DWORD);
__declspec(dllimport) BOOL WINAPI FlushInstructionCache(HANDLE,LPCVOID,SIZE_T);
__declspec(dllimport) HANDLE WINAPI CreateFileW(const wchar_t*,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,DWORD*,LPVOID);
__declspec(dllimport) HANDLE WINAPI CreateMutexW(LPVOID,BOOL,const wchar_t*);
__declspec(dllimport) DWORD WINAPI GetLastError(void);
}

static const DWORD DLL_PROCESS_ATTACH_VALUE=1u;
static const DWORD PAGE_EXECUTE_READ_VALUE=0x20u;
static const DWORD PAGE_EXECUTE_READWRITE_VALUE=0x40u;
static const DWORD MEM_COMMIT_VALUE=0x1000u;
static const DWORD MEM_RESERVE_VALUE=0x2000u;
static const DWORD MEM_RELEASE_VALUE=0x8000u;
static const DWORD GENERIC_WRITE_VALUE=0x40000000u;
static const DWORD FILE_SHARE_READ_VALUE=0x1u;
static const DWORD FILE_SHARE_WRITE_VALUE=0x2u;
static const DWORD CREATE_ALWAYS_VALUE=2u;
static const DWORD FILE_ATTRIBUTE_NORMAL_VALUE=0x80u;
static const DWORD ERROR_ALREADY_EXISTS_VALUE=183u;
static const u32 MAX_PATH_CHARS=1024u;

static const u32 EXPECTED_PE_TIMESTAMP=0x6A3DAE46u;
static const u32 EXPECTED_SIZE_OF_IMAGE=0x0B292000u;
static const u32 EXPECTED_PE_SIGNATURE=0x00004550u;
static const u32 BAGGAGE_VISIBILITY_CALLSITE_RVA=0x00F6C09Du;
static const u32 NATIVE_BAGGAGE_VISIBILITY_RVA=0x01194520u;

static u8* g_imageBase=0;
static u8* g_relay=0;
static HANDLE g_mutex=0;
static wchar_t g_logPath[MAX_PATH_CHARS];

typedef void (FASTCALL *BaggageVisibilityFn)(u8*,u8,u32);
static BaggageVisibilityFn g_originalVisibility=0;
static bool bytes_equal(const void* first,const void* second,SIZE_T count) {
    const u8* a=(const u8*)first; const u8* b=(const u8*)second;
    for (SIZE_T i=0;i<count;++i) if (a[i]!=b[i]) return false;
    return true;
}
static void copy_bytes(void* destination,const void* source,SIZE_T count) {
    u8* out=(u8*)destination; const u8* in=(const u8*)source;
    for (SIZE_T i=0;i<count;++i) out[i]=in[i];
}
static u32 ascii_length(const char* text) {
    if (!text) return 0u; u32 n=0u; while (text[n]) ++n; return n;
}
static void write_text(HANDLE file,const char* text) {
    if (!file || file==(HANDLE)(s64)-1 || !text) return;
    DWORD written=0u; const u32 n=ascii_length(text);
    if (n) WriteFile(file,text,n,&written,0);
}
static bool build_log_path() {
    DWORD n=GetModuleFileNameW((HMODULE)0,g_logPath,MAX_PATH_CHARS);
    if (!n || n>=MAX_PATH_CHARS) return false;
    u32 slash=0u;
    for (u32 i=0;i<n;++i) if (g_logPath[i]==L'\\' || g_logPath[i]==L'/') slash=i+1u;
    static const wchar_t name[]=L"ds2_sneaky_sam.log";
    u32 out=slash;
    for (u32 i=0u;name[i];++i) {
        if (out+1u>=MAX_PATH_CHARS) return false;
        g_logPath[out++]=name[i];
    }
    g_logPath[out]=0; return true;
}
static HANDLE open_log_create() {
    if (!build_log_path()) return (HANDLE)(s64)-1;
    return CreateFileW(g_logPath,GENERIC_WRITE_VALUE,FILE_SHARE_READ_VALUE|FILE_SHARE_WRITE_VALUE,0,
                       CREATE_ALWAYS_VALUE,FILE_ATTRIBUTE_NORMAL_VALUE,0);
}
static bool validate_pe() {
    if (!g_imageBase || *(volatile const u16*)g_imageBase!=0x5A4Du) return false;
    const u32 peOffset=*(volatile const u32*)(g_imageBase+0x3Cu);
    if (peOffset<0x40u || peOffset>0x1000u) return false;
    u8* nt=g_imageBase+peOffset;
    if (*(volatile const u32*)nt!=EXPECTED_PE_SIGNATURE) return false;
    return *(volatile const u32*)(nt+8u)==EXPECTED_PE_TIMESTAMP &&
           *(volatile const u32*)(nt+0x50u)==EXPECTED_SIZE_OF_IMAGE;
}
static bool rel32_fits(const u8* instructionNext,const u8* destination) {
    const s64 d=(s64)((u64)destination-(u64)instructionNext);
    return d>=(s64)-2147483648LL && d<=(s64)2147483647LL;
}
static void write_u64_le(u8* output,u64 value) {
    for (u32 i=0u;i<8u;++i) output[i]=(u8)(value>>(i*8u));
}
static void build_call_bytes(u8 output[5],const u8* callSite,const u8* destination) {
    const s64 difference=(s64)((u64)destination-(u64)(callSite+5u));
    const s32 displacement=(s32)difference;
    output[0]=0xE8u;
    output[1]=(u8)(displacement&0xFF);
    output[2]=(u8)((displacement>>8)&0xFF);
    output[3]=(u8)((displacement>>16)&0xFF);
    output[4]=(u8)((displacement>>24)&0xFF);
}
static u8* finalize_relay(u8* memory,u8* callSite,u8* destination) {
    if (!memory) return 0;
    if (!rel32_fits(callSite+5u,memory)) {
        VirtualFree(memory,0u,MEM_RELEASE_VALUE); return 0;
    }
    memory[0]=0x48u; memory[1]=0xB8u;
    write_u64_le(memory+2u,(u64)destination);
    memory[10]=0xFFu; memory[11]=0xE0u;
    FlushInstructionCache(GetCurrentProcess(),memory,12u);
    DWORD oldProtect=0u;
    if (!VirtualProtect(memory,0x1000u,PAGE_EXECUTE_READ_VALUE,&oldProtect)) return 0;
    return memory;
}
static u8* allocate_near_relay(u8* callSite,u8* destination) {
    const u64 granularity=0x10000ull;
    const u64 maxDistance=0x7FFF0000ull;
    const u64 imageEnd=(u64)g_imageBase+(u64)EXPECTED_SIZE_OF_IMAGE;
    const u64 first=(imageEnd+granularity-1ull)&~(granularity-1ull);
    u8* memory=(u8*)VirtualAlloc((LPVOID)first,0x1000u,MEM_RESERVE_VALUE|MEM_COMMIT_VALUE,
                                 PAGE_EXECUTE_READWRITE_VALUE);
    memory=finalize_relay(memory,callSite,destination);
    if (memory) return memory;
    const u64 aligned=((u64)callSite)&~(granularity-1ull);
    for (u64 delta=granularity;delta<=maxDistance;delta+=granularity) {
        const u64 candidates[2]={aligned+delta,aligned>delta?aligned-delta:0ull};
        for (u32 i=0u;i<2u;++i) {
            if (!candidates[i] || candidates[i]==first) continue;
            memory=(u8*)VirtualAlloc((LPVOID)candidates[i],0x1000u,
                                     MEM_RESERVE_VALUE|MEM_COMMIT_VALUE,
                                     PAGE_EXECUTE_READWRITE_VALUE);
            memory=finalize_relay(memory,callSite,destination);
            if (memory) return memory;
        }
    }
    return 0;
}
static bool write_code(u8* address,const u8* bytes,u32 count) {
    DWORD oldProtect=0u;
    if (!VirtualProtect(address,count,PAGE_EXECUTE_READWRITE_VALUE,&oldProtect)) return false;
    copy_bytes(address,bytes,count);
    FlushInstructionCache(GetCurrentProcess(),address,count);
    const bool ok=bytes_equal(address,bytes,count);
    DWORD ignored=0u;
    const bool restored=VirtualProtect(address,count,oldProtect,&ignored)!=0;
    return ok && restored;
}
static void FASTCALL sneaky_baggage_visibility(u8* baggage,u8 visible,u32 reason) {
    if (!g_originalVisibility) return;
    if (baggage) {
        const u8 slot=*(volatile const u8*)baggage;
        if (slot>=4u && slot<=7u) visible=0u;
    }
    g_originalVisibility(baggage,visible,reason);
}
static bool install_hook(HANDLE log) {
    u8* callSite=g_imageBase+BAGGAGE_VISIBILITY_CALLSITE_RVA;
    u8 expected[5];
    build_call_bytes(expected,callSite,g_imageBase+NATIVE_BAGGAGE_VISIBILITY_RVA);
    if (!bytes_equal(callSite,expected,5u)) {
        write_text(log,"status=CALLSITE_CONTEXT_REJECTED\r\n"); return false;
    }
    g_originalVisibility=(BaggageVisibilityFn)(g_imageBase+NATIVE_BAGGAGE_VISIBILITY_RVA);
    g_relay=allocate_near_relay(callSite,(u8*)&sneaky_baggage_visibility);
    if (!g_relay) {
        write_text(log,"status=NEAR_RELAY_ALLOCATION_FAILED\r\n"); return false;
    }
    u8 replacement[5];
    build_call_bytes(replacement,callSite,g_relay);
    if (!write_code(callSite,replacement,5u)) {
        write_text(log,"status=CALLSITE_PATCH_FAILED\r\n"); return false;
    }
    write_text(log,"hook=DSPlayerEquipmentManage visibility refresh\r\n");
    write_text(log,"callsite_rva=0x00F6C09D native_target_rva=0x01194520\r\n");
    write_text(log,"slots=4 RightArm,5 LeftArm,6 RightWaist,7 LeftWaist\r\n");
    return true;
}
static DWORD WINAPI worker_thread(LPVOID) {
    Sleep(1200u);
    g_imageBase=(u8*)GetModuleHandleW((const wchar_t*)0);
    HANDLE log=open_log_create();
    if (log==(HANDLE)(s64)-1) return 1u;
    write_text(log,"DS2 Sneaky Sam v1.0.0\r\n");
    write_text(log,"mode=HIDE_SHOULDER_AND_WAIST_BAGGAGE_VISUALS_ONLY\r\n");
    write_text(log,"target=DS2.exe v1.10.89.0\r\n");
    write_text(log,"expected_sha256=BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B\r\n");
    if (!validate_pe()) {
        write_text(log,"status=BASELINE_REJECTED\r\n"); CloseHandle(log); return 2u;
    }
    write_text(log,"baseline_check=PASS\r\n");
    HMODULE pinned=0;
    if (!GetModuleHandleExW(5u,(const wchar_t*)&worker_thread,&pinned)) {
        write_text(log,"status=MODULE_PIN_FAILED\r\n"); CloseHandle(log); return 3u;
    }
    if (!install_hook(log)) {
        CloseHandle(log); return 4u;
    }
    write_text(log,"status=PATCH_APPLIED\r\n");
    write_text(log,"inventory_weight_gameplay=UNMODIFIED\r\n");
    write_text(log,"stealth_detection=UNMODIFIED\r\n");
    CloseHandle(log);
    return 0u;
}
extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID) {
    if (reason==DLL_PROCESS_ATTACH_VALUE) {
        DisableThreadLibraryCalls(module);
        g_mutex=CreateMutexW(0,0,L"Local\\DS2_SneakySam_v1_0_0");
        if (!g_mutex) return 1;
        if (GetLastError()==ERROR_ALREADY_EXISTS_VALUE) {
            CloseHandle(g_mutex); g_mutex=0; return 1;
        }
        HANDLE thread=CreateThread(0,0,worker_thread,0,0,0);
        if (thread) CloseHandle(thread);
    }
    return 1;
}
