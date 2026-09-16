// DS2 Proficiency Bonus Multiplier v1.0.0
// Target: DS2.exe 1.10.89.0
// One-shot data patch after proficiency registry initialization.
// No code hooks, no INT3, no post-patch polling.
// Strictly verifies the proven vanilla descriptor tables before any write.
// Scaling policy:
//   - varying row with level-1 neutral 1.0: 1 + (vanilla - 1) * multiplier
//   - varying row with level-1 neutral 0.0: vanilla * multiplier
//   - constant rows: unchanged
//   - Melee consecutive-attack count: scales around neutral count 1 and rounds to a whole number

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed int s32;
typedef signed long long s64;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const wchar_t* LPCWSTR;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned long long SIZE_T;
typedef int BOOL;
#define WINAPI __stdcall
#define DLL_PROCESS_ATTACH 1u
#define TRUE_VALUE 1
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define INVALID_HANDLE_VALUE ((HANDLE)(s64)-1)
#define MEM_COMMIT 0x1000u
#define PAGE_NOACCESS 0x01u
#define PAGE_GUARD 0x100u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u

struct MEMORY_BASIC_INFORMATION_X64 { LPVOID BaseAddress; LPVOID AllocationBase; DWORD AllocationProtect; WORD PartitionId; u16 _pad0; SIZE_T RegionSize; DWORD State; DWORD Protect; DWORD Type; DWORD _pad1; };
typedef DWORD (WINAPI *ThreadProc)(LPVOID);
extern "C" {
__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(LPCWSTR);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE,wchar_t*,DWORD);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE,const void*,DWORD,DWORD*,void*);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE);
__declspec(dllimport) HANDLE WINAPI CreateThread(void*,SIZE_T,ThreadProc,LPVOID,DWORD,DWORD*);
__declspec(dllimport) void WINAPI Sleep(DWORD);
__declspec(dllimport) SIZE_T WINAPI VirtualQuery(const void*,MEMORY_BASIC_INFORMATION_X64*,SIZE_T);
__declspec(dllimport) DWORD WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,wchar_t*,DWORD,LPCWSTR);
int _fltused=0x9875;
}

static const u32 kExpectedTimestamp=0x6A3DAE46u;
static const u32 kExpectedImageSize=0x0B292000u;
static const u32 kDSPlayerSystemGlobalRva=0x0623E9C8u;
static HMODULE gSelf=0; static u64 gBase=0; static HANDLE gLog=INVALID_HANDLE_VALUE;

static char* put_s(char* p,const char* s){while(*s)*p++=*s++;return p;}
static char* put_u64(char* p,u64 v){char t[32];u32 n=0;if(!v){*p++='0';return p;}while(v&&n<31){t[n++]=(char)('0'+v%10u);v/=10u;}while(n)*p++=t[--n];return p;}
static char* put_hex(char* p,u64 v,u32 d){static const char h[]="0123456789ABCDEF";*p++='0';*p++='x';for(s32 i=(s32)d-1;i>=0;--i)*p++=h[(v>>(i*4))&0xFu];return p;}
static char* put_f32(char* p,float v){union{float f;u32 u;}z;z.f=v;if(v<0.0f){*p++='-';v=-v;}u64 ip=(u64)v;float frac=v-(float)ip;u32 fp=(u32)(frac*1000000.0f+0.5f);if(fp>=1000000u){++ip;fp-=1000000u;}p=put_u64(p,ip);*p++='.';u32 div=100000u;for(u32 i=0;i<6u;++i){*p++=(char)('0'+(fp/div)%10u);div/=10u;}p=put_s(p," (bits=");p=put_hex(p,z.u,8);*p++=')';return p;}
static void write_raw(const char* s,u32 n){if(gLog==INVALID_HANDLE_VALUE)return;DWORD w=0;WriteFile(gLog,s,n,&w,0);} 
static void line(const char* s){char b[1024];char* p=b;p=put_s(p,s);*p++='\r';*p++='\n';write_raw(b,(u32)(p-b));}
static bool sibling_path(const wchar_t* name,wchar_t* path,u32 cap){DWORD n=GetModuleFileNameW(gSelf,path,cap);if(!n||n+2u>=cap)return false;s32 slash=-1;for(u32 i=0;i<n;++i)if(path[i]==L'\\'||path[i]==L'/')slash=(s32)i;u32 pos=(slash>=0)?(u32)slash+1u:0u;u32 j=0;while(name[j]&&pos+j+1u<cap){path[pos+j]=name[j];++j;}if(name[j])return false;path[pos+j]=0;return true;}
static bool open_log(const wchar_t* name){wchar_t path[1024];if(!sibling_path(name,path,1024u))return false;gLog=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);return gLog!=INVALID_HANDLE_VALUE;}
static bool pe_identity(HMODULE m,u32* ts,u32* is){if(ts)*ts=0;if(is)*is=0;if(!m)return false;u8* b=(u8*)m;u32 lf=*(u32*)(b+0x3Cu);u8* nt=b+lf;if(*(u32*)nt!=0x00004550u)return false;u32 t=*(u32*)(nt+8u);u8* opt=nt+24u;u32 s=*(u32*)(opt+0x38u);if(ts)*ts=t;if(is)*is=s;return true;}
static bool region_ok(const void* ptr,u64 bytes,bool wantWrite){if(!ptr||!bytes)return false;MEMORY_BASIC_INFORMATION_X64 m;SIZE_T n=VirtualQuery(ptr,&m,(SIZE_T)sizeof(m));if(!n)return false;if(m.State!=MEM_COMMIT||(m.Protect&PAGE_NOACCESS)||(m.Protect&PAGE_GUARD))return false;if(wantWrite){u32 p=m.Protect&0xFFu;if(p!=PAGE_READWRITE&&p!=PAGE_WRITECOPY&&p!=PAGE_EXECUTE_READWRITE&&p!=PAGE_EXECUTE_WRITECOPY)return false;}u64 a=(u64)ptr,b=(u64)m.BaseAddress,e=b+(u64)m.RegionSize;if(a<b||a>e)return false;return bytes<=e-a;}
static bool readable(const void* p,u64 n){return region_ok(p,n,false);} static bool writable(const void* p,u64 n){return region_ok(p,n,true);} 
static bool r8(u64 a,u8* v){if(!readable((void*)a,1))return false;*v=*(u8*)a;return true;} 
static bool r16(u64 a,u16* v){if(!readable((void*)a,2))return false;*v=*(u16*)a;return true;} 
static bool r32(u64 a,u32* v){if(!readable((void*)a,4))return false;*v=*(u32*)a;return true;} 
static bool r64(u64 a,u64* v){if(!readable((void*)a,8))return false;*v=*(u64*)a;return true;}
static u32 fbits(float f){union{float f;u32 u;}z;z.f=f;return z.u;}
static float bitsf(u32 u){union{float f;u32 u;}z;z.u=u;return z.f;}
static float parse_float(const wchar_t* s,float def){if(!s)return def;u32 i=0;while(s[i]==L' '||s[i]==L'\t')++i;bool neg=false;if(s[i]==L'-'){neg=true;++i;}else if(s[i]==L'+')++i;bool any=false;float v=0.0f;while(s[i]>=L'0'&&s[i]<=L'9'){any=true;v=v*10.0f+(float)(s[i]-L'0');++i;}if(s[i]==L'.'||s[i]==L','){++i;float m=0.1f;while(s[i]>=L'0'&&s[i]<=L'9'){any=true;v+=(float)(s[i]-L'0')*m;m*=0.1f;++i;}}if(!any)return def;if(neg)v=-v;return v;}
static s32 parse_int(const wchar_t* s,s32 def){if(!s)return def;u32 i=0;while(s[i]==L' '||s[i]==L'\t')++i;bool neg=false;if(s[i]==L'-'){neg=true;++i;}else if(s[i]==L'+')++i;bool any=false;s32 v=0;while(s[i]>=L'0'&&s[i]<=L'9'){any=true;v=v*10+(s32)(s[i]-L'0');++i;}if(!any)return def;return neg?-v:v;}
static float ini_float(const wchar_t* path,const wchar_t* section,const wchar_t* key,float fallback){wchar_t b[64];GetPrivateProfileStringW(section,key,L"1.0",b,64u,path);float v=parse_float(b,fallback);if(v<0.0f)v=0.0f;if(v>10.0f)v=10.0f;return v;}
static bool ini_enabled(const wchar_t* path,const wchar_t* section,bool fallback){wchar_t b[32];GetPrivateProfileStringW(section,L"Enabled",fallback?L"1":L"0",b,32u,path);return parse_int(b,fallback?1:0)!=0;}
static bool ini_bool_key(const wchar_t* path,const wchar_t* section,const wchar_t* key,bool fallback){wchar_t b[32];GetPrivateProfileStringW(section,key,fallback?L"1":L"0",b,32u,path);return parse_int(b,fallback?1:0)!=0;}

enum RowMode { RM_NONE=0, RM_UNIT=1, RM_ZERO=2, RM_CONSTANT=3, RM_UNSUPPORTED=4, RM_COUNT=5 };
struct StatSpec {
    const wchar_t* section;
    const char* name;
    u32 id20;
    u16 f38,f3a,f3c;
    s32 thresholds[5];
    u8 rowCount[4];
    u32 rows[4][5];
    bool defaultEnabled;
};

#define F(x) x##u
static const StatSpec kStats[] = {
 {L"TerrainTraversal","TerrainTraversal",0,0x0008,0x0075,0x0008,{0,150,300,700,1500},{5,5,0,0},{{F(0x3F800000),F(0x3F666666),F(0x3F4CCCCD),F(0x3F333333),F(0x3F19999A)},{F(0x3F800000),F(0x3F666666),F(0x3F4CCCCD),F(0x3F333333),F(0x3F19999A)},{0,0,0,0,0},{0,0,0,0,0}},true},
 {L"CarryingCapacity","CarryingCapacity",1,0x000A,0x0075,0x0008,{0,3000,10000,25000,50000},{5,5,5,5},{{F(0x00000000),F(0x41200000),F(0x41700000),F(0x41A00000),F(0x41F00000)},{F(0x41A00000),F(0x41A00000),F(0x41A00000),F(0x41A00000),F(0x41A00000)},{F(0x42700000),F(0x42700000),F(0x42700000),F(0x42700000),F(0x42700000)},{F(0x3F99999A),F(0x3F99999A),F(0x3F99999A),F(0x3F99999A),F(0x3F99999A)}},true},
 {L"Endurance","Endurance",2,0x000E,0x0075,0x0008,{0,1000,5000,10000,30000},{5,0,0,0},{{F(0x3F800000),F(0x3F666666),F(0x3F59999A),F(0x3F4CCCCD),F(0x3F333333)},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},true},
 {L"MeleeCombat","MeleeCombat",3,0x002E,0x003F,0x0008,{0,15,30,60,100},{5,5,0,0},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F8CCCCD),F(0x3F99999A),F(0x3F99999A)},{F(0x40400000),F(0x40400000),F(0x40800000),F(0x40800000),F(0x40A00000)},{0,0,0,0,0},{0,0,0,0,0}},true},
 {L"MachineGuns","MachineGuns",4,0x002E,0x0041,0x0008,{0,15,30,60,100},{5,5,5,5},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)}},true},
 {L"AssaultRifles","AssaultRifles",5,0x002E,0x0043,0x0008,{0,15,30,60,100},{5,5,5,5},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)}},true},
 {L"Shotguns","Shotguns",6,0x002E,0x0045,0x0008,{0,15,30,60,100},{5,5,5,5},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)}},true},
 {L"SniperRifles","SniperRifles",7,0x002E,0x0047,0x0008,{0,15,30,60,100},{5,5,5,5},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)}},true},
 {L"Handguns","Handguns",8,0x002E,0x0049,0x0008,{0,15,30,60,100},{5,5,5,5},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)},{F(0x3F800000),F(0x3F600000),F(0x3F400000),F(0x3F200000),F(0x3F000000)}},true},
 {L"StrandSkills","StrandSkills",9,0x002E,0x004B,0x0008,{0,30,100,200,300},{5,5,5,5},{{F(0x3F800000),F(0x3F866666),F(0x3F8CCCCD),F(0x3F933333),F(0x3F99999A)},{F(0x3F800000),F(0x3F866666),F(0x3F8CCCCD),F(0x3F933333),F(0x3F99999A)},{F(0x3F800000),F(0x3F866666),F(0x3F8CCCCD),F(0x3F933333),F(0x3F99999A)},{F(0x3F800000),F(0x3F99999A),F(0x3FC00000),F(0x3FD9999A),F(0x40000000)}},true},
 {L"KOResistance","KOResistance",10,0x000C,0x0075,0x0008,{0,300,1000,3000,10000},{5,0,0,0},{{F(0x3F800000),F(0x3F666666),F(0x3F59999A),F(0x3F4CCCCD),F(0x3F333333)},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},true},
 {L"LungCapacity","LungCapacity",11,0x002E,0x0075,0x0006,{0,60,300,600,1200},{5,0,0,0},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},true},
 {L"StealthTakedownInternal","StealthTakedownInternal",12,0x002E,0x000B,0x0008,{0,15,30,60,100},{5,0,0,0},{{F(0x3F800000),F(0x3F8CCCCD),F(0x3F99999A),F(0x3FA66666),F(0x3FB33333)},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},false}
};
static const u32 kStatCount=(u32)(sizeof(kStats)/sizeof(kStats[0]));

static void log_mults(const StatSpec& s,bool enabled,const float m[5]){char b[1024];char* p=b;p=put_s(p,"CONFIG stat=");p=put_s(p,s.name);p=put_s(p," enabled=");p=put_u64(p,enabled?1u:0u);p=put_s(p," multipliers=");for(u32 i=0;i<5u;++i){if(i)*p++=',';p=put_f32(p,m[i]);}p=put_s(p,"\r\n");write_raw(b,(u32)(p-b));}
static void log_stat(const char* prefix,const StatSpec& s,u64 d){char b[1024];char* p=b;p=put_s(p,prefix);p=put_s(p," stat=");p=put_s(p,s.name);p=put_s(p," descriptor=");p=put_hex(p,d,16);p=put_s(p,"\r\n");write_raw(b,(u32)(p-b));}
static void log_row(const char* prefix,const StatSpec& s,u32 row,RowMode mode){char b[1024];char* p=b;p=put_s(p,prefix);p=put_s(p," stat=");p=put_s(p,s.name);p=put_s(p," row=");p=put_u64(p,row);p=put_s(p," mode=");const char* n=(mode==RM_UNIT)?"UNIT_BASE_1":(mode==RM_ZERO)?"ZERO_BASE_0":(mode==RM_CONSTANT)?"CONSTANT_SKIP":(mode==RM_UNSUPPORTED)?"UNSUPPORTED_SKIP":(mode==RM_COUNT)?"COUNT_BASE_1_ROUNDED":"NONE";p=put_s(p,n);p=put_s(p,"\r\n");write_raw(b,(u32)(p-b));}
static void log_patch(const StatSpec& s,u32 row,u32 lv,float before,float after,float mult){char b[1024];char* p=b;p=put_s(p,"PATCH stat=");p=put_s(p,s.name);p=put_s(p," row=");p=put_u64(p,row);p=put_s(p," level_index=");p=put_u64(p,lv);p=put_s(p," multiplier=");p=put_f32(p,mult);p=put_s(p," before=");p=put_f32(p,before);p=put_s(p," after=");p=put_f32(p,after);p=put_s(p,"\r\n");write_raw(b,(u32)(p-b));}

static bool verify_descriptor(u64 d,const StatSpec& s,u64 ptrs[4]){
    if(!d||!readable((void*)d,0x90u))return false;
    u32 id=0;u16 f38=0,f3a=0,f3c=0;u8 count40=0;u64 tp=0;
    if(!r32(d+0x20u,&id)||id!=s.id20)return false;
    if(!r16(d+0x38u,&f38)||f38!=s.f38||!r16(d+0x3Au,&f3a)||f3a!=s.f3a||!r16(d+0x3Cu,&f3c)||f3c!=s.f3c)return false;
    if(!r8(d+0x40u,&count40)||count40!=5u||!r64(d+0x48u,&tp)||!tp||!readable((void*)tp,20u))return false;
    for(u32 i=0;i<5u;++i)if(*(s32*)(tp+(u64)i*4u)!=s.thresholds[i])return false;
    const u32 off[4]={0x50u,0x60u,0x70u,0x80u};
    for(u32 row=0;row<4u;++row){ptrs[row]=0;u32 c=0;u64 p=0;if(!r32(d+off[row],&c)||!r64(d+off[row]+8u,&p))return false;if(c!=s.rowCount[row])return false;if(c==0u){if(p!=0u)return false;continue;}if(c!=5u||!p||!readable((void*)p,20u)||!writable((void*)p,20u))return false;for(u32 lv=0;lv<5u;++lv)if(*(u32*)(p+(u64)lv*4u)!=s.rows[row][lv])return false;ptrs[row]=p;}
    return true;
}
static RowMode row_mode(const StatSpec& s,u32 row){if(row>=4u||s.rowCount[row]==0u)return RM_NONE;if(s.id20==3u&&row==1u)return RM_COUNT;bool same=true;for(u32 i=1;i<5u;++i)if(s.rows[row][i]!=s.rows[row][0]){same=false;break;}if(same)return RM_CONSTANT;if(s.rows[row][0]==0x3F800000u)return RM_UNIT;if(s.rows[row][0]==0x00000000u)return RM_ZERO;return RM_UNSUPPORTED;}
static float calc_after(RowMode mode,float before,float mult,bool clampReductions){
    if(mode==RM_UNIT){
        float after=1.0f+(before-1.0f)*mult;
        if(clampReductions&&before<1.0f&&after<0.0f)after=0.0f;
        return after;
    }
    if(mode==RM_ZERO)return before*mult;
    if(mode==RM_COUNT){
        float raw=1.0f+(before-1.0f)*mult;
        if(raw<1.0f)raw=1.0f;
        if(raw>255.0f)raw=255.0f;
        u32 rounded=(u32)(raw+0.5f);
        if(rounded<1u)rounded=1u;
        return (float)rounded;
    }
    return before;
}

static DWORD WINAPI worker(LPVOID){
    if(!open_log(L"ds2_proficiency_bonus_multiplier.log"))return 0;
    line("DS2 Proficiency Bonus Multiplier v1.0.0");
    line("MODE=one-shot data patch; no code hook; no INT3; no post-patch polling");
    line("POLICY=baseline-1 rows scale around 1; baseline-0 rows scale around 0; MeleeCombat consecutive-attack count scales around neutral count 1 and rounds to whole attacks; constant/unknown rows are unchanged");
    wchar_t ini[1024];if(!sibling_path(L"DS2_ProficiencyBonusMultiplier.ini",ini,1024u)){line("ERROR: could not construct INI path. No changes made.");CloseHandle(gLog);gLog=INVALID_HANDLE_VALUE;return 0;}
    bool clampReductions=ini_bool_key(ini,L"Safety",L"ClampReductionFactorsAtZero",true);
    {char sb[256];char* sp=sb;sp=put_s(sp,"SAFETY ClampReductionFactorsAtZero=");sp=put_u64(sp,clampReductions?1u:0u);sp=put_s(sp,"\r\n");write_raw(sb,(u32)(sp-sb));}
    bool enabled[kStatCount];float mult[kStatCount][5];
    for(u32 si=0;si<kStatCount;++si){enabled[si]=ini_enabled(ini,kStats[si].section,kStats[si].defaultEnabled);for(u32 lv=0;lv<5u;++lv){const wchar_t* key=(lv==0u)?L"Level1Multiplier":(lv==1u)?L"Level2Multiplier":(lv==2u)?L"Level3Multiplier":(lv==3u)?L"Level4Multiplier":L"Level5Multiplier";mult[si][lv]=ini_float(ini,kStats[si].section,key,1.0f);}log_mults(kStats[si],enabled[si],mult[si]);}
    char b[1024];char* p=b;p=put_s(p,"EXE base=");p=put_hex(p,gBase,16);p=put_s(p," timestamp=0x6A3DAE46 image_size=0x0B292000\r\n");write_raw(b,(u32)(p-b));
    u64 dsp=0,mgr=0,list=0;u32 count=0,attempt=0;for(attempt=0;attempt<2400u;++attempt){if(r64(gBase+kDSPlayerSystemGlobalRva,&dsp)&&dsp&&r64(dsp+0x18u,&mgr)&&mgr&&r32(mgr+0x110u,&count)&&count==13u&&r64(mgr+0x118u,&list)&&list&&readable((void*)list,(u64)count*8u))break;if((attempt%20u)==19u){p=b;p=put_s(p,"WAIT seconds=");p=put_u64(p,(attempt+1u)/4u);p=put_s(p," dsp=");p=put_hex(p,dsp,16);p=put_s(p," mgr=");p=put_hex(p,mgr,16);p=put_s(p," count=");p=put_u64(p,count);p=put_s(p,"\r\n");write_raw(b,(u32)(p-b));}Sleep(250u);}if(attempt>=2400u){line("ERROR: proficiency registry did not initialize within 600 seconds. No changes made.");CloseHandle(gLog);gLog=INVALID_HANDLE_VALUE;return 0;}

    u64 desc[kStatCount];u64 ptrs[kStatCount][4];for(u32 si=0;si<kStatCount;++si){desc[si]=0;for(u32 r=0;r<4u;++r)ptrs[si][r]=0;}
    for(u32 i=0;i<count;++i){u64 d=0;u32 id=0;if(!r64(list+(u64)i*8u,&d)||!d||!r32(d+0x20u,&id))continue;for(u32 si=0;si<kStatCount;++si)if(id==kStats[si].id20)desc[si]=d;}
    for(u32 si=0;si<kStatCount;++si){if(!desc[si]){log_stat("ERROR_MISSING",kStats[si],0);line("ABORT: one or more proven descriptors are missing. No changes made.");CloseHandle(gLog);gLog=INVALID_HANDLE_VALUE;return 0;}if(!verify_descriptor(desc[si],kStats[si],ptrs[si])){log_stat("ERROR_VERIFY",kStats[si],desc[si]);line("ABORT: a descriptor/table does not exactly match the proven vanilla snapshot. Disable older proof/mapping ASIs. No changes made.");CloseHandle(gLog);gLog=INVALID_HANDLE_VALUE;return 0;}log_stat("VERIFY_PASS",kStats[si],desc[si]);}
    line("VERIFY_ALL=PASS all 13 descriptors exactly match the proven vanilla runtime snapshot");

    u32 patched=0,skippedConstant=0,skippedUnsupported=0,disabledStats=0;
    for(u32 si=0;si<kStatCount;++si){if(!enabled[si]){++disabledStats;log_stat("DISABLED",kStats[si],desc[si]);continue;}for(u32 row=0;row<4u;++row){RowMode mode=row_mode(kStats[si],row);if(mode==RM_NONE)continue;log_row("ROW",kStats[si],row,mode);if(mode==RM_CONSTANT){++skippedConstant;continue;}if(mode==RM_UNSUPPORTED){++skippedUnsupported;continue;}for(u32 lv=0;lv<5u;++lv){float before=*(float*)(ptrs[si][row]+(u64)lv*4u);float rawAfter=(mode==RM_UNIT)?(1.0f+(before-1.0f)*mult[si][lv]):((mode==RM_ZERO)?before*mult[si][lv]:((mode==RM_COUNT)?(1.0f+(before-1.0f)*mult[si][lv]):before));float after=calc_after(mode,before,mult[si][lv],clampReductions);if(after!=rawAfter){char cb[512];char* cp=cb;cp=put_s(cp,"CLAMP stat=");cp=put_s(cp,kStats[si].name);cp=put_s(cp," row=");cp=put_u64(cp,row);cp=put_s(cp," level_index=");cp=put_u64(cp,lv);cp=put_s(cp," raw_after=");cp=put_f32(cp,rawAfter);cp=put_s(cp," clamped_after=");cp=put_f32(cp,after);cp=put_s(cp,"\r\n");write_raw(cb,(u32)(cp-cb));}*(float*)(ptrs[si][row]+(u64)lv*4u)=after;log_patch(kStats[si],row,lv,before,after,mult[si][lv]);++patched;}}}

    bool ok=true;for(u32 si=0;si<kStatCount;++si){if(!enabled[si])continue;for(u32 row=0;row<4u;++row){RowMode mode=row_mode(kStats[si],row);if(mode!=RM_UNIT&&mode!=RM_ZERO&&mode!=RM_COUNT)continue;for(u32 lv=0;lv<5u;++lv){float vanilla=bitsf(kStats[si].rows[row][lv]);float expected=calc_after(mode,vanilla,mult[si][lv],clampReductions);if(fbits(*(float*)(ptrs[si][row]+(u64)lv*4u))!=fbits(expected))ok=false;}}}
    p=b;p=put_s(p,"SUMMARY patched_values=");p=put_u64(p,patched);p=put_s(p," skipped_constant_rows=");p=put_u64(p,skippedConstant);p=put_s(p," skipped_unsupported_rows=");p=put_u64(p,skippedUnsupported);p=put_s(p," disabled_stats=");p=put_u64(p,disabledStats);p=put_s(p,"\r\n");write_raw(b,(u32)(p-b));
    if(ok){line("PATCH_VERIFY=PASS all supported rows match INI-derived expectations");line("NOTE: MeleeCombat row 1 is confirmed as consecutive-attack count (3/3/4/4/5) and is scaled around neutral count 1, rounded to whole attacks.");line("NOTE: CarryingCapacity constant rows are intentionally not modified; only its level-varying 0/10/15/20/30 row is scalable.");line("NOTE: reduction-type factors are clamped at 0.0 by default to prevent negative gameplay multipliers; set [Safety] ClampReductionFactorsAtZero=0 only for deliberate experiments.");line("DONE. Multi-stat INI patch applied.");}else line("PATCH_VERIFY=FAIL unexpected post-write value detected");
    CloseHandle(gLog);gLog=INVALID_HANDLE_VALUE;return 0;
}
static wchar_t lower_ascii(wchar_t c){if(c>=L'A'&&c<=L'Z')return (wchar_t)(c+(L'a'-L'A'));return c;}
static bool host_is_ds2_exe(HMODULE host){
    wchar_t path[1024];DWORD n=GetModuleFileNameW(host,path,1024u);if(!n||n>=1024u)return false;
    s32 slash=-1;for(u32 i=0;i<n;++i)if(path[i]==L'\\'||path[i]==L'/')slash=(s32)i;
    const wchar_t* name=path+((slash>=0)?slash+1:0);const wchar_t target[]=L"DS2.exe";
    u32 i=0;for(;target[i];++i){if(!name[i]||lower_ascii(name[i])!=lower_ascii(target[i]))return false;}return name[i]==0;
}
static void unsupported_ds2(HMODULE host,u32 ts,u32 is){
    if(!open_log(L"ds2_proficiency_bonus_multiplier.log"))return;
    char b[768];char* p=b;
    p=put_s(p,"DS2 Proficiency Bonus Multiplier v1.0.0\r\nSTATUS=UNSUPPORTED_GAME_BUILD\r\nHOST base=");p=put_hex(p,(u64)host,16);
    p=put_s(p," timestamp=");p=put_hex(p,ts,8);p=put_s(p," image_size=");p=put_hex(p,is,8);
    p=put_s(p,"\r\nEXPECTED timestamp=0x6A3DAE46 image_size=0x0B292000 (DS2.exe 1.10.89.0)\r\nNo game data was modified.\r\n");
    write_raw(b,(u32)(p-b));CloseHandle(gLog);gLog=INVALID_HANDLE_VALUE;
}
extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE mod,DWORD reason,void*){
    if(reason==DLL_PROCESS_ATTACH){
        gSelf=mod;DisableThreadLibraryCalls(mod);HMODULE host=GetModuleHandleW(0);u32 ts=0,is=0;
        if(pe_identity(host,&ts,&is)&&ts==kExpectedTimestamp&&is==kExpectedImageSize){gBase=(u64)host;HANDLE th=CreateThread(0,0,worker,0,0,0);if(th)CloseHandle(th);}
        else if(host_is_ds2_exe(host))unsupported_ds2(host,ts,is);
    }
    return TRUE_VALUE;
}
