// Integration of the visually validated native footprint filter into Sam Overhaul.
// Uses the existing worker, INI, log and pinned ASI lifetime; no second DLL/thread.
#include "footprint_api.h"
#include "footprint_core.h"
#include <bcrypt.h>

namespace sam_footprints {
static std::atomic<unsigned> state{0};
static bool installed=false;
static bool firstBlockReported=false;
static bool invalidContextReported=false;

static void Log(void* handle,const char* text) {
    if(!handle || handle==INVALID_HANDLE_VALUE || !text)return;
    DWORD length=0,written=0;while(text[length])++length;
    WriteFile(handle,text,length,&written,nullptr);
}
static bool Equal(const unsigned char* a,const unsigned char* b,SIZE_T count) {
    for(SIZE_T i=0;i<count;++i)if(a[i]!=b[i])return false;
    return true;
}
bool ReadEnabled(const wchar_t* path,bool* invalid) {
    if(invalid)*invalid=false;
    if(!path || !path[0])return false;
    wchar_t value[16];
    const DWORD n=GetPrivateProfileStringW(L"Footprints",L"HideFootprints",L"0",value,16,path);
    if(n==1 && value[0]==L'0')return false;
    if(n==1 && value[0]==L'1')return true;
    if(invalid)*invalid=true;
    return false; // Empty/malformed/truncated configuration never enables the hook.
}
static bool HashMatchesTarget(const wchar_t* path) {
    static const unsigned char expected[32]={
        0xBF,0x3D,0x1C,0x66,0x55,0x45,0x93,0x0B,0xC8,0x50,0xD8,0xF5,0xDF,0x48,0x6F,0x73,
        0x95,0x88,0x5B,0xB7,0x29,0xD4,0xFD,0x40,0x8F,0xDB,0x03,0x39,0x0D,0xE0,0x76,0x5B};
    BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;
    HANDLE file=INVALID_HANDLE_VALUE,heap=GetProcessHeap();
    PUCHAR object=nullptr,buffer=nullptr;
    unsigned char digest[32];DWORD size=0,returned=0,read=0;
    bool ok=false;
    do {
        if(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)break;
        if(BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof size,&returned,0)<0 || !size || size>1048576)break;
        object=static_cast<PUCHAR>(HeapAlloc(heap,0,size));
        buffer=static_cast<PUCHAR>(HeapAlloc(heap,0,65536));
        if(!object || !buffer)break;
        if(BCryptCreateHash(alg,&hash,object,size,nullptr,0,0)<0)break;
        file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
            nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)break;
        bool readOk=true;
        for(;;) {
            if(!ReadFile(file,buffer,65536,&read,nullptr)){readOk=false;break;}
            if(!read)break;
            if(BCryptHashData(hash,buffer,read,0)<0){readOk=false;break;}
        }
        if(!readOk || BCryptFinishHash(hash,digest,sizeof digest,0)<0)break;
        ok=Equal(digest,expected,sizeof expected);
    }while(false);
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
    if(hash)BCryptDestroyHash(hash);
    if(object)HeapFree(heap,0,object);
    if(buffer)HeapFree(heap,0,buffer);
    if(alg)BCryptCloseAlgorithmProvider(alg,0);
    return ok;
}
static bool Error(void* log,unsigned code,const char* message) {
    state.store(code,std::memory_order_release);Log(log,message);return false;
}
}
extern "C" bool SamFootprintsInstall(void* image,const wchar_t* ini,void* log) {
    using namespace sam_footprints;
    if(installed)return true;
    bool invalidValue=false;
    if(!ReadEnabled(ini,&invalidValue)) {
        state.store(invalidValue?3u:1u,std::memory_order_release);
        Log(log,invalidValue?"footprints=DISABLED invalid_HideFootprints_value_using_0\r\n":
            "footprints=DISABLED HideFootprints=0\r\n");
        return true; // Existing/old INIs install no footprint hook and perform no hashing.
    }
    Log(log,"HideFootprints=1\r\n");
    if(GetModuleHandleW(L"ds2_footprint_native_probe.asi"))
        return Error(log,4,"footprints=REFUSED standalone_probe_already_loaded_remove_probe_and_restart\r\n");
    wchar_t path[1024];
    const DWORD n=GetModuleFileNameW(nullptr,path,1024);
    if(!n || n>=1024 || !HashMatchesTarget(path))
        return Error(log,5,"footprints=REFUSED executable_SHA256_not_supported\r\n");
    Log(log,"footprints_executable_sha256=PASS\r\n");
    // Guard both the native function and the source-to-target image identity.
    if(!image || image!=GetModuleHandleW(nullptr))
        return Error(log,6,"footprints=REFUSED image_base_mismatch\r\n");
    footprint::base=reinterpret_cast<uintptr_t>(image);
    unsigned char bytes[sizeof footprint::Prologue];
    if(!footprint::Read(footprint::base+footprint::AppendRva,bytes,sizeof bytes) ||
       !Equal(bytes,footprint::Prologue,sizeof bytes))
        return Error(log,6,"footprints=REFUSED function_context_mismatch_or_duplicate_hook\r\n");
    // MinHook is privately, statically linked into this one Sam Overhaul ASI.
    MH_STATUS status=MH_Initialize();
    if(status!=MH_OK)return Error(log,7,"footprints=REFUSED hook_library_initialization_failed\r\n");
    void* target=reinterpret_cast<void*>(footprint::base+footprint::AppendRva);
    status=MH_CreateHook(target,reinterpret_cast<void*>(&footprint::OnAppend),reinterpret_cast<void**>(&footprint::original));
    if(status!=MH_OK) {
        MH_Uninitialize();
        return Error(log,7,"footprints=REFUSED trampoline_creation_failed\r\n");
    }
    footprint::suppress.store(true,std::memory_order_release);
    status=MH_EnableHook(target);
    if(status!=MH_OK) {
        footprint::suppress.store(false,std::memory_order_release);
        // Keep the original trampoline/module lifetime if cleanup itself ever fails.
        if(MH_RemoveHook(target)==MH_OK)MH_Uninitialize();
        return Error(log,7,"footprints=REFUSED hook_enable_failed\r\n");
    }
    installed=true;
    state.store(2,std::memory_order_release);
    Log(log,"footprints=APPLIED append_rva=0x02250210 resources=FOOTPRINTS_ONLY\r\n");
    Log(log,"footprints_scope=NEW_AND_RELOADED ground_and_scan_visible save_files=UNMODIFIED ReShade=NOT_REQUIRED\r\n");
    return true;
}
extern "C" void SamFootprintsPoll(void* log) {
    using namespace sam_footprints;
    if(!installed)return;
    if(!firstBlockReported && footprint::blocked.load(std::memory_order_relaxed)) {
        firstBlockReported=true;Log(log,"footprints_runtime=FIRST_MATCH_BLOCKED\r\n");
    }
    if(!invalidContextReported && footprint::invalid.load(std::memory_order_relaxed)) {
        invalidContextReported=true;Log(log,"footprints_runtime=UNKNOWN_CONTEXT_PASSED_THROUGH\r\n");
    }
}
extern "C" __declspec(dllexport) unsigned SamOverhaulFootprintsState() {
    return sam_footprints::state.load(std::memory_order_acquire);
}