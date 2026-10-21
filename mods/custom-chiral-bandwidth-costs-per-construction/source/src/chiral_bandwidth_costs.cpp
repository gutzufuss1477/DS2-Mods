#include "windows.h"

extern "C" int _fltused=0;

extern "C" void* memcpy(void* destination,const void* source,SIZE_T count){
    u8* out=(u8*)destination;
    const u8* in=(const u8*)source;
    for(SIZE_T i=0;i<count;i++)out[i]=in[i];
    return destination;
}

extern "C" void* memset(void* destination,int value,SIZE_T count){
    u8* out=(u8*)destination;
    for(SIZE_T i=0;i<count;i++)out[i]=(u8)value;
    return destination;
}

extern "C" long _InterlockedCompareExchange(volatile long*,long,long);
#pragma intrinsic(_InterlockedCompareExchange)

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u

static const u64 RVA_GLOBAL_ROOT=0x0623EAD8ull;
static const u64 RVA_MANAGER_VTABLE=0x03280940ull;
static const u64 RVA_FACILITY_VTABLE=0x032827C8ull;
static const u64 RVA_CAPACITY_CHECK=0x0133755Cull;
static const u64 RVA_ACCOUNTING_ADD=0x0127D859ull;
static const u64 RVA_ACCOUNTING_VALUE=0x0127D87Cull;
static const u64 RVA_ACCOUNTING_SUBTRACT=0x0129CCC5ull;
static const u64 RVA_UI_COST=0x016BCFF8ull;

static const u32 OFF_ROOT_MANAGER=0x28u;
static const u32 OFF_FACILITY_CONFIG=0x30u;
static const u32 OFF_QPID_COST=0x4E4u;
static const u32 MAX_CONFIGURED_COST=1000000u;

static const u64 MANAGER_VFUNCS[4]={0x012426B0ull,0x01274280ull,0x00109DE0ull,0x00109EB0ull};
static const u64 FACILITY_VFUNCS[4]={0x01242690ull,0x00291C50ull,0x00109DE0ull,0x00109EB0ull};

static const u8 EXPECTED_CAPACITY_CHECK[14]={
    0x8B,0x8B,0xE4,0x04,0x00,0x00,0x03,0x4C,0x24,0x58,0x3B,0x4C,0x24,0x68
};
static const u8 EXPECTED_ACCOUNTING_ADD[11]={
    0x8B,0x88,0xE4,0x04,0x00,0x00,0x43,0x01,0x4C,0x88,0x04
};
static const u8 EXPECTED_ACCOUNTING_VALUE[11]={
    0x8B,0x90,0xE4,0x04,0x00,0x00,0x48,0x8D,0x44,0x24,0x60
};
static const u8 EXPECTED_ACCOUNTING_SUBTRACT[10]={
    0x8B,0x90,0xE4,0x04,0x00,0x00,0x41,0x29,0x51,0x04
};
static const u8 EXPECTED_UI_COST[9]={
    0x8B,0x80,0xE4,0x04,0x00,0x00,0x89,0x45,0x88
};

struct SettingDefinition{
    const WCHAR* key;
    u32 nativeCost;
};

static const SettingDefinition SETTING_DEFINITIONS[]={
    {L"Postbox",200u},
    {L"SafeHouse",2500u},
    {L"Watchtower",250u},
    {L"CargoCatapult",450u},
    {L"Generator",350u},
    {L"RainShelter",500u},
    {L"Zipline",500u},
    {L"Bridge",1000u},
    {L"ChiralBridge",250u},
    {L"JumpRamp",550u},
    {L"HotSpringDigger",0u}
};
static const u32 SETTING_COUNT=(u32)(sizeof(SETTING_DEFINITIONS)/sizeof(SETTING_DEFINITIONS[0]));

struct TargetDefinition{
    const WCHAR* label;
    u32 settingIndex;
    u32 managerOffset;
    u32 alternateManagerOffset;
    u64 configVtableRva;
    u64 vfuncs[4];
};

static const TargetDefinition TARGETS[]={
    {L"Postbox",0u,0x38u,0u,0x03291028ull,{0x01305660ull,0x01305670ull,0x00109DE0ull,0x00109EB0ull}},
    {L"SafeHouse",1u,0x30u,0u,0x032913C0ull,{0x01307B90ull,0x01307C60ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Watchtower",2u,0x40u,0u,0x0327F8F8ull,{0x01245220ull,0x01245230ull,0x00109DE0ull,0x00109EB0ull}},
    {L"CargoCatapult",3u,0x48u,0u,0x03293488ull,{0x01311E70ull,0x01311FF0ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Generator",4u,0x50u,0u,0x032938E8ull,{0x0131A0F0ull,0x0131A220ull,0x00109DE0ull,0x00109EB0ull}},
    {L"RainShelter",5u,0x58u,0u,0x03291EC0ull,{0x01306030ull,0x013060D0ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Zipline",6u,0x68u,0u,0x032863A8ull,{0x01243560ull,0x01243720ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Bridge30m",7u,0x80u,0x138u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Bridge30mAlias",7u,0x138u,0x80u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Bridge45m",7u,0x140u,0u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull}},
    {L"Bridge80m",7u,0x148u,0u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull}},
    {L"ChiralBridge",8u,0x98u,0u,0x03292870ull,{0x0131CEC0ull,0x0131CED0ull,0x00109DE0ull,0x00109EB0ull}},
    {L"JumpRamp",9u,0xA0u,0u,0x03291CF0ull,{0x012EE520ull,0x012EE610ull,0x00109DE0ull,0x00109EB0ull}},
    {L"HotSpringDigger",10u,0xF0u,0u,0x032931F8ull,{0x0130C650ull,0x0130C660ull,0x00109DE0ull,0x00109EB0ull}}
};
static const u32 PATCH_TARGET_COUNT=(u32)(sizeof(TARGETS)/sizeof(TARGETS[0]));

struct CostSetting{
    bool custom;
    u32 value;
};

struct Settings{
    bool enabled;
    bool debugLog;
    CostSetting costs[sizeof(SETTING_DEFINITIONS)/sizeof(SETTING_DEFINITIONS[0])];
};

struct ProtectionPage{
    u8* address;
    DWORD originalProtection;
    DWORD restoreProtection;
    bool protectionChanged;
    bool protectionConflict;
};

struct Observation{
    u8* manager;
    u8* facility;
    u8* config;
    u32 stablePolls;
};

struct TransactionField{
    u32 targetIndex;
    u8* facility;
    u8* config;
    u8* field;
    u32 before;
    u32 after;
    bool changed;
    bool writeOwner;
    bool written;
};

enum SettingsResult{SETTINGS_INVALID,SETTINGS_DISABLED,SETTINGS_READY};
enum ResolveResult{RESOLVE_NOT_READY,RESOLVE_INVALID,RESOLVE_READY};
enum ApplyResult{APPLY_NOT_READY,APPLY_CONFLICT,APPLY_FAILED,APPLY_CRITICAL,APPLY_READY};

static HMODULE g_self=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static bool g_debugLog=false;
static volatile long g_started=0;
static WCHAR g_iniSection[8192];

#ifdef DS2_TESTING
static s32 g_testFailAfterWrites=-1;
static s32 g_testFailRestoreCalls=0;
static u64* g_testManagerSlot=0;
static u64 g_testManagerReplacement=0;
static u8* g_testFieldRace=0;
static u32 g_testFieldRaceValue=0;
static u8* g_testRollbackRace=0;
static u32 g_testRollbackRaceValue=0;
static u8* g_testProtectionRacePage=0;
static DWORD g_testProtectionRaceValue=0;
#endif

static bool bytesEqual(const u8* left,const u8* right,u32 count){
    if(!left||!right)return false;
    for(u32 i=0;i<count;i++)if(left[i]!=right[i])return false;
    return true;
}

static u16 readU16Raw(const u8* source){
    return (u16)((u16)source[0]|((u16)source[1]<<8));
}

static u32 readU32Raw(const u8* source){
    return (u32)source[0]|((u32)source[1]<<8)|((u32)source[2]<<16)|((u32)source[3]<<24);
}

static WCHAR lowerAscii(WCHAR value){
    return value>=L'A'&&value<=L'Z'?(WCHAR)(value+(L'a'-L'A')):value;
}

static bool equalsSpan(const WCHAR* value,u32 begin,u32 end,const WCHAR* token){
    if(!value||!token||end<begin)return false;
    u32 length=0;
    while(token[length])length++;
    if(length!=end-begin)return false;
    for(u32 i=0;i<length;i++)if(lowerAscii(value[begin+i])!=lowerAscii(token[i]))return false;
    return true;
}

static void trimSpan(const WCHAR* value,u32* begin,u32* end){
    if(!value||!begin||!end)return;
    while(*begin<*end&&(value[*begin]==L' '||value[*begin]==L'\t'))(*begin)++;
    while(*end>*begin&&(value[*end-1u]==L' '||value[*end-1u]==L'\t'))(*end)--;
}

static bool parseBoolSpan(const WCHAR* value,u32 begin,u32 end,bool* out){
    if(!value||!out)return false;
    trimSpan(value,&begin,&end);
    if(equalsSpan(value,begin,end,L"1")||equalsSpan(value,begin,end,L"true")||
       equalsSpan(value,begin,end,L"yes")||equalsSpan(value,begin,end,L"on")){
        *out=true;
        return true;
    }
    if(equalsSpan(value,begin,end,L"0")||equalsSpan(value,begin,end,L"false")||
       equalsSpan(value,begin,end,L"no")||equalsSpan(value,begin,end,L"off")){
        *out=false;
        return true;
    }
    return false;
}

static bool parseCostSpan(const WCHAR* value,u32 begin,u32 end,CostSetting* out){
    if(!value||!out)return false;
    trimSpan(value,&begin,&end);
    if(begin==end)return false;
    if(equalsSpan(value,begin,end,L"Native")){
        out->custom=false;
        out->value=0;
        return true;
    }
    u64 parsed=0;
    for(u32 i=begin;i<end;i++){
        if(value[i]<L'0'||value[i]>L'9')return false;
        parsed=parsed*10u+(u64)(value[i]-L'0');
        if(parsed>MAX_CONFIGURED_COST)return false;
    }
    out->custom=true;
    out->value=(u32)parsed;
    return true;
}

static s32 settingKeyIndex(const WCHAR* value,u32 begin,u32 end){
    for(u32 i=0;i<SETTING_COUNT;i++){
        if(equalsSpan(value,begin,end,SETTING_DEFINITIONS[i].key))return (s32)i;
    }
    if(equalsSpan(value,begin,end,L"Enabled"))return (s32)SETTING_COUNT;
    if(equalsSpan(value,begin,end,L"DebugLog"))return (s32)(SETTING_COUNT+1u);
    return -1;
}

static bool parseSettingsSection(const WCHAR* section,u32 length,Settings* settings){
    if(!section||!length||!settings)return false;
    memset(settings,0,sizeof(*settings));
    bool seen[(sizeof(SETTING_DEFINITIONS)/sizeof(SETTING_DEFINITIONS[0]))+2u];
    memset(seen,0,sizeof(seen));

    u32 position=0;
    while(position<length){
        u32 end=position;
        while(end<length&&section[end])end++;
        if(end==position||end>=length+1u)return false;

        u32 equals=end;
        for(u32 i=position;i<end;i++){
            if(section[i]==L'='){equals=i;break;}
        }
        if(equals==end)return false;
        u32 keyBegin=position,keyEnd=equals;
        u32 valueBegin=equals+1u,valueEnd=end;
        trimSpan(section,&keyBegin,&keyEnd);
        trimSpan(section,&valueBegin,&valueEnd);
        if(keyBegin==keyEnd||valueBegin==valueEnd)return false;

        s32 key=settingKeyIndex(section,keyBegin,keyEnd);
        if(key<0||seen[(u32)key])return false;
        seen[(u32)key]=true;
        if((u32)key<SETTING_COUNT){
            if(!parseCostSpan(section,valueBegin,valueEnd,&settings->costs[(u32)key]))return false;
        }else if((u32)key==SETTING_COUNT){
            if(!parseBoolSpan(section,valueBegin,valueEnd,&settings->enabled))return false;
        }else{
            if(!parseBoolSpan(section,valueBegin,valueEnd,&settings->debugLog))return false;
        }
        position=end+1u;
    }
    return seen[SETTING_COUNT];
}

static void modulePath(WCHAR* out,const WCHAR* name){
    if(!out||!name)return;
    out[0]=0;
    DWORD length=GetModuleFileNameW(g_self,out,520);
    if(!length||length>=520){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<length;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1u;
    DWORD j=0;
    while(name[j]&&slash+j+1u<520u){out[slash+j]=name[j];j++;}
    if(name[j]){out[0]=0;return;}
    out[slash+j]=0;
}

static bool isDs2Process(HMODULE game){
    WCHAR path[520];
    DWORD length=GetModuleFileNameW(game,path,520);
    if(!length||length>=520)return false;
    DWORD start=0;
    for(DWORD i=0;i<length;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1u;
    const WCHAR wanted[]=L"ds2.exe";
    DWORD j=0;
    while(wanted[j]){
        if(start+j>=length||lowerAscii(path[start+j])!=wanted[j])return false;
        j++;
    }
    return start+j==length;
}

static void logRaw(const char* text,DWORD length){
    if(g_log==INVALID_HANDLE_VALUE||!text||!length)return;
    DWORD written=0;
    WriteFile(g_log,text,length,&written,0);
    FlushFileBuffers(g_log);
}

static void openLog(){
    if(g_log!=INVALID_HANDLE_VALUE)return;
    WCHAR path[520];
    modulePath(path,L"ds2_chiral_bandwidth_costs.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Chiral Bandwidth Costs v" MOD_VERSION "\r\n"
            "Expected target: Steam PC 1.10.89.0\r\n";
        logRaw(header,(DWORD)(sizeof(header)-1u));
    }
}

static void closeLog(){
    if(g_log==INVALID_HANDLE_VALUE)return;
    FlushFileBuffers(g_log);
    CloseHandle(g_log);
    g_log=INVALID_HANDLE_VALUE;
}

static void logDebug(const char* text,DWORD length){
    if(!g_debugLog)return;
    openLog();
    logRaw(text,length);
}

static void logCritical(const char* text,DWORD length){
    openLog();
    logRaw(text,length);
}

static u32 appendAscii(char* out,u32 position,u32 capacity,const char* text){
    if(!out||!text||!capacity)return position;
    while(*text&&position+1u<capacity)out[position++]=*text++;
    return position;
}

static void logTargetLine(bool critical,const char* prefix,u32 targetIndex,const char* suffix){
    if(targetIndex>=PATCH_TARGET_COUNT)return;
    char line[384];
    u32 position=appendAscii(line,0u,(u32)sizeof(line),prefix);
    const WCHAR* key=TARGETS[targetIndex].label;
    for(u32 i=0;key[i]&&position+1u<(u32)sizeof(line);i++){
        WCHAR value=key[i];
        line[position++]=value>=32&&value<=126?(char)value:'?';
    }
    position=appendAscii(line,position,(u32)sizeof(line),suffix);
    line[position]=0;
    if(critical)logCritical(line,position);else logDebug(line,position);
}

static SettingsResult loadSettings(Settings* settings){
    if(!settings)return SETTINGS_INVALID;
    WCHAR path[520];
    modulePath(path,L"ds2_chiral_bandwidth_costs.ini");
    if(!path[0])return SETTINGS_INVALID;
    memset(g_iniSection,0,sizeof(g_iniSection));
    DWORD length=GetPrivateProfileSectionW(
        L"ChiralBandwidthCosts",g_iniSection,
        (DWORD)(sizeof(g_iniSection)/sizeof(g_iniSection[0])),path
    );
    if(!length||length>=(DWORD)(sizeof(g_iniSection)/sizeof(g_iniSection[0]))-2u||
       !parseSettingsSection(g_iniSection,(u32)length,settings)){
        const char message[]=
            "ERROR: [ChiralBandwidthCosts] is missing, truncated, or contains an invalid, duplicate, or unknown value; no writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        return SETTINGS_INVALID;
    }
    g_debugLog=settings->debugLog;
    return settings->enabled?SETTINGS_READY:SETTINGS_DISABLED;
}

static bool readBuildMetadata(const u8* base,u32* timestamp,u32* imageSize){
    if(!base||!timestamp||!imageSize)return false;
    if(readU16Raw(base)!=0x5A4Du)return false;
    u32 pe=readU32Raw(base+0x3C);
    if(pe<0x40u||pe>=0x2000u||readU32Raw(base+pe)!=0x00004550u)return false;
    if(readU16Raw(base+pe+4u)!=0x8664u)return false;
    if(readU16Raw(base+pe+20u)<0x00F0u||readU16Raw(base+pe+24u)!=0x020Bu)return false;
    *timestamp=readU32Raw(base+pe+8u);
    *imageSize=readU32Raw(base+pe+0x50u);
    return true;
}

static bool preflightCostCode(const u8* base,u32 imageSize){
    if(!base||
       RVA_CAPACITY_CHECK+sizeof(EXPECTED_CAPACITY_CHECK)>imageSize||
       RVA_ACCOUNTING_ADD+sizeof(EXPECTED_ACCOUNTING_ADD)>imageSize||
       RVA_ACCOUNTING_VALUE+sizeof(EXPECTED_ACCOUNTING_VALUE)>imageSize||
       RVA_ACCOUNTING_SUBTRACT+sizeof(EXPECTED_ACCOUNTING_SUBTRACT)>imageSize||
       RVA_UI_COST+sizeof(EXPECTED_UI_COST)>imageSize)return false;
    return bytesEqual(base+RVA_CAPACITY_CHECK,EXPECTED_CAPACITY_CHECK,(u32)sizeof(EXPECTED_CAPACITY_CHECK))&&
           bytesEqual(base+RVA_ACCOUNTING_ADD,EXPECTED_ACCOUNTING_ADD,(u32)sizeof(EXPECTED_ACCOUNTING_ADD))&&
           bytesEqual(base+RVA_ACCOUNTING_VALUE,EXPECTED_ACCOUNTING_VALUE,(u32)sizeof(EXPECTED_ACCOUNTING_VALUE))&&
           bytesEqual(base+RVA_ACCOUNTING_SUBTRACT,EXPECTED_ACCOUNTING_SUBTRACT,(u32)sizeof(EXPECTED_ACCOUNTING_SUBTRACT))&&
           bytesEqual(base+RVA_UI_COST,EXPECTED_UI_COST,(u32)sizeof(EXPECTED_UI_COST));
}

static bool rangeReadable(const void* pointer,u64 length){
    if(!pointer||!length)return false;
    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(pointer,&info,sizeof(info))<sizeof(info))return false;
    if(info.State!=MEM_COMMIT)return false;
    DWORD protection=info.Protect;
    if((protection&PAGE_GUARD)||((protection&0xFFu)==PAGE_NOACCESS))return false;
    u64 address=(u64)pointer;
    u64 base=(u64)info.BaseAddress;
    u64 end=base+info.RegionSize;
    return address>=base&&address+length>=address&&address+length<=end;
}

static bool readU32(const void* pointer,u32* value){
    if(!value||!rangeReadable(pointer,4u))return false;
    bool complete=false;
    __try{
        *value=*(volatile const u32*)pointer;
        complete=true;
    }__except(1){
        complete=false;
    }
    return complete;
}

static bool readU64(const void* pointer,u64* value){
    if(!value||!rangeReadable(pointer,8u))return false;
    bool complete=false;
    __try{
        *value=*(volatile const u64*)pointer;
        complete=true;
    }__except(1){
        complete=false;
    }
    return complete;
}

static bool compareExchangeU32(u8* pointer,u32 expected,u32 desired,u32* observed){
    if(!pointer||!observed||((u64)pointer&3u)!=0u||!rangeReadable(pointer,4u))return false;
    bool complete=false;
    __try{
        long previous=_InterlockedCompareExchange(
            (volatile long*)pointer,(long)desired,(long)expected
        );
        *observed=(u32)previous;
        complete=true;
    }__except(1){
        complete=false;
    }
    return complete;
}

static bool validateVtableEntries(const u8* base,u64 tableRva,const u64* functions,u32 count){
    if(!base||!functions||!rangeReadable(base+tableRva,(u64)count*8u))return false;
    for(u32 i=0;i<count;i++){
        u64 entry=0;
        if(!readU64(base+tableRva+(u64)i*8u,&entry)||entry!=(u64)(base+functions[i]))return false;
    }
    return true;
}

static bool validateAllVtables(const u8* base){
    if(!validateVtableEntries(base,RVA_MANAGER_VTABLE,MANAGER_VFUNCS,4u)||
       !validateVtableEntries(base,RVA_FACILITY_VTABLE,FACILITY_VFUNCS,4u))return false;
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        if(!validateVtableEntries(base,TARGETS[i].configVtableRva,TARGETS[i].vfuncs,4u))return false;
    }
    return true;
}

static bool validateObjectVtable(const void* object,const u8* base,u64 expectedRva){
    u64 vtable=0;
    return readU64(object,&vtable)&&vtable==(u64)(base+expectedRva);
}

static ResolveResult resolveManager(u8* base,u8** managerOut){
    if(!base||!managerOut)return RESOLVE_INVALID;
    *managerOut=0;
    u64 root=0,manager=0;
    if(!readU64(base+RVA_GLOBAL_ROOT,&root))return RESOLVE_INVALID;
    if(!root)return RESOLVE_NOT_READY;
    if(!readU64((u8*)root+OFF_ROOT_MANAGER,&manager))return RESOLVE_INVALID;
    if(!manager)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)manager,base,RVA_MANAGER_VTABLE))return RESOLVE_INVALID;
    *managerOut=(u8*)manager;
    return RESOLVE_READY;
}

static ResolveResult resolveTargetAtOffset(
    u8* base,u8* manager,u32 targetIndex,u32 managerOffset,u8** facilityOut,u8** configOut
){
    if(!base||!manager||targetIndex>=PATCH_TARGET_COUNT||!managerOffset||
       !facilityOut||!configOut)return RESOLVE_INVALID;
    *facilityOut=0;
    *configOut=0;
    u64 facility=0,config=0;
    if(!readU64(manager+managerOffset,&facility))return RESOLVE_INVALID;
    if(!facility)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)facility,base,RVA_FACILITY_VTABLE))return RESOLVE_INVALID;
    if(!readU64((u8*)facility+OFF_FACILITY_CONFIG,&config))return RESOLVE_INVALID;
    if(!config)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)config,base,TARGETS[targetIndex].configVtableRva))return RESOLVE_INVALID;
    if((((u64)config+OFF_QPID_COST)&3u)!=0u||
       !rangeReadable((u8*)config+OFF_QPID_COST,4u))return RESOLVE_INVALID;
    *facilityOut=(u8*)facility;
    *configOut=(u8*)config;
    return RESOLVE_READY;
}

static ResolveResult resolveTargetWithManager(
    u8* base,u8* manager,u32 targetIndex,u8** facilityOut,u8** configOut
){
    if(!base||!manager||targetIndex>=PATCH_TARGET_COUNT||!facilityOut||!configOut){
        return RESOLVE_INVALID;
    }
    *facilityOut=0;
    *configOut=0;

    ResolveResult primary=resolveTargetAtOffset(
        base,manager,targetIndex,TARGETS[targetIndex].managerOffset,facilityOut,configOut
    );
    if(primary!=RESOLVE_NOT_READY||!TARGETS[targetIndex].alternateManagerOffset)return primary;

    u8* alternateFacility=0;
    u8* alternateConfig=0;
    ResolveResult alternate=resolveTargetAtOffset(
        base,manager,targetIndex,TARGETS[targetIndex].alternateManagerOffset,
        &alternateFacility,&alternateConfig
    );
    if(alternate==RESOLVE_READY){
        *facilityOut=alternateFacility;
        *configOut=alternateConfig;
    }
    return alternate;
}

#ifdef DS2_TESTING
static ResolveResult resolveTarget(u8* base,u32 targetIndex,u8** facilityOut,u8** configOut){
    if(!base||targetIndex>=PATCH_TARGET_COUNT||!facilityOut||!configOut)return RESOLVE_INVALID;
    u8* manager=0;
    ResolveResult managerResult=resolveManager(base,&manager);
    if(managerResult!=RESOLVE_READY)return managerResult;
    return resolveTargetWithManager(base,manager,targetIndex,facilityOut,configOut);
}
#endif

static bool addProtectionPage(ProtectionPage* pages,u32* count,u32 capacity,u8* address,u32 pageSize){
    if(!pages||!count||!address||!pageSize)return false;
    u64 mask=(u64)pageSize-1u;
    u8* page=(u8*)((u64)address&~mask);
    for(u32 i=0;i<*count;i++)if(pages[i].address==page)return true;
    if(*count>=capacity)return false;

    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(page,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
    DWORD protection=info.Protect;
    if((protection&PAGE_GUARD)||((protection&0xFFu)==PAGE_NOACCESS))return false;
    u64 regionStart=(u64)info.BaseAddress;
    u64 regionEnd=regionStart+info.RegionSize;
    if((u64)page<regionStart||(u64)page+(u64)pageSize>regionEnd)return false;
    pages[*count].address=page;
    pages[*count].originalProtection=protection;
    pages[*count].restoreProtection=protection;
    pages[*count].protectionChanged=false;
    pages[*count].protectionConflict=false;
    (*count)++;
    return true;
}

static bool getPageSize(u32* pageSize){
    if(!pageSize)return false;
    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 value=systemInfo.PageSize;
    if(value<4096u||value>65536u||(value&(value-1u))!=0)return false;
    *pageSize=value;
    return true;
}

static bool makePagesWritable(ProtectionPage* pages,u32 count,u32 pageSize){
    if(!pages||!count||!pageSize)return false;
    for(u32 i=0;i<count;i++){
        ProtectionPage* page=&pages[i];
        if(page->protectionConflict)return false;
        MEMORY_BASIC_INFORMATION_X64 info;
        if(VirtualQuery(page->address,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
        DWORD expectedBefore=page->protectionChanged?page->restoreProtection:page->originalProtection;
        if(page->protectionChanged){
            if(info.Protect==PAGE_READWRITE)continue;
            if(info.Protect!=expectedBefore)return false;
        }else if(info.Protect!=expectedBefore){
            return false;
        }
#ifdef DS2_TESTING
        if(g_testProtectionRacePage==page->address){
            DWORD ignored=0;
            VirtualProtect(page->address,pageSize,g_testProtectionRaceValue,&ignored);
            g_testProtectionRacePage=0;
            g_testProtectionRaceValue=0;
        }
#endif
        DWORD previous=0;
        if(!VirtualProtect(page->address,pageSize,PAGE_READWRITE,&previous))return false;
        if(previous!=expectedBefore){
            page->restoreProtection=previous;
            page->protectionChanged=previous!=PAGE_READWRITE;
            page->protectionConflict=true;
            if(page->protectionChanged){
                DWORD undoPrevious=0;
                if(VirtualProtect(page->address,pageSize,previous,&undoPrevious)&&
                   undoPrevious==PAGE_READWRITE)page->protectionChanged=false;
            }
            return false;
        }
        page->restoreProtection=previous;
        page->protectionChanged=previous!=PAGE_READWRITE;
    }
    return true;
}

static bool restorePages(ProtectionPage* pages,u32 count,u32 pageSize){
    if(!pages||!count||!pageSize)return false;
    bool complete=true;
    for(u32 i=count;i>0;i--){
        ProtectionPage* page=&pages[i-1u];
        if(!page->protectionChanged)continue;
        MEMORY_BASIC_INFORMATION_X64 info;
        if(VirtualQuery(page->address,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT){
            complete=false;
            continue;
        }
        if(info.Protect==page->restoreProtection){
            page->protectionChanged=false;
            continue;
        }
        if(info.Protect!=PAGE_READWRITE){
            complete=false;
            continue;
        }
        DWORD previous=0;
        if(!VirtualProtect(page->address,pageSize,page->restoreProtection,&previous)||
           previous!=PAGE_READWRITE){
            complete=false;
        }else{
            page->protectionChanged=false;
        }
    }
    for(u32 i=0;i<count;i++){
        MEMORY_BASIC_INFORMATION_X64 info;
        if(VirtualQuery(pages[i].address,&info,sizeof(info))<sizeof(info)||
           info.State!=MEM_COMMIT||
           info.Protect!=(pages[i].protectionConflict?
               pages[i].restoreProtection:pages[i].originalProtection))complete=false;
        if(pages[i].protectionConflict)complete=false;
    }
#ifdef DS2_TESTING
    if(g_testFailRestoreCalls>0){
        g_testFailRestoreCalls--;
        complete=false;
    }
#endif
    return complete;
}

static ApplyResult rollbackTransaction(
    TransactionField* fields,u32 fieldCount,ProtectionPage* pages,u32 pageCount,u32 pageSize,
    ApplyResult cleanResult
){
    bool writable=makePagesWritable(pages,pageCount,pageSize);
    bool ownedValuesCleared=writable;
    bool conflict=cleanResult==APPLY_CONFLICT;
    if(writable){
#ifdef DS2_TESTING
        if(g_testRollbackRace){
            *(volatile u32*)g_testRollbackRace=g_testRollbackRaceValue;
            g_testRollbackRace=0;
            g_testRollbackRaceValue=0;
        }
#endif
        for(u32 i=fieldCount;i>0;i--){
            TransactionField* field=&fields[i-1u];
            if(!field->written)continue;
            u32 observed=0;
            if(!compareExchangeU32(field->field,field->after,field->before,&observed)){
                ownedValuesCleared=false;
            }else if(observed!=field->after&&observed!=field->before){
                conflict=true;
            }
        }
        for(u32 i=0;i<fieldCount;i++){
            if(!fields[i].written)continue;
            u32 current=0;
            if(!readU32(fields[i].field,&current)){
                ownedValuesCleared=false;
            }else if(current==fields[i].after&&fields[i].after!=fields[i].before){
                ownedValuesCleared=false;
            }else if(current!=fields[i].before){
                conflict=true;
            }
        }
    }
    bool protectionsRestored=restorePages(pages,pageCount,pageSize);
    if(!writable||!ownedValuesCleared||!protectionsRestored)return APPLY_CRITICAL;
    return conflict?APPLY_CONFLICT:cleanResult;
}

static ApplyResult applyBatch(
    u8* base,const Settings* settings,const Observation* observations,u32* failedTarget
){
    if(failedTarget)*failedTarget=PATCH_TARGET_COUNT;
    if(!base||!settings||!observations)return APPLY_FAILED;
    u8* phaseManager=0;
    ResolveResult managerResult=resolveManager(base,&phaseManager);
    if(managerResult==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(managerResult!=RESOLVE_READY)return APPLY_FAILED;
    TransactionField fields[sizeof(TARGETS)/sizeof(TARGETS[0])];
    u32 fieldCount=0;
    for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
        u32 settingIndex=TARGETS[i].settingIndex;
        if(!settings->costs[settingIndex].custom||
           settings->costs[settingIndex].value==SETTING_DEFINITIONS[settingIndex].nativeCost)continue;
        if(observations[i].stablePolls<2u){
            if(failedTarget)*failedTarget=i;
            return APPLY_NOT_READY;
        }
        u8* facility=0;
        u8* config=0;
        ResolveResult resolved=resolveTargetWithManager(base,phaseManager,i,&facility,&config);
        if(resolved!=RESOLVE_READY||observations[i].manager!=phaseManager||
           facility!=observations[i].facility||config!=observations[i].config){
            if(failedTarget)*failedTarget=i;
            return APPLY_NOT_READY;
        }
        u32 current=0;
        if(!readU32(config+OFF_QPID_COST,&current)){
            if(failedTarget)*failedTarget=i;
            return APPLY_FAILED;
        }
        u32 target=settings->costs[settingIndex].value;
        u32 nativeCost=SETTING_DEFINITIONS[settingIndex].nativeCost;
        if(current==0u&&nativeCost!=0u&&target!=0u){
            if(failedTarget)*failedTarget=i;
            return APPLY_NOT_READY;
        }
        if(current!=nativeCost&&current!=target){
            if(failedTarget)*failedTarget=i;
            return APPLY_CONFLICT;
        }
        u8* costField=config+OFF_QPID_COST;
        bool writeOwner=true;
        for(u32 previous=0;previous<fieldCount;previous++){
            if(fields[previous].field==costField){
                if(fields[previous].after!=target){
                    if(failedTarget)*failedTarget=i;
                    return APPLY_CONFLICT;
                }
                writeOwner=false;
                break;
            }
        }
        fields[fieldCount].targetIndex=i;
        fields[fieldCount].facility=facility;
        fields[fieldCount].config=config;
        fields[fieldCount].field=costField;
        fields[fieldCount].before=current;
        fields[fieldCount].after=target;
        fields[fieldCount].changed=current!=target;
        fields[fieldCount].writeOwner=writeOwner;
        fields[fieldCount].written=false;
        fieldCount++;
    }
    if(!fieldCount)return APPLY_NOT_READY;

    ProtectionPage pages[sizeof(TARGETS)/sizeof(TARGETS[0])];
    u32 pageCount=0,pageSize=0;
    if(!getPageSize(&pageSize))return APPLY_FAILED;
    for(u32 i=0;i<fieldCount;i++){
        if(fields[i].changed&&fields[i].writeOwner&&!addProtectionPage(
            pages,&pageCount,(u32)(sizeof(pages)/sizeof(pages[0])),fields[i].field,pageSize
        ))return APPLY_FAILED;
    }
    if(!pageCount)return APPLY_READY;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

#ifdef DS2_TESTING
    if(g_testManagerSlot){
        *g_testManagerSlot=g_testManagerReplacement;
        g_testManagerSlot=0;
        g_testManagerReplacement=0;
    }
#endif
    u8* commitManager=0;
    managerResult=resolveManager(base,&commitManager);
    if(managerResult!=RESOLVE_READY||commitManager!=phaseManager){
        return restorePages(pages,pageCount,pageSize)?APPLY_NOT_READY:APPLY_CRITICAL;
    }
    for(u32 i=0;i<fieldCount;i++){
        u8* facility=0;
        u8* config=0;
        ResolveResult resolved=resolveTargetWithManager(
            base,commitManager,fields[i].targetIndex,&facility,&config
        );
        u32 current=0;
        if(resolved!=RESOLVE_READY||facility!=fields[i].facility||config!=fields[i].config||
           !readU32(fields[i].field,&current)||current!=fields[i].before){
            if(failedTarget)*failedTarget=fields[i].targetIndex;
            return restorePages(pages,pageCount,pageSize)?APPLY_NOT_READY:APPLY_CRITICAL;
        }
    }
    u8* writeManager=0;
    managerResult=resolveManager(base,&writeManager);
    if(managerResult!=RESOLVE_READY||writeManager!=commitManager){
        return restorePages(pages,pageCount,pageSize)?APPLY_NOT_READY:APPLY_CRITICAL;
    }

    bool writesComplete=true;
    ApplyResult failureResult=APPLY_FAILED;
#ifdef DS2_TESTING
    s32 completedWrites=0;
    if(g_testFieldRace){
        *(volatile u32*)g_testFieldRace=g_testFieldRaceValue;
        g_testFieldRace=0;
        g_testFieldRaceValue=0;
    }
#endif
    for(u32 i=0;i<fieldCount;i++){
        if(!fields[i].changed||!fields[i].writeOwner)continue;
#ifdef DS2_TESTING
        if(g_testFailAfterWrites>=0&&completedWrites>=g_testFailAfterWrites){
            writesComplete=false;
            break;
        }
#endif
        u32 observed=0;
        if(!compareExchangeU32(fields[i].field,fields[i].before,fields[i].after,&observed)){
            writesComplete=false;
            break;
        }
        if(observed==fields[i].before){
            fields[i].written=true;
        }else if(observed!=fields[i].after){
            writesComplete=false;
            failureResult=APPLY_CONFLICT;
            if(failedTarget)*failedTarget=fields[i].targetIndex;
            break;
        }
#ifdef DS2_TESTING
        completedWrites++;
#endif
    }
    bool verified=writesComplete;
    if(writesComplete){
        for(u32 i=0;i<fieldCount;i++){
            u32 current=0;
            if(!readU32(fields[i].field,&current)){
                verified=false;
            }else if(current!=fields[i].after){
                verified=false;
                failureResult=APPLY_CONFLICT;
                if(failedTarget&&*failedTarget>=PATCH_TARGET_COUNT){
                    *failedTarget=fields[i].targetIndex;
                }
            }
        }
    }
    if(!verified){
        return rollbackTransaction(
            fields,fieldCount,pages,pageCount,pageSize,failureResult
        );
    }
    if(!restorePages(pages,pageCount,pageSize)){
        return rollbackTransaction(
            fields,fieldCount,pages,pageCount,pageSize,APPLY_FAILED
        );
    }
    return APPLY_READY;
}

static bool hasRequestedChanges(const Settings* settings){
    if(!settings)return false;
    for(u32 i=0;i<SETTING_COUNT;i++){
        if(settings->costs[i].custom&&
           settings->costs[i].value!=SETTING_DEFINITIONS[i].nativeCost)return true;
    }
    return false;
}

static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    Sleep(1000u);

    Settings settings;
    SettingsResult settingsResult=loadSettings(&settings);
    if(settingsResult!=SETTINGS_READY){
        closeLog();
        return 0;
    }
    if(g_debugLog)openLog();
    if(!hasRequestedChanges(&settings)){
        const char message[]="STATE: all configured values preserve native costs; no memory writes are needed.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }

    u8* base=(u8*)game;
    u32 timestamp=0,imageSize=0;
    if(!readBuildMetadata(base,&timestamp,&imageSize)||
       timestamp!=EXPECTED_TIMESTAMP||imageSize!=EXPECTED_IMAGE_SIZE){
        const char message[]="ERROR: unsupported DS2.exe build; no writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    if(!preflightCostCode(base,imageSize)){
        const char message[]="ERROR: chiral-bandwidth accounting signatures do not match; no writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    if(!validateAllVtables(base)){
        const char message[]="ERROR: expected construction RTTI/VTables do not match; no writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }

    const char waiting[]="Validated target; waiting for stable construction resources.\r\n";
    logDebug(waiting,(DWORD)(sizeof(waiting)-1u));
    Observation observations[sizeof(TARGETS)/sizeof(TARGETS[0])];
    bool active[sizeof(TARGETS)/sizeof(TARGETS[0])];
    bool invalidLogged[sizeof(TARGETS)/sizeof(TARGETS[0])];
    bool conflictLogged[sizeof(TARGETS)/sizeof(TARGETS[0])];
    bool managerInvalidLogged=false;
    bool failedLogged=false;
    memset(observations,0,sizeof(observations));
    memset(active,0,sizeof(active));
    memset(invalidLogged,0,sizeof(invalidLogged));
    memset(conflictLogged,0,sizeof(conflictLogged));

    for(;;){
        DWORD delay=1000u;
        bool anyStable=false;
        u8* pollManager=0;
        ResolveResult pollManagerResult=resolveManager(base,&pollManager);
        if(pollManagerResult!=RESOLVE_READY){
            delay=250u;
            failedLogged=false;
            for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
                observations[i].manager=0;
                observations[i].facility=0;
                observations[i].config=0;
                observations[i].stablePolls=0;
                active[i]=false;
            }
            if(pollManagerResult==RESOLVE_INVALID&&!managerInvalidLogged){
                const char message[]="WARNING: construction manager path is temporarily invalid; writes are paused.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                managerInvalidLogged=true;
            }
        }else{
            managerInvalidLogged=false;
            for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
                u32 settingIndex=TARGETS[i].settingIndex;
                if(!settings.costs[settingIndex].custom||
                   settings.costs[settingIndex].value==SETTING_DEFINITIONS[settingIndex].nativeCost)continue;
                u8* facility=0;
                u8* config=0;
                ResolveResult resolved=resolveTargetWithManager(base,pollManager,i,&facility,&config);
                if(resolved==RESOLVE_READY){
                    invalidLogged[i]=false;
                    if(observations[i].manager==pollManager&&observations[i].facility==facility&&
                       observations[i].config==config){
                        if(observations[i].stablePolls<2u)observations[i].stablePolls++;
                    }else{
                        observations[i].manager=pollManager;
                        observations[i].facility=facility;
                        observations[i].config=config;
                        observations[i].stablePolls=1u;
                        active[i]=false;
                        conflictLogged[i]=false;
                    }
                    if(observations[i].stablePolls>=2u)anyStable=true;
                    else delay=250u;
                }else{
                    observations[i].manager=0;
                    observations[i].facility=0;
                    observations[i].config=0;
                    observations[i].stablePolls=0;
                    active[i]=false;
                    delay=250u;
                    if(resolved==RESOLVE_INVALID&&!invalidLogged[i]){
                        logTargetLine(true,"WARNING: ",i," resource path is temporarily invalid; writes are paused.\r\n");
                        invalidLogged[i]=true;
                    }
                }
            }
        }
        if(!anyStable)failedLogged=false;

        if(anyStable){
            u32 failedTarget=PATCH_TARGET_COUNT;
            ApplyResult applied=applyBatch(base,&settings,observations,&failedTarget);
            if(applied!=APPLY_FAILED)failedLogged=false;
            if(applied==APPLY_READY){
                for(u32 i=0;i<PATCH_TARGET_COUNT;i++){
                    u32 settingIndex=TARGETS[i].settingIndex;
                    if(observations[i].stablePolls<2u||!settings.costs[settingIndex].custom||
                       settings.costs[settingIndex].value==SETTING_DEFINITIONS[settingIndex].nativeCost)continue;
                    u32 current=0;
                    if(readU32(observations[i].config+OFF_QPID_COST,&current)&&
                       current==settings.costs[settingIndex].value){
                        conflictLogged[i]=false;
                        if(!active[i]){
                            logTargetLine(false,"ACTIVE: ",i," uses the configured chiral-bandwidth cost.\r\n");
                            active[i]=true;
                        }
                    }
                }
            }else if(applied==APPLY_NOT_READY){
                delay=250u;
            }else if(applied==APPLY_CONFLICT){
                if(failedTarget<PATCH_TARGET_COUNT&&!conflictLogged[failedTarget]){
                    logTargetLine(true,"ERROR: ",failedTarget," has an unexpected cost; possible mod conflict. No batch writes were applied.\r\n");
                    conflictLogged[failedTarget]=true;
                }
            }else if(applied==APPLY_FAILED){
                if(!failedLogged){
                    const char message[]="ERROR: transactional cost write failed and was rolled back; it will be retried.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1u));
                    failedLogged=true;
                }
            }else{
                const char message[]="CRITICAL: cost rollback or memory-protection restore was incomplete; stop DS2.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                break;
            }
        }
        Sleep(delay);
    }
    closeLog();
    return 0;
}

static void start(){
    if(_InterlockedCompareExchange(&g_started,1,0)!=0)return;
    HANDLE thread=CreateThread(0,0,worker,0,0,0);
    if(thread){
        CloseHandle(thread);
    }else{
        _InterlockedCompareExchange(&g_started,0,1);
    }
}

extern "C" __declspec(dllexport) void InitializeASI(){
    start();
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){
        g_self=module;
        DisableThreadLibraryCalls(module);
#ifndef DS2_TESTING
        start();
#endif
    }
    return TRUE;
}
