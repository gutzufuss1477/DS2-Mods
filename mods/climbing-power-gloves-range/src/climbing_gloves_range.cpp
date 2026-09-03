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
extern "C" long _InterlockedExchange(volatile long*,long);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u

static const u64 RVA_ITEM_SYSTEM_GLOBAL=0x0623E5B0ull;
static const u64 RVA_ITEM_SYSTEM_VTABLE=0x032036E0ull;
static const u64 RVA_ITEM_PARAMETER_VTABLE=0x03203920ull;
static const u64 RVA_ITEM_SYSTEM_INIT=0x01EC40A3ull;
static const u64 RVA_ITEM_SYSTEM_CONSTRUCTOR=0x00BAC790ull;
static const u64 RVA_ITEM_LOOKUP=0x00BAD380ull;
static const u64 RVA_ITEM_LOOKUP_CORE=0x00BAD3A0ull;
static const u64 RVA_ITEM_PARAMETER_DESCRIPTOR=0x043320A0ull;
static const u64 RVA_ITEM_PARAMETER_TYPE_NAME=0x04330A60ull;
static const u64 RVA_ITEM_PARAMETER_FACTORY=0x00BAC320ull;
static const u64 RVA_ITEM_PARAMETER_DESTRUCTOR_THUNK=0x00BAC3A0ull;

static const u64 ITEM_SYSTEM_VFUNCS[2]={0x00BAC240ull,0x00BAC7E0ull};
static const u64 ITEM_PARAMETER_VFUNCS[4]={
    0x00BAC170ull,0x00BAC180ull,0x00109DE0ull,0x00109EB0ull
};

static const u8 EXPECTED_ITEM_SYSTEM_INIT[35]={
    0x48,0x8D,0x0D,0x56,0xE1,0x46,0x02,0xE8,0x31,0xFD,0x23,0xFE,
    0x48,0x8B,0xC8,0x48,0x8B,0xD8,0xE8,0xD6,0x86,0xCE,0xFE,
    0x48,0x89,0x1D,0xEF,0xA4,0x37,0x04,0xE8,0xAA,0x87,0xCE,0xFE
};

static const u8 EXPECTED_ITEM_SYSTEM_CONSTRUCTOR[32]={
    0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x48,0x8D,0x05,
    0x40,0x6F,0x65,0x02,0x48,0x89,0x01,0xBA,0x67,0x00,0x00,0x00,
    0x33,0xC0,0x48,0x83,0xC1,0x10,0x48,0x89
};

static const u8 EXPECTED_ITEM_LOOKUP[32]={
    0x48,0x89,0x5C,0x24,0x08,0x48,0x63,0x41,0x10,0x45,0x33,0xD2,
    0x48,0x8B,0xD9,0x85,0xC0,0x7E,0x24,0x4C,0x8B,0x49,0x18,0x4C,
    0x8B,0xD8,0x45,0x8B,0xC2,0x0F,0x1F,0x00
};

static const u8 EXPECTED_ITEM_LOOKUP_CORE[21]={
    0x49,0x8B,0x01,0x3A,0x50,0x20,0x74,0x17,0x41,0xFF,0xC2,
    0x49,0xFF,0xC0,0x49,0x83,0xC1,0x08,0x4D,0x3B,0xC3
};

static const u8 EXPECTED_ITEM_PARAMETER_DESCRIPTOR[24]={
    0xFF,0xFF,0xFF,0xFF,0x04,0x00,0x01,0x00,0x00,0xFF,0x18,0x00,
    0x00,0x00,0x00,0x00,0xA0,0x00,0x00,0x00,0x08,0x00,0x00,0x00
};

static const char EXPECTED_ITEM_PARAMETER_TYPE_NAME[]="DSItemParameter";

static const u32 OFF_ITEM_SYSTEM_ARRAY=0x10u;
static const u32 OFF_ITEM_ID=0x20u;
static const u32 OFF_ITEM_LEVEL=0x21u;
static const u32 OFF_ITEM_CATEGORY=0x22u;
static const u32 OFF_ITEM_SUBCATEGORY=0x23u;
static const u32 OFF_ITEM_PARAMS=0x68u;
static const u32 ITEM_SYSTEM_MAX_COUNT=103u;
static const u32 ITEM_PARAM_COUNT=8u;
static const u8 ITEM_ID_LEVEL1=53u;
static const u8 ITEM_ID_LEVEL2=54u;
static const u8 ITEM_CATEGORY_GLOVE=8u;
static const u8 ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE=6u;
static const float NATIVE_LEVEL1_RANGE=8.0f;
static const float NATIVE_LEVEL2_RANGE=10.0f;
static const float DEFAULT_LEVEL1_RANGE=30.0f;
static const float DEFAULT_LEVEL2_RANGE=50.0f;
static const float MAX_CONFIGURED_RANGE=100.0f;

struct RawArray{
    u32 count;
    u32 capacity;
    u64 entries;
};

struct GameImage{
    u8* base;
    u32 timestamp;
    u32 imageSize;
};

struct Settings{
    float level1Range;
    float level2Range;
    u32 level1Bits;
    u32 level2Bits;
    bool enabled;
    bool debugLog;
};

struct ResolvedItems{
    u8* system;
    u8* level1;
    u8* level2;
};

struct GloveView{
    u8* object;
    u8* rangeCell;
    u32 nativeBits;
    u32 targetBits;
};

enum SettingsResult{SETTINGS_INVALID,SETTINGS_DISABLED,SETTINGS_READY};
enum ResolveResult{RESOLVE_NOT_READY,RESOLVE_INVALID,RESOLVE_READY};
enum InspectResult{INSPECT_INVALID,INSPECT_CONFLICT,INSPECT_NATIVE,INSPECT_TARGET};
enum ExchangeResult{
    EXCHANGE_FAILED,EXCHANGE_CONFLICT,EXCHANGE_ALREADY_TARGET,EXCHANGE_CHANGED
};
enum ApplyResult{
    APPLY_NOT_READY,APPLY_INVALID,APPLY_CONFLICT,APPLY_FAILED,APPLY_CRITICAL,APPLY_READY
};

static HMODULE g_self=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static bool g_debugLog=false;
static volatile long g_started=0;

static u32 floatBits(float value){
    union FloatBits{float value;u32 bits;};
    FloatBits converted;
    converted.value=value;
    return converted.bits;
}

static bool bytesEqual(const u8* left,const u8* right,u32 count){
    if(!left||!right)return false;
    for(u32 i=0;i<count;i++)if(left[i]!=right[i])return false;
    return true;
}

static bool readMemory(const void* source,void* destination,SIZE_T count){
    if(!source||!destination||!count)return false;
    SIZE_T read=0;
    return ReadProcessMemory(GetCurrentProcess(),source,destination,count,&read)!=FALSE&&
           read==count;
}

static bool readU8(const void* source,u8* value){
    return readMemory(source,value,1u);
}

static bool readU16(const void* source,u16* value){
    return readMemory(source,value,2u);
}

static bool readU32(const void* source,u32* value){
    return readMemory(source,value,4u);
}

static bool readU64(const void* source,u64* value){
    return readMemory(source,value,8u);
}

static bool memoryEquals(const void* source,const u8* expected,u32 count){
    if(!source||!expected||!count||count>128u)return false;
    u8 actual[128];
    if(!readMemory(source,actual,count))return false;
    return bytesEqual(actual,expected,count);
}

static bool isWritableCell(void* address){
    if(!address||((u64)address&3u)!=0u)return false;
    MEMORY_BASIC_INFORMATION_X64 info;
    memset(&info,0,sizeof(info));
    if(VirtualQuery(address,&info,sizeof(info))!=sizeof(info)||info.State!=MEM_COMMIT||
       (info.Protect&PAGE_GUARD))return false;
    DWORD protection=info.Protect&0xFFu;
    if(protection!=PAGE_READWRITE&&protection!=PAGE_EXECUTE_READWRITE)return false;
    u64 begin=(u64)address;
    u64 end=(u64)info.BaseAddress+info.RegionSize;
    return begin+4u>=begin&&begin+4u<=end;
}

static WCHAR lowerAscii(WCHAR value){
    return value>=L'A'&&value<=L'Z'?(WCHAR)(value+(L'a'-L'A')):value;
}

static void modulePath(WCHAR* out,const WCHAR* name){
    if(!out||!name)return;
    out[0]=0;
    DWORD length=GetModuleFileNameW(g_self,out,520u);
    if(!length||length>=520u){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<length;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1u;
    DWORD j=0;
    while(name[j]&&slash+j+1u<520u){out[slash+j]=name[j];j++;}
    if(name[j]){out[0]=0;return;}
    out[slash+j]=0;
}

static bool isDs2Process(HMODULE game){
    WCHAR path[520];
    DWORD length=GetModuleFileNameW(game,path,520u);
    if(!length||length>=520u)return false;
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
    modulePath(path,L"ds2_climbing_gloves_range.log");
    if(!path[0])return;
    g_log=CreateFileW(
        path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
    );
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Climbing Power Gloves Range v" MOD_VERSION "\r\n"
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

static void logCritical(const char* text,DWORD length){
    openLog();
    logRaw(text,length);
}

static void logDebug(const char* text,DWORD length){
    if(!g_debugLog)return;
    openLog();
    logRaw(text,length);
}

static void trimmedBounds(const WCHAR* value,u32* begin,u32* end){
    u32 length=0;
    while(value[length])length++;
    u32 first=0;
    while(first<length&&(value[first]==L' '||value[first]==L'\t'))first++;
    while(length>first&&(value[length-1u]==L' '||value[length-1u]==L'\t'))length--;
    *begin=first;
    *end=length;
}

static bool equalsToken(const WCHAR* value,u32 begin,u32 end,const WCHAR* token){
    u32 tokenLength=0;
    while(token[tokenLength])tokenLength++;
    if(tokenLength!=end-begin)return false;
    for(u32 i=0;i<tokenLength;i++){
        if(lowerAscii(value[begin+i])!=lowerAscii(token[i]))return false;
    }
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
    u32 digits=0;
    u32 position=begin;
    while(position<end&&value[position]>=L'0'&&value[position]<=L'9'){
        whole=whole*10u+(u64)(value[position]-L'0');
        if(whole>1000u)return false;
        digits++;
        position++;
    }
    if(!digits)return false;
    u32 fraction=0;
    u32 scale=1;
    if(position<end&&value[position]==L'.'){
        position++;
        u32 fractionDigits=0;
        while(position<end&&value[position]>=L'0'&&value[position]<=L'9'){
            if(fractionDigits>=3u)return false;
            fraction=fraction*10u+(u32)(value[position]-L'0');
            scale*=10u;
            fractionDigits++;
            position++;
        }
        if(!fractionDigits)return false;
    }
    if(position!=end)return false;
    double parsed=(double)whole+(double)fraction/(double)scale;
    if(parsed<(double)minimum||parsed>(double)maximum)return false;
    *out=(float)parsed;
    return true;
}

static bool readIniValue(const WCHAR* key,const WCHAR* fallback,WCHAR* out,DWORD capacity){
    if(!key||!fallback||!out||capacity<3u)return false;
    WCHAR path[520];
    modulePath(path,L"ds2_climbing_gloves_range.ini");
    if(!path[0])return false;
    DWORD length=GetPrivateProfileStringW(
        L"ClimbingGlovesRange",key,fallback,out,capacity,path
    );
    return length>0u&&length<capacity-1u;
}

static SettingsResult loadSettings(Settings* settings){
    if(!settings)return SETTINGS_INVALID;
    settings->enabled=true;
    settings->debugLog=false;
    settings->level1Range=DEFAULT_LEVEL1_RANGE;
    settings->level2Range=DEFAULT_LEVEL2_RANGE;
    WCHAR value[64];
    if(!readIniValue(L"Enabled",L"1",value,64u)||
       !parseBool(value,&settings->enabled))return SETTINGS_INVALID;
    if(!readIniValue(L"DebugLog",L"0",value,64u)||
       !parseBool(value,&settings->debugLog))return SETTINGS_INVALID;
    if(!readIniValue(L"Level1RangeMeters",L"30",value,64u)||
       !parseDecimal(value,NATIVE_LEVEL1_RANGE,MAX_CONFIGURED_RANGE,
                     &settings->level1Range))return SETTINGS_INVALID;
    if(!readIniValue(L"Level2RangeMeters",L"50",value,64u)||
       !parseDecimal(value,NATIVE_LEVEL2_RANGE,MAX_CONFIGURED_RANGE,
                     &settings->level2Range))return SETTINGS_INVALID;
    if(settings->level1Range>settings->level2Range)return SETTINGS_INVALID;
    settings->level1Bits=floatBits(settings->level1Range);
    settings->level2Bits=floatBits(settings->level2Range);
    g_debugLog=settings->debugLog;
    return settings->enabled?SETTINGS_READY:SETTINGS_DISABLED;
}

static bool readBuildMetadata(u8* base,GameImage* image){
    if(!base||!image)return false;
    u16 mz=0,magic=0;
    u32 peOffset=0,signature=0,timestamp=0,imageSize=0;
    if(!readU16(base,&mz)||mz!=0x5A4Du||!readU32(base+0x3Cu,&peOffset)||
       peOffset<0x40u||peOffset>0x1000u||
       !readU32(base+peOffset,&signature)||signature!=0x00004550u||
       !readU32(base+peOffset+8u,&timestamp)||
       !readU16(base+peOffset+24u,&magic)||magic!=0x020Bu||
       !readU32(base+peOffset+24u+56u,&imageSize))return false;
    image->base=base;
    image->timestamp=timestamp;
    image->imageSize=imageSize;
    return true;
}

static bool pointerInImage(const GameImage* image,u64 pointer){
    if(!image||!image->base)return false;
    u64 begin=(u64)image->base;
    u64 end=begin+(u64)image->imageSize;
    return pointer>=begin&&pointer<end;
}

static bool validateVtable(
    const GameImage* image,u64 tableRva,const u64* functionRvas,u32 count
){
    if(!image||!functionRvas||!count||tableRva+(u64)count*8u>image->imageSize)return false;
    for(u32 i=0;i<count;i++){
        u64 entry=0;
        if(!readU64(image->base+tableRva+(u64)i*8u,&entry)||
           entry!=(u64)(image->base+functionRvas[i])||!pointerInImage(image,entry))return false;
    }
    return true;
}

static bool validateStaticTargets(const GameImage* image){
    if(!image||!image->base||
       RVA_ITEM_SYSTEM_GLOBAL+8u>image->imageSize||
       RVA_ITEM_SYSTEM_INIT+sizeof(EXPECTED_ITEM_SYSTEM_INIT)>image->imageSize||
       RVA_ITEM_SYSTEM_CONSTRUCTOR+sizeof(EXPECTED_ITEM_SYSTEM_CONSTRUCTOR)>
           image->imageSize||
       RVA_ITEM_LOOKUP+sizeof(EXPECTED_ITEM_LOOKUP)>image->imageSize||
       RVA_ITEM_LOOKUP_CORE+sizeof(EXPECTED_ITEM_LOOKUP_CORE)>image->imageSize||
       RVA_ITEM_PARAMETER_DESCRIPTOR+0x48u>image->imageSize||
       RVA_ITEM_PARAMETER_TYPE_NAME+sizeof(EXPECTED_ITEM_PARAMETER_TYPE_NAME)>
           image->imageSize)return false;
    if(!memoryEquals(
           image->base+RVA_ITEM_SYSTEM_INIT,EXPECTED_ITEM_SYSTEM_INIT,
           (u32)sizeof(EXPECTED_ITEM_SYSTEM_INIT)
       )||!memoryEquals(
           image->base+RVA_ITEM_SYSTEM_CONSTRUCTOR,EXPECTED_ITEM_SYSTEM_CONSTRUCTOR,
           (u32)sizeof(EXPECTED_ITEM_SYSTEM_CONSTRUCTOR)
       )||!memoryEquals(
           image->base+RVA_ITEM_LOOKUP,EXPECTED_ITEM_LOOKUP,
           (u32)sizeof(EXPECTED_ITEM_LOOKUP)
       )||!memoryEquals(
           image->base+RVA_ITEM_LOOKUP_CORE,EXPECTED_ITEM_LOOKUP_CORE,
           (u32)sizeof(EXPECTED_ITEM_LOOKUP_CORE)
       )||!memoryEquals(
           image->base+RVA_ITEM_PARAMETER_DESCRIPTOR,
           EXPECTED_ITEM_PARAMETER_DESCRIPTOR,
           (u32)sizeof(EXPECTED_ITEM_PARAMETER_DESCRIPTOR)
       )||!memoryEquals(
           image->base+RVA_ITEM_PARAMETER_TYPE_NAME,
           (const u8*)EXPECTED_ITEM_PARAMETER_TYPE_NAME,
           (u32)sizeof(EXPECTED_ITEM_PARAMETER_TYPE_NAME)
       )||!validateVtable(image,RVA_ITEM_SYSTEM_VTABLE,ITEM_SYSTEM_VFUNCS,2u)||
       !validateVtable(image,RVA_ITEM_PARAMETER_VTABLE,ITEM_PARAMETER_VFUNCS,4u))return false;
    u64 factory=0,destructor=0,typeName=0;
    if(!readU64(image->base+RVA_ITEM_PARAMETER_DESCRIPTOR+0x18u,&factory)||
       factory!=(u64)(image->base+RVA_ITEM_PARAMETER_FACTORY)||
       !readU64(image->base+RVA_ITEM_PARAMETER_DESCRIPTOR+0x20u,&destructor)||
       destructor!=(u64)(image->base+RVA_ITEM_PARAMETER_DESTRUCTOR_THUNK)||
       !readU64(image->base+RVA_ITEM_PARAMETER_DESCRIPTOR+0x40u,&typeName)||
       typeName!=(u64)(image->base+RVA_ITEM_PARAMETER_TYPE_NAME))return false;
    return true;
}

static bool hasExactVtable(const GameImage* image,const u8* object,u64 vtableRva){
    if(!image||!object)return false;
    u64 vtable=0;
    return readU64(object,&vtable)&&vtable==(u64)(image->base+vtableRva);
}

static ResolveResult resolveItems(const GameImage* image,ResolvedItems* resolved){
    if(!image||!resolved)return RESOLVE_INVALID;
    memset(resolved,0,sizeof(*resolved));
    u64 systemPointer=0;
    if(!readU64(image->base+RVA_ITEM_SYSTEM_GLOBAL,&systemPointer))return RESOLVE_INVALID;
    if(!systemPointer)return RESOLVE_NOT_READY;
    u8* system=(u8*)systemPointer;
    if(!hasExactVtable(image,system,RVA_ITEM_SYSTEM_VTABLE))return RESOLVE_INVALID;
    RawArray array;
    if(!readMemory(system+OFF_ITEM_SYSTEM_ARRAY,&array,sizeof(array)))return RESOLVE_INVALID;
    if(array.count>ITEM_SYSTEM_MAX_COUNT||array.capacity<array.count||
       array.capacity>ITEM_SYSTEM_MAX_COUNT)return RESOLVE_INVALID;
    if(!array.count||!array.entries)return RESOLVE_NOT_READY;
    if(!readMemory((void*)array.entries,&systemPointer,8u))return RESOLVE_NOT_READY;
    u32 level1Matches=0,level2Matches=0;
    for(u32 i=0;i<array.count;i++){
        u64 objectPointer=0;
        if(!readU64((void*)(array.entries+(u64)i*8u),&objectPointer)||!objectPointer){
            return RESOLVE_NOT_READY;
        }
        u8 id=0;
        if(!readU8((u8*)objectPointer+OFF_ITEM_ID,&id))return RESOLVE_NOT_READY;
        if(id!=ITEM_ID_LEVEL1&&id!=ITEM_ID_LEVEL2)continue;
        if(!hasExactVtable(image,(u8*)objectPointer,RVA_ITEM_PARAMETER_VTABLE)){
            return RESOLVE_INVALID;
        }
        if(id==ITEM_ID_LEVEL1){
            resolved->level1=(u8*)objectPointer;
            level1Matches++;
        }else{
            resolved->level2=(u8*)objectPointer;
            level2Matches++;
        }
    }
    if(!level1Matches||!level2Matches)return RESOLVE_NOT_READY;
    if(level1Matches!=1u||level2Matches!=1u)return RESOLVE_INVALID;
    resolved->system=system;
    return RESOLVE_READY;
}

static void nativeParams(u8 id,u32* values){
    if(id==ITEM_ID_LEVEL1){
        values[0]=floatBits(3.0f);values[1]=floatBits(3.0f);
        values[2]=floatBits(3.0f);values[3]=floatBits(10.0f);
        values[4]=floatBits(10.0f);values[5]=floatBits(10.0f);
        values[6]=floatBits(NATIVE_LEVEL1_RANGE);values[7]=floatBits(100.0f);
    }else{
        values[0]=floatBits(1.5f);values[1]=floatBits(1.5f);
        values[2]=floatBits(1.5f);values[3]=floatBits(5.0f);
        values[4]=floatBits(5.0f);values[5]=floatBits(5.0f);
        values[6]=floatBits(NATIVE_LEVEL2_RANGE);values[7]=floatBits(120.0f);
    }
}

static InspectResult inspectGlove(
    const GameImage* image,u8* object,u8 expectedId,u8 expectedLevel,u32 targetBits,
    GloveView* view
){
    if(!image||!object||!view||
       !hasExactVtable(image,object,RVA_ITEM_PARAMETER_VTABLE))return INSPECT_INVALID;
    u8 id=0,level=0,category=0,subcategory=0;
    if(!readU8(object+OFF_ITEM_ID,&id)||!readU8(object+OFF_ITEM_LEVEL,&level)||
       !readU8(object+OFF_ITEM_CATEGORY,&category)||
       !readU8(object+OFF_ITEM_SUBCATEGORY,&subcategory)||id!=expectedId||
       level!=expectedLevel||category!=ITEM_CATEGORY_GLOVE||
       subcategory!=ITEM_SUBCATEGORY_CLIMBING_POWER_GLOVE)return INSPECT_INVALID;
    RawArray params;
    if(!readMemory(object+OFF_ITEM_PARAMS,&params,sizeof(params))||
       params.count!=ITEM_PARAM_COUNT||params.capacity<ITEM_PARAM_COUNT||
       params.capacity>32u||!params.entries)return INSPECT_INVALID;
    u32 values[ITEM_PARAM_COUNT];
    if(!readMemory((void*)params.entries,values,sizeof(values)))return INSPECT_INVALID;
    u32 expected[ITEM_PARAM_COUNT];
    nativeParams(expectedId,expected);
    for(u32 i=0;i<ITEM_PARAM_COUNT;i++){
        if(i!=6u&&values[i]!=expected[i])return INSPECT_INVALID;
    }
    view->object=object;
    view->rangeCell=(u8*)params.entries+6u*4u;
    view->nativeBits=expected[6];
    view->targetBits=targetBits;
    if(values[6]==targetBits)return INSPECT_TARGET;
    if(values[6]==expected[6])return INSPECT_NATIVE;
    return INSPECT_CONFLICT;
}

static ExchangeResult exchangeBits(u8* cell,u32 target,u32 expected){
    if(!isWritableCell(cell))return EXCHANGE_FAILED;
    long observed=_InterlockedCompareExchange(
        (volatile long*)cell,(long)target,(long)expected
    );
    if((u32)observed==expected)return EXCHANGE_CHANGED;
    if((u32)observed==target)return EXCHANGE_ALREADY_TARGET;
    return EXCHANGE_CONFLICT;
}

static bool rollbackChanged(const GloveView* view,bool changed){
    if(!changed)return true;
    if(!view||!isWritableCell(view->rangeCell))return false;
    long observed=_InterlockedCompareExchange(
        (volatile long*)view->rangeCell,(long)view->nativeBits,(long)view->targetBits
    );
    return (u32)observed==view->targetBits||(u32)observed==view->nativeBits;
}

static ApplyResult reconcileGloves(
    const GameImage* image,const Settings* settings,ResolvedItems* resolvedOut
){
    if(!image||!settings)return APPLY_INVALID;
    ResolvedItems resolved;
    ResolveResult resolveResult=resolveItems(image,&resolved);
    if(resolveResult==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(resolveResult!=RESOLVE_READY)return APPLY_INVALID;
    if(resolvedOut)*resolvedOut=resolved;
    GloveView level1,level2;
    InspectResult state1=inspectGlove(
        image,resolved.level1,ITEM_ID_LEVEL1,0u,settings->level1Bits,&level1
    );
    InspectResult state2=inspectGlove(
        image,resolved.level2,ITEM_ID_LEVEL2,1u,settings->level2Bits,&level2
    );
    if(state1==INSPECT_INVALID||state2==INSPECT_INVALID)return APPLY_INVALID;
    if(state1==INSPECT_CONFLICT||state2==INSPECT_CONFLICT)return APPLY_CONFLICT;
    if(state1==INSPECT_TARGET&&state2==INSPECT_TARGET)return APPLY_READY;
    bool changed1=false,changed2=false;
    if(state1==INSPECT_NATIVE&&level1.nativeBits!=level1.targetBits){
        ExchangeResult exchange=exchangeBits(
            level1.rangeCell,level1.targetBits,level1.nativeBits
        );
        if(exchange==EXCHANGE_CHANGED)changed1=true;
        else if(exchange==EXCHANGE_CONFLICT)return APPLY_CONFLICT;
        else if(exchange!=EXCHANGE_ALREADY_TARGET)return APPLY_FAILED;
    }
    if(state2==INSPECT_NATIVE&&level2.nativeBits!=level2.targetBits){
        ExchangeResult exchange=exchangeBits(
            level2.rangeCell,level2.targetBits,level2.nativeBits
        );
        if(exchange==EXCHANGE_CHANGED)changed2=true;
        else if(exchange!=EXCHANGE_ALREADY_TARGET){
            if(!rollbackChanged(&level1,changed1))return APPLY_CRITICAL;
            return exchange==EXCHANGE_CONFLICT?APPLY_CONFLICT:APPLY_FAILED;
        }
    }
    GloveView verified1,verified2;
    InspectResult verify1=inspectGlove(
        image,resolved.level1,ITEM_ID_LEVEL1,0u,settings->level1Bits,&verified1
    );
    InspectResult verify2=inspectGlove(
        image,resolved.level2,ITEM_ID_LEVEL2,1u,settings->level2Bits,&verified2
    );
    if(verify1==INSPECT_TARGET&&verify2==INSPECT_TARGET)return APPLY_READY;
    bool rollback2=rollbackChanged(&level2,changed2);
    bool rollback1=rollbackChanged(&level1,changed1);
    if(!rollback1||!rollback2)return APPLY_CRITICAL;
    return verify1==INSPECT_CONFLICT||verify2==INSPECT_CONFLICT?
           APPLY_CONFLICT:APPLY_FAILED;
}

#ifndef CLIMBING_GLOVES_TEST_BUILD
static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    openLog();
    Settings settings;
    SettingsResult settingsResult=loadSettings(&settings);
    if(settingsResult==SETTINGS_INVALID){
        const char message[]=
            "ERROR: invalid INI. Use 8-100 m for Level 1, 10-100 m for Level 2, and keep Level 1 <= Level 2. No patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    if(settingsResult==SETTINGS_DISABLED){
        const char message[]="DISABLED: Enabled=0; no patch applied.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    const char configMessage[]=
        "CONFIG: tier-specific range values accepted from ds2_climbing_gloves_range.ini.\r\n";
    logRaw(configMessage,(DWORD)(sizeof(configMessage)-1u));
    GameImage image;
    if(!readBuildMetadata((u8*)game,&image)){
        const char message[]="ERROR: could not read DS2.exe metadata; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    if(image.timestamp!=EXPECTED_TIMESTAMP||image.imageSize!=EXPECTED_IMAGE_SIZE){
        const char message[]="ERROR: unsupported DS2.exe build; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    if(!validateStaticTargets(&image)){
        const char message[]=
            "ERROR: DSItemSystem signatures, RTTI, or VTables do not match; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }
    const char ready[]=
        "READY: validated hook-free DSItemSystem resolver; waiting for both glove tiers.\r\n";
    logRaw(ready,(DWORD)(sizeof(ready)-1u));
    bool waitingLogged=false,invalidLogged=false,conflictLogged=false;
    bool failureLogged=false,activeLogged=false;
    for(;;){
        ApplyResult result=reconcileGloves(&image,&settings,0);
        if(result==APPLY_READY){
            if(!activeLogged){
                const char message[]=
                    "ACTIVE: Level 1 and Level 2 Params[6] match their configured pickup ranges; all other item parameters are unchanged.\r\n";
                logRaw(message,(DWORD)(sizeof(message)-1u));
                activeLogged=true;
            }
            waitingLogged=false;
            invalidLogged=false;
            conflictLogged=false;
            failureLogged=false;
        }else if(result==APPLY_NOT_READY){
            activeLogged=false;
            if(!waitingLogged){
                const char message[]="STATE: DSItemSystem or glove resources are not loaded yet; waiting.\r\n";
                logDebug(message,(DWORD)(sizeof(message)-1u));
                waitingLogged=true;
            }
        }else if(result==APPLY_CONFLICT){
            activeLogged=false;
            if(!conflictLogged){
                const char message[]=
                    "ERROR: another mod changed a climbing-glove range cell; neither tier was overwritten.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                conflictLogged=true;
            }
        }else if(result==APPLY_INVALID){
            activeLogged=false;
            if(!invalidLogged){
                const char message[]=
                    "ERROR: runtime item layout or native glove tuple is invalid; writes are paused.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                invalidLogged=true;
            }
        }else if(result==APPLY_CRITICAL){
            const char message[]=
                "CRITICAL: a transactional rollback failed; stop DS2 before continuing.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            break;
        }else{
            activeLogged=false;
            if(!failureLogged){
                const char message[]=
                    "ERROR: range write failed and was rolled back; the mod will retry.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                failureLogged=true;
            }
        }
        Sleep(1000u);
    }
    closeLog();
    return 0;
}
#endif

extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){
        g_self=module;
        DisableThreadLibraryCalls(module);
#ifndef CLIMBING_GLOVES_TEST_BUILD
        if(_InterlockedCompareExchange(&g_started,1,0)==0){
            HANDLE thread=CreateThread(0,0,&worker,0,0,0);
            if(thread)CloseHandle(thread);
            else _InterlockedExchange(&g_started,0);
        }
#endif
    }
    return TRUE;
}
