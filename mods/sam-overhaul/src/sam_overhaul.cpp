// DS2 Sam Overhaul v1.1.0-dev.28
// Target: DEATH STRANDING 2 v1.10.89.0
// Step 1: configurable visual hiding for backpack, shoulder and hip cargo.
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
__declspec(dllimport) DWORD WINAPI GetPrivateProfileStringW(const wchar_t*,const wchar_t*,const wchar_t*,wchar_t*,DWORD,const wchar_t*);
int _fltused=0;
__declspec(dllimport) unsigned int WINAPI GetPrivateProfileIntW(const wchar_t*,const wchar_t*,int,const wchar_t*);
}

#include "footprints/footprint_api.h"
extern "C" bool SamConstructionRangesInstall(void*,const wchar_t*,HANDLE);
extern "C" bool SamConstructionRefreshEnabled();
extern "C" void SamConstructionRefreshPoll(HANDLE);
extern "C" void SamConstructionRangesPollPending(HANDLE);
extern "C" bool SamConstructionShelterInstall(void*,const wchar_t*,HANDLE);
extern "C" bool SamConstructionShelterEnabled();
extern "C" void SamConstructionShelterPoll(HANDLE);
extern "C" bool SamConstructionRepairGateInstall(void*,const wchar_t*,HANDLE);
extern "C" void SamConstructionRepairGatePoll();
extern "C" bool SamShelterRestLabelConfigure(void*,const wchar_t*);
extern "C" bool SamShelterRestLabelEnabled();
extern "C" int SamShelterRestLabelOnStreamResource(void*);
extern "C" s32 SamShelterRestLabelChangedCount();
extern "C" void SamShelterRestLabelPoll(HANDLE);
extern "C" void SamShelterRestLabelNotifyActiveShelter();

static const DWORD DLL_PROCESS_ATTACH_VALUE=1u;
static const DWORD PAGE_READWRITE_VALUE=0x04u;
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
static const u32 BACKPACK_VISIBILITY_REFRESH_CALLSITE_RVA=0x00F6EB7Au;
static const u32 NATIVE_BAGGAGE_VISIBILITY_RVA=0x01194520u;
static const u32 BACKPACK_OUTLINE_SLOT_GATE_RVA=0x011FBE11u;
static const u32 BAGGAGE_OUTLINE_CHILD_FLAG_RVA=0x011FBE23u;
static const u32 MONORAIL_FINAL_BLOCK_CALL_RVA=0x0106FAFCu;
static const u32 SPARE_SHOES_VISIBILITY_BOOL_RVA=0x00F6EC75u;
static const u32 BACKPACK_BIND_EFFECT_STATE_READ_RVA=0x00DD48B3u;
static const u32 ZIPLINE_DETACH_ACTION_GATE_RVA=0x0103933Fu;
static const u32 LANDING_ROLL_TYPE_DECISION_RVA=0x0110CB80u;
static const u32 LANDING_CLASSIFIER_CALLSITE_RVA=0x0104A966u;
static const u32 NATIVE_LANDING_CLASSIFIER_RVA=0x0104C790u;
static const u32 LANDING_NO_BACKPACK_ANIM_WRITER_RVA=0x00E06D43u;

static u8* g_imageBase=0;
static u8* g_relay=0;
static u8* g_landingClassifierRelay=0;
static u8* g_landingAnimWriterCave=0;
static u8* g_autoDriveTimerCave=0;
static volatile u8 g_autoDriveHookExecuted=0;
#include "autodrive_timer_code.h"
#include "autodrive_settings.h"
static HANDLE g_mutex=0;
static wchar_t g_logPath[MAX_PATH_CHARS];
static wchar_t g_iniPath[MAX_PATH_CHARS];
static bool g_hideShoulderCargo=true;
static bool g_hideHipCargo=true;
static bool g_hideBackpackCargo=true;
static bool g_hideSpareShoes=true;
static bool g_jumpFromMonorailAnywhere=true;
static bool g_jumpFromZiplineAnywhere=true;
static bool g_landingRollWithBackpack=true;
static sam_autodrive::Settings g_autoDriveSettings={2.0f,2.5f,"DEFAULT_SECONDS",false,true};
static bool g_tuneHeavyMachineGun=true;
static bool g_tuneMortar=true;
static bool g_tuneChiralParticleCannon=true;
static bool g_tuneMissileLauncher=true;
static float g_heavyMachineGunRangeMultiplier=1.75f;
static float g_mortarRangeMultiplier=1.75f;
static float g_chiralParticleCannonRangeMultiplier=1.75f;
static float g_missileLauncherRangeMultiplier=1.75f;
static float g_heavyMachineGunAimSpeedMultiplier=2.50f;
static float g_mortarAimSpeedMultiplier=2.50f;
static float g_chiralParticleCannonAimSpeedMultiplier=2.50f;
static float g_missileLauncherAimSpeedMultiplier=2.50f;
static float g_heavyMachineGunLockOnSeconds=0.20f;
static float g_heavyMachineGunFireSeconds=1.50f;
static float g_heavyMachineGunFireIntervalSeconds=0.50f;
static float g_mortarLockOnSeconds=0.40f;
static float g_mortarFireEndSeconds=0.40f;
static float g_mortarFireIntervalSeconds=0.75f;
static float g_chiralParticleCannonLockOnSeconds=0.20f;
static float g_chiralParticleCannonFireEndSeconds=0.40f;
static float g_chiralParticleCannonFireIntervalSeconds=1.00f;
static float g_chiralParticleCannonChargeSeconds=0.75f;
static float g_missileLauncherLockOnSeconds=0.40f;
static float g_missileLauncherFireEndSeconds=0.40f;
static float g_missileLauncherFireIntervalSeconds=0.75f;

typedef void (FASTCALL *BaggageVisibilityFn)(u8*,u8,u32);
typedef u32 (FASTCALL *LandingClassifierFn)(u8*,u64);
static BaggageVisibilityFn g_originalVisibility=0;
static LandingClassifierFn g_originalLandingClassifier=0;
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
    static const wchar_t name[]=L"ds2_sam_overhaul.log";
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
static bool build_ini_path() {
    DWORD n=GetModuleFileNameW((HMODULE)0,g_iniPath,MAX_PATH_CHARS);
    if (!n || n>=MAX_PATH_CHARS) return false;
    u32 slash=0u;
    for (u32 i=0;i<n;++i) if (g_iniPath[i]==L'\\' || g_iniPath[i]==L'/') slash=i+1u;
    static const wchar_t name[]=L"ds2_sam_overhaul.ini";
    u32 out=slash;
    for (u32 i=0u;name[i];++i) {
        if (out+1u>=MAX_PATH_CHARS) return false;
        g_iniPath[out++]=name[i];
    }
    g_iniPath[out]=0;
    return true;
}
#include "autodrive_config_io.inl"
#include "truck_weapon_config.inl"
static void load_config() {
    if (!build_ini_path()) return;
    g_hideShoulderCargo=GetPrivateProfileIntW(L"CargoVisibility",L"HideShoulderCargo",1,g_iniPath)!=0u;
    g_hideHipCargo=GetPrivateProfileIntW(L"CargoVisibility",L"HideHipCargo",1,g_iniPath)!=0u;
    g_hideBackpackCargo=GetPrivateProfileIntW(L"CargoVisibility",L"HideBackpackCargo",1,g_iniPath)!=0u;
    g_hideSpareShoes=GetPrivateProfileIntW(L"CargoVisibility",L"HideSpareShoes",1,g_iniPath)!=0u;
    g_jumpFromMonorailAnywhere=GetPrivateProfileIntW(L"Movement",L"JumpFromMonorailAnywhere",1,g_iniPath)!=0u;
    g_jumpFromZiplineAnywhere=GetPrivateProfileIntW(L"Movement",L"JumpFromZiplineAnywhere",1,g_iniPath)!=0u;
    g_landingRollWithBackpack=GetPrivateProfileIntW(L"Movement",L"LandingRollWithBackpack",1,g_iniPath)!=0u;
    load_autodrive_config();
    load_truck_weapon_config();
}
#include "truck_weapon_streaming.inl"
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
static void build_jump_bytes(u8 output[5],const u8* jumpSite,const u8* destination) {
    const s64 difference=(s64)((u64)destination-(u64)(jumpSite+5u));
    const s32 displacement=(s32)difference;
    output[0]=0xE9u;
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
                                 PAGE_READWRITE_VALUE);
    memory=finalize_relay(memory,callSite,destination);
    if (memory) return memory;
    const u64 aligned=((u64)callSite)&~(granularity-1ull);
    for (u64 delta=granularity;delta<=maxDistance;delta+=granularity) {
        const u64 candidates[2]={aligned+delta,aligned>delta?aligned-delta:0ull};
        for (u32 i=0u;i<2u;++i) {
            if (!candidates[i] || candidates[i]==first) continue;
            memory=(u8*)VirtualAlloc((LPVOID)candidates[i],0x1000u,
                                     MEM_RESERVE_VALUE|MEM_COMMIT_VALUE,
                                     PAGE_READWRITE_VALUE);
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
static u32 FASTCALL sam_overhaul_landing_classifier(u8* context,u64 mode) {
    if (!g_originalLandingClassifier) return 8u;
    if (!g_landingRollWithBackpack || !context) return g_originalLandingClassifier(context,mode);
    u8* player=*(u8**)(context+0xA8u);
    if (!player) return g_originalLandingClassifier(context,mode);

    volatile u32* flags=(volatile u32*)(player+0x5F4u);
    const u32 savedFlags=*flags;
    *flags=(savedFlags | 0x20000000u) & ~0x40000000u;

    u8* carrier=*(u8**)(player+0x1340u);
    u32 savedCarrierC8=0u;
    if (carrier) {
        volatile u32* c8Bits=(volatile u32*)(carrier+0xC8u);
        savedCarrierC8=*c8Bits;
        *c8Bits=0xBF800000u;
    }

    const u32 result=g_originalLandingClassifier(context,mode);
    if (carrier) *(volatile u32*)(carrier+0xC8u)=savedCarrierC8;
    *flags=savedFlags;
    return result;
}
static void FASTCALL sam_overhaul_baggage_visibility(u8* baggage,u8 visible,u32 reason) {
    if (!g_originalVisibility) return;
    if (baggage) {
        const u8 slot=*(volatile const u8*)baggage;
        const bool hideBackpack=(slot==1u && g_hideBackpackCargo);
        const bool hideShoulder=((slot==4u || slot==5u) && g_hideShoulderCargo);
        const bool hideHip=((slot==6u || slot==7u) && g_hideHipCargo);
        if (hideBackpack || hideShoulder || hideHip) visible=0u;
    }
    g_originalVisibility(baggage,visible,reason);
}
static bool install_hidden_cargo_outline_patch(HANDLE log) {
    if (!g_hideBackpackCargo && !g_hideSpareShoes) {
        write_text(log,"hidden_cargo_outline_patch=DISABLED\r\n");
        return true;
    }

    u8* gate=g_imageBase+BACKPACK_OUTLINE_SLOT_GATE_RVA;
    static const u8 expected[16]={
        0x3C,0x04,0x74,0x0C,0x3C,0x06,0x74,0x08,
        0x3C,0x05,0x74,0x04,0x3C,0x07,0x75,0x05
    };
    // Equivalent logic: slot == 1 OR slot in [4..7] -> vanilla "no baggage outline" path.
    static const u8 replacement[16]={
        0x3C,0x01,0x74,0x0C,0x2C,0x04,0x3C,0x03,
        0x76,0x06,0xEB,0x09,0x90,0x90,0x90,0x90
    };

    if (!bytes_equal(gate,expected,16u)) {
        write_text(log,"status=OUTLINE_GATE_CONTEXT_REJECTED\r\n");
        return false;
    }
    if (!write_code(gate,replacement,16u)) {
        write_text(log,"status=OUTLINE_GATE_PATCH_FAILED\r\n");
        return false;
    }

    u8* childFlag=g_imageBase+BAGGAGE_OUTLINE_CHILD_FLAG_RVA;
    static const u8 childExpected[3]={0x41,0xB5,0x01}; // mov r13b,1
    static const u8 childReplacement[3]={0x45,0x32,0xED}; // xor r13b,r13b
    if (!bytes_equal(childFlag,childExpected,3u)) {
        write_text(log,"status=OUTLINE_CHILD_CONTEXT_REJECTED\r\n");
        return false;
    }
    if (!write_code(childFlag,childReplacement,3u)) {
        write_text(log,"status=OUTLINE_CHILD_PATCH_FAILED\r\n");
        return false;
    }

    write_text(log,"hidden_cargo_outline_patch=APPLIED\r\n");
    write_text(log,"outline_gate_rva=0x011FBE11 slots=1,4,5,6,7\r\n");
    write_text(log,"outline_child_flag_rva=0x011FBE23 forced=0\r\n");
    return true;
}

static bool install_backpack_bind_effect_patch(HANDLE log) {
    if (!g_hideBackpackCargo) {
        write_text(log,"backpack_bind_effect_patch=DISABLED\r\n");
        return true;
    }

    u8* address=g_imageBase+BACKPACK_BIND_EFFECT_STATE_READ_RVA;
    static const u8 expected[3]={0x8B,0x43,0x54}; // mov eax,[rbx+54h]
    static const u8 replacement[3]={0x33,0xC0,0x90}; // xor eax,eax ; nop

    if (!bytes_equal(address,expected,3u)) {
        write_text(log,"status=BACKPACK_BIND_EFFECT_CONTEXT_REJECTED\r\n");
        return false;
    }
    if (!write_code(address,replacement,3u)) {
        write_text(log,"status=BACKPACK_BIND_EFFECT_PATCH_FAILED\r\n");
        return false;
    }

    write_text(log,"backpack_bind_effect_patch=APPLIED\r\n");
    write_text(log,"bind_effect_update_rva=0x00DD47B0 state_read_rva=0x00DD48B3 forced_state=0\r\n");
    write_text(log,"high_stack_bind_meshes=HIDDEN\r\n");
    return true;
}

static bool install_spare_shoes_visibility_patch(HANDLE log) {
    if (!g_hideSpareShoes) {
        write_text(log,"spare_shoes_visibility_patch=DISABLED\r\n");
        return true;
    }

    u8* address=g_imageBase+SPARE_SHOES_VISIBILITY_BOOL_RVA;
    static const u8 expected[3]={0x0F,0x94,0xC2}; // sete dl
    static const u8 replacement[3]={0x31,0xD2,0x90}; // xor edx,edx ; nop

    if (!bytes_equal(address,expected,3u)) {
        write_text(log,"status=SPARE_SHOES_CONTEXT_REJECTED\r\n");
        return false;
    }
    if (!write_code(address,replacement,3u)) {
        write_text(log,"status=SPARE_SHOES_PATCH_FAILED\r\n");
        return false;
    }

    write_text(log,"spare_shoes_visibility_patch=APPLIED\r\n");
    write_text(log,"equipment_refresh_rva=0x00F6E580 spare_shoes_slot=15 visibility_bool_rva=0x00F6EC75\r\n");
    write_text(log,"worn_shoes_slot_14=UNMODIFIED\r\n");
    return true;
}

static bool install_zipline_jump_patch(HANDLE log) {
    if (!g_jumpFromZiplineAnywhere) {
        write_text(log,"zipline_jump_patch=DISABLED\r\n");
        return true;
    }

    u8* detachGate=g_imageBase+ZIPLINE_DETACH_ACTION_GATE_RVA;
    static const u8 expected[6]={0x0F,0x84,0x74,0xF5,0xFF,0xFF}; // je back to ride loop if detach bit 0x20 is clear
    static const u8 replacement[6]={0x90,0x90,0x90,0x90,0x90,0x90};

    if (!bytes_equal(detachGate,expected,6u)) {
        write_text(log,"status=ZIPLINE_DETACH_GATE_CONTEXT_REJECTED\r\n");
        return false;
    }
    if (!write_code(detachGate,replacement,6u)) {
        write_text(log,"status=ZIPLINE_DETACH_GATE_PATCH_FAILED\r\n");
        return false;
    }

    write_text(log,"zipline_jump_patch=APPLIED\r\n");
    write_text(log,"zipline_action_plugin_update_rva=0x010385C0\r\n");
    write_text(log,"zipline_detach_flag_test_rva=0x01039338 flag=0x20\r\n");
    write_text(log,"zipline_detach_gate_rva=0x0103933F bypassed=TRUE\r\n");
    write_text(log,"zipline_input_action=0xC0 detach_handler_rva=0x01038100 animation=VANILLA\r\n");
    return true;
}

static bool install_landing_roll_backpack_patch(HANDLE log) {
    if (!g_landingRollWithBackpack) {
        write_text(log,"landing_roll_backpack_patch=DISABLED\r\n");
        return true;
    }

    u8* decision=g_imageBase+LANDING_ROLL_TYPE_DECISION_RVA;
    u8* classifierCall=g_imageBase+LANDING_CLASSIFIER_CALLSITE_RVA;
    u8* animWriter=g_imageBase+LANDING_NO_BACKPACK_ANIM_WRITER_RVA;
    static const u8 decisionExpected[6]={0x40,0x53,0x48,0x83,0xEC,0x20};
    static const u8 decisionReplacement[6]={0xB0,0x01,0xC3,0x90,0x90,0x90};
    static const u8 animWriterExpected[7]={0x0F,0xB6,0x82,0x50,0x2C,0x00,0x00};
    u8 classifierExpected[5];
    build_call_bytes(classifierExpected,classifierCall,g_imageBase+NATIVE_LANDING_CLASSIFIER_RVA);

    if (!bytes_equal(decision,decisionExpected,6u) ||
        !bytes_equal(classifierCall,classifierExpected,5u) ||
        !bytes_equal(animWriter,animWriterExpected,7u)) {
        write_text(log,"status=LANDING_ROLL_CONTEXT_REJECTED\r\n");
        return false;
    }

    g_originalLandingClassifier=(LandingClassifierFn)(g_imageBase+NATIVE_LANDING_CLASSIFIER_RVA);
    g_landingClassifierRelay=allocate_near_relay(classifierCall,(u8*)&sam_overhaul_landing_classifier);
    if (!g_landingClassifierRelay) {
        write_text(log,"status=LANDING_CLASSIFIER_RELAY_FAILED\r\n");
        return false;
    }

    // Test24: the central DSPlayerState animation writer normally derives bit 0x10 at
    // anim+0x2C50 from the physical no-backpack predicate. A/B runtime capture proved that
    // the vanilla roll animation is represented by anim+0x3740 == 2.0f. Use that actual
    // animation state as the gate, because player-context +0x34C is cleared before this
    // central animation writer executes.
    g_landingAnimWriterCave=allocate_near_relay(animWriter,animWriter);
    if (!g_landingAnimWriterCave ||
        !rel32_fits(g_landingAnimWriterCave+26u,animWriter+7u)) {
        write_text(log,"status=LANDING_ANIM_CAVE_ALLOCATION_FAILED\r\n");
        return false;
    }
    u8 cave[26]={
        0x81,0xBA,0x40,0x37,0x00,0x00,0x00,0x00,0x00,0x40, // cmp dword ptr [rdx+3740h],40000000h
        0x75,0x02,                                           // jne original read
        0xB1,0x10,                                           // mov cl,10h (NoBackpack anim bit)
        0x0F,0xB6,0x82,0x50,0x2C,0x00,0x00,                 // original: movzx eax,byte ptr [rdx+2C50h]
        0x00,0x00,0x00,0x00,0x00                            // jmp back -> RVA E06D4A
    };
    build_jump_bytes(cave+21u,g_landingAnimWriterCave+21u,animWriter+7u);
    if (!write_code(g_landingAnimWriterCave,cave,26u)) {
        write_text(log,"status=LANDING_ANIM_CAVE_WRITE_FAILED\r\n");
        return false;
    }

    u8 classifierReplacement[5];
    build_call_bytes(classifierReplacement,classifierCall,g_landingClassifierRelay);
    if (!write_code(classifierCall,classifierReplacement,5u)) {
        write_text(log,"status=LANDING_CLASSIFIER_HOOK_FAILED\r\n");
        return false;
    }
    if (!write_code(decision,decisionReplacement,6u)) {
        write_text(log,"status=LANDING_ROLL_PATCH_FAILED\r\n");
        return false;
    }

    u8 animWriterReplacement[7];
    build_jump_bytes(animWriterReplacement,animWriter,g_landingAnimWriterCave);
    animWriterReplacement[5]=0x90u;
    animWriterReplacement[6]=0x90u;
    if (!write_code(animWriter,animWriterReplacement,7u)) {
        write_text(log,"status=LANDING_ANIM_WRITER_HOOK_FAILED\r\n");
        return false;
    }

    write_text(log,"landing_roll_backpack_patch=APPLIED\r\n");
    write_text(log,"landing_classifier_callsite_rva=0x0104A966 no_backpack_context=TEMPORARY\r\n");
    write_text(log,"landing_roll_type_decision_rva=0x0110CB80 forced_result=TRUE\r\n");
    write_text(log,"landing_no_backpack_anim_writer_rva=0x00E06D43 hook=APPLIED\r\n");
    write_text(log,"landing_no_backpack_anim_bit=0x10 target_offset=0x2C50 gate=anim_0x3740_equals_2.0\r\n");
    write_text(log,"player_flags_and_carrier_values=RESTORED_AFTER_CLASSIFIER_CALL\r\n");
    write_text(log,"height_thresholds=PRESERVED fall_damage=PRESERVED normal_backpack_animation=PRESERVED_OUTSIDE_ROLL_LANDING\r\n");
    return true;
}

#include "autodrive_install.inl"

static bool install_monorail_jump_patch(HANDLE log) {
    if (!g_jumpFromMonorailAnywhere) {
        write_text(log,"monorail_jump_patch=DISABLED\r\n");
        return true;
    }

    u8* callSite=g_imageBase+MONORAIL_FINAL_BLOCK_CALL_RVA;
    u8 expected[5];
    build_call_bytes(expected,callSite,g_imageBase+0x00FE3E90u);
    if (!bytes_equal(callSite,expected,5u)) {
        write_text(log,"status=MONORAIL_CONTEXT_REJECTED\r\n");
        return false;
    }

    // Preserve every earlier CanJumpDown state check.
    // Only replace the final safety/height blocker result with false:
    // xor eax,eax ; nop ; nop ; nop
    static const u8 replacement[5]={0x31,0xC0,0x90,0x90,0x90};
    if (!write_code(callSite,replacement,5u)) {
        write_text(log,"status=MONORAIL_PATCH_FAILED\r\n");
        return false;
    }

    write_text(log,"monorail_jump_patch=APPLIED\r\n");
    write_text(log,"monorail_can_jump_rva=0x0106F9A0 final_block_call_rva=0x0106FAFC\r\n");
    write_text(log,"monorail_state_checks=PRESERVED final_safety_height_block=BYPASSED\r\n");
    return true;
}

static bool install_hook(HANDLE log) {
    u8* callSite=g_imageBase+BAGGAGE_VISIBILITY_CALLSITE_RVA;
    u8* backpackRefreshCall=g_imageBase+BACKPACK_VISIBILITY_REFRESH_CALLSITE_RVA;
    u8 expected[5];
    build_call_bytes(expected,callSite,g_imageBase+NATIVE_BAGGAGE_VISIBILITY_RVA);
    if (!bytes_equal(callSite,expected,5u)) {
        write_text(log,"status=CALLSITE_CONTEXT_REJECTED\r\n"); return false;
    }
    g_originalVisibility=(BaggageVisibilityFn)(g_imageBase+NATIVE_BAGGAGE_VISIBILITY_RVA);
    g_relay=allocate_near_relay(callSite,(u8*)&sam_overhaul_baggage_visibility);
    if (!g_relay) {
        write_text(log,"status=NEAR_RELAY_ALLOCATION_FAILED\r\n"); return false;
    }
    u8 backpackRefreshExpected[5];
    build_call_bytes(backpackRefreshExpected,backpackRefreshCall,g_imageBase+NATIVE_BAGGAGE_VISIBILITY_RVA);
    if (!bytes_equal(backpackRefreshCall,backpackRefreshExpected,5u)) {
        write_text(log,"status=BACKPACK_REFRESH_CONTEXT_REJECTED\r\n"); return false;
    }

    u8 replacement[5];
    build_call_bytes(replacement,callSite,g_relay);
    if (!write_code(callSite,replacement,5u)) {
        write_text(log,"status=CALLSITE_PATCH_FAILED\r\n"); return false;
    }
    u8 backpackRefreshReplacement[5];
    build_call_bytes(backpackRefreshReplacement,backpackRefreshCall,g_relay);
    if (!write_code(backpackRefreshCall,backpackRefreshReplacement,5u)) {
        write_text(log,"status=BACKPACK_REFRESH_HOOK_FAILED\r\n"); return false;
    }
    write_text(log,"hook=DSPlayerEquipmentManage visibility refresh\r\n");
    write_text(log,"callsite_rva=0x00F6C09D native_target_rva=0x01194520\r\n");
    write_text(log,"backpack_refresh_callsite_rva=0x00F6EB7A routed_through_visibility_wrapper=TRUE\r\n");
    write_text(log,"slots=1 Backpack,4 RightArm,5 LeftArm,6 RightWaist,7 LeftWaist\r\n");
    if (!install_hidden_cargo_outline_patch(log)) return false;
    if (!install_backpack_bind_effect_patch(log)) return false;
    if (!install_spare_shoes_visibility_patch(log)) return false;
    if (!install_zipline_jump_patch(log)) return false;
    if (!install_landing_roll_backpack_patch(log)) return false;
    if (!install_autodrive_activation_patch(log)) return false;
    if (!install_monorail_jump_patch(log)) return false;
    return true;
}
static DWORD WINAPI worker_thread(LPVOID) {
    Sleep(1200u);
    g_imageBase=(u8*)GetModuleHandleW((const wchar_t*)0);
    HANDLE log=open_log_create();
    if (log==(HANDLE)(s64)-1) return 1u;
    write_text(log,"DS2 Sam Overhaul v1.1.0-dev.28\r\n");
    write_text(log,"step=1 CARGO_VISIBILITY\r\n");
    load_config();
    write_text(log,g_hideShoulderCargo ? "HideShoulderCargo=1\r\n" : "HideShoulderCargo=0\r\n");
    write_text(log,g_hideHipCargo ? "HideHipCargo=1\r\n" : "HideHipCargo=0\r\n");
    write_text(log,g_hideBackpackCargo ? "HideBackpackCargo=1\r\n" : "HideBackpackCargo=0\r\n");
    write_text(log,g_hideSpareShoes ? "HideSpareShoes=1\r\n" : "HideSpareShoes=0\r\n");
    write_text(log,g_jumpFromMonorailAnywhere ? "JumpFromMonorailAnywhere=1\r\n" : "JumpFromMonorailAnywhere=0\r\n");
    write_text(log,g_jumpFromZiplineAnywhere ? "JumpFromZiplineAnywhere=1\r\n" : "JumpFromZiplineAnywhere=0\r\n");
    write_text(log,g_landingRollWithBackpack ? "LandingRollWithBackpack=1\r\n" : "LandingRollWithBackpack=0\r\n");
    write_text(log,g_tuneHeavyMachineGun ? "EnableHeavyMachineGunTuning=1\r\n" : "EnableHeavyMachineGunTuning=0\r\n");
    write_text(log,g_tuneMortar ? "EnableMortarTuning=1\r\n" : "EnableMortarTuning=0\r\n");
    write_text(log,g_tuneChiralParticleCannon ? "EnableChiralParticleCannonTuning=1\r\n" : "EnableChiralParticleCannonTuning=0\r\n");
    write_text(log,g_tuneMissileLauncher ? "EnableMissileLauncherTuning=1\r\n" : "EnableMissileLauncherTuning=0\r\n");
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
    const bool restLabelEnabled=SamShelterRestLabelConfigure(g_imageBase,g_iniPath);
    write_text(log,restLabelEnabled ?
       "shelter_rest_label=STREAMING_LISTENER_ENABLED exact_two_UUIDs German_only\r\n" :
       "shelter_rest_label=DISABLED\r\n");
    if (!install_truck_weapon_streaming_listener(log)) {
        write_text(log,"truck_weapon_tuning=WARNING listener_not_active base_mod_continues\r\n");
    }
    if (!SamFootprintsInstall(g_imageBase,g_iniPath,log)) {
        write_text(log,"footprints=WARNING filter_not_active base_mod_continues\r\n");
    }
    if (!SamConstructionRangesInstall(g_imageBase,g_iniPath,log)) {
        write_text(log,"construction_ranges=WARNING hook_not_active base_mod_continues\r\n");
    }
    if(!SamConstructionShelterInstall(g_imageBase,g_iniPath,log)) {
        write_text(log,"construction_shelter=WARNING disabled_or_hook_install_failed\r\n");
    }
    if(SamConstructionShelterEnabled() &&
       !SamConstructionRepairGateInstall(g_imageBase,g_iniPath,log)){
        write_text(log,"construction_repair_gate=WARNING scoped_gate_not_active\r\n");
    }
    write_text(log,"status=PATCH_APPLIED\r\n");
    write_text(log,"inventory_weight_gameplay=UNMODIFIED\r\n");
    write_text(log,"stealth_detection=UNMODIFIED\r\n");
    if (g_autoDriveSettings.enabled) {
        write_text(log,"autodrive_timer_hook=WAITING_FOR_ELIGIBLE_DRIVING\r\n");
        while (!g_autoDriveHookExecuted) { SamFootprintsPoll(log); SamConstructionRangesPollPending(log); SamConstructionRefreshPoll(log); SamConstructionShelterPoll(log); SamConstructionRepairGatePoll(); SamShelterRestLabelPoll(log); Sleep(250u); }
        write_text(log,"autodrive_timer_hook=EXECUTED\r\n");
    }
    if (SamConstructionRefreshEnabled() || SamConstructionShelterEnabled()) {
        write_text(log,"construction_physics_refresh=WORKER_ACTIVE\r\n");
        while (SamConstructionRefreshEnabled() || SamConstructionShelterEnabled()) {
            SamConstructionRangesPollPending(log);
            SamConstructionRefreshPoll(log);
            SamConstructionShelterPoll(log);
            SamConstructionRepairGatePoll();
            SamShelterRestLabelPoll(log);
            SamFootprintsPoll(log);
            Sleep(250u);
        }
    }
    CloseHandle(log);
    return 0u;
}
extern "C" __declspec(dllexport) void InitializeASI() {}
extern "C" __declspec(dllexport) const char* SamOverhaulVersion() { return "1.1.0-dev.28"; }

extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID) {
    if (reason==DLL_PROCESS_ATTACH_VALUE) {
        DisableThreadLibraryCalls(module);
        g_mutex=CreateMutexW(0,0,L"Local\\DS2_SamOverhaul_v1_0_0");
        if (!g_mutex) return 1;
        if (GetLastError()==ERROR_ALREADY_EXISTS_VALUE) {
            CloseHandle(g_mutex); g_mutex=0; return 1;
        }
        HANDLE thread=CreateThread(0,0,worker_thread,0,0,0);
        if (thread) CloseHandle(thread);
    }
    return 1;
}
