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
extern "C" unsigned char _InterlockedCompareExchange128(
    volatile long long*,long long,long long,long long*
);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange128)

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u

static const u64 RVA_RANGE_GETTER=0x0124A8E0ull;
static const u64 RVA_CANDIDATE_RANGE=0x012CB0B8ull;
static const u64 RVA_FINAL_RANGE=0x0124B0F0ull;
static const u64 RVA_CONSTRUCTOR_DEFAULTS=0x012432B5ull;
static const u64 RVA_REALIZATION_USAGE=0x01268031ull;
static const u64 RVA_DISTANCE_OPEN_DEFAULT=0x012435A6ull;
static const u64 RVA_GLOBAL_ROOT=0x0623EAD8ull;
static const u64 RVA_UI_DISTANCE_USAGE=0x017CE1D5ull;
static const u64 RVA_FIXED_RIDEABLE_RANGE_CONTEXT=0x0124C94Dull;
static const u64 RVA_FIXED_RIDEABLE_RANGE_BLOCK=0x0124C950ull;
static const u64 RVA_RIDEABLE_FINAL_RANGE_GATE=0x0124CEE0ull;
static const u64 RVA_CONNECTION_OBSTACLE_RESULT=0x0124D1B5ull;
static const u64 RVA_UI_CACHE_ROOT=0x0623EBF8ull;
static const u64 RVA_TRAVEL_SPEED_CONTEXT=0x010389D1ull;
static const u64 RVA_TRAVEL_SPEED_LOAD=0x010389DCull;
static const u64 RVA_TRAVEL_SPEED_DISPLACEMENT=0x010389E0ull;
static const u64 RVA_NATIVE_TRAVEL_SPEED=0x03461ED0ull;

static const u64 RVA_MANAGER_VTABLE=0x03280940ull;
static const u64 RVA_FACILITY_VTABLE=0x032827C8ull;
static const u64 RVA_ZIPLINE_CONFIG_VTABLE=0x032863A8ull;
static const u64 RVA_ENTITY_RESOURCE_VTABLE=0x0311B4F0ull;
static const u64 RVA_IMAGINARY_COMPONENT_VTABLE=0x03281BC0ull;
static const u64 RVA_UI_RESIDENT_VTABLE=0x032BAE58ull;
static const u64 RVA_UI_GAME_PARAMS_VTABLE=0x032BAB60ull;

static const u64 MANAGER_VFUNCS[4]={0x012426B0ull,0x01274280ull,0x00109DE0ull,0x00109EB0ull};
static const u64 FACILITY_VFUNCS[4]={0x01242690ull,0x00291C50ull,0x00109DE0ull,0x00109EB0ull};
static const u64 ZIPLINE_CONFIG_VFUNCS[4]={0x01243560ull,0x01243720ull,0x00109DE0ull,0x00109EB0ull};
static const u64 ENTITY_RESOURCE_VFUNCS[4]={0x0015B2D0ull,0x00184EF0ull,0x00109DE0ull,0x00109EB0ull};
static const u64 IMAGINARY_COMPONENT_VFUNCS[4]={0x01243420ull,0x01243440ull,0x00109DE0ull,0x00109EB0ull};
static const u64 UI_RESIDENT_VFUNCS[4]={0x01482140ull,0x01489AD0ull,0x00109DE0ull,0x00109EB0ull};
static const u64 UI_GAME_PARAMS_VFUNCS[4]={0x01482130ull,0x01489200ull,0x00109DE0ull,0x00109EB0ull};

static const u32 OFF_ROOT_MANAGER_CONFIG=0x28u;
static const u32 OFF_MANAGER_ZIPLINE=0x68u;
static const u32 OFF_FACILITY_IMAGINARY_ENTITY=0x28u;
static const u32 OFF_FACILITY_CONFIG=0x30u;
static const u32 OFF_ENTITY_COMPONENT_ARRAY=0x48u;
static const u32 OFF_MAX_LEVEL=0x2Cu;
static const u32 OFF_DISTANCE_REALIZE=0x4D4u;
static const u32 OFF_DISTANCE_UNREALIZE=0x4D8u;
static const u32 OFF_PASSABLE_LEVEL1=0x784u;
static const u32 OFF_DISTANCE_OPEN=0x7A0u;
static const u32 OFF_FORCE_REALIZE_RADIUS=0x70u;
static const u32 OFF_UI_CACHE_RESIDENT=0xD0u;
static const u32 OFF_UI_RESIDENT_GAME_PARAMS=0x50u;
static const u32 OFF_UI_ZIPLINE_VISIBLE_DISTANCE=0x84u;
static const u32 VISIBLE_LEVELS=2u;
static const u32 SERIALIZED_MAX_LEVEL=3u;
static const u32 TOTAL_LEVEL_FIELDS=5u;

static const float NATIVE_LEVELS[TOTAL_LEVEL_FIELDS]={301.0f,351.0f,351.0f,351.0f,351.0f};
static const float CONSTRUCTOR_LEVELS[TOTAL_LEVEL_FIELDS]={300.0f,351.0f,351.0f,351.0f,351.0f};
static const float MIN_CONFIGURED_LEVELS[VISIBLE_LEVELS]={300.0f,350.0f};
static const float MAX_CONFIGURED_RANGE=1000.0f;
static const float NATIVE_TOLERANCE=1.0f;
static const float NATIVE_DISTANCE_REALIZE=400.0f;
static const float NATIVE_DISTANCE_UNREALIZE=416.0f;
static const float NATIVE_DISTANCE_OPEN=1000.0f;
static const float NATIVE_FORCE_REALIZE_RADIUS=400.0f;
static const float NATIVE_UI_ZIPLINE_VISIBLE_DISTANCE=400.0f;
static const float CONSTRUCTOR_DISTANCE_REALIZE=256.0f;
static const float CONSTRUCTOR_DISTANCE_UNREALIZE=260.0f;
static const float NATIVE_TRAVEL_SPEED=16.0f;
static const float MIN_SPEED_MULTIPLIER=0.25f;
static const float MAX_SPEED_MULTIPLIER=10.0f;

static const u8 EXPECTED_RANGE_GETTER[65]={
    0x48,0x8B,0x05,0xF1,0x41,0xFF,0x04,0x48,0x8B,0x40,0x28,0x48,0x85,0xC0,0x75,0x13,
    0x48,0x63,0x89,0xE0,0x01,0x00,0x00,0x33,0xC0,0xC5,0xFA,0x10,0x84,0x88,0x80,0x07,
    0x00,0x00,0xC3,0x48,0x8B,0x40,0x68,0x48,0x85,0xC0,0x74,0xE4,0x48,0x8B,0x40,0x30,
    0x48,0x63,0x89,0xE0,0x01,0x00,0x00,0xC5,0xFA,0x10,0x84,0x88,0x80,0x07,0x00,0x00,0xC3
};

static const u8 EXPECTED_CANDIDATE_RANGE[62]={
    0x48,0x8B,0x05,0x19,0x3A,0xF7,0x04,0x48,0x8B,0x48,0x28,0x48,0x85,0xC9,0x75,0x06,
    0x33,0xC0,0x8B,0xC8,0xEB,0x13,0x48,0x8B,0x49,0x68,0x48,0x85,0xC9,0x75,0x06,0x33,
    0xC0,0x8B,0xC8,0xEB,0x04,0x48,0x8B,0x49,0x30,0x48,0x8B,0x84,0x24,0x28,0x02,0x00,
    0x00,0x48,0x63,0x40,0x18,0xC5,0x4A,0x5F,0x84,0x81,0x80,0x07,0x00,0x00
};

static const u8 EXPECTED_FINAL_RANGE[45]={
    0x48,0x8B,0x4D,0x28,0x48,0x85,0xC9,0x75,0x05,0x48,0x8B,0xCE,0xEB,0x12,0x48,0x8B,
    0x49,0x68,0x48,0x85,0xC9,0x75,0x05,0x48,0x8B,0xCE,0xEB,0x04,0x48,0x8B,0x49,0x30,
    0x49,0x63,0x47,0x18,0xC5,0xC2,0x5F,0xBC,0x81,0x80,0x07,0x00,0x00
};

static const u8 EXPECTED_CONSTRUCTOR_DEFAULTS[50]={
    0xC7,0x83,0x84,0x07,0x00,0x00,0x00,0x00,0x96,0x43,0xC7,0x83,0x88,0x07,0x00,0x00,
    0x00,0x80,0xAF,0x43,0xC7,0x83,0x8C,0x07,0x00,0x00,0x00,0x80,0xAF,0x43,0xC7,0x83,
    0x90,0x07,0x00,0x00,0x00,0x80,0xAF,0x43,0xC7,0x83,0x94,0x07,0x00,0x00,0x00,0x80,
    0xAF,0x43
};

static const u8 EXPECTED_REALIZATION_USAGE[67]={
    0x48,0x8B,0x40,0x30,0xC5,0xFA,0x10,0xB0,0xD4,0x04,0x00,0x00,0x80,0xFA,0x0E,0x74,
    0x0A,0x48,0x8B,0x84,0xD1,0x38,0x01,0x00,0x00,0xEB,0x0A,0x41,0x0F,0xB6,0x46,0x28,
    0x48,0x8B,0x44,0xC1,0x20,0x48,0x85,0xC0,0x75,0x05,0x49,0x8B,0xC0,0xEB,0x04,0x48,
    0x8B,0x40,0x30,0x41,0x80,0xBD,0x50,0x07,0x00,0x00,0x00,0xC5,0xFA,0x10,0xB8,0xD8,
    0x04,0x00,0x00
};

static const u8 EXPECTED_DISTANCE_OPEN_DEFAULT[10]={
    0xC7,0x83,0xA0,0x07,0x00,0x00,0x00,0x00,0x7A,0x44
};

static const u8 EXPECTED_UI_DISTANCE_USAGE[61]={
    0x48,0x8B,0x1D,0x1C,0x0A,0xA7,0x04,0x4C,0x39,0xA3,0xD0,0x00,0x00,0x00,0x75,0x1A,
    0xE8,0xD6,0xCE,0xEA,0xFE,0x48,0x8B,0x88,0x58,0x02,0x00,0x00,0x48,0x8B,0x05,0x00,
    0x0A,0xA7,0x04,0x48,0x89,0x88,0xD0,0x00,0x00,0x00,0x48,0x8B,0x83,0xD0,0x00,0x00,
    0x00,0x48,0x8B,0x48,0x50,0xC5,0xCA,0x5F,0xB1,0x84,0x00,0x00,0x00
};

static const u8 EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT[27]={
    0x45,0x84,0xF6,0x75,0x24,0xC5,0xF8,0x2F,0x3D,0xAE,0x5A,0x21,0x02,0x0F,0x87,0x7C,
    0x05,0x00,0x00,0xC5,0xFA,0x10,0x85,0x28,0x01,0x00,0x00
};

static const u8 EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK[16]={
    0x75,0x24,0xC5,0xF8,0x2F,0x3D,0xAE,0x5A,0x21,0x02,0x0F,0x87,0x7C,0x05,0x00,0x00
};

static const u8 PATCHED_FIXED_RIDEABLE_RANGE_BLOCK[16]={
    0x75,0x24,0xC4,0xC1,0x78,0x2F,0xF9,0x90,0x90,0x90,0x0F,0x87,0x7C,0x05,0x00,0x00
};

static const u8 EXPECTED_RIDEABLE_FINAL_RANGE_GATE[17]={
    0x45,0x84,0xF6,0x75,0x0A,0xC4,0xC1,0x78,0x2F,0xF9,0x0F,0x96,0xC0,0xEB,0x02,0xB0,
    0x01
};

static const u8 EXPECTED_CONNECTION_OBSTACLE_RESULT[17]={
    0x85,0xC0,0x0F,0x94,0xC0,0xEB,0x02,0x32,0xC0,0x4C,0x8D,0x9C,0x24,0x58,0x01,0x00,0x00
};

static const u8 EXPECTED_TRAVEL_SPEED_CONTEXT[104]={
    0x48,0x8B,0x49,0x30,0x48,0x8B,0x96,0xD8,0x00,0x00,0x00,0xC5,0xFA,0x10,0x0D,0xEC,
    0x94,0x42,0x02,0x80,0x7A,0x08,0x00,0x74,0x13,0x80,0xBA,0x87,0x01,0x00,0x00,0x00,
    0x74,0x0A,0xC5,0xF2,0x59,0x89,0xDC,0x07,0x00,0x00,0xEB,0x28,0x49,0x83,0xB8,0x20,
    0x02,0x00,0x00,0xFF,0x74,0x10,0x48,0x83,0xF8,0xFF,0x74,0x10,0xC5,0xF2,0x59,0x89,
    0xD8,0x07,0x00,0x00,0xEB,0x0E,0x48,0x83,0xF8,0xFF,0x74,0x08,0xC5,0xF2,0x59,0x89,
    0xD4,0x07,0x00,0x00,0xC5,0xF2,0x59,0xC6,0xC5,0xFA,0x58,0x8E,0x90,0x01,0x00,0x00,
    0xC5,0xFA,0x11,0x8E,0x90,0x01,0x00,0x00
};

static const u32 EXPECTED_TRAVEL_SPEED_DISPLACEMENT=0x024294ECu;

static const u8 PATCHED_CONNECTION_OBSTACLE_RESULT=0x31;

struct Settings{
    float displayedRange[VISIBLE_LEVELS];
    u32 targetBits[TOTAL_LEVEL_FIELDS];
    u32 distanceRealizeBits;
    u32 distanceUnrealizeBits;
    u32 distanceOpenBits;
    u32 forceRealizeRadiusBits;
    u32 uiVisibleDistanceBits;
    float speedMultiplier;
    u32 travelSpeedBits;
    bool ignoreConnectionObstacles;
};

struct RawArray{
    u32 count;
    u32 capacity;
    u64 entries;
};

struct ProtectionPage{
    u8* address;
    DWORD originalProtection;
};

enum SettingsResult{SETTINGS_INVALID,SETTINGS_DISABLED,SETTINGS_READY};
enum ResolveResult{RESOLVE_NOT_READY,RESOLVE_INVALID,RESOLVE_READY};
enum ApplyResult{APPLY_NOT_READY,APPLY_CONFLICT,APPLY_FAILED,APPLY_CRITICAL,APPLY_READY};

static HMODULE g_self=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static bool g_debugLog=false;
static volatile long g_started=0;
static u8* g_travelSpeedCell=0;

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

static u64 readU64Raw(const u8* source){
    return (u64)readU32Raw(source)|((u64)readU32Raw(source+4)<<32);
}

static u32 floatBits(float value){
    union FloatBits{float value;u32 bits;};
    FloatBits converted;
    converted.value=value;
    return converted.bits;
}

static WCHAR lowerAscii(WCHAR value){
    return value>=L'A'&&value<=L'Z'?(WCHAR)(value+(L'a'-L'A')):value;
}

static void modulePath(WCHAR* out,const WCHAR* name){
    if(!out||!name)return;
    out[0]=0;
    DWORD length=GetModuleFileNameW(g_self,out,520);
    if(!length||length>=520){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<length;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1;
    DWORD j=0;
    while(name[j]&&slash+j+1<520){out[slash+j]=name[j];j++;}
    if(name[j]){out[0]=0;return;}
    out[slash+j]=0;
}

static bool isDs2Process(HMODULE game){
    WCHAR path[520];
    DWORD length=GetModuleFileNameW(game,path,520);
    if(!length||length>=520)return false;
    DWORD start=0;
    for(DWORD i=0;i<length;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1;
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
    modulePath(path,L"ds2_zipline_range.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Zipline Range and Speed v" MOD_VERSION "\r\n"
            "Expected target: Steam PC 1.10.89.0\r\n";
        logRaw(header,(DWORD)(sizeof(header)-1));
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

static bool equalsToken(const WCHAR* value,u32 begin,u32 end,const WCHAR* token){
    u32 length=end-begin;
    u32 i=0;
    while(token[i])i++;
    if(i!=length)return false;
    for(i=0;i<length;i++)if(lowerAscii(value[begin+i])!=lowerAscii(token[i]))return false;
    return true;
}

static void trimmedBounds(const WCHAR* value,u32* begin,u32* end){
    u32 length=0;
    while(value[length])length++;
    u32 first=0;
    while(first<length&&(value[first]==L' '||value[first]==L'\t'))first++;
    while(length>first&&(value[length-1]==L' '||value[length-1]==L'\t'))length--;
    *begin=first;
    *end=length;
}

static bool readIniValue(const WCHAR* key,WCHAR* out,DWORD capacity){
    if(!key||!out||capacity<3)return false;
    WCHAR path[520];
    modulePath(path,L"ds2_zipline_range.ini");
    if(!path[0])return false;
    const WCHAR missing[2]={1,0};
    out[0]=0;
    DWORD length=GetPrivateProfileStringW(L"ZiplineRange",key,missing,out,capacity,path);
    if(length==1&&out[0]==1&&out[1]==0)return false;
    if(length>=capacity-1)return false;
    return true;
}

static bool parseBool(const WCHAR* value,bool* out){
    if(!value||!out)return false;
    u32 begin=0,end=0;
    trimmedBounds(value,&begin,&end);
    if(equalsToken(value,begin,end,L"1")||equalsToken(value,begin,end,L"true")||
       equalsToken(value,begin,end,L"yes")||equalsToken(value,begin,end,L"on")){
        *out=true;
        return true;
    }
    if(equalsToken(value,begin,end,L"0")||equalsToken(value,begin,end,L"false")||
       equalsToken(value,begin,end,L"no")||equalsToken(value,begin,end,L"off")){
        *out=false;
        return true;
    }
    return false;
}

static bool parseDecimal(const WCHAR* value,float minimum,float maximum,float* out){
    if(!value||!out)return false;
    u32 begin=0,end=0;
    trimmedBounds(value,&begin,&end);
    if(begin==end)return false;

    u64 whole=0;
    u32 wholeDigits=0;
    u32 pos=begin;
    while(pos<end&&value[pos]>=L'0'&&value[pos]<=L'9'){
        whole=whole*10u+(u64)(value[pos]-L'0');
        wholeDigits++;
        if(whole>10000u)return false;
        pos++;
    }
    if(!wholeDigits)return false;

    u32 fraction=0;
    u32 scale=1;
    if(pos<end&&value[pos]==L'.'){
        pos++;
        u32 fractionDigits=0;
        while(pos<end&&value[pos]>=L'0'&&value[pos]<=L'9'){
            if(fractionDigits>=3)return false;
            fraction=fraction*10u+(u32)(value[pos]-L'0');
            scale*=10u;
            fractionDigits++;
            pos++;
        }
        if(!fractionDigits)return false;
    }
    if(pos!=end)return false;

    double parsed=(double)whole+(double)fraction/(double)scale;
    if(parsed<(double)minimum||parsed>(double)maximum)return false;
    *out=(float)parsed;
    return true;
}

static bool parseRange(const WCHAR* value,float minimum,float* out){
    return parseDecimal(value,minimum,MAX_CONFIGURED_RANGE,out);
}

static bool deriveRangeSettings(Settings* settings){
    if(!settings||settings->displayedRange[0]>settings->displayedRange[1])return false;
    for(u32 i=0;i<VISIBLE_LEVELS;i++){
        settings->targetBits[i]=floatBits(settings->displayedRange[i]+NATIVE_TOLERANCE);
    }
    for(u32 i=VISIBLE_LEVELS;i<TOTAL_LEVEL_FIELDS;i++){
        settings->targetBits[i]=settings->targetBits[VISIBLE_LEVELS-1u];
    }
    float maximum=settings->displayedRange[VISIBLE_LEVELS-1u];
    float distanceRealize=maximum+50.0f;
    float distanceUnrealize=maximum+66.0f;
    float distanceOpen=maximum+50.0f;
    if(distanceRealize<NATIVE_DISTANCE_REALIZE)distanceRealize=NATIVE_DISTANCE_REALIZE;
    if(distanceUnrealize<NATIVE_DISTANCE_UNREALIZE)distanceUnrealize=NATIVE_DISTANCE_UNREALIZE;
    if(distanceOpen<NATIVE_DISTANCE_OPEN)distanceOpen=NATIVE_DISTANCE_OPEN;
    settings->distanceRealizeBits=floatBits(distanceRealize);
    settings->distanceUnrealizeBits=floatBits(distanceUnrealize);
    settings->distanceOpenBits=floatBits(distanceOpen);
    settings->forceRealizeRadiusBits=floatBits(distanceRealize);
    settings->uiVisibleDistanceBits=floatBits(distanceRealize);
    return true;
}

static SettingsResult loadSettings(Settings* settings){
    if(!settings)return SETTINGS_INVALID;
    WCHAR value[128];
    bool enabled=false;
    if(!readIniValue(L"Enabled",value,128)||!parseBool(value,&enabled)){
        const char message[]="ERROR: missing or invalid [ZiplineRange] Enabled value; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        return SETTINGS_INVALID;
    }
    if(!enabled)return SETTINGS_DISABLED;

    if(readIniValue(L"DebugLog",value,128)){
        if(!parseBool(value,&g_debugLog)){
            const char message[]="ERROR: invalid DebugLog value; no patch applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1));
            return SETTINGS_INVALID;
        }
    }
    settings->ignoreConnectionObstacles=false;
    if(readIniValue(L"IgnoreConnectionObstacles",value,128)){
        if(!parseBool(value,&settings->ignoreConnectionObstacles)){
            const char message[]="ERROR: invalid IgnoreConnectionObstacles value; no patch applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1));
            return SETTINGS_INVALID;
        }
    }
    if(!readIniValue(L"SpeedMultiplier",value,128)||
       !parseDecimal(value,MIN_SPEED_MULTIPLIER,MAX_SPEED_MULTIPLIER,&settings->speedMultiplier)){
        const char message[]="ERROR: SpeedMultiplier must be a decimal value from 0.25 to 10.0; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        return SETTINGS_INVALID;
    }
    settings->travelSpeedBits=floatBits(NATIVE_TRAVEL_SPEED*settings->speedMultiplier);
    if(settings->speedMultiplier>4.0f){
        const char message[]="WARNING: SpeedMultiplier above 4.0 is experimental; reduce it if streaming, camera, audio, or endpoint transitions misbehave.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
    }

    const WCHAR* keys[VISIBLE_LEVELS]={L"Level1RangeMeters",L"Level2RangeMeters"};
    for(u32 i=0;i<VISIBLE_LEVELS;i++){
        if(!readIniValue(keys[i],value,128)||!parseRange(value,MIN_CONFIGURED_LEVELS[i],&settings->displayedRange[i])){
            if(i==0){
                const char message[]="ERROR: Level1RangeMeters must be a decimal value from 300 to 1000.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1));
            }else{
                const char message[]="ERROR: Level2RangeMeters must be a decimal value from 350 to 1000.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1));
            }
            return SETTINGS_INVALID;
        }
    }
    if(!deriveRangeSettings(settings)){
        const char message[]="ERROR: configured ranges must not decrease between levels; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        return SETTINGS_INVALID;
    }

    const char message[]="INI valid: preserving DS2's native +1 m placement tolerance; travel-speed multiplier validated.\r\n";
    logDebug(message,(DWORD)(sizeof(message)-1));
    return SETTINGS_READY;
}

static bool readBuildMetadata(const u8* base,u32* timestamp,u32* imageSize){
    if(!base||!timestamp||!imageSize)return false;
    if(readU16Raw(base)!=0x5A4Du)return false;
    u32 pe=readU32Raw(base+0x3C);
    if(pe<0x40||pe>=0x2000||readU32Raw(base+pe)!=0x00004550u)return false;
    if(readU16Raw(base+pe+4)!=0x8664u)return false;
    if(readU16Raw(base+pe+20)<0x00F0u||readU16Raw(base+pe+24)!=0x020Bu)return false;
    *timestamp=readU32Raw(base+pe+8);
    *imageSize=readU32Raw(base+pe+0x50);
    return true;
}

static bool preflightCode(
    const u8* base,
    u32 imageSize,
    bool requireObstaclePatchSite,
    bool requireTravelSpeedPatchSite
){
    if(RVA_RANGE_GETTER+sizeof(EXPECTED_RANGE_GETTER)>imageSize||
       RVA_CANDIDATE_RANGE+sizeof(EXPECTED_CANDIDATE_RANGE)>imageSize||
       RVA_FINAL_RANGE+sizeof(EXPECTED_FINAL_RANGE)>imageSize||
       RVA_CONSTRUCTOR_DEFAULTS+sizeof(EXPECTED_CONSTRUCTOR_DEFAULTS)>imageSize||
       RVA_REALIZATION_USAGE+sizeof(EXPECTED_REALIZATION_USAGE)>imageSize||
       RVA_DISTANCE_OPEN_DEFAULT+sizeof(EXPECTED_DISTANCE_OPEN_DEFAULT)>imageSize||
       RVA_UI_DISTANCE_USAGE+sizeof(EXPECTED_UI_DISTANCE_USAGE)>imageSize||
       (requireTravelSpeedPatchSite&&
        (RVA_TRAVEL_SPEED_CONTEXT+sizeof(EXPECTED_TRAVEL_SPEED_CONTEXT)>imageSize||
         RVA_NATIVE_TRAVEL_SPEED+4u>imageSize))||
       RVA_FIXED_RIDEABLE_RANGE_CONTEXT+sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT)>imageSize||
       RVA_RIDEABLE_FINAL_RANGE_GATE+sizeof(EXPECTED_RIDEABLE_FINAL_RANGE_GATE)>imageSize||
       (requireObstaclePatchSite&&
        RVA_CONNECTION_OBSTACLE_RESULT+sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)>imageSize))return false;
    return bytesEqual(base+RVA_RANGE_GETTER,EXPECTED_RANGE_GETTER,(u32)sizeof(EXPECTED_RANGE_GETTER))&&
           bytesEqual(base+RVA_CANDIDATE_RANGE,EXPECTED_CANDIDATE_RANGE,(u32)sizeof(EXPECTED_CANDIDATE_RANGE))&&
           bytesEqual(base+RVA_FINAL_RANGE,EXPECTED_FINAL_RANGE,(u32)sizeof(EXPECTED_FINAL_RANGE))&&
           bytesEqual(base+RVA_CONSTRUCTOR_DEFAULTS,EXPECTED_CONSTRUCTOR_DEFAULTS,(u32)sizeof(EXPECTED_CONSTRUCTOR_DEFAULTS))&&
           bytesEqual(base+RVA_REALIZATION_USAGE,EXPECTED_REALIZATION_USAGE,(u32)sizeof(EXPECTED_REALIZATION_USAGE))&&
           bytesEqual(base+RVA_DISTANCE_OPEN_DEFAULT,EXPECTED_DISTANCE_OPEN_DEFAULT,(u32)sizeof(EXPECTED_DISTANCE_OPEN_DEFAULT))&&
           bytesEqual(base+RVA_UI_DISTANCE_USAGE,EXPECTED_UI_DISTANCE_USAGE,(u32)sizeof(EXPECTED_UI_DISTANCE_USAGE))&&
           (!requireTravelSpeedPatchSite||(
               bytesEqual(
                   base+RVA_TRAVEL_SPEED_CONTEXT,
                   EXPECTED_TRAVEL_SPEED_CONTEXT,
                   (u32)sizeof(EXPECTED_TRAVEL_SPEED_CONTEXT)
               )&&
               readU32Raw(base+RVA_NATIVE_TRAVEL_SPEED)==floatBits(NATIVE_TRAVEL_SPEED)
           ))&&
           bytesEqual(
               base+RVA_FIXED_RIDEABLE_RANGE_CONTEXT,
               EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT,
               (u32)sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT)
           )&&
           bytesEqual(
               base+RVA_RIDEABLE_FINAL_RANGE_GATE,
               EXPECTED_RIDEABLE_FINAL_RANGE_GATE,
               (u32)sizeof(EXPECTED_RIDEABLE_FINAL_RANGE_GATE)
           )&&
           (!requireObstaclePatchSite||bytesEqual(
               base+RVA_CONNECTION_OBSTACLE_RESULT,
               EXPECTED_CONNECTION_OBSTACLE_RESULT,
               (u32)sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)
           ));
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
    if(!value||!rangeReadable(pointer,4))return false;
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
    if(!value||!rangeReadable(pointer,8))return false;
    bool complete=false;
    __try{
        *value=*(volatile const u64*)pointer;
        complete=true;
    }__except(1){
        complete=false;
    }
    return complete;
}

static bool writeU32(u8* pointer,u32 value){
    if(!rangeReadable(pointer,4))return false;
    bool complete=false;
    __try{
        *(volatile u32*)pointer=value;
        complete=true;
    }__except(1){
        complete=false;
    }
    return complete;
}

static bool writeU8(u8* pointer,u8 value){
    if(!pointer||!rangeReadable(pointer,1))return false;
    bool complete=false;
    __try{
        *(volatile u8*)pointer=value;
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
        if(!readU64(base+tableRva+(u64)i*8u,&entry))return false;
        if(entry!=(u64)(base+functions[i]))return false;
    }
    return true;
}

static bool validateObjectVtable(const void* object,const u8* base,u64 expectedRva){
    u64 vtable=0;
    return readU64(object,&vtable)&&vtable==(u64)(base+expectedRva);
}

static ResolveResult resolveZiplineConfig(u8* base,u8** config,u8** facilityOut){
    if(!base||!config||!facilityOut)return RESOLVE_INVALID;
    *config=0;
    *facilityOut=0;
    u64 root=0,manager=0,facility=0,zipline=0;
    if(!readU64(base+RVA_GLOBAL_ROOT,&root))return RESOLVE_INVALID;
    if(!root)return RESOLVE_NOT_READY;
    if(!readU64((u8*)root+OFF_ROOT_MANAGER_CONFIG,&manager))return RESOLVE_INVALID;
    if(!manager)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)manager,base,RVA_MANAGER_VTABLE))return RESOLVE_INVALID;
    if(!readU64((u8*)manager+OFF_MANAGER_ZIPLINE,&facility))return RESOLVE_INVALID;
    if(!facility)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)facility,base,RVA_FACILITY_VTABLE))return RESOLVE_INVALID;
    if(!readU64((u8*)facility+OFF_FACILITY_CONFIG,&zipline))return RESOLVE_INVALID;
    if(!zipline)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)zipline,base,RVA_ZIPLINE_CONFIG_VTABLE))return RESOLVE_INVALID;
    if(!rangeReadable((u8*)zipline+OFF_MAX_LEVEL,4)||
       !rangeReadable((u8*)zipline+OFF_PASSABLE_LEVEL1,TOTAL_LEVEL_FIELDS*4u))return RESOLVE_INVALID;

    *config=(u8*)zipline;
    *facilityOut=(u8*)facility;
    return RESOLVE_READY;
}

static ResolveResult resolveImaginaryComponent(u8* base,u8* facility,u8** imaginaryComponent){
    if(!base||!facility||!imaginaryComponent)return RESOLVE_INVALID;
    *imaginaryComponent=0;
    u64 imaginaryEntity=0;
    if(!readU64((u8*)facility+OFF_FACILITY_IMAGINARY_ENTITY,&imaginaryEntity))return RESOLVE_INVALID;
    if(!imaginaryEntity)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)imaginaryEntity,base,RVA_ENTITY_RESOURCE_VTABLE))return RESOLVE_INVALID;
    if(!rangeReadable((u8*)imaginaryEntity+OFF_ENTITY_COMPONENT_ARRAY,sizeof(RawArray)))return RESOLVE_INVALID;
    RawArray components;
    if(!readU32((u8*)imaginaryEntity+OFF_ENTITY_COMPONENT_ARRAY,&components.count)||
       !readU32((u8*)imaginaryEntity+OFF_ENTITY_COMPONENT_ARRAY+4u,&components.capacity)||
       !readU64((u8*)imaginaryEntity+OFF_ENTITY_COMPONENT_ARRAY+8u,&components.entries))return RESOLVE_INVALID;
    if(!components.count||!components.entries)return RESOLVE_NOT_READY;
    if(components.count>64u||components.capacity<components.count||components.capacity>256u)return RESOLVE_INVALID;
    if(!rangeReadable((void*)components.entries,(u64)components.count*8u))return RESOLVE_INVALID;

    u8* found=0;
    for(u32 i=0;i<components.count;i++){
        u64 component=0;
        if(!readU64((u8*)components.entries+(u64)i*8u,&component))return RESOLVE_INVALID;
        if(!component)continue;
        u64 vtable=0;
        if(!readU64((void*)component,&vtable))return RESOLVE_INVALID;
        if(vtable==(u64)(base+RVA_IMAGINARY_COMPONENT_VTABLE)){
            if(found)return RESOLVE_INVALID;
            found=(u8*)component;
        }
    }
    if(!found)return RESOLVE_NOT_READY;
    if(!rangeReadable(found+OFF_FORCE_REALIZE_RADIUS,4))return RESOLVE_INVALID;
    *imaginaryComponent=found;
    return RESOLVE_READY;
}

static ResolveResult resolveCanonicalImaginaryComponent(u8* base,u8** facilityOut,u8** imaginaryComponent){
    if(!base||!facilityOut||!imaginaryComponent)return RESOLVE_INVALID;
    *facilityOut=0;
    *imaginaryComponent=0;
    u8* config=0;
    u8* facility=0;
    ResolveResult configResult=resolveZiplineConfig(base,&config,&facility);
    if(configResult!=RESOLVE_READY)return configResult;
    ResolveResult componentResult=resolveImaginaryComponent(base,facility,imaginaryComponent);
    if(componentResult!=RESOLVE_READY)return componentResult;
    *facilityOut=facility;
    return RESOLVE_READY;
}

static ResolveResult resolveUiGameDesign(u8* base,u8** gameParameters){
    if(!base||!gameParameters)return RESOLVE_INVALID;
    *gameParameters=0;
    u64 cache=0,resident=0,parameters=0;
    if(!readU64(base+RVA_UI_CACHE_ROOT,&cache))return RESOLVE_INVALID;
    if(!cache)return RESOLVE_NOT_READY;
    if(!readU64((u8*)cache+OFF_UI_CACHE_RESIDENT,&resident))return RESOLVE_INVALID;
    if(!resident)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)resident,base,RVA_UI_RESIDENT_VTABLE))return RESOLVE_INVALID;
    if(!readU64((u8*)resident+OFF_UI_RESIDENT_GAME_PARAMS,&parameters))return RESOLVE_INVALID;
    if(!parameters)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)parameters,base,RVA_UI_GAME_PARAMS_VTABLE))return RESOLVE_INVALID;
    if(!rangeReadable((u8*)parameters+OFF_UI_ZIPLINE_VISIBLE_DISTANCE,4u))return RESOLVE_INVALID;
    *gameParameters=(u8*)parameters;
    return RESOLVE_READY;
}

static bool tupleMatches(const u32* values,const float* expected,u32 count){
    for(u32 i=0;i<count;i++)if(values[i]!=floatBits(expected[i]))return false;
    return true;
}

static bool verifyConfigOriginals(u8* config,const u32* levels,u32 distanceRealize,u32 distanceUnrealize,u32 distanceOpen){
    if(!config||!levels)return false;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        u32 value=0;
        if(!readU32(config+OFF_PASSABLE_LEVEL1+i*4u,&value)||value!=levels[i])return false;
    }
    u32 currentRealize=0,currentUnrealize=0,currentOpen=0;
    return readU32(config+OFF_DISTANCE_REALIZE,&currentRealize)&&currentRealize==distanceRealize&&
           readU32(config+OFF_DISTANCE_UNREALIZE,&currentUnrealize)&&currentUnrealize==distanceUnrealize&&
           readU32(config+OFF_DISTANCE_OPEN,&currentOpen)&&currentOpen==distanceOpen;
}

static bool isWritableProtection(DWORD protection){
    DWORD base=protection&0xFFu;
    return base==PAGE_READWRITE||base==PAGE_WRITECOPY||
           base==PAGE_EXECUTE_READWRITE||base==PAGE_EXECUTE_WRITECOPY;
}

static bool addProtectionPage(ProtectionPage* pages,u32* count,u32 capacity,u8* address,u32 pageSize){
    if(!pages||!count||!address||!pageSize||*count>=capacity)return false;
    u64 mask=(u64)pageSize-1u;
    u8* page=(u8*)((u64)address&~mask);
    for(u32 i=0;i<*count;i++)if(pages[i].address==page)return true;

    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(page,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
    DWORD protection=info.Protect;
    if((protection&PAGE_GUARD)||((protection&0xFFu)==PAGE_NOACCESS))return false;
    u64 regionStart=(u64)info.BaseAddress;
    u64 regionEnd=regionStart+info.RegionSize;
    if((u64)page<regionStart||(u64)page+(u64)pageSize>regionEnd)return false;
    pages[*count].address=page;
    pages[*count].originalProtection=protection;
    (*count)++;
    return true;
}

static bool addProtectionInterval(ProtectionPage* pages,u32* count,u32 capacity,u8* address,u64 length,u32 pageSize){
    if(!pages||!count||!address||!length||!pageSize||(pageSize&(pageSize-1u))!=0)return false;
    u64 start=(u64)address;
    u64 last=start+length-1u;
    if(last<start)return false;
    u64 mask=(u64)pageSize-1u;
    u64 page=start&~mask;
    u64 finalPage=last&~mask;
    for(;;){
        if(!addProtectionPage(pages,count,capacity,(u8*)page,pageSize))return false;
        if(page==finalPage)break;
        page+=(u64)pageSize;
        if(page<start)return false;
    }
    return true;
}

static bool collectConfigWritePages(u8* config,ProtectionPage* pages,u32* count,u32 capacity){
    if(!config||!pages||!count)return false;
    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    if(pageSize<4096u||pageSize>65536u||(pageSize&(pageSize-1u))!=0)return false;
    *count=0;
    return addProtectionInterval(pages,count,capacity,config+OFF_DISTANCE_REALIZE,8u,pageSize)&&
           addProtectionInterval(pages,count,capacity,config+OFF_PASSABLE_LEVEL1,TOTAL_LEVEL_FIELDS*4u,pageSize)&&
           addProtectionInterval(pages,count,capacity,config+OFF_DISTANCE_OPEN,4u,pageSize);
}

static bool makePagesWritable(ProtectionPage* pages,u32 count,u32 pageSize){
    if(!pages||!count||!pageSize)return false;
    for(u32 i=0;i<count;i++){
        MEMORY_BASIC_INFORMATION_X64 info;
        if(VirtualQuery(pages[i].address,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
        if(isWritableProtection(info.Protect))continue;
        DWORD previous=0;
        if(!VirtualProtect(pages[i].address,pageSize,PAGE_READWRITE,&previous))return false;
    }
    return true;
}

static bool restorePages(ProtectionPage* pages,u32 count,u32 pageSize){
    if(!pages||!count||!pageSize)return false;
    bool complete=true;
    for(u32 i=count;i>0;i--){
        DWORD ignored=0;
        if(!VirtualProtect(pages[i-1].address,pageSize,pages[i-1].originalProtection,&ignored))complete=false;
    }
    for(u32 i=0;i<count;i++){
        MEMORY_BASIC_INFORMATION_X64 info;
        if(VirtualQuery(pages[i].address,&info,sizeof(info))<sizeof(info)||
           info.State!=MEM_COMMIT||info.Protect!=pages[i].originalProtection)complete=false;
    }
    return complete;
}

static bool connectionObstaclePatchMatches(const u8* address){
    return address&&
           address[0]==PATCHED_CONNECTION_OBSTACLE_RESULT&&
           bytesEqual(
               address+1,EXPECTED_CONNECTION_OBSTACLE_RESULT+1,
               (u32)(sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)-1u)
           );
}

static bool protectionMatches(const void* address,DWORD expected){
    MEMORY_BASIC_INFORMATION_X64 info;
    return address&&VirtualQuery(address,&info,sizeof(info))>=sizeof(info)&&
           info.State==MEM_COMMIT&&info.Protect==expected;
}

static bool fixedRideableRangePatchMatches(const u8* base){
    return base&&bytesEqual(
        base+RVA_FIXED_RIDEABLE_RANGE_BLOCK,
        PATCHED_FIXED_RIDEABLE_RANGE_BLOCK,
        (u32)sizeof(PATCHED_FIXED_RIDEABLE_RANGE_BLOCK)
    );
}

static bool atomicReplace16(u8* address,const u8* expected,const u8* replacement){
    if(!address||!expected||!replacement||((u64)address&15u)!=0u||!rangeReadable(address,16u))return false;
    __declspec(align(16)) long long comparand[2]={
        (long long)readU64Raw(expected),
        (long long)readU64Raw(expected+8)
    };
    return _InterlockedCompareExchange128(
        (volatile long long*)address,
        (long long)readU64Raw(replacement+8),
        (long long)readU64Raw(replacement),
        comparand
    )!=0;
}

static ApplyResult rollbackFixedRideableRangePatch(
    u8* base,
    DWORD originalProtection
){
    u8* address=base+RVA_FIXED_RIDEABLE_RANGE_BLOCK;
    DWORD protectionBeforeRollback=0;
    bool writable=VirtualProtect(address,16u,PAGE_EXECUTE_READWRITE,&protectionBeforeRollback)!=FALSE;
    bool bytesRestored=false;
    if(writable){
        bool replaced=bytesEqual(
            address,EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK,
            (u32)sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK)
        )||atomicReplace16(
            address,PATCHED_FIXED_RIDEABLE_RANGE_BLOCK,EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK
        );
        BOOL flushed=FlushInstructionCache(GetCurrentProcess(),address,16u);
        bytesRestored=replaced&&flushed&&bytesEqual(
            address,EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK,
            (u32)sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK)
        );
    }
    DWORD ignored=0;
    bool protectionsRestored=writable&&
        VirtualProtect(address,16u,originalProtection,&ignored)!=FALSE&&
        protectionMatches(address,originalProtection);
    return writable&&bytesRestored&&protectionsRestored?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult applyFixedRideableRangePatch(u8* base){
    if(!base)return APPLY_FAILED;
    u8* context=base+RVA_FIXED_RIDEABLE_RANGE_CONTEXT;
    u8* address=base+RVA_FIXED_RIDEABLE_RANGE_BLOCK;
    if(fixedRideableRangePatchMatches(base))return APPLY_READY;
    if(!bytesEqual(
        context,
        EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT,
        (u32)sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT)
    ))return APPLY_CONFLICT;

    DWORD originalProtection=0;
    if(!VirtualProtect(address,16u,PAGE_EXECUTE_READWRITE,&originalProtection))return APPLY_FAILED;
    if(!bytesEqual(
        context,
        EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT,
        (u32)sizeof(EXPECTED_FIXED_RIDEABLE_RANGE_CONTEXT)
    )){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,16u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }

    bool replaced=atomicReplace16(
        address,EXPECTED_FIXED_RIDEABLE_RANGE_BLOCK,PATCHED_FIXED_RIDEABLE_RANGE_BLOCK
    );
    if(!replaced){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,16u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),address,16u);
    if(!flushed||!fixedRideableRangePatchMatches(base)){
        return rollbackFixedRideableRangePatch(base,originalProtection);
    }
    DWORD ignored=0;
    if(!VirtualProtect(address,16u,originalProtection,&ignored)||
       !protectionMatches(address,originalProtection)){
        return rollbackFixedRideableRangePatch(base,originalProtection);
    }
    return APPLY_READY;
}

static bool travelSpeedDisplacement(const u8* base,const u8* cell,u32* displacement){
    if(!base||!cell||!displacement)return false;
    const u8* nextInstruction=base+RVA_TRAVEL_SPEED_LOAD+8u;
    s64 delta=(s64)(u64)cell-(s64)(u64)nextInstruction;
    if(delta<(-2147483647ll-1ll)||delta>2147483647ll)return false;
    *displacement=(u32)(s32)delta;
    return true;
}

static bool travelSpeedPatchMatches(const u8* base,const u8* cell,u32 targetBits){
    if(!base||!cell)return false;
    const u8 expectedOpcode[4]={0xC5,0xFA,0x10,0x0D};
    const u8* instruction=base+RVA_TRAVEL_SPEED_LOAD;
    if(!bytesEqual(instruction,expectedOpcode,4u))return false;
    u32 expectedDisplacement=0,currentDisplacement=0,currentTarget=0;
    if(!travelSpeedDisplacement(base,cell,&expectedDisplacement)||
       !readU32(instruction+4u,&currentDisplacement)||
       !readU32(cell,&currentTarget))return false;
    return currentDisplacement==expectedDisplacement&&currentTarget==targetBits;
}

static u8* tryAllocateTravelSpeedCell(u64 candidate,u32 pageSize,u32 targetBits){
    if(!candidate||!pageSize)return 0;
    u8* cell=(u8*)VirtualAlloc(
        (LPVOID)candidate,(SIZE_T)pageSize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE
    );
    if(!cell)return 0;
    if(cell!=(u8*)candidate){
        VirtualFree(cell,0,MEM_RELEASE);
        return 0;
    }
    if(!writeU32(cell,targetBits)){
        VirtualFree(cell,0,MEM_RELEASE);
        return 0;
    }
    DWORD originalProtection=0;
    if(!VirtualProtect(cell,pageSize,PAGE_READONLY,&originalProtection)||
       !protectionMatches(cell,PAGE_READONLY)){
        VirtualFree(cell,0,MEM_RELEASE);
        return 0;
    }
    return cell;
}

static u8* scanForTravelSpeedCell(
    const u8* base,
    u64 begin,
    u64 end,
    u32 pageSize,
    u32 allocationGranularity,
    u32 targetBits
){
    if(!base||begin>=end||!pageSize||!allocationGranularity)return 0;
    u64 cursor=begin;
    while(cursor<end){
        MEMORY_BASIC_INFORMATION_X64 info;
        if(VirtualQuery((LPCVOID)cursor,&info,sizeof(info))<sizeof(info)||!info.RegionSize)return 0;
        u64 regionStart=(u64)info.BaseAddress;
        u64 regionEnd=regionStart+info.RegionSize;
        if(regionEnd<=cursor)return 0;
        if(info.State==MEM_FREE){
            u64 usableStart=regionStart>cursor?regionStart:cursor;
            u64 mask=(u64)allocationGranularity-1u;
            u64 candidate=(usableStart+mask)&~mask;
            u64 usableEnd=regionEnd<end?regionEnd:end;
            if(candidate>=usableStart&&candidate+(u64)pageSize>=candidate&&
               candidate+(u64)pageSize<=usableEnd){
                u32 displacement=0;
                if(travelSpeedDisplacement(base,(u8*)candidate,&displacement)){
                    u8* cell=tryAllocateTravelSpeedCell(candidate,pageSize,targetBits);
                    if(cell)return cell;
                }
            }
        }
        cursor=regionEnd;
    }
    return 0;
}

static u8* allocateTravelSpeedCell(
    const u8* base,
    u32 imageSize,
    u32 targetBits
){
    if(!base||!imageSize)return 0;
    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    u32 granularity=systemInfo.AllocationGranularity;
    if(pageSize<4096u||pageSize>65536u||(pageSize&(pageSize-1u))!=0u||
       granularity<pageSize||(granularity&(granularity-1u))!=0u)return 0;

    u64 origin=(u64)(base+RVA_TRAVEL_SPEED_LOAD+8u);
    u64 applicationMinimum=(u64)systemInfo.MinimumApplicationAddress;
    u64 applicationMaximum=(u64)systemInfo.MaximumApplicationAddress;
    const u64 reach=0x7FFF0000ull;
    u64 lower=origin>reach?origin-reach:applicationMinimum;
    if(lower<applicationMinimum)lower=applicationMinimum;
    u64 upper=origin+reach;
    if(upper<origin||upper>applicationMaximum)upper=applicationMaximum;
    if(upper<=lower||(u64)pageSize>upper-lower)return 0;

    u64 mask=(u64)granularity-1u;
    u64 preferred=((u64)base+(u64)imageSize+mask)&~mask;
    if(preferred<lower)preferred=(lower+mask)&~mask;
    if(preferred>upper)preferred=upper;

    u8* cell=0;
    if(preferred<upper){
        cell=scanForTravelSpeedCell(base,preferred,upper,pageSize,granularity,targetBits);
    }
    if(!cell&&lower<preferred){
        cell=scanForTravelSpeedCell(base,lower,preferred,pageSize,granularity,targetBits);
    }
    return cell;
}

static ApplyResult rollbackTravelSpeedPatch(
    u8* base,
    u32 replacementDisplacement,
    DWORD originalProtection
){
    u8* address=base+RVA_TRAVEL_SPEED_DISPLACEMENT;
    DWORD protectionBeforeRollback=0;
    bool writable=VirtualProtect(address,4u,PAGE_EXECUTE_READWRITE,&protectionBeforeRollback)!=FALSE;
    bool bytesRestored=false;
    if(writable){
        u32 current=0;
        bool readComplete=readU32(address,&current);
        bool replaced=readComplete&&current==EXPECTED_TRAVEL_SPEED_DISPLACEMENT;
        if(readComplete&&current==replacementDisplacement){
            long observed=_InterlockedCompareExchange(
                (volatile long*)address,
                (long)EXPECTED_TRAVEL_SPEED_DISPLACEMENT,
                (long)replacementDisplacement
            );
            replaced=(u32)observed==replacementDisplacement;
        }
        BOOL flushed=FlushInstructionCache(GetCurrentProcess(),base+RVA_TRAVEL_SPEED_LOAD,8u);
        bytesRestored=replaced&&flushed&&bytesEqual(
            base+RVA_TRAVEL_SPEED_CONTEXT,
            EXPECTED_TRAVEL_SPEED_CONTEXT,
            (u32)sizeof(EXPECTED_TRAVEL_SPEED_CONTEXT)
        );
    }
    DWORD ignored=0;
    bool protectionsRestored=writable&&
        VirtualProtect(address,4u,originalProtection,&ignored)!=FALSE&&
        protectionMatches(address,originalProtection);
    return writable&&bytesRestored&&protectionsRestored?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult applyTravelSpeedPatchWithCell(
    u8* base,
    u8* cell,
    u32 targetBits
){
    if(!base||!cell||((u64)(base+RVA_TRAVEL_SPEED_DISPLACEMENT)&3u)!=0u)return APPLY_FAILED;
    if(travelSpeedPatchMatches(base,cell,targetBits))return APPLY_READY;
    if(!bytesEqual(
        base+RVA_TRAVEL_SPEED_CONTEXT,
        EXPECTED_TRAVEL_SPEED_CONTEXT,
        (u32)sizeof(EXPECTED_TRAVEL_SPEED_CONTEXT)
    ))return APPLY_CONFLICT;
    u32 nativeTravelSpeed=0;
    if(!readU32(base+RVA_NATIVE_TRAVEL_SPEED,&nativeTravelSpeed))return APPLY_FAILED;
    if(nativeTravelSpeed!=floatBits(NATIVE_TRAVEL_SPEED))return APPLY_CONFLICT;

    u32 replacementDisplacement=0;
    if(!travelSpeedDisplacement(base,cell,&replacementDisplacement))return APPLY_FAILED;
    u8* address=base+RVA_TRAVEL_SPEED_DISPLACEMENT;
    DWORD originalProtection=0;
    if(!VirtualProtect(address,4u,PAGE_EXECUTE_READWRITE,&originalProtection))return APPLY_FAILED;
    if(!bytesEqual(
        base+RVA_TRAVEL_SPEED_CONTEXT,
        EXPECTED_TRAVEL_SPEED_CONTEXT,
        (u32)sizeof(EXPECTED_TRAVEL_SPEED_CONTEXT)
    )){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,4u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    if(!readU32(base+RVA_NATIVE_TRAVEL_SPEED,&nativeTravelSpeed)||
       nativeTravelSpeed!=floatBits(NATIVE_TRAVEL_SPEED)){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,4u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }

    long observed=_InterlockedCompareExchange(
        (volatile long*)address,
        (long)replacementDisplacement,
        (long)EXPECTED_TRAVEL_SPEED_DISPLACEMENT
    );
    if((u32)observed!=EXPECTED_TRAVEL_SPEED_DISPLACEMENT){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,4u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),base+RVA_TRAVEL_SPEED_LOAD,8u);
    if(!flushed||!travelSpeedPatchMatches(base,cell,targetBits)){
        return rollbackTravelSpeedPatch(base,replacementDisplacement,originalProtection);
    }
    DWORD ignored=0;
    if(!VirtualProtect(address,4u,originalProtection,&ignored)||
       !protectionMatches(address,originalProtection)){
        return rollbackTravelSpeedPatch(base,replacementDisplacement,originalProtection);
    }
    return APPLY_READY;
}

static ApplyResult installTravelSpeedPatch(
    u8* base,
    u32 imageSize,
    const Settings* settings
){
    if(!base||!settings)return APPLY_FAILED;
    if(settings->travelSpeedBits==floatBits(NATIVE_TRAVEL_SPEED))return APPLY_READY;
    if(g_travelSpeedCell){
        return travelSpeedPatchMatches(base,g_travelSpeedCell,settings->travelSpeedBits)?
            APPLY_READY:APPLY_CONFLICT;
    }
    u8* cell=allocateTravelSpeedCell(base,imageSize,settings->travelSpeedBits);
    if(!cell)return APPLY_FAILED;
    ApplyResult result=applyTravelSpeedPatchWithCell(base,cell,settings->travelSpeedBits);
    if(result==APPLY_READY||result==APPLY_CRITICAL){
        // A critical rollback result may still leave the patched instruction
        // referencing this cell. Keep it mapped to avoid a dangling RIP load.
        g_travelSpeedCell=cell;
    }else{
        VirtualFree(cell,0,MEM_RELEASE);
    }
    return result;
}

static ApplyResult rollbackConnectionObstaclePatch(
    u8* address,
    DWORD originalProtection
){
    DWORD protectionBeforeRollback=0;
    bool writable=VirtualProtect(address,1u,PAGE_EXECUTE_READWRITE,&protectionBeforeRollback)!=FALSE;
    bool bytesRestored=false;
    if(writable){
        bool wrote=writeU8(address,EXPECTED_CONNECTION_OBSTACLE_RESULT[0]);
        BOOL flushed=FlushInstructionCache(GetCurrentProcess(),address,1u);
        bytesRestored=wrote&&flushed&&bytesEqual(
            address,EXPECTED_CONNECTION_OBSTACLE_RESULT,
            (u32)sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)
        );
    }
    DWORD ignored=0;
    bool protectionsRestored=writable&&
        VirtualProtect(address,1u,originalProtection,&ignored)!=FALSE&&
        protectionMatches(address,originalProtection);
    return writable&&bytesRestored&&protectionsRestored?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult applyConnectionObstacleSetting(u8* base,const Settings* settings){
    if(!base||!settings)return APPLY_FAILED;
    if(!settings->ignoreConnectionObstacles)return APPLY_READY;
    u8* address=base+RVA_CONNECTION_OBSTACLE_RESULT;
    if(connectionObstaclePatchMatches(address))return APPLY_READY;
    if(!bytesEqual(
        address,EXPECTED_CONNECTION_OBSTACLE_RESULT,
        (u32)sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)
    ))return APPLY_CONFLICT;

    DWORD originalProtection=0;
    if(!VirtualProtect(address,1u,PAGE_EXECUTE_READWRITE,&originalProtection))return APPLY_FAILED;

    if(!bytesEqual(
        address,EXPECTED_CONNECTION_OBSTACLE_RESULT,
        (u32)sizeof(EXPECTED_CONNECTION_OBSTACLE_RESULT)
    )){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,1u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    bool wrote=writeU8(address,PATCHED_CONNECTION_OBSTACLE_RESULT);
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),address,1u);
    if(!wrote||!flushed||!connectionObstaclePatchMatches(address)){
        return rollbackConnectionObstaclePatch(address,originalProtection);
    }
    DWORD ignored=0;
    if(!VirtualProtect(address,1u,originalProtection,&ignored)||
       !protectionMatches(address,originalProtection)){
        return rollbackConnectionObstaclePatch(address,originalProtection);
    }
    return APPLY_READY;
}

static ApplyResult rollbackConfigTransaction(
    u8* config,
    const u32* originalLevels,
    u32 originalDistanceRealize,
    u32 originalDistanceUnrealize,
    u32 originalDistanceOpen,
    ProtectionPage* pages,
    u32 pageCount,
    u32 pageSize
){
    bool writable=makePagesWritable(pages,pageCount,pageSize);
    bool valuesRestored=false;
    if(writable){
        bool writesComplete=writeU32(config+OFF_DISTANCE_REALIZE,originalDistanceRealize)&&
                            writeU32(config+OFF_DISTANCE_UNREALIZE,originalDistanceUnrealize);
        for(u32 i=0;i<TOTAL_LEVEL_FIELDS&&writesComplete;i++){
            writesComplete=writeU32(config+OFF_PASSABLE_LEVEL1+i*4u,originalLevels[i]);
        }
        if(writesComplete)writesComplete=writeU32(config+OFF_DISTANCE_OPEN,originalDistanceOpen);
        valuesRestored=writesComplete&&verifyConfigOriginals(
            config,originalLevels,originalDistanceRealize,originalDistanceUnrealize,originalDistanceOpen
        );
    }
    bool protectionsRestored=restorePages(pages,pageCount,pageSize);
    return writable&&valuesRestored&&protectionsRestored?APPLY_FAILED:APPLY_CRITICAL;
}

static bool targetTupleMatches(const u32* values,const Settings* settings){
    if(!values||!settings)return false;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++)if(values[i]!=settings->targetBits[i])return false;
    return true;
}

static ApplyResult applyRanges(u8* base,u8* config,const Settings* settings){
    if(!base||!config||!settings)return APPLY_FAILED;
    u8* confirmedConfig=0;
    u8* confirmedFacility=0;
    ResolveResult identity=resolveZiplineConfig(base,&confirmedConfig,&confirmedFacility);
    if(identity==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(identity!=RESOLVE_READY)return APPLY_FAILED;
    if(confirmedConfig!=config)return APPLY_NOT_READY;
    u32 maxLevel=0;
    if(!readU32(config+OFF_MAX_LEVEL,&maxLevel))return APPLY_FAILED;
    if(maxLevel==0)return APPLY_NOT_READY;
    if(maxLevel!=1u&&maxLevel!=SERIALIZED_MAX_LEVEL)return APPLY_CONFLICT;

    u32 current[TOTAL_LEVEL_FIELDS];
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        if(!readU32(config+OFF_PASSABLE_LEVEL1+i*4u,&current[i]))return APPLY_FAILED;
    }
    bool allZero=true;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++)if(current[i]!=0)allZero=false;
    if(allZero)return APPLY_NOT_READY;

    bool constructorTuple=tupleMatches(current,CONSTRUCTOR_LEVELS,TOTAL_LEVEL_FIELDS);
    bool nativeTuple=tupleMatches(current,NATIVE_LEVELS,TOTAL_LEVEL_FIELDS);
    bool targetTuple=targetTupleMatches(current,settings);
    if(!constructorTuple&&!nativeTuple&&!targetTuple)return APPLY_CONFLICT;

    u32 distanceRealize=0,distanceUnrealize=0,distanceOpen=0;
    if(!readU32(config+OFF_DISTANCE_REALIZE,&distanceRealize)||
       !readU32(config+OFF_DISTANCE_UNREALIZE,&distanceUnrealize)||
       !readU32(config+OFF_DISTANCE_OPEN,&distanceOpen))return APPLY_FAILED;
    if(!distanceRealize||!distanceUnrealize||!distanceOpen)return APPLY_NOT_READY;
    if(constructorTuple&&distanceRealize==floatBits(CONSTRUCTOR_DISTANCE_REALIZE)&&
       distanceUnrealize==floatBits(CONSTRUCTOR_DISTANCE_UNREALIZE))return APPLY_NOT_READY;
    if((distanceRealize!=floatBits(NATIVE_DISTANCE_REALIZE)&&distanceRealize!=settings->distanceRealizeBits)||
       (distanceUnrealize!=floatBits(NATIVE_DISTANCE_UNREALIZE)&&distanceUnrealize!=settings->distanceUnrealizeBits)||
       (distanceOpen!=floatBits(NATIVE_DISTANCE_OPEN)&&distanceOpen!=settings->distanceOpenBits))return APPLY_CONFLICT;

    bool needsWrite=false;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++)if(current[i]!=settings->targetBits[i])needsWrite=true;
    if(distanceRealize!=settings->distanceRealizeBits||distanceUnrealize!=settings->distanceUnrealizeBits||
       distanceOpen!=settings->distanceOpenBits)needsWrite=true;
    if(!needsWrite)return APPLY_READY;

    if(constructorTuple){
        const char message[]="STATE: accepted persistent constructor baseline 300/351/351/351/351.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1));
    }else if(nativeTuple){
        const char message[]="STATE: accepted serialized resource baseline 301/351/351/351/351.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1));
    }

    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    ProtectionPage pages[8];
    u32 pageCount=0;
    if(!collectConfigWritePages(config,pages,&pageCount,8u))return APPLY_FAILED;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

    confirmedConfig=0;
    confirmedFacility=0;
    identity=resolveZiplineConfig(base,&confirmedConfig,&confirmedFacility);
    if(identity!=RESOLVE_READY||confirmedConfig!=config){
        return restorePages(pages,pageCount,pageSize)?APPLY_NOT_READY:APPLY_CRITICAL;
    }

    bool writesComplete=writeU32(config+OFF_DISTANCE_REALIZE,settings->distanceRealizeBits)&&
                        writeU32(config+OFF_DISTANCE_UNREALIZE,settings->distanceUnrealizeBits);
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS&&writesComplete;i++){
        writesComplete=writeU32(config+OFF_PASSABLE_LEVEL1+i*4u,settings->targetBits[i]);
    }
    if(writesComplete)writesComplete=writeU32(config+OFF_DISTANCE_OPEN,settings->distanceOpenBits);
    bool verified=writesComplete;
    for(u32 i=0;i<TOTAL_LEVEL_FIELDS;i++){
        u32 value=0;
        if(!readU32(config+OFF_PASSABLE_LEVEL1+i*4u,&value)||value!=settings->targetBits[i])verified=false;
    }
    u32 verifyRealize=0,verifyUnrealize=0,verifyOpen=0;
    if(!readU32(config+OFF_DISTANCE_REALIZE,&verifyRealize)||verifyRealize!=settings->distanceRealizeBits||
       !readU32(config+OFF_DISTANCE_UNREALIZE,&verifyUnrealize)||verifyUnrealize!=settings->distanceUnrealizeBits||
       !readU32(config+OFF_DISTANCE_OPEN,&verifyOpen)||verifyOpen!=settings->distanceOpenBits)verified=false;

    if(!verified){
        return rollbackConfigTransaction(
            config,current,distanceRealize,distanceUnrealize,distanceOpen,
            pages,pageCount,pageSize
        );
    }

    if(!restorePages(pages,pageCount,pageSize)){
        return rollbackConfigTransaction(
            config,current,distanceRealize,distanceUnrealize,distanceOpen,
            pages,pageCount,pageSize
        );
    }
    return APPLY_READY;
}

static ApplyResult rollbackSingleField(u8* field,u32 original,ProtectionPage* pages,u32 pageCount,u32 pageSize){
    bool writable=makePagesWritable(pages,pageCount,pageSize);
    bool valueRestored=false;
    if(writable){
        bool writeComplete=writeU32(field,original);
        u32 current=0;
        valueRestored=writeComplete&&readU32(field,&current)&&current==original;
    }
    bool protectionsRestored=restorePages(pages,pageCount,pageSize);
    return writable&&valueRestored&&protectionsRestored?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult applyForceRadius(u8* base,u8* facility,u8* imaginaryComponent,const Settings* settings){
    if(!base||!facility||!imaginaryComponent||!settings)return APPLY_FAILED;
    u8* confirmedFacility=0;
    u8* confirmedComponent=0;
    ResolveResult identity=resolveCanonicalImaginaryComponent(base,&confirmedFacility,&confirmedComponent);
    if(identity==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(identity!=RESOLVE_READY)return APPLY_FAILED;
    if(confirmedFacility!=facility||confirmedComponent!=imaginaryComponent)return APPLY_NOT_READY;
    u8* field=imaginaryComponent+OFF_FORCE_REALIZE_RADIUS;
    u32 current=0;
    if(!readU32(field,&current))return APPLY_FAILED;
    if(!current)return APPLY_NOT_READY;
    if(current!=floatBits(NATIVE_FORCE_REALIZE_RADIUS)&&current!=settings->forceRealizeRadiusBits)return APPLY_CONFLICT;
    if(current==settings->forceRealizeRadiusBits)return APPLY_READY;

    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    ProtectionPage pages[2];
    u32 pageCount=0;
    if(!addProtectionInterval(pages,&pageCount,2u,field,4u,pageSize))return APPLY_FAILED;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

    confirmedFacility=0;
    confirmedComponent=0;
    identity=resolveCanonicalImaginaryComponent(base,&confirmedFacility,&confirmedComponent);
    if(identity!=RESOLVE_READY||confirmedFacility!=facility||confirmedComponent!=imaginaryComponent){
        return restorePages(pages,pageCount,pageSize)?APPLY_NOT_READY:APPLY_CRITICAL;
    }

    bool writeComplete=writeU32(field,settings->forceRealizeRadiusBits);
    u32 verified=0;
    if(!writeComplete||!readU32(field,&verified)||verified!=settings->forceRealizeRadiusBits){
        return rollbackSingleField(field,current,pages,pageCount,pageSize);
    }
    if(!restorePages(pages,pageCount,pageSize)){
        return rollbackSingleField(field,current,pages,pageCount,pageSize);
    }
    return APPLY_READY;
}

static ApplyResult applyUiVisibleDistance(u8* base,u8* gameParameters,const Settings* settings){
    if(!base||!gameParameters||!settings)return APPLY_FAILED;
    u8* confirmedParameters=0;
    ResolveResult identity=resolveUiGameDesign(base,&confirmedParameters);
    if(identity==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(identity!=RESOLVE_READY)return APPLY_FAILED;
    if(confirmedParameters!=gameParameters)return APPLY_NOT_READY;

    u8* field=gameParameters+OFF_UI_ZIPLINE_VISIBLE_DISTANCE;
    u32 current=0;
    if(!readU32(field,&current))return APPLY_FAILED;
    if(!current)return APPLY_NOT_READY;
    if(current!=floatBits(NATIVE_UI_ZIPLINE_VISIBLE_DISTANCE)&&current!=settings->uiVisibleDistanceBits)return APPLY_CONFLICT;
    if(current==settings->uiVisibleDistanceBits)return APPLY_READY;

    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    ProtectionPage pages[2];
    u32 pageCount=0;
    if(!addProtectionInterval(pages,&pageCount,2u,field,4u,pageSize))return APPLY_FAILED;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

    confirmedParameters=0;
    identity=resolveUiGameDesign(base,&confirmedParameters);
    if(identity!=RESOLVE_READY||confirmedParameters!=gameParameters){
        return restorePages(pages,pageCount,pageSize)?APPLY_NOT_READY:APPLY_CRITICAL;
    }

    bool writeComplete=writeU32(field,settings->uiVisibleDistanceBits);
    u32 verified=0;
    if(!writeComplete||!readU32(field,&verified)||verified!=settings->uiVisibleDistanceBits){
        return rollbackSingleField(field,current,pages,pageCount,pageSize);
    }
    if(!restorePages(pages,pageCount,pageSize)){
        return rollbackSingleField(field,current,pages,pageCount,pageSize);
    }
    return APPLY_READY;
}

static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    Sleep(1000);

    Settings settings;
    SettingsResult settingsResult=loadSettings(&settings);
    if(settingsResult!=SETTINGS_READY){
        closeLog();
        return 0;
    }
    if(g_debugLog)openLog();

    u8* base=(u8*)game;
    u32 timestamp=0,imageSize=0;
    if(!readBuildMetadata(base,&timestamp,&imageSize)){
        const char message[]="ERROR: could not read DS2.exe metadata; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(timestamp!=EXPECTED_TIMESTAMP||imageSize!=EXPECTED_IMAGE_SIZE){
        const char message[]="ERROR: unsupported DS2.exe build; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    bool changeTravelSpeed=settings.travelSpeedBits!=floatBits(NATIVE_TRAVEL_SPEED);
    if(!preflightCode(base,imageSize,settings.ignoreConnectionObstacles,changeTravelSpeed)){
        const char message[]="ERROR: zipline range signatures do not match; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(!validateVtableEntries(base,RVA_MANAGER_VTABLE,MANAGER_VFUNCS,4)||
       !validateVtableEntries(base,RVA_FACILITY_VTABLE,FACILITY_VFUNCS,4)||
       !validateVtableEntries(base,RVA_ZIPLINE_CONFIG_VTABLE,ZIPLINE_CONFIG_VFUNCS,4)||
       !validateVtableEntries(base,RVA_ENTITY_RESOURCE_VTABLE,ENTITY_RESOURCE_VFUNCS,4)||
       !validateVtableEntries(base,RVA_IMAGINARY_COMPONENT_VTABLE,IMAGINARY_COMPONENT_VFUNCS,4)||
       !validateVtableEntries(base,RVA_UI_RESIDENT_VTABLE,UI_RESIDENT_VFUNCS,4)||
       !validateVtableEntries(base,RVA_UI_GAME_PARAMS_VTABLE,UI_GAME_PARAMS_VFUNCS,4)){
        const char message[]="ERROR: expected DS2 RTTI/VTables do not match; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }

    ApplyResult travelSpeedPatch=installTravelSpeedPatch(base,imageSize,&settings);
    if(travelSpeedPatch==APPLY_CONFLICT){
        const char message[]="ERROR: Zipline travel-speed load or native 16.0 value is already modified; no range resource writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(travelSpeedPatch==APPLY_FAILED){
        const char message[]="ERROR: Zipline travel-speed patch failed and was rolled back; no range resource writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(travelSpeedPatch==APPLY_CRITICAL){
        const char message[]="CRITICAL: Zipline travel-speed rollback or memory-protection restore was incomplete; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(changeTravelSpeed){
        const char message[]="ACTIVE: Zipline travel progress uses the INI SpeedMultiplier for normal, carrier, and ADS rides.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1));
    }else{
        const char message[]="STATE: SpeedMultiplier=1.0; native Zipline travel speed is unchanged.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1));
    }

    ApplyResult rideableRangePatch=applyFixedRideableRangePatch(base);
    if(rideableRangePatch==APPLY_CONFLICT){
        const char message[]="ERROR: fixed 351 m rideable-range comparison has unexpected bytes; no resource writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(rideableRangePatch==APPLY_FAILED){
        const char message[]="ERROR: fixed 351 m rideable-connection patch failed and was rolled back; no resource writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(rideableRangePatch==APPLY_CRITICAL){
        const char message[]="CRITICAL: fixed 351 m rideable-connection patch rollback or memory-protection restore was incomplete; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    {
        const char message[]="ACTIVE: the former fixed 351 m rideable-range comparison now uses the per-connection INI-derived limit.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1));
    }

    ApplyResult obstacleSetting=applyConnectionObstacleSetting(base,&settings);
    if(obstacleSetting==APPLY_CONFLICT){
        const char message[]="ERROR: Zipline connection-obstacle patch site has unexpected bytes; no resource writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(obstacleSetting==APPLY_FAILED){
        const char message[]="ERROR: Zipline connection-obstacle patch failed and was rolled back; no resource writes were applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(obstacleSetting==APPLY_CRITICAL){
        const char message[]="CRITICAL: connection-obstacle patch rollback or memory-protection restore was incomplete; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1));
        closeLog();
        return 0;
    }
    if(settings.ignoreConnectionObstacles){
        const char message[]="ACTIVE: shared Zipline/monorail connection obstacle hits are ignored; configured distance gates remain active.\r\n";
        logDebug(message,(DWORD)(sizeof(message)-1));
    }

    const char waiting[]="Validated target; waiting for canonical Zipline and UI resources.\r\n";
    logDebug(waiting,(DWORD)(sizeof(waiting)-1));
    bool configWasActive=false;
    bool configPendingLogged=false;
    bool configInvalidLogged=false;
    bool configConflictLogged=false;
    bool configFailedLogged=false;
    bool componentWasActive=false;
    bool componentInvalidLogged=false;
    bool componentConflictLogged=false;
    bool componentFailedLogged=false;
    bool uiWasActive=false;
    bool uiInvalidLogged=false;
    bool uiConflictLogged=false;
    bool uiFailedLogged=false;
    for(;;){
        bool stop=false;
        DWORD delay=1000u;
        u8* config=0;
        u8* facility=0;
        u8* imaginaryComponent=0;
        ResolveResult resolved=resolveZiplineConfig(base,&config,&facility);
        if(resolved==RESOLVE_NOT_READY){
            delay=250u;
        }else if(resolved==RESOLVE_INVALID){
            if(!configInvalidLogged){
                const char message[]="WARNING: canonical Zipline resource path is temporarily invalid; config writes are paused.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1));
                configInvalidLogged=true;
            }
        }else{
            configInvalidLogged=false;
            ApplyResult applied=applyRanges(base,config,&settings);
            if(applied==APPLY_NOT_READY){
                if(!configPendingLogged){
                    const char message[]="STATE: Zipline config object exists but its serialized values are not ready; waiting.\r\n";
                    logDebug(message,(DWORD)(sizeof(message)-1));
                    configPendingLogged=true;
                }
                delay=250u;
            }else if(applied==APPLY_CONFLICT){
                if(!configConflictLogged){
                    const char message[]="ERROR: Zipline config has unexpected values or level count; possible mod conflict. Writes are paused and will be retried.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1));
                    configConflictLogged=true;
                }
            }else if(applied==APPLY_FAILED){
                if(!configFailedLogged){
                    const char message[]="ERROR: transactional Zipline range write failed and was rolled back; it will be retried.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1));
                    configFailedLogged=true;
                }
            }else if(applied==APPLY_CRITICAL){
                const char message[]="CRITICAL: Zipline range rollback or memory-protection restore was incomplete; stop DS2.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1));
                stop=true;
            }else{
                configPendingLogged=false;
                configConflictLogged=false;
                configFailedLogged=false;
                if(!configWasActive){
                    const char message[]="ACTIVE: all five internal connection-range slots and config realization distances use the INI-derived values.\r\n";
                    logDebug(message,(DWORD)(sizeof(message)-1));
                    configWasActive=true;
                }
            }

            if(!stop){
                u8* componentFacility=0;
                ResolveResult componentResult=resolveCanonicalImaginaryComponent(base,&componentFacility,&imaginaryComponent);
                if(componentResult==RESOLVE_READY&&componentFacility!=facility)componentResult=RESOLVE_NOT_READY;
                if(componentResult==RESOLVE_INVALID){
                    if(!componentInvalidLogged){
                        const char message[]="WARNING: optional Zipline imaginary component is temporarily invalid; its write is paused.\r\n";
                        logCritical(message,(DWORD)(sizeof(message)-1));
                        componentInvalidLogged=true;
                    }
                }else if(componentResult==RESOLVE_READY){
                    componentInvalidLogged=false;
                    ApplyResult forceApplied=applyForceRadius(base,facility,imaginaryComponent,&settings);
                    if(forceApplied==APPLY_CONFLICT){
                        if(!componentConflictLogged){
                            const char message[]="WARNING: Zipline ForceRealizeRadius has an unexpected value; its write is paused and will be retried.\r\n";
                            logCritical(message,(DWORD)(sizeof(message)-1));
                            componentConflictLogged=true;
                        }
                    }else if(forceApplied==APPLY_FAILED){
                        if(!componentFailedLogged){
                            const char message[]="WARNING: Zipline ForceRealizeRadius write failed and was rolled back; it will be retried.\r\n";
                            logCritical(message,(DWORD)(sizeof(message)-1));
                            componentFailedLogged=true;
                        }
                    }else if(forceApplied==APPLY_CRITICAL){
                        const char message[]="CRITICAL: ForceRealizeRadius rollback or memory-protection restore was incomplete; stop DS2.\r\n";
                        logCritical(message,(DWORD)(sizeof(message)-1));
                        stop=true;
                    }else if(forceApplied==APPLY_READY){
                        componentConflictLogged=false;
                        componentFailedLogged=false;
                        if(!componentWasActive){
                            const char message[]="ACTIVE: Zipline ForceRealizeRadius also uses the INI-derived value.\r\n";
                            logDebug(message,(DWORD)(sizeof(message)-1));
                            componentWasActive=true;
                        }
                    }
                }
            }
        }

        if(stop)break;

        u8* gameParameters=0;
        ResolveResult uiResult=resolveUiGameDesign(base,&gameParameters);
        if(uiResult==RESOLVE_INVALID){
            if(!uiInvalidLogged){
                const char message[]="WARNING: canonical Zipline UI distance path is temporarily invalid; UI write is paused.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1));
                uiInvalidLogged=true;
            }
        }else if(uiResult==RESOLVE_READY){
            uiInvalidLogged=false;
            ApplyResult uiApplied=applyUiVisibleDistance(base,gameParameters,&settings);
            if(uiApplied==APPLY_CONFLICT){
                if(!uiConflictLogged){
                    const char message[]="ERROR: Zipline UI visibility distance has an unexpected value; possible mod conflict. Its write is paused and will be retried.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1));
                    uiConflictLogged=true;
                }
            }else if(uiApplied==APPLY_FAILED){
                if(!uiFailedLogged){
                    const char message[]="ERROR: Zipline UI visibility distance write failed and was rolled back; it will be retried.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1));
                    uiFailedLogged=true;
                }
            }else if(uiApplied==APPLY_CRITICAL){
                const char message[]="CRITICAL: UI distance rollback or memory-protection restore was incomplete; stop DS2.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1));
                break;
            }else if(uiApplied==APPLY_READY){
                uiConflictLogged=false;
                uiFailedLogged=false;
                if(!uiWasActive){
                    const char message[]="ACTIVE: Zipline HUD/selection visibility distance uses the INI-derived value.\r\n";
                    logDebug(message,(DWORD)(sizeof(message)-1));
                    uiWasActive=true;
                }
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
        start();
    }
    return TRUE;
}
