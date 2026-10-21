#include "windows.h"

extern "C" void* memcpy(void*d,const void*s,SIZE_T n){u8*a=(u8*)d;const u8*b=(const u8*)s;for(SIZE_T i=0;i<n;i++)a[i]=b[i];return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){u8*a=(u8*)d;for(SIZE_T i=0;i<n;i++)a[i]=(u8)v;return d;}

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46UL
#define EXPECTED_IMAGE 0x0B292000UL
#define SELECTOR_LOOKUP_CALL_RVA 0x1430FF1ULL
#define SELECTOR_OFFSET 0x5DULL
#define FLAGS_OFFSET 0x60ULL
#define NORMAL_SELECTOR 2ULL

#define EFLAGSOFF 0x44
#define RCXOFF 0x80
#define RDXOFF 0x88
#define RIPOFF 0xF8

static HMODULE g_self=0,g_mod=0;
static LPVOID g_veh=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static volatile LONG g_shutdown=0,g_active=0,g_stepTid=0;
static u64 g_moduleBase=0,g_target=0;
static u32 g_actualTimestamp=0,g_actualImage=0;
static u8 g_originalByte=0;
static bool g_breakInstalled=false,g_debugLog=false;
static const u8 g_expected[9]={0xE8,0x9A,0xB5,0xFE,0xFF,0xC5,0xF8,0x28,0xF0};

static bool canon(u64 p){return p>=0x10000ULL&&p<0x800000000000ULL;}
static bool readableProtect(DWORD q){q&=255U;return q==PAGE_READONLY||q==PAGE_READWRITE||q==PAGE_WRITECOPY||q==PAGE_EXECUTE||q==PAGE_EXECUTE_READ||q==PAGE_EXECUTE_READWRITE||q==PAGE_EXECUTE_WRITECOPY;}
static bool rd(u64 p,void*o,u64 n){if(!canon(p)||!o||!n)return false;MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD)||!readableProtect(m.Protect))return false;u64 e=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize;if(e<p||e-p<n)return false;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)p,o,(SIZE_T)n,&got)&&got==n;}
static u64 cg(CONTEXT*c,u32 o){u64 v=0;memcpy(&v,c->raw+o,8);return v;}
static void cs(CONTEXT*c,u32 o,u64 v){memcpy(c->raw+o,&v,8);}

static void modulePath(WCHAR*out,const WCHAR*name){if(!out)return;out[0]=0;DWORD n=GetModuleFileNameW(g_self,out,520);if(!n||n>=520){out[0]=0;return;}DWORD slash=0;for(DWORD i=0;i<n;i++)if(out[i]=='\\'||out[i]=='/')slash=i+1;DWORD j=0;while(name[j]&&slash+j+1<520){out[slash+j]=name[j];j++;}if(name[j]){out[0]=0;return;}out[slash+j]=0;}
static void logRaw(const char*text,DWORD len){if(!g_debugLog||g_log==INVALID_HANDLE_VALUE)return;DWORD w=0;WriteFile(g_log,text,len,&w,0);FlushFileBuffers(g_log);}
static void logEvent(bool patched,u8 selector,u8 flags){char b[128];DWORD n=0;const char*pre=patched?"PATCH ECX 0->2; selector=0x":"PASS selector=0x";for(DWORD i=0;pre[i]&&n+1<sizeof(b);i++)b[n++]=pre[i];const char hex[]="0123456789ABCDEF";b[n++]=hex[(selector>>4)&15];b[n++]=hex[selector&15];const char*mid="; flags=0x";for(DWORD i=0;mid[i]&&n+1<sizeof(b);i++)b[n++]=mid[i];b[n++]=hex[(flags>>4)&15];b[n++]=hex[flags&15];b[n++]='\r';b[n++]='\n';logRaw(b,n);}

static bool getBuild(u32*ts,u32*im){u64 b=(u64)(ULONG_PTR)g_mod;u32 pe=0,sig=0,t=0,i=0;if(!rd(b+0x3c,&pe,4)||pe>=0x2000)return false;if(!rd(b+pe,&sig,4)||sig!=0x4550)return false;if(!rd(b+pe+8,&t,4)||!rd(b+pe+0x50,&i,4))return false;*ts=t;*im=i;return true;}
static bool signatureOk(){u8 b[9];if(!rd(g_target,b,9))return false;for(u32 i=0;i<9;i++)if(b[i]!=g_expected[i])return false;return true;}
static bool patchByte(u8 value){DWORD old=0;if(!VirtualProtect((LPVOID)(ULONG_PTR)g_target,1,PAGE_EXECUTE_READWRITE,&old))return false;*(volatile u8*)(ULONG_PTR)g_target=value;FlushInstructionCache(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)g_target,1);DWORD tmp=0;VirtualProtect((LPVOID)(ULONG_PTR)g_target,1,old,&tmp);return true;}
static bool installBreak(){if(g_breakInstalled)return true;u8 b=0;if(!rd(g_target,&b,1))return false;if(b==0xCC)return false;g_originalByte=b;if(g_originalByte!=g_expected[0])return false;if(!patchByte(0xCC))return false;g_breakInstalled=true;return true;}
static void removeBreak(){if(!g_breakInstalled)return;patchByte(g_originalByte);g_breakInstalled=false;}

static bool boolIni(const WCHAR*key,bool def){WCHAR path[520],buf[32];modulePath(path,L"ds2_no_magellan_evaluation.ini");if(!path[0])return def;buf[0]=0;GetPrivateProfileStringW(L"NoMagellanEvaluation",key,def?L"1":L"0",buf,32,path);return !(buf[0]==L'0'||buf[0]==L'n'||buf[0]==L'N'||buf[0]==L'f'||buf[0]==L'F');}
static WCHAR lowerAscii(WCHAR c){return (c>=L'A'&&c<=L'Z')?(WCHAR)(c+(L'a'-L'A')):c;}
static bool isDs2Process(){WCHAR path[520];DWORD n=GetModuleFileNameW(g_mod,path,520);if(!n||n>=520)return false;DWORD start=0;for(DWORD i=0;i<n;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1;const WCHAR want[]=L"ds2.exe";DWORD j=0;while(want[j]){if(start+j>=n||lowerAscii(path[start+j])!=want[j])return false;j++;}return start+j==n;}
static void openDebugLog(){if(!g_debugLog)return;WCHAR path[520];modulePath(path,L"ds2_no_magellan_evaluation.log");if(!path[0])return;g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(g_log!=INVALID_HANDLE_VALUE){const char h[]="No Magellan Evaluation v" MOD_VERSION "\r\nValidated target: Steam PC 1.10.89.0\r\nRule: selector=2, flags.bit0=1, ECX=0 => lookup selector 2\r\n";logRaw(h,sizeof(h)-1);}}
static void closeDebugLog(){if(g_log!=INVALID_HANDLE_VALUE){FlushFileBuffers(g_log);CloseHandle(g_log);g_log=INVALID_HANDLE_VALUE;}}

static LONG WINAPI veh(EXCEPTION_POINTERS*e){if(!e||!e->ExceptionRecord||!e->ContextRecord)return EXCEPTION_CONTINUE_SEARCH;CONTEXT*c=e->ContextRecord;DWORD tid=GetCurrentThreadId();DWORD code=e->ExceptionRecord->ExceptionCode;u64 rip=cg(c,RIPOFF);if(!g_active)return EXCEPTION_CONTINUE_SEARCH;
    if(code==EXCEPTION_BREAKPOINT){u64 ea=(u64)(ULONG_PTR)e->ExceptionRecord->ExceptionAddress;if(ea!=g_target&&rip!=g_target+1ULL)return EXCEPTION_CONTINUE_SEARCH;removeBreak();u64 rdx=cg(c,RDXOFF);u64 rcx=cg(c,RCXOFF);u8 selector=0xff,flags=0xff;bool ok=rd(rdx+SELECTOR_OFFSET,&selector,1)&&rd(rdx+FLAGS_OFFSET,&flags,1);bool isMagellanPenalty=ok&&selector==NORMAL_SELECTOR&&((flags&1U)!=0U)&&((u32)rcx==0U);if(isMagellanPenalty){cs(c,RCXOFF,NORMAL_SELECTOR);logEvent(true,selector,flags);}else if(ok){logEvent(false,selector,flags);}cs(c,RIPOFF,g_target);g_stepTid=(LONG)tid;cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)|0x100ULL);return EXCEPTION_CONTINUE_EXECUTION;}
    if(code==EXCEPTION_SINGLE_STEP&&g_stepTid==(LONG)tid){cs(c,EFLAGSOFF,cg(c,EFLAGSOFF)&~0x100ULL);g_stepTid=0;if(!installBreak()){g_active=0;const char m[]="ERROR: could not re-arm hook; mod disabled for this session.\r\n";logRaw(m,sizeof(m)-1);}return EXCEPTION_CONTINUE_EXECUTION;}return EXCEPTION_CONTINUE_SEARCH;}

static DWORD WINAPI worker(LPVOID){while(!g_shutdown&&!(g_mod=GetModuleHandleW(0)))Sleep(100);if(g_shutdown||!g_mod)return 0;if(!isDs2Process())return 0;if(!boolIni(L"Enabled",true))return 0;g_debugLog=boolIni(L"DebugLog",false);openDebugLog();g_moduleBase=(u64)(ULONG_PTR)g_mod;g_target=g_moduleBase+SELECTOR_LOOKUP_CALL_RVA;if(!getBuild(&g_actualTimestamp,&g_actualImage)){const char m[]="ERROR: could not read DS2.exe build metadata; no hook installed.\r\n";logRaw(m,sizeof(m)-1);closeDebugLog();return 0;}if(g_actualTimestamp!=EXPECTED_TIMESTAMP||g_actualImage!=EXPECTED_IMAGE){const char m[]="ERROR: unsupported DS2.exe build metadata; no hook installed.\r\n";logRaw(m,sizeof(m)-1);closeDebugLog();return 0;}if(!signatureOk()){const char m[]="ERROR: instruction signature mismatch at DS2.exe+0x1430FF1; no hook installed.\r\n";logRaw(m,sizeof(m)-1);closeDebugLog();return 0;}g_veh=AddVectoredExceptionHandler(1,veh);if(!g_veh){const char m[]="ERROR: AddVectoredExceptionHandler failed; no hook installed.\r\n";logRaw(m,sizeof(m)-1);closeDebugLog();return 0;}if(!installBreak()){const char m[]="ERROR: could not arm hook; remove conflicting ASIs and restart DS2.\r\n";logRaw(m,sizeof(m)-1);RemoveVectoredExceptionHandler(g_veh);g_veh=0;closeDebugLog();return 0;}g_active=1;const char a[]="ACTIVE: hook armed.\r\n";logRaw(a,sizeof(a)-1);while(!g_shutdown&&g_active)Sleep(100);g_active=0;removeBreak();LPVOID v=g_veh;g_veh=0;if(v)RemoveVectoredExceptionHandler(v);closeDebugLog();return 0;}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){if(reason==1){g_self=module;DisableThreadLibraryCalls(module);HANDLE t=CreateThread(0,0,worker,0,0,0);if(t)CloseHandle(t);}else if(reason==0){g_shutdown=1;g_active=0;removeBreak();}return TRUE;}
