#include "windows.h"
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){u8*a=(u8*)d;const u8*b=(const u8*)s;for(SIZE_T i=0;i<n;i++)a[i]=b[i];return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){u8*a=(u8*)d;for(SIZE_T i=0;i<n;i++)a[i]=(u8)v;return d;}

#define LIKE_TARGET_RVA 0x1E1335DULL
#define LIKE_CALLER_RVA 0x1E14C74ULL
#define EXPECTED_TIMESTAMP 0x6A3DAE46UL
#define EXPECTED_IMAGE 0x0B292000UL

#define DR0 0x48
#define DR1 0x50
#define DR6 0x68
#define DR7 0x70
#define RCXOFF 0x80
#define RBXOFF 0x90
#define RSPOFF 0x98
#define RSIOFF 0xA8
#define RDIOFF 0xB0
#define R8OFF 0xB8
#define R9OFF 0xC0
#define RIPOFF 0xF8
#define EFLAGSOFF 0x44
#define PENDING_MAX_TICKS 20U

static HMODULE g_self,g_mod;
static LPVOID g_veh;
static volatile LONG g_active,g_shutdown,g_stepTid,g_bonusArmed,g_workerTick;
static volatile u64 g_extraLikes=5000,g_connTarget=0,g_pendingRecord=0,g_pendingBoosted=0;
static volatile DWORD g_pendingTid=0;
static volatile u32 g_pendingTick=0;

static bool canon(u64 p){return p>=0x10000&&p<0x800000000000ULL;}
static bool rd(u64 p,void*o,u64 n){if(!canon(p)||!o||!n)return false;MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD))return false;DWORD q=m.Protect&255;if(q!=2&&q!=4&&q!=8&&q!=16&&q!=32&&q!=64&&q!=128)return false;u64 e=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize;if(e<p||e-p<n)return false;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)p,o,(SIZE_T)n,&got)&&got==n;}
static bool wr32(u64 p,u32 v){if(!canon(p))return false;MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD))return false;DWORD q=m.Protect&255;if(q!=4&&q!=8&&q!=64&&q!=128)return false;u64 e=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize;if(e<p||e-p<4)return false;*(volatile u32*)(ULONG_PTR)p=v;return true;}
static u64 q64(u64 p,bool*ok){u64 v=0;*ok=rd(p,&v,8);return v;}
static u32 q32(u64 p,bool*ok){u32 v=0;*ok=rd(p,&v,4);return v;}
static u64 cg(CONTEXT*c,u32 o){u64 v=0;memcpy(&v,c->raw+o,8);return v;}
static void cs(CONTEXT*c,u32 o,u64 v){memcpy(c->raw+o,&v,8);}

static void loadConfig(){g_extraLikes=5000;WCHAR path[520],buf[64];DWORD n=GetModuleFileNameW(g_self,path,520);if(!n||n>=520)return;DWORD slash=0;for(DWORD i=0;i<n;i++)if(path[i]=='\\'||path[i]=='/')slash=i+1;const WCHAR name[]=L"ds2_lost_cargo_likes_booster.ini";DWORD j=0;while(name[j]&&slash+j+1<520){path[slash+j]=name[j];j++;}if(name[j])return;path[slash+j]=0;DWORD got=GetPrivateProfileStringW(L"LostCargoLikesBooster",L"ExtraLikes",L"",buf,64,path);if(!got||got>=64)return;u64 v=0;for(DWORD i=0;i<got;i++){if(buf[i]<'0'||buf[i]>'9')return;v=v*10+(u64)(buf[i]-'0');if(v>1000000)return;}if(v)g_extraLikes=v;}
static bool valid(){u64 b=(u64)(ULONG_PTR)g_mod;u32 po=0,s=0,ts=0,im=0;return rd(b+0x3c,&po,4)&&po<0x1000&&rd(b+po,&s,4)&&s==0x4550&&rd(b+po+8,&ts,4)&&ts==EXPECTED_TIMESTAMP&&rd(b+po+0x50,&im,4)&&im==EXPECTED_IMAGE;}
static bool pat(const u8*p){static const u8 x[]={0x8B,0x56,0x08,0x44,0x8B,0xC0,0x48,0x8B,0xCF,0xE8,0,0,0,0,0x45,0x33,0xE4,0x44,0x8B,0xF8,0x44,0x3B,0xF0};for(u32 i=0;i<23;i++){if(i>=10&&i<=13)continue;if(p[i]!=x[i])return false;}return true;}
static u64 findConn(){u64 base=(u64)(ULONG_PTR)g_mod,end=base+EXPECTED_IMAGE,cur=base,found=0;u32 count=0;while(cur<end){MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)cur,&m,sizeof(m))!=sizeof(m))break;u64 rb=(u64)(ULONG_PTR)m.BaseAddress,re=rb+m.RegionSize;if(re<=cur)break;DWORD q=m.Protect&255;bool exec=m.State==MEM_COMMIT&&!(m.Protect&PAGE_GUARD)&&(q==16||q==32||q==64||q==128);u64 s=cur>rb?cur:rb,uend=re<end?re:end;if(exec&&uend>s+23){const u8*p=(const u8*)(ULONG_PTR)s;u64 n=uend-s;for(u64 i=0;i+23<=n;i++){if(pat(p+i)){found=s+i;count++;if(count>1)return 0;}}}cur=re;}return count==1?found:0;}
static void clearPending(){g_pendingRecord=0;g_pendingBoosted=0;g_pendingTid=0;g_pendingTick=0;InterlockedExchange(&g_bonusArmed,0);}
static bool pendingExpired(){return ((u32)g_workerTick-g_pendingTick)>=PENDING_MAX_TICKS;}
static bool samePending(u64 record,DWORD tid){return g_bonusArmed&&g_pendingRecord==record&&g_pendingTid==tid;}

static LONG WINAPI veh(EXCEPTION_POINTERS*e){
    if(!e||!e->ExceptionRecord||!e->ContextRecord||e->ExceptionRecord->ExceptionCode!=EXCEPTION_SINGLE_STEP)return EXCEPTION_CONTINUE_SEARCH;
    CONTEXT*c=e->ContextRecord;u64 dr6=cg(c,DR6),rip=cg(c,RIPOFF),like=(u64)(ULONG_PTR)g_mod+LIKE_TARGET_RVA,conn=g_connTarget;DWORD tid=GetCurrentThreadId();
    if(!g_active)return EXCEPTION_CONTINUE_SEARCH;
    if(g_stepTid==(LONG)tid&&rip!=like&&rip!=conn){cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)&~0x100ULL);cs(c,DR6,0);u64 d=cg(c,DR7)|1ULL;if(conn)d|=4ULL;cs(c,DR7,d);g_stepTid=0;return EXCEPTION_CONTINUE_EXECUTION;}
    if((dr6&1)&&rip==like){
        cs(c,DR6,0);cs(c,DR7,cg(c,DR7)&~1ULL);
        u64 sp=cg(c,RSPOFF),in=cg(c,RCXOFF),r8=cg(c,R8OFF),r9=cg(c,R9OFF),rbx=cg(c,RBXOFF);bool ok=0;u64 caller=q64(sp,&ok),pre=0;bool preok=false;if(canon(r9))pre=q64(r9,&preok);
        bool eligible=ok&&caller==(u64)(ULONG_PTR)g_mod+LIKE_CALLER_RVA&&rbx>1&&r8==0&&preok&&pre<=0xFFFFFFFFFFFFFFDDULL&&in==pre+rbx&&in<=0x7FFFFFFFFFFFFFFFULL-g_extraLikes;
        if(eligible){
            bool same=samePending(r9,tid);
            if(g_bonusArmed&&(!same||pendingExpired()))clearPending();
            if(!g_bonusArmed&&InterlockedCompareExchange(&g_bonusArmed,1,0)==0){u64 boosted=in+g_extraLikes;g_pendingRecord=r9;g_pendingBoosted=boosted;g_pendingTid=tid;g_pendingTick=(u32)g_workerTick;cs(c,RCXOFF,boosted);}
        }
        g_stepTid=(LONG)tid;cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)|0x100ULL);return EXCEPTION_CONTINUE_EXECUTION;
    }
    if(conn&&(dr6&2)&&rip==conn){
        cs(c,DR6,0);cs(c,DR7,cg(c,DR7)&~4ULL);
        u64 rsi=cg(c,RSIOFF),record=rsi+0x18ULL;bool cOk=0,vOk=0;u32 current=q32(rsi+8,&cOk);u64 visible=q64(record,&vOk);
        if(samePending(record,tid)){
            u64 synced=(u64)current+g_extraLikes;
            if(cOk&&vOk&&visible>=g_pendingBoosted&&synced<=0xFFFFFFFFULL&&visible>=synced)wr32(rsi+8,(u32)synced);
            clearPending();
        }else if(g_bonusArmed&&pendingExpired())clearPending();
        g_stepTid=(LONG)tid;cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)|0x100ULL);return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static bool bp(DWORD tid,bool en){if(tid==GetCurrentThreadId())return false;if(en&&(LONG)tid==g_stepTid)return false;HANDLE h=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,tid);if(!h)return false;SuspendThread(h);CONTEXT c;memset(&c,0,sizeof(c));DWORD fl=CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_DEBUG_REGISTERS;memcpy(c.raw+0x30,&fl,4);bool ok=GetThreadContext(h,&c);if(ok){u64 conn=g_connTarget;cs(&c,DR0,en?(u64)(ULONG_PTR)g_mod+LIKE_TARGET_RVA:0);cs(&c,DR1,en?conn:0);cs(&c,DR6,0);u64 d=cg(&c,DR7);if(en){d|=1ULL;if(conn)d|=4ULL;}else d&=~(1ULL|4ULL);cs(&c,DR7,d);ok=SetThreadContext(h,&c);}ResumeThread(h);CloseHandle(h);return ok;}
static void scan(bool en){HANDLE s=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(s==INVALID_HANDLE_VALUE)return;THREADENTRY32 t;memset(&t,0,sizeof(t));t.dwSize=sizeof(t);DWORD p=GetCurrentProcessId();if(Thread32First(s,&t)){do{if(t.th32OwnerProcessID==p)bp(t.th32ThreadID,en);t.dwSize=sizeof(t);}while(Thread32Next(s,&t));}CloseHandle(s);}
static DWORD WINAPI worker(LPVOID){while(!g_shutdown&&!(g_mod=GetModuleHandleW(0)))Sleep(250);if(g_shutdown||!g_mod||!valid())return 0;loadConfig();g_connTarget=findConn();if(!g_connTarget)return 0;g_veh=AddVectoredExceptionHandler(1,veh);if(!g_veh)return 0;g_active=1;scan(1);while(!g_shutdown&&g_active){scan(1);Sleep(250);InterlockedIncrement(&g_workerTick);}g_active=0;scan(0);LPVOID v=g_veh;g_veh=0;if(v)RemoveVectoredExceptionHandler(v);return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD r,LPVOID){if(r==1){g_self=module;DisableThreadLibraryCalls(module);HANDLE t=CreateThread(0,0,worker,0,0,0);if(t)CloseHandle(t);}else if(r==0){g_shutdown=1;g_active=0;}return TRUE;}
