#include "windows.h"

extern "C" void* memcpy(void*d,const void*s,SIZE_T n){u8*a=(u8*)d;const u8*b=(const u8*)s;for(SIZE_T i=0;i<n;i++)a[i]=b[i];return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){u8*a=(u8*)d;for(SIZE_T i=0;i<n;i++)a[i]=(u8)v;return d;}

#define MOD_VERSION "1.1.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46UL
#define EXPECTED_IMAGE 0x0B292000UL
#define RAXOFF 0x78
#define RCXOFF 0x80
#define RDXOFF 0x88
#define RBPOFF 0xA0
#define R8OFF 0xB8
#define R10OFF 0xC8
#define R12OFF 0xD8
#define R13OFF 0xE0
#define R14OFF 0xE8
#define EFLAGSOFF 0x44
#define RIPOFF 0xF8

enum { KIND_LIKES=1, KIND_RANK=2, KIND_RECORD=3, KIND_PERSIST=4, KIND_EVAL=5, KIND_BESTRANK=6 };
struct HookSite { u64 rva; u8 bytes[5]; u8 length; u8 kind; u16 sourceReg; u8 original; volatile LONG armed; };
static HookSite g_sites[]={
    {0x1420EA7ULL,{0xE8,0xE4,0xB6,0xFF,0xFF},5,KIND_LIKES,R8OFF,0,0},
    {0x142CDF1ULL,{0xE8,0x9A,0xF7,0xFE,0xFF},5,KIND_LIKES,R10OFF,0,0},
    {0x142E251ULL,{0xE8,0x3A,0xE3,0xFE,0xFF},5,KIND_LIKES,R10OFF,0,0},
    {0x1430ED2ULL,{0xE8,0xB9,0xB6,0xFE,0xFF},5,KIND_LIKES,RDXOFF,0,0},
    {0x1430FF1ULL,{0xE8,0x9A,0xB5,0xFE,0xFF},5,KIND_LIKES,RDXOFF,0,0},
    {0x1433648ULL,{0xE8,0x43,0x8F,0xFE,0xFF},5,KIND_LIKES,RDXOFF,0,0},
    {0x1433B94ULL,{0xE8,0xF7,0x89,0xFE,0xFF},5,KIND_LIKES,RDXOFF,0,0},
    {0x1433C81ULL,{0xE8,0x0A,0x89,0xFE,0xFF},5,KIND_LIKES,RDXOFF,0,0},
    {0x1420E78ULL,{0xA8,0x01},2,KIND_RANK,R8OFF,0,0},
    {0x14213D1ULL,{0xA8,0x01},2,KIND_RANK,RBPOFF,0,0},
    {0x1421453ULL,{0xA8,0x01},2,KIND_RANK,RBPOFF,0,0},
    {0x142147FULL,{0xA8,0x01},2,KIND_RANK,RBPOFF,0,0},
    {0x144B09CULL,{0x41,0x88,0x46,0x29},4,KIND_RECORD,R12OFF,0,0},
    {0x144A3E1ULL,{0xE8,0x4A,0x04,0x00,0x00},5,KIND_PERSIST,RDXOFF,0,0},
    {0x1449BABULL,{0xFF,0x50,0x20},3,KIND_EVAL,RDXOFF,0,0},
    {0x1449D0FULL,{0x0F,0x44,0xC8},3,KIND_BESTRANK,RBPOFF,0,0}
};
#define SITE_COUNT (sizeof(g_sites)/sizeof(g_sites[0]))
struct StepSlot { volatile LONG tid; LONG site; };
static StepSlot g_steps[32]={};
static HMODULE g_self=0,g_mod=0;
static LPVOID g_veh=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static volatile LONG g_shutdown=0,g_active=0;
static u64 g_moduleBase=0;
static bool g_debugLog=false,g_likes=true,g_rank=true;

static bool canon(u64 p){return p>=0x10000ULL&&p<0x800000000000ULL;}
static bool readableProtect(DWORD q){q&=255U;return q==PAGE_READONLY||q==PAGE_READWRITE||q==PAGE_WRITECOPY||q==PAGE_EXECUTE||q==PAGE_EXECUTE_READ||q==PAGE_EXECUTE_READWRITE||q==PAGE_EXECUTE_WRITECOPY;}
static bool rd(u64 p,void*o,u64 n){if(!canon(p)||!o||!n)return false;MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD)||!readableProtect(m.Protect))return false;u64 e=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize;if(e<p||e-p<n)return false;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)p,o,(SIZE_T)n,&got)&&got==n;}
static bool wrStackDword(u64 p,u32 value){
    if(!canon(p))return false;
    MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));
    if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD)||(m.Protect&255U)!=PAGE_READWRITE)return false;
    u64 end=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize;if(end<p||end-p<4ULL)return false;
    *(volatile u32*)(ULONG_PTR)p=value;
    u32 verify=0;return rd(p,&verify,4)&&verify==value;
}
static u64 cg(CONTEXT*c,u32 o){u64 v=0;memcpy(&v,c->raw+o,8);return v;}
static void cs(CONTEXT*c,u32 o,u64 v){memcpy(c->raw+o,&v,8);}
static void modulePath(WCHAR*out,const WCHAR*name){if(!out)return;out[0]=0;DWORD n=GetModuleFileNameW(g_self,out,520);if(!n||n>=520){out[0]=0;return;}DWORD slash=0;for(DWORD i=0;i<n;i++)if(out[i]=='\\'||out[i]=='/')slash=i+1;DWORD j=0;while(name[j]&&slash+j+1<520){out[slash+j]=name[j];j++;}if(name[j]){out[0]=0;return;}out[slash+j]=0;}
static void logRaw(const char*message,DWORD len){if(!g_debugLog||g_log==INVALID_HANDLE_VALUE)return;DWORD written=0;WriteFile(g_log,message,len,&written,0);FlushFileBuffers(g_log);}
static void logString(const char*message){DWORD n=0;while(message[n])n++;logRaw(message,n);}
static void logEvent(const HookSite*site,bool patch,u8 selector,u8 flags){if(!g_debugLog)return;char b[144];DWORD n=0;const char*prefix=site->kind==KIND_BESTRANK?(patch?"BESTRANK PATCH rank=0x":"BESTRANK PASS rank=0x"):(site->kind==KIND_EVAL?(patch?"EVAL PATCH selector=0x":"EVAL PASS selector=0x"):(site->kind==KIND_PERSIST?(patch?"PERSIST PATCH selector=0x":"PERSIST PASS selector=0x"):(site->kind==KIND_RANK?(patch?"RANK PATCH selector=0x":"RANK PASS selector=0x"):(site->kind==KIND_RECORD?(patch?"RECORD PATCH selector=0x":"RECORD PASS selector=0x"):(patch?"LIKES PATCH selector=0x":"LIKES PASS selector=0x")))));while(*prefix&&n+1<sizeof(b))b[n++]=*prefix++;const char hex[]="0123456789ABCDEF";b[n++]=hex[selector>>4];b[n++]=hex[selector&15];const char*mid="; flags=0x";while(*mid&&n+1<sizeof(b))b[n++]=*mid++;b[n++]=hex[flags>>4];b[n++]=hex[flags&15];b[n++]='\r';b[n++]='\n';logRaw(b,n);}
static bool patchByte(u64 target,u8 value){DWORD old=0;if(!VirtualProtect((LPVOID)(ULONG_PTR)target,1,PAGE_EXECUTE_READWRITE,&old))return false;*(volatile u8*)(ULONG_PTR)target=value;FlushInstructionCache(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)target,1);DWORD tmp=0;return VirtualProtect((LPVOID)(ULONG_PTR)target,1,old,&tmp)!=0;}
static bool enabledSite(const HookSite*s){return s->kind==KIND_LIKES?g_likes:g_rank;}
static bool signatureOk(const HookSite*s){u8 b[5]={};if(!rd(g_moduleBase+s->rva,b,s->length))return false;for(DWORD i=0;i<s->length;i++)if(b[i]!=s->bytes[i])return false;u8 prev[3]={};if(s->kind==KIND_LIKES){if(!rd(g_moduleBase+s->rva-3,prev,3)||prev[0]!=0x0F||prev[1]!=0x44||prev[2]!=0xC8)return false;}else if(s->kind==KIND_RANK){if(!rd(g_moduleBase+s->rva-2,prev,2)||prev[0]!=0xD1||prev[1]!=0xE8)return false;}else if(s->kind==KIND_RECORD){if(!rd(g_moduleBase+s->rva-2,prev,2)||prev[0]!=0x24||prev[1]!=0x01)return false;}else if(s->kind==KIND_PERSIST){u8 p7[7]={};if(!rd(g_moduleBase+s->rva-7,p7,7)||p7[0]!=0x48||p7[1]!=0x8D||p7[2]!=0x55||p7[3]!=0xB8||p7[4]!=0x49||p7[5]!=0x8B||p7[6]!=0xCD)return false;}else if(s->kind==KIND_EVAL){u8 p7[7]={};if(!rd(g_moduleBase+s->rva-7,p7,7)||p7[0]!=0x48||p7[1]!=0x8B||p7[2]!=0x01||p7[3]!=0x48||p7[4]!=0x8D||p7[5]!=0x55||p7[6]!=0xB8)return false;}else if(s->kind==KIND_BESTRANK){const u8 expected[11]={0x40,0x84,0x75,0x18,0x41,0x8B,0xCE,0x41,0x0F,0xB6,0xC0};u8 actual[11]={};if(!rd(g_moduleBase+s->rva-11,actual,11))return false;for(u32 i=0;i<11;i++)if(actual[i]!=expected[i])return false;}return true;}
static bool installBreak(HookSite*s){if(s->armed)return true;u8 b=0;if(!rd(g_moduleBase+s->rva,&b,1)||b!=s->bytes[0])return false;s->original=b;if(!patchByte(g_moduleBase+s->rva,0xCC))return false;InterlockedExchange(&s->armed,1);return true;}
static void removeBreak(HookSite*s){if(!s->armed)return;patchByte(g_moduleBase+s->rva,s->original);InterlockedExchange(&s->armed,0);}
static void removeAll(){for(DWORD i=0;i<SITE_COUNT;i++)removeBreak(&g_sites[i]);}
static StepSlot* claimStep(LONG tid,LONG site){for(DWORD i=0;i<32;i++)if(InterlockedCompareExchange(&g_steps[i].tid,tid,0)==0){g_steps[i].site=site;return &g_steps[i];}return 0;}
static StepSlot* findStep(LONG tid){for(DWORD i=0;i<32;i++)if(g_steps[i].tid==tid)return &g_steps[i];return 0;}
static bool getBuild(u32*ts,u32*im){u64 b=g_moduleBase;u32 pe=0,sig=0,t=0,i=0;if(!rd(b+0x3c,&pe,4)||pe>=0x2000)return false;if(!rd(b+pe,&sig,4)||sig!=0x4550)return false;if(!rd(b+pe+8,&t,4)||!rd(b+pe+0x50,&i,4))return false;*ts=t;*im=i;return true;}
static bool boolIni(const WCHAR*key,bool def){WCHAR path[520],buf[32];modulePath(path,L"ds2_no_magellan_evaluation.ini");if(!path[0])return def;buf[0]=0;GetPrivateProfileStringW(L"NoMagellanEvaluation",key,def?L"1":L"0",buf,32,path);return !(buf[0]==L'0'||buf[0]==L'n'||buf[0]==L'N'||buf[0]==L'f'||buf[0]==L'F');}
static WCHAR lowerAscii(WCHAR c){return (c>=L'A'&&c<=L'Z')?(WCHAR)(c+(L'a'-L'A')):c;}
static bool isDs2Process(){WCHAR path[520];DWORD n=GetModuleFileNameW(g_mod,path,520);if(!n||n>=520)return false;DWORD start=0;for(DWORD i=0;i<n;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1;const WCHAR want[]=L"ds2.exe";DWORD j=0;while(want[j]){if(start+j>=n||lowerAscii(path[start+j])!=want[j])return false;j++;}return start+j==n;}
static void openDebugLog(){if(!g_debugLog)return;WCHAR path[520];modulePath(path,L"ds2_no_magellan_evaluation.log");if(!path[0])return;g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(g_log!=INVALID_HANDLE_VALUE){const char h[]="No Magellan Evaluation v" MOD_VERSION "\r\nValidated target: Steam PC 1.10.89.0\r\nRule: preserve native grade in best-rank bitmask, normalize Magellan Likes\r\n";logRaw(h,sizeof(h)-1);}}
static void closeDebugLog(){if(g_log!=INVALID_HANDLE_VALUE){FlushFileBuffers(g_log);CloseHandle(g_log);g_log=INVALID_HANDLE_VALUE;}}

static LONG WINAPI veh(EXCEPTION_POINTERS*e){
    if(!e||!e->ExceptionRecord||!e->ContextRecord||!g_active)return EXCEPTION_CONTINUE_SEARCH;
    CONTEXT*c=e->ContextRecord;LONG tid=(LONG)GetCurrentThreadId();DWORD code=e->ExceptionRecord->ExceptionCode;u64 rip=cg(c,RIPOFF);
    if(code==EXCEPTION_BREAKPOINT){
        u64 exceptionAt=(u64)(ULONG_PTR)e->ExceptionRecord->ExceptionAddress;
        for(LONG i=0;i<(LONG)SITE_COUNT;i++){
            HookSite*s=&g_sites[i];u64 target=g_moduleBase+s->rva;
            if(!s->armed||(exceptionAt!=target&&rip!=target+1ULL))continue;
            StepSlot*step=claimStep(tid,i);if(!step){logString("ERROR: no single-step slot.\r\n");return EXCEPTION_CONTINUE_SEARCH;}
            removeBreak(s);
            u64 source=cg(c,(u32)s->sourceReg);u8 selector=0xFF;u32 flags=0xFFFFFFFFU;
            bool ok=false;
            if(s->kind==KIND_BESTRANK){
                selector=(u8)cg(c,R8OFF);
                ok=rd(source+0x18ULL,&flags,4);
            }else{
                ok=rd(source+0x5d,&selector,1)&&rd(source+0x60,&flags,4);
            }
            bool magellanRank=ok&&selector>=1U&&selector<=8U&&((flags&0x22U)==0x02U);
            bool magellanLikes=magellanRank&&((flags&1U)!=0U);
            bool changed=false;
            if(s->kind==KIND_BESTRANK){
                u64 mission=cg(c,R13OFF);
                u64 packed=0;u32 previousMask=0;
                bool validMission=rd(mission+0x28ULL,&packed,8)&&rd(mission+0x54ULL,&previousMask,4);
                u8 type=(u8)((packed>>32)&0x3FU);
                bool standard=(type==3U||type==4U||type==17U||type==18U||type==19U);
                u64 eflags=cg(c,EFLAGSOFF);
                if(ok&&validMission&&standard&&((flags&0x23U)==0x03U)&&selector>=1U&&selector<=8U&&
                    ((u32)cg(c,RCXOFF)==0U)&&((u32)cg(c,RAXOFF)==selector)&&((eflags&0x40ULL)==0ULL)){
                    cs(c,EFLAGSOFF,eflags|0x40ULL);
                    changed=true;
                }
            }else if(magellanLikes&&s->kind==KIND_LIKES&&((u32)cg(c,RCXOFF)==0U)&&((u8)cg(c,RAXOFF)==selector)){
                cs(c,RCXOFF,(u64)selector);changed=true;
            }else if(magellanRank&&s->kind==KIND_RANK&&(cg(c,RAXOFF)&1ULL)){
                cs(c,RAXOFF,cg(c,RAXOFF)&~1ULL);changed=true;
            }else if(magellanRank&&s->kind==KIND_EVAL&&source+0x48ULL==cg(c,RBPOFF)){
                if(wrStackDword(source+0x60ULL,flags&~2U))changed=true;
            }else if(magellanRank&&s->kind==KIND_PERSIST&&source+0x48ULL==cg(c,RBPOFF)){
                if(wrStackDword(source+0x60ULL,flags&~2U))changed=true;
            }else if(magellanRank&&s->kind==KIND_RECORD&&((u8)cg(c,RAXOFF)==1U)){
                u8 recordedSelector=0xff;
                u64 destination=cg(c,R14OFF);
                if(rd(destination+0x28,&recordedSelector,1)&&recordedSelector==selector){
                    cs(c,RAXOFF,cg(c,RAXOFF)&~1ULL);changed=true;
                }
            }
            if(ok)logEvent(s,changed,selector,(u8)flags);
            cs(c,RIPOFF,target);
            cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)|0x100ULL);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    if(code==EXCEPTION_SINGLE_STEP){
        StepSlot*step=findStep(tid);if(!step)return EXCEPTION_CONTINUE_SEARCH;
        LONG idx=step->site;
        cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)&~0x100ULL);
        if(idx>=0&&idx<(LONG)SITE_COUNT&&g_active&&!installBreak(&g_sites[idx])){
            logString("ERROR: hook re-arm failed; disabling all hooks.\r\n");
            InterlockedExchange(&g_active,0);removeAll();
        }
        step->site=-1;InterlockedExchange(&step->tid,0);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
static DWORD WINAPI worker(LPVOID){
    while(!g_shutdown&&!(g_mod=GetModuleHandleW(0)))Sleep(100);
    if(g_shutdown||!g_mod||!isDs2Process()||!boolIni(L"Enabled",true))return 0;
    g_likes=boolIni(L"RemoveLikePenalty",true);g_rank=boolIni(L"RemoveMagellanRank",true);
    if(!g_likes&&!g_rank)return 0;
    g_debugLog=boolIni(L"DebugLog",false);openDebugLog();
    g_moduleBase=(u64)(ULONG_PTR)g_mod;
    u32 t=0,i=0;
    if(!getBuild(&t,&i)||t!=EXPECTED_TIMESTAMP||i!=EXPECTED_IMAGE){logString("ERROR: unsupported executable metadata; no hooks installed.\r\n");closeDebugLog();return 0;}
    for(DWORD j=0;j<SITE_COUNT;j++)if(enabledSite(&g_sites[j])&&!signatureOk(&g_sites[j])){
        logString("ERROR: hook signature mismatch or conflicting ASI; no hooks installed.\r\n");closeDebugLog();return 0;
    }
    g_veh=AddVectoredExceptionHandler(1,veh);
    if(!g_veh){logString("ERROR: AddVectoredExceptionHandler failed.\r\n");closeDebugLog();return 0;}
    InterlockedExchange(&g_active,1);
    bool success=true;
    for(DWORD j=0;j<SITE_COUNT;j++)if(enabledSite(&g_sites[j])&&!installBreak(&g_sites[j])){success=false;break;}
    if(!success){InterlockedExchange(&g_active,0);removeAll();RemoveVectoredExceptionHandler(g_veh);g_veh=0;logString("ERROR: hook installation failed; rolled back.\r\n");closeDebugLog();return 0;}
    logString("ACTIVE: Likes, native best-rank, evaluation, record and persistence hooks armed.\r\n");
    while(!g_shutdown&&g_active)Sleep(100);
    InterlockedExchange(&g_active,0);removeAll();
    LPVOID v=g_veh;g_veh=0;if(v)RemoveVectoredExceptionHandler(v);
    closeDebugLog();return 0;
}
extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
    if(reason==1){g_self=module;DisableThreadLibraryCalls(module);HANDLE t=CreateThread(0,0,worker,0,0,0);if(t)CloseHandle(t);}
    else if(reason==0){InterlockedExchange(&g_shutdown,1);InterlockedExchange(&g_active,0);removeAll();}
    return TRUE;
}
