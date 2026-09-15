typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long DWORD;
typedef unsigned long long u64;
typedef unsigned long long SIZE_T;
typedef long LONG;
typedef long long LONGLONG;
typedef int BOOL;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef wchar_t WCHAR;

#define WINAPI __stdcall
#define DLL_PROCESS_ATTACH 1u
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define PAGE_EXECUTE_READWRITE 0x40u

extern "C" {
__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(const WCHAR*);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE,WCHAR*,DWORD);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HANDLE WINAPI CreateThread(LPVOID,SIZE_T,DWORD (WINAPI*)(LPVOID),LPVOID,DWORD,DWORD*);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE);
__declspec(dllimport) HANDLE WINAPI CreateFileW(const WCHAR*,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,DWORD*,LPVOID);
__declspec(dllimport) BOOL WINAPI FlushFileBuffers(HANDLE);
__declspec(dllimport) BOOL WINAPI ReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
__declspec(dllimport) BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,DWORD*);
__declspec(dllimport) BOOL WINAPI FlushInstructionCache(HANDLE,LPCVOID,SIZE_T);
__declspec(dllimport) DWORD WINAPI GetPrivateProfileStringW(const WCHAR*,const WCHAR*,const WCHAR*,WCHAR*,DWORD,const WCHAR*);
__declspec(dllimport) unsigned int WINAPI GetPrivateProfileIntW(const WCHAR*,const WCHAR*,int,const WCHAR*);
LONG _InterlockedCompareExchange(volatile LONG*,LONG,LONG);
void bt_cord_range_hook(void);
}
#pragma intrinsic(_InterlockedCompareExchange)

extern "C" {
int _fltused = 0;
void* g_helper_addr = 0;
void* g_return_addr = 0;
float g_range_multiplier = 5.0f;
}

static const u64 RVA_PATCH_START = 0x0109B72Aull;
static const u64 RVA_HELPER      = 0x02ACA41Aull;
static const u64 RVA_RETURN      = 0x0109B73Bull;
static const u32 PATCH_SIZE      = 17u;

static const u8 EXPECTED[17] = {
    0xC5,0xFA,0x10,0x80,0xD0,0x00,0x00,0x00,
    0xE8,0xE3,0xEC,0xA2,0x01,
    0xC5,0xF8,0x2F,0xF0
};

static HMODULE g_self=0;
static HANDLE g_log=(HANDLE)(LONGLONG)-1;
static volatile LONG g_started=0;
static WCHAR g_ini_path[520];

static u32 strLen(const char* s){
    u32 n=0;
    if(!s)return 0;
    while(s[n])n++;
    return n;
}
static bool bytesEqual(const u8* a,const u8* b,u32 n){
    if(!a||!b)return false;
    for(u32 i=0;i<n;i++)if(a[i]!=b[i])return false;
    return true;
}
static void modulePath(WCHAR* out,const WCHAR* name){
    if(!out||!name)return;
    out[0]=0;
    DWORD len=GetModuleFileNameW(g_self,out,520u);
    if(!len||len>=520u){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<len;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1u;
    DWORD j=0;
    while(name[j]&&slash+j+1u<520u){out[slash+j]=name[j];j++;}
    if(name[j]){out[0]=0;return;}
    out[slash+j]=0;
}
static void openLog(){
    WCHAR path[520];
    modulePath(path,L"ds2_extended_bt_cord_cutting_range.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
}
static void logRaw(const char* s){
    if(g_log==(HANDLE)(LONGLONG)-1||!s)return;
    DWORD w=0;
    WriteFile(g_log,s,strLen(s),&w,0);
    FlushFileBuffers(g_log);
}
static bool readMem(const void* src,void* dst,SIZE_T n){
    SIZE_T got=0;
    return ReadProcessMemory(GetCurrentProcess(),src,dst,n,&got)&&got==n;
}
static char* appendText(char* p,const char* s){while(*s)*p++=*s++;return p;}
static char* appendU32(char* p,u32 v){
    char t[16];
    u32 n=0;
    if(v==0){*p++='0';return p;}
    while(v&&n<16){t[n++]=(char)('0'+(v%10));v/=10;}
    while(n)*p++=t[--n];
    return p;
}
static char* appendFloat1(char* p,float f){
    if(f<0.0f){*p++='-';f=-f;}
    u32 whole=(u32)f;
    float rem=f-(float)whole;
    u32 frac=(u32)(rem*10.0f+0.5f);
    if(frac>=10){whole++;frac=0;}
    p=appendU32(p,whole);
    *p++='.';
    *p++=(char)('0'+frac);
    return p;
}
static float parseFloatW(const WCHAR* s,float def){
    if(!s||!*s)return def;
    bool neg=false;
    u32 i=0;
    if(s[i]==L'-'){neg=true;i++;}
    float v=0.0f;
    bool any=false;
    while(s[i]>=L'0'&&s[i]<=L'9'){
        v=v*10.0f+(float)(s[i]-L'0');
        i++;
        any=true;
    }
    if(s[i]==L'.'||s[i]==L','){
        i++;
        float place=0.1f;
        while(s[i]>=L'0'&&s[i]<=L'9'){
            v+=(float)(s[i]-L'0')*place;
            place*=0.1f;
            i++;
            any=true;
        }
    }
    if(!any)return def;
    return neg?-v:v;
}
static void logMultiplier(float f){
    char b[96];
    char* p=b;
    p=appendText(p,"multiplier=");
    p=appendFloat1(p,f);
    *p++='\r';
    *p++='\n';
    *p=0;
    logRaw(b);
}

static DWORD WINAPI worker(LPVOID){
    openLog();
    logRaw("DS2 Extended BT Cord Cutting Range v1.0.0\r\n");
    logRaw("mode=confirmed_CodeCutWithBoomerang_length_multiplier\r\n");

    modulePath(g_ini_path,L"ds2_extended_bt_cord_cutting_range.ini");
    unsigned int enabled=GetPrivateProfileIntW(L"Range",L"Enabled",1,g_ini_path);
    if(!enabled){
        logRaw("status=disabled_by_ini\r\n");
        return 0;
    }

    WCHAR multBuf[64];
    GetPrivateProfileStringW(L"Range",L"Multiplier",L"5.0",multBuf,64,g_ini_path);
    float mult=parseFloatW(multBuf,5.0f);
    if(mult<1.0f)mult=1.0f;
    if(mult>50.0f)mult=50.0f;
    g_range_multiplier=mult;
    logMultiplier(mult);

    u8* base=(u8*)GetModuleHandleW(0);
    if(!base){
        logRaw("status=abort_no_game_module\r\n");
        return 0;
    }

    u8* target=base+RVA_PATCH_START;
    u8 actual[PATCH_SIZE];
    if(!readMem(target,actual,PATCH_SIZE)){
        logRaw("status=abort_target_unreadable\r\n");
        return 0;
    }
    if(!bytesEqual(actual,EXPECTED,PATCH_SIZE)){
        logRaw("status=abort_signature_mismatch\r\n");
        return 0;
    }
    logRaw("status=target_verified\r\n");

    g_helper_addr=(void*)(base+RVA_HELPER);
    g_return_addr=(void*)(base+RVA_RETURN);

    u8 patch[PATCH_SIZE];
    patch[0]=0xFF; patch[1]=0x25;
    patch[2]=0; patch[3]=0; patch[4]=0; patch[5]=0;
    u64 hook=(u64)(void*)&bt_cord_range_hook;
    for(u32 i=0;i<8;i++)patch[6+i]=(u8)((hook>>(i*8))&0xFFu);
    patch[14]=0x90; patch[15]=0x90; patch[16]=0x90;

    DWORD old=0;
    if(!VirtualProtect(target,PATCH_SIZE,PAGE_EXECUTE_READWRITE,&old)){
        logRaw("status=abort_virtualprotect_failed\r\n");
        return 0;
    }

    for(u32 i=0;i<PATCH_SIZE;i++)target[i]=patch[i];
    FlushInstructionCache(GetCurrentProcess(),target,PATCH_SIZE);

    DWORD ignored=0;
    VirtualProtect(target,PATCH_SIZE,old,&ignored);

    u8 verify[PATCH_SIZE];
    if(!readMem(target,verify,PATCH_SIZE)||!bytesEqual(verify,patch,PATCH_SIZE)){
        logRaw("status=abort_patch_verify_failed\r\n");
        return 0;
    }

    logRaw("status=patched\r\n");
    logRaw("note=original_distance_failure_branch_preserved\r\n");
    return 0;
}

extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){
        g_self=module;
        DisableThreadLibraryCalls(module);
        if(_InterlockedCompareExchange(&g_started,1,0)==0){
            HANDLE t=CreateThread(0,0,&worker,0,0,0);
            if(t)CloseHandle(t);
            else g_started=0;
        }
    }
    return 1;
}
