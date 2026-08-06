#include "windows.h"
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){u8*a=(u8*)d;const u8*b=(const u8*)s;for(SIZE_T i=0;i<n;i++)a[i]=b[i];return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){u8*a=(u8*)d;for(SIZE_T i=0;i<n;i++)a[i]=(u8)v;return d;}
#define TARGET_RVA 0x1E1335DULL
#define EXPECTED_TIMESTAMP 0x6A3DAE46UL
#define EXPECTED_IMAGE 0x0B292000UL
#define DR0 0x48
#define DR6 0x68
#define DR7 0x70
#define RCXOFF 0x80
#define RDXOFF 0x88
#define R8OFF 0xB8
#define RBXOFF 0x90
#define R9OFF 0xC0
#define RSPOFF 0x98
#define RIPOFF 0xF8
#define EFLAGSOFF 0x44
static HMODULE g_self,g_mod; static LPVOID g_veh; static volatile LONG g_active,g_shutdown,g_pending; static volatile u64 g_lastRecord,g_lastPre,g_extraLikes=5000;
static bool canon(u64 p){return p>=0x10000&&p<0x800000000000ULL;}
static bool rd(u64 p,void*o,u64 n){if(!canon(p)||!o||!n)return false;MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD))return false;DWORD q=m.Protect&255;if(q!=2&&q!=4&&q!=8&&q!=32&&q!=64&&q!=128)return false;u64 e=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize; if(e<p||e-p<n)return false;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)p,o,(SIZE_T)n,&got)&&got==n;}
static u64 q(u64 p,bool*ok){u64 v=0;*ok=rd(p,&v,8);return v;}
static u64 cg(CONTEXT*c,u32 o){u64 v=0;memcpy(&v,c->raw+o,8);return v;} static void cs(CONTEXT*c,u32 o,u64 v){memcpy(c->raw+o,&v,8);}
static void loadConfig(){g_extraLikes=5000;WCHAR path[520],buf[64];DWORD n=GetModuleFileNameW(g_self,path,520);if(!n||n>=520)return;DWORD slash=0;for(DWORD i=0;i<n;i++)if(path[i]=='\\'||path[i]=='/')slash=i+1;const WCHAR name[]=L"ds2_lost_cargo_likes_booster.ini";DWORD j=0;while(name[j]&&slash+j+1<520){path[slash+j]=name[j];j++;}if(name[j])return;path[slash+j]=0;DWORD got=GetPrivateProfileStringW(L"LostCargoLikesBooster",L"ExtraLikes",L"",buf,64,path);if(!got||got>=64)return;u64 v=0;for(DWORD i=0;i<got;i++){if(buf[i]<'0'||buf[i]>'9')return;v=v*10+(u64)(buf[i]-'0');if(v>1000000)return;}if(v)g_extraLikes=v;}
static bool valid(){u64 b=(u64)(ULONG_PTR)g_mod;u32 po=0,s=0,ts=0,im=0;return rd(b+0x3c,&po,4)&&po<0x1000&&rd(b+po,&s,4)&&s==0x4550&&rd(b+po+8,&ts,4)&&ts==EXPECTED_TIMESTAMP&&rd(b+po+0x50,&im,4)&&im==EXPECTED_IMAGE;}
static LONG WINAPI veh(EXCEPTION_POINTERS*e){if(!e||!e->ExceptionRecord||!e->ContextRecord||e->ExceptionRecord->ExceptionCode!=EXCEPTION_SINGLE_STEP)return EXCEPTION_CONTINUE_SEARCH;CONTEXT*c=e->ContextRecord;u64 dr6=cg(c,DR6),rip=cg(c,RIPOFF),target=(u64)(ULONG_PTR)g_mod+TARGET_RVA;DWORD tid=GetCurrentThreadId();if(!g_active)return EXCEPTION_CONTINUE_SEARCH;if(g_pending==(LONG)tid&&rip!=target){cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)&~0x100ULL);cs(c,DR6,0);cs(c,DR7,cg(c,DR7)|1ULL);g_pending=0;return EXCEPTION_CONTINUE_EXECUTION;}if(!(dr6&1)||rip!=target)return EXCEPTION_CONTINUE_SEARCH;cs(c,DR6,0);cs(c,DR7,cg(c,DR7)&~1ULL);u64 sp=cg(c,RSPOFF),in=cg(c,RCXOFF),r8=cg(c,R8OFF),r9=cg(c,R9OFF),rbx=cg(c,RBXOFF);bool ok=0;u64 caller=q(sp,&ok),pre=0;bool preok=false;if(canon(r9))pre=q(r9,&preok);bool eligible=ok&&caller==(u64)(ULONG_PTR)g_mod+0x1E14C74ULL&&rbx==0x22&&r8==0&&preok&&pre<=0xFFFFFFFFFFFFFFDDULL&&in==pre+rbx&&in<=0x7FFFFFFFFFFFFFFFULL-g_extraLikes&&(g_lastRecord!=r9||g_lastPre!=pre);if(eligible){cs(c,RCXOFF,in+g_extraLikes);g_lastRecord=r9;g_lastPre=pre;}g_pending=(LONG)tid;cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)|0x100ULL);return EXCEPTION_CONTINUE_EXECUTION;}
static bool bp(DWORD tid,bool en){if(tid==GetCurrentThreadId())return false;if(en&&(LONG)tid==g_pending)return false;HANDLE h=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,tid);if(!h)return false;SuspendThread(h);CONTEXT c;memset(&c,0,sizeof(c));DWORD fl=CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_DEBUG_REGISTERS;memcpy(c.raw+0x30,&fl,4);bool ok=GetThreadContext(h,&c);if(ok){cs(&c,DR0,en?(u64)(ULONG_PTR)g_mod+TARGET_RVA:0);cs(&c,DR6,0);u64 d=cg(&c,DR7);cs(&c,DR7,en?(d|1):(d&~1ULL));ok=SetThreadContext(h,&c);}ResumeThread(h);CloseHandle(h);return ok;}
static void scan(bool en){HANDLE s=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(s==INVALID_HANDLE_VALUE)return;THREADENTRY32 t;memset(&t,0,sizeof(t));t.dwSize=sizeof(t);DWORD p=GetCurrentProcessId();if(Thread32First(s,&t)){do{if(t.th32OwnerProcessID==p)bp(t.th32ThreadID,en);t.dwSize=sizeof(t);}while(Thread32Next(s,&t));}CloseHandle(s);}
static DWORD WINAPI worker(LPVOID){while(!g_shutdown&&!(g_mod=GetModuleHandleW(0)))Sleep(250);if(g_shutdown||!g_mod||!valid())return 0;loadConfig();g_veh=AddVectoredExceptionHandler(1,veh);if(!g_veh)return 0;g_active=1;scan(1);while(!g_shutdown&&g_active){scan(1);Sleep(250);}g_active=0;scan(0);LPVOID v=g_veh;g_veh=0;if(v)RemoveVectoredExceptionHandler(v);return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD r,LPVOID){if(r==1){g_self=module;DisableThreadLibraryCalls(module);HANDLE t=CreateThread(0,0,worker,0,0,0);if(t)CloseHandle(t);}else if(r==0){g_shutdown=1;g_active=0;}return TRUE;}
