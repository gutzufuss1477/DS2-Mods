#include "windows.h"

extern "C" void* memcpy(void* destination,const void* source,SIZE_T count){
    u8* out=(u8*)destination;
    const u8* in=(const u8*)source;
    for(SIZE_T i=0;i<count;i++)out[i]=in[i];
    return destination;
}

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u
#define SIGNATURE_RVA 0x00FA4247ull
#define PATCH_RVA 0x00FA4255ull

static HMODULE g_self = 0;
static HANDLE g_log = INVALID_HANDLE_VALUE;
static bool g_debugLog = false;

static const u8 EXPECTED_SIGNATURE[22] = {
    0x49,0x0F,0x44,0xC0,
    0x48,0x8B,0x00,
    0x44,0x38,0x88,0xE7,0x01,0x00,0x00,
    0x74,0x06,
    0xB8,0x06,0x00,0x00,0x00,
    0xC3
};

static const u8 PATCHED_SIGNATURE[22] = {
    0x49,0x0F,0x44,0xC0,
    0x48,0x8B,0x00,
    0x44,0x38,0x88,0xE7,0x01,0x00,0x00,
    0xEB,0x06,
    0xB8,0x06,0x00,0x00,0x00,
    0xC3
};

static const u8 ORIGINAL_PATCH[1] = {0x74};
static const u8 REPLACEMENT_PATCH[1] = {0xEB};

static bool bytesEqual(const u8* left,const u8* right,u32 count){
    if(!left||!right)return false;
    for(u32 i=0;i<count;i++)if(left[i]!=right[i])return false;
    return true;
}

static u32 readU32(const u8* source){
    return (u32)source[0]|((u32)source[1]<<8)|((u32)source[2]<<16)|((u32)source[3]<<24);
}

static u16 readU16(const u8* source){
    return (u16)((u16)source[0]|((u16)source[1]<<8));
}

static void modulePath(WCHAR* out,const WCHAR* name){
    if(!out||!name)return;
    out[0]=0;
    DWORD length=GetModuleFileNameW(g_self,out,520);
    if(!length||length>=520){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<length;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1;
    DWORD j=0;
    while(name[j]&&slash+j+1<520){out[slash+j]=name[j];j++;}
    if(name[j]){out[0]=0;return;}
    out[slash+j]=0;
}

static bool boolIni(const WCHAR* key,bool fallback){
    WCHAR path[520],value[32];
    modulePath(path,L"ds2_weapons_anywhere.ini");
    if(!path[0])return fallback;
    value[0]=0;
    GetPrivateProfileStringW(L"WeaponsAnywhere",key,fallback?L"1":L"0",value,32,path);
    return !(value[0]==L'0'||value[0]==L'n'||value[0]==L'N'||value[0]==L'f'||value[0]==L'F');
}

static WCHAR lowerAscii(WCHAR value){
    return value>=L'A'&&value<=L'Z'?(WCHAR)(value+(L'a'-L'A')):value;
}

static bool isDs2Process(HMODULE game){
    WCHAR path[520];
    DWORD length=GetModuleFileNameW(game,path,520);
    if(!length||length>=520)return false;
    DWORD start=0;
    for(DWORD i=0;i<length;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1;
    const WCHAR wanted[]=L"ds2.exe";
    DWORD j=0;
    while(wanted[j]){
        if(start+j>=length||lowerAscii(path[start+j])!=wanted[j])return false;
        j++;
    }
    return start+j==length;
}

static void logRaw(const char* text,DWORD length){
    if(!g_debugLog||g_log==INVALID_HANDLE_VALUE||!text||!length)return;
    DWORD written=0;
    WriteFile(g_log,text,length,&written,0);
    FlushFileBuffers(g_log);
}

static void openDebugLog(){
    if(!g_debugLog)return;
    WCHAR path[520];
    modulePath(path,L"ds2_weapons_anywhere.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Weapons Anywhere v" MOD_VERSION "\r\n"
            "Expected target: Steam PC 1.10.89.0\r\n"
            "Policy: skip only prohibited-area return reason 6; preserve warning presentation.\r\n";
        logRaw(header,(DWORD)(sizeof(header)-1));
    }
}

static void closeDebugLog(){
    if(g_log==INVALID_HANDLE_VALUE)return;
    FlushFileBuffers(g_log);
    CloseHandle(g_log);
    g_log=INVALID_HANDLE_VALUE;
}

static void logCritical(const char* text,DWORD length){
    if(g_log==INVALID_HANDLE_VALUE){
        g_debugLog=true;
        openDebugLog();
    }
    logRaw(text,length);
}

static bool readBuildMetadata(const u8* base,u32* timestamp,u32* imageSize){
    if(!base||!timestamp||!imageSize)return false;
    if(readU16(base)!=0x5A4Du)return false;
    u32 pe=readU32(base+0x3C);
    if(pe<0x40||pe>=0x2000||readU32(base+pe)!=0x00004550u)return false;
    if(readU16(base+pe+4)!=0x8664u)return false;
    if(readU16(base+pe+20)<0x00F0u||readU16(base+pe+24)!=0x020Bu)return false;
    *timestamp=readU32(base+pe+8);
    *imageSize=readU32(base+pe+0x50);
    if(SIGNATURE_RVA+sizeof(EXPECTED_SIGNATURE)>(u64)*imageSize||
       PATCH_RVA+sizeof(ORIGINAL_PATCH)>(u64)*imageSize)return false;
    return true;
}

static bool writePatchTransactional(u8* target,const u8* signature){
    if(!target||!signature)return false;
    DWORD oldProtection=0;
    if(!VirtualProtect(target,1,PAGE_EXECUTE_READWRITE,&oldProtection)){
        const char message[]="ERROR: could not make the patch site writable; no byte was changed.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        return false;
    }

    ((volatile u8*)target)[0]=REPLACEMENT_PATCH[0];
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),target,1);
    bool verified=bytesEqual(signature,PATCHED_SIGNATURE,(u32)sizeof(PATCHED_SIGNATURE));

    if(!flushed||!verified){
        ((volatile u8*)target)[0]=ORIGINAL_PATCH[0];
        BOOL rollbackFlushed=FlushInstructionCache(GetCurrentProcess(),target,1);
        bool rollbackVerified=bytesEqual(signature,EXPECTED_SIGNATURE,(u32)sizeof(EXPECTED_SIGNATURE));
        DWORD ignored=0;
        BOOL protectionRestored=VirtualProtect(target,1,oldProtection,&ignored);
        if(rollbackFlushed&&rollbackVerified&&protectionRestored){
            const char message[]="ERROR: patch verification failed; original byte and page protection restored.\r\n";
            logRaw(message,(DWORD)(sizeof(message)-1));
        }else{
            const char message[]="CRITICAL: patch verification failed and rollback was incomplete; stop DS2.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1));
        }
        return false;
    }

    DWORD ignored=0;
    if(VirtualProtect(target,1,oldProtection,&ignored))return true;

    DWORD rollbackProtection=0;
    if(!VirtualProtect(target,1,PAGE_EXECUTE_READWRITE,&rollbackProtection)){
        const char message[]="CRITICAL: page-protection restore failed and the patch site could not be reopened for rollback; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        return false;
    }
    ((volatile u8*)target)[0]=ORIGINAL_PATCH[0];
    BOOL rollbackFlushed=FlushInstructionCache(GetCurrentProcess(),target,1);
    bool rollbackVerified=bytesEqual(signature,EXPECTED_SIGNATURE,(u32)sizeof(EXPECTED_SIGNATURE));
    BOOL protectionRecovered=VirtualProtect(target,1,oldProtection,&ignored);
    if(rollbackFlushed&&rollbackVerified&&protectionRecovered){
        const char message[]="ERROR: page-protection restore failed after patching; original byte and protection recovered.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
    }else{
        const char message[]="CRITICAL: page-protection restore failed and rollback was incomplete; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
    }
    return false;
}

static bool applyPatch(u8* base){
    u8* signature=base+SIGNATURE_RVA;
    u8* target=base+PATCH_RVA;
    if(bytesEqual(signature,PATCHED_SIGNATURE,(u32)sizeof(PATCHED_SIGNATURE))){
        const char message[]="INFO: patch was already present; no second write was made.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        return true;
    }
    if(!bytesEqual(signature,EXPECTED_SIGNATURE,(u32)sizeof(EXPECTED_SIGNATURE))){
        const char message[]="ERROR: instruction signature mismatch at DS2.exe+0xFA4247; no patch applied.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        return false;
    }
    if(!bytesEqual(target,ORIGINAL_PATCH,(u32)sizeof(ORIGINAL_PATCH))){
        const char message[]="ERROR: patch-site bytes were not original; no patch applied.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        return false;
    }
    if(!writePatchTransactional(target,signature))return false;
    const char message[]="ACTIVE: weapon prohibited-area return reason 6 is bypassed.\r\n";
    logRaw(message,(DWORD)(sizeof(message)-1));
    return true;
}

static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    if(!boolIni(L"Enabled",true))return 0;
    g_debugLog=boolIni(L"DebugLog",false);
    openDebugLog();

    u8* base=(u8*)game;
    u32 timestamp=0,imageSize=0;
    if(!readBuildMetadata(base,&timestamp,&imageSize)){
        const char message[]="ERROR: could not read DS2.exe build metadata; no patch applied.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        closeDebugLog();
        return 0;
    }
    if(timestamp!=EXPECTED_TIMESTAMP||imageSize!=EXPECTED_IMAGE_SIZE){
        const char message[]="ERROR: unsupported DS2.exe build; no patch applied.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        closeDebugLog();
        return 0;
    }

    applyPatch(base);
    closeDebugLog();
    return 0;
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
    if(reason==1){
        g_self=module;
        DisableThreadLibraryCalls(module);
        HANDLE thread=CreateThread(0,0,worker,0,0,0);
        if(thread)CloseHandle(thread);
    }
    return TRUE;
}
