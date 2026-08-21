#include "windows.h"

extern "C" void* memcpy(void* d,const void* s,SIZE_T n){u8* a=(u8*)d;const u8* b=(const u8*)s;for(SIZE_T i=0;i<n;i++)a[i]=b[i];return d;}
extern "C" void* memset(void* d,int v,SIZE_T n){u8* a=(u8*)d;for(SIZE_T i=0;i<n;i++)a[i]=(u8)v;return d;}

#define MOD_VERSION "1.0.0"
#define INI_NAME L"ds2_infrastructure_one_unit.ini"
#define LOG_NAME L"ds2_infrastructure_one_unit.log"
#define INI_SECTION L"InfrastructureOneUnit"

#define EXPECTED_TIMESTAMP 0x6A3DAE46UL
#define EXPECTED_IMAGE 0x0B292000UL

#define NEED_RESULT_RVA 0x012922F1ULL
#define CURRENT_RESULT_RVA 0x012923FBULL

#define ROAD_VTABLE_RVA 0x03281058ULL
#define MONORAIL_VTABLE_RVA 0x03280258ULL
#define MINE_VTABLE_RVA 0x03287438ULL

#define OBJECT_PROGRESS_OFFSET 0x280ULL
#define OBJECT_MODE_OFFSET 0x3F5ULL
#define OBJECT_FROM_RBX_DELTA 0x388ULL

#define RCXOFF 0x80
#define RBXOFF 0x90
#define RSPOFF 0x98
#define RDIOFF 0xB0
#define RIPOFF 0xF8

#define MAX_VALUE 2000000
#define CACHE_SLOTS 32
#define CACHE_MAX_AGE_SECONDS 2
#define CALLER_PAIR_COUNT 16

static HMODULE g_self=0,g_mod=0;
static LPVOID g_veh=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static volatile LONG g_shutdown=0,g_active=0,g_cacheLock=0;
static u64 g_moduleBase=0;
static u32 g_actualTimestamp=0,g_actualImage=0;
static i64 g_qpcFrequency=0;

static bool g_debugLog=false;
static u32 g_remainingUnits=1;
static bool g_roadConstruction=true;
static bool g_roadUpgrades=true;
static bool g_monorailConstruction=true;
static bool g_mineConstruction=true;
static bool g_mineUpgrades=true;

struct HookPoint { u64 address; u8 original; bool installed; };
static HookPoint g_needHook={0,0,false};
static HookPoint g_currentHook={0,0,false};

struct CallerPair { u64 needReturnRva; u64 currentReturnRva; };
static const CallerPair g_callerPairs[CALLER_PAIR_COUNT]={
  {0x0B8825BULL,0x0B88267ULL},
  {0x12B135EULL,0x12B137AULL},
  {0x13553AFULL,0x13553DBULL},
  {0x1533DC2ULL,0x1533DE1ULL},
  {0x15352D6ULL,0x15352E3ULL},
  {0x1535CD5ULL,0x1535D11ULL},
  {0x1536996ULL,0x15369C9ULL},
  {0x153D4AEULL,0x153D4F0ULL},
  {0x153E263ULL,0x153E297ULL},
  {0x15461DCULL,0x1546200ULL},
  {0x1560D20ULL,0x1560D55ULL},
  {0x1561E5FULL,0x1561E8DULL},
  {0x165B73DULL,0x165B74AULL},
  {0x16C3E01ULL,0x16C3E0EULL},
  {0x175B7F1ULL,0x175B806ULL},
  {0x17EBEB3ULL,0x17EBEBFULL}
};

struct NeedCache {
  DWORD tid;
  u32 pairId;
  i32 need[6];
  i64 qpc;
  bool ready;
};
static NeedCache g_cache[CACHE_SLOTS];

static const u8 g_needExpected[5]={0x48,0x8B,0x4C,0x24,0x48};
static const u8 g_currentExpected[5]={0x48,0x8B,0x4C,0x24,0x50};
static const u8 g_objectAnchor[13]={0xC5,0xF9,0xEF,0xC0,0x33,0xC0,0x48,0x8D,0x9E,0x88,0x03,0x00,0x00};
static const u8 g_modeLoad[7]={0x0F,0xB6,0xB6,0xF5,0x03,0x00,0x00};
static const u8 g_cumulativeWrite0[8]={0x8B,0x06,0x01,0x87,0x80,0x02,0x00,0x00};
static const u8 g_cumulativeWrite3[8]={0x8B,0x46,0x0C,0x01,0x87,0x8C,0x02,0x00};
static const u8 g_cumulativeRead0[8]={0x8B,0x87,0x80,0x02,0x00,0x00,0x01,0x03};
static const u8 g_cumulativeRead3[9]={0x8B,0x87,0x8C,0x02,0x00,0x00,0x01,0x43,0x0C};
static const u8 g_stageSubtract3[7]={0x8B,0x44,0x24,0x44,0x29,0x47,0x0C};

static bool canon(u64 p){return p>=0x10000ULL&&p<0x800000000000ULL;}
static bool readableProtect(DWORD q){q&=255U;return q==PAGE_READONLY||q==PAGE_READWRITE||q==PAGE_WRITECOPY||q==PAGE_EXECUTE||q==PAGE_EXECUTE_READ||q==PAGE_EXECUTE_READWRITE||q==PAGE_EXECUTE_WRITECOPY;}
static bool writableProtect(DWORD q){q&=255U;return q==PAGE_READWRITE||q==PAGE_WRITECOPY||q==PAGE_EXECUTE_READWRITE||q==PAGE_EXECUTE_WRITECOPY;}
static bool regionOk(u64 p,u64 n,bool write){if(!canon(p)||!n)return false;MEMORY_BASIC_INFORMATION m;memset(&m,0,sizeof(m));if(VirtualQuery((LPCVOID)(ULONG_PTR)p,&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||(m.Protect&PAGE_GUARD))return false;if(write?!writableProtect(m.Protect):!readableProtect(m.Protect))return false;u64 e=(u64)(ULONG_PTR)m.BaseAddress+m.RegionSize;return e>=p&&e-p>=n;}
static bool rd(u64 p,void*o,u64 n){if(!o||!regionOk(p,n,false))return false;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)p,o,(SIZE_T)n,&got)&&got==n;}
static bool wr32(u64 p,i32 v){if(!regionOk(p,4,true))return false;*(volatile i32*)(ULONG_PTR)p=v;return true;}
static u64 cg(CONTEXT*c,u32 o){u64 v=0;memcpy(&v,c->raw+o,8);return v;}
static void cs(CONTEXT*c,u32 o,u64 v){memcpy(c->raw+o,&v,8);}

static void modulePath(WCHAR*out,const WCHAR*name){if(!out)return;out[0]=0;DWORD n=GetModuleFileNameW(g_self,out,520);if(!n||n>=520){out[0]=0;return;}DWORD slash=0;for(DWORD i=0;i<n;i++)if(out[i]=='\\'||out[i]=='/')slash=i+1;DWORD j=0;while(name[j]&&slash+j+1<520){out[slash+j]=name[j];j++;}if(name[j]){out[0]=0;return;}out[slash+j]=0;}
static void logRaw(const char*text,DWORD len){if(!g_debugLog||g_log==INVALID_HANDLE_VALUE||!text||!len)return;DWORD w=0;WriteFile(g_log,text,len,&w,0);FlushFileBuffers(g_log);}
static DWORD appendText(char*b,DWORD n,DWORD cap,const char*s){if(!b||!s)return n;for(DWORD i=0;s[i]&&n+1<cap;i++)b[n++]=s[i];return n;}
static DWORD appendU32(char*b,DWORD n,DWORD cap,u32 v){char t[16];DWORD k=0;if(v==0)t[k++]='0';else{while(v&&k<15){t[k++]=(char)('0'+(v%10));v/=10;}for(DWORD i=0;i<k/2;i++){char c=t[i];t[i]=t[k-1-i];t[k-1-i]=c;}}for(DWORD i=0;i<k&&n+1<cap;i++)b[n++]=t[i];return n;}
static DWORD appendHex64(char*b,DWORD n,DWORD cap,u64 v){const char h[]="0123456789ABCDEF";n=appendText(b,n,cap,"0x");bool started=false;for(int s=60;s>=0;s-=4){u8 d=(u8)((v>>s)&15ULL);if(d||started||s==0){started=true;if(n+1<cap)b[n++]=h[d];}}return n;}
static void logSimple(const char*text){if(text)logRaw(text,(DWORD)([] (const char*s){DWORD n=0;while(s[n])n++;return n;})(text));}
static void logApply(const char*kind,u64 obj,u32 mode,u32 mask,u32 changed){if(!g_debugLog)return;char b[256];DWORD n=0;n=appendText(b,n,sizeof(b),"APPLY type=");n=appendText(b,n,sizeof(b),kind);n=appendText(b,n,sizeof(b)," obj=");n=appendHex64(b,n,sizeof(b),obj);n=appendText(b,n,sizeof(b)," mode=");n=appendU32(b,n,sizeof(b),mode);n=appendText(b,n,sizeof(b)," mask=");n=appendHex64(b,n,sizeof(b),mask);n=appendText(b,n,sizeof(b)," changed=");n=appendU32(b,n,sizeof(b),changed);n=appendText(b,n,sizeof(b)," remaining=");n=appendU32(b,n,sizeof(b),g_remainingUnits);if(n+2<sizeof(b)){b[n++]='\r';b[n++]='\n';}logRaw(b,n);}

static bool boolIni(const WCHAR*key,bool def){WCHAR path[520],buf[32];modulePath(path,INI_NAME);if(!path[0])return def;buf[0]=0;GetPrivateProfileStringW(INI_SECTION,key,def?L"1":L"0",buf,32,path);return !(buf[0]==L'0'||buf[0]==L'n'||buf[0]==L'N'||buf[0]==L'f'||buf[0]==L'F');}
static u32 uintIni(const WCHAR*key,u32 def,u32 minv,u32 maxv){WCHAR path[520],buf[32];modulePath(path,INI_NAME);if(!path[0])return def;buf[0]=0;GetPrivateProfileStringW(INI_SECTION,key,L"",buf,32,path);if(!buf[0])return def;u64 v=0;DWORD i=0;while(buf[i]>=L'0'&&buf[i]<=L'9'){v=v*10ULL+(u64)(buf[i]-L'0');if(v>maxv)return maxv;i++;}if(i==0)return def;if(v<minv)return minv;if(v>maxv)return maxv;return (u32)v;}
static WCHAR lowerAscii(WCHAR c){return (c>=L'A'&&c<=L'Z')?(WCHAR)(c+(L'a'-L'A')):c;}
static bool isDs2Process(){WCHAR path[520];DWORD n=GetModuleFileNameW(g_mod,path,520);if(!n||n>=520)return false;DWORD start=0;for(DWORD i=0;i<n;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1;const WCHAR want[]=L"ds2.exe";DWORD j=0;while(want[j]){if(start+j>=n||lowerAscii(path[start+j])!=want[j])return false;j++;}return start+j==n;}
static void openDebugLog(){if(!g_debugLog)return;WCHAR path[520];modulePath(path,LOG_NAME);if(!path[0])return;g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(g_log!=INVALID_HANDLE_VALUE){const char h[]="DS2 Infrastructure One Unit v" MOD_VERSION "\r\nValidated target: Steam PC 1.10.89.0\r\nPolicy: supported infrastructure remaining material units are reduced automatically.\r\n";logRaw(h,sizeof(h)-1);}}
static void closeDebugLog(){if(g_log!=INVALID_HANDLE_VALUE){FlushFileBuffers(g_log);CloseHandle(g_log);g_log=INVALID_HANDLE_VALUE;}}

static bool getBuild(u32*ts,u32*im){u64 b=(u64)(ULONG_PTR)g_mod;u32 pe=0,sig=0,t=0,i=0;if(!rd(b+0x3c,&pe,4)||pe>=0x2000)return false;if(!rd(b+pe,&sig,4)||sig!=0x4550)return false;if(!rd(b+pe+8,&t,4)||!rd(b+pe+0x50,&i,4))return false;*ts=t;*im=i;return true;}
static bool bytesEqual(u64 p,const u8*expected,u32 n){u8 b[32];if(!expected||n>sizeof(b)||!rd(p,b,n))return false;for(u32 i=0;i<n;i++)if(b[i]!=expected[i])return false;return true;}
static bool signaturesOk(){u64 b=g_moduleBase;return bytesEqual(b+NEED_RESULT_RVA,g_needExpected,5)&&bytesEqual(b+CURRENT_RESULT_RVA,g_currentExpected,5)&&bytesEqual(b+0x129235EULL,g_objectAnchor,13)&&bytesEqual(b+0x129238CULL,g_modeLoad,7)&&bytesEqual(b+0x128D04FULL,g_cumulativeWrite0,8)&&bytesEqual(b+0x128D074ULL,g_cumulativeWrite3,8)&&bytesEqual(b+0x128DAB2ULL,g_cumulativeRead0,8)&&bytesEqual(b+0x128DACCULL,g_cumulativeRead3,9)&&bytesEqual(b+0x12923E6ULL,g_stageSubtract3,7);}
static bool patchByte(u64 address,u8 value){DWORD old=0;if(!VirtualProtect((LPVOID)(ULONG_PTR)address,1,PAGE_EXECUTE_READWRITE,&old))return false;*(volatile u8*)(ULONG_PTR)address=value;FlushInstructionCache(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)address,1);DWORD tmp=0;VirtualProtect((LPVOID)(ULONG_PTR)address,1,old,&tmp);return true;}
static bool installHook(HookPoint*h,u64 address,u8 expected){if(!h||h->installed)return h&&h->installed;u8 b=0;if(!rd(address,&b,1)||b!=expected||b==0xCC)return false;h->address=address;h->original=b;if(!patchByte(address,0xCC))return false;h->installed=true;return true;}
static void removeHook(HookPoint*h){if(!h||!h->installed)return;patchByte(h->address,h->original);h->installed=false;}
static void removeHooks(){removeHook(&g_needHook);removeHook(&g_currentHook);}

static bool cacheAcquire(){for(u32 i=0;i<10000;i++)if(InterlockedCompareExchange(&g_cacheLock,1,0)==0)return true;return false;}
static void cacheRelease(){InterlockedExchange(&g_cacheLock,0);}
static i64 qpcNow(){LARGE_INTEGER q;q.QuadPart=0;if(QueryPerformanceCounter(&q))return q.QuadPart;return 0;}
static bool vectorPlausible(const i32*v){if(!v)return false;bool positive=false;for(u32 i=0;i<6;i++){if(v[i]<0||v[i]>MAX_VALUE)return false;if(v[i]>0)positive=true;}return positive;}
static void cacheStore(DWORD tid,u32 pairId,const i32*need){if(!need||!pairId||!cacheAcquire())return;int slot=-1;for(int i=0;i<CACHE_SLOTS;i++)if(g_cache[i].ready&&g_cache[i].tid==tid&&g_cache[i].pairId==pairId){slot=i;break;}if(slot<0)for(int i=0;i<CACHE_SLOTS;i++)if(!g_cache[i].ready){slot=i;break;}if(slot<0)slot=(int)(tid%CACHE_SLOTS);g_cache[slot].tid=tid;g_cache[slot].pairId=pairId;for(u32 i=0;i<6;i++)g_cache[slot].need[i]=need[i];g_cache[slot].qpc=qpcNow();g_cache[slot].ready=true;cacheRelease();}
static bool cacheTake(DWORD tid,u32 pairId,i32*out){if(!out||!pairId||!cacheAcquire())return false;bool ok=false;i64 now=qpcNow();for(int i=0;i<CACHE_SLOTS;i++){NeedCache&c=g_cache[i];if(!c.ready||c.tid!=tid||c.pairId!=pairId)continue;bool fresh=true;if(g_qpcFrequency>0&&c.qpc>0&&now>=c.qpc){i64 maxAge=g_qpcFrequency*(i64)CACHE_MAX_AGE_SECONDS;if(now-c.qpc>maxAge)fresh=false;}if(fresh){for(u32 k=0;k<6;k++)out[k]=c.need[k];ok=true;}c.ready=false;break;}cacheRelease();return ok;}

static u32 needPairFromReturn(u64 ret){for(u32 i=0;i<CALLER_PAIR_COUNT;i++)if(ret==g_moduleBase+g_callerPairs[i].needReturnRva)return i+1;return 0;}
static u32 currentPairFromReturn(u64 ret){for(u32 i=0;i<CALLER_PAIR_COUNT;i++)if(ret==g_moduleBase+g_callerPairs[i].currentReturnRva)return i+1;return 0;}

static bool configAllows(u64 vtable,u32 mode,const char**kind){if(kind)*kind="Unknown";if(vtable==g_moduleBase+ROAD_VTABLE_RVA){if(mode==1&&g_roadConstruction){if(kind)*kind="RoadBuild";return true;}if(mode==2&&g_roadUpgrades){if(kind)*kind="RoadUpgrade";return true;}return false;}if(vtable==g_moduleBase+MONORAIL_VTABLE_RVA){if(mode==1&&g_monorailConstruction){if(kind)*kind="MonorailBuild";return true;}return false;}if(vtable==g_moduleBase+MINE_VTABLE_RVA){if(mode==1&&g_mineConstruction){if(kind)*kind="MineBuild";return true;}if(mode==2&&g_mineUpgrades){if(kind)*kind="MineUpgrade2";return true;}if(mode==3&&g_mineUpgrades){if(kind)*kind="MineUpgrade3";return true;}return false;}return false;}

static bool applyPolicy(u64 object,u64 vectorPtr,const i32*need){if(!canon(object)||!canon(vectorPtr)||!need)return false;u64 vtable=0;u8 mode=0;i32 current[6],live[6];if(!rd(object,&vtable,8)||!rd(object+OBJECT_MODE_OFFSET,&mode,1)||!rd(vectorPtr,current,sizeof(current))||!rd(object+OBJECT_PROGRESS_OFFSET,live,sizeof(live)))return false;if(!vectorPlausible(need))return false;const char*kind=0;if(!configAllows(vtable,(u32)mode,&kind))return false;i32 newCurrent[6],newLive[6];u32 mask=0,changed=0;for(u32 i=0;i<6;i++){newCurrent[i]=current[i];newLive[i]=live[i];if(need[i]<0||need[i]>MAX_VALUE||current[i]<0||current[i]>need[i]||live[i]<0||live[i]>MAX_VALUE)return false;i32 remaining=need[i]-current[i];if(remaining>(i32)g_remainingUnits){i32 targetCurrent=need[i]-(i32)g_remainingUnits;i64 delta=(i64)targetCurrent-(i64)current[i];i64 targetLive=(i64)live[i]+delta;if(delta<=0||targetCurrent<0||targetCurrent>MAX_VALUE||targetLive<0||targetLive>MAX_VALUE)return false;newCurrent[i]=targetCurrent;newLive[i]=(i32)targetLive;mask|=(1U<<i);changed++;}}
  if(!changed)return false;if(!regionOk(object+OBJECT_PROGRESS_OFFSET,sizeof(live),true)||!regionOk(vectorPtr,sizeof(current),true))return false;u32 applied=0;for(u32 i=0;i<6;i++){if(!(mask&(1U<<i)))continue;if(!wr32(object+OBJECT_PROGRESS_OFFSET+i*4ULL,newLive[i]))goto rollback;applied|=(1U<<i);}for(u32 i=0;i<6;i++){if(!(mask&(1U<<i)))continue;if(!wr32(vectorPtr+i*4ULL,newCurrent[i]))goto rollback;}i32 verifyLive[6],verifyCurrent[6];if(!rd(object+OBJECT_PROGRESS_OFFSET,verifyLive,sizeof(verifyLive))||!rd(vectorPtr,verifyCurrent,sizeof(verifyCurrent)))goto rollback;for(u32 i=0;i<6;i++)if((mask&(1U<<i))&&(verifyLive[i]!=newLive[i]||verifyCurrent[i]!=newCurrent[i]))goto rollback;logApply(kind,object,(u32)mode,mask,changed);return true;
rollback:
  for(u32 i=0;i<6;i++)if(applied&(1U<<i))wr32(object+OBJECT_PROGRESS_OFFSET+i*4ULL,live[i]);for(u32 i=0;i<6;i++)if(mask&(1U<<i))wr32(vectorPtr+i*4ULL,current[i]);g_active=0;logSimple("ERROR: transactional progress update failed; mod disabled for this session.\r\n");return false;}

static void handleNeed(CONTEXT*c){u64 rsp=cg(c,RSPOFF),rdi=cg(c,RDIOFF),ret=0;i32 need[6];if(!rd(rsp+0x58,&ret,8))return;u32 pairId=needPairFromReturn(ret);if(!pairId||!rd(rdi,need,sizeof(need))||!vectorPlausible(need))return;cacheStore(GetCurrentThreadId(),pairId,need);}
static void handleCurrent(CONTEXT*c){u64 rsp=cg(c,RSPOFF),rdi=cg(c,RDIOFF),rbx=cg(c,RBXOFF),ret=0;if(!rd(rsp+0x78,&ret,8))return;u32 pairId=currentPairFromReturn(ret);if(!pairId||rbx<OBJECT_FROM_RBX_DELTA)return;i32 need[6];if(!cacheTake(GetCurrentThreadId(),pairId,need))return;u64 object=rbx-OBJECT_FROM_RBX_DELTA;applyPolicy(object,rdi,need);}

static LONG WINAPI veh(EXCEPTION_POINTERS*e){if(!e||!e->ExceptionRecord||!e->ContextRecord||e->ExceptionRecord->ExceptionCode!=EXCEPTION_BREAKPOINT)return EXCEPTION_CONTINUE_SEARCH;CONTEXT*c=e->ContextRecord;u64 rip=cg(c,RIPOFF);u64 ea=(u64)(ULONG_PTR)e->ExceptionRecord->ExceptionAddress;HookPoint*h=0;bool need=false;if((ea==g_needHook.address||rip==g_needHook.address+1ULL)&&g_needHook.installed){h=&g_needHook;need=true;}else if((ea==g_currentHook.address||rip==g_currentHook.address+1ULL)&&g_currentHook.installed){h=&g_currentHook;need=false;}else return EXCEPTION_CONTINUE_SEARCH;if(g_active){if(need)handleNeed(c);else handleCurrent(c);}u64 rsp=cg(c,RSPOFF),newRcx=0;u64 stackOffset=need?0x48ULL:0x50ULL;if(!rd(rsp+stackOffset,&newRcx,8)){g_active=0;logSimple("ERROR: could not emulate hooked MOV; mod disabled for this session.\r\n");cs(c,RIPOFF,h->address+5ULL);return EXCEPTION_CONTINUE_EXECUTION;}cs(c,RCXOFF,newRcx);cs(c,RIPOFF,h->address+5ULL);return EXCEPTION_CONTINUE_EXECUTION;}

static DWORD WINAPI worker(LPVOID){while(!g_shutdown&&!(g_mod=GetModuleHandleW(0)))Sleep(100);if(g_shutdown||!g_mod)return 0;if(!isDs2Process())return 0;if(!boolIni(L"Enabled",true))return 0;g_debugLog=boolIni(L"DebugLog",false);g_remainingUnits=uintIni(L"RemainingUnits",1,1,1000000);g_roadConstruction=boolIni(L"RoadConstruction",true);g_roadUpgrades=boolIni(L"RoadUpgrades",true);g_monorailConstruction=boolIni(L"MonorailConstruction",true);g_mineConstruction=boolIni(L"MineConstruction",true);g_mineUpgrades=boolIni(L"MineUpgrades",true);openDebugLog();g_moduleBase=(u64)(ULONG_PTR)g_mod;if(!getBuild(&g_actualTimestamp,&g_actualImage)){logSimple("ERROR: could not read DS2.exe build metadata; no hooks installed.\r\n");closeDebugLog();return 0;}if(g_actualTimestamp!=EXPECTED_TIMESTAMP||g_actualImage!=EXPECTED_IMAGE){logSimple("ERROR: unsupported DS2.exe build metadata; no hooks installed.\r\n");closeDebugLog();return 0;}if(!signaturesOk()){logSimple("ERROR: instruction signature mismatch; no hooks installed.\r\n");closeDebugLog();return 0;}LARGE_INTEGER f;f.QuadPart=0;if(QueryPerformanceFrequency(&f))g_qpcFrequency=f.QuadPart;g_veh=AddVectoredExceptionHandler(1,veh);if(!g_veh){logSimple("ERROR: AddVectoredExceptionHandler failed; no hooks installed.\r\n");closeDebugLog();return 0;}if(!installHook(&g_needHook,g_moduleBase+NEED_RESULT_RVA,g_needExpected[0])||!installHook(&g_currentHook,g_moduleBase+CURRENT_RESULT_RVA,g_currentExpected[0])){removeHooks();RemoveVectoredExceptionHandler(g_veh);g_veh=0;logSimple("ERROR: could not arm hooks; remove conflicting ASIs and restart DS2.\r\n");closeDebugLog();return 0;}g_active=1;logSimple("ACTIVE: exact Need/Current caller-pair hooks armed. Known caller pairs: 16.\r\n");while(!g_shutdown&&g_active)Sleep(100);g_active=0;removeHooks();LPVOID v=g_veh;g_veh=0;if(v)RemoveVectoredExceptionHandler(v);closeDebugLog();return 0;}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){if(reason==1){g_self=module;DisableThreadLibraryCalls(module);HANDLE t=CreateThread(0,0,worker,0,0,0);if(t)CloseHandle(t);}else if(reason==0){g_shutdown=1;g_active=0;removeHooks();}return TRUE;}
