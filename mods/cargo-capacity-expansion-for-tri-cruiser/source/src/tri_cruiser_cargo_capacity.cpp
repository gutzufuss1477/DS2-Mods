typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long long SIZE_T;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef const wchar_t* LPCWSTR;
typedef DWORD* LPDWORD;
typedef void* LPSECURITY_ATTRIBUTES;
typedef DWORD (__stdcall *LPTHREAD_START_ROUTINE)(LPVOID);

#define TRUE 1
#define FALSE 0
#define DLL_PROCESS_ATTACH 1
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x8000
#define PAGE_EXECUTE_READWRITE 0x40

extern "C" __declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
extern "C" __declspec(dllimport) LPVOID __stdcall VirtualAlloc(LPVOID,SIZE_T,DWORD,DWORD);
extern "C" __declspec(dllimport) BOOL __stdcall VirtualFree(LPVOID,SIZE_T,DWORD);
extern "C" __declspec(dllimport) BOOL __stdcall VirtualProtect(LPVOID,SIZE_T,DWORD,DWORD*);
extern "C" __declspec(dllimport) BOOL __stdcall FlushInstructionCache(HANDLE,LPCVOID,SIZE_T);
extern "C" __declspec(dllimport) HANDLE __stdcall GetCurrentProcess(void);
extern "C" __declspec(dllimport) HANDLE __stdcall CreateThread(LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
extern "C" __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
extern "C" __declspec(dllimport) BOOL __stdcall DisableThreadLibraryCalls(HMODULE);
extern "C" long _InterlockedCompareExchange(volatile long*,long,long);
#pragma intrinsic(_InterlockedCompareExchange)

#define DECL_STUB(name) \
extern "C" const u8 name##StubStart[]; \
extern "C" const u8 name##StubEnd[]; \
extern "C" const u8 name##ReturnPtr[];
DECL_STUB(SKeyA)
DECL_STUB(SKeyB)
DECL_STUB(MKeyA)
DECL_STUB(MKeyB)
DECL_STUB(LKeyA)
DECL_STUB(LKeyB)
DECL_STUB(XLKeyA)
DECL_STUB(XLKeyB)
DECL_STUB(ItemCap)
DECL_STUB(PackLimitA)
DECL_STUB(PackLimitB)
DECL_STUB(Accessory)

static volatile long g_started = 0;
static u64 g_base = 0;
static u64 g_cave = 0;
static long g_patchMask = 0;

static const u64 RVA_PACK_A0 = 0x01187BF1ULL;
static const u64 RVA_PACK_A1 = 0x01187C41ULL;
static const u64 RVA_PACK_B0 = 0x01187C85ULL;
static const u64 RVA_PACK_B1 = 0x01187CB5ULL;
static const u64 RVA_ACCESSORY = 0x01187BB5ULL;
static const u64 RVA_ITEMCAP = 0x012378ABULL;
static const u64 RVA_HOOK[8] = {
    0x0118A175ULL, 0x0118A2FBULL,
    0x0118A25BULL, 0x0118A3E4ULL,
    0x0118A4A6ULL, 0x0118A512ULL,
    0x0118A58AULL, 0x0118A5FAULL
};

static const u8 EXP_PACK_A0[6] = {0x41,0xB8,0x06,0x00,0x00,0x00};
static const u8 EXP_PACK_A1[3] = {0x83,0xF9,0x06};
static const u8 EXP_PACK_B0[6] = {0x41,0xB8,0x06,0x00,0x00,0x00};
static const u8 EXP_PACK_B1[3] = {0x83,0xFF,0x06};
static const u8 EXP_ACCESSORY[8] = {0x0F,0xB6,0x48,0x39,0x44,0x38,0x60,0x38};
static const u8 EXP_ITEMCAP[5] = {0x41,0x0F,0xB7,0x4F,0x26};
static const u8 EXP_HOOK[8][5] = {
    {0x4C,0x8B,0x64,0x24,0x20}, {0x4C,0x8B,0x64,0x24,0x30},
    {0x4C,0x8B,0x44,0x24,0x28}, {0x4C,0x8B,0x44,0x24,0x38},
    {0x4C,0x8B,0x64,0x24,0x40}, {0x4C,0x8B,0x6C,0x24,0x50},
    {0x4C,0x8B,0x64,0x24,0x48}, {0x4C,0x8B,0x64,0x24,0x58}
};

static BOOL eqbytes(const u8* p,const u8* q,u32 n){
    for(u32 i=0;i<n;i++) if(p[i]!=q[i]) return FALSE;
    return TRUE;
}
static void put32(u8* p,u32 v){p[0]=(u8)v;p[1]=(u8)(v>>8);p[2]=(u8)(v>>16);p[3]=(u8)(v>>24);}
static void put64(u8* p,u64 v){for(u32 i=0;i<8;i++)p[i]=(u8)(v>>(i*8));}
static BOOL rel32ok(u64 fromNext,u64 to){long long d=(long long)to-(long long)fromNext;return d>=(-2147483647LL-1LL)&&d<=2147483647LL;}
static u32 rel32(u64 fromNext,u64 to){return (u32)((long long)to-(long long)fromNext);}
static BOOL writeMem(u8* dst,const u8* src,u32 n){
    DWORD old=0,tmp=0;
    if(!VirtualProtect(dst,n,PAGE_EXECUTE_READWRITE,&old)) return FALSE;
    for(u32 i=0;i<n;i++) dst[i]=src[i];
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),dst,n);
    BOOL restored=VirtualProtect(dst,n,old,&tmp);
    return flushed&&restored;
}
static BOOL patchSpanJump(u8* at,u8* stub,u32 span){
    if(span<5 || span>16 || !rel32ok((u64)at+5,(u64)stub)) return FALSE;
    u8 p[16];
    p[0]=0xE9; put32(p+1,rel32((u64)at+5,(u64)stub));
    for(u32 i=5;i<span;i++) p[i]=0x90;
    return writeMem(at,p,span);
}

struct StubDef{const u8* start;const u8* end;const u8* ret;u64 hookRva;u32 span;};
static StubDef stubs[12]={
    {SKeyAStubStart,SKeyAStubEnd,SKeyAReturnPtr,RVA_HOOK[0],5},
    {SKeyBStubStart,SKeyBStubEnd,SKeyBReturnPtr,RVA_HOOK[1],5},
    {MKeyAStubStart,MKeyAStubEnd,MKeyAReturnPtr,RVA_HOOK[2],5},
    {MKeyBStubStart,MKeyBStubEnd,MKeyBReturnPtr,RVA_HOOK[3],5},
    {LKeyAStubStart,LKeyAStubEnd,LKeyAReturnPtr,RVA_HOOK[4],5},
    {LKeyBStubStart,LKeyBStubEnd,LKeyBReturnPtr,RVA_HOOK[5],5},
    {XLKeyAStubStart,XLKeyAStubEnd,XLKeyAReturnPtr,RVA_HOOK[6],5},
    {XLKeyBStubStart,XLKeyBStubEnd,XLKeyBReturnPtr,RVA_HOOK[7],5},
    {ItemCapStubStart,ItemCapStubEnd,ItemCapReturnPtr,RVA_ITEMCAP,5},
    {PackLimitAStubStart,PackLimitAStubEnd,PackLimitAReturnPtr,RVA_PACK_A0,6},
    {PackLimitBStubStart,PackLimitBStubEnd,PackLimitBReturnPtr,RVA_PACK_B0,6},
    {AccessoryStubStart,AccessoryStubEnd,AccessoryReturnPtr,RVA_ACCESSORY,8}
};
static const u32 STUB_STRIDE=0x100;
static const SIZE_T CAVE_SIZE=0x2000;

static u8* allocNear(u64 target){
    const u64 step=0x100000ULL,maxd=0x70000000ULL;
    for(u64 d=step;d<maxd;d+=step){
        u64 candidates[2]={(target>d)?target-d:0,target+d};
        for(u32 k=0;k<2;k++){
            if(!candidates[k]) continue;
            u64 hint=candidates[k]&~0xFFFFULL;
            u8* p=(u8*)VirtualAlloc((LPVOID)hint,CAVE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
            if(!p) continue;
            BOOL ok=TRUE;
            for(u32 i=0;i<12;i++) if(!rel32ok(g_base+stubs[i].hookRva+5,(u64)p+i*STUB_STRIDE)){ok=FALSE;break;}
            if(ok) return p;
            VirtualFree(p,0,MEM_RELEASE);
        }
    }
    return 0;
}

static BOOL rollback(void){
    BOOL ok=TRUE;
    if(!g_base) return TRUE;
    if(g_patchMask&1) ok=writeMem((u8*)(g_base+RVA_PACK_A0),EXP_PACK_A0,6)&&ok;
    if(g_patchMask&2) ok=writeMem((u8*)(g_base+RVA_PACK_A1),EXP_PACK_A1,3)&&ok;
    if(g_patchMask&4) ok=writeMem((u8*)(g_base+RVA_PACK_B0),EXP_PACK_B0,6)&&ok;
    if(g_patchMask&8) ok=writeMem((u8*)(g_base+RVA_PACK_B1),EXP_PACK_B1,3)&&ok;
    for(u32 i=0;i<8;i++) if(g_patchMask&(1L<<(i+4))) ok=writeMem((u8*)(g_base+RVA_HOOK[i]),EXP_HOOK[i],5)&&ok;
    if(g_patchMask&(1L<<12)) ok=writeMem((u8*)(g_base+RVA_ITEMCAP),EXP_ITEMCAP,5)&&ok;
    if(g_patchMask&(1L<<13)) ok=writeMem((u8*)(g_base+RVA_ACCESSORY),EXP_ACCESSORY,8)&&ok;
    return ok;
}

static BOOL install(void){
    g_base=(u64)GetModuleHandleW(0);
    if(!g_base) return FALSE;

    // Fail closed on unsupported executables or conflicting patches.
    if(!eqbytes((u8*)(g_base+RVA_PACK_A0),EXP_PACK_A0,6)) return FALSE;
    if(!eqbytes((u8*)(g_base+RVA_PACK_A1),EXP_PACK_A1,3)) return FALSE;
    if(!eqbytes((u8*)(g_base+RVA_PACK_B0),EXP_PACK_B0,6)) return FALSE;
    if(!eqbytes((u8*)(g_base+RVA_PACK_B1),EXP_PACK_B1,3)) return FALSE;
    if(!eqbytes((u8*)(g_base+RVA_ACCESSORY),EXP_ACCESSORY,8)) return FALSE;
    for(u32 i=0;i<8;i++) if(!eqbytes((u8*)(g_base+RVA_HOOK[i]),EXP_HOOK[i],5)) return FALSE;
    if(!eqbytes((u8*)(g_base+RVA_ITEMCAP),EXP_ITEMCAP,5)) return FALSE;

    u8* cave=allocNear(g_base+RVA_HOOK[0]);
    if(!cave) return FALSE;
    g_cave=(u64)cave;

    for(u32 i=0;i<12;i++){
        SIZE_T sz=(SIZE_T)(stubs[i].end-stubs[i].start);
        SIZE_T ro=(SIZE_T)(stubs[i].ret-stubs[i].start);
        if(!sz||sz>STUB_STRIDE||ro+8>sz){VirtualFree(cave,0,MEM_RELEASE);g_cave=0;return FALSE;}
        for(SIZE_T j=0;j<sz;j++) cave[i*STUB_STRIDE+j]=stubs[i].start[j];
        put64(cave+i*STUB_STRIDE+ro,g_base+stubs[i].hookRva+stubs[i].span);
    }
    if(!FlushInstructionCache(GetCurrentProcess(),cave,CAVE_SIZE)){VirtualFree(cave,0,MEM_RELEASE);g_cave=0;return FALSE;}

    // The compare instructions use R8D. PackLimitA/B load the effective limit:
    // regular two-side mode = 160 per side (320 total),
    // native one-side mode = full 320 on the active virtual cargo side.
    const u8 P_A1[3]={0x44,0x39,0xC1}; // cmp ecx,r8d
    const u8 P_B1[3]={0x44,0x39,0xC7}; // cmp edi,r8d

    if(!patchSpanJump((u8*)(g_base+RVA_PACK_A0),cave+9*STUB_STRIDE,6)) goto fail; g_patchMask|=1;
    if(!writeMem((u8*)(g_base+RVA_PACK_A1),P_A1,3)) goto fail; g_patchMask|=2;
    if(!patchSpanJump((u8*)(g_base+RVA_PACK_B0),cave+10*STUB_STRIDE,6)) goto fail; g_patchMask|=4;
    if(!writeMem((u8*)(g_base+RVA_PACK_B1),P_B1,3)) goto fail; g_patchMask|=8;
    for(u32 i=0;i<8;i++){
        if(!patchSpanJump((u8*)(g_base+RVA_HOOK[i]),cave+i*STUB_STRIDE,5)) goto fail;
        g_patchMask|=(1L<<(i+4));
    }
    if(!patchSpanJump((u8*)(g_base+RVA_ITEMCAP),cave+8*STUB_STRIDE,5)) goto fail;
    g_patchMask|=(1L<<12);
    if(!patchSpanJump((u8*)(g_base+RVA_ACCESSORY),cave+11*STUB_STRIDE,8)) goto fail;
    g_patchMask|=(1L<<13);
    return TRUE;

fail:
    rollback();
    VirtualFree(cave,0,MEM_RELEASE);
    g_cave=0;
    return FALSE;
}

static DWORD __stdcall Worker(LPVOID){install();return 0;}
static void StartOnce(void){
    if(_InterlockedCompareExchange(&g_started,1,0)!=0) return;
    HANDLE h=CreateThread(0,0,Worker,0,0,0);
    if(h) CloseHandle(h);
}

extern "C" __declspec(dllexport) void InitializeASI(void){StartOnce();}
extern "C" BOOL __stdcall DllMain(HMODULE h,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){DisableThreadLibraryCalls(h);StartOnce();}
    return TRUE;
}
