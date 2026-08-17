typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef long long s64;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef unsigned long DWORD;
typedef int BOOL;
typedef DWORD* LPDWORD;
#define TRUE 1
#define FALSE 0
#define WINAPI __stdcall
#define DLL_PROCESS_ATTACH 1
#define MEM_COMMIT 0x1000u
#define MEM_RESERVE 0x2000u
#define MEM_RELEASE 0x8000u
#define PAGE_NOACCESS 0x01u
#define PAGE_READWRITE 0x04u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_GUARD 0x100u

struct MEMORY_BASIC_INFORMATION_X64 {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    u16 PartitionId;
    u16 Padding0;
    u64 RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
    DWORD Padding1;
};

extern "C" __declspec(dllimport) HMODULE WINAPI GetModuleHandleW(const wchar_t*);
extern "C" __declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE,wchar_t*,DWORD);
extern "C" __declspec(dllimport) void WINAPI Sleep(DWORD);
extern "C" __declspec(dllimport) HANDLE WINAPI CreateThread(LPVOID,u64,DWORD (WINAPI*)(LPVOID),LPVOID,DWORD,LPDWORD);
extern "C" __declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE);
extern "C" __declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
extern "C" __declspec(dllimport) LPVOID WINAPI VirtualAlloc(LPVOID,u64,DWORD,DWORD);
extern "C" __declspec(dllimport) BOOL WINAPI VirtualFree(LPVOID,u64,DWORD);
extern "C" __declspec(dllimport) BOOL WINAPI VirtualProtect(LPVOID,u64,DWORD,LPDWORD);
extern "C" __declspec(dllimport) u64 WINAPI VirtualQuery(const void*,LPVOID,u64);
extern "C" __declspec(dllimport) BOOL WINAPI FlushInstructionCache(HANDLE,const void*,u64);
extern "C" __declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
extern "C" long _InterlockedCompareExchange(volatile long*,long,long);
#pragma intrinsic(_InterlockedCompareExchange)

static volatile long g_started=0;
static u8* g_base=0;
static u8* g_cave=0;
static u32 g_patch_mask=0;
static u32 g_ctor_patch_mask=0;

// Proven Floating Carrier footprint path for DS2.exe v1.10.89.0.
static const u32 RVA_SITE_A=0x0123779Bu;
static const u32 RVA_LIMIT_A=0x0118851Au;
static const u32 RVA_LIMIT_B=0x0118852Au;
static const u8 EXP_SITE_A[5]={0x41,0x0F,0xB7,0x4F,0x26};
static const u8 EXP_LIMIT_A[3]={0x83,0xF9,0x12};
static const u8 EXP_LIMIT_B[3]={0x83,0xF9,0x12};
static const u8 NEW_LIMIT_A[3]={0x83,0xF9,0x78};
static const u8 NEW_LIMIT_B[3]={0x83,0xF9,0x78};
static const u32 NATIVE_ITEM_MAX=36u;
static const u32 VIRTUAL_ITEM_MAX=240u;

// Offline-confirmed global DSBaggageConfig singleton and class layout.
// root = *(DS2.exe + 0x0623EA48), config = *(root + 0x10).
static const u32 RVA_GLOBAL_CONFIG_ROOT=0x0623EA48u;
static const u32 RVA_DSBAGGAGE_CONFIG_VTABLE=0x0327CBE0u;
static const u32 OFF_ROOT_CONFIG=0x10u;
static const u32 OFF_LV1_LIMIT=0x324u;
static const u32 OFF_LV1_ACTION=0x328u;
static const u32 OFF_LV1_STOP=0x32Cu;
static const u32 OFF_LV2_LIMIT=0x330u;
static const u32 OFF_LV2_ACTION=0x334u;
static const u32 OFF_LV2_STOP=0x338u;

// Constructor fallback for DSBaggageConfig objects created after ASI startup.
static const u32 RVA_CTOR_LV1_LIMIT_IMM=0x011AEA86u;
static const u32 RVA_CTOR_LV1_ACTION_IMM=0x011AEA90u;
static const u32 RVA_CTOR_LV1_STOP_IMM=0x011AEA9Au;
static const u32 RVA_CTOR_LV2_LIMIT_IMM=0x011AEAA4u;
static const u32 RVA_CTOR_LV2_ACTION_IMM=0x011AEAAEu;
static const u32 RVA_CTOR_LV2_STOP_IMM=0x011AEAB8u;

static const u32 F300=0x43960000u;
static const u32 F150=0x43160000u;
static const u32 F900=0x44610000u;
static const u32 F450=0x43E10000u;
static const u32 F5000=0x459C4000u;
static const u32 F2500=0x451C4000u;

static BOOL eq(const u8*a,const u8*b,u32 n){for(u32 i=0;i<n;i++)if(a[i]!=b[i])return FALSE;return TRUE;}
static void cp(u8*d,const u8*s,u32 n){for(u32 i=0;i<n;i++)d[i]=s[i];}
static void zero(u8*d,u32 n){for(u32 i=0;i<n;i++)d[i]=0;}
static void wr32(u8*p,u32 v){p[0]=(u8)v;p[1]=(u8)(v>>8);p[2]=(u8)(v>>16);p[3]=(u8)(v>>24);}
static void wr64(u8*p,u64 v){for(u32 i=0;i<8;i++)p[i]=(u8)(v>>(8*i));}
static BOOL rel32ok(u64 fromNext,u64 to){s64 d=(s64)to-(s64)fromNext;return d>=(-2147483647LL-1LL)&&d<=2147483647LL;}

static BOOL isDS2(){
    wchar_t p[520];DWORD n=GetModuleFileNameW(0,p,520);if(!n||n>=520)return FALSE;
    const wchar_t*b=p;for(DWORD i=0;i<n;i++)if(p[i]==L'\\'||p[i]==L'/')b=p+i+1;
    const wchar_t t[]=L"DS2.exe";
    for(u32 i=0;;i++){wchar_t a=b[i],q=t[i];if(a>=L'A'&&a<=L'Z')a+=32;if(q>=L'A'&&q<=L'Z')q+=32;if(a!=q)return FALSE;if(!a)return TRUE;}
}

static BOOL rangeReadable(const void*p,u64 n){
    if(!p||!n)return FALSE;
    MEMORY_BASIC_INFORMATION_X64 m;
    if(VirtualQuery(p,&m,sizeof(m))<sizeof(m))return FALSE;
    if(m.State!=MEM_COMMIT)return FALSE;
    DWORD prot=m.Protect;
    if((prot&PAGE_GUARD)||((prot&0xFFu)==PAGE_NOACCESS))return FALSE;
    u64 a=(u64)p,b=(u64)m.BaseAddress,e=b+m.RegionSize;
    return a>=b&&a+n>=a&&a+n<=e;
}

static BOOL read32(const u8*p,u32&v){if(!rangeReadable(p,4))return FALSE;v=*(volatile const u32*)p;return TRUE;}
static BOOL read64(const u8*p,u64&v){if(!rangeReadable(p,8))return FALSE;v=*(volatile const u64*)p;return TRUE;}

static BOOL writeMem(u8*dst,const u8*src,u32 n){
    DWORD old=0,tmp=0;
    if(!VirtualProtect(dst,n,PAGE_EXECUTE_READWRITE,&old))return FALSE;
    cp(dst,src,n);
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),dst,n);
    BOOL restored=VirtualProtect(dst,n,old,&tmp);
    return flushed&&restored;
}

static BOOL writeData32(u8*dst,u32 v){
    DWORD old=0,tmp=0;
    if(!rangeReadable(dst,4))return FALSE;
    if(!VirtualProtect(dst,4,PAGE_READWRITE,&old))return FALSE;
    *(volatile u32*)dst=v;
    return VirtualProtect(dst,4,old,&tmp);
}

static BOOL patchJump5(u8*at,u8*to){
    if(!rel32ok((u64)at+5,(u64)to))return FALSE;
    u8 p[5]={0xE9,0,0,0,0};
    wr32(p+1,(u32)((s64)(u64)to-(s64)((u64)at+5)));
    return writeMem(at,p,5);
}

static u8* allocNear(u8*target){
    const u64 gran=0x10000ULL;u64 t=(u64)target;
    for(u64 step=gran;step<0x70000000ULL;step+=gran){
        u64 hi=(t+step)&~(gran-1);
        LPVOID p=VirtualAlloc((LPVOID)hi,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
        if(p){if(rel32ok(t+5,(u64)p))return (u8*)p;VirtualFree(p,0,MEM_RELEASE);}
        if(t>step){
            u64 lo=(t-step)&~(gran-1);
            p=VirtualAlloc((LPVOID)lo,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
            if(p){if(rel32ok(t+5,(u64)p))return (u8*)p;VirtualFree(p,0,MEM_RELEASE);}
        }
    }
    return 0;
}

static void buildSiteAStub(){
    u8*s=g_cave;u32 o=0;
    // Original instruction.
    s[o++]=0x41;s[o++]=0x0F;s[o++]=0xB7;s[o++]=0x4F;s[o++]=0x26;
    // Preserve incoming flags because MOVZX itself does not modify them.
    s[o++]=0x9C;
    s[o++]=0x83;s[o++]=0xF9;s[o++]=(u8)NATIVE_ITEM_MAX;
    s[o++]=0x75;u32 jnepos=o++;
    s[o++]=0xB9;wr32(s+o,VIRTUAL_ITEM_MAX);o+=4;
    u32 restore=o;
    s[jnepos]=(u8)((s64)(s+restore)-(s64)(s+jnepos+1));
    s[o++]=0x9D;
    s[o++]=0xFF;s[o++]=0x25;s[o++]=0;s[o++]=0;s[o++]=0;s[o++]=0;
    wr64(s+o,(u64)(g_base+RVA_SITE_A+5));o+=8;
}

static BOOL rollback(){
    BOOL ok=TRUE;if(!g_base)return TRUE;
    if(g_patch_mask&1)ok=writeMem(g_base+RVA_SITE_A,EXP_SITE_A,5)&&ok;
    if(g_patch_mask&2)ok=writeMem(g_base+RVA_LIMIT_A,EXP_LIMIT_A,3)&&ok;
    if(g_patch_mask&4)ok=writeMem(g_base+RVA_LIMIT_B,EXP_LIMIT_B,3)&&ok;
    if(g_ctor_patch_mask&1){u8 x[4];wr32(x,F300);ok=writeMem(g_base+RVA_CTOR_LV1_LIMIT_IMM,x,4)&&ok;}
    if(g_ctor_patch_mask&2){u8 x[4];wr32(x,F150);ok=writeMem(g_base+RVA_CTOR_LV1_ACTION_IMM,x,4)&&ok;}
    if(g_ctor_patch_mask&4){u8 x[4];wr32(x,F300);ok=writeMem(g_base+RVA_CTOR_LV1_STOP_IMM,x,4)&&ok;}
    if(g_ctor_patch_mask&8){u8 x[4];wr32(x,F900);ok=writeMem(g_base+RVA_CTOR_LV2_LIMIT_IMM,x,4)&&ok;}
    if(g_ctor_patch_mask&16){u8 x[4];wr32(x,F450);ok=writeMem(g_base+RVA_CTOR_LV2_ACTION_IMM,x,4)&&ok;}
    if(g_ctor_patch_mask&32){u8 x[4];wr32(x,F900);ok=writeMem(g_base+RVA_CTOR_LV2_STOP_IMM,x,4)&&ok;}
    return ok;
}

static BOOL install(){
    g_base=(u8*)GetModuleHandleW(0);if(!g_base)return FALSE;
    if(!eq(g_base+RVA_SITE_A,EXP_SITE_A,5))return FALSE;
    if(!eq(g_base+RVA_LIMIT_A,EXP_LIMIT_A,3))return FALSE;
    if(!eq(g_base+RVA_LIMIT_B,EXP_LIMIT_B,3))return FALSE;

    u8 native300[4],native150[4],native900[4],native450[4];
    wr32(native300,F300);wr32(native150,F150);wr32(native900,F900);wr32(native450,F450);
    if(!eq(g_base+RVA_CTOR_LV1_LIMIT_IMM,native300,4))return FALSE;
    if(!eq(g_base+RVA_CTOR_LV1_ACTION_IMM,native150,4))return FALSE;
    if(!eq(g_base+RVA_CTOR_LV1_STOP_IMM,native300,4))return FALSE;
    if(!eq(g_base+RVA_CTOR_LV2_LIMIT_IMM,native900,4))return FALSE;
    if(!eq(g_base+RVA_CTOR_LV2_ACTION_IMM,native450,4))return FALSE;
    if(!eq(g_base+RVA_CTOR_LV2_STOP_IMM,native900,4))return FALSE;

    g_cave=allocNear(g_base+RVA_SITE_A);if(!g_cave)return FALSE;
    zero(g_cave,4096);buildSiteAStub();
    if(!FlushInstructionCache(GetCurrentProcess(),g_cave,4096)){VirtualFree(g_cave,0,MEM_RELEASE);g_cave=0;return FALSE;}

    if(!patchJump5(g_base+RVA_SITE_A,g_cave))goto fail;g_patch_mask|=1;
    if(!writeMem(g_base+RVA_LIMIT_A,NEW_LIMIT_A,3))goto fail;g_patch_mask|=2;
    if(!writeMem(g_base+RVA_LIMIT_B,NEW_LIMIT_B,3))goto fail;g_patch_mask|=4;

    u8 v[4];
    wr32(v,F5000);if(!writeMem(g_base+RVA_CTOR_LV1_LIMIT_IMM,v,4))goto fail;g_ctor_patch_mask|=1;
    wr32(v,F2500);if(!writeMem(g_base+RVA_CTOR_LV1_ACTION_IMM,v,4))goto fail;g_ctor_patch_mask|=2;
    wr32(v,F5000);if(!writeMem(g_base+RVA_CTOR_LV1_STOP_IMM,v,4))goto fail;g_ctor_patch_mask|=4;
    wr32(v,F5000);if(!writeMem(g_base+RVA_CTOR_LV2_LIMIT_IMM,v,4))goto fail;g_ctor_patch_mask|=8;
    wr32(v,F2500);if(!writeMem(g_base+RVA_CTOR_LV2_ACTION_IMM,v,4))goto fail;g_ctor_patch_mask|=16;
    wr32(v,F5000);if(!writeMem(g_base+RVA_CTOR_LV2_STOP_IMM,v,4))goto fail;g_ctor_patch_mask|=32;
    return TRUE;

fail:
    rollback();
    return FALSE;
}

static void updateLiveConfig(){
    if(!g_base)return;
    u64 root=0,cfg=0,vt=0;
    if(!read64(g_base+RVA_GLOBAL_CONFIG_ROOT,root)||!root)return;
    if(!read64((u8*)root+OFF_ROOT_CONFIG,cfg)||!cfg)return;
    if(!read64((u8*)cfg,vt))return;
    if(vt!=(u64)(g_base+RVA_DSBAGGAGE_CONFIG_VTABLE))return;

    u32 a=0,b=0,c=0,d=0,e=0,f=0;
    if(!read32((u8*)cfg+OFF_LV1_LIMIT,a)||!read32((u8*)cfg+OFF_LV1_ACTION,b)||!read32((u8*)cfg+OFF_LV1_STOP,c)||
       !read32((u8*)cfg+OFF_LV2_LIMIT,d)||!read32((u8*)cfg+OFF_LV2_ACTION,e)||!read32((u8*)cfg+OFF_LV2_STOP,f))return;

    // Fail closed: every field must still be either its native value or this mod's target value.
    if(!((a==F300||a==F5000)&&(b==F150||b==F2500)&&(c==F300||c==F5000)&&
         (d==F900||d==F5000)&&(e==F450||e==F2500)&&(f==F900||f==F5000)))return;

    if(a!=F5000)writeData32((u8*)cfg+OFF_LV1_LIMIT,F5000);
    if(b!=F2500)writeData32((u8*)cfg+OFF_LV1_ACTION,F2500);
    if(c!=F5000)writeData32((u8*)cfg+OFF_LV1_STOP,F5000);
    if(d!=F5000)writeData32((u8*)cfg+OFF_LV2_LIMIT,F5000);
    if(e!=F2500)writeData32((u8*)cfg+OFF_LV2_ACTION,F2500);
    if(f!=F5000)writeData32((u8*)cfg+OFF_LV2_STOP,F5000);
}

static DWORD WINAPI worker(LPVOID){
    if(!isDS2())return 0;
    Sleep(1200);
    if(!install())return 0;
    // The live singleton may be created or recreated after the ASI is loaded.
    for(;;){updateLiveConfig();Sleep(500);}
}

static void start(){
    if(_InterlockedCompareExchange(&g_started,1,0)!=0)return;
    HANDLE h=CreateThread(0,0,worker,0,0,0);if(h)CloseHandle(h);
}

extern "C" __declspec(dllexport) void InitializeASI(){start();}
extern "C" BOOL WINAPI DllMain(HMODULE h,DWORD r,LPVOID){if(r==DLL_PROCESS_ATTACH){DisableThreadLibraryCalls(h);start();}return TRUE;}
