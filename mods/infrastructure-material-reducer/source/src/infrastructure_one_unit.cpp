#include "windows.h"

extern "C" void* memcpy(void* d,const void* s,SIZE_T n){u8* a=(u8*)d;const u8* b=(const u8*)s;for(SIZE_T i=0;i<n;i++)a[i]=b[i];return d;}
extern "C" void* memset(void* d,int v,SIZE_T n){u8* a=(u8*)d;for(SIZE_T i=0;i<n;i++)a[i]=(u8)v;return d;}

#define MOD_VERSION "1.1.0"
#define INI_NAME L"ds2_infrastructure_one_unit.ini"
#define LOG_NAME L"ds2_infrastructure_one_unit.log"
#define INI_SECTION L"InfrastructureOneUnit"

#define EXPECTED_TIMESTAMP 0x6A3DAE46UL
#define EXPECTED_IMAGE 0x0B292000UL

#define NEED_RESULT_RVA 0x012922F1ULL
#define CURRENT_RESULT_RVA 0x012923FBULL

// Native game build v1.10.89.0: positive mine extraction step, menu, and output.
#define MINE_COST_GETTER_RVA 0x1292597ULL
#define MINE_COST_PROCESS_RVA 0x134D437ULL
#define MINE_COST_LOOP_RVA 0x134D654ULL
#define MINE_COST_BUDGET_RVA 0x134D3D9ULL
#define MINE_COST_UI_RVA 0x156DAE7ULL
#define MINE_COST_CONFIRM_RVA 0x1536CE0ULL
#define MINE_MENU_DENOM_RVA 0x155FDA7ULL

#define MINE_VTABLE_2_RVA 0x3280240ULL
#define MINE_VTABLE_3_RVA 0x3281FF0ULL
#define MINE_VTABLE_4_RVA 0x3282028ULL

#define ROAD_VTABLE_RVA 0x03281058ULL
#define MONORAIL_VTABLE_RVA 0x03280258ULL
#define MINE_VTABLE_RVA 0x03287438ULL

#define OBJECT_PROGRESS_OFFSET 0x280ULL
#define OBJECT_MODE_OFFSET 0x3F5ULL
#define OBJECT_FROM_RBX_DELTA 0x388ULL

#define RCXOFF 0x80
#define RDXOFF 0x88
#define RBPOFF 0xA0
#define RBXOFF 0x90
#define RSPOFF 0x98
#define RDIOFF 0xB0
#define RIPOFF 0xF8
#define RAXOFF 0x78
#define R13OFF 0xE0
#define R15OFF 0xF0

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
static u32 g_mineCrystalCost=0; // 0 = vanilla, 1..1000000 = one batch costs N
static volatile LONG g_mineCostGetterHits=0,g_mineCostUIHits=0;
static volatile LONG g_mineCostConfirmHits=0,g_mineCostProductionHits=0;
static volatile LONG g_mineCostBudgetHits=0,g_mineCostLoopHits=0;
static volatile LONG g_mineMenuDenomHits=0;

struct HookPoint { u64 address; u8 original; bool installed; };
static HookPoint g_needHook={0,0,false};
static HookPoint g_currentHook={0,0,false};
static HookPoint g_mineGetterHook={0,0,false};
static HookPoint g_mineProcessHook={0,0,false};
static HookPoint g_mineLoopHook={0,0,false};
static HookPoint g_mineBudgetHook={0,0,false};
static HookPoint g_mineUiHook={0,0,false};
static HookPoint g_mineConfirmHook={0,0,false};
static HookPoint g_mineMenuDenomHook={0,0,false};

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
// Exact original instructions at the six mine-specific hook positions.
static const u8 g_getterExpected[3]={0x8B,0x40,0x20};
static const u8 g_processExpected[3]={0x8B,0x49,0x20};
static const u8 g_loopExpected[3]={0x03,0x79,0x20};
static const u8 g_budgetExpected[4]={0x89,0x44,0x24,0x38};
static const u8 g_uiExpected[6]={0x89,0x83,0x8C,0x00,0x00,0x00};
static const u8 g_confirmExpected[6]={0x8B,0x81,0x88,0x00,0x00,0x00};
static const u8 g_mineMenuDenomExpected[3]={0x8B,0x59,0x20};
static const u8 g_nativeDebitExpected[4]={0x41,0x89,0x7F,0x28};
static const u8 g_mineConfirmVtableExpected[3]={0x48,0x8D,0x05};

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
static bool mineCostSignaturesOk(){
  u64 b=g_moduleBase;
  return bytesEqual(b+MINE_COST_GETTER_RVA,g_getterExpected,3) &&
    bytesEqual(b+MINE_COST_PROCESS_RVA,g_processExpected,3) &&
    bytesEqual(b+MINE_COST_LOOP_RVA,g_loopExpected,3) &&
    bytesEqual(b+MINE_COST_BUDGET_RVA,g_budgetExpected,4) &&
    bytesEqual(b+MINE_COST_UI_RVA,g_uiExpected,6) &&
    bytesEqual(b+MINE_COST_CONFIRM_RVA,g_confirmExpected,6) &&
    bytesEqual(b+MINE_MENU_DENOM_RVA,g_mineMenuDenomExpected,3) &&
    bytesEqual(b+0x134D650ULL,g_nativeDebitExpected,4) &&
    bytesEqual(b+0x1536D79ULL,g_mineConfirmVtableExpected,3);
}
static bool patchByte(u64 address,u8 value){DWORD old=0;if(!VirtualProtect((LPVOID)(ULONG_PTR)address,1,PAGE_EXECUTE_READWRITE,&old))return false;*(volatile u8*)(ULONG_PTR)address=value;FlushInstructionCache(GetCurrentProcess(),(LPCVOID)(ULONG_PTR)address,1);DWORD tmp=0;VirtualProtect((LPVOID)(ULONG_PTR)address,1,old,&tmp);return true;}
static bool installHook(HookPoint*h,u64 address,u8 expected){if(!h||h->installed)return h&&h->installed;u8 b=0;if(!rd(address,&b,1)||b!=expected||b==0xCC)return false;h->address=address;h->original=b;if(!patchByte(address,0xCC))return false;h->installed=true;return true;}
static void removeHook(HookPoint*h){if(!h||!h->installed)return;patchByte(h->address,h->original);h->installed=false;}
static void removeMineCostHooks(){
  removeHook(&g_mineMenuDenomHook);
  removeHook(&g_mineConfirmHook);
  removeHook(&g_mineUiHook);
  removeHook(&g_mineBudgetHook);
  removeHook(&g_mineLoopHook);
  removeHook(&g_mineProcessHook);
  removeHook(&g_mineGetterHook);
}
static void removeHooks(){
  removeMineCostHooks();
  removeHook(&g_currentHook);
  removeHook(&g_needHook);
}

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


// 0 = disabled, positive values set the step per extraction, never zero.
static i32 reducedMineCost(i32 native){
  if(native<=0||native>10000000||g_mineCrystalCost==0)return native;
  i32 configured=(i32)g_mineCrystalCost;
  return configured<native?configured:native;
}
// The getter's RBX is a resolved construction-resource object, NOT a
// DSMiningMachine instance. The old DSMiningMachine-vtable check never matched.
// Scope the interception to Ghidra-verified mining callsites instead.
// The construction-details and six-slot UI use the stage's type 0x11/0x12,
// while the shared affordability path still requires the same resource identity.
static u64 g_mineStageResourceObject=0;
static volatile LONG g_mineGetterDebugCount=0,g_mineTableCostHits=0,g_mineDetailsCostHits=0;
static bool mineGetterForResource(CONTEXT*c,u64 resource){
  u64 ret=0;
  if(!canon(resource)||!rd(cg(c,RSPOFF)+0x28ULL,&ret,8)||
     ret<g_moduleBase||ret>=g_moduleBase+(u64)EXPECTED_IMAGE)return false;
  u64 site=ret-g_moduleBase;
  switch(site){
    case 0x12B2056ULL: // ConstructionMiningCommand only
    case 0x134CD3CULL: // DSMiningMachine::mining event only
    case 0x1535378ULL: // mine contribution quantity in case 9
    case 0x153D923ULL: // mine material quantity validator
    case 0x153E4FAULL: // mine material selection validator (case 9)
    case 0x1546150ULL: // mine current-cost cursor logic
    case 0x15735F6ULL: // mine production UI event (case 0x2B)
      g_mineStageResourceObject=resource;
      return true;
    case 0x150D914ULL: // six-slot construction material row
    case 0x16BD8CAULL: { // construction details: displayed mine price
      // Ghidra: FUN_1416bcb70 only calls the latter getter in the
      // resource-type 0x11/0x12 branch. It runs before the menu's
      // mine-specific getter, so cached resource identity can be absent.
      // Read the stage resource type directly instead of using a cache.
      u8 resourceType=0;
      return rd(resource+0x164ULL,&resourceType,1) &&
             (resourceType==0x11||resourceType==0x12);
    }
    case 0x15033C9ULL: // shared inventory affordability check
      return resource==g_mineStageResourceObject&&g_mineStageResourceObject!=0;
    default:
      return false;
  }
}
static void mineLogHit(volatile LONG*counter,const char*tag){
  if(InterlockedIncrement(counter)==1)logSimple(tag);
}
// Getter 0x1292597: original MOV EAX,[RAX+0x20]. RBX resolves a
// construction-resource record, not a DSMiningMachine vtable.
// The exact callsite and resource identity distinguish mining from roads.
static void mineGetter(CONTEXT*c){
  u64 cfg=cg(c,RAXOFF),obj=cg(c,RBXOFF);
  i32 amount=0;
  if(!rd(cfg+0x20ULL,&amount,4)){
    g_active=0;
    logSimple("MINE_COST_ERROR: getter operand unreadable; hooks disabling.\r\n");
  }else{
    if(g_active&&g_mineCrystalCost>0&&mineGetterForResource(c,obj)){
      i32 newAmount=reducedMineCost(amount);
      if(newAmount!=amount){
        if(g_debugLog){
          u64 ret=0;
          if(rd(cg(c,RSPOFF)+0x28ULL,&ret,8)){
            if(ret==g_moduleBase+0x150D914ULL){
              mineLogHit(&g_mineTableCostHits,"MINE_COST: material row requirement display reduced.\r\n");
            }else if(ret==g_moduleBase+0x16BD8CAULL){
              mineLogHit(&g_mineDetailsCostHits,"MINE_COST: construction detail requirement display reduced.\r\n");
            }
          }
        }
        if(g_debugLog&&InterlockedIncrement(&g_mineGetterDebugCount)<=10){
          u64 ret=0;
          rd(cg(c,RSPOFF)+0x28ULL,&ret,8);
          char b[160];DWORD n=0;
          n=appendText(b,n,sizeof(b),"MINE_COST: getter caller=");
          n=appendHex64(b,n,sizeof(b),ret>=g_moduleBase?ret-g_moduleBase:ret);
          n=appendText(b,n,sizeof(b)," native=");
          n=appendU32(b,n,sizeof(b),(u32)amount);
          n=appendText(b,n,sizeof(b)," configured=");
          n=appendU32(b,n,sizeof(b),(u32)newAmount);
          if(n+2<sizeof(b)){b[n++]='\r';b[n++]='\n';}
          logRaw(b,n);
        }
        amount=newAmount;
        mineLogHit(&g_mineCostGetterHits,"MINE_COST: getter reduced.\r\n");
      }
    }
  }
  cs(c,RAXOFF,(u64)(u32)amount);
  cs(c,RIPOFF,g_mineGetterHook.address+3ULL);
}
// Native DSMiningMachine processing step, original MOV ECX,[RCX+0x20].
static void mineProcess(CONTEXT*c){
  u64 cfg=cg(c,RCXOFF);
  i32 native=0;
  if(!rd(cfg+0x20ULL,&native,4)){
    g_active=0;
    logSimple("MINE_COST_ERROR: processing-step operand unreadable.\r\n");
  }else{
    if(g_active&&g_mineCrystalCost>0){
      i32 cheaper=reducedMineCost(native);
      if(cheaper!=native){
        native=cheaper;
        mineLogHit(&g_mineCostProductionHits,"MINE_COST: production step reduced.\r\n");
      }
    }
  }
  cs(c,RCXOFF,(u64)(u32)native);
  cs(c,RIPOFF,g_mineProcessHook.address+3ULL);
}
// Native DSMiningMachine production loop: original ADD EDI,[RCX+0x20].
// The native compare overwrites arithmetic flags before any branch.
static void mineLoop(CONTEXT*c){
  i32 native=0;
  if(!rd(cg(c,RCXOFF)+0x20ULL,&native,4)){
    g_active=0;
    logSimple("MINE_COST_ERROR: production-loop step unreadable.\r\n");
    native=0;
  }
  if(g_active&&g_mineCrystalCost>0){
    i32 cheaper=reducedMineCost(native);
    if(cheaper!=native){
      native=cheaper;
      mineLogHit(&g_mineCostLoopHits,"MINE_COST: native loop step reduced.\r\n");
    }
  }
  u32 edi=(u32)cg(c,RDIOFF);
  cs(c,RDIOFF,(u64)(u32)(edi+(u32)native));
  cs(c,RIPOFF,g_mineLoopHook.address+3ULL);
}
// Native DSMiningMachine calculation: store budget to a stack local.
// Cap available output to a single positive batch even when a mine has older
// contributions from prior tests. Do NOT alter the deposited balance.
// Original MOV [RSP+0x38],EAX.
static void mineBudgetCap(CONTEXT*c){
  i32 budget=(i32)(u32)cg(c,RAXOFF);
  if(g_active&&g_mineCrystalCost>0&&budget>0){
    u64 controller=cg(c,R13OFF),bank=cg(c,R15OFF);
    u64 check=0;
    i32 consumed=0;
    if(rd(controller+0x58ULL,&check,8)&&check==bank&&
       rd(bank+0x28ULL,&consumed,4)&&consumed>=0){
      u64 allowed=(u64)(u32)consumed+(u64)g_mineCrystalCost;
      if(allowed<=0x7FFFFFFFULL&&budget>(i32)allowed){
        budget=(i32)allowed;
        mineLogHit(&g_mineCostBudgetHits,"MINE_COST: excess budget capped to one batch.\r\n");
      }
    }
  }
  if(!wr32(cg(c,RSPOFF)+0x38ULL,budget)){
    g_active=0;logSimple("MINE_COST_ERROR: cannot emulate budget store.\r\n");
  }
  cs(c,RIPOFF,g_mineBudgetHook.address+4ULL);
}
// Mining-selection UI: original MOV [RBX+0x8c],EAX.  RDI owns the
// quantity widget; RBX is its menu model. The original model mode must be 9.
// Keep both selected widget value and visible model aligned.
static void mineUiSelection(CONTEXT*c){
  u64 menu=cg(c,RBXOFF),widget=cg(c,RDIOFF),linked=0;
  u8 mode=0;
  i32 value=(i32)(u32)cg(c,RAXOFF);
  if(g_active&&g_mineCrystalCost>0&&value>0&&
     rd(widget+0x1b0ULL,&linked,8)&&linked==menu&&
     rd(menu+0xc30ULL,&mode,1)&&mode==9){
    if(value>(i32)g_mineCrystalCost &&
       regionOk(widget+0xd0ULL,4,true)){
      value=(i32)g_mineCrystalCost;
      if(wr32(widget+0xd0ULL,value)){
        mineLogHit(&g_mineCostUIHits,"MINE_COST: selected UI amount reduced.\r\n");
      }
    }
  }
  if(!wr32(menu+0x8cULL,value)){
    g_active=0;logSimple("MINE_COST_ERROR: cannot emulate quantity store.\r\n");
  }
  cs(c,RIPOFF,g_mineUiHook.address+6ULL);
}
// Mining-only confirmation (case 9) before native material handling.
// Original MOV EAX,[RCX+0x88]. Protect against an old UI selection of 1000
// producing 1000 batches at the new one-crystal step.
static void mineConfirm(CONTEXT*c){
  u64 menu=cg(c,RCXOFF);
  u8 mode=0;
  i32 selected=0;
  if(g_active&&g_mineCrystalCost>0&&
     rd(menu+0xc30ULL,&mode,1)&&mode==9&&
     rd(menu+0x8cULL,&selected,4)&&selected>(i32)g_mineCrystalCost){
    if(wr32(menu+0x8cULL,(i32)g_mineCrystalCost)){
      mineLogHit(&g_mineCostConfirmHits,"MINE_COST: old selection clipped before confirmation.\r\n");
    }
  }
  i32 total=0;
  if(!rd(menu+0x88ULL,&total,4)){
    g_active=0;logSimple("MINE_COST_ERROR: cannot emulate confirmation total load.\r\n");
  }
  cs(c,RAXOFF,(u64)(u32)total);
  cs(c,RIPOFF,g_mineConfirmHook.address+6ULL);
}


// The exact active MINE contribution menu is case 9 in FUN_14155A9C0,
// which invokes FUN_14155FCF0 at callsite 0x155AB14. The latter constructs
// 1/1000 directly: iVar7 = selected + donated, iVar6 = [stage+0x20].
//
// At 0x155FDA7 (only inside this menu) original code is MOV EBX,[RCX+0x20].
// The altered EBX is *only* used for the visible denominator, ratios,
// progress bar and enabled/disabled UI flags. It is NOT written to the game,
// mine budget or player wallet. Everything else stays vanilla.
static void mineMenuDenominator(CONTEXT*c){
  u64 cfg=cg(c,RCXOFF),viewController=cg(c,R13OFF);
  i32 native=0;
  if(!rd(cfg+0x20ULL,&native,4)){
    g_active=0;
    logSimple("MINE_MENU_ERROR: UI stage price unreadable; mod disabled.\r\n");
    cs(c,RBXOFF,0);
    cs(c,RIPOFF,g_mineMenuDenomHook.address+3ULL);
    return;
  }
  i32 displayed=native;
  if(g_active&&g_mineCrystalCost>0&&native>0&&native<=MAX_VALUE){
    u64 menu=0;u8 mode=0;
    if(rd(viewController+0xe0ULL,&menu,8)&&
       rd(menu+0xc30ULL,&mode,1)&&mode==9){
      displayed=reducedMineCost(native);
      LONG count=InterlockedIncrement(&g_mineMenuDenomHits);
      if(g_debugLog&&count<=12){
        char log[170];DWORD n=0;
        n=appendText(log,n,sizeof(log),"MINE_MENU_DENOM mode=");
        n=appendU32(log,n,sizeof(log),(u32)mode);
        n=appendText(log,n,sizeof(log)," original=");
        n=appendU32(log,n,sizeof(log),(u32)native);
        n=appendText(log,n,sizeof(log)," displayed=");
        n=appendU32(log,n,sizeof(log),(u32)displayed);
        if(n+2<sizeof(log)){log[n++]='\r';log[n++]='\n';}
        logRaw(log,n);
      }
    }
  }
  // MOV EBX,[RCX+0x20] zero-extends to 64-bit; emulate exactly.
  cs(c,RBXOFF,(u64)(u32)displayed);
  cs(c,RIPOFF,g_mineMenuDenomHook.address+3ULL);
}

static LONG WINAPI veh(EXCEPTION_POINTERS*e){
  if(!e||!e->ExceptionRecord||!e->ContextRecord||
     e->ExceptionRecord->ExceptionCode!=EXCEPTION_BREAKPOINT)
    return EXCEPTION_CONTINUE_SEARCH;
  CONTEXT*c=e->ContextRecord;
  u64 rip=cg(c,RIPOFF),ea=(u64)(ULONG_PTR)e->ExceptionRecord->ExceptionAddress;
  if(g_mineMenuDenomHook.installed&&
     (ea==g_mineMenuDenomHook.address||rip==g_mineMenuDenomHook.address+1ULL)){
    mineMenuDenominator(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  if(g_mineGetterHook.installed&&(ea==g_mineGetterHook.address||rip==g_mineGetterHook.address+1ULL)){
    mineGetter(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  if(g_mineProcessHook.installed&&(ea==g_mineProcessHook.address||rip==g_mineProcessHook.address+1ULL)){
    mineProcess(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  if(g_mineLoopHook.installed&&(ea==g_mineLoopHook.address||rip==g_mineLoopHook.address+1ULL)){
    mineLoop(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  if(g_mineBudgetHook.installed&&(ea==g_mineBudgetHook.address||rip==g_mineBudgetHook.address+1ULL)){
    mineBudgetCap(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  if(g_mineUiHook.installed&&(ea==g_mineUiHook.address||rip==g_mineUiHook.address+1ULL)){
    mineUiSelection(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  if(g_mineConfirmHook.installed&&(ea==g_mineConfirmHook.address||rip==g_mineConfirmHook.address+1ULL)){
    mineConfirm(c);return EXCEPTION_CONTINUE_EXECUTION;
  }
  HookPoint*h=0;bool need=false;
  if((ea==g_needHook.address||rip==g_needHook.address+1ULL)&&g_needHook.installed){
    h=&g_needHook;need=true;
  }else if((ea==g_currentHook.address||rip==g_currentHook.address+1ULL)&&g_currentHook.installed){
    h=&g_currentHook;need=false;
  }else return EXCEPTION_CONTINUE_SEARCH;
  if(g_active){
    if(need)handleNeed(c);else handleCurrent(c);
  }
  u64 rsp=cg(c,RSPOFF),newRcx=0;
  u64 offset=need?0x48ULL:0x50ULL;
  if(!rd(rsp+offset,&newRcx,8)){
    g_active=0;
    logSimple("ERROR: cannot emulate material MOV; session disabled.\r\n");
    cs(c,RIPOFF,h->address+5ULL);
    return EXCEPTION_CONTINUE_EXECUTION;
  }
  cs(c,RCXOFF,newRcx);
  cs(c,RIPOFF,h->address+5ULL);
  return EXCEPTION_CONTINUE_EXECUTION;
}

static DWORD WINAPI worker(LPVOID){while(!g_shutdown&&!(g_mod=GetModuleHandleW(0)))Sleep(100);if(g_shutdown||!g_mod)return 0;if(!isDs2Process())return 0;if(!boolIni(L"Enabled",true))return 0;g_debugLog=boolIni(L"DebugLog",false);g_remainingUnits=uintIni(L"RemainingUnits",1,1,1000000);g_roadConstruction=boolIni(L"RoadConstruction",true);g_roadUpgrades=boolIni(L"RoadUpgrades",true);g_monorailConstruction=boolIni(L"MonorailConstruction",true);g_mineConstruction=boolIni(L"MineConstruction",true);g_mineUpgrades=boolIni(L"MineUpgrades",true);
g_mineCrystalCost=uintIni(L"MineProductionCrystalCost",0,0,1000000);
openDebugLog();g_moduleBase=(u64)(ULONG_PTR)g_mod;if(!getBuild(&g_actualTimestamp,&g_actualImage)){logSimple("ERROR: could not read DS2.exe build metadata; no hooks installed.\r\n");closeDebugLog();return 0;}if(g_actualTimestamp!=EXPECTED_TIMESTAMP||g_actualImage!=EXPECTED_IMAGE){logSimple("ERROR: unsupported DS2.exe build metadata; no hooks installed.\r\n");closeDebugLog();return 0;}if(!signaturesOk()){logSimple("ERROR: instruction signature mismatch; no hooks installed.\r\n");closeDebugLog();return 0;}
if(g_mineCrystalCost>0&&!mineCostSignaturesOk()){
  logSimple("ERROR: unsupported mine production signatures; new option disabled.\r\n");
  g_mineCrystalCost=0;
}
LARGE_INTEGER f;f.QuadPart=0;if(QueryPerformanceFrequency(&f))g_qpcFrequency=f.QuadPart;g_veh=AddVectoredExceptionHandler(1,veh);if(!g_veh){logSimple("ERROR: AddVectoredExceptionHandler failed; no hooks installed.\r\n");closeDebugLog();return 0;}if(!installHook(&g_needHook,g_moduleBase+NEED_RESULT_RVA,g_needExpected[0])||!installHook(&g_currentHook,g_moduleBase+CURRENT_RESULT_RVA,g_currentExpected[0])){removeHooks();RemoveVectoredExceptionHandler(g_veh);g_veh=0;logSimple("ERROR: could not arm hooks; remove conflicting ASIs and restart DS2.\r\n");closeDebugLog();return 0;}if(g_mineCrystalCost>0){
  bool ok=installHook(&g_mineGetterHook,g_moduleBase+MINE_COST_GETTER_RVA,g_getterExpected[0])&&
    installHook(&g_mineProcessHook,g_moduleBase+MINE_COST_PROCESS_RVA,g_processExpected[0])&&
    installHook(&g_mineLoopHook,g_moduleBase+MINE_COST_LOOP_RVA,g_loopExpected[0])&&
    installHook(&g_mineBudgetHook,g_moduleBase+MINE_COST_BUDGET_RVA,g_budgetExpected[0])&&
    installHook(&g_mineUiHook,g_moduleBase+MINE_COST_UI_RVA,g_uiExpected[0])&&
    installHook(&g_mineConfirmHook,g_moduleBase+MINE_COST_CONFIRM_RVA,g_confirmExpected[0])&&
    installHook(&g_mineMenuDenomHook,g_moduleBase+MINE_MENU_DENOM_RVA,g_mineMenuDenomExpected[0]);
  if(!ok){
    removeMineCostHooks();
    g_mineCrystalCost=0;
    logSimple("ERROR: mine cost hook installation failed; optional mode disabled.\r\n");
  }
}
g_active=1;
logSimple("ACTIVE: exact Need/Current caller-pair hooks armed. Known caller pairs: 16.\r\n");
if(g_mineCrystalCost>0){
  char b[100];DWORD n=0;
  n=appendText(b,n,sizeof(b),"ACTIVE: mine crystal cost per batch=");
  n=appendU32(b,n,sizeof(b),g_mineCrystalCost);
  if(n+2<sizeof(b)){b[n++]='\r';b[n++]='\n';}
  logRaw(b,n);
}while(!g_shutdown&&g_active)Sleep(100);g_active=0;removeHooks();LPVOID v=g_veh;g_veh=0;if(v)RemoveVectoredExceptionHandler(v);closeDebugLog();return 0;}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){if(reason==1){g_self=module;DisableThreadLibraryCalls(module);HANDLE t=CreateThread(0,0,worker,0,0,0);if(t)CloseHandle(t);}else if(reason==0){g_shutdown=1;g_active=0;removeHooks();}return TRUE;}
