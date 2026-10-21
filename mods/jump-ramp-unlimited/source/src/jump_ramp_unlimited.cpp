#include "windows.h"

extern "C" int _fltused=0;
extern "C" void* memcpy(void* d,const void* s,SIZE_T n){
    u8* o=(u8*)d;const u8* i=(const u8*)s;
    for(SIZE_T k=0;k<n;k++)o[k]=i[k];return d;
}
extern "C" void* memset(void* d,int v,SIZE_T n){
    u8* o=(u8*)d;for(SIZE_T k=0;k<n;k++)o[k]=(u8)v;return d;
}

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u

static const u64 RVA_STAGE_PATCH=0x0106153Cull;
static const u64 RVA_STAGE2_CONTINUE=0x01061541ull;
static const u64 RVA_STAGE_SUCCESS=0x01061561ull;
static const u64 RVA_STAGE_RETURN=0x0106156Dull;
static const u64 RVA_STAGE_HELPER=0x01061A40ull;
static const u8 VANILLA_STAGE_BYTES[5]={0x83,0xF9,0x01,0x75,0x2C};

struct Settings{u32 enabled;};
static HMODULE g_self=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static u8* g_base=0;
static u8* g_cave=0;
static u32 g_caveCodeSize=0;
static u8 g_patch[5]={0};
static WCHAR lowerAscii(WCHAR c){
    return c>=L'A'&&c<=L'Z'?(WCHAR)(c+(L'a'-L'A')):c;
}
static void modulePath(WCHAR* out,const WCHAR* name){
    out[0]=0;DWORD n=GetModuleFileNameW(g_self,out,520u);
    if(!n||n>=520u){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<n;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1u;
    DWORD j=0;
    while(name[j]&&slash+j+1u<520u){out[slash+j]=name[j];j++;}
    if(name[j]){out[0]=0;return;}
    out[slash+j]=0;
}
static bool isDs2Process(HMODULE game){
    WCHAR p[520];DWORD n=GetModuleFileNameW(game,p,520u);
    if(!n||n>=520u)return false;
    DWORD s=0;for(DWORD i=0;i<n;i++)if(p[i]==L'\\'||p[i]==L'/')s=i+1u;
    const WCHAR wanted[]=L"ds2.exe";u32 j=0;
    while(wanted[j]){
        if(s+j>=n||lowerAscii(p[s+j])!=wanted[j])return false;
        j++;
    }
    return s+j==n;
}
static void openLog(){
    WCHAR p[520];modulePath(p,L"ds2_jump_ramp_unlimited.log");
    if(!p[0])return;
    g_log=CreateFileW(p,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char h[]="DS2 Jump Ramp Unlimited " MOD_VERSION "\r\nTarget: Steam 1.10.89.0\r\n";
        DWORD w=0;WriteFile(g_log,h,(DWORD)(sizeof(h)-1u),&w,0);FlushFileBuffers(g_log);
    }
}
static void logText(const char* s){
    if(g_log==INVALID_HANDLE_VALUE||!s)return;
    DWORD n=0;while(s[n])n++;
    DWORD w=0;WriteFile(g_log,s,n,&w,0);
    const char eol[]="\r\n";
    WriteFile(g_log,eol,2,&w,0);FlushFileBuffers(g_log);
}
static bool validateBuild(){
    if(!g_base||*(u16*)g_base!=0x5A4Du)return false;
    u32 pe=*(u32*)(g_base+0x3Cu);
    if(pe<0x40u||pe>0x1000u||*(u32*)(g_base+pe)!=0x00004550u)return false;
    if(*(u16*)(g_base+pe+4u)!=0x8664u||*(u16*)(g_base+pe+24u)!=0x020Bu)return false;
    return *(u32*)(g_base+pe+8u)==EXPECTED_TIMESTAMP &&
           *(u32*)(g_base+pe+24u+56u)==EXPECTED_IMAGE_SIZE;
}
static bool bytesEqual(const u8* a,const u8* b,u32 n){
    for(u32 i=0;i<n;i++)if(a[i]!=b[i])return false;
    return true;
}
static bool rel32ok(u64 fromNext,u64 to){
    s64 d=(s64)to-(s64)fromNext;
    return d>=(-2147483647LL-1LL)&&d<=2147483647LL;
}
static u32 rel32(u64 fromNext,u64 to){
    return (u32)((s64)to-(s64)fromNext);
}
static void put32(u8* b,u32* p,u32 v){
    b[(*p)++]=(u8)v;b[(*p)++]=(u8)(v>>8);
    b[(*p)++]=(u8)(v>>16);b[(*p)++]=(u8)(v>>24);
}
static u8* allocNear(u64 target){
    const u64 step=0x100000ull,maxd=0x70000000ull;
    for(u64 d=step;d<maxd;d+=step){
        u64 c[2]={(target>d)?target-d:0,target+d};
        for(u32 k=0;k<2u;k++){
            if(!c[k])continue;
            u64 hint=c[k]&~0xFFFFull;
            u8* p=(u8*)VirtualAlloc((LPVOID)hint,0x1000u,
                                    MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
            if(!p)continue;
            if(rel32ok(target+5u,(u64)p)){memset(p,0x90,0x1000u);return p;}
            VirtualFree(p,0u,MEM_RELEASE);
        }
    }
    return 0;
}
static void emitRel32(u8* b,u32* p,u8 opcode,u64 target){
    b[(*p)++]=opcode;
    u64 next=(u64)(b+*p+4u);
    put32(b,p,rel32(next,target));
}
static void emitJcc32(u8* b,u32* p,u8 cc,u64 target){
    b[(*p)++]=0x0F;b[(*p)++]=cc;
    u64 next=(u64)(b+*p+4u);
    put32(b,p,rel32(next,target));
}
static void emitBytes(u8* b,u32* p,const u8* src,u32 n){
    for(u32 i=0;i<n;i++)b[(*p)++]=src[i];
}
static bool buildCave(){
    u8* site=g_base+RVA_STAGE_PATCH;
    if(!bytesEqual(site,VANILLA_STAGE_BYTES,5u))return false;
    g_cave=allocNear((u64)site);
    if(!g_cave)return false;
    const u64 targets[]={RVA_STAGE2_CONTINUE,RVA_STAGE_SUCCESS,RVA_STAGE_RETURN,RVA_STAGE_HELPER};
    for(u32 i=0;i<4;i++)if(!rel32ok((u64)g_cave,(u64)(g_base+targets[i]))||
                                !rel32ok((u64)(g_cave+0x1000),(u64)(g_base+targets[i]))){
        VirtualFree(g_cave,0,MEM_RELEASE);g_cave=0;return false;
    }

    u32 p=0;
    // The dispatcher reaches this hook with ECX = stage - 1.
    g_cave[p++]=0x83;g_cave[p++]=0xF9;g_cave[p++]=0x01;
    // Stage 2 on a cap-2 ramp is already the final trick.
    u32 notStage2=p;emitJcc32(g_cave,&p,0x85,0);
    const u8 cap2[]={0x80,0xBB,0xFC,0x1D,0,0,2};
    emitBytes(g_cave,&p,cap2,7);
    emitJcc32(g_cave,&p,0x85,(u64)(g_base+RVA_STAGE2_CONTINUE));
    const u8 repeat2[]={0xC6,0x83,0xFB,0x1D,0,0,1,0xB2,2};
    emitBytes(g_cave,&p,repeat2,9);
    u32 toCommon=p;emitRel32(g_cave,&p,0xE9,0);
    *(u32*)(g_cave+notStage2+2)=rel32((u64)(g_cave+notStage2+6),(u64)(g_cave+p));

    g_cave[p++]=0x83;g_cave[p++]=0xF9;g_cave[p++]=0x02;
    emitJcc32(g_cave,&p,0x85,(u64)(g_base+RVA_STAGE_RETURN));
    const u8 cap3[]={0x80,0xBB,0xFC,0x1D,0,0,3};
    emitBytes(g_cave,&p,cap3,7);
    emitJcc32(g_cave,&p,0x85,(u64)(g_base+RVA_STAGE_RETURN));
    g_cave[p++]=0xC6;g_cave[p++]=0x83;g_cave[p++]=0xFB;
    g_cave[p++]=0x1D;g_cave[p++]=0x00;g_cave[p++]=0x00;g_cave[p++]=0x02;
    g_cave[p++]=0xB2;g_cave[p++]=0x03;
    *(u32*)(g_cave+toCommon+1)=rel32((u64)(g_cave+toCommon+5),(u64)(g_cave+p));
    g_cave[p++]=0x48;g_cave[p++]=0x8B;g_cave[p++]=0xCB;
    emitRel32(g_cave,&p,0xE8,(u64)(g_base+RVA_STAGE_HELPER));
    g_cave[p++]=0x84;g_cave[p++]=0xC0;
    u32 toSuccess=p;emitJcc32(g_cave,&p,0x85,0);
    // An unsuccessful helper leaves the temporarily decremented stage intact.
    const u8 restore[]={0xFE,0x83,0xFB,0x1D,0,0};
    emitBytes(g_cave,&p,restore,6);
    emitRel32(g_cave,&p,0xE9,(u64)(g_base+RVA_STAGE_RETURN));
    *(u32*)(g_cave+toSuccess+2)=rel32((u64)(g_cave+toSuccess+6),(u64)(g_cave+p));
    emitRel32(g_cave,&p,0xE9,(u64)(g_base+RVA_STAGE_SUCCESS));
    g_caveCodeSize=p;
    DWORD oldProtect=0;
    if(!VirtualProtect(g_cave,0x1000u,PAGE_EXECUTE_READ,&oldProtect)||
       !FlushInstructionCache(GetCurrentProcess(),g_cave,p)){
        VirtualFree(g_cave,0,MEM_RELEASE);g_cave=0;return false;
    }

    g_patch[0]=0xE9;
    u32 go=rel32((u64)site+5u,(u64)g_cave);
    g_patch[1]=(u8)go;g_patch[2]=(u8)(go>>8);
    g_patch[3]=(u8)(go>>16);g_patch[4]=(u8)(go>>24);
    return true;
}
struct Signature{u32 rva;u32 size;const u8* bytes;};
static const u8 DISPATCH_PREFIX[]={0x0F,0xB6,0x8B,0xFB,0x1D,0,0,0x85,0xC9,0x74,0x22,0x83,0xE9,1,0x74,0x11};
static const u8 CONTINUE_BYTES[]={0xB2,3,0x48,0x8B,0xCB,0xE8,0xF5,4,0,0};
static const u8 SUCCESS_BYTES[]={0x84,0xC0,0x74,8,0x48,0x8B,0xCB,0xE8,0x13,0x0B,0,0};
static const u8 RETURN_BYTES[]={0xC5,0xF8,0x28,0x74,0x24,0x30,0x48,0x83,0xC4,0x40,0x5B,0xC3};
static const u8 HELPER_CAP_BYTES[]={0x40,0x3A,0xB9,0xFC,0x1D,0,0,0x0F,0x87,0xC4,1,0,0};
static const Signature SIGNATURES[]={
    {0x106152C,sizeof(DISPATCH_PREFIX),DISPATCH_PREFIX},
    {0x106153C,sizeof(VANILLA_STAGE_BYTES),VANILLA_STAGE_BYTES},
    {0x1061541,sizeof(CONTINUE_BYTES),CONTINUE_BYTES},
    {0x1061561,sizeof(SUCCESS_BYTES),SUCCESS_BYTES},
    {0x106156D,sizeof(RETURN_BYTES),RETURN_BYTES},
    {0x1061A81,sizeof(HELPER_CAP_BYTES),HELPER_CAP_BYTES}
};
static bool validateSites(){
    for(u32 i=0;i<sizeof(SIGNATURES)/sizeof(Signature);i++){
        const Signature* sig=&SIGNATURES[i];
        if(!bytesEqual(g_base+sig->rva,sig->bytes,sig->size))return false;
    }
    return true;
}
// Install once at startup. INI changes require a restart; no runtime code toggles.
static int installPatch(){
    u8* site=g_base+RVA_STAGE_PATCH;
    if(!bytesEqual(site,VANILLA_STAGE_BYTES,5u))return 0;
    DWORD old=0,tmp=0;
    if(!VirtualProtect(site,5u,PAGE_EXECUTE_READWRITE,&old))return 0;
    for(u32 i=0;i<5u;i++)site[i]=g_patch[i];
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),site,5u);
    BOOL protectedAgain=VirtualProtect(site,5u,old,&tmp);
    // Once the jump is written, the cave must stay alive even if an OS call fails.
    return flushed&&protectedAgain?1:2;
}
static bool parseUInt(const WCHAR* s,u32 minV,u32 maxV,u32* out){
    if(!s||!out)return false;u64 v=0;u32 n=0;
    while(*s==L' '||*s==L'\t')s++;
    while(*s>=L'0'&&*s<=L'9'){v=v*10u+(u64)(*s-L'0');s++;n++;if(v>maxV)return false;}
    while(*s==L' '||*s==L'\t')s++;
    if(!n||*s||v<minV||v>maxV)return false;
    *out=(u32)v;return true;
}
static bool readIni(const WCHAR* key,const WCHAR* fallback,WCHAR* out,DWORD cap){
    WCHAR p[520];modulePath(p,L"ds2_jump_ramp_unlimited.ini");
    if(!p[0])return false;
    DWORD n=GetPrivateProfileStringW(L"JumpRampUnlimited",key,fallback,out,cap,p);
    return n>0u&&n<cap-1u;
}
static bool loadSettings(Settings* s){
    WCHAR v[64];if(!s)return false;
    if(!readIni(L"Enabled",L"1",v,64u)||!parseUInt(v,0u,1u,&s->enabled))return false;
    return true;
}
static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    g_base=(u8*)game;openLog();
    if(!validateBuild()){logText("ERROR: unsupported DS2.exe build; mod inactive.");return 0;}
    if(GetModuleHandleW(L"ds2_jump_ramp_probe.asi")){
        logText("ERROR: remove ds2_jump_ramp_probe.asi and restart; mod inactive.");return 0;
    }
    Settings s;
    if(!loadSettings(&s)){logText("ERROR: Enabled must be 0 or 1; mod inactive.");return 0;}
    if(!s.enabled){logText("DISABLED: Enabled=0; game unchanged.");return 0;}
    if(!validateSites()){
        logText("ERROR: game instructions differ or another mod occupies the hook; mod inactive.");return 0;
    }
    if(!buildCave()){
        logText("ERROR: could not prepare the repeat hook; mod inactive.");return 0;
    }
    int result=installPatch();
    if(!result){
        VirtualFree(g_cave,0,MEM_RELEASE);g_cave=0;
        logText("ERROR: hook installation refused; mod inactive.");return 0;
    }
    if(result==2){
        logText("WARNING: hook was written, but cache flush or protection restoration failed; restart required.");return 0;
    }
    logText("ACTIVE: Jump Ramp Unlimited 1.0.0. Ramp trick limit removed.");
    logText("Settings are read at startup. Restart DS2 after changing the INI.");
    return 0;
}
extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){
        g_self=module;DisableThreadLibraryCalls(module);
        HANDLE t=CreateThread(0,0,&worker,0,0,0);
        if(t)CloseHandle(t);
    }
    return TRUE;
}
