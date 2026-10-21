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
#define SIGNATURE_SIZE 20u
#define RET_OPCODE 0xC3u

static HMODULE g_self=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static bool g_debugLog=false;

struct PatchDefinition{
    const char* name;
    u64 rva;
    u8 expected[SIGNATURE_SIZE];
};

struct PatchRuntime{
    const PatchDefinition* definition;
    u8* address;
    DWORD originalProtection;
    bool enabled;
    bool original;
    bool changedByUs;
};

static const PatchDefinition PROXIMITY_PATCH={
    "NearbyFacility",
    0x01337820ull,
    {0x40,0x53,0x41,0x54,0x41,0x57,0x48,0x81,0xEC,0x10,0x04,0x00,0x00,0x48,0x8B,0x05,0xCC,0x1F,0x68,0x02}
};

static const PatchDefinition PERMITTED_AREA_PATCH={
    "PermittedArea",
    0x01336530ull,
    {0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x40,0x48,0x8B,0x5A,0x08,0x48,0x8B,0xFA,0xE8,0x6A,0xA5}
};

static const PatchDefinition UNCONSTRUCTABLE_AREA_PATCH={
    "UnconstructableArea",
    0x01339AE0ull,
    {0x4C,0x8B,0xDC,0x49,0x89,0x5B,0x20,0x56,0x57,0x41,0x54,0x41,0x56,0x41,0x57,0x48,0x81,0xEC,0x30,0x03}
};

static bool bytesEqual(const u8* left,const u8* right,u32 count){
    if(!left||!right)return false;
    for(u32 i=0;i<count;i++)if(left[i]!=right[i])return false;
    return true;
}

static bool matchesPatched(const u8* bytes,const PatchDefinition* definition){
    if(!bytes||!definition||bytes[0]!=RET_OPCODE)return false;
    return bytesEqual(bytes+1,definition->expected+1,SIGNATURE_SIZE-1);
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

static WCHAR lowerAscii(WCHAR value){
    return value>=L'A'&&value<=L'Z'?(WCHAR)(value+(L'a'-L'A')):value;
}

static bool equalsInsensitive(const WCHAR* left,const WCHAR* right){
    if(!left||!right)return false;
    u32 i=0;
    while(left[i]&&right[i]){
        if(lowerAscii(left[i])!=lowerAscii(right[i]))return false;
        i++;
    }
    return left[i]==0&&right[i]==0;
}

static bool boolIni(const WCHAR* key,bool fallback){
    WCHAR path[520],value[32];
    modulePath(path,L"ds2_construction_anywhere.ini");
    if(!path[0])return fallback;
    value[0]=0;
    GetPrivateProfileStringW(L"ConstructionAnywhere",key,fallback?L"1":L"0",value,32,path);
    if(equalsInsensitive(value,L"1")||equalsInsensitive(value,L"true")||
       equalsInsensitive(value,L"yes")||equalsInsensitive(value,L"on"))return true;
    if(equalsInsensitive(value,L"0")||equalsInsensitive(value,L"false")||
       equalsInsensitive(value,L"no")||equalsInsensitive(value,L"off"))return false;
    return false;
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
    modulePath(path,L"ds2_construction_anywhere.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Construction Anywhere v" MOD_VERSION "\r\n"
            "Expected target: Steam PC 1.10.89.0\r\n"
            "Scope: NearbyFacility, UnconstructableArea and optional PermittedArea only.\r\n";
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
    return true;
}

static bool preflightSite(PatchRuntime* runtime,u8* base,u32 imageSize){
    if(!runtime||!runtime->definition||!base)return false;
    if(runtime->definition->rva+SIGNATURE_SIZE>(u64)imageSize)return false;
    runtime->address=base+runtime->definition->rva;
    if(bytesEqual(runtime->address,runtime->definition->expected,SIGNATURE_SIZE)){
        runtime->original=true;
        return true;
    }
    if(matchesPatched(runtime->address,runtime->definition)){
        runtime->original=false;
        return true;
    }
    return false;
}

static bool rollbackSite(PatchRuntime* runtime){
    if(!runtime||!runtime->changedByUs)return true;
    DWORD protectionBeforeRollback=0;
    if(!VirtualProtect(runtime->address,1,PAGE_EXECUTE_READWRITE,&protectionBeforeRollback))return false;
    ((volatile u8*)runtime->address)[0]=runtime->definition->expected[0];
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),runtime->address,1);
    bool verified=bytesEqual(runtime->address,runtime->definition->expected,SIGNATURE_SIZE);
    DWORD ignored=0;
    BOOL restored=VirtualProtect(runtime->address,1,runtime->originalProtection,&ignored);
    if(flushed&&verified&&restored){
        runtime->changedByUs=false;
        return true;
    }
    return false;
}

static bool rollbackAll(PatchRuntime* runtimes,u32 count){
    bool complete=true;
    for(u32 i=count;i>0;i--)if(!rollbackSite(&runtimes[i-1]))complete=false;
    return complete;
}

static bool applySites(PatchRuntime* runtimes,u32 count,u8* base,u32 imageSize){
    bool anyEnabled=false;
    for(u32 i=0;i<count;i++){
        if(!runtimes[i].enabled)continue;
        anyEnabled=true;
        if(!preflightSite(&runtimes[i],base,imageSize)){
            const char message[]="ERROR: enabled patch signature mismatch; no patch was applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1));
            return false;
        }
    }
    if(!anyEnabled){
        const char message[]="INFO: no construction checks were selected in the INI.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1));
        return true;
    }

    for(u32 i=0;i<count;i++){
        PatchRuntime* runtime=&runtimes[i];
        if(!runtime->enabled||!runtime->original)continue;
        DWORD oldProtection=0;
        if(!VirtualProtect(runtime->address,1,PAGE_EXECUTE_READWRITE,&oldProtection)){
            const char message[]="ERROR: could not make an enabled patch site writable.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1));
            if(!rollbackAll(runtimes,count)){
                const char critical[]="CRITICAL: transaction rollback was incomplete; stop DS2.\r\n";
                logCritical(critical,(DWORD)(sizeof(critical)-1));
            }
            return false;
        }
        runtime->originalProtection=oldProtection;
        ((volatile u8*)runtime->address)[0]=RET_OPCODE;
        runtime->changedByUs=true;
        BOOL flushed=FlushInstructionCache(GetCurrentProcess(),runtime->address,1);
        bool verified=matchesPatched(runtime->address,runtime->definition);
        DWORD ignored=0;
        BOOL protectionRestored=VirtualProtect(runtime->address,1,oldProtection,&ignored);
        if(!flushed||!verified||!protectionRestored){
            const char message[]="ERROR: patch write or verification failed; rolling back the transaction.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1));
            if(!rollbackAll(runtimes,count)){
                const char critical[]="CRITICAL: transaction rollback was incomplete; stop DS2.\r\n";
                logCritical(critical,(DWORD)(sizeof(critical)-1));
            }
            return false;
        }
    }

    const char message[]="ACTIVE: selected construction checks are bypassed.\r\n";
    logRaw(message,(DWORD)(sizeof(message)-1));
    return true;
}

static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    if(!boolIni(L"Enabled",true))return 0;

    bool removeProximity=boolIni(L"RemoveProximityLimit",true);
    bool allowRestricted=boolIni(L"AllowRestrictedAreas",true);
    bool ignorePermittedWhitelist=boolIni(L"IgnorePermittedAreaWhitelist",false);
    g_debugLog=boolIni(L"DebugLog",false);
    openDebugLog();

    u8* base=(u8*)game;
    u32 timestamp=0,imageSize=0;
    if(!readBuildMetadata(base,&timestamp,&imageSize)){
        const char message[]="ERROR: could not read DS2.exe build metadata; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeDebugLog();
        return 0;
    }
    if(timestamp!=EXPECTED_TIMESTAMP||imageSize!=EXPECTED_IMAGE_SIZE){
        const char message[]="ERROR: unsupported DS2.exe build; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeDebugLog();
        return 0;
    }

    PatchRuntime patches[3]={
        {&PROXIMITY_PATCH,0,0,removeProximity,false,false},
        {&UNCONSTRUCTABLE_AREA_PATCH,0,0,allowRestricted,false,false},
        {&PERMITTED_AREA_PATCH,0,0,ignorePermittedWhitelist,false,false}
    };
    applySites(patches,3,base,imageSize);
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
