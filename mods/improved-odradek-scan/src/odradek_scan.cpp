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
extern "C" long _InterlockedIncrement(volatile long*);
extern "C" char _InterlockedExchange8(volatile char*,char);
extern "C" long long _InterlockedCompareExchange64(volatile long long*,long long,long long);
extern "C" long long _InterlockedExchange64(volatile long long*,long long);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedExchange8)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchange64)

#define MOD_VERSION "1.0.1"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u

static const u64 RVA_SURVEY_FACTORY=0x01D73FA0ull;
static const u64 RVA_SURVEY_CONSTRUCTOR=0x01D89140ull;
static const u64 RVA_SURVEY_VTABLE=0x0338E748ull;
static const u64 RVA_SURVEY_FACTORY_SLOT=0x0449AF08ull;
static const u64 RVA_GAMEPLAY_SCAN_START=0x00DE2420ull;
static const u64 RVA_GAMEPLAY_SCAN_SLOT=0x0323B560ull;
static const u64 RVA_SENSOR_GATE_RADIUS_LOAD=0x00DE6FDDull;
static const u64 RVA_SENSOR_GATE=0x00DE6E40ull;
static const u64 RVA_SENSOR_GATE_CALL_WINDOW=0x01CE5AD0ull;
static const u64 RVA_SENSOR_GATE_CALL=0x01CE5AD1ull;
static const u64 RVA_BODY_SECONDARY_VTABLE=0x0323B530ull;
static const u64 RVA_SENSOR_VTABLE=0x0323B190ull;
static const u64 RVA_SENSOR_MANAGER_VTABLE=0x0323AF90ull;
static const u64 RVA_EFFECT_INSTANCE_VTABLE=0x0338E2C8ull;
static const u64 RVA_EFFECT_RESOURCE_VTABLE=0x0338E5C8ull;
static const u64 RVA_SENSOR_MANAGER_GLOBAL=0x0623E9F0ull;
static const u64 RVA_UI_CACHE_ROOT=0x0623EBF8ull;
static const u64 RVA_UI_RESIDENT_VTABLE=0x032BAE58ull;
static const u64 RVA_UI_GAME_PARAMS_VTABLE=0x032BAB60ull;
static const u64 RVA_UI_CACHE_LOAD=0x017C57E6ull;
static const u64 RVA_UI_SEARCHED_DISTANCE_LOAD=0x017C5818ull;
static const u64 RVA_BAGGAGE_MANAGER_GLOBAL=0x0623EA48ull;
static const u64 RVA_BAGGAGE_MANAGER_VTABLE=0x0327CA78ull;
static const u64 RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE=0x0327C570ull;
static const u64 RVA_BAGGAGE_CONFIG_VTABLE=0x0327CBE0ull;
static const u64 RVA_BAGGAGE_CONFIG_LOAD=0x011C564Eull;
static const u64 RVA_BAGGAGE_CONFIG_CACHE_STORES=0x011C58A1ull;
static const u64 RVA_BAGGAGE_MARKER_UNTOUCHED_LOAD=0x011BC6A3ull;
static const u64 RVA_BAGGAGE_MARKER_TOUCHED_LOAD=0x011BC692ull;
static const u64 RVA_BAGGAGE_MARKER_MISSION_LOAD=0x011BC64Bull;
static const u64 RVA_BAGGAGE_FOUND_DISTANCE_LOAD=0x011AF4BCull;
static const u64 RVA_PLAYER_RESOURCE_ROOT=0x0623DF40ull;
static const u64 RVA_PLAYER_RESOURCE_ROOT_LOAD=0x00D845B6ull;
static const u64 RVA_PLAYER_RESOURCE_ENUMERATION=0x00D845C5ull;
static const u64 RVA_PLAYER_ODRADEK_CONTEXT_PATH=0x00D845F9ull;
static const u64 RVA_PLAYER_ODRADEK_CONFIG_GETTER=0x00DA07A0ull;
static const u64 RVA_PLAYER_ODRADEK_CONFIG_VTABLE=0x032459A8ull;
static const u64 RVA_DETECTED_CONSTANT_UPDATE_CORE=0x01CE57C0ull;
static const u64 RVA_DETECTED_CONSTANT_UPDATE_THUNK=0x01CF5A40ull;
static const u64 RVA_DETECTED_CONSTANT_UPDATE_MESSAGE=0x041CEA30ull;
static const u64 RVA_DETECTED_CONSTANT_UPDATE_MESSAGE_SLOT=0x04490180ull;
static const u64 RVA_DETECTED_CONSTANT_UPDATE_HANDLER_SLOT=0x04490188ull;
static const u64 RVA_DETECTED_COMPONENT_VTABLE=0x033855D8ull;
static const u64 RVA_FIND_ENTITY_COMPONENT=0x0011FFA0ull;
static const u64 RVA_BAGGAGE_COMPONENT_RTTI=0x0436C9F0ull;
static const u64 RVA_BAGGAGE_COMPONENT_VTABLE=0x0327C440ull;
static const u64 RVA_BAGGAGE_SYMBOLS_RTTI=0x0436C1B0ull;
static const u64 RVA_BAGGAGE_SYMBOLS_VTABLE=0x0327C738ull;
static const u64 RVA_BAGGAGE_SYMBOLS_SECONDARY_VTABLE=0x0327CB20ull;
static const u64 RVA_BAGGAGE_SYMBOLS_EVENT_CORE=0x011FC160ull;
[[maybe_unused]] static const u64 RVA_BAGGAGE_SYMBOLS_EVENT_THUNK=0x011FF980ull;
[[maybe_unused]] static const u64 RVA_BAGGAGE_SYMBOLS_EVENT_MESSAGE=0x0448B280ull;
[[maybe_unused]] static const u64 RVA_BAGGAGE_SYMBOLS_EVENT_MESSAGE_SLOT=0x0436C040ull;
[[maybe_unused]] static const u64 RVA_BAGGAGE_SYMBOLS_EVENT_HANDLER_SLOT=0x0436C048ull;
static const u64 RVA_REALIZED_BAGGAGE_VTABLE=0x0327CF28ull;
static const u64 RVA_REALIZED_FOUND_BRIDGE=0x011BC740ull;
static const u64 RVA_REALIZED_MARKER_BRIDGE=0x011BC520ull;
static const u64 RVA_REALIZED_BAGGAGE_INIT_CORE=0x011B4C90ull;
static const u64 RVA_REALIZED_BAGGAGE_INIT_THUNK=0x01200190ull;
static const u64 RVA_REALIZED_BAGGAGE_INIT_MESSAGE=0x041D5220ull;
static const u64 RVA_REALIZED_BAGGAGE_INIT_MESSAGE_SLOT=0x0436BD90ull;
static const u64 RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT=0x0436BD98ull;
static const u64 RVA_REALIZED_BAGGAGE_UNLINK_CORE=0x011B5460ull;
static const u64 RVA_REALIZED_BAGGAGE_UNLINK_THUNK=0x01200160ull;
static const u64 RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE=0x041D4040ull;
static const u64 RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT=0x0436BDC0ull;
static const u64 RVA_REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT=0x0436BDC8ull;
static const u64 RVA_REALIZED_BAGGAGE_UPDATE_CORE=0x011BB340ull;
static const u64 RVA_REALIZED_BAGGAGE_UPDATE_THUNK=0x01200170ull;
static const u64 RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE=0x041D45C0ull;
static const u64 RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT=0x0436BDB0ull;
static const u64 RVA_REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT=0x0436BDB8ull;
static const u64 RVA_ENTITY_MANAGER_GAME_UPDATE=0x006FD510ull;
static const u64 RVA_ENTITY_MANAGER_GAME_VTABLE=0x0318A498ull;
static const u64 RVA_ENTITY_MANAGER_GAME_UPDATE_SLOT=0x0318A4A8ull;
static const u64 RVA_ENTITY_MANAGER_GAME_COL=0x034FB058ull;
static const u64 RVA_ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR=0x060E80D0ull;
static const u64 RVA_ENTITY_REQUEST_REALIZE=0x001312B0ull;
static const u64 RVA_REALIZATION_COORDINATOR=0x00171D90ull;
static const u64 RVA_REALIZATION_COORDINATOR_CALL_WINDOW=0x001826C8ull;
static const u64 RVA_REALIZATION_COORDINATOR_CALL=0x001826CAull;

static const u64 SURVEY_VFUNCS[4]={0x01D73F20ull,0x01D89730ull,0x00109DE0ull,0x00109EB0ull};
static const u64 BODY_SECONDARY_VFUNCS[4]={0x00DEBB10ull,0x00DD7AE0ull,0x00DCFD10ull,0x00DCFDF0ull};
static const u64 SENSOR_VFUNCS[4]={0x00DD7BF0ull,0x00DD7C00ull,0x00109DE0ull,0x00109EB0ull};
static const u64 SENSOR_MANAGER_VFUNCS[4]={0x00DD7BD0ull,0x00DE6550ull,0x03596660ull,0x00DE7A10ull};
static const u64 EFFECT_INSTANCE_VFUNCS[4]={0x01D70110ull,0x01D70400ull,0x00109DE0ull,0x00109EB0ull};
static const u64 EFFECT_RESOURCE_VFUNCS[4]={0x01D70120ull,0x01D896E0ull,0x00109DE0ull,0x00109EB0ull};
static const u64 UI_RESIDENT_VFUNCS[4]={0x01482140ull,0x01489AD0ull,0x00109DE0ull,0x00109EB0ull};
static const u64 UI_GAME_PARAMS_VFUNCS[4]={0x01482130ull,0x01489200ull,0x00109DE0ull,0x00109EB0ull};
static const u64 BAGGAGE_MANAGER_VFUNCS[4]={
    0x011AEE20ull,0x011C2C90ull,0x035B7250ull,0x00DCCB80ull
};
static const u64 BAGGAGE_MANAGER_SECONDARY_VFUNCS[4]={
    0x0120B9ECull,0x000A10E0ull,0x000A10E0ull,0x000A10E0ull
};
static const u64 BAGGAGE_CONFIG_VFUNCS[4]={
    0x011AE5F0ull,0x011AEBC0ull,0x00109DE0ull,0x00109EB0ull
};
static const u64 PLAYER_ODRADEK_CONFIG_VFUNCS[4]={
    0x00E3EE20ull,0x00E60430ull,0x00109DE0ull,0x00109EB0ull
};
static const u64 DETECTED_COMPONENT_VFUNCS[4]={
    0x01CE53E0ull,0x01CE53F0ull,0x00109DE0ull,0x00109EB0ull
};
static const u64 BAGGAGE_COMPONENT_VFUNCS[4]={
    0x011AF150ull,0x011AF1C0ull,0x00109DE0ull,0x00109EB0ull
};
[[maybe_unused]] static const u64 BAGGAGE_SYMBOLS_VFUNCS[4]={
    0x011AF400ull,0x011AF410ull,0x00109DE0ull,0x00109EB0ull
};
[[maybe_unused]] static const u64 BAGGAGE_SYMBOLS_SECONDARY_VFUNCS[4]={
    0x0120B998ull,0x0120B980ull,0x035B71E8ull,0x00157770ull
};
static const u64 REALIZED_BAGGAGE_VFUNCS[4]={
    0x011AF220ull,0x011AF340ull,0x00109DE0ull,0x00109EB0ull
};
static const u64 ENTITY_MANAGER_GAME_VFUNCS[4]={
    0x00174B20ull,0x00174FB0ull,0x006FD510ull,0x006FD570ull
};

static const u8 EXPECTED_SURVEY_FACTORY[16]={
    0x48,0x85,0xD2,0x74,0x08,0x48,0x8B,0xCA,0xE9,0x93,0x51,0x01,0x00,0xC3,0xCC,0xCC
};
static const u8 EXPECTED_SURVEY_CONSTRUCTOR[21]={
    0x33,0xD2,0x48,0x8D,0x05,0xFF,0x55,0x60,0x01,0x48,0x89,0x51,0x10,0x48,0x89,0x51,
    0x18,0x48,0x89,0x51,0x50
};
static const u8 EXPECTED_GAMEPLAY_SCAN_START[26]={
    0x48,0x83,0xEC,0x48,0x48,0x8B,0x81,0xA0,0x00,0x00,0x00,0x48,0x8B,
    0x89,0xA8,0x00,0x00,0x00,0xC5,0xFB,0x10,0x80,0x80,0x02,0x00,0x00
};
static const u8 EXPECTED_SENSOR_GATE_RADIUS_LOAD[8]={
    0xC5,0xFA,0x10,0x83,0x98,0x00,0x00,0x00
};
static const u8 EXPECTED_SENSOR_GATE_CALL_WINDOW[8]={
    0x00,0xE8,0x6A,0x13,0x10,0xFF,0x84,0xC0
};
static const u8 EXPECTED_UI_SEARCHED_DISTANCE_LOAD[9]={
    0x48,0x8B,0x48,0x50,0xC5,0xFA,0x10,0x41,0x30
};
static const u8 EXPECTED_UI_CACHE_LOAD[7]={
    0x48,0x8B,0x1D,0x0B,0x94,0xA7,0x04
};
static const u8 EXPECTED_BAGGAGE_CONFIG_LOAD[7]={
    0x48,0x8B,0x3D,0xF3,0x93,0x07,0x05
};
static const u8 EXPECTED_BAGGAGE_CONFIG_CACHE_STORES[51]={
    0xC5,0xFA,0x10,0x46,0x30,0xC5,0xFA,0x59,0xC8,0xC5,0xFA,0x11,0x8F,0x38,0x66,0x03,
    0x00,0xC5,0xFA,0x10,0x56,0x34,0xC5,0xEA,0x59,0xC2,0xC5,0xFA,0x11,0x87,0x3C,0x66,
    0x03,0x00,0xC5,0xFA,0x10,0x4E,0x38,0xC5,0xF2,0x59,0xD1,0xC5,0xFA,0x11,0x97,0x40,
    0x66,0x03,0x00
};
static const u8 EXPECTED_BAGGAGE_MARKER_UNTOUCHED_LOAD[24]={
    0xC5,0xFA,0x10,0x80,0x38,0x66,0x03,0x00,0xC5,0xF8,0x2F,0x86,0xF8,0x01,0x00,0x00,
    0x0F,0x93,0xC0,0x84,0xC0,0x74,0x6C,0x8B
};
static const u8 EXPECTED_BAGGAGE_MARKER_TOUCHED_LOAD[8]={
    0xC5,0xFA,0x10,0x80,0x3C,0x66,0x03,0x00
};
static const u8 EXPECTED_BAGGAGE_MARKER_MISSION_LOAD[8]={
    0xC5,0xFA,0x10,0x80,0x40,0x66,0x03,0x00
};
static const u8 EXPECTED_BAGGAGE_FOUND_DISTANCE_LOAD[5]={
    0xC5,0xFA,0x10,0x41,0x48
};
static const u8 EXPECTED_PLAYER_RESOURCE_ROOT_LOAD[7]={
    0x48,0x8B,0x05,0x83,0x99,0x4B,0x05
};
static const u8 EXPECTED_PLAYER_RESOURCE_ENUMERATION[35]={
    0x48,0x8B,0x50,0x40,0x48,0x63,0x40,0x38,0x4C,0x8D,0x04,0xC2,
    0x49,0x3B,0xD0,0x74,0x12,0x48,0x8B,0x02,0x83,0x78,0x70,0x00,
    0x74,0x1A,0x48,0x83,0xC2,0x08,0x49,0x3B,0xD0,0x75,0xEE
};
static const u8 EXPECTED_PLAYER_ODRADEK_CONTEXT_PATH[29]={
    0x48,0x85,0xC0,0x74,0xEA,0x48,0x8B,0x48,0x48,0x48,0x85,0xC9,
    0x74,0xE1,0x48,0x8B,0x89,0xC8,0x56,0x00,0x00,0xE8,0x8D,0xC1,
    0x01,0x00,0x8B,0x48,0x68
};
static const u8 EXPECTED_PLAYER_ODRADEK_CONFIG_GETTER[16]={
    0x48,0x83,0xEC,0x28,0x48,0x8B,0x41,0x30,
    0x48,0x8B,0x40,0x28,0x48,0x85,0xC0,0x75
};
static const u8 EXPECTED_DETECTED_CONSTANT_UPDATE_THUNK[8]={
    0xE9,0x7B,0xFD,0xFE,0xFF,0xCC,0xCC,0xCC
};
static const u8 EXPECTED_DETECTED_CONSTANT_UPDATE_CORE[16]={
    0x40,0x55,0x57,0x41,0x54,0x41,0x56,0x41,
    0x57,0x48,0x8D,0x6C,0x24,0xC9,0x48,0x81
};
static const u8 EXPECTED_FIND_ENTITY_COMPONENT[16]={
    0x48,0x89,0x5C,0x24,0x08,0x4C,0x8B,0x49,
    0x18,0x4C,0x8B,0xD9,0x4C,0x63,0x41,0x10
};
static const u8 EXPECTED_BAGGAGE_SYMBOLS_RTTI[24]={
    0xFF,0xFF,0xFF,0xFF,0x04,0x00,0x01,0x04,
    0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
    0x80,0x00,0x00,0x00,0x08,0x00,0x00,0x00
};
[[maybe_unused]] static const u8 EXPECTED_BAGGAGE_SYMBOLS_EVENT_CORE[24]={
    0x48,0x89,0x5C,0x24,0x18,0x55,0x48,0x8B,
    0xEC,0x48,0x83,0xEC,0x70,0x48,0x8B,0x05,
    0xD4,0x28,0x04,0x05,0x48,0x8B,0xD9,0x4C
};
[[maybe_unused]] static const u8 EXPECTED_BAGGAGE_SYMBOLS_EVENT_THUNK[8]={
    0xE9,0xDB,0xC7,0xFF,0xFF,0xCC,0xCC,0xCC
};
[[maybe_unused]] static const u8 EXPECTED_BAGGAGE_SYMBOLS_EVENT_MESSAGE[24]={
    0xFF,0xFF,0xFF,0xFF,0x04,0x00,0x01,0x00,
    0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
    0xB0,0x00,0x00,0x00,0x10,0x00,0x00,0x00
};
static const u8 EXPECTED_REALIZED_FOUND_BRIDGE[16]={
    0x4C,0x8B,0xDC,0x49,0x89,0x5B,0x18,0x49,
    0x89,0x73,0x20,0x41,0x56,0x48,0x81,0xEC
};
static const u8 EXPECTED_REALIZED_MARKER_BRIDGE[16]={
    0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,
    0x24,0x18,0x57,0x48,0x83,0xEC,0x20,0x8B
};
static const u8 EXPECTED_REALIZED_BAGGAGE_INIT_THUNK[8]={
    0xE9,0xFB,0x4A,0xFB,0xFF,0xCC,0xCC,0xCC
};
static const u8 EXPECTED_REALIZED_BAGGAGE_INIT_CORE[24]={
    0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,
    0x24,0x18,0x48,0x89,0x7C,0x24,0x20,0x41,
    0x54,0x41,0x56,0x41,0x57,0x48,0x83,0xEC
};
static const u8 EXPECTED_REALIZED_BAGGAGE_UNLINK_THUNK[8]={
    0xE9,0xFB,0x52,0xFB,0xFF,0xCC,0xCC,0xCC
};
static const u8 EXPECTED_REALIZED_BAGGAGE_UNLINK_CORE[16]={
    0x48,0x89,0x5C,0x24,0x18,0x48,0x89,0x74,
    0x24,0x20,0x41,0x56,0x48,0x83,0xEC,0x20
};
static const u8 EXPECTED_REALIZED_BAGGAGE_UPDATE_THUNK[8]={
    0xE9,0xCB,0xB1,0xFB,0xFF,0xCC,0xCC,0xCC
};
static const u8 EXPECTED_REALIZED_BAGGAGE_UPDATE_CORE[16]={
    0x4C,0x8B,0xDC,0x57,0x41,0x56,0x48,0x81,
    0xEC,0x38,0x02,0x00,0x00,0x4C,0x8B,0x71
};
static const u8 EXPECTED_ENTITY_MANAGER_GAME_UPDATE[24]={
    0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,
    0xEC,0x20,0x48,0x8B,0xFA,0x48,0x8B,0xD9,
    0xE8,0xDB,0xB0,0xA7,0xFF,0x80,0x3F,0x02
};
static const u8 EXPECTED_ENTITY_REQUEST_REALIZE[24]={
    0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,
    0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48,
    0x8B,0x81,0x98,0x00,0x00,0x00,0x0F,0xB6
};
static const u8 EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW[8]={
    0x8B,0xCB,0xE8,0xC1,0xF6,0xFE,0xFF,0x48
};
static const char EXPECTED_ENTITY_MANAGER_GAME_TYPE_NAME[]=
    ".?AVEntityManagerGame@@";

static const u32 OFF_SURVEY_SHAPE=0x160u;
static const u32 OFF_SURVEY_ANGLE=0x164u;
static const u32 OFF_SURVEY_RANGE=0x1D8u;
static const u32 OFF_SURVEY_BASE_TIME=0x1E0u;
static const u32 OFF_SURVEY_FADE_TIME=0x1E4u;
static const u32 OFF_SURVEY_CIRCLE_TIME=0x1E8u;
static const u32 OFF_SURVEY_SIZE_FADE=0x1ECu;
static const u32 OFF_SURVEY_NUM_GRID=0x214u;

static const u32 OFF_BODY_SENSOR=0xA8u;
static const u32 OFF_SENSOR_EFFECT_INSTANCE=0x50u;
static const u32 OFF_EFFECT_RESOURCE=0xC0u;
static const u32 OFF_EFFECT_FULL_ANGLE=0x27Cu;
static const u32 OFF_EFFECT_SIZE_OVERRIDE=0x2A0u;
static const u32 OFF_EFFECT_FLAGS=0x2B4u;
static const u32 OFF_EFFECT_RESOURCE_SIZE=0x118u;
static const u32 OFF_EFFECT_RESOURCE_ADDITIONAL=0xF0u;
static const u32 OFF_ADDITIONAL_SIZE=0x40u;
static const u32 OFF_MANAGER_ACTIVE=0x28u;
static const u32 OFF_MANAGER_FULL_ANGLE=0x94u;
static const u32 OFF_MANAGER_GATE_RADIUS=0x98u;
static const u32 OFF_MANAGER_CURRENT_GENERATION=0xA4u;
static const u32 OFF_MANAGER_SCAN_SNAPSHOT=0xB0u;
static const u32 OFF_MANAGER_SNAPSHOT_GENERATION=0xE8u;
static const u32 OFF_UI_CACHE_RESIDENT=0xD0u;
static const u32 OFF_UI_RESIDENT_GAME_PARAMS=0x50u;
static const u32 OFF_UI_SEARCHED_VISIBLE_DISTANCE=0x30u;
static const u32 OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE=0x48u;
static const u32 OFF_BAGGAGE_MANAGER_CONFIG=0x10u;
static const u32 OFF_BAGGAGE_CONFIG_REALIZE_DISTANCE=0x28u;
static const u32 OFF_BAGGAGE_CONFIG_UNREALIZE_DISTANCE=0x2Cu;
static const u32 OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED=0x36638u;
static const u32 OFF_BAGGAGE_TOUCHED_CACHE_SQUARED=0x3663Cu;
static const u32 OFF_BAGGAGE_MISSION_CACHE_SQUARED=0x36640u;
static const u32 OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE=0x30u;
static const u32 OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE=0x34u;
static const u32 OFF_BAGGAGE_CONFIG_MISSION_DISTANCE=0x38u;
static const u32 OFF_PLAYER_ROOT_COUNT=0x38u;
static const u32 OFF_PLAYER_ROOT_ITEMS=0x40u;
static const u32 OFF_PLAYER_ENTRY_CONTAINER=0x48u;
static const u32 OFF_PLAYER_ENTRY_KIND=0x70u;
static const u32 OFF_PLAYER_CONTAINER_CONTEXT=0x56C8u;
static const u32 OFF_PLAYER_CONTEXT_RESOURCE=0x30u;
static const u32 OFF_PLAYER_RESOURCE_ODRADEK_CONFIG=0x28u;
static const u32 OFF_PLAYER_CARGO_DETECT_RANGE=0x44u;
static const u32 OFF_PLAYER_CARGO_SEARCH_RANGE=0x48u;
static const u32 MAX_PLAYER_ROOT_ENTRIES=128u;
static const u32 OFF_MANAGER_DETECTED_COUNT=0x10u;
static const u32 OFF_MANAGER_DETECTED_CAPACITY=0x14u;
static const u32 OFF_MANAGER_DETECTED_ITEMS=0x18u;
static const u32 OFF_MANAGER_DETECTED_LOCK=0x20u;
static const u32 OFF_DETECTED_OWNER=0x48u;
static const u32 OFF_DETECTED_ODRADEK_SENSOR=0x51u;
static const u32 OFF_DETECTED_FRESH_HIT=0x84u;
static const u32 OFF_ENTITY_COMPONENTS=0xA0u;
static const u32 OFF_ENTITY_LIFECYCLE_FLAGS=0x98u;
static const u32 OFF_ENTITY_WORLD_POSITION=0xE8u;
static const u64 ENTITY_FLAG_INACTIVE=0x0000000000000200ull;
static const u64 ENTITY_FLAG_REQUEST_UNREALIZE=0x0000000400000000ull;
static const u64 ENTITY_FLAG_STAGED_UNREALIZE=0x0000000800000000ull;
static const u32 OFF_BAGGAGE_DETECTED_COMPONENT=0x230u;
static const u32 OFF_BAGGAGE_REALIZED_COMPONENT=0x168u;
static const u32 OFF_BAGGAGE_SYMBOLS_OWNER=0x48u;
static const u32 OFF_BAGGAGE_SYMBOLS_BAGGAGE=0x50u;
static const u32 OFF_BAGGAGE_SYMBOLS_RECORD_COUNT=0x70u;
static const u32 OFF_BAGGAGE_SYMBOLS_RECORD_CAPACITY=0x74u;
static const u32 OFF_BAGGAGE_SYMBOLS_RECORDS=0x78u;
static const u32 BAGGAGE_SYMBOLS_SIZE=0x80u;
static const u32 BAGGAGE_SYMBOL_RECORD_SIZE=0x48u;
static const u32 OFF_BAGGAGE_SYMBOL_RECORD_TEMPLATE=0x08u;
static const u32 OFF_BAGGAGE_SYMBOL_RECORD_REMAINING=0x20u;
static const u32 OFF_BAGGAGE_SYMBOL_TEMPLATE_DURATION=0x30u;
static const u32 MAX_BAGGAGE_SYMBOL_RECORDS=256u;
static const u32 MAX_BAGGAGE_SYMBOL_CAPACITY=4096u;
static const u32 BAGGAGE_SYMBOL_EVENT_ID=0x4C87C18Fu;
static const u8 BAGGAGE_SYMBOL_EVENT_CATEGORY=11u;
static const u32 OFF_REALIZED_BAGGAGE_COMPONENT=0x110u;
static const u32 OFF_REALIZED_DISTANCE_SQUARED=0x1F8u;
static const u32 DETECTED_COMPONENT_SIZE=0x88u;
static const u32 BAGGAGE_COMPONENT_SIZE=0x258u;
static const u32 REALIZED_BAGGAGE_SIZE=0x4ACu;
static const u32 MAX_DETECTED_REFRESH_COMPONENTS=4096u;
static const u32 MAX_DETECTED_TICK_CLAIMS=8192u;
static const u32 DETECTED_TICK_BUCKET_COUNT=4u;
static const u32 MAX_DETECTED_MANAGER_CAPACITY=131072u;
static const u32 MAX_PENDING_CARGO_HITS=256u;
static const u32 MAX_PENDING_CARGO_DRAIN_PER_UPDATE=32u;
static const u64 PENDING_CARGO_TTL_MS=5000ull;
static const u32 MAX_REALIZE_LEASES=32u;
static const u64 REALIZE_LEASE_TTL_MS=15000ull;

static const u32 NATIVE_SURVEY_SHAPE=1u;
static const u32 NATIVE_SURVEY_ANGLE_BITS=0x43070000u;
static const u32 NATIVE_SURVEY_RANGE_BITS=0x42480000u;
static const u32 NATIVE_SURVEY_GRID=150u;
static const u32 NATIVE_SURVEY_BASE_TIME_BITS=0x41C80000u;
static const u32 NATIVE_SURVEY_FADE_TIME_BITS=0x40A00000u;
static const u32 NATIVE_SURVEY_CIRCLE_TIME_BITS=0x40A00000u;
static const u32 NATIVE_SURVEY_SIZE_FADE_BITS=0x41A00000u;
static const u32 NATIVE_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE_BITS=0x41C80000u;
static const u32 NATIVE_BAGGAGE_UNTOUCHED_DISTANCE_BITS=0x41C80000u;
static const u32 NATIVE_BAGGAGE_UNTOUCHED_CACHE_BITS=0x441C4000u;

static const float MIN_RANGE_METERS=50.0f;
static const float MAX_RANGE_METERS=1000.0f;
static const float MAX_PLAUSIBLE_RUNTIME_VALUE=2000.0f;
static const float FULL_CIRCLE_FAN_ANGLE=360.0f;
static const float FAR_REFRESH_END_TOLERANCE_METERS=8.0f;
static const u32 FULL_CIRCLE_RADIANS_BITS=0x40C90FDBu;
static const u32 INACTIVE_SIZE_OVERRIDE_BITS=0xBF800000u;
static const DWORD RETRY_DELAY_MS=500u;
static const bool ENABLE_UNVERIFIED_PLAYER_CARGO_FIELDS=false;

enum SettingsResult{SETTINGS_INVALID,SETTINGS_DISABLED,SETTINGS_READY};
enum ApplyResult{APPLY_NOT_READY,APPLY_CONFLICT,APPLY_FAILED,APPLY_CRITICAL,APPLY_READY};
enum ResolveResult{RESOLVE_NOT_READY,RESOLVE_INVALID,RESOLVE_READY};
enum DirectIdentityResult{DIRECT_UNREADABLE,DIRECT_MISMATCH,DIRECT_VALID};

struct Settings{
    float rangeMeters;
    u32 rangeBits;
    u32 uiRangeBits;
    u32 shapeBits;
    u32 angleBits;
    bool fullCircle;
    bool extendCargoMarkers;
};

// v0.1.10's forced realization/lease path regressed live scan performance and
// did not produce the requested HUD reveal. An old INI must not re-enable it.
// Keep the historical routines available to the offline regression harness.
static bool disableRejectedCargoLifecycle(Settings* settings){
    if(!settings)return false;
    bool legacyRequested=settings->extendCargoMarkers;
    settings->extendCargoMarkers=false;
    return legacyRequested;
}

struct GameImage{
    u8* base;
    u32 timestamp;
    u32 imageSize;
};

struct ProtectionPage{
    u8* address;
    DWORD originalProtection;
};

struct FieldPatch{
    u8* address;
    u32 original;
    u32 target;
    bool changed;
};

struct GameplayObjects{
    u8* sensor;
    u8* instance;
    u8* resource;
    u8* manager;
};

struct CargoMarkerObjects{
    u8* manager;
    u8* config;
};

struct OwnedField{
    u8* address;
    u32 nativeValue;
    u32 extendedValue;
    bool owned;
};

struct CargoMarkerOwnership{
    CargoMarkerObjects objects;
    OwnedField fields[2];
    u32 serial;
};

struct HudMarkerOwnership{
    u8* parameters;
    OwnedField field;
    u32 serial;
};

struct PendingCargoHit{
    u8* baggage;
    u8* detected;
    u8* owner;
    u8* readyRealized;
    u32 sweepSerial;
    u32 scanEpoch;
    u32 messageTick;
    u32 targetRadiusBits;
    u32 distanceSquaredBits;
    u64 createdMs;
    double scanOrigin[3];
};

struct RealizeLease{
    u8* owner;
    u8* baggage;
    u8* detected;
    u32 scanEpoch;
    u32 sweepSerial;
    u64 createdMs;
};

struct RealizedInitFlight{
    u8* realized;
    u32 references;
};

struct RealizedInitFlightToken{
    u8* realized;
    bool tracked;
    bool overflow;
};

static HMODULE g_self=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static bool g_debugLog=false;
static float g_visualWaveScale=1.0f;
static volatile long long g_visualWaveInstance=0;
static volatile long g_started=0;
static volatile long g_hooksReady=0;
static volatile long long g_surveyResource=0;
static u8* g_gameBase=0;
static volatile long long g_originalGameplayScan=0;
static u32 g_targetEffectDiameterBits=0;
static bool g_gameplayFullCircle=false;
static volatile long g_gameplayStatus=0;
static volatile long g_gameplayBeforeDiameterBits=0;
static volatile long g_gameplayAfterDiameterBits=0;
static volatile long g_gameplayBeforeGateRadiusBits=0;
static volatile long g_gameplayAfterGateRadiusBits=0;
static volatile long g_gameplayBeforeInstanceAngleBits=0;
static volatile long g_gameplayBeforeManagerAngleBits=0;
static __declspec(align(8)) volatile long long g_scanPresentationState=0;
static __declspec(align(8)) volatile long long g_originalSensorGate=0;
static volatile long g_gateTelemetryReady=0;
static volatile long g_gateCaptureActive=0;
static volatile long g_gateScanSerial=0;
static volatile long g_gateCalls=0;
static volatile long g_gatePasses=0;
static volatile long g_gateRejects=0;
static volatile long g_gateDistanceInvalid=0;
static volatile long g_gateBinTo50=0;
static volatile long g_gateBin50To100=0;
static volatile long g_gateBin100To200=0;
static volatile long g_gateBin200To300=0;
static volatile long g_gateBin300To400=0;
static volatile long g_gateBinOver400=0;
static volatile long g_gatePassOver200=0;
static volatile long g_gateRejectOver200=0;
static volatile long g_gateMaxDistanceSquaredBits=0;
static volatile long g_gateLatestRadiusBits=0;
static volatile long g_gateMaxRadiusBits=0;
static volatile long g_gateLatestAngleBits=0;
static volatile long g_gateLatestActive=0;
static __declspec(align(8)) volatile long long g_gateLatestSnapshot=0;
static __declspec(align(8)) volatile long long g_originalDetectedConstantUpdate=0;
static __declspec(align(8)) volatile long long g_originalEntityManagerGameUpdate=0;
static __declspec(align(8)) volatile long long g_originalRealizedBaggageInit=0;
static __declspec(align(8)) volatile long long g_originalRealizedBaggageUnlink=0;
static __declspec(align(8)) volatile long long g_originalRealizedBaggageUpdate=0;
static __declspec(align(8)) volatile long long g_originalRealizationCoordinator=0;
static volatile long g_farRefreshReady=0;
static volatile long g_lateCargoReady=0;
static volatile long g_farRefreshPending=0;
static volatile long g_farRefreshBusy=0;
static volatile long g_farRefreshScanSerial=0;
static volatile long g_scanAttemptEpoch=0;
static volatile long g_farRefreshScanEpoch=0;
static volatile long g_farRefreshPendingSerial=0;
static volatile long g_farRefreshCompletedSerial=0;
static volatile long g_farRefreshManagerCount=0;
static volatile long g_farRefreshSnapshotCount=0;
static volatile long g_farRefreshEligible=0;
static volatile long g_farRefreshCalls=0;
static volatile long g_farRefreshSkipped=0;
static volatile long g_farRefreshTruncated=0;
static volatile long g_farRefreshRadiusBits=0;
static volatile long g_farRefreshBridges=0;
static volatile long g_farRefreshBridgeFailures=0;
static volatile long g_farRefreshDuplicateClaims=0;
static volatile long g_farRefreshPostJoinCalls=0;
static volatile long g_farRefreshContextReady=0;
static volatile long g_farRefreshContextInvalid=0;
static volatile long g_farRefreshContextInactive=0;
static volatile long g_farRefreshContextDeltaInvalid=0;
static volatile long g_farRefreshContextBucketFailed=0;
static volatile long g_farRefreshManagerMissing=0;
static volatile long g_farRefreshManagerInvalid=0;
static volatile long g_farRefreshManagerInactive=0;
static volatile long g_farRefreshSnapshotInactive=0;
static volatile long g_farRefreshGenerationMismatch=0;
static volatile long g_farRefreshRadiusInvalid=0;
static volatile long g_farRefreshRadiusPending=0;
static volatile long g_farRefreshSerialMismatch=0;
static volatile long g_farRefreshBusyRejected=0;
static volatile long g_farRefreshSnapshotFailed=0;
static volatile long g_farRefreshLastContextFlag=0;
static volatile long g_farRefreshLastTick=0;
static volatile long g_farRefreshLastDeltaBits=0;
static volatile long g_farRefreshLastManagerActive=0;
static volatile long g_farRefreshLastSnapshotActive=0;
static volatile long g_farRefreshLastCurrentGeneration=0;
static volatile long g_farRefreshLastSnapshotGeneration=0;
static volatile long g_farRefreshDetectedValid=0;
static volatile long g_farRefreshDetectedInvalid=0;
static volatile long g_farRefreshDistanceInvalid=0;
static volatile long g_farRefreshOutsideRadius=0;
static volatile long g_farRefreshInRadius=0;
static volatile long g_farRefreshNoBaggage=0;
static volatile long g_farRefreshBaggageFound=0;
static volatile long g_farRefreshRealizedFound=0;
static volatile long g_farRefreshUnrealized=0;
static volatile long g_farRefreshDeferredUnrealized=0;
static volatile long g_farRefreshInvalidCargo=0;
static volatile long g_symbolRevealAttempts=0;
static volatile long g_symbolRevealSucceeded=0;
static volatile long g_symbolRevealLookupFailed=0;
static volatile long g_symbolRevealIdentityFailed=0;
static volatile long g_symbolRevealTemplateFailed=0;
static volatile long g_symbolRevealCoreFailed=0;
static volatile long g_farRefreshClaimDuplicate=0;
static volatile long g_farRefreshClaimFull=0;
static volatile long g_farRefreshFreshHits=0;
static volatile long g_farRefreshFreshMisses=0;
static volatile long g_farRefreshFoundCalls=0;
static volatile long g_farRefreshMarkerCalls=0;
static volatile long g_farRefreshPostCoreBridgeFailed=0;
static volatile long g_farRefreshMaxDetectedDistanceSquaredBits=0;
static volatile long g_farRefreshMaxCargoDistanceSquaredBits=0;
static volatile long g_pendingRecorded=0;
static volatile long g_pendingDedupe=0;
static volatile long g_pendingFull=0;
static volatile long g_pendingExpired=0;
static volatile long g_pendingEpochPurged=0;
static volatile long g_pendingIdentityConflicts=0;
static volatile long g_pendingInitCalls=0;
static volatile long g_pendingInitNoMatch=0;
static volatile long g_pendingInitIdentityReject=0;
static volatile long g_pendingInitDistanceReject=0;
static volatile long g_pendingConsumed=0;
static volatile long g_pendingSyntheticFresh=0;
static volatile long g_pendingNaturalFresh=0;
static volatile long g_pendingFoundCalls=0;
static volatile long g_pendingMarkerCalls=0;
static volatile long g_pendingNativeUpdateCalls=0;
static volatile long g_pendingBridgeFailed=0;
static volatile long g_pendingUnlinkPurged=0;
static volatile long g_pendingHighWater=0;
static volatile long g_pendingMaxConsumeAgeMs=0;
static volatile long g_realizeLeaseRecorded=0;
static volatile long g_realizeLeaseDedupe=0;
static volatile long g_realizeLeaseFull=0;
static volatile long g_realizeLeaseExpired=0;
static volatile long g_realizeLeaseEpochPurged=0;
static volatile long g_realizeLeaseIdentityPurged=0;
static volatile long g_realizeLeaseUnlinkPurged=0;
static volatile long g_realizeLeaseHighWater=0;
static volatile long g_realizeRequestInitial=0;
static volatile long g_realizeRequestRelay=0;
static volatile long g_realizeInitialStateRejected=0;
static volatile long g_realizeCoordinatorCalls=0;
static volatile long g_realizeCoordinatorSnapshots=0;
static u32 g_targetScanRadiusBits=0;
static u8* g_farRefreshSnapshot[MAX_DETECTED_REFRESH_COMPONENTS];

struct DetectedTickBucket{
    bool valid;
    u32 tick;
    volatile long fanoutCount;
    volatile long sweepGuard;
    volatile long pinCount;
    u8* claims[MAX_DETECTED_TICK_CLAIMS];
};

static SRWLOCK_X64 g_farRefreshTickLock={0};
static SRWLOCK_X64 g_farRefreshStateLock={0};
static SRWLOCK_X64 g_pendingCargoLock={0};
static SRWLOCK_X64 g_realizeLeaseLock={0};
static DetectedTickBucket g_farRefreshTickBuckets[DETECTED_TICK_BUCKET_COUNT]={};
static u32 g_farRefreshTickReplacementCursor=0u;
static volatile long g_farRefreshOutstandingFanout=0;
static PendingCargoHit g_pendingCargoHits[MAX_PENDING_CARGO_HITS]={};
static u32 g_pendingCargoCount=0u;
static volatile long g_pendingPublishedCount=0;
static RealizeLease g_realizeLeases[MAX_REALIZE_LEASES]={};
static u32 g_realizeLeaseCount=0u;
static volatile long g_realizeLeasePublishedCount=0;
static RealizedInitFlight g_realizedInitFlights[MAX_PENDING_CARGO_HITS]={};
static volatile long g_realizedInitFlightOverflow=0;
#ifdef ODRADEK_TEST_BUILD
static __declspec(align(8)) volatile long long g_pendingTestNowMs=-1;
typedef u8* (*TestFindEntityComponent)(u8*,const void*);
typedef void (*TestBaggageSymbolsEvent)(u8*,u8*);
typedef void (*TestRequestEntityRealize)(u8*,u8,u8);
static TestFindEntityComponent g_testFindEntityComponent=0;
static TestBaggageSymbolsEvent g_testBaggageSymbolsEvent=0;
static TestRequestEntityRealize g_testRequestEntityRealize=0;
#endif

extern "C" u8 sensorGateTelemetryHook(u8*,const double*,float,u8*);
extern "C" void detectedConstantUpdateHook(u8*,void*);
extern "C" void entityManagerGameUpdateHook(void*,void*);
extern "C" void realizedBaggageInitHook(u8*,void*);
extern "C" void realizedBaggageUnlinkHook(u8*,void*);
extern "C" void realizedBaggageUpdateHook(u8*,void*);
extern "C" void realizationCoordinatorHook(void*);

static u64 scanPresentationValue(u32 serial,bool active){
    return ((u64)serial<<1u)|(active?1u:0u);
}

static u64 scanPresentationSnapshot(u32* serial,bool* active){
    u64 value=(u64)_InterlockedCompareExchange64(
        &g_scanPresentationState,0,0
    );
    if(serial)*serial=(u32)(value>>1u);
    if(active)*active=(value&1u)!=0u;
    return value;
}

static void publishScanPresentation(u32 serial,bool active){
    _InterlockedExchange64(
        &g_scanPresentationState,(long long)scanPresentationValue(serial,active)
    );
}

static void clearAnyScanPresentation(){
    long long current=_InterlockedCompareExchange64(
        &g_scanPresentationState,0,0
    );
    while(((u64)current&1u)!=0u){
        long long desired=(long long)((u64)current&~1ull);
        long long observed=_InterlockedCompareExchange64(
            &g_scanPresentationState,desired,current
        );
        if(observed==current)return;
        current=observed;
    }
}

static bool clearScanPresentation(u32 serial){
    long long expected=(long long)scanPresentationValue(serial,true);
    long long desired=(long long)scanPresentationValue(serial,false);
    return _InterlockedCompareExchange64(
        &g_scanPresentationState,desired,expected
    )==expected;
}

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

static u32 floatBits(float value){
    union FloatBits{float value;u32 bits;};
    FloatBits converted;
    converted.value=value;
    return converted.bits;
}

static u32 hudBaggageFoundTarget(const Settings* settings,bool scanActive){
    return scanActive&&settings&&settings->extendCargoMarkers&&
           settings->rangeMeters>25.0f?
           settings->uiRangeBits:NATIVE_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE_BITS;
}

static bool finiteFloat(u32 bits,float* value){
    if((bits&0x7F800000u)==0x7F800000u)return false;
    union FloatBits{float value;u32 bits;};
    FloatBits converted;
    converted.bits=bits;
    if(value)*value=converted.value;
    return true;
}

static bool finitePositiveFloat(u32 bits,float* value){
    float converted=0.0f;
    if(!finiteFloat(bits,&converted)||!(converted>0.0f)||
       converted>MAX_PLAUSIBLE_RUNTIME_VALUE)return false;
    if(value)*value=converted;
    return true;
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

static void trimmedBounds(const WCHAR* value,u32* begin,u32* end){
    u32 length=0;
    while(value[length])length++;
    u32 first=0;
    while(first<length&&(value[first]==L' '||value[first]==L'\t'))first++;
    while(length>first&&(value[length-1]==L' '||value[length-1]==L'\t'))length--;
    *begin=first;
    *end=length;
}

static bool equalsToken(const WCHAR* value,u32 begin,u32 end,const WCHAR* token){
    if(!value||!token||end<begin)return false;
    u32 length=end-begin;
    u32 i=0;
    while(token[i])i++;
    if(i!=length)return false;
    for(i=0;i<length;i++)if(lowerAscii(value[begin+i])!=lowerAscii(token[i]))return false;
    return true;
}

static bool readIniValue(const WCHAR* key,WCHAR* out,DWORD capacity){
    if(!key||!out||capacity<3u)return false;
    WCHAR path[520];
    modulePath(path,L"ds2_odradek_scan.ini");
    if(!path[0])return false;
    const WCHAR missing[2]={1,0};
    out[0]=0;
    DWORD length=GetPrivateProfileStringW(L"OdradekScan",key,missing,out,capacity,path);
    if(length==1u&&out[0]==1&&out[1]==0)return false;
    return length<capacity-1u;
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
    u32 digits=0,pos=begin;
    while(pos<end&&value[pos]>=L'0'&&value[pos]<=L'9'){
        whole=whole*10u+(u64)(value[pos]-L'0');
        if(whole>10000u)return false;
        digits++;
        pos++;
    }
    if(!digits)return false;
    u32 fraction=0,scale=1;
    if(pos<end&&value[pos]==L'.'){
        pos++;
        u32 fractionDigits=0;
        while(pos<end&&value[pos]>=L'0'&&value[pos]<=L'9'){
            if(fractionDigits>=3u)return false;
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

static void logRaw(const char* text,DWORD length){
    if(!g_debugLog||g_log==INVALID_HANDLE_VALUE||!text||!length)return;
    DWORD written=0;
    WriteFile(g_log,text,length,&written,0);
    FlushFileBuffers(g_log);
}

static void openLog(){
    if(!g_debugLog||g_log!=INVALID_HANDLE_VALUE)return;
    WCHAR path[520];
    modulePath(path,L"ds2_odradek_scan.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Odradek 360 Scan v" MOD_VERSION "\r\n"
            "Expected target: Steam PC 1.10.89.0\r\n"
            "Scope: 360 gameplay sensor and extended cargo scan radius. Native terrain-survey values, cargo lifecycle, native hits and global marker/HUD distances remain native.\r\n";
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
    if(g_log==INVALID_HANDLE_VALUE){
        g_debugLog=true;
        openLog();
    }
    logRaw(text,length);
}

static u32 appendText(char* buffer,u32 position,u32 capacity,const char* text){
    if(!buffer||!text||position>=capacity)return position;
    while(*text&&position+1u<capacity)buffer[position++]=*text++;
    buffer[position]=0;
    return position;
}

static u32 appendHex32(char* buffer,u32 position,u32 capacity,u32 value){
    static const char digits[]="0123456789ABCDEF";
    position=appendText(buffer,position,capacity,"0x");
    for(s32 shift=28;shift>=0&&position+1u<capacity;shift-=4)buffer[position++]=digits[(value>>(u32)shift)&0xFu];
    if(position<capacity)buffer[position]=0;
    return position;
}

static u32 appendHex64(char* buffer,u32 position,u32 capacity,u64 value){
    static const char digits[]="0123456789ABCDEF";
    position=appendText(buffer,position,capacity,"0x");
    for(s32 shift=60;shift>=0&&position+1u<capacity;shift-=4){
        buffer[position++]=digits[(value>>(u32)shift)&0xFu];
    }
    if(position<capacity)buffer[position]=0;
    return position;
}

static u32 appendUnsigned(char* buffer,u32 position,u32 capacity,u32 value){
    char reversed[10];
    u32 count=0;
    do{
        reversed[count++]=(char)('0'+value%10u);
        value/=10u;
    }while(value&&count<10u);
    while(count&&position+1u<capacity)buffer[position++]=reversed[--count];
    if(position<capacity)buffer[position]=0;
    return position;
}

static bool readU32(const void* pointer,u32* value);
static bool readU64(const void* pointer,u64* value);
static bool queryProtection(const void* address,DWORD* protection);
static bool isWritableProtection(DWORD protection);

static void logSurveyValues(const char* state,u8* object){
    if(!state||!object)return;
    u32 shape=0,angle=0,range=0;
    if(!readU32(object+OFF_SURVEY_SHAPE,&shape)||
       !readU32(object+OFF_SURVEY_ANGLE,&angle)||
       !readU32(object+OFF_SURVEY_RANGE,&range))return;
    char buffer[256];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,256u,state);
    position=appendText(buffer,position,256u," address=");
    position=appendHex64(buffer,position,256u,(u64)object);
    position=appendText(buffer,position,256u," shape=");
    position=appendHex32(buffer,position,256u,shape);
    position=appendText(buffer,position,256u," angleBits=");
    position=appendHex32(buffer,position,256u,angle);
    position=appendText(buffer,position,256u," rangeBits=");
    position=appendHex32(buffer,position,256u,range);
    position=appendText(buffer,position,256u,"\r\n");
    logRaw(buffer,position);
}

static void logHookState(const char* state,const char* name,u8* address){
    if(!state||!name||!address)return;
    u64 pointer=0;
    DWORD protection=0;
    readU64(address,&pointer);
    queryProtection(address,&protection);
    char buffer[256];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,256u,state);
    position=appendText(buffer,position,256u," ");
    position=appendText(buffer,position,256u,name);
    position=appendText(buffer,position,256u," slot=");
    position=appendHex64(buffer,position,256u,(u64)address);
    position=appendText(buffer,position,256u," pointer=");
    position=appendHex64(buffer,position,256u,pointer);
    position=appendText(buffer,position,256u," protection=");
    position=appendHex32(buffer,position,256u,protection);
    position=appendText(buffer,position,256u,"\r\n");
    logRaw(buffer,position);
}

static void logHudValues(const char* state,u8* parameters){
    if(!state||!parameters)return;
    u32 searched=0,baggage=0;
    if(!readU32(parameters+OFF_UI_SEARCHED_VISIBLE_DISTANCE,&searched)||
       !readU32(parameters+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE,&baggage))return;
    char buffer[256];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,256u,state);
    position=appendText(buffer,position,256u," address=");
    position=appendHex64(buffer,position,256u,(u64)parameters);
    position=appendText(buffer,position,256u," searchedVisible=");
    position=appendHex32(buffer,position,256u,searched);
    position=appendText(buffer,position,256u," baggageFoundVisible=");
    position=appendHex32(buffer,position,256u,baggage);
    position=appendText(buffer,position,256u,"\r\n");
    logRaw(buffer,position);
}

static void logCargoMarkerValues(const char* state,const CargoMarkerObjects* objects){
    if(!state||!objects||!objects->manager||!objects->config)return;
    u32 untouched=0,touched=0,mission=0;
    u32 untouchedSquared=0,touchedSquared=0,missionSquared=0;
    if(!readU32(objects->config+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE,&untouched)||
       !readU32(objects->config+OFF_BAGGAGE_CONFIG_TOUCHED_DISTANCE,&touched)||
       !readU32(objects->config+OFF_BAGGAGE_CONFIG_MISSION_DISTANCE,&mission)||
       !readU32(objects->manager+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED,&untouchedSquared)||
       !readU32(objects->manager+OFF_BAGGAGE_TOUCHED_CACHE_SQUARED,&touchedSquared)||
       !readU32(objects->manager+OFF_BAGGAGE_MISSION_CACHE_SQUARED,&missionSquared))return;
    char buffer[448];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,448u,state);
    position=appendText(buffer,position,448u," config=");
    position=appendHex64(buffer,position,448u,(u64)objects->config);
    position=appendText(buffer,position,448u," markerDistances=");
    position=appendHex32(buffer,position,448u,untouched);
    position=appendText(buffer,position,448u,",");
    position=appendHex32(buffer,position,448u,touched);
    position=appendText(buffer,position,448u,",");
    position=appendHex32(buffer,position,448u,mission);
    position=appendText(buffer,position,448u," cachedSquared=");
    position=appendHex32(buffer,position,448u,untouchedSquared);
    position=appendText(buffer,position,448u,",");
    position=appendHex32(buffer,position,448u,touchedSquared);
    position=appendText(buffer,position,448u,",");
    position=appendHex32(buffer,position,448u,missionSquared);
    position=appendText(buffer,position,448u,"\r\n");
    logRaw(buffer,position);
}

static void logPlayerCargoRanges(const char* state,u8* config){
    if(!state||!config)return;
    u32 detect=0,search=0;
    if(!readU32(config+OFF_PLAYER_CARGO_DETECT_RANGE,&detect)||
       !readU32(config+OFF_PLAYER_CARGO_SEARCH_RANGE,&search))return;
    char buffer[256];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,256u,state);
    position=appendText(buffer,position,256u," config=");
    position=appendHex64(buffer,position,256u,(u64)config);
    position=appendText(buffer,position,256u," detectRange=");
    position=appendHex32(buffer,position,256u,detect);
    position=appendText(buffer,position,256u," searchRange=");
    position=appendHex32(buffer,position,256u,search);
    position=appendText(buffer,position,256u,"\r\n");
    logRaw(buffer,position);
}

static void logGameplayValues(){
    char buffer[384];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,384u,"ACTIVE: gameplay sensor scan patched: diameterBefore=");
    position=appendHex32(buffer,position,384u,(u32)g_gameplayBeforeDiameterBits);
    position=appendText(buffer,position,384u," diameterAfter=");
    position=appendHex32(buffer,position,384u,(u32)g_gameplayAfterDiameterBits);
    position=appendText(buffer,position,384u," gateRadiusBefore=");
    position=appendHex32(buffer,position,384u,(u32)g_gameplayBeforeGateRadiusBits);
    position=appendText(buffer,position,384u," gateRadiusAfter=");
    position=appendHex32(buffer,position,384u,(u32)g_gameplayAfterGateRadiusBits);
    position=appendText(buffer,position,384u," instanceAngleBefore=");
    position=appendHex32(buffer,position,384u,(u32)g_gameplayBeforeInstanceAngleBits);
    position=appendText(buffer,position,384u," managerAngleBefore=");
    position=appendHex32(buffer,position,384u,(u32)g_gameplayBeforeManagerAngleBits);
    position=appendText(buffer,position,384u," targetAngle=");
    position=appendHex32(buffer,position,384u,FULL_CIRCLE_RADIANS_BITS);
    position=appendText(buffer,position,384u,"\r\n");
    logRaw(buffer,position);
}

static void logGameplayFailure(u32 status){
    char buffer[192];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,192u,"ERROR: gameplay scan hook could not patch a validated live field; status=");
    position=appendHex32(buffer,position,192u,status);
    position=appendText(buffer,position,192u,"; the next scan will retry.\r\n");
    logCritical(buffer,position);
}

static void logGateTelemetry(){
    char buffer[768];
    buffer[0]=0;
    u32 position=0;
    u32 serial=(u32)_InterlockedCompareExchange(&g_gateScanSerial,0,0);
    u32 calls=(u32)_InterlockedCompareExchange(&g_gateCalls,0,0);
    u32 passes=(u32)_InterlockedCompareExchange(&g_gatePasses,0,0);
    u32 rejects=(u32)_InterlockedCompareExchange(&g_gateRejects,0,0);
    position=appendText(buffer,position,768u,"SCAN-GATE: serial=");
    position=appendUnsigned(buffer,position,768u,serial);
    position=appendText(buffer,position,768u," calls=");
    position=appendUnsigned(buffer,position,768u,calls);
    position=appendText(buffer,position,768u," pass=");
    position=appendUnsigned(buffer,position,768u,passes);
    position=appendText(buffer,position,768u," reject=");
    position=appendUnsigned(buffer,position,768u,rejects);
    position=appendText(buffer,position,768u," invalidDistance=");
    position=appendUnsigned(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateDistanceInvalid,0,0)
    );
    position=appendText(buffer,position,768u," bins[<=50,50-100,100-200,200-300,300-400,>400]=[");
    volatile long* bins[6]={
        &g_gateBinTo50,&g_gateBin50To100,&g_gateBin100To200,
        &g_gateBin200To300,&g_gateBin300To400,&g_gateBinOver400
    };
    for(u32 i=0;i<6u;i++){
        if(i)position=appendText(buffer,position,768u,",");
        position=appendUnsigned(
            buffer,position,768u,(u32)_InterlockedCompareExchange(bins[i],0,0)
        );
    }
    position=appendText(buffer,position,768u,"] passOver200=");
    position=appendUnsigned(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gatePassOver200,0,0)
    );
    position=appendText(buffer,position,768u," rejectOver200=");
    position=appendUnsigned(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateRejectOver200,0,0)
    );
    position=appendText(buffer,position,768u," maxDistanceSqBits=");
    position=appendHex32(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateMaxDistanceSquaredBits,0,0)
    );
    position=appendText(buffer,position,768u," radiusLatest/max=");
    position=appendHex32(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateLatestRadiusBits,0,0)
    );
    position=appendText(buffer,position,768u,"/");
    position=appendHex32(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateMaxRadiusBits,0,0)
    );
    position=appendText(buffer,position,768u," angle=");
    position=appendHex32(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateLatestAngleBits,0,0)
    );
    position=appendText(buffer,position,768u," active=");
    position=appendUnsigned(
        buffer,position,768u,(u32)_InterlockedCompareExchange(&g_gateLatestActive,0,0)
    );
    position=appendText(buffer,position,768u," snapshot=");
    position=appendHex64(
        buffer,position,768u,(u64)_InterlockedCompareExchange64(&g_gateLatestSnapshot,0,0)
    );
    position=appendText(buffer,position,768u,"\r\n");
    logRaw(buffer,position);
}

static u32 appendAtomicCounter(
    char* buffer,u32 position,u32 capacity,const char* label,volatile long* value
){
    position=appendText(buffer,position,capacity,label);
    return appendUnsigned(
        buffer,position,capacity,
        (u32)_InterlockedCompareExchange(value,0,0)
    );
}

static void logFarRefreshDiagnostics(u32 serial){
    char gate[768];
    gate[0]=0;
    u32 position=0;
    position=appendText(gate,position,768u,"CARGO-REFRESH-GATE: serial=");
    position=appendUnsigned(gate,position,768u,serial);
    position=appendAtomicCounter(
        gate,position,768u," postJoin=",&g_farRefreshPostJoinCalls
    );
    position=appendAtomicCounter(
        gate,position,768u," contextReady=",&g_farRefreshContextReady
    );
    position=appendAtomicCounter(
        gate,position,768u," contextInvalid=",&g_farRefreshContextInvalid
    );
    position=appendAtomicCounter(
        gate,position,768u," contextInactive=",&g_farRefreshContextInactive
    );
    position=appendAtomicCounter(
        gate,position,768u," deltaInvalid=",&g_farRefreshContextDeltaInvalid
    );
    position=appendAtomicCounter(
        gate,position,768u," bucketFailed=",&g_farRefreshContextBucketFailed
    );
    position=appendAtomicCounter(
        gate,position,768u," managerMissing=",&g_farRefreshManagerMissing
    );
    position=appendAtomicCounter(
        gate,position,768u," managerInvalid=",&g_farRefreshManagerInvalid
    );
    position=appendAtomicCounter(
        gate,position,768u," managerInactive=",&g_farRefreshManagerInactive
    );
    position=appendAtomicCounter(
        gate,position,768u," snapshotInactive=",&g_farRefreshSnapshotInactive
    );
    position=appendAtomicCounter(
        gate,position,768u," generationMismatch=",&g_farRefreshGenerationMismatch
    );
    position=appendAtomicCounter(
        gate,position,768u," radiusInvalid=",&g_farRefreshRadiusInvalid
    );
    position=appendAtomicCounter(
        gate,position,768u," radiusPending=",&g_farRefreshRadiusPending
    );
    position=appendAtomicCounter(
        gate,position,768u," serialMismatch=",&g_farRefreshSerialMismatch
    );
    position=appendAtomicCounter(
        gate,position,768u," busy=",&g_farRefreshBusyRejected
    );
    position=appendAtomicCounter(
        gate,position,768u," snapshotFailed=",&g_farRefreshSnapshotFailed
    );
    position=appendText(gate,position,768u," lastRadiusBits=");
    position=appendHex32(
        gate,position,768u,
        (u32)_InterlockedCompareExchange(&g_farRefreshRadiusBits,0,0)
    );
    position=appendText(gate,position,768u," contextFlag/tick/delta=");
    position=appendHex32(
        gate,position,768u,(u32)_InterlockedCompareExchange(
            &g_farRefreshLastContextFlag,0,0
        )
    );
    position=appendText(gate,position,768u,"/");
    position=appendHex32(
        gate,position,768u,
        (u32)_InterlockedCompareExchange(&g_farRefreshLastTick,0,0)
    );
    position=appendText(gate,position,768u,"/");
    position=appendHex32(
        gate,position,768u,
        (u32)_InterlockedCompareExchange(&g_farRefreshLastDeltaBits,0,0)
    );
    position=appendText(gate,position,768u," manager/snapshot/gen=");
    position=appendHex32(
        gate,position,768u,(u32)_InterlockedCompareExchange(
            &g_farRefreshLastManagerActive,0,0
        )
    );
    position=appendText(gate,position,768u,"/");
    position=appendHex32(
        gate,position,768u,(u32)_InterlockedCompareExchange(
            &g_farRefreshLastSnapshotActive,0,0
        )
    );
    position=appendText(gate,position,768u,"/");
    position=appendHex32(
        gate,position,768u,(u32)_InterlockedCompareExchange(
            &g_farRefreshLastCurrentGeneration,0,0
        )
    );
    position=appendText(gate,position,768u,"/");
    position=appendHex32(
        gate,position,768u,(u32)_InterlockedCompareExchange(
            &g_farRefreshLastSnapshotGeneration,0,0
        )
    );
    position=appendText(gate,position,768u,"\r\n");
    logRaw(gate,position);

    char path[896];
    path[0]=0;
    position=0;
    position=appendText(path,position,896u,"CARGO-REFRESH-PATH: serial=");
    position=appendUnsigned(path,position,896u,serial);
    position=appendAtomicCounter(
        path,position,896u," detectedValid=",&g_farRefreshDetectedValid
    );
    position=appendAtomicCounter(
        path,position,896u," detectedInvalid=",&g_farRefreshDetectedInvalid
    );
    position=appendAtomicCounter(
        path,position,896u," distanceInvalid=",&g_farRefreshDistanceInvalid
    );
    position=appendAtomicCounter(
        path,position,896u," outsideRadius=",&g_farRefreshOutsideRadius
    );
    position=appendAtomicCounter(
        path,position,896u," inRadius=",&g_farRefreshInRadius
    );
    position=appendAtomicCounter(
        path,position,896u," noBaggage=",&g_farRefreshNoBaggage
    );
    position=appendAtomicCounter(
        path,position,896u," baggageFound=",&g_farRefreshBaggageFound
    );
    position=appendAtomicCounter(
        path,position,896u," realizedFound=",&g_farRefreshRealizedFound
    );
    position=appendAtomicCounter(
        path,position,896u," unrealized=",&g_farRefreshUnrealized
    );
    position=appendAtomicCounter(
        path,position,896u," deferredUnrealized=",&g_farRefreshDeferredUnrealized
    );
    position=appendAtomicCounter(
        path,position,896u," invalidCargo=",&g_farRefreshInvalidCargo
    );
    position=appendAtomicCounter(
        path,position,896u," claimDuplicate=",&g_farRefreshClaimDuplicate
    );
    position=appendAtomicCounter(
        path,position,896u," claimFull=",&g_farRefreshClaimFull
    );
    position=appendAtomicCounter(
        path,position,896u," freshHit=",&g_farRefreshFreshHits
    );
    position=appendAtomicCounter(
        path,position,896u," freshMiss=",&g_farRefreshFreshMisses
    );
    position=appendAtomicCounter(
        path,position,896u," foundCalls=",&g_farRefreshFoundCalls
    );
    position=appendAtomicCounter(
        path,position,896u," markerCalls=",&g_farRefreshMarkerCalls
    );
    position=appendAtomicCounter(
        path,position,896u," postCoreFailed=",&g_farRefreshPostCoreBridgeFailed
    );
    u32 terminalSum=(u32)_InterlockedCompareExchange(
            &g_farRefreshDetectedInvalid,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshDistanceInvalid,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshOutsideRadius,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshNoBaggage,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshDeferredUnrealized,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshInvalidCargo,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshClaimDuplicate,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshClaimFull,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshFreshMisses,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshPostCoreBridgeFailed,0,0
        )+(u32)_InterlockedCompareExchange(
            &g_farRefreshBridges,0,0
        );
    position=appendText(path,position,896u," terminalSum=");
    position=appendUnsigned(path,position,896u,terminalSum);
    position=appendText(path,position,896u," maxDetectedDistSqBits=");
    position=appendHex32(
        path,position,896u,(u32)_InterlockedCompareExchange(
            &g_farRefreshMaxDetectedDistanceSquaredBits,0,0
        )
    );
    position=appendText(path,position,896u," maxCargoDistSqBits=");
    position=appendHex32(
        path,position,896u,(u32)_InterlockedCompareExchange(
            &g_farRefreshMaxCargoDistanceSquaredBits,0,0
        )
    );
    position=appendText(path,position,896u,"\r\n");
    logRaw(path,position);
}

static void logFarRefreshTelemetry(){
    char buffer[896];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,896u,"NATIVE-CARGO-SCAN: serial=");
    position=appendUnsigned(
        buffer,position,896u,
        (u32)_InterlockedCompareExchange(&g_farRefreshCompletedSerial,0,0)
    );
    position=appendAtomicCounter(
        buffer,position,896u," fresh=",&g_farRefreshFreshHits
    );
    position=appendAtomicCounter(
        buffer,position,896u," detectedValid=",&g_farRefreshDetectedValid
    );
    position=appendAtomicCounter(
        buffer,position,896u," detectedInvalid=",&g_farRefreshDetectedInvalid
    );
    position=appendAtomicCounter(
        buffer,position,896u," distanceInvalid=",&g_farRefreshDistanceInvalid
    );
    position=appendAtomicCounter(
        buffer,position,896u," outsideRadius=",&g_farRefreshOutsideRadius
    );
    position=appendAtomicCounter(
        buffer,position,896u," inRadius=",&g_farRefreshInRadius
    );
    position=appendAtomicCounter(
        buffer,position,896u," nonCargo=",&g_farRefreshNoBaggage
    );
    position=appendAtomicCounter(
        buffer,position,896u," cargo=",&g_farRefreshBaggageFound
    );
    position=appendAtomicCounter(
        buffer,position,896u," realizedNative=",&g_farRefreshRealizedFound
    );
    position=appendAtomicCounter(
        buffer,position,896u," unrealized=",&g_farRefreshUnrealized
    );
    position=appendAtomicCounter(
        buffer,position,896u," deferred=",&g_farRefreshDeferredUnrealized
    );
    position=appendAtomicCounter(
        buffer,position,896u," invalidCargo=",&g_farRefreshInvalidCargo
    );
    position=appendAtomicCounter(
        buffer,position,896u," pendingFull=",&g_farRefreshClaimFull
    );
    position=appendAtomicCounter(
        buffer,position,896u," captureFailed=",&g_farRefreshPostCoreBridgeFailed
    );
    position=appendText(buffer,position,896u," maxCargoDistSqBits=");
    position=appendHex32(
        buffer,position,896u,(u32)_InterlockedCompareExchange(
            &g_farRefreshMaxCargoDistanceSquaredBits,0,0
        )
    );
    position=appendText(buffer,position,896u,"\r\n");
    logRaw(buffer,position);
}

static u32 pendingCargoCountSnapshot(){
    AcquireSRWLockShared(&g_pendingCargoLock);
    u32 count=g_pendingCargoCount;
    ReleaseSRWLockShared(&g_pendingCargoLock);
    return count;
}

static u32 realizeLeaseCountSnapshot(){
    AcquireSRWLockShared(&g_realizeLeaseLock);
    u32 count=g_realizeLeaseCount;
    ReleaseSRWLockShared(&g_realizeLeaseLock);
    return count;
}

static u32 pendingCargoActivitySnapshot(){
    volatile long* counters[]={
        &g_pendingRecorded,&g_pendingDedupe,&g_pendingFull,&g_pendingExpired,
        &g_pendingEpochPurged,&g_pendingIdentityConflicts,&g_pendingInitCalls,
        &g_pendingInitNoMatch,&g_pendingInitIdentityReject,
        &g_pendingInitDistanceReject,&g_pendingConsumed,
        &g_pendingSyntheticFresh,&g_pendingNaturalFresh,
        &g_pendingFoundCalls,&g_pendingMarkerCalls,
        &g_pendingNativeUpdateCalls,&g_pendingBridgeFailed,
        &g_pendingUnlinkPurged,&g_realizeLeaseRecorded,
        &g_realizeLeaseDedupe,&g_realizeLeaseFull,&g_realizeLeaseExpired,
        &g_realizeLeaseEpochPurged,&g_realizeLeaseIdentityPurged,
        &g_realizeLeaseUnlinkPurged,&g_realizeRequestInitial,
        &g_realizeRequestRelay,&g_realizeInitialStateRejected,
        &g_realizeCoordinatorCalls,
        &g_realizeCoordinatorSnapshots
    };
    u32 activity=0u;
    for(u32 i=0u;i<(u32)(sizeof(counters)/sizeof(counters[0]));i++){
        activity+=(u32)_InterlockedCompareExchange(counters[i],0,0);
    }
    return activity;
}

static void logPendingCargoTelemetry(){
    char buffer[1536];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,1536u,"LATE-CARGO: epoch/serial=");
    position=appendUnsigned(
        buffer,position,1536u,
        (u32)_InterlockedCompareExchange(&g_scanAttemptEpoch,0,0)
    );
    position=appendText(buffer,position,1536u,"/");
    position=appendUnsigned(
        buffer,position,1536u,
        (u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0)
    );
    position=appendText(buffer,position,1536u," pending=");
    position=appendUnsigned(buffer,position,1536u,pendingCargoCountSnapshot());
    position=appendAtomicCounter(
        buffer,position,1536u," recorded=",&g_pendingRecorded
    );
    position=appendAtomicCounter(
        buffer,position,1536u," dedupe=",&g_pendingDedupe
    );
    position=appendAtomicCounter(
        buffer,position,1536u," full=",&g_pendingFull
    );
    position=appendAtomicCounter(
        buffer,position,1536u," expired=",&g_pendingExpired
    );
    position=appendAtomicCounter(
        buffer,position,1536u," epochPurged=",&g_pendingEpochPurged
    );
    position=appendAtomicCounter(
        buffer,position,1536u," identityConflict=",&g_pendingIdentityConflicts
    );
    position=appendAtomicCounter(
        buffer,position,1536u," initCalls=",&g_pendingInitCalls
    );
    position=appendAtomicCounter(
        buffer,position,1536u," initNoMatch=",&g_pendingInitNoMatch
    );
    position=appendAtomicCounter(
        buffer,position,1536u," identityReject=",&g_pendingInitIdentityReject
    );
    position=appendAtomicCounter(
        buffer,position,1536u," distanceReject=",&g_pendingInitDistanceReject
    );
    position=appendAtomicCounter(
        buffer,position,1536u," consumed=",&g_pendingConsumed
    );
    position=appendAtomicCounter(
        buffer,position,1536u," freshSynthetic=",&g_pendingSyntheticFresh
    );
    position=appendAtomicCounter(
        buffer,position,1536u," freshNatural=",&g_pendingNaturalFresh
    );
    position=appendAtomicCounter(
        buffer,position,1536u," found=",&g_pendingFoundCalls
    );
    position=appendAtomicCounter(
        buffer,position,1536u," marker=",&g_pendingMarkerCalls
    );
    position=appendAtomicCounter(
        buffer,position,1536u," nativeUpdate=",&g_pendingNativeUpdateCalls
    );
    position=appendAtomicCounter(
        buffer,position,1536u," failed=",&g_pendingBridgeFailed
    );
    position=appendAtomicCounter(
        buffer,position,1536u," unlinkPurged=",&g_pendingUnlinkPurged
    );
    position=appendAtomicCounter(
        buffer,position,1536u," highWater=",&g_pendingHighWater
    );
    position=appendAtomicCounter(
        buffer,position,1536u," maxAgeMs=",&g_pendingMaxConsumeAgeMs
    );
    position=appendText(buffer,position,1536u," leases=");
    position=appendUnsigned(buffer,position,1536u,realizeLeaseCountSnapshot());
    position=appendAtomicCounter(
        buffer,position,1536u," leaseRecorded=",&g_realizeLeaseRecorded
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseDedupe=",&g_realizeLeaseDedupe
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseFull=",&g_realizeLeaseFull
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseExpired=",&g_realizeLeaseExpired
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseEpochPurged=",&g_realizeLeaseEpochPurged
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseIdentityPurged=",&g_realizeLeaseIdentityPurged
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseUnlinkPurged=",&g_realizeLeaseUnlinkPurged
    );
    position=appendAtomicCounter(
        buffer,position,1536u," leaseHighWater=",&g_realizeLeaseHighWater
    );
    position=appendAtomicCounter(
        buffer,position,1536u," realizeInitial=",&g_realizeRequestInitial
    );
    position=appendAtomicCounter(
        buffer,position,1536u," realizeRelay=",&g_realizeRequestRelay
    );
    position=appendAtomicCounter(
        buffer,position,1536u," initialStateReject=",
        &g_realizeInitialStateRejected
    );
    position=appendAtomicCounter(
        buffer,position,1536u," coordinatorCalls=",&g_realizeCoordinatorCalls
    );
    position=appendAtomicCounter(
        buffer,position,1536u," coordinatorSnapshots=",&g_realizeCoordinatorSnapshots
    );
    position=appendText(buffer,position,1536u,"\r\n");
    logRaw(buffer,position);
}

static void resetFarRefreshDiagnostics(){
    volatile long* counters[]={
        &g_farRefreshManagerCount,&g_farRefreshSnapshotCount,
        &g_farRefreshEligible,&g_farRefreshCalls,&g_farRefreshSkipped,
        &g_farRefreshTruncated,&g_farRefreshRadiusBits,&g_farRefreshBridges,
        &g_farRefreshBridgeFailures,&g_farRefreshDuplicateClaims,
        &g_farRefreshPostJoinCalls,&g_farRefreshContextReady,
        &g_farRefreshContextInvalid,&g_farRefreshContextInactive,
        &g_farRefreshContextDeltaInvalid,&g_farRefreshContextBucketFailed,
        &g_farRefreshManagerMissing,&g_farRefreshManagerInvalid,
        &g_farRefreshManagerInactive,&g_farRefreshSnapshotInactive,
        &g_farRefreshGenerationMismatch,&g_farRefreshRadiusInvalid,
        &g_farRefreshRadiusPending,&g_farRefreshSerialMismatch,
        &g_farRefreshBusyRejected,&g_farRefreshSnapshotFailed,
        &g_farRefreshLastContextFlag,&g_farRefreshLastTick,
        &g_farRefreshLastDeltaBits,&g_farRefreshLastManagerActive,
        &g_farRefreshLastSnapshotActive,&g_farRefreshLastCurrentGeneration,
        &g_farRefreshLastSnapshotGeneration,
        &g_farRefreshDetectedValid,
        &g_farRefreshDetectedInvalid,&g_farRefreshDistanceInvalid,
        &g_farRefreshOutsideRadius,&g_farRefreshInRadius,
        &g_farRefreshNoBaggage,&g_farRefreshBaggageFound,
        &g_farRefreshRealizedFound,&g_farRefreshUnrealized,
        &g_farRefreshDeferredUnrealized,&g_farRefreshInvalidCargo,
        &g_symbolRevealAttempts,&g_symbolRevealSucceeded,
        &g_symbolRevealLookupFailed,&g_symbolRevealIdentityFailed,
        &g_symbolRevealTemplateFailed,&g_symbolRevealCoreFailed,
        &g_farRefreshClaimDuplicate,
        &g_farRefreshClaimFull,&g_farRefreshFreshHits,
        &g_farRefreshFreshMisses,&g_farRefreshFoundCalls,
        &g_farRefreshMarkerCalls,&g_farRefreshPostCoreBridgeFailed,
        &g_farRefreshMaxDetectedDistanceSquaredBits,
        &g_farRefreshMaxCargoDistanceSquaredBits
    };
    for(u32 i=0;i<(u32)(sizeof(counters)/sizeof(counters[0]));i++){
        _InterlockedExchange(counters[i],0);
    }
}

static void logFarRefreshWaiting(){
    char buffer[320];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,320u,"CARGO-REFRESH-WAIT: serial=");
    position=appendUnsigned(
        buffer,position,320u,
        (u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0)
    );
    position=appendText(buffer,position,320u," pending=");
    position=appendUnsigned(
        buffer,position,320u,
        (u32)_InterlockedCompareExchange(&g_farRefreshPending,0,0)
    );
    position=appendText(buffer,position,320u," busy=");
    position=appendUnsigned(
        buffer,position,320u,
        (u32)_InterlockedCompareExchange(&g_farRefreshBusy,0,0)
    );
    position=appendText(buffer,position,320u," latestRadiusBits=");
    position=appendHex32(
        buffer,position,320u,
        (u32)_InterlockedCompareExchange(&g_gateLatestRadiusBits,0,0)
    );
    position=appendText(buffer,position,320u,"\r\n");
    logRaw(buffer,position);
    logFarRefreshDiagnostics(
        (u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0)
    );
}

static void logFarRefreshCancelled(u32 serial,const char* reason){
    char buffer[256];
    buffer[0]=0;
    u32 position=0;
    position=appendText(buffer,position,256u,"CARGO-REFRESH-CANCELLED: serial=");
    position=appendUnsigned(
        buffer,position,256u,
        serial
    );
    position=appendText(buffer,position,256u," reason=");
    position=appendText(buffer,position,256u,reason?reason:"unknown");
    position=appendText(buffer,position,256u,"\r\n");
    logRaw(buffer,position);
}

static bool cancelFarRefresh(u32 serial,const char* reason){
    if(!serial)return false;
    bool cancelled=false;
    AcquireSRWLockExclusive(&g_farRefreshStateLock);
    if((u32)_InterlockedCompareExchange(
           &g_farRefreshScanSerial,0,0
       )==serial&&(u32)_InterlockedCompareExchange(
           &g_farRefreshPendingSerial,0,0
       )==serial&&_InterlockedCompareExchange(
           &g_farRefreshBusy,0,0
       )==0&&_InterlockedCompareExchange(
           &g_farRefreshPending,0,1
       )==1){
        clearScanPresentation(serial);
        cancelled=true;
    }
    ReleaseSRWLockExclusive(&g_farRefreshStateLock);
    if(cancelled){
        logFarRefreshCancelled(serial,reason);
        logFarRefreshDiagnostics(serial);
    }
    return cancelled;
}

static SettingsResult loadSettings(Settings* settings){
    if(!settings)return SETTINGS_INVALID;
    WCHAR value[128];
    bool enabled=false;
    if(!readIniValue(L"Enabled",value,128u)||!parseBool(value,&enabled)){
        const char message[]="ERROR: missing or invalid [OdradekScan] Enabled value; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        return SETTINGS_INVALID;
    }
    if(!enabled)return SETTINGS_DISABLED;
    if(readIniValue(L"DebugLog",value,128u)&&!parseBool(value,&g_debugLog)){
        const char message[]="ERROR: invalid DebugLog value; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        return SETTINGS_INVALID;
    }
    if(!readIniValue(L"RangeMeters",value,128u)||
       !parseDecimal(value,MIN_RANGE_METERS,MAX_RANGE_METERS,&settings->rangeMeters)){
        const char message[]="ERROR: RangeMeters radius must be a decimal value from 50 to 1000; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        return SETTINGS_INVALID;
    }
    g_visualWaveScale=1.0f;
    if(readIniValue(L"VisualWaveScale",value,128u)&&
       !parseDecimal(value,0.25f,1.0f,&g_visualWaveScale)){
        const char message[]="ERROR: VisualWaveScale must be from 0.25 to 1; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));return SETTINGS_INVALID;
    }
    settings->rangeBits=floatBits(settings->rangeMeters*2.0f);
    settings->uiRangeBits=floatBits(settings->rangeMeters);
    settings->fullCircle=true;
    if(readIniValue(L"FullCircle",value,128u)&&!parseBool(value,&settings->fullCircle)){
        const char message[]="ERROR: invalid FullCircle value; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        return SETTINGS_INVALID;
    }
    settings->shapeBits=NATIVE_SURVEY_SHAPE;
    settings->angleBits=NATIVE_SURVEY_ANGLE_BITS;
    if(settings->fullCircle){
        if(!readIniValue(L"FullCircleMode",value,128u)){
            const char message[]="ERROR: missing FullCircleMode value; no patch applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            return SETTINGS_INVALID;
        }
        u32 begin=0,end=0;
        trimmedBounds(value,&begin,&end);
        if(equalsToken(value,begin,end,L"sphere")||
           equalsToken(value,begin,end,L"fan360")){
            // Terrain-safe 360 mode: keep the native Fan survey shape so
            // shallow/deep/tar hazard overlays continue to use their native
            // survey path. "Sphere" stays as a compatibility alias for
            // existing v1.0.0 INIs.
            settings->shapeBits=NATIVE_SURVEY_SHAPE;
            settings->angleBits=floatBits(FULL_CIRCLE_FAN_ANGLE);
        }else if(equalsToken(value,begin,end,L"legacysphere")){
            settings->shapeBits=0u;
            settings->angleBits=NATIVE_SURVEY_ANGLE_BITS;
        }else if(equalsToken(value,begin,end,L"spherical")){
            settings->shapeBits=2u;
            settings->angleBits=NATIVE_SURVEY_ANGLE_BITS;
        }
        else{
            const char message[]="ERROR: FullCircleMode must be Sphere, Fan360, LegacySphere, or Spherical; no patch applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            return SETTINGS_INVALID;
        }
    }
    settings->extendCargoMarkers=false;
    if(readIniValue(L"ExtendCargoMarkers",value,128u)&&
       !parseBool(value,&settings->extendCargoMarkers)){
        const char message[]="ERROR: invalid ExtendCargoMarkers value; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        return SETTINGS_INVALID;
    }
    return SETTINGS_READY;
}

static bool readBuildMetadata(u8* base,GameImage* image){
    if(!base||!image||readU16Raw(base)!=0x5A4Du)return false;
    u32 pe=readU32Raw(base+0x3Cu);
    if(pe<0x40u||pe>=0x2000u||readU32Raw(base+pe)!=0x00004550u)return false;
    if(readU16Raw(base+pe+4u)!=0x8664u)return false;
    if(readU16Raw(base+pe+20u)<0x00F0u||readU16Raw(base+pe+24u)!=0x020Bu)return false;
    image->base=base;
    image->timestamp=readU32Raw(base+pe+8u);
    image->imageSize=readU32Raw(base+pe+0x50u);
    return image->imageSize>=0x100000u;
}

static bool rangeReadable(const void* pointer,u64 length){
    if(!pointer||!length)return false;
    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(pointer,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
    DWORD protection=info.Protect;
    if((protection&PAGE_GUARD)||((protection&0xFFu)==PAGE_NOACCESS))return false;
    u64 address=(u64)pointer;
    u64 base=(u64)info.BaseAddress;
    u64 end=base+info.RegionSize;
    return address>=base&&address+length>=address&&address+length<=end;
}

static bool readU32(const void* pointer,u32* value){
    if(!value||!rangeReadable(pointer,4u))return false;
    u32 copy=0;
    SIZE_T received=0;
    if(!ReadProcessMemory(GetCurrentProcess(),pointer,&copy,4u,&received)||received!=4u)return false;
    *value=copy;
    return true;
}

static bool readU64(const void* pointer,u64* value){
    if(!value||!rangeReadable(pointer,8u))return false;
    u64 copy=0;
    SIZE_T received=0;
    if(!ReadProcessMemory(GetCurrentProcess(),pointer,&copy,8u,&received)||received!=8u)return false;
    *value=copy;
    return true;
}

static bool compareExchangeU32(u8* pointer,u32 desired,u32 expected,u32* observed){
    if(!pointer||!observed||((u64)pointer&3u)!=0u||!rangeReadable(pointer,4u))return false;
    DWORD protection=0;
    if(!queryProtection(pointer,&protection)||!isWritableProtection(protection))return false;
    *observed=(u32)_InterlockedCompareExchange(
        (volatile long*)pointer,(long)desired,(long)expected
    );
    return true;
}

static bool compareExchangeU64(u8* pointer,u64 desired,u64 expected,u64* observed){
    if(!pointer||!observed||((u64)pointer&7u)!=0u||!rangeReadable(pointer,8u))return false;
    DWORD protection=0;
    if(!queryProtection(pointer,&protection)||!isWritableProtection(protection))return false;
    *observed=(u64)_InterlockedCompareExchange64(
        (volatile long long*)pointer,(long long)desired,(long long)expected
    );
    return true;
}

static bool pointerInImage(const GameImage* image,u64 pointer){
    if(!image||!image->base)return false;
    u64 begin=(u64)image->base;
    u64 end=begin+(u64)image->imageSize;
    return pointer>=begin&&pointer<end;
}

static bool validateVtable(const GameImage* image,u8* object,u64 expectedRva,const u64* functions){
    if(!image||!object||!functions)return false;
    u64 vtable=0;
    if(!readU64(object,&vtable)||vtable!=(u64)(image->base+expectedRva))return false;
    for(u32 i=0;i<4u;i++){
        u64 entry=0;
        if(!readU64((const void*)(vtable+(u64)i*8u),&entry)||
           entry!=(u64)(image->base+functions[i])||!pointerInImage(image,entry))return false;
    }
    return true;
}

static DirectIdentityResult directVtableIdentity(
    const GameImage* image,u8* object,u64 expectedRva
){
    if(!image||!image->base||!object)return DIRECT_UNREADABLE;
    u64 vtable=0u;
    if(!readU64(object,&vtable))return DIRECT_UNREADABLE;
    if(vtable!=(u64)(image->base+expectedRva))return DIRECT_MISMATCH;
    return DIRECT_VALID;
}

static bool hasExactVtable(u8* object,u64 expectedRva){
    u64 vtable=0;
    return g_gameBase&&object&&readU64(object,&vtable)&&
           vtable==(u64)(g_gameBase+expectedRva);
}

static ResolveResult resolveUiGameDesign(const GameImage* image,u8** parametersOut){
    if(!image||!image->base||!parametersOut)return RESOLVE_INVALID;
    *parametersOut=0;
    u64 cache=0,resident=0,parameters=0;
    if(!readU64(image->base+RVA_UI_CACHE_ROOT,&cache))return RESOLVE_INVALID;
    if(!cache)return RESOLVE_NOT_READY;
    if(!readU64((u8*)cache+OFF_UI_CACHE_RESIDENT,&resident))return RESOLVE_INVALID;
    if(!resident)return RESOLVE_NOT_READY;
    if(!validateVtable(
        image,(u8*)resident,RVA_UI_RESIDENT_VTABLE,UI_RESIDENT_VFUNCS
    ))return RESOLVE_INVALID;
    if(!readU64((u8*)resident+OFF_UI_RESIDENT_GAME_PARAMS,&parameters))return RESOLVE_INVALID;
    if(!parameters)return RESOLVE_NOT_READY;
    if(!validateVtable(
        image,(u8*)parameters,RVA_UI_GAME_PARAMS_VTABLE,UI_GAME_PARAMS_VFUNCS
    ))return RESOLVE_INVALID;
    if(!rangeReadable(
        (u8*)parameters+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE,4u
    ))return RESOLVE_INVALID;
    *parametersOut=(u8*)parameters;
    return RESOLVE_READY;
}

static ResolveResult resolveCargoMarkerObjects(
    const GameImage* image,CargoMarkerObjects* objects
){
    if(!image||!image->base||!objects)return RESOLVE_INVALID;
    memset(objects,0,sizeof(*objects));
    u64 manager=0,config=0;
    if(!readU64(image->base+RVA_BAGGAGE_MANAGER_GLOBAL,&manager))return RESOLVE_INVALID;
    if(!manager)return RESOLVE_NOT_READY;
    objects->manager=(u8*)manager;
    if(!validateVtable(
        image,objects->manager,RVA_BAGGAGE_MANAGER_VTABLE,BAGGAGE_MANAGER_VFUNCS
    )||!validateVtable(
        image,objects->manager+8u,RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE,
        BAGGAGE_MANAGER_SECONDARY_VFUNCS
    ))return RESOLVE_INVALID;
    if(!readU64(objects->manager+OFF_BAGGAGE_MANAGER_CONFIG,&config))return RESOLVE_INVALID;
    if(!config)return RESOLVE_NOT_READY;
    objects->config=(u8*)config;
    if(!validateVtable(
        image,objects->config,RVA_BAGGAGE_CONFIG_VTABLE,BAGGAGE_CONFIG_VFUNCS
    ))return RESOLVE_INVALID;
    u32 realizeBits=0,unrealizeBits=0;
    float realize=0.0f,unrealize=0.0f;
    if(!readU32(objects->config+OFF_BAGGAGE_CONFIG_REALIZE_DISTANCE,&realizeBits)||
       !readU32(objects->config+OFF_BAGGAGE_CONFIG_UNREALIZE_DISTANCE,&unrealizeBits)||
       !finitePositiveFloat(realizeBits,&realize)||
       !finitePositiveFloat(unrealizeBits,&unrealize)||unrealize<=realize){
        return RESOLVE_INVALID;
    }
    if(!rangeReadable(
        objects->config+OFF_BAGGAGE_CONFIG_MISSION_DISTANCE,4u
    )||!rangeReadable(
        objects->manager+OFF_BAGGAGE_MISSION_CACHE_SQUARED,4u
    ))return RESOLVE_INVALID;
    return RESOLVE_READY;
}

static void failClosedPresentationWatchdog(const GameImage* image){
    u32 serial=0u;
    bool active=false;
    u64 state=scanPresentationSnapshot(&serial,&active);
    if(!active)return;
    u64 managerPointer=0u;
    u32 managerActive=0u;
    bool stillActive=image&&image->base&&readU64(
            image->base+RVA_SENSOR_MANAGER_GLOBAL,&managerPointer
        )&&managerPointer&&readU32(
            (u8*)managerPointer+OFF_MANAGER_ACTIVE,&managerActive
        )&&(managerActive&0xFFu)==1u;
    if(stillActive){
        return;
    }
    AcquireSRWLockExclusive(&g_farRefreshStateLock);
    if((u64)_InterlockedCompareExchange64(
           &g_scanPresentationState,0,0
       )==state){
        clearScanPresentation(serial);
        _InterlockedExchange(&g_farRefreshCompletedSerial,(long)serial);
    }
    ReleaseSRWLockExclusive(&g_farRefreshStateLock);
}

static ResolveResult resolvePlayerOdradekConfig(
    const GameImage* image,u8** configOut
){
    if(!image||!image->base||!configOut)return RESOLVE_INVALID;
    *configOut=0;
    u64 root=0,items=0;
    u32 count=0;
    if(!readU64(image->base+RVA_PLAYER_RESOURCE_ROOT,&root))return RESOLVE_INVALID;
    if(!root)return RESOLVE_NOT_READY;
    if(!readU32((u8*)root+OFF_PLAYER_ROOT_COUNT,&count)||
       !readU64((u8*)root+OFF_PLAYER_ROOT_ITEMS,&items))return RESOLVE_INVALID;
    if(!count||!items)return RESOLVE_NOT_READY;
    if(count>MAX_PLAYER_ROOT_ENTRIES||
       !rangeReadable((const void*)items,(u64)count*8u))return RESOLVE_INVALID;

    for(u32 i=0;i<count;i++){
        u64 entry=0;
        if(!readU64((const void*)(items+(u64)i*8u),&entry))return RESOLVE_INVALID;
        if(!entry)continue;
        u32 kind=0;
        if(!readU32((u8*)entry+OFF_PLAYER_ENTRY_KIND,&kind))return RESOLVE_INVALID;
        if(kind!=0u)continue;

        u64 container=0,context=0,resource=0,config=0;
        if(!readU64((u8*)entry+OFF_PLAYER_ENTRY_CONTAINER,&container)){
            return RESOLVE_INVALID;
        }
        if(!container)return RESOLVE_NOT_READY;
        if(!readU64((u8*)container+OFF_PLAYER_CONTAINER_CONTEXT,&context)){
            return RESOLVE_INVALID;
        }
        if(!context)return RESOLVE_NOT_READY;
        if(!readU64((u8*)context+OFF_PLAYER_CONTEXT_RESOURCE,&resource)){
            return RESOLVE_INVALID;
        }
        if(!resource)return RESOLVE_NOT_READY;
        if(!readU64((u8*)resource+OFF_PLAYER_RESOURCE_ODRADEK_CONFIG,&config)){
            return RESOLVE_INVALID;
        }
        if(!config)return RESOLVE_NOT_READY;
        if(!validateVtable(
            image,(u8*)config,RVA_PLAYER_ODRADEK_CONFIG_VTABLE,
            PLAYER_ODRADEK_CONFIG_VFUNCS
        ))return RESOLVE_INVALID;

        u32 detectBits=0,searchBits=0;
        float detect=0.0f,search=0.0f;
        if(!readU32((u8*)config+OFF_PLAYER_CARGO_DETECT_RANGE,&detectBits)||
           !readU32((u8*)config+OFF_PLAYER_CARGO_SEARCH_RANGE,&searchBits)||
           !finiteFloat(detectBits,&detect)||!finiteFloat(searchBits,&search)||
           detect<0.0f||search<0.0f||
           detect>MAX_PLAUSIBLE_RUNTIME_VALUE||
           search>MAX_PLAUSIBLE_RUNTIME_VALUE)return RESOLVE_INVALID;
        *configOut=(u8*)config;
        return RESOLVE_READY;
    }
    return RESOLVE_NOT_READY;
}

static bool validateSurveyIdentity(const GameImage* image,u8* object){
    if(!validateVtable(image,object,RVA_SURVEY_VTABLE,SURVEY_VFUNCS))return false;
    u32 value=0;
    return readU32(object+OFF_SURVEY_BASE_TIME,&value)&&value==NATIVE_SURVEY_BASE_TIME_BITS&&
           readU32(object+OFF_SURVEY_FADE_TIME,&value)&&value==NATIVE_SURVEY_FADE_TIME_BITS&&
           readU32(object+OFF_SURVEY_CIRCLE_TIME,&value)&&value==NATIVE_SURVEY_CIRCLE_TIME_BITS&&
           readU32(object+OFF_SURVEY_SIZE_FADE,&value)&&value==NATIVE_SURVEY_SIZE_FADE_BITS&&
           readU32(object+OFF_SURVEY_NUM_GRID,&value)&&value==NATIVE_SURVEY_GRID;
}

static bool validateSurveyNative(const GameImage* image,u8* object){
    u32 shape=0,angle=0,range=0;
    return validateSurveyIdentity(image,object)&&
           readU32(object+OFF_SURVEY_SHAPE,&shape)&&shape==NATIVE_SURVEY_SHAPE&&
           readU32(object+OFF_SURVEY_ANGLE,&angle)&&angle==NATIVE_SURVEY_ANGLE_BITS&&
           readU32(object+OFF_SURVEY_RANGE,&range)&&range==NATIVE_SURVEY_RANGE_BITS;
}

[[maybe_unused]] static bool validateSurveyTarget(const GameImage* image,u8* object,const Settings* settings){
    u32 shape=0,angle=0,range=0;
    return settings&&validateSurveyIdentity(image,object)&&
           readU32(object+OFF_SURVEY_SHAPE,&shape)&&shape==settings->shapeBits&&
           readU32(object+OFF_SURVEY_ANGLE,&angle)&&angle==settings->angleBits&&
           readU32(object+OFF_SURVEY_RANGE,&range)&&range==settings->rangeBits;
}

static bool validateSurveyPatchable(const GameImage* image,u8* object,const Settings* settings){
    if(!settings)return false;
    if(validateSurveyNative(image,object))return true;
    if(!validateSurveyIdentity(image,object))return false;
    u64 pair=0;
    u32 range=0;
    u64 nativePair=((u64)NATIVE_SURVEY_ANGLE_BITS<<32)|NATIVE_SURVEY_SHAPE;
    u64 targetPair=((u64)settings->angleBits<<32)|settings->shapeBits;
    return readU64(object+OFF_SURVEY_SHAPE,&pair)&&
           (pair==nativePair||pair==targetPair)&&
           readU32(object+OFF_SURVEY_RANGE,&range)&&
           (range==NATIVE_SURVEY_RANGE_BITS||range==settings->rangeBits);
}

static bool isWritableProtection(DWORD protection){
    if(protection&PAGE_GUARD)return false;
    DWORD base=protection&0xFFu;
    return base==PAGE_READWRITE||base==PAGE_WRITECOPY||
           base==PAGE_EXECUTE_READWRITE||base==PAGE_EXECUTE_WRITECOPY;
}

static bool queryProtection(const void* address,DWORD* protection){
    if(!address||!protection)return false;
    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(address,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
    *protection=info.Protect;
    return true;
}

static DWORD protectionAfterOwnedWrite(DWORD original){
    DWORD modifiers=original&~0xFFu;
    DWORD base=original&0xFFu;
    if(base==PAGE_WRITECOPY)base=PAGE_READWRITE;
    else if(base==PAGE_EXECUTE_WRITECOPY)base=PAGE_EXECUTE_READWRITE;
    return modifiers|base;
}

static bool protectionValueMatchesAfterOwnedWrite(DWORD actual,DWORD original){
    return actual==original||actual==protectionAfterOwnedWrite(original);
}

static bool protectionMatches(const void* address,DWORD expected){
    DWORD actual=0;
    return queryProtection(address,&actual)&&actual==expected;
}

static bool protectionMatchesAfterOwnedWrite(const void* address,DWORD expected){
    DWORD actual=0;
    return queryProtection(address,&actual)&&protectionValueMatchesAfterOwnedWrite(actual,expected);
}

static bool addProtectionPage(
    ProtectionPage* pages,u32* count,u32 capacity,u8* address,u32 pageSize
){
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

static bool collectFieldPages(
    FieldPatch* fields,u32 fieldCount,ProtectionPage* pages,u32* pageCount,u32 capacity,u32 pageSize
){
    if(!fields||!fieldCount||!pages||!pageCount)return false;
    *pageCount=0;
    for(u32 i=0;i<fieldCount;i++){
        if(!addProtectionPage(pages,pageCount,capacity,fields[i].address,pageSize))return false;
    }
    return true;
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
        if(!VirtualProtect(pages[i-1u].address,pageSize,pages[i-1u].originalProtection,&ignored))complete=false;
    }
    for(u32 i=0;i<count;i++)if(!protectionMatches(pages[i].address,pages[i].originalProtection))complete=false;
    return complete;
}

static bool rollbackFields(FieldPatch* fields,u32 count){
    if(!fields)return false;
    bool complete=true;
    for(u32 i=count;i>0;i--){
        FieldPatch* field=&fields[i-1u];
        if(!field->changed)continue;
        u32 observed=0;
        if(!compareExchangeU32(field->address,field->original,field->target,&observed)||
           (observed!=field->target&&observed!=field->original))complete=false;
        u32 verified=0;
        if(!readU32(field->address,&verified)||verified!=field->original)complete=false;
        field->changed=false;
    }
    return complete;
}

static ApplyResult applyFieldTransaction(FieldPatch* fields,u32 count){
    if(!fields||!count||count>8u)return APPLY_FAILED;
    for(u32 i=0;i<count;i++){
        fields[i].changed=false;
        if(((u64)fields[i].address&3u)!=0u)return APPLY_FAILED;
        u32 current=0;
        if(!readU32(fields[i].address,&current))return APPLY_NOT_READY;
        if(current!=fields[i].original&&current!=fields[i].target)return APPLY_CONFLICT;
    }
    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    if(pageSize<4096u||pageSize>65536u||(pageSize&(pageSize-1u))!=0u)return APPLY_FAILED;
    ProtectionPage pages[8];
    u32 pageCount=0;
    if(!collectFieldPages(fields,count,pages,&pageCount,8u,pageSize))return APPLY_FAILED;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

    ApplyResult failure=APPLY_READY;
    for(u32 i=0;i<count;i++){
        if(fields[i].original==fields[i].target)continue;
        u32 current=0;
        if(!readU32(fields[i].address,&current)){failure=APPLY_FAILED;break;}
        if(current==fields[i].target)continue;
        if(current!=fields[i].original){failure=APPLY_CONFLICT;break;}
        u32 observed=0;
        if(!compareExchangeU32(
            fields[i].address,fields[i].target,fields[i].original,&observed
        )){failure=APPLY_FAILED;break;}
        if(observed!=fields[i].original){failure=APPLY_CONFLICT;break;}
        fields[i].changed=true;
        u32 verified=0;
        if(!readU32(fields[i].address,&verified)||verified!=fields[i].target){failure=APPLY_FAILED;break;}
    }
    if(failure==APPLY_READY){
        for(u32 i=0;i<count;i++){
            u32 verified=0;
            if(!readU32(fields[i].address,&verified)||verified!=fields[i].target){
                failure=APPLY_FAILED;
                break;
            }
        }
    }
    if(failure!=APPLY_READY){
        bool rolledBack=rollbackFields(fields,count);
        bool restored=restorePages(pages,pageCount,pageSize);
        return rolledBack&&restored?failure:APPLY_CRITICAL;
    }
    if(!restorePages(pages,pageCount,pageSize)){
        if(!makePagesWritable(pages,pageCount,pageSize))return APPLY_CRITICAL;
        bool rolledBack=rollbackFields(fields,count);
        bool restored=restorePages(pages,pageCount,pageSize);
        return rolledBack&&restored?APPLY_FAILED:APPLY_CRITICAL;
    }
    return APPLY_READY;
}

static bool rollbackSurveyValues(
    u8* object,u64 originalPair,u64 targetPair,bool pairChanged,
    u32 originalRange,u32 targetRange,bool rangeChanged
){
    bool complete=true;
    if(pairChanged){
        u64 observed=0,current=0;
        if(!compareExchangeU64(object+OFF_SURVEY_SHAPE,originalPair,targetPair,&observed)||
           (observed!=targetPair&&observed!=originalPair)||
           !readU64(object+OFF_SURVEY_SHAPE,&current)||current!=originalPair)complete=false;
    }
    if(rangeChanged){
        u32 observed=0,current=0;
        if(!compareExchangeU32(object+OFF_SURVEY_RANGE,originalRange,targetRange,&observed)||
           (observed!=targetRange&&observed!=originalRange)||
           !readU32(object+OFF_SURVEY_RANGE,&current)||current!=originalRange)complete=false;
    }
    return complete;
}

[[maybe_unused]] static ApplyResult applySurvey(u8* object,const Settings* settings){
    if(!object||!settings||((u64)(object+OFF_SURVEY_SHAPE)&7u)!=0u)return APPLY_FAILED;
    u64 originalPair=((u64)NATIVE_SURVEY_ANGLE_BITS<<32)|NATIVE_SURVEY_SHAPE;
    u64 targetPair=((u64)settings->angleBits<<32)|settings->shapeBits;
    u64 currentPair=0;
    u32 currentRange=0;
    if(!readU64(object+OFF_SURVEY_SHAPE,&currentPair)||
       !readU32(object+OFF_SURVEY_RANGE,&currentRange))return APPLY_NOT_READY;
    if((currentPair!=originalPair&&currentPair!=targetPair)||
       (currentRange!=NATIVE_SURVEY_RANGE_BITS&&currentRange!=settings->rangeBits))return APPLY_CONFLICT;

    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    if(pageSize<4096u||pageSize>65536u||(pageSize&(pageSize-1u))!=0u)return APPLY_FAILED;
    ProtectionPage pages[2];
    u32 pageCount=0;
    if(!addProtectionPage(pages,&pageCount,2u,object+OFF_SURVEY_SHAPE,pageSize)||
       !addProtectionPage(pages,&pageCount,2u,object+OFF_SURVEY_RANGE,pageSize))return APPLY_FAILED;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

    bool rangeChanged=false,pairChanged=false;
    ApplyResult failure=APPLY_READY;
    if(currentRange==NATIVE_SURVEY_RANGE_BITS&&currentRange!=settings->rangeBits){
        u32 observed=0;
        if(!compareExchangeU32(
            object+OFF_SURVEY_RANGE,settings->rangeBits,NATIVE_SURVEY_RANGE_BITS,&observed
        ))failure=APPLY_FAILED;
        else if(observed!=NATIVE_SURVEY_RANGE_BITS)failure=APPLY_CONFLICT;
        else rangeChanged=true;
    }
    if(failure==APPLY_READY&&currentPair==originalPair&&currentPair!=targetPair){
        u64 observed=0;
        if(!compareExchangeU64(object+OFF_SURVEY_SHAPE,targetPair,originalPair,&observed))failure=APPLY_FAILED;
        else if(observed!=originalPair)failure=APPLY_CONFLICT;
        else pairChanged=true;
    }
    u64 verifyPair=0;
    u32 verifyRange=0;
    if(failure==APPLY_READY&&(
        !readU64(object+OFF_SURVEY_SHAPE,&verifyPair)||verifyPair!=targetPair||
        !readU32(object+OFF_SURVEY_RANGE,&verifyRange)||verifyRange!=settings->rangeBits
    ))failure=APPLY_FAILED;
    if(failure!=APPLY_READY){
        bool rolledBack=rollbackSurveyValues(
            object,originalPair,targetPair,pairChanged,
            NATIVE_SURVEY_RANGE_BITS,settings->rangeBits,rangeChanged
        );
        bool restored=restorePages(pages,pageCount,pageSize);
        return rolledBack&&restored?failure:APPLY_CRITICAL;
    }
    if(!restorePages(pages,pageCount,pageSize)){
        if(!makePagesWritable(pages,pageCount,pageSize))return APPLY_CRITICAL;
        bool rolledBack=rollbackSurveyValues(
            object,originalPair,targetPair,pairChanged,
            NATIVE_SURVEY_RANGE_BITS,settings->rangeBits,rangeChanged
        );
        bool restored=restorePages(pages,pageCount,pageSize);
        return rolledBack&&restored?APPLY_FAILED:APPLY_CRITICAL;
    }
    return APPLY_READY;
}

static bool buildAtLeastFloatField(
    u8* address,float target,bool positive,bool allowLarge,FieldPatch* field
){
    if(!address||!field||!(target>0.0f))return false;
    u32 currentBits=0;
    float current=0.0f;
    if(!readU32(address,&currentBits)||!finiteFloat(currentBits,&current)||
       (positive?!(current>0.0f):current<0.0f)||
       (!allowLarge&&current>MAX_PLAUSIBLE_RUNTIME_VALUE))return false;
    field->address=address;
    field->original=currentBits;
    field->target=floatBits(current>=target?current:target);
    field->changed=false;
    return true;
}

static void buildExactTransitionField(
    u8* address,u32 nativeValue,u32 extendedValue,bool activate,
    FieldPatch* field
){
    field->address=address;
    field->original=activate?nativeValue:extendedValue;
    field->target=activate?extendedValue:nativeValue;
    field->changed=false;
}

static bool buildCargoMarkerFields(
    const CargoMarkerObjects* objects,const Settings* settings,bool scanActive,
    FieldPatch* fields
){
    if(!objects||!objects->manager||!objects->config||!settings||!fields)return false;
    float target=settings->rangeMeters;
    float nativeDistance=0.0f;
    if(!finitePositiveFloat(
           NATIVE_BAGGAGE_UNTOUCHED_DISTANCE_BITS,&nativeDistance
       ))return false;
    float extendedDistance=target>nativeDistance?target:nativeDistance;
    // Only untouched Lost Cargo belongs to this feature. Touched/destroyed
    // containers and mission cargo keep their native presentation distances.
    buildExactTransitionField(
        objects->config+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE,
        NATIVE_BAGGAGE_UNTOUCHED_DISTANCE_BITS,floatBits(extendedDistance),
        scanActive,&fields[0]
    );
    buildExactTransitionField(
        objects->manager+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED,
        NATIVE_BAGGAGE_UNTOUCHED_CACHE_BITS,
        floatBits(extendedDistance*extendedDistance),scanActive,&fields[1]
    );
    return true;
}

static bool sameCargoMarkerObjects(
    const CargoMarkerObjects* left,const CargoMarkerObjects* right
){
    return left&&right&&left->manager==right->manager&&left->config==right->config;
}

static DirectIdentityResult validateCargoMarkerConfigDirect(
    const GameImage* image,const CargoMarkerObjects* objects
){
    if(!objects||!objects->config)return DIRECT_UNREADABLE;
    DirectIdentityResult identity=directVtableIdentity(
        image,objects->config,RVA_BAGGAGE_CONFIG_VTABLE
    );
    if(identity!=DIRECT_VALID)return identity;
    return rangeReadable(
        objects->config+OFF_BAGGAGE_CONFIG_UNTOUCHED_DISTANCE,4u
    )?DIRECT_VALID:DIRECT_UNREADABLE;
}

static DirectIdentityResult validateCargoMarkerManagerDirect(
    const GameImage* image,const CargoMarkerObjects* objects
){
    if(!objects||!objects->manager)return DIRECT_UNREADABLE;
    DirectIdentityResult primary=directVtableIdentity(
        image,objects->manager,RVA_BAGGAGE_MANAGER_VTABLE
    );
    if(primary!=DIRECT_VALID)return primary;
    DirectIdentityResult secondary=directVtableIdentity(
        image,objects->manager+8u,RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE
    );
    if(secondary!=DIRECT_VALID)return secondary;
    return rangeReadable(
        objects->manager+OFF_BAGGAGE_UNTOUCHED_CACHE_SQUARED,4u
    )?DIRECT_VALID:DIRECT_UNREADABLE;
}

static ApplyResult mergeApplyResult(ApplyResult current,ApplyResult next){
    if(current==APPLY_CRITICAL||next==APPLY_CRITICAL)return APPLY_CRITICAL;
    if(current==APPLY_FAILED||next==APPLY_FAILED)return APPLY_FAILED;
    if(current==APPLY_NOT_READY||next==APPLY_NOT_READY)return APPLY_NOT_READY;
    if(current==APPLY_CONFLICT||next==APPLY_CONFLICT)return APPLY_CONFLICT;
    return APPLY_READY;
}

static bool cargoMarkerOwnershipPresent(const CargoMarkerOwnership* ownership){
    return ownership&&(ownership->fields[0].owned||ownership->fields[1].owned);
}

static ApplyResult drainCargoMarkerOwnership(
    const GameImage* image,CargoMarkerOwnership* ownership
){
    if(!ownership||!cargoMarkerOwnershipPresent(ownership))return APPLY_READY;
    ApplyResult aggregate=APPLY_READY;
    for(u32 i=0u;i<2u;i++){
        OwnedField* owned=&ownership->fields[i];
        if(!owned->owned)continue;
        DirectIdentityResult parentIdentity=i==0u?
            validateCargoMarkerConfigDirect(image,&ownership->objects):
            validateCargoMarkerManagerDirect(image,&ownership->objects);
        if(parentIdentity!=DIRECT_VALID){
            // A positively different VTable proves pointer reuse and releases
            // only this record. Transient unreadability retains ownership so
            // the next worker pass/final drain can retry safely.
            if(parentIdentity==DIRECT_MISMATCH)owned->owned=false;
            aggregate=mergeApplyResult(aggregate,APPLY_NOT_READY);
            continue;
        }
        FieldPatch reverse;
        buildExactTransitionField(
            owned->address,owned->nativeValue,owned->extendedValue,false,&reverse
        );
        ApplyResult result=applyFieldTransaction(&reverse,1u);
        if(result==APPLY_READY||result==APPLY_CONFLICT){
            // READY means restored/already native. CONFLICT means another
            // writer now owns a third value, which must remain untouched.
            owned->owned=false;
        }
        aggregate=mergeApplyResult(aggregate,result);
    }
    if(!cargoMarkerOwnershipPresent(ownership)){
        memset(ownership,0,sizeof(*ownership));
    }
    return aggregate;
}

static bool validateCargoMarkerTarget(
    const GameImage* image,const CargoMarkerObjects* expected,const Settings* settings,
    bool scanActive
){
    if(!image||!expected||!settings)return false;
    CargoMarkerObjects current;
    if(resolveCargoMarkerObjects(image,&current)!=RESOLVE_READY||
       current.manager!=expected->manager||current.config!=expected->config)return false;
    FieldPatch fields[2];
    if(!buildCargoMarkerFields(&current,settings,scanActive,fields))return false;
    for(u32 i=0;i<2u;i++){
        u32 bits=0;
        if(!readU32(fields[i].address,&bits)||bits!=fields[i].target)return false;
    }
    return true;
}

static ApplyResult applyCargoMarkerRanges(
    const GameImage* image,const CargoMarkerObjects* expected,const Settings* settings,
    bool scanActive,u32* changedMask=0
){
    if(changedMask)*changedMask=0u;
    if(!image||!expected||!settings)return APPLY_FAILED;
    CargoMarkerObjects current;
    ResolveResult resolved=resolveCargoMarkerObjects(image,&current);
    if(resolved==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(resolved!=RESOLVE_READY)return APPLY_FAILED;
    if(!sameCargoMarkerObjects(&current,expected))return APPLY_NOT_READY;
    FieldPatch fields[2];
    if(!buildCargoMarkerFields(&current,settings,scanActive,fields))return APPLY_NOT_READY;
    ApplyResult applied=applyFieldTransaction(fields,2u);
    if(applied!=APPLY_READY)return applied;
    if(changedMask){
        for(u32 i=0u;i<2u;i++)if(fields[i].changed)*changedMask|=1u<<i;
    }
    return APPLY_READY;
}

static ApplyResult reconcileCargoMarkerRanges(
    const GameImage* image,const CargoMarkerObjects* expected,
    const Settings* settings,CargoMarkerOwnership* ownership,bool* activeOut
){
    if(activeOut)*activeOut=false;
    for(u32 attempt=0u;attempt<3u;attempt++){
        u32 serial=0u;
        u64 state=scanPresentationSnapshot(&serial,0);
        // Genuine Realized-Baggage components own discovery and lifetime.
        // Global untouched-cargo ranges must remain native even while the
        // scan wave is active.
        bool active=false;

        if(cargoMarkerOwnershipPresent(ownership)&&(
               !active||ownership->serial!=serial||!expected||
               !sameCargoMarkerObjects(&ownership->objects,expected)
           )){
            ApplyResult drained=drainCargoMarkerOwnership(image,ownership);
            u64 afterDrain=scanPresentationSnapshot(0,0);
            if(drained==APPLY_CRITICAL)return APPLY_CRITICAL;
            if(afterDrain!=state)continue;
            if(drained!=APPLY_READY&&drained!=APPLY_CONFLICT)return drained;
            if(!active){
                if(activeOut)*activeOut=false;
                return drained;
            }
        }

        if(!active){
            if(activeOut)*activeOut=false;
            return APPLY_READY;
        }
        if(!expected)return APPLY_NOT_READY;

        ApplyResult result=APPLY_READY;
        if(!validateCargoMarkerTarget(image,expected,settings,active)){
            FieldPatch candidateFields[2];
            if(!buildCargoMarkerFields(
                   expected,settings,true,candidateFields
               ))return APPLY_FAILED;
            bool previouslyOwned[2]={
                ownership&&ownership->fields[0].owned,
                ownership&&ownership->fields[1].owned
            };
            bool candidateOwned[2]={false,false};
            if(ownership){
                for(u32 i=0u;i<2u;i++){
                    u32 current=0u;
                    if(readU32(candidateFields[i].address,&current)&&
                       current==candidateFields[i].original&&
                       candidateFields[i].original!=candidateFields[i].target){
                        ownership->objects=*expected;
                        ownership->serial=serial;
                        ownership->fields[i].address=candidateFields[i].address;
                        ownership->fields[i].nativeValue=candidateFields[i].original;
                        ownership->fields[i].extendedValue=candidateFields[i].target;
                        ownership->fields[i].owned=true;
                        candidateOwned[i]=true;
                    }
                }
            }
            u32 changedMask=0u;
            result=applyCargoMarkerRanges(
                image,expected,settings,active,&changedMask
            );
            if(ownership){
                for(u32 i=0u;i<2u;i++){
                    if(result==APPLY_READY&&(changedMask&(1u<<i))){
                        ownership->objects=*expected;
                        ownership->serial=serial;
                        ownership->fields[i].address=candidateFields[i].address;
                        ownership->fields[i].nativeValue=candidateFields[i].original;
                        ownership->fields[i].extendedValue=candidateFields[i].target;
                        ownership->fields[i].owned=true;
                    }else if(candidateOwned[i]&&!previouslyOwned[i]&&
                             result!=APPLY_CRITICAL){
                        ownership->fields[i].owned=false;
                    }
                }
                if(!cargoMarkerOwnershipPresent(ownership)){
                    memset(ownership,0,sizeof(*ownership));
                }
            }
            // A critical result can mean that page protection was not fully
            // restored. Do not let an unrelated identity/epoch change below
            // turn that fatal transaction result into a harmless retry.
            if(result==APPLY_CRITICAL)return APPLY_CRITICAL;
        }
        CargoMarkerObjects confirmed;
        bool identityStable=resolveCargoMarkerObjects(image,&confirmed)==RESOLVE_READY&&
                            sameCargoMarkerObjects(&confirmed,expected);
        u64 verifiedState=scanPresentationSnapshot(0,0);
        if(verifiedState!=state||!identityStable){
            ApplyResult drained=drainCargoMarkerOwnership(image,ownership);
            if(drained==APPLY_CRITICAL)return APPLY_CRITICAL;
            continue;
        }
        if(result==APPLY_READY){
            if(activeOut)*activeOut=active;
            return APPLY_READY;
        }
        return result;
    }
    return APPLY_NOT_READY;
}

static bool buildPlayerCargoFields(
    u8* config,const Settings* settings,FieldPatch* fields
){
    if(!config||!settings||!fields)return false;
    return buildAtLeastFloatField(
               config+OFF_PLAYER_CARGO_DETECT_RANGE,
               settings->rangeMeters,false,false,&fields[0]
           )&&buildAtLeastFloatField(
               config+OFF_PLAYER_CARGO_SEARCH_RANGE,
               settings->rangeMeters,false,false,&fields[1]
           );
}

static bool validatePlayerCargoTarget(
    const GameImage* image,u8* expected,const Settings* settings
){
    if(!image||!expected||!settings)return false;
    u8* current=0;
    if(resolvePlayerOdradekConfig(image,&current)!=RESOLVE_READY||
       current!=expected)return false;
    const u32 offsets[2]={
        OFF_PLAYER_CARGO_DETECT_RANGE,OFF_PLAYER_CARGO_SEARCH_RANGE
    };
    for(u32 i=0;i<2u;i++){
        u32 bits=0;
        float value=0.0f;
        if(!readU32(current+offsets[i],&bits)||!finiteFloat(bits,&value)||
           value<settings->rangeMeters)return false;
    }
    return true;
}

static ApplyResult applyPlayerCargoRanges(
    const GameImage* image,u8* expected,const Settings* settings
){
    if(!image||!expected||!settings)return APPLY_FAILED;
    u8* current=0;
    ResolveResult resolved=resolvePlayerOdradekConfig(image,&current);
    if(resolved==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(resolved!=RESOLVE_READY)return APPLY_FAILED;
    if(current!=expected)return APPLY_NOT_READY;
    FieldPatch fields[2];
    if(!buildPlayerCargoFields(current,settings,fields))return APPLY_NOT_READY;
    u8* confirmed=0;
    if(resolvePlayerOdradekConfig(image,&confirmed)!=RESOLVE_READY||
       confirmed!=current)return APPLY_NOT_READY;
    return applyFieldTransaction(fields,2u);
}

static u32 buildHudFields(
    u8* parameters,const Settings* settings,bool scanActive,
    FieldPatch* fields,u32 capacity
){
    if(!parameters||!settings||!fields||capacity<1u)return 0u;
    // Never include the shared searched-marker field (+0x30) in a write
    // transaction. It is used by unrelated equipment/world-marker classes.
    buildExactTransitionField(
        parameters+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE,
        NATIVE_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE_BITS,
        hudBaggageFoundTarget(settings,true),
        scanActive&&settings->extendCargoMarkers,&fields[0]
    );
    return 1u;
}

static bool validateHudTarget(
    const GameImage* image,u8* parameters,const Settings* settings,bool scanActive
){
    if(!image||!parameters||!settings)return false;
    u8* confirmed=0;
    if(resolveUiGameDesign(image,&confirmed)!=RESOLVE_READY||confirmed!=parameters)return false;
    FieldPatch fields[1];
    u32 count=buildHudFields(parameters,settings,scanActive,fields,1u);
    if(!count)return false;
    for(u32 i=0;i<count;i++){
        u32 value=0;
        if(!readU32(fields[i].address,&value)||value!=fields[i].target)return false;
    }
    return true;
}

static DirectIdentityResult validateHudParametersDirect(
    const GameImage* image,u8* parameters
){
    DirectIdentityResult identity=directVtableIdentity(
        image,parameters,RVA_UI_GAME_PARAMS_VTABLE
    );
    if(identity!=DIRECT_VALID)return identity;
    return rangeReadable(
        parameters+OFF_UI_BAGGAGE_FOUND_VISIBLE_DISTANCE,4u
    )?DIRECT_VALID:DIRECT_UNREADABLE;
}

static ApplyResult drainHudMarkerOwnership(
    const GameImage* image,HudMarkerOwnership* ownership
){
    if(!ownership||!ownership->field.owned)return APPLY_READY;
    DirectIdentityResult identity=validateHudParametersDirect(
        image,ownership->parameters
    );
    if(identity!=DIRECT_VALID){
        if(identity==DIRECT_MISMATCH)memset(ownership,0,sizeof(*ownership));
        return APPLY_NOT_READY;
    }
    FieldPatch reverse;
    buildExactTransitionField(
        ownership->field.address,ownership->field.nativeValue,
        ownership->field.extendedValue,false,&reverse
    );
    ApplyResult result=applyFieldTransaction(&reverse,1u);
    if(result==APPLY_READY||result==APPLY_CONFLICT){
        memset(ownership,0,sizeof(*ownership));
    }
    return result;
}

static ApplyResult applyHudMarkers(
    const GameImage* image,u8* parameters,const Settings* settings,bool scanActive,
    bool* changedOut=0
){
    if(changedOut)*changedOut=false;
    if(!image||!parameters||!settings)return APPLY_FAILED;
    u8* confirmed=0;
    ResolveResult identity=resolveUiGameDesign(image,&confirmed);
    if(identity==RESOLVE_NOT_READY)return APPLY_NOT_READY;
    if(identity!=RESOLVE_READY)return APPLY_FAILED;
    if(confirmed!=parameters)return APPLY_NOT_READY;

    FieldPatch fields[1];
    u32 count=buildHudFields(parameters,settings,scanActive,fields,1u);
    if(!count)return APPLY_FAILED;
    for(u32 i=0;i<count;i++){
        u32 current=0;
        if(!readU32(fields[i].address,&current))return APPLY_NOT_READY;
        if(!current)return APPLY_NOT_READY;
        if(current!=fields[i].original&&current!=fields[i].target)return APPLY_CONFLICT;
    }

    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    if(pageSize<4096u||pageSize>65536u||(pageSize&(pageSize-1u))!=0u)return APPLY_FAILED;
    ProtectionPage pages[2];
    u32 pageCount=0;
    if(!collectFieldPages(fields,count,pages,&pageCount,2u,pageSize))return APPLY_FAILED;
    if(!makePagesWritable(pages,pageCount,pageSize)){
        return restorePages(pages,pageCount,pageSize)?APPLY_FAILED:APPLY_CRITICAL;
    }

    confirmed=0;
    identity=resolveUiGameDesign(image,&confirmed);
    if(identity!=RESOLVE_READY||confirmed!=parameters){
        bool restored=restorePages(pages,pageCount,pageSize);
        if(!restored)return APPLY_CRITICAL;
        return identity==RESOLVE_INVALID?APPLY_FAILED:APPLY_NOT_READY;
    }

    ApplyResult failure=APPLY_READY;
    for(u32 i=0;i<count;i++){
        u32 current=0;
        if(!readU32(fields[i].address,&current)){failure=APPLY_FAILED;break;}
        if(current==fields[i].target)continue;
        if(current!=fields[i].original){failure=APPLY_CONFLICT;break;}
        u32 observed=0;
        if(!compareExchangeU32(
            fields[i].address,fields[i].target,fields[i].original,&observed
        )){failure=APPLY_FAILED;break;}
        if(observed!=fields[i].original){failure=APPLY_CONFLICT;break;}
        fields[i].changed=true;
        u32 verified=0;
        if(!readU32(fields[i].address,&verified)||verified!=fields[i].target){
            failure=APPLY_FAILED;
            break;
        }
    }
    if(failure==APPLY_READY){
        confirmed=0;
        identity=resolveUiGameDesign(image,&confirmed);
        if(identity!=RESOLVE_READY||confirmed!=parameters){
            failure=identity==RESOLVE_INVALID?APPLY_FAILED:APPLY_NOT_READY;
        }
    }
    if(failure==APPLY_READY){
        for(u32 i=0;i<count;i++){
            u32 verified=0;
            if(!readU32(fields[i].address,&verified)||verified!=fields[i].target){
                failure=APPLY_FAILED;
                break;
            }
        }
    }
    if(failure!=APPLY_READY){
        bool rolledBack=rollbackFields(fields,count);
        bool restored=restorePages(pages,pageCount,pageSize);
        return rolledBack&&restored?failure:APPLY_CRITICAL;
    }
    if(!restorePages(pages,pageCount,pageSize)){
        if(!makePagesWritable(pages,pageCount,pageSize))return APPLY_CRITICAL;
        bool rolledBack=rollbackFields(fields,count);
        bool restored=restorePages(pages,pageCount,pageSize);
        return rolledBack&&restored?APPLY_FAILED:APPLY_CRITICAL;
    }
    if(changedOut)*changedOut=fields[0].changed;
    return APPLY_READY;
}

static ApplyResult reconcileHudMarkers(
    const GameImage* image,u8* parameters,const Settings* settings,
    HudMarkerOwnership* ownership,bool* activeOut
){
    if(activeOut)*activeOut=false;
    for(u32 attempt=0u;attempt<3u;attempt++){
        u32 serial=0u;
        u64 state=scanPresentationSnapshot(&serial,0);
        // The global baggage-found distance exposes every known candidate.
        // Exact native Realized-Baggage processing does not require it, so
        // keep it native.
        bool active=false;

        if(ownership&&ownership->field.owned&&(
               !active||ownership->serial!=serial||
               ownership->parameters!=parameters
           )){
            ApplyResult drained=drainHudMarkerOwnership(image,ownership);
            u64 afterDrain=scanPresentationSnapshot(0,0);
            if(drained==APPLY_CRITICAL)return APPLY_CRITICAL;
            if(afterDrain!=state)continue;
            if(drained!=APPLY_READY&&drained!=APPLY_CONFLICT)return drained;
            if(!active){
                if(activeOut)*activeOut=false;
                return drained;
            }
        }

        if(!active){
            if(activeOut)*activeOut=false;
            return APPLY_READY;
        }
        if(!parameters)return APPLY_NOT_READY;

        ApplyResult result=APPLY_READY;
        if(!validateHudTarget(image,parameters,settings,active)){
            FieldPatch candidate;
            if(!buildHudFields(
                   parameters,settings,true,&candidate,1u
               ))return APPLY_FAILED;
            bool previouslyOwned=ownership&&ownership->field.owned;
            bool candidateOwned=false;
            if(ownership){
                u32 current=0u;
                if(readU32(candidate.address,&current)&&
                   current==candidate.original&&
                   candidate.original!=candidate.target){
                    ownership->parameters=parameters;
                    ownership->serial=serial;
                    ownership->field.address=candidate.address;
                    ownership->field.nativeValue=candidate.original;
                    ownership->field.extendedValue=candidate.target;
                    ownership->field.owned=true;
                    candidateOwned=true;
                }
            }
            bool changed=false;
            result=applyHudMarkers(
                image,parameters,settings,active,&changed
            );
            if(result==APPLY_READY&&changed&&ownership){
                ownership->parameters=parameters;
                ownership->serial=serial;
                ownership->field.address=candidate.address;
                ownership->field.nativeValue=candidate.original;
                ownership->field.extendedValue=candidate.target;
                ownership->field.owned=true;
            }else if(candidateOwned&&!previouslyOwned&&
                     result!=APPLY_CRITICAL&&ownership){
                memset(ownership,0,sizeof(*ownership));
            }
            // Preserve fatal transaction failures even if the target or scan
            // epoch changes before the post-write stability check.
            if(result==APPLY_CRITICAL)return APPLY_CRITICAL;
        }
        u8* confirmed=0;
        bool identityStable=resolveUiGameDesign(image,&confirmed)==RESOLVE_READY&&
                            confirmed==parameters;
        u64 verifiedState=scanPresentationSnapshot(0,0);
        if(verifiedState!=state||!identityStable){
            ApplyResult drained=drainHudMarkerOwnership(image,ownership);
            if(drained==APPLY_CRITICAL)return APPLY_CRITICAL;
            continue;
        }
        if(result==APPLY_READY){
            if(activeOut)*activeOut=active;
            return APPLY_READY;
        }
        return result;
    }
    return APPLY_NOT_READY;
}

static ApplyResult rollbackPointerHook(
    u8* address,u64 expected,u64 replacement,DWORD protection
){
    DWORD currentProtection=0;
    if(!queryProtection(address,&currentProtection))return APPLY_CRITICAL;
    bool compatibleProtection=protectionValueMatchesAfterOwnedWrite(currentProtection,protection);
    bool protectionChanged=false;
    if(!isWritableProtection(currentProtection)){
        DWORD before=0;
        if(!VirtualProtect(address,8u,PAGE_READWRITE,&before)||before!=currentProtection){
            if(before&&before!=currentProtection){
                DWORD ignored=0;
                VirtualProtect(address,8u,before,&ignored);
            }
            return APPLY_CRITICAL;
        }
        protectionChanged=true;
    }
    bool restoredPointer=false;
    u64 current=0;
    if(readU64(address,&current)){
        if(current==expected){
            restoredPointer=true;
        }else if(current==replacement){
            u64 observed=0;
            restoredPointer=compareExchangeU64(address,expected,replacement,&observed)&&
                            (observed==replacement||observed==expected)&&
                            readU64(address,&current)&&current==expected;
        }
    }
    bool restoredProtection=true;
    if(protectionChanged){
        DWORD ignored=0;
        restoredProtection=VirtualProtect(address,8u,currentProtection,&ignored)!=FALSE&&
                           protectionMatchesAfterOwnedWrite(address,currentProtection);
    }else{
        restoredProtection=protectionMatchesAfterOwnedWrite(address,protection);
    }
    return restoredPointer&&restoredProtection&&compatibleProtection?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult installPointerHook(
    u8* address,u64 expected,u64 replacement,DWORD* originalProtection,bool* changed
){
    if(!address||!expected||!replacement||!originalProtection||!changed||((u64)address&7u)!=0u)return APPLY_FAILED;
    *changed=false;
    u64 current=0;
    if(!readU64(address,&current))return APPLY_FAILED;
    if(current==replacement)return APPLY_READY;
    if(current!=expected)return APPLY_CONFLICT;
    DWORD protection=0;
    if(!queryProtection(address,&protection)||(protection&PAGE_GUARD)||
       ((protection&0xFFu)==PAGE_NOACCESS))return APPLY_FAILED;
    *originalProtection=protection;
    bool protectionChanged=false;
    if(!isWritableProtection(protection)){
        DWORD before=0;
        if(!VirtualProtect(address,8u,PAGE_READWRITE,&before))return APPLY_FAILED;
        if(before!=protection){
            DWORD ignored=0;
            bool restored=VirtualProtect(address,8u,before,&ignored)!=FALSE&&
                          protectionMatches(address,before);
            return restored?APPLY_CONFLICT:APPLY_CRITICAL;
        }
        protectionChanged=true;
    }
    if(!readU64(address,&current)||current!=expected){
        bool restored=true;
        if(protectionChanged){
            DWORD ignored=0;
            restored=VirtualProtect(address,8u,protection,&ignored)!=FALSE&&
                     protectionMatches(address,protection);
        }
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    u64 observed=0;
    if(!compareExchangeU64(address,replacement,expected,&observed)||observed!=expected){
        bool restored=true;
        if(protectionChanged){
            DWORD ignored=0;
            restored=VirtualProtect(address,8u,protection,&ignored)!=FALSE&&
                     protectionMatches(address,protection);
        }
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    *changed=true;
    if(!readU64(address,&current)||current!=replacement){
        return rollbackPointerHook(address,expected,replacement,protection);
    }
    if(protectionChanged){
        DWORD ignored=0;
        if(!VirtualProtect(address,8u,protection,&ignored)||
           !protectionMatchesAfterOwnedWrite(address,protection)){
            return rollbackPointerHook(address,expected,replacement,protection);
        }
    }else if(!protectionMatchesAfterOwnedWrite(address,protection)){
        return rollbackPointerHook(address,expected,replacement,protection);
    }
    return APPLY_READY;
}

static bool relayDisplacement(
    const u8* originInstruction,const u8* relay,u32* displacement
){
    if(!originInstruction||!relay||!displacement)return false;
    const u8* nextInstruction=originInstruction+5u;
    s64 delta=(s64)(u64)relay-(s64)(u64)nextInstruction;
    if(delta<(-2147483647ll-1ll)||delta>2147483647ll)return false;
    *displacement=(u32)(s32)delta;
    return true;
}

static bool gateRelayDisplacement(const u8* base,const u8* relay,u32* displacement){
    return base&&relayDisplacement(base+RVA_SENSOR_GATE_CALL,relay,displacement);
}

static u8* tryAllocateRelay(
    const u8* originInstruction,u64 candidate,u32 pageSize,u64 wrapperAddress,
    u32 captureMode
){
    if(!originInstruction||!candidate||!pageSize||!wrapperAddress||captureMode>2u)return 0;
    u8* relay=(u8*)VirtualAlloc(
        (LPVOID)candidate,(SIZE_T)pageSize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE
    );
    if(!relay)return 0;
    if(relay!=(u8*)candidate){
        VirtualFree(relay,0,MEM_RELEASE);
        return 0;
    }
    u32 displacement=0;
    if(!relayDisplacement(originInstruction,relay,&displacement)){
        VirtualFree(relay,0,MEM_RELEASE);
        return 0;
    }
    u8 code[26]={0};
    u32 codeSize=12u;
    u32 addressOffset=2u;
    code[0]=0x48;
    code[1]=0xB8;
    if(captureMode==1u){
        code[0]=0x49;
        code[1]=0x89;
        code[2]=0xF9;
        code[3]=0x48;
        code[4]=0xB8;
        addressOffset=5u;
        codeSize=15u;
    }
    memcpy(code+addressOffset,&wrapperAddress,8u);
    code[addressOffset+8u]=0xFF;
    code[addressOffset+9u]=0xE0;
    if(captureMode==2u){
        // Preserve all four native arguments; append RDI as argument five.
        // Entry RSP is 8 mod 16. Allocate shadow space + fifth argument.
        const u8 fifth[]={0x48,0x83,0xEC,0x28,0x48,0x89,0x7C,0x24,0x20,
            0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xD0,0x48,0x83,0xC4,0x28,0xC3};
        memcpy(code,fifth,sizeof(fifth));memcpy(code+11u,&wrapperAddress,8u);
        codeSize=sizeof(fifth);
    }
    memcpy(relay,code,codeSize);
    if(!bytesEqual(relay,code,codeSize)){
        VirtualFree(relay,0,MEM_RELEASE);
        return 0;
    }
    DWORD originalProtection=0;
    if(!VirtualProtect(relay,pageSize,PAGE_EXECUTE_READ,&originalProtection)||
       !FlushInstructionCache(GetCurrentProcess(),relay,codeSize)||
       !protectionMatches(relay,PAGE_EXECUTE_READ)||
       !bytesEqual(relay,code,codeSize)){
        VirtualFree(relay,0,MEM_RELEASE);
        return 0;
    }
    return relay;
}

static u8* scanForRelay(
    const u8* originInstruction,u64 begin,u64 end,u32 pageSize,
    u32 allocationGranularity,u64 wrapperAddress,u32 captureMode
){
    if(!originInstruction||begin>=end||!pageSize||!allocationGranularity)return 0;
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
                if(relayDisplacement(originInstruction,(u8*)candidate,&displacement)){
                    u8* relay=tryAllocateRelay(
                        originInstruction,candidate,pageSize,wrapperAddress,
                        captureMode
                    );
                    if(relay)return relay;
                }
            }
        }
        cursor=regionEnd;
    }
    return 0;
}

static u8* allocateRelay(
    const u8* base,u32 imageSize,u64 originRva,u64 wrapperAddress,
    u32 captureMode
){
    if(!base||!imageSize||originRva+5u>(u64)imageSize||!wrapperAddress)return 0;
    SYSTEM_INFO_X64 systemInfo;
    memset(&systemInfo,0,sizeof(systemInfo));
    GetSystemInfo(&systemInfo);
    u32 pageSize=systemInfo.PageSize;
    u32 granularity=systemInfo.AllocationGranularity;
    if(pageSize<4096u||pageSize>65536u||(pageSize&(pageSize-1u))!=0u||
       granularity<pageSize||(granularity&(granularity-1u))!=0u)return 0;

    const u8* originInstruction=base+originRva;
    u64 origin=(u64)(originInstruction+5u);
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
    u8* relay=0;
    if(preferred<upper){
        relay=scanForRelay(
            originInstruction,preferred,upper,pageSize,granularity,wrapperAddress,
            captureMode
        );
    }
    if(!relay&&lower<preferred){
        relay=scanForRelay(
            originInstruction,lower,preferred,pageSize,granularity,wrapperAddress,
            captureMode
        );
    }
    return relay;
}

static u8* allocateGateRelay(
    const u8* base,u32 imageSize,u64 wrapperAddress
){
    return allocateRelay(
        base,imageSize,RVA_SENSOR_GATE_CALL,wrapperAddress,true
    );
}

static bool gateTelemetryPatchMatches(const u8* base,u64 replacement){
    u64 current=0;
    return base&&readU64(base+RVA_SENSOR_GATE_CALL_WINDOW,&current)&&current==replacement;
}

static ApplyResult rollbackGateTelemetryPatch(
    u8* base,u64 replacement,DWORD originalProtection
){
    static const u64 expected=0xC084FF10136AE800ull;
    u8* address=base+RVA_SENSOR_GATE_CALL_WINDOW;
    DWORD beforeRollback=0;
    bool writable=VirtualProtect(address,8u,PAGE_EXECUTE_READWRITE,&beforeRollback)!=FALSE;
    bool restoredBytes=false;
    if(writable){
        u64 current=0;
        if(readU64(address,&current)){
            if(current==expected){
                restoredBytes=true;
            }else if(current==replacement){
                u64 observed=0;
                restoredBytes=compareExchangeU64(
                    address,expected,replacement,&observed
                )&&(observed==replacement||observed==expected);
            }
        }
        if(!FlushInstructionCache(GetCurrentProcess(),address,8u))restoredBytes=false;
        if(!readU64(address,&current)||current!=expected)restoredBytes=false;
    }
    DWORD ignored=0;
    bool restoredProtection=writable&&
        VirtualProtect(address,8u,originalProtection,&ignored)!=FALSE&&
        protectionMatchesAfterOwnedWrite(address,originalProtection);
    return restoredBytes&&restoredProtection?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult installGateTelemetry(
    u8* base,u32 imageSize,u8** relayOut
){
    static const u64 expected=0xC084FF10136AE800ull;
    if(!base||!imageSize||!relayOut||
       ((u64)(base+RVA_SENSOR_GATE_CALL_WINDOW)&7u)!=0u)return APPLY_FAILED;
    *relayOut=0;
    if(!bytesEqual(
        base+RVA_SENSOR_GATE_CALL_WINDOW,EXPECTED_SENSOR_GATE_CALL_WINDOW,
        (u32)sizeof(EXPECTED_SENSOR_GATE_CALL_WINDOW)
    ))return APPLY_CONFLICT;
    u8* relay=allocateGateRelay(
        base,imageSize,(u64)(const void*)&sensorGateTelemetryHook
    );
    if(!relay)return APPLY_FAILED;
    u32 displacement=0;
    if(!gateRelayDisplacement(base,relay,&displacement)){
        VirtualFree(relay,0,MEM_RELEASE);
        return APPLY_FAILED;
    }
    u8 replacementBytes[8];
    memcpy(replacementBytes,EXPECTED_SENSOR_GATE_CALL_WINDOW,8u);
    memcpy(replacementBytes+2u,&displacement,4u);
    u64 replacement=0;
    memcpy(&replacement,replacementBytes,8u);

    DWORD originalProtection=0;
    u8* address=base+RVA_SENSOR_GATE_CALL_WINDOW;
    if(!VirtualProtect(address,8u,PAGE_EXECUTE_READWRITE,&originalProtection)){
        VirtualFree(relay,0,MEM_RELEASE);
        return APPLY_FAILED;
    }
    if(!bytesEqual(
        address,EXPECTED_SENSOR_GATE_CALL_WINDOW,
        (u32)sizeof(EXPECTED_SENSOR_GATE_CALL_WINDOW)
    )){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,8u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        VirtualFree(relay,0,MEM_RELEASE);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    u64 observed=0;
    bool published=compareExchangeU64(address,replacement,expected,&observed)&&observed==expected;
    if(!published){
        DWORD ignored=0;
        bool restored=VirtualProtect(address,8u,originalProtection,&ignored)!=FALSE&&
                      protectionMatches(address,originalProtection);
        VirtualFree(relay,0,MEM_RELEASE);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    if(!FlushInstructionCache(GetCurrentProcess(),address,8u)||
       !gateTelemetryPatchMatches(base,replacement)){
        *relayOut=relay;
        return rollbackGateTelemetryPatch(base,replacement,originalProtection);
    }
    DWORD ignored=0;
    if(!VirtualProtect(address,8u,originalProtection,&ignored)||
       !protectionMatchesAfterOwnedWrite(address,originalProtection)){
        *relayOut=relay;
        return rollbackGateTelemetryPatch(base,replacement,originalProtection);
    }
    *relayOut=relay;
    return APPLY_READY;
}

static bool realizationCoordinatorRelayDisplacement(
    const u8* base,const u8* relay,u32* displacement
){
    return base&&relayDisplacement(
        base+RVA_REALIZATION_COORDINATOR_CALL,relay,displacement
    );
}

static bool realizationCoordinatorPatchMatches(
    const u8* base,u64 replacement
){
    u64 current=0u;
    return base&&readU64(
        base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW,&current
    )&&current==replacement;
}

static ApplyResult rollbackRealizationCoordinatorPatch(
    u8* base,u64 replacement,DWORD originalProtection
){
    static const u64 expected=0x48FFFEF6C1E8CB8Bull;
    u8* address=base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW;
    DWORD beforeRollback=0;
    bool writable=VirtualProtect(
        address,8u,PAGE_EXECUTE_READWRITE,&beforeRollback
    )!=FALSE;
    bool restoredBytes=false;
    if(writable){
        u64 current=0u;
        if(readU64(address,&current)){
            if(current==expected){
                restoredBytes=true;
            }else if(current==replacement){
                u64 observed=0u;
                restoredBytes=compareExchangeU64(
                    address,expected,replacement,&observed
                )&&(observed==replacement||observed==expected);
            }
        }
        if(!FlushInstructionCache(GetCurrentProcess(),address,8u)){
            restoredBytes=false;
        }
        if(!readU64(address,&current)||current!=expected)restoredBytes=false;
    }
    DWORD ignored=0;
    bool restoredProtection=writable&&VirtualProtect(
        address,8u,originalProtection,&ignored
    )!=FALSE&&protectionMatchesAfterOwnedWrite(address,originalProtection);
    return restoredBytes&&restoredProtection?APPLY_FAILED:APPLY_CRITICAL;
}

static ApplyResult installRealizationCoordinatorHook(
    u8* base,u32 imageSize,u8** relayOut
){
    static const u64 expected=0x48FFFEF6C1E8CB8Bull;
    if(!base||!imageSize||!relayOut||
       ((u64)(base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW)&7u)!=0u){
        return APPLY_FAILED;
    }
    *relayOut=0;
    if(!bytesEqual(
           base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW,
           EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW,
           (u32)sizeof(EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW)
       ))return APPLY_CONFLICT;
    u8* relay=allocateRelay(
        base,imageSize,RVA_REALIZATION_COORDINATOR_CALL,
        (u64)(const void*)&realizationCoordinatorHook,false
    );
    if(!relay)return APPLY_FAILED;
    u32 displacement=0u;
    if(!realizationCoordinatorRelayDisplacement(base,relay,&displacement)){
        VirtualFree(relay,0,MEM_RELEASE);
        return APPLY_FAILED;
    }
    u8 replacementBytes[8];
    memcpy(
        replacementBytes,EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW,8u
    );
    memcpy(replacementBytes+3u,&displacement,4u);
    u64 replacement=0u;
    memcpy(&replacement,replacementBytes,8u);

    DWORD originalProtection=0;
    u8* address=base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW;
    if(!VirtualProtect(
           address,8u,PAGE_EXECUTE_READWRITE,&originalProtection
       )){
        VirtualFree(relay,0,MEM_RELEASE);
        return APPLY_FAILED;
    }
    if(!bytesEqual(
           address,EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW,
           (u32)sizeof(EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW)
       )){
        DWORD ignored=0;
        bool restored=VirtualProtect(
            address,8u,originalProtection,&ignored
        )!=FALSE&&protectionMatches(address,originalProtection);
        VirtualFree(relay,0,MEM_RELEASE);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    u64 observed=0u;
    bool published=compareExchangeU64(
        address,replacement,expected,&observed
    )&&observed==expected;
    if(!published){
        DWORD ignored=0;
        bool restored=VirtualProtect(
            address,8u,originalProtection,&ignored
        )!=FALSE&&protectionMatches(address,originalProtection);
        VirtualFree(relay,0,MEM_RELEASE);
        return restored?APPLY_CONFLICT:APPLY_CRITICAL;
    }
    if(!FlushInstructionCache(GetCurrentProcess(),address,8u)||
       !realizationCoordinatorPatchMatches(base,replacement)){
        *relayOut=relay;
        return rollbackRealizationCoordinatorPatch(
            base,replacement,originalProtection
        );
    }
    DWORD ignored=0;
    if(!VirtualProtect(address,8u,originalProtection,&ignored)||
       !protectionMatchesAfterOwnedWrite(address,originalProtection)){
        *relayOut=relay;
        return rollbackRealizationCoordinatorPatch(
            base,replacement,originalProtection
        );
    }
    *relayOut=relay;
    return APPLY_READY;
}

static bool readObjectPointer(u8* object,u32 offset,u8** value){
    if(!object||!value)return false;
    u64 pointer=0;
    if(!readU64(object+offset,&pointer)||!pointer)return false;
    *value=(u8*)pointer;
    return true;
}

static bool captureGameplayObjects(void* self,GameplayObjects* objects){
    if(!objects)return false;
    memset(objects,0,sizeof(*objects));
    u8* body=(u8*)self;
    if(!hasExactVtable(body,RVA_BODY_SECONDARY_VTABLE)||
       !readObjectPointer(body,OFF_BODY_SENSOR,&objects->sensor)||
       !hasExactVtable(objects->sensor,RVA_SENSOR_VTABLE)||
       !readObjectPointer(objects->sensor,OFF_SENSOR_EFFECT_INSTANCE,&objects->instance)||
       !hasExactVtable(objects->instance,RVA_EFFECT_INSTANCE_VTABLE)||
       !readObjectPointer(objects->instance,OFF_EFFECT_RESOURCE,&objects->resource)||
       !hasExactVtable(objects->resource,RVA_EFFECT_RESOURCE_VTABLE))return false;
    u64 manager=0;
    if(!readU64(g_gameBase+RVA_SENSOR_MANAGER_GLOBAL,&manager)||!manager)return false;
    objects->manager=(u8*)manager;
    u32 active=0;
    return hasExactVtable(objects->manager,RVA_SENSOR_MANAGER_VTABLE)&&
           readU32(objects->manager+OFF_MANAGER_ACTIVE,&active)&&(active&0xFFu)==1u;
}

static bool raiseAnglePair(
    u8* instanceAddress,u8* managerAddress,u32 targetBits,
    u32* beforeInstance,u32* beforeManager,bool* rollbackFailed
){
    if(!instanceAddress||!managerAddress||!beforeInstance||!beforeManager||!rollbackFailed)return false;
    *rollbackFailed=false;
    float target=0.0f;
    if(!finitePositiveFloat(targetBits,&target))return false;

    u32 instanceBits=0,managerBits=0;
    float instanceValue=0.0f,managerValue=0.0f;
    if(!readU32(instanceAddress,&instanceBits)||
       !finitePositiveFloat(instanceBits,&instanceValue)||
       !readU32(managerAddress,&managerBits)||
       !finitePositiveFloat(managerBits,&managerValue))return false;
    *beforeInstance=instanceBits;
    *beforeManager=managerBits;

    bool instanceChanged=false;
    if(instanceValue<target){
        u32 observed=0;
        if(!compareExchangeU32(instanceAddress,targetBits,instanceBits,&observed))return false;
        if(observed==instanceBits){
            instanceChanged=true;
        }else{
            float concurrent=0.0f;
            if(!finitePositiveFloat(observed,&concurrent)||concurrent<target)return false;
        }
    }

    bool managerChanged=false;
    bool managerReady=managerValue>=target;
    if(!managerReady){
        u32 observed=0;
        if(compareExchangeU32(managerAddress,targetBits,managerBits,&observed)){
            if(observed==managerBits){
                managerReady=true;
                managerChanged=true;
            }else{
                float concurrent=0.0f;
                managerReady=finitePositiveFloat(observed,&concurrent)&&concurrent>=target;
            }
        }
    }
    if(!managerReady){
        if(instanceChanged){
            u32 observed=0;
            if(!compareExchangeU32(instanceAddress,instanceBits,targetBits,&observed)||
               (observed!=targetBits&&observed!=instanceBits))*rollbackFailed=true;
        }
        return false;
    }

    u32 verifiedInstance=0,verifiedManager=0;
    float verifiedInstanceValue=0.0f,verifiedManagerValue=0.0f;
    bool verified=readU32(instanceAddress,&verifiedInstance)&&
                  finitePositiveFloat(verifiedInstance,&verifiedInstanceValue)&&
                  verifiedInstanceValue>=target&&
                  readU32(managerAddress,&verifiedManager)&&
                  finitePositiveFloat(verifiedManager,&verifiedManagerValue)&&
                  verifiedManagerValue>=target;
    if(verified)return true;
    if(managerChanged){
        u32 observed=0;
        if(!compareExchangeU32(managerAddress,managerBits,targetBits,&observed)||
           (observed!=targetBits&&observed!=managerBits))*rollbackFailed=true;
    }
    if(instanceChanged){
        u32 observed=0;
        if(!compareExchangeU32(instanceAddress,instanceBits,targetBits,&observed)||
           (observed!=targetBits&&observed!=instanceBits))*rollbackFailed=true;
    }
    return false;
}

static bool ensureInstanceDiameter(
    const GameplayObjects* objects,u32 targetBits,u32* beforeBits,u32* afterBits,
    u32* originalOverrideBits,bool* overrideChanged
){
    if(!objects||!objects->instance||!objects->resource||!beforeBits||!afterBits||
       !originalOverrideBits||!overrideChanged)return false;
    *overrideChanged=false;
    float target=0.0f;
    if(!finitePositiveFloat(targetBits,&target))return false;

    u32 overrideBits=0;
    float overrideValue=0.0f;
    if(!readU32(objects->instance+OFF_EFFECT_SIZE_OVERRIDE,&overrideBits)||
       !finiteFloat(overrideBits,&overrideValue))return false;
    *originalOverrideBits=overrideBits;

    float effective=0.0f;
    if(overrideValue>=0.0f){
        if(overrideValue>MAX_PLAUSIBLE_RUNTIME_VALUE)return false;
        effective=overrideValue;
    }else{
        if(overrideBits!=INACTIVE_SIZE_OVERRIDE_BITS)return false;
        u32 resourceSizeBits=0;
        if(!readU32(objects->resource+OFF_EFFECT_RESOURCE_SIZE,&resourceSizeBits)||
           !finitePositiveFloat(resourceSizeBits,&effective))return false;
        u32 flags=0;
        if(!readU32(objects->instance+OFF_EFFECT_FLAGS,&flags))return false;
        if(((flags>>8)&0xFFu)!=0u){
            u64 additionalResource=0;
            if(!readU64(objects->resource+OFF_EFFECT_RESOURCE_ADDITIONAL,&additionalResource))return false;
            if(additionalResource){
                u32 additionalBits=0;
                float additional=0.0f;
                if(!readU32((u8*)additionalResource+OFF_ADDITIONAL_SIZE,&additionalBits)||
                   !finiteFloat(additionalBits,&additional)||additional<0.0f||
                   additional>MAX_PLAUSIBLE_RUNTIME_VALUE)return false;
                effective+=additional;
            }
        }
    }
    if(!(effective>=0.0f)||effective>MAX_PLAUSIBLE_RUNTIME_VALUE*2.0f)return false;
    *beforeBits=floatBits(effective);
    if(effective>=target){
        *afterBits=*beforeBits;
        return true;
    }

    u32 observed=0;
    if(!compareExchangeU32(
        objects->instance+OFF_EFFECT_SIZE_OVERRIDE,targetBits,overrideBits,&observed
    ))return false;
    if(observed!=overrideBits){
        float concurrent=0.0f;
        if(!finiteFloat(observed,&concurrent)||concurrent<target)return false;
        *afterBits=observed;
        return true;
    }
    *afterBits=targetBits;
    *overrideChanged=true;
    return true;
}

typedef u8 (*SensorGate)(u8*,const double*,float);
static u8 retryCargoSensorGate(SensorGate original,u8* manager,const double* target,
                              float optionalRadius,u8* component);

static void resetGateTelemetry(){
    _InterlockedExchange(&g_gateCaptureActive,0);
    _InterlockedExchange(&g_gateCalls,0);
    _InterlockedExchange(&g_gatePasses,0);
    _InterlockedExchange(&g_gateRejects,0);
    _InterlockedExchange(&g_gateDistanceInvalid,0);
    _InterlockedExchange(&g_gateBinTo50,0);
    _InterlockedExchange(&g_gateBin50To100,0);
    _InterlockedExchange(&g_gateBin100To200,0);
    _InterlockedExchange(&g_gateBin200To300,0);
    _InterlockedExchange(&g_gateBin300To400,0);
    _InterlockedExchange(&g_gateBinOver400,0);
    _InterlockedExchange(&g_gatePassOver200,0);
    _InterlockedExchange(&g_gateRejectOver200,0);
    _InterlockedExchange(&g_gateMaxDistanceSquaredBits,0);
    _InterlockedExchange(&g_gateLatestRadiusBits,0);
    _InterlockedExchange(&g_gateMaxRadiusBits,0);
    _InterlockedExchange(&g_gateLatestAngleBits,0);
    _InterlockedExchange(&g_gateLatestActive,0);
    _InterlockedExchange64(&g_gateLatestSnapshot,0);
    _InterlockedIncrement(&g_gateScanSerial);
    _InterlockedExchange(&g_gateCaptureActive,1);
}

static bool gateTelemetryMayFinish(bool managerSampleValid,u32 active,u32 stableTicks){
    // Unchanged counters can mean a stalled game thread, not a completed scan.
    // The worker runs independently and must keep capturing while the native
    // manager is active, including through multi-second pauses.
    return managerSampleValid&&(active&0xFFu)==0u&&stableTicks>=2u;
}

extern "C" u8 sensorGateTelemetryHook(
    u8* manager,const double* target,float optionalRadius,u8* component
){
    SensorGate original=(SensorGate)(u64)g_originalSensorGate;
    u8 passed=original?original(manager,target,optionalRadius):0u;
    if(!passed&&original)passed=retryCargoSensorGate(original,manager,target,optionalRadius,component);

    if(!manager||!target||g_gateCaptureActive!=1||
       *(volatile u8*)(manager+OFF_MANAGER_ACTIVE)==0u)return passed;

    _InterlockedIncrement(&g_gateCalls);
    if(passed)_InterlockedIncrement(&g_gatePasses);
    else _InterlockedIncrement(&g_gateRejects);

    const double* origin=(const double*)(manager+0x38u);
    double dx=target[0]-origin[0];
    double dy=target[1]-origin[1];
    double dz=target[2]-origin[2];
    double distanceSquared=dx*dx+dy*dy+dz*dz;
    if(!(distanceSquared>=0.0)||distanceSquared>4000000.0){
        _InterlockedIncrement(&g_gateDistanceInvalid);
        return passed;
    }

    union FloatBitsLocal{float value;long bits;};
    FloatBitsLocal distanceBits;
    distanceBits.value=(float)distanceSquared;
    long current=g_gateMaxDistanceSquaredBits;
    while((u32)current<(u32)distanceBits.bits){
        long observed=_InterlockedCompareExchange(
            &g_gateMaxDistanceSquaredBits,distanceBits.bits,current
        );
        if(observed==current)break;
        current=observed;
    }

    if(distanceSquared<=2500.0){
        _InterlockedIncrement(&g_gateBinTo50);
    }else if(distanceSquared<=10000.0){
        _InterlockedIncrement(&g_gateBin50To100);
    }else if(distanceSquared<=40000.0){
        _InterlockedIncrement(&g_gateBin100To200);
    }else if(distanceSquared<=90000.0){
        _InterlockedIncrement(&g_gateBin200To300);
    }else if(distanceSquared<=160000.0){
        _InterlockedIncrement(&g_gateBin300To400);
    }else{
        _InterlockedIncrement(&g_gateBinOver400);
    }
    if(distanceSquared>40000.0){
        if(passed)_InterlockedIncrement(&g_gatePassOver200);
        else _InterlockedIncrement(&g_gateRejectOver200);
    }

    long radiusBits=*(volatile long*)(manager+OFF_MANAGER_GATE_RADIUS);
    long angleBits=*(volatile long*)(manager+OFF_MANAGER_FULL_ANGLE);
    _InterlockedExchange(&g_gateLatestRadiusBits,radiusBits);
    _InterlockedExchange(&g_gateLatestAngleBits,angleBits);
    if(((u32)radiusBits&0x80000000u)==0u&&
       ((u32)radiusBits&0x7F800000u)!=0x7F800000u){
        current=g_gateMaxRadiusBits;
        while((u32)current<(u32)radiusBits){
            long observed=_InterlockedCompareExchange(&g_gateMaxRadiusBits,radiusBits,current);
            if(observed==current)break;
            current=observed;
        }
    }
    return passed;
}

typedef void (*DetectedConstantUpdate)(u8*,void*);
typedef u8* (*FindEntityComponent)(u8*,const void*);
typedef void (*BaggageSymbolsEvent)(u8*,u8*);
typedef void (*RealizedBaggageBridge)(u8*);
typedef void (*RealizedBaggageLifecycle)(u8*,void*);
typedef void (*RequestEntityRealize)(u8*,u8,u8);
typedef void (*RealizationCoordinator)(void*);

enum CargoBridgeResult{
    CARGO_BRIDGE_NOT_APPLICABLE,
    CARGO_BRIDGE_UNREALIZED,
    CARGO_BRIDGE_FAILED,
    CARGO_BRIDGE_READY
};

struct CargoBridgeObjects{
    u8* baggage;
    u8* realized;
};

struct DetectedMessageKey{
    u32 tick;
};

static bool detectedMessageKey(void* message,DetectedMessageKey* key){
    if(!message||!key||!rangeReadable(message,0x20u))return false;
    u32 tick=0;
    if(!readU32((u8*)message+0x18u,&tick))return false;
    key->tick=tick;
    return true;
}

enum TickClaimResult{TICK_CLAIM_FULL,TICK_CLAIM_EXISTS,TICK_CLAIM_NEW};

// Caller holds g_farRefreshTickLock shared or exclusive. Slot publication is
// atomic, so worker threads can claim different components concurrently.
static TickClaimResult claimDetectedPointer(
    DetectedTickBucket* bucket,u8* pointer
){
    if(!bucket||!pointer)return TICK_CLAIM_FULL;
    u64 value=(u64)pointer;
    u32 slot=(u32)(
        ((value>>4u)^(value>>17u)^(value>>31u))&
        (MAX_DETECTED_TICK_CLAIMS-1u)
    );
    for(u32 probe=0;probe<MAX_DETECTED_TICK_CLAIMS;probe++){
        long long existing=_InterlockedCompareExchange64(
            (volatile long long*)&bucket->claims[slot],
            (long long)(u64)pointer,0
        );
        if(existing==0)return TICK_CLAIM_NEW;
        if((u64)existing==(u64)pointer)return TICK_CLAIM_EXISTS;
        slot=(slot+1u)&(MAX_DETECTED_TICK_CLAIMS-1u);
    }
    return TICK_CLAIM_FULL;
}

static void initializeDetectedTickBucket(
    DetectedTickBucket* bucket,u32 tick
){
    if(!bucket)return;
    long bucketFanout=_InterlockedCompareExchange(
        &bucket->fanoutCount,0,0
    );
    long bucketGuard=_InterlockedCompareExchange(&bucket->sweepGuard,0,0);
    if(bucket->valid&&(bucketFanout>0||bucketGuard>0)){
        long outstanding=_InterlockedCompareExchange(
            &g_farRefreshOutstandingFanout,0,0
        )-bucketFanout-bucketGuard;
        _InterlockedExchange(
            &g_farRefreshOutstandingFanout,outstanding>0?outstanding:0
        );
    }
    bucket->valid=true;
    bucket->tick=tick;
    _InterlockedExchange(&bucket->fanoutCount,0);
    _InterlockedExchange(&bucket->sweepGuard,0);
    _InterlockedExchange(&bucket->pinCount,0);
    memset(bucket->claims,0,sizeof(bucket->claims));
}

// Caller holds g_farRefreshTickLock.
static DetectedTickBucket* detectedTickBucket(u32 tick,bool create){
    for(u32 i=0;i<DETECTED_TICK_BUCKET_COUNT;i++){
        if(g_farRefreshTickBuckets[i].valid&&
           g_farRefreshTickBuckets[i].tick==tick){
            return &g_farRefreshTickBuckets[i];
        }
    }
    if(!create)return 0;
    for(u32 i=0;i<DETECTED_TICK_BUCKET_COUNT;i++){
        if(!g_farRefreshTickBuckets[i].valid){
            initializeDetectedTickBucket(&g_farRefreshTickBuckets[i],tick);
            return &g_farRefreshTickBuckets[i];
        }
    }
    // The value carried by the engine message is an identity key, not a
    // monotonic counter. Live traces proved that it can decrease between
    // frames. Recycle only an entirely unowned bucket and never order keys.
    for(u32 probe=0u;probe<DETECTED_TICK_BUCKET_COUNT;probe++){
        u32 index=(g_farRefreshTickReplacementCursor+probe)%
                  DETECTED_TICK_BUCKET_COUNT;
        DetectedTickBucket* candidate=&g_farRefreshTickBuckets[index];
        if(_InterlockedCompareExchange(&candidate->pinCount,0,0)!=0||
           _InterlockedCompareExchange(&candidate->fanoutCount,0,0)!=0||
           _InterlockedCompareExchange(&candidate->sweepGuard,0,0)!=0)continue;
        initializeDetectedTickBucket(candidate,tick);
        g_farRefreshTickReplacementCursor=(index+1u)%DETECTED_TICK_BUCKET_COUNT;
        return candidate;
    }
    return 0;
}

[[maybe_unused]] static void resetUnownedDetectedTickBuckets(){
    AcquireSRWLockExclusive(&g_farRefreshTickLock);
    for(u32 i=0u;i<DETECTED_TICK_BUCKET_COUNT;i++){
        DetectedTickBucket* bucket=&g_farRefreshTickBuckets[i];
        if(_InterlockedCompareExchange(&bucket->pinCount,0,0)!=0||
           _InterlockedCompareExchange(&bucket->fanoutCount,0,0)!=0||
           _InterlockedCompareExchange(&bucket->sweepGuard,0,0)!=0)continue;
        bucket->valid=false;
        bucket->tick=0u;
        memset(bucket->claims,0,sizeof(bucket->claims));
    }
    g_farRefreshTickReplacementCursor=0u;
    ReleaseSRWLockExclusive(&g_farRefreshTickLock);
}

// Track the engine's natural dispatches for this tick. The handler always
// continues into the original function; the post-join fanout uses these
// claims only to avoid calling a component twice in the same tick.
[[maybe_unused]] static bool beginNaturalDetectedUpdate(u8* component,void* message){
    if(!component||
       (_InterlockedCompareExchange(&g_farRefreshPending,0,0)!=1&&
        _InterlockedCompareExchange(&g_farRefreshBusy,0,0)!=1&&
        _InterlockedCompareExchange(&g_farRefreshOutstandingFanout,0,0)==0)){
        return false;
    }
    DetectedMessageKey key;
    if(!detectedMessageKey(message,&key))return false;
    bool suppress=false;
    AcquireSRWLockShared(&g_farRefreshTickLock);
    DetectedTickBucket* bucket=detectedTickBucket(key.tick,false);
    ReleaseSRWLockShared(&g_farRefreshTickLock);
    if(!bucket){
        AcquireSRWLockExclusive(&g_farRefreshTickLock);
        bucket=detectedTickBucket(key.tick,true);
        ReleaseSRWLockExclusive(&g_farRefreshTickLock);
    }
    AcquireSRWLockShared(&g_farRefreshTickLock);
    bucket=detectedTickBucket(key.tick,false);
    if(bucket&&
       (_InterlockedCompareExchange(&g_farRefreshPending,0,0)==1||
        _InterlockedCompareExchange(&g_farRefreshBusy,0,0)==1||
        _InterlockedCompareExchange(&bucket->fanoutCount,0,0)>0||
        _InterlockedCompareExchange(&bucket->sweepGuard,0,0)>0)){
        TickClaimResult claimed=claimDetectedPointer(bucket,component);
        suppress=claimed==TICK_CLAIM_EXISTS;
    }
    ReleaseSRWLockShared(&g_farRefreshTickLock);
    if(suppress)_InterlockedIncrement(&g_farRefreshDuplicateClaims);
    return suppress;
}

enum FanoutReserveResult{
    FANOUT_RESERVE_NO_BUCKET,
    FANOUT_RESERVE_LIMIT,
    FANOUT_RESERVE_DUPLICATE,
    FANOUT_RESERVE_FULL,
    FANOUT_RESERVE_READY
};

static FanoutReserveResult reserveDetectedFanout(u8* component,void* message){
    if(!component)return FANOUT_RESERVE_NO_BUCKET;
    DetectedMessageKey key;
    if(!detectedMessageKey(message,&key))return FANOUT_RESERVE_NO_BUCKET;
    FanoutReserveResult result=FANOUT_RESERVE_NO_BUCKET;
    AcquireSRWLockShared(&g_farRefreshTickLock);
    DetectedTickBucket* bucket=detectedTickBucket(key.tick,false);
    if(bucket){
        if((u32)_InterlockedCompareExchange(
               &bucket->fanoutCount,0,0
           )>=MAX_DETECTED_REFRESH_COMPONENTS){
            result=FANOUT_RESERVE_LIMIT;
        }else{
            TickClaimResult claim=claimDetectedPointer(bucket,component);
            if(claim==TICK_CLAIM_NEW){
                _InterlockedIncrement(&bucket->fanoutCount);
                _InterlockedIncrement(&g_farRefreshOutstandingFanout);
                result=FANOUT_RESERVE_READY;
            }else if(claim==TICK_CLAIM_EXISTS){
                result=FANOUT_RESERVE_DUPLICATE;
            }else{
                result=FANOUT_RESERVE_FULL;
            }
        }
    }
    ReleaseSRWLockShared(&g_farRefreshTickLock);
    return result;
}

static bool retainDetectedTickGuard(void* message){
    DetectedMessageKey key;
    if(!detectedMessageKey(message,&key))return false;
    bool retained=false;
    // Creation and pinning are one exclusive operation. Otherwise a busy
    // worker can recycle an unpinned freshly-created bucket in between them.
    AcquireSRWLockExclusive(&g_farRefreshTickLock);
    DetectedTickBucket* bucket=detectedTickBucket(key.tick,true);
    if(bucket){
        if(_InterlockedCompareExchange(&bucket->sweepGuard,1,0)==0){
            _InterlockedIncrement(&g_farRefreshOutstandingFanout);
        }
        _InterlockedExchange(&bucket->pinCount,1);
        retained=true;
    }
    ReleaseSRWLockExclusive(&g_farRefreshTickLock);
    return retained;
}

static void subtractOutstandingFanout(long amount){
    if(amount<=0)return;
    long current=_InterlockedCompareExchange(&g_farRefreshOutstandingFanout,0,0);
    for(;;){
        long desired=current>amount?current-amount:0;
        long observed=_InterlockedCompareExchange(
            &g_farRefreshOutstandingFanout,desired,current
        );
        if(observed==current)return;
        current=observed;
    }
}

static void releaseDetectedTickGuard(void* message){
    DetectedMessageKey key;
    if(!detectedMessageKey(message,&key))return;
    long released=0;
    AcquireSRWLockExclusive(&g_farRefreshTickLock);
    DetectedTickBucket* bucket=detectedTickBucket(key.tick,false);
    if(bucket){
        long fanout=_InterlockedExchange(&bucket->fanoutCount,0);
        long guard=_InterlockedExchange(&bucket->sweepGuard,0);
        _InterlockedExchange(&bucket->pinCount,0);
        if(fanout>0)released+=fanout;
        if(guard>0)released++;
    }
    subtractOutstandingFanout(released);
    ReleaseSRWLockExclusive(&g_farRefreshTickLock);
}

static bool validateDetectedRefreshComponent(u8* component,u8** ownerOut){
    if(ownerOut)*ownerOut=0;
    if(!component||!rangeReadable(component,DETECTED_COMPONENT_SIZE)||
       !hasExactVtable(component,RVA_DETECTED_COMPONENT_VTABLE)||
       component[0x38u]!=1u||component[0x50u]!=1u||
       component[OFF_DETECTED_ODRADEK_SENSOR]!=1u)return false;
    u64 resource=0,owner=0;
    if(!readU64(component+0x30u,&resource)||!resource||
       !rangeReadable((const void*)resource,0x38u)||
       !readU64(component+OFF_DETECTED_OWNER,&owner)||!owner||
       !rangeReadable(
           (const void*)owner,OFF_ENTITY_WORLD_POSITION+3u*sizeof(double)
       ))return false;
    if(ownerOut)*ownerOut=(u8*)owner;
    return true;
}

static bool detectedDistanceSquared(
    u8* manager,u8* owner,float* distanceSquared
){
    if(!manager||!owner||!distanceSquared||
       !rangeReadable(manager+0x38u,3u*sizeof(double))||
       !rangeReadable(
           owner+OFF_ENTITY_WORLD_POSITION,3u*sizeof(double)
       ))return false;
    const double* origin=(const double*)(manager+0x38u);
    const double* target=(const double*)(owner+OFF_ENTITY_WORLD_POSITION);
    double dx=target[0]-origin[0];
    double dy=target[1]-origin[1];
    double dz=target[2]-origin[2];
    double squared=dx*dx+dy*dy+dz*dz;
    if(!(squared>=0.0)||squared>4000000.0)return false;
    *distanceSquared=(float)squared;
    return true;
}

static bool validateCargoBridgeObjects(
    u8* detected,const CargoBridgeObjects* objects
){
    if(!detected||!objects||!objects->baggage||!objects->realized||
       !rangeReadable(detected,DETECTED_COMPONENT_SIZE)||
       !hasExactVtable(detected,RVA_DETECTED_COMPONENT_VTABLE)||
       !rangeReadable(objects->baggage,BAGGAGE_COMPONENT_SIZE)||
       !hasExactVtable(objects->baggage,RVA_BAGGAGE_COMPONENT_VTABLE)||
       !rangeReadable(objects->realized,REALIZED_BAGGAGE_SIZE)||
       !hasExactVtable(objects->realized,RVA_REALIZED_BAGGAGE_VTABLE))return false;
    u64 baggageDetected=0,realizedPointer=0,realizedBaggage=0;
    return readU64(
               objects->baggage+OFF_BAGGAGE_DETECTED_COMPONENT,&baggageDetected
           )&&baggageDetected==(u64)detected&&
           readU64(
               objects->baggage+OFF_BAGGAGE_REALIZED_COMPONENT,&realizedPointer
           )&&realizedPointer==(u64)objects->realized&&
           readU64(
               objects->realized+OFF_REALIZED_BAGGAGE_COMPONENT,&realizedBaggage
           )&&realizedBaggage==(u64)objects->baggage;
}

static CargoBridgeResult resolveCargoBridgeObjects(
    u8* detected,u8* owner,CargoBridgeObjects* objects
){
    if(objects){objects->baggage=0;objects->realized=0;}
    if(!detected||!owner||!objects||!g_gameBase)return CARGO_BRIDGE_FAILED;
    FindEntityComponent findComponent=
        (FindEntityComponent)(g_gameBase+RVA_FIND_ENTITY_COMPONENT);
    u8* baggage=findComponent(
        owner+OFF_ENTITY_COMPONENTS,
        (const void*)(g_gameBase+RVA_BAGGAGE_COMPONENT_RTTI)
    );
    if(!baggage)return CARGO_BRIDGE_NOT_APPLICABLE;
    objects->baggage=baggage;
    if(!rangeReadable(baggage,BAGGAGE_COMPONENT_SIZE)||
       !hasExactVtable(baggage,RVA_BAGGAGE_COMPONENT_VTABLE))return CARGO_BRIDGE_FAILED;
    u64 baggageDetected=0,realizedPointer=0;
    if(!readU64(
           baggage+OFF_BAGGAGE_DETECTED_COMPONENT,&baggageDetected
       )||baggageDetected!=(u64)detected||
       !readU64(
            baggage+OFF_BAGGAGE_REALIZED_COMPONENT,&realizedPointer
       ))return CARGO_BRIDGE_FAILED;
    if(!realizedPointer)return CARGO_BRIDGE_UNREALIZED;
    u8* realized=(u8*)realizedPointer;
    objects->realized=realized;
    return validateCargoBridgeObjects(detected,objects)?
           CARGO_BRIDGE_READY:CARGO_BRIDGE_FAILED;
}

enum SymbolRevealResult{
    SYMBOL_REVEAL_LOOKUP_FAILED,
    SYMBOL_REVEAL_IDENTITY_FAILED,
    SYMBOL_REVEAL_CORE_FAILED,
    SYMBOL_REVEAL_TEMPLATE_FAILED,
    SYMBOL_REVEAL_READY
};

static bool validateBaggageSymbolsIdentity(
    u8* symbols,u8* owner,u8* baggage,u8* detected
){
    if(!symbols||!owner||!baggage||!detected||
       !rangeReadable(symbols,BAGGAGE_SYMBOLS_SIZE)||
       symbols[0x38u]!=1u||
       !hasExactVtable(symbols,RVA_BAGGAGE_SYMBOLS_VTABLE)||
       !hasExactVtable(
           symbols+0x20u,RVA_BAGGAGE_SYMBOLS_SECONDARY_VTABLE
       ))return false;
    u64 symbolsOwner=0u,symbolsBaggage=0u,baggageDetected=0u;
    return readU64(
               symbols+OFF_BAGGAGE_SYMBOLS_OWNER,&symbolsOwner
           )&&symbolsOwner==(u64)owner&&
           readU64(
               symbols+OFF_BAGGAGE_SYMBOLS_BAGGAGE,&symbolsBaggage
           )&&symbolsBaggage==(u64)baggage&&
           readU64(
               baggage+OFF_BAGGAGE_DETECTED_COMPONENT,&baggageDetected
           )&&baggageDetected==(u64)detected;
}

static bool baggageSymbolRecordActive(u8* symbols){
    if(!symbols||!rangeReadable(symbols,BAGGAGE_SYMBOLS_SIZE))return false;
    u32 count=0u,capacity=0u;
    u64 recordsPointer=0u;
    if(!readU32(
           symbols+OFF_BAGGAGE_SYMBOLS_RECORD_COUNT,&count
       )||!readU32(
           symbols+OFF_BAGGAGE_SYMBOLS_RECORD_CAPACITY,&capacity
       )||count>MAX_BAGGAGE_SYMBOL_RECORDS||capacity<count||
       capacity>MAX_BAGGAGE_SYMBOL_CAPACITY||
       !readU64(
           symbols+OFF_BAGGAGE_SYMBOLS_RECORDS,&recordsPointer
       ))return false;
    if(!count)return false;
    if(!recordsPointer||!rangeReadable(
           (const void*)recordsPointer,
           (u64)count*BAGGAGE_SYMBOL_RECORD_SIZE
       ))return false;
    for(u32 i=0u;i<count;i++){
        u8* record=(u8*)recordsPointer+(u64)i*BAGGAGE_SYMBOL_RECORD_SIZE;
        if(record[0]!=BAGGAGE_SYMBOL_EVENT_CATEGORY)continue;
        u64 templatePointer=0u;
        u32 remainingBits=0u,durationBits=0u;
        float remaining=0.0f,duration=0.0f;
        if(!readU64(
               record+OFF_BAGGAGE_SYMBOL_RECORD_TEMPLATE,&templatePointer
           )||!templatePointer||
           !rangeReadable((u8*)templatePointer,0x34u)||
           ((u8*)templatePointer)[0x20u]!=BAGGAGE_SYMBOL_EVENT_CATEGORY||
           !readU32(
               record+OFF_BAGGAGE_SYMBOL_RECORD_REMAINING,&remainingBits
           )||!readU32(
               (u8*)templatePointer+OFF_BAGGAGE_SYMBOL_TEMPLATE_DURATION,
               &durationBits
           )||!finitePositiveFloat(remainingBits,&remaining)||
           !finitePositiveFloat(durationBits,&duration)||
           remainingBits!=durationBits)return false;
        return true;
    }
    return false;
}

// DSBaggageManagerSymbols owns the native, per-cargo scan display and its
// lifetime timer. Invoke it only while the game's natural Detected callback
// keeps this entity/component graph alive. No pointer from this lookup may be
// published or used later.
[[maybe_unused]] static SymbolRevealResult revealUnrealizedCargoSymbol(
    u8* owner,u8* baggage,u8* detected
){
    if(!g_gameBase||!owner||!baggage||!detected){
        return SYMBOL_REVEAL_IDENTITY_FAILED;
    }
    FindEntityComponent findComponent=
        (FindEntityComponent)(g_gameBase+RVA_FIND_ENTITY_COMPONENT);
    BaggageSymbolsEvent eventCore=
        (BaggageSymbolsEvent)(g_gameBase+RVA_BAGGAGE_SYMBOLS_EVENT_CORE);
#ifdef ODRADEK_TEST_BUILD
    if(g_testFindEntityComponent)findComponent=
        (FindEntityComponent)g_testFindEntityComponent;
    if(g_testBaggageSymbolsEvent)eventCore=
        (BaggageSymbolsEvent)g_testBaggageSymbolsEvent;
#endif
    u8* symbols=findComponent(
        owner+OFF_ENTITY_COMPONENTS,
        (const void*)(g_gameBase+RVA_BAGGAGE_SYMBOLS_RTTI)
    );
    if(!symbols)return SYMBOL_REVEAL_LOOKUP_FAILED;
    if(!validateBaggageSymbolsIdentity(
           symbols,owner,baggage,detected
       ))return SYMBOL_REVEAL_IDENTITY_FAILED;

    __declspec(align(16)) u8 eventMessage[0xB0u];
    memset(eventMessage,0,sizeof(eventMessage));
    *(u32*)(eventMessage+0x10u)=BAGGAGE_SYMBOL_EVENT_ID;
    eventMessage[0x98u]=BAGGAGE_SYMBOL_EVENT_CATEGORY;
    eventMessage[0x9Cu]=0u;
    eventCore(symbols,eventMessage);
    if(eventMessage[8u]!=1u)return SYMBOL_REVEAL_CORE_FAILED;
    return baggageSymbolRecordActive(symbols)?
        SYMBOL_REVEAL_READY:SYMBOL_REVEAL_TEMPLATE_FAILED;
}

static CargoBridgeResult bridgeFreshCargoHit(
    u8* detected,const CargoBridgeObjects* objects,float distanceSquared,
    bool* foundCalled,bool* markerCalled
){
    if(foundCalled)*foundCalled=false;
    if(markerCalled)*markerCalled=false;
    if(!detected||!objects||detected[OFF_DETECTED_FRESH_HIT]!=1u){
        return CARGO_BRIDGE_NOT_APPLICABLE;
    }
    if(!validateCargoBridgeObjects(detected,objects))return CARGO_BRIDGE_FAILED;

    _InterlockedExchange(
        (volatile long*)(objects->realized+OFF_REALIZED_DISTANCE_SQUARED),
        (long)floatBits(distanceSquared)
    );
    RealizedBaggageBridge foundBridge=
        (RealizedBaggageBridge)(g_gameBase+RVA_REALIZED_FOUND_BRIDGE);
    RealizedBaggageBridge markerBridge=
        (RealizedBaggageBridge)(g_gameBase+RVA_REALIZED_MARKER_BRIDGE);
    foundBridge(objects->realized);
    if(foundCalled)*foundCalled=true;

    if(!validateCargoBridgeObjects(detected,objects))return CARGO_BRIDGE_FAILED;
    // The found consumer has used the one-shot hit. The marker bridge reads the
    // persistent sensor bits, so clear the hit before it can be consumed again.
    _InterlockedExchange8(
        (volatile char*)(detected+OFF_DETECTED_FRESH_HIT),0
    );
    markerBridge(objects->realized);
    if(markerCalled)*markerCalled=true;
    return CARGO_BRIDGE_READY;
}

enum PendingRememberResult{
    PENDING_REMEMBER_REJECTED,
    PENDING_REMEMBER_DEDUPE,
    PENDING_REMEMBER_FULL,
    PENDING_REMEMBER_RECORDED
};

enum PendingValidationResult{
    PENDING_VALIDATION_EXPIRED,
    PENDING_VALIDATION_IDENTITY_REJECT,
    PENDING_VALIDATION_DISTANCE_REJECT,
    PENDING_VALIDATION_READY
};

enum PendingClaimMode{
    PENDING_CLAIM_UNREADY_OR_SAME,
    PENDING_CLAIM_EXACT_READY
};

enum RealizeLeaseRememberResult{
    REALIZE_LEASE_REJECTED,
    REALIZE_LEASE_DEDUPE,
    REALIZE_LEASE_FULL,
    REALIZE_LEASE_RECORDED
};

static u64 pendingNowMs(){
#ifdef ODRADEK_TEST_BUILD
    long long testNow=_InterlockedCompareExchange64(&g_pendingTestNowMs,0,0);
    if(testNow>=0)return (u64)testNow;
#endif
    return GetTickCount64();
}

static void updateAtomicMaximum(volatile long* target,u32 value){
    if(!target)return;
    long current=_InterlockedCompareExchange(target,0,0);
    while((u32)current<value){
        long observed=_InterlockedCompareExchange(target,(long)value,current);
        if(observed==current)return;
        current=observed;
    }
}

static bool pendingCargoExpired(const PendingCargoHit* hit,u64 now){
    return !hit||!hit->baggage||now<hit->createdMs||
           now-hit->createdMs>=PENDING_CARGO_TTL_MS;
}

// Caller holds g_pendingCargoLock exclusively.
static void clearPendingCargoSlotLocked(u32 index){
    if(index>=MAX_PENDING_CARGO_HITS||!g_pendingCargoHits[index].baggage)return;
    memset(&g_pendingCargoHits[index],0,sizeof(g_pendingCargoHits[index]));
    if(g_pendingCargoCount)g_pendingCargoCount--;
    _InterlockedExchange(&g_pendingPublishedCount,(long)g_pendingCargoCount);
}

// Caller holds g_pendingCargoLock exclusively. Records from another scan
// attempt are never allowed to survive, even if an older fanout is still
// finishing on another thread.
static void purgePendingCargoLocked(u32 currentEpoch,u64 now,bool clearAll){
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        PendingCargoHit* hit=&g_pendingCargoHits[i];
        if(!hit->baggage)continue;
        bool expired=pendingCargoExpired(hit,now);
        bool foreign=clearAll||!currentEpoch||hit->scanEpoch!=currentEpoch;
        if(!expired&&!foreign)continue;
        clearPendingCargoSlotLocked(i);
        if(expired)_InterlockedIncrement(&g_pendingExpired);
        else _InterlockedIncrement(&g_pendingEpochPurged);
    }
}

static void purgePendingCargo(bool clearAll){
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    u32 effectiveEpoch=clearAll?0u:(u32)_InterlockedCompareExchange(
        &g_scanAttemptEpoch,0,0
    );
    u64 now=pendingNowMs();
    purgePendingCargoLocked(effectiveEpoch,now,clearAll);
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
}

static RealizedInitFlightToken beginRealizedInitFlight(u8* realized){
    RealizedInitFlightToken token={realized,false,false};
    if(!realized)return token;
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    u32 emptyIndex=MAX_PENDING_CARGO_HITS;
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        RealizedInitFlight* entry=&g_realizedInitFlights[i];
        if(entry->realized==realized){
            if(entry->references<0xFFFFFFFFu){
                entry->references++;
                token.tracked=true;
            }else{
                g_realizedInitFlightOverflow++;
                token.overflow=true;
            }
            ReleaseSRWLockExclusive(&g_pendingCargoLock);
            return token;
        }
        if(!entry->realized&&emptyIndex==MAX_PENDING_CARGO_HITS){
            emptyIndex=i;
        }
    }
    if(emptyIndex<MAX_PENDING_CARGO_HITS){
        g_realizedInitFlights[emptyIndex].realized=realized;
        g_realizedInitFlights[emptyIndex].references=1u;
        token.tracked=true;
    }else{
        g_realizedInitFlightOverflow++;
        token.overflow=true;
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    return token;
}

static void endRealizedInitFlight(const RealizedInitFlightToken* token){
    if(!token||!token->realized)return;
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    if(token->overflow){
        if(g_realizedInitFlightOverflow>0)g_realizedInitFlightOverflow--;
        ReleaseSRWLockExclusive(&g_pendingCargoLock);
        return;
    }
    if(token->tracked){
        for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
            RealizedInitFlight* entry=&g_realizedInitFlights[i];
            if(entry->realized!=token->realized)continue;
            if(entry->references>1u){
                entry->references--;
            }else{
                memset(entry,0,sizeof(*entry));
            }
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
}

static bool realizedInitInFlight(u8* realized){
    if(!realized)return false;
    bool inFlight=false;
    AcquireSRWLockShared(&g_pendingCargoLock);
    if(g_realizedInitFlightOverflow>0){
        inFlight=true;
    }else{
        for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
            if(g_realizedInitFlights[i].realized==realized&&
               g_realizedInitFlights[i].references>0u){
                inFlight=true;
                break;
            }
        }
    }
    ReleaseSRWLockShared(&g_pendingCargoLock);
    return inFlight;
}

static PendingRememberResult rememberPendingCargoHit(
    u8* baggage,u8* detected,u8* owner,u32 sweepSerial,u32 scanEpoch,
    u32 messageTick,u32 targetRadiusBits,float distanceSquared,
    const double* scanOrigin
){
    float target=0.0f;
    if(!baggage||!detected||!owner||!sweepSerial||!scanEpoch||!scanOrigin||
       !finitePositiveFloat(targetRadiusBits,&target)||
       !(distanceSquared>=0.0f)||distanceSquared>target*target){
        return PENDING_REMEMBER_REJECTED;
    }
    PendingRememberResult result=PENDING_REMEMBER_REJECTED;
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    u64 now=pendingNowMs();
    u32 currentEpoch=(u32)_InterlockedCompareExchange(&g_scanAttemptEpoch,0,0);
    u32 currentSerial=(u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0);
    purgePendingCargoLocked(currentEpoch,now,false);
    if(currentEpoch!=scanEpoch||currentSerial!=sweepSerial||
       _InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1){
        ReleaseSRWLockExclusive(&g_pendingCargoLock);
        return PENDING_REMEMBER_REJECTED;
    }

    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        PendingCargoHit* hit=&g_pendingCargoHits[i];
        if(hit->baggage!=baggage)continue;
        if(hit->detected==detected&&hit->owner==owner&&
           hit->sweepSerial==sweepSerial&&hit->scanEpoch==scanEpoch){
            _InterlockedIncrement(&g_pendingDedupe);
            result=PENDING_REMEMBER_DEDUPE;
        }else{
            // Reusing one baggage identity for another component/owner in the
            // same epoch is ambiguous. Drop both identities and fail closed.
            clearPendingCargoSlotLocked(i);
            _InterlockedIncrement(&g_pendingIdentityConflicts);
        }
        ReleaseSRWLockExclusive(&g_pendingCargoLock);
        return result;
    }

    if(g_pendingCargoCount>=MAX_PENDING_CARGO_HITS){
        _InterlockedIncrement(&g_pendingFull);
        ReleaseSRWLockExclusive(&g_pendingCargoLock);
        return PENDING_REMEMBER_FULL;
    }
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        PendingCargoHit* hit=&g_pendingCargoHits[i];
        if(hit->baggage)continue;
        hit->baggage=baggage;
        hit->detected=detected;
        hit->owner=owner;
        hit->sweepSerial=sweepSerial;
        hit->scanEpoch=scanEpoch;
        hit->messageTick=messageTick;
        hit->targetRadiusBits=targetRadiusBits;
        hit->distanceSquaredBits=floatBits(distanceSquared);
        hit->createdMs=now;
        hit->scanOrigin[0]=scanOrigin[0];
        hit->scanOrigin[1]=scanOrigin[1];
        hit->scanOrigin[2]=scanOrigin[2];
        g_pendingCargoCount++;
        _InterlockedExchange(&g_pendingPublishedCount,(long)g_pendingCargoCount);
        updateAtomicMaximum(&g_pendingHighWater,g_pendingCargoCount);
        _InterlockedIncrement(&g_pendingRecorded);
        result=PENDING_REMEMBER_RECORDED;
        break;
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    return result;
}

static bool markPendingCargoReady(u8* realized){
    if(!realized||
       _InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1||
       !rangeReadable(realized,REALIZED_BAGGAGE_SIZE)||
       !hasExactVtable(realized,RVA_REALIZED_BAGGAGE_VTABLE))return false;
    // Read all engine-owned state before taking our table lock. The init hook
    // only publishes an identity; it never calls Found/Marker itself.
    u64 baggagePointer=0,reciprocalRealized=0;
    if(!readU64(
           realized+OFF_REALIZED_BAGGAGE_COMPONENT,&baggagePointer
       )||!baggagePointer||
       !rangeReadable((u8*)baggagePointer,BAGGAGE_COMPONENT_SIZE)||
       !hasExactVtable((u8*)baggagePointer,RVA_BAGGAGE_COMPONENT_VTABLE)||
       !readU64(
           (u8*)baggagePointer+OFF_BAGGAGE_REALIZED_COMPONENT,
           &reciprocalRealized
       )||reciprocalRealized!=(u64)realized)return false;

    bool marked=false;
    bool conflict=false;
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    u64 now=pendingNowMs();
    u32 currentEpoch=(u32)_InterlockedCompareExchange(
        &g_scanAttemptEpoch,0,0
    );
    u32 currentSerial=(u32)_InterlockedCompareExchange(
        &g_farRefreshScanSerial,0,0
    );
    purgePendingCargoLocked(currentEpoch,now,false);
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        PendingCargoHit* hit=&g_pendingCargoHits[i];
        if(hit->baggage!=(u8*)baggagePointer)continue;
        if(hit->scanEpoch!=currentEpoch||hit->sweepSerial!=currentSerial){
            clearPendingCargoSlotLocked(i);
            break;
        }
        if(!hit->readyRealized||hit->readyRealized==realized){
            hit->readyRealized=realized;
            marked=true;
        }else{
            clearPendingCargoSlotLocked(i);
            conflict=true;
        }
        break;
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    if(conflict)_InterlockedIncrement(&g_pendingIdentityConflicts);
    return marked;
}

static bool takePendingCargoHitMatching(
    u8* baggage,u8* readyRealized,PendingClaimMode mode,
    PendingCargoHit* out
){
    if(out)memset(out,0,sizeof(*out));
    if(!baggage||!out)return false;
    bool found=false;
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    u64 now=pendingNowMs();
    u32 currentEpoch=(u32)_InterlockedCompareExchange(&g_scanAttemptEpoch,0,0);
    u32 currentSerial=(u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0);
    purgePendingCargoLocked(currentEpoch,now,false);
    bool conflict=false;
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        PendingCargoHit* hit=&g_pendingCargoHits[i];
        if(hit->baggage!=baggage)continue;
        bool compatible=mode==PENDING_CLAIM_EXACT_READY?
            readyRealized&&hit->readyRealized==readyRealized:
            !hit->readyRealized||hit->readyRealized==readyRealized;
        if(!compatible){
            clearPendingCargoSlotLocked(i);
            conflict=true;
            break;
        }
        if(hit->scanEpoch==currentEpoch&&hit->sweepSerial==currentSerial){
            *out=*hit;
            found=true;
        }
        clearPendingCargoSlotLocked(i);
        break;
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    if(conflict)_InterlockedIncrement(&g_pendingIdentityConflicts);
    return found;
}

static void clearFreshHitIfStillDetected(u8* detected){
    if(detected&&rangeReadable(detected,DETECTED_COMPONENT_SIZE)&&
       hasExactVtable(detected,RVA_DETECTED_COMPONENT_VTABLE)){
        _InterlockedExchange8(
            (volatile char*)(detected+OFF_DETECTED_FRESH_HIT),0
        );
    }
}

static bool purgePendingCargoForBaggage(u8* baggage){
    if(!baggage)return false;
    bool purged=false;
    AcquireSRWLockExclusive(&g_pendingCargoLock);
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS;i++){
        if(g_pendingCargoHits[i].baggage==baggage){
            clearPendingCargoSlotLocked(i);
            purged=true;
        }
    }
    ReleaseSRWLockExclusive(&g_pendingCargoLock);
    return purged;
}

static bool realizeLeaseExpired(const RealizeLease* lease,u64 now){
    return !lease||!lease->owner||now<lease->createdMs||
           now-lease->createdMs>=REALIZE_LEASE_TTL_MS;
}

// Caller holds g_realizeLeaseLock exclusively.
static void clearRealizeLeaseSlotLocked(u32 index){
    if(index>=MAX_REALIZE_LEASES||!g_realizeLeases[index].owner)return;
    memset(&g_realizeLeases[index],0,sizeof(g_realizeLeases[index]));
    if(g_realizeLeaseCount)g_realizeLeaseCount--;
    _InterlockedExchange(
        &g_realizeLeasePublishedCount,(long)g_realizeLeaseCount
    );
}

// Caller holds g_realizeLeaseLock exclusively. No engine-owned pointer is
// dereferenced here; identity validation happens only after a bounded copy.
static void purgeRealizeLeasesLocked(u32 currentEpoch,u64 now,bool clearAll){
    for(u32 i=0u;i<MAX_REALIZE_LEASES;i++){
        RealizeLease* lease=&g_realizeLeases[i];
        if(!lease->owner)continue;
        bool expired=realizeLeaseExpired(lease,now);
        bool foreign=clearAll||!currentEpoch||lease->scanEpoch!=currentEpoch;
        if(!expired&&!foreign)continue;
        clearRealizeLeaseSlotLocked(i);
        if(expired)_InterlockedIncrement(&g_realizeLeaseExpired);
        else _InterlockedIncrement(&g_realizeLeaseEpochPurged);
    }
}

static void purgeRealizeLeases(bool clearAll){
    AcquireSRWLockExclusive(&g_realizeLeaseLock);
    u32 currentEpoch=clearAll?0u:(u32)_InterlockedCompareExchange(
        &g_scanAttemptEpoch,0,0
    );
    purgeRealizeLeasesLocked(currentEpoch,pendingNowMs(),clearAll);
    ReleaseSRWLockExclusive(&g_realizeLeaseLock);
}

static RealizeLeaseRememberResult publishRealizeLease(
    u8* owner,u8* baggage,u8* detected,u32 scanEpoch,u32 sweepSerial
){
    if(!owner||!baggage||!detected||!scanEpoch||!sweepSerial||
       !rangeReadable(owner,0x310u))return REALIZE_LEASE_REJECTED;

    RealizeLeaseRememberResult result=REALIZE_LEASE_REJECTED;
    AcquireSRWLockExclusive(&g_realizeLeaseLock);
    u64 now=pendingNowMs();
    u32 currentEpoch=(u32)_InterlockedCompareExchange(
        &g_scanAttemptEpoch,0,0
    );
    u32 currentSerial=(u32)_InterlockedCompareExchange(
        &g_farRefreshScanSerial,0,0
    );
    purgeRealizeLeasesLocked(currentEpoch,now,false);
    if(currentEpoch!=scanEpoch||currentSerial!=sweepSerial||
       _InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1){
        ReleaseSRWLockExclusive(&g_realizeLeaseLock);
        return REALIZE_LEASE_REJECTED;
    }

    for(u32 i=0u;i<MAX_REALIZE_LEASES;i++){
        RealizeLease* lease=&g_realizeLeases[i];
        if(!lease->owner)continue;
        bool exact=lease->owner==owner&&lease->baggage==baggage&&
                   lease->detected==detected&&lease->scanEpoch==scanEpoch&&
                   lease->sweepSerial==sweepSerial;
        if(exact){
            _InterlockedIncrement(&g_realizeLeaseDedupe);
            result=REALIZE_LEASE_DEDUPE;
            ReleaseSRWLockExclusive(&g_realizeLeaseLock);
            return result;
        }
        if(lease->owner==owner||lease->baggage==baggage||
           lease->detected==detected){
            clearRealizeLeaseSlotLocked(i);
            _InterlockedIncrement(&g_realizeLeaseIdentityPurged);
            ReleaseSRWLockExclusive(&g_realizeLeaseLock);
            return REALIZE_LEASE_REJECTED;
        }
    }
    if(g_realizeLeaseCount>=MAX_REALIZE_LEASES){
        _InterlockedIncrement(&g_realizeLeaseFull);
        ReleaseSRWLockExclusive(&g_realizeLeaseLock);
        return REALIZE_LEASE_FULL;
    }
    for(u32 i=0u;i<MAX_REALIZE_LEASES;i++){
        RealizeLease* lease=&g_realizeLeases[i];
        if(lease->owner)continue;
        // Publish every identity field under the exclusive lock before the
        // count becomes visible to the coordinator relay.
        lease->owner=owner;
        lease->baggage=baggage;
        lease->detected=detected;
        lease->scanEpoch=scanEpoch;
        lease->sweepSerial=sweepSerial;
        lease->createdMs=now;
        g_realizeLeaseCount++;
        _InterlockedExchange(
            &g_realizeLeasePublishedCount,(long)g_realizeLeaseCount
        );
        updateAtomicMaximum(&g_realizeLeaseHighWater,g_realizeLeaseCount);
        _InterlockedIncrement(&g_realizeLeaseRecorded);
        result=REALIZE_LEASE_RECORDED;
        break;
    }
    ReleaseSRWLockExclusive(&g_realizeLeaseLock);
    return result;
}

static bool purgeRealizeLeaseExact(const RealizeLease* expected){
    if(!expected||!expected->owner)return false;
    bool purged=false;
    AcquireSRWLockExclusive(&g_realizeLeaseLock);
    for(u32 i=0u;i<MAX_REALIZE_LEASES;i++){
        const RealizeLease* lease=&g_realizeLeases[i];
        if(lease->owner==expected->owner&&lease->baggage==expected->baggage&&
           lease->detected==expected->detected&&
           lease->scanEpoch==expected->scanEpoch&&
           lease->sweepSerial==expected->sweepSerial){
            clearRealizeLeaseSlotLocked(i);
            purged=true;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_realizeLeaseLock);
    return purged;
}

static bool purgeRealizeLeaseForBaggage(u8* baggage){
    if(!baggage)return false;
    bool purged=false;
    AcquireSRWLockExclusive(&g_realizeLeaseLock);
    for(u32 i=0u;i<MAX_REALIZE_LEASES;i++){
        if(g_realizeLeases[i].baggage!=baggage)continue;
        clearRealizeLeaseSlotLocked(i);
        purged=true;
    }
    ReleaseSRWLockExclusive(&g_realizeLeaseLock);
    return purged;
}

static u32 snapshotRealizeLeases(RealizeLease* leases,u32 capacity){
    if(!leases||!capacity)return 0u;
    u32 count=0u;
    AcquireSRWLockExclusive(&g_realizeLeaseLock);
    u32 currentEpoch=(u32)_InterlockedCompareExchange(
        &g_scanAttemptEpoch,0,0
    );
    purgeRealizeLeasesLocked(currentEpoch,pendingNowMs(),false);
    for(u32 i=0u;i<MAX_REALIZE_LEASES&&count<capacity;i++){
        if(!g_realizeLeases[i].owner)continue;
        leases[count++]=g_realizeLeases[i];
    }
    ReleaseSRWLockExclusive(&g_realizeLeaseLock);
    return count;
}

static bool validateRealizeLeaseIdentity(const RealizeLease* lease){
    if(!lease||!lease->owner||!lease->baggage||!lease->detected||
       realizeLeaseExpired(lease,pendingNowMs())||
       lease->scanEpoch!=(u32)_InterlockedCompareExchange(
           &g_scanAttemptEpoch,0,0
       )||lease->sweepSerial!=(u32)_InterlockedCompareExchange(
           &g_farRefreshScanSerial,0,0
       )||!rangeReadable(lease->owner,0x310u))return false;
    u8* currentOwner=0;
    CargoBridgeObjects current;
    if(!validateDetectedRefreshComponent(lease->detected,&currentOwner)||
       currentOwner!=lease->owner)return false;
    CargoBridgeResult state=resolveCargoBridgeObjects(
        lease->detected,currentOwner,&current
    );
    return (state==CARGO_BRIDGE_UNREALIZED||state==CARGO_BRIDGE_READY)&&
           current.baggage==lease->baggage;
}

static bool requestEntityRealize(u8* owner,bool relay){
    RequestEntityRealize request=0;
#ifdef ODRADEK_TEST_BUILD
    if(g_testRequestEntityRealize){
        request=(RequestEntityRealize)g_testRequestEntityRealize;
    }
#endif
    if(!request&&g_gameBase){
        request=(RequestEntityRealize)(g_gameBase+RVA_ENTITY_REQUEST_REALIZE);
    }
    if(!request||!owner||!rangeReadable(owner,0x310u))return false;
    // The initial inactive->realize request mirrors the native baggage-manager
    // call.  The coordinator relay only cancels a pending unrealize request;
    // r8=0 avoids requesting the unrelated post-realize event in case the
    // entity changes state between validation and this call.
    request(owner,0u,relay?0u:1u);
    _InterlockedIncrement(
        relay?&g_realizeRequestRelay:&g_realizeRequestInitial
    );
    return true;
}

extern "C" void realizationCoordinatorHook(void* coordinator){
    RealizationCoordinator original=(RealizationCoordinator)(u64)
        g_originalRealizationCoordinator;
    if(!original)return;
    // Preserve the native coordinator exactly once, then cancel only pending
    // unrealize transitions for the bounded, scan-owned entity set.
    original(coordinator);
    _InterlockedIncrement(&g_realizeCoordinatorCalls);
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1||
       _InterlockedCompareExchange(&g_realizeLeasePublishedCount,0,0)<=0){
        return;
    }
    RealizeLease leases[MAX_REALIZE_LEASES];
    memset(leases,0,sizeof(leases));
    u32 count=snapshotRealizeLeases(leases,MAX_REALIZE_LEASES);
    if(count)_InterlockedIncrement(&g_realizeCoordinatorSnapshots);
    for(u32 i=0u;i<count;i++){
        if(!validateRealizeLeaseIdentity(&leases[i])){
            if(purgeRealizeLeaseExact(&leases[i])){
                _InterlockedIncrement(&g_realizeLeaseIdentityPurged);
            }
            continue;
        }
        u64 lifecycleFlags=0u;
        if(!readU64(
               leases[i].owner+OFF_ENTITY_LIFECYCLE_FLAGS,&lifecycleFlags
           )||
           (lifecycleFlags&ENTITY_FLAG_INACTIVE)!=0u||
           (lifecycleFlags&ENTITY_FLAG_REQUEST_UNREALIZE)==0u||
           (lifecycleFlags&ENTITY_FLAG_STAGED_UNREALIZE)!=0u){
            continue;
        }
        requestEntityRealize(leases[i].owner,true);
    }
}

static PendingValidationResult validatePendingCargoHit(
    const PendingCargoHit* hit,u8* realized,CargoBridgeObjects* objects,
    float* distanceSquared
){
    if(objects){objects->baggage=0;objects->realized=0;}
    if(distanceSquared)*distanceSquared=0.0f;
    if(pendingCargoExpired(hit,pendingNowMs())){
        return PENDING_VALIDATION_EXPIRED;
    }
    if(!hit||!realized||!objects||!distanceSquared||
       !hit->baggage||!hit->detected||!hit->owner||
       _InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1||
       (u32)_InterlockedCompareExchange(&g_scanAttemptEpoch,0,0)!=hit->scanEpoch||
       (u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0)!=hit->sweepSerial){
        return PENDING_VALIDATION_IDENTITY_REJECT;
    }
    u8* currentOwner=0;
    u64 realizedOwner=0u;
    CargoBridgeObjects current;
    if(!validateDetectedRefreshComponent(hit->detected,&currentOwner)||
       currentOwner!=hit->owner||
       resolveCargoBridgeObjects(hit->detected,currentOwner,&current)!=
           CARGO_BRIDGE_READY||
       current.baggage!=hit->baggage||current.realized!=realized||
       !readU64(realized+OFF_DETECTED_OWNER,&realizedOwner)||
       realizedOwner!=(u64)hit->owner){
        return PENDING_VALIDATION_IDENTITY_REJECT;
    }
    float target=0.0f,initialDistance=0.0f;
    if(!finitePositiveFloat(hit->targetRadiusBits,&target)||
       !finiteFloat(hit->distanceSquaredBits,&initialDistance)||
       initialDistance<0.0f||initialDistance>target*target||
       !rangeReadable(
           hit->owner+OFF_ENTITY_WORLD_POSITION,3u*sizeof(double)
       ))return PENDING_VALIDATION_DISTANCE_REJECT;
    const double* position=(const double*)(hit->owner+OFF_ENTITY_WORLD_POSITION);
    double dx=position[0]-hit->scanOrigin[0];
    double dy=position[1]-hit->scanOrigin[1];
    double dz=position[2]-hit->scanOrigin[2];
    double squared=dx*dx+dy*dy+dz*dz;
    double allowed=(double)target+(double)FAR_REFRESH_END_TOLERANCE_METERS;
    if(!(squared>=0.0)||squared>allowed*allowed)return PENDING_VALIDATION_DISTANCE_REJECT;
    *objects=current;
    *distanceSquared=(float)squared;
    return PENDING_VALIDATION_READY;
}

static bool consumePendingCargoHitKnown(
    u8* realized,u8* baggage,bool requireReady,bool countNoMatch
){
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1||
       !realized||!baggage)return false;
    if(realizedInitInFlight(realized))return false;
    PendingCargoHit hit;
    if(!takePendingCargoHitMatching(
           baggage,realized,
           requireReady?PENDING_CLAIM_EXACT_READY:
               PENDING_CLAIM_UNREADY_OR_SAME,
           &hit
       )){
        if(countNoMatch)_InterlockedIncrement(&g_pendingInitNoMatch);
        return false;
    }
    u64 now=pendingNowMs();
    u64 age=now>=hit.createdMs?now-hit.createdMs:0ull;
    updateAtomicMaximum(
        &g_pendingMaxConsumeAgeMs,age>0xFFFFFFFFull?0xFFFFFFFFu:(u32)age
    );
    CargoBridgeObjects objects;
    float distanceSquared=0.0f;
    PendingValidationResult valid=validatePendingCargoHit(
        &hit,realized,&objects,&distanceSquared
    );
    if(valid==PENDING_VALIDATION_EXPIRED){
        _InterlockedIncrement(&g_pendingExpired);
        return false;
    }
    if(valid==PENDING_VALIDATION_IDENTITY_REJECT){
        _InterlockedIncrement(&g_pendingInitIdentityReject);
        return false;
    }
    if(valid==PENDING_VALIDATION_DISTANCE_REJECT){
        _InterlockedIncrement(&g_pendingInitDistanceReject);
        return false;
    }
    if((u32)_InterlockedCompareExchange(
           &g_scanAttemptEpoch,0,0
       )!=hit.scanEpoch||(u32)_InterlockedCompareExchange(
           &g_farRefreshScanSerial,0,0
       )!=hit.sweepSerial||pendingCargoExpired(&hit,pendingNowMs())||
       realizedInitInFlight(realized)||
       !validateCargoBridgeObjects(hit.detected,&objects)){
        _InterlockedIncrement(&g_pendingInitIdentityReject);
        return false;
    }
    char prior=_InterlockedExchange8(
        (volatile char*)(hit.detected+OFF_DETECTED_FRESH_HIT),1
    );
    if(prior==1)_InterlockedIncrement(&g_pendingNaturalFresh);
    else _InterlockedIncrement(&g_pendingSyntheticFresh);
    bool foundCalled=false,markerCalled=false;
    CargoBridgeResult bridged=bridgeFreshCargoHit(
        hit.detected,&objects,distanceSquared,&foundCalled,&markerCalled
    );
    if(foundCalled)_InterlockedIncrement(&g_pendingFoundCalls);
    if(markerCalled)_InterlockedIncrement(&g_pendingMarkerCalls);
    if(bridged==CARGO_BRIDGE_READY){
        _InterlockedIncrement(&g_pendingConsumed);
        return true;
    }else{
        clearFreshHitIfStillDetected(hit.detected);
        _InterlockedIncrement(&g_pendingBridgeFailed);
    }
    return false;
}

struct ReadyPendingCargoCandidate{
    u8* baggage;
    u8* realized;
};

static u32 snapshotReadyPendingCargo(
    ReadyPendingCargoCandidate* candidates,u32 capacity
){
    if(!candidates||!capacity)return 0u;
    u32 count=0u;
    AcquireSRWLockShared(&g_pendingCargoLock);
    for(u32 i=0u;i<MAX_PENDING_CARGO_HITS&&count<capacity;i++){
        const PendingCargoHit* hit=&g_pendingCargoHits[i];
        if(!hit->baggage||!hit->readyRealized)continue;
        candidates[count].baggage=hit->baggage;
        candidates[count].realized=hit->readyRealized;
        count++;
    }
    ReleaseSRWLockShared(&g_pendingCargoLock);
    return count;
}

[[maybe_unused]] static void drainReadyPendingCargoHits(){
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1||
       _InterlockedCompareExchange(&g_pendingPublishedCount,0,0)<=0)return;
    // Expiry is maintained even when no matching Realized object ever loads.
    purgePendingCargo(false);
    if(_InterlockedCompareExchange(&g_pendingPublishedCount,0,0)<=0)return;
    ReadyPendingCargoCandidate candidates[MAX_PENDING_CARGO_DRAIN_PER_UPDATE];
    memset(candidates,0,sizeof(candidates));
    u32 count=snapshotReadyPendingCargo(
        candidates,MAX_PENDING_CARGO_DRAIN_PER_UPDATE
    );
    for(u32 i=0u;i<count;i++){
        consumePendingCargoHitKnown(
            candidates[i].realized,candidates[i].baggage,true,false
        );
    }
}

extern "C" void realizedBaggageInitHook(u8* realized,void* message){
    RealizedBaggageLifecycle original=(RealizedBaggageLifecycle)(u64)
        g_originalRealizedBaggageInit;
    if(!original)return;
    // Links become visible before the native initializer is finished. Track
    // the exact object across the native call without holding our lock so no
    // scan bridge can treat that half-initialized state as ready.
    RealizedInitFlightToken flight=beginRealizedInitFlight(realized);
    original(realized,message);
    endRealizedInitFlight(&flight);
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1)return;
    // Most lifecycle events have no delayed scan hit. Avoid VTable reads and
    // lock traffic on save/load bursts; the in-flight-aware post-publish
    // catch-up closes the zero-to-one race.
    if(_InterlockedCompareExchange(&g_pendingPublishedCount,0,0)<=0)return;
    _InterlockedIncrement(&g_pendingInitCalls);
    if(!markPendingCargoReady(realized)){
        _InterlockedIncrement(&g_pendingInitNoMatch);
    }
}

extern "C" void realizedBaggageUnlinkHook(u8* realized,void* message){
    RealizedBaggageLifecycle original=(RealizedBaggageLifecycle)(u64)
        g_originalRealizedBaggageUnlink;
    if(!original)return;
    u64 baggagePointer=0;
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)==1&&realized&&
       rangeReadable(realized,REALIZED_BAGGAGE_SIZE)&&
       hasExactVtable(realized,RVA_REALIZED_BAGGAGE_VTABLE)){
        readU64(realized+OFF_REALIZED_BAGGAGE_COMPONENT,&baggagePointer);
    }
    if(baggagePointer&&purgePendingCargoForBaggage((u8*)baggagePointer)){
        _InterlockedIncrement(&g_pendingUnlinkPurged);
    }
    if(baggagePointer&&purgeRealizeLeaseForBaggage((u8*)baggagePointer)){
        _InterlockedIncrement(&g_realizeLeaseUnlinkPurged);
    }
    // Purge while the reciprocal link still exists, then let the native
    // handler destroy it exactly once.
    original(realized,message);
}

// Capture only genuine one-shot hits produced by the game's own detected
// component update. This callback already runs inside the entity dispatcher,
// so the component graph is live while it is inspected. READY cargo remains
// entirely native. Only an exact UNREALIZED Lost Cargo hit is transferred to
// the bounded pending table before the one-shot byte is retired.
static void captureNaturalFreshCargoHit(u8* detected,void* message){
    if(!detected||detected[OFF_DETECTED_FRESH_HIT]!=1u)return;

    u32 serial=0u;
    bool presentationActive=false;
    scanPresentationSnapshot(&serial,&presentationActive);
    u32 currentSerial=(u32)_InterlockedCompareExchange(
        &g_farRefreshScanSerial,0,0
    );
    u32 scanEpoch=(u32)_InterlockedCompareExchange(
        &g_farRefreshScanEpoch,0,0
    );
    u32 currentEpoch=(u32)_InterlockedCompareExchange(
        &g_scanAttemptEpoch,0,0
    );
    if(!presentationActive||!serial||serial!=currentSerial||
       !scanEpoch||scanEpoch!=currentEpoch)return;

    u64 managerPointer=0u;
    u32 managerActive=0u,snapshotActive=0u;
    u32 currentGeneration=0u,snapshotGeneration=0u;
    if(!g_gameBase||!readU64(
           g_gameBase+RVA_SENSOR_MANAGER_GLOBAL,&managerPointer
       )||!managerPointer||
       !hasExactVtable((u8*)managerPointer,RVA_SENSOR_MANAGER_VTABLE)||
       !readU32((u8*)managerPointer+OFF_MANAGER_ACTIVE,&managerActive)||
       (managerActive&0xFFu)!=1u||
       !readU32(
           (u8*)managerPointer+OFF_MANAGER_SCAN_SNAPSHOT,&snapshotActive
       )||(snapshotActive&0xFFu)!=1u||
       !readU32(
           (u8*)managerPointer+OFF_MANAGER_CURRENT_GENERATION,
           &currentGeneration
       )||!readU32(
           (u8*)managerPointer+OFF_MANAGER_SNAPSHOT_GENERATION,
           &snapshotGeneration
       )||currentGeneration!=snapshotGeneration){
        _InterlockedIncrement(&g_farRefreshManagerInvalid);
        return;
    }

    _InterlockedIncrement(&g_farRefreshFreshHits);
    u8* owner=0;
    if(!validateDetectedRefreshComponent(detected,&owner)){
        _InterlockedIncrement(&g_farRefreshDetectedInvalid);
        return;
    }
    _InterlockedIncrement(&g_farRefreshDetectedValid);

    float distanceSquared=0.0f,target=0.0f;
    if(!detectedDistanceSquared(
           (u8*)managerPointer,owner,&distanceSquared
       )||!finitePositiveFloat(g_targetScanRadiusBits,&target)){
        _InterlockedIncrement(&g_farRefreshDistanceInvalid);
        return;
    }
    updateAtomicMaximum(
        &g_farRefreshMaxDetectedDistanceSquaredBits,
        floatBits(distanceSquared)
    );
    if(distanceSquared>target*target){
        _InterlockedIncrement(&g_farRefreshOutsideRadius);
        return;
    }
    _InterlockedIncrement(&g_farRefreshInRadius);

    CargoBridgeObjects cargo;
    CargoBridgeResult cargoState=resolveCargoBridgeObjects(
        detected,owner,&cargo
    );
    if(cargoState==CARGO_BRIDGE_NOT_APPLICABLE){
        _InterlockedIncrement(&g_farRefreshNoBaggage);
        return;
    }
    if(cargoState==CARGO_BRIDGE_FAILED||!cargo.baggage){
        _InterlockedIncrement(&g_farRefreshInvalidCargo);
        return;
    }
    _InterlockedIncrement(&g_farRefreshBaggageFound);
    updateAtomicMaximum(
        &g_farRefreshMaxCargoDistanceSquaredBits,floatBits(distanceSquared)
    );
    if(cargoState==CARGO_BRIDGE_READY){
        // The native Realized-Baggage update owns this fresh hit. Calling
        // Found/Marker here would process the same event twice.
        _InterlockedIncrement(&g_farRefreshRealizedFound);
        return;
    }

    _InterlockedIncrement(&g_farRefreshUnrealized);
    // Publish the complete delayed native hit before publishing the separate
    // realization lease. The coordinator relay can therefore never expose a
    // Realized component for which no exact replay identity exists.
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1)return;
    // A missing Realized-Baggage link alone does not prove that the owner is
    // safely inactive. RequestRealize is only an activation request in this
    // exact state. For an active or already-unrealizing owner it would either
    // do nothing or cancel an unrelated native transition, so leave the fresh
    // hit untouched and fail closed.
    u64 lifecycleFlags=0u;
    if(!readU64(
           owner+OFF_ENTITY_LIFECYCLE_FLAGS,&lifecycleFlags
       )||(lifecycleFlags&ENTITY_FLAG_INACTIVE)==0u||
       (lifecycleFlags&(
           ENTITY_FLAG_REQUEST_UNREALIZE|ENTITY_FLAG_STAGED_UNREALIZE
       ))!=0u){
        _InterlockedIncrement(&g_realizeInitialStateRejected);
        return;
    }
    DetectedMessageKey key;
    if(!detectedMessageKey(message,&key)||
       !rangeReadable(
           (u8*)managerPointer+0x38u,3u*sizeof(double)
       )){
        _InterlockedIncrement(&g_farRefreshContextInvalid);
        return;
    }
    double scanOrigin[3];
    const double* origin=(const double*)((u8*)managerPointer+0x38u);
    scanOrigin[0]=origin[0];
    scanOrigin[1]=origin[1];
    scanOrigin[2]=origin[2];
    PendingRememberResult pending=rememberPendingCargoHit(
        cargo.baggage,detected,owner,serial,scanEpoch,key.tick,
        g_targetScanRadiusBits,distanceSquared,scanOrigin
    );
    if(pending==PENDING_REMEMBER_RECORDED||
       pending==PENDING_REMEMBER_DEDUPE){
        RealizeLeaseRememberResult lease=publishRealizeLease(
            owner,cargo.baggage,detected,scanEpoch,serial
        );
        if(lease==REALIZE_LEASE_RECORDED||
           lease==REALIZE_LEASE_DEDUPE){
            // Same-entity message dispatch is serialized. Retire the one-shot
            // only after both bounded records are visible, then ask the native
            // entity state machine to realize this exact owner.
            _InterlockedExchange8(
                (volatile char*)(detected+OFF_DETECTED_FRESH_HIT),0
            );
            if(requestEntityRealize(owner,false)){
                _InterlockedIncrement(&g_farRefreshDeferredUnrealized);
            }else{
                // A validated runtime should never take this path. Restore the
                // native hit and withdraw both publications fail closed.
                _InterlockedExchange8(
                    (volatile char*)(detected+OFF_DETECTED_FRESH_HIT),1
                );
                purgeRealizeLeaseForBaggage(cargo.baggage);
                purgePendingCargoForBaggage(cargo.baggage);
                _InterlockedIncrement(&g_farRefreshPostCoreBridgeFailed);
            }
        }else{
            purgePendingCargoForBaggage(cargo.baggage);
            if(lease==REALIZE_LEASE_FULL){
                _InterlockedIncrement(&g_farRefreshClaimFull);
            }else{
                _InterlockedIncrement(&g_farRefreshPostCoreBridgeFailed);
            }
        }
    }else if(pending==PENDING_REMEMBER_FULL){
        _InterlockedIncrement(&g_farRefreshClaimFull);
    }else{
        _InterlockedIncrement(&g_farRefreshPostCoreBridgeFailed);
    }
}

extern "C" void realizedBaggageUpdateHook(u8* realized,void* message){
    RealizedBaggageLifecycle original=(RealizedBaggageLifecycle)(u64)
        g_originalRealizedBaggageUpdate;
    if(!original)return;

    bool injected=false;
    if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)==1&&
       _InterlockedCompareExchange(&g_pendingPublishedCount,0,0)>0&&
       realized&&!realizedInitInFlight(realized)&&
       rangeReadable(realized,REALIZED_BAGGAGE_SIZE)&&
       hasExactVtable(realized,RVA_REALIZED_BAGGAGE_VTABLE)){
        u64 baggagePointer=0u;
        if(readU64(
               realized+OFF_REALIZED_BAGGAGE_COMPONENT,&baggagePointer
           )&&baggagePointer){
            PendingCargoHit hit;
            if(takePendingCargoHitMatching(
                   (u8*)baggagePointer,realized,
                   PENDING_CLAIM_EXACT_READY,&hit
               )){
                u64 now=pendingNowMs();
                u64 age=now>=hit.createdMs?now-hit.createdMs:0ull;
                updateAtomicMaximum(
                    &g_pendingMaxConsumeAgeMs,
                    age>0xFFFFFFFFull?0xFFFFFFFFu:(u32)age
                );
                CargoBridgeObjects objects;
                float distanceSquared=0.0f;
                PendingValidationResult valid=validatePendingCargoHit(
                    &hit,realized,&objects,&distanceSquared
                );
                if(valid==PENDING_VALIDATION_READY&&
                   objects.baggage==(u8*)baggagePointer&&
                   objects.realized==realized){
                    char prior=_InterlockedExchange8(
                        (volatile char*)(
                            hit.detected+OFF_DETECTED_FRESH_HIT
                        ),1
                    );
                    if(prior==1)_InterlockedIncrement(
                        &g_pendingNaturalFresh
                    );
                    else _InterlockedIncrement(&g_pendingSyntheticFresh);
                    injected=true;
                }else if(valid==PENDING_VALIDATION_EXPIRED){
                    _InterlockedIncrement(&g_pendingExpired);
                }else if(valid==PENDING_VALIDATION_DISTANCE_REJECT){
                    _InterlockedIncrement(&g_pendingInitDistanceReject);
                }else{
                    _InterlockedIncrement(&g_pendingInitIdentityReject);
                }
            }
        }
    }

    // The native update calculates DistanceSq, consumes the fresh bit through
    // its own Found event, and runs its normal marker state machine exactly
    // once. No manual Found/Marker call is made by the mod.
    original(realized,message);
    if(injected){
        _InterlockedIncrement(&g_pendingNativeUpdateCalls);
        _InterlockedIncrement(&g_pendingConsumed);
    }
}

static bool snapshotDetectedComponents(
    u8* manager,u32* snapshotCount,u32* managerCount,bool* truncated
){
    if(!manager||!snapshotCount||!managerCount||!truncated||
       !rangeReadable(manager,OFF_MANAGER_DETECTED_LOCK+sizeof(SRWLOCK_X64)))return false;
    *snapshotCount=0u;
    *managerCount=0u;
    *truncated=false;
    SRWLOCK_X64* lock=(SRWLOCK_X64*)(manager+OFF_MANAGER_DETECTED_LOCK);
    AcquireSRWLockShared(lock);
    u32 count=*(volatile u32*)(manager+OFF_MANAGER_DETECTED_COUNT);
    u32 capacity=*(volatile u32*)(manager+OFF_MANAGER_DETECTED_CAPACITY);
    u8** items=*(u8***)(manager+OFF_MANAGER_DETECTED_ITEMS);
    bool valid=count<=capacity&&capacity<=MAX_DETECTED_MANAGER_CAPACITY&&
               (!count||items);
    u32 copyCount=count;
    if(copyCount>MAX_DETECTED_REFRESH_COMPONENTS){
        copyCount=MAX_DETECTED_REFRESH_COMPONENTS;
        *truncated=true;
    }
    if(valid&&copyCount){
        valid=rangeReadable(items,(u64)copyCount*sizeof(u8*));
        if(valid){
            for(u32 i=0;i<copyCount;i++)g_farRefreshSnapshot[i]=items[i];
        }
    }
    ReleaseSRWLockShared(lock);
    if(!valid)return false;
    *snapshotCount=copyCount;
    *managerCount=count;
    return true;
}

extern "C" void detectedConstantUpdateHook(u8* current,void* message){
    DetectedConstantUpdate original=
        (DetectedConstantUpdate)(u64)g_originalDetectedConstantUpdate;
    if(!original)return;
    original(current,message);
    captureNaturalFreshCargoHit(current,message);
}

[[maybe_unused]] static void tryFarRefresh(void* message){
    DetectedConstantUpdate original=
        (DetectedConstantUpdate)(u64)g_originalDetectedConstantUpdate;
    if(!original)return;
    if(_InterlockedCompareExchange(&g_farRefreshReady,0,0)!=1||
       _InterlockedCompareExchange(&g_farRefreshPending,0,0)!=1)return;
    if(!g_gameBase){
        _InterlockedIncrement(&g_farRefreshManagerMissing);
        return;
    }
    if(!message){
        _InterlockedIncrement(&g_farRefreshContextInvalid);
        return;
    }

    u64 managerPointer=0;
    if(!readU64(g_gameBase+RVA_SENSOR_MANAGER_GLOBAL,&managerPointer)||
       !managerPointer){
        _InterlockedIncrement(&g_farRefreshManagerMissing);
        return;
    }
    u8* manager=(u8*)managerPointer;
    u32 active=0,radiusBits=0;
    u32 snapshotActive=0,currentGeneration=0,snapshotGeneration=0;
    float radius=0.0f,target=0.0f;
    if(!hasExactVtable(manager,RVA_SENSOR_MANAGER_VTABLE)){
        _InterlockedIncrement(&g_farRefreshManagerInvalid);
        return;
    }
    if(!readU32(manager+OFF_MANAGER_ACTIVE,&active)){
        _InterlockedIncrement(&g_farRefreshManagerInvalid);
        return;
    }
    _InterlockedExchange(&g_farRefreshLastManagerActive,(long)active);
    if((active&0xFFu)!=1u){
        _InterlockedIncrement(&g_farRefreshManagerInactive);
        return;
    }
    if(!readU32(manager+OFF_MANAGER_SCAN_SNAPSHOT,&snapshotActive)){
        _InterlockedIncrement(&g_farRefreshManagerInvalid);
        return;
    }
    _InterlockedExchange(&g_farRefreshLastSnapshotActive,(long)snapshotActive);
    if((snapshotActive&0xFFu)!=1u){
        _InterlockedIncrement(&g_farRefreshSnapshotInactive);
        return;
    }
    if(!readU32(
           manager+OFF_MANAGER_CURRENT_GENERATION,&currentGeneration
       )||!readU32(
           manager+OFF_MANAGER_SNAPSHOT_GENERATION,&snapshotGeneration
       )){
        _InterlockedIncrement(&g_farRefreshManagerInvalid);
        return;
    }
    _InterlockedExchange(
        &g_farRefreshLastCurrentGeneration,(long)currentGeneration
    );
    _InterlockedExchange(
        &g_farRefreshLastSnapshotGeneration,(long)snapshotGeneration
    );
    if(currentGeneration!=snapshotGeneration){
        _InterlockedIncrement(&g_farRefreshGenerationMismatch);
        return;
    }
    if(!readU32(manager+OFF_MANAGER_GATE_RADIUS,&radiusBits)){
        _InterlockedIncrement(&g_farRefreshRadiusInvalid);
        return;
    }
    _InterlockedExchange(&g_farRefreshRadiusBits,(long)radiusBits);
    if(!finitePositiveFloat(radiusBits,&radius)||
       !finitePositiveFloat(g_targetScanRadiusBits,&target)){
        _InterlockedIncrement(&g_farRefreshRadiusInvalid);
        return;
    }
    // The visual/runtime radius reaches 500 m, but the post-join sampling
    // point disappears a few metres earlier when the scan flips inactive.
    // Trigger in that observed end window and filter candidates by the exact
    // configured target below.
    if(radius+FAR_REFRESH_END_TOLERANCE_METERS<target){
        _InterlockedIncrement(&g_farRefreshRadiusPending);
        return;
    }

    u32 serial=(u32)_InterlockedCompareExchange(&g_farRefreshScanSerial,0,0);
    if(!serial||(u32)_InterlockedCompareExchange(
           &g_farRefreshPendingSerial,0,0
       )!=serial){
        _InterlockedIncrement(&g_farRefreshSerialMismatch);
        return;
    }
    u32 scanEpoch=(u32)_InterlockedCompareExchange(&g_farRefreshScanEpoch,0,0);
    if(!scanEpoch||(u32)_InterlockedCompareExchange(
           &g_scanAttemptEpoch,0,0
       )!=scanEpoch){
        _InterlockedIncrement(&g_farRefreshSerialMismatch);
        return;
    }

    if(_InterlockedCompareExchange(&g_farRefreshBusy,1,0)!=0){
        _InterlockedIncrement(&g_farRefreshBusyRejected);
        return;
    }
    if(_InterlockedCompareExchange(&g_farRefreshPending,0,1)!=1){
        _InterlockedIncrement(&g_farRefreshSerialMismatch);
        _InterlockedExchange(&g_farRefreshBusy,0);
        return;
    }
    u32 claimedSerial=(u32)_InterlockedCompareExchange(
        &g_farRefreshPendingSerial,0,0
    );
    if((u32)_InterlockedCompareExchange(
           &g_farRefreshScanSerial,0,0
       )!=serial||claimedSerial!=serial){
        _InterlockedIncrement(&g_farRefreshSerialMismatch);
        u32 completed=(u32)_InterlockedCompareExchange(
            &g_farRefreshCompletedSerial,0,0
        );
        if(claimedSerial&&completed!=claimedSerial){
            _InterlockedExchange(&g_farRefreshPending,1);
        }
        _InterlockedExchange(&g_farRefreshBusy,0);
        return;
    }
    if(!retainDetectedTickGuard(message)){
        _InterlockedIncrement(&g_farRefreshContextBucketFailed);
        _InterlockedExchange(&g_farRefreshPending,1);
        _InterlockedExchange(&g_farRefreshBusy,0);
        return;
    }

    DetectedMessageKey refreshKey;
    double scanOrigin[3]={0.0,0.0,0.0};
    if(!detectedMessageKey(message,&refreshKey)||
       !rangeReadable(manager+0x38u,3u*sizeof(double))){
        _InterlockedIncrement(&g_farRefreshContextInvalid);
        releaseDetectedTickGuard(message);
        _InterlockedExchange(&g_farRefreshPending,1);
        _InterlockedExchange(&g_farRefreshBusy,0);
        return;
    }
    const double* managerOrigin=(const double*)(manager+0x38u);
    scanOrigin[0]=managerOrigin[0];
    scanOrigin[1]=managerOrigin[1];
    scanOrigin[2]=managerOrigin[2];

    u32 snapshotCount=0,managerCount=0;
    bool truncated=false;
    if(!snapshotDetectedComponents(
        manager,&snapshotCount,&managerCount,&truncated
    )){
        _InterlockedIncrement(&g_farRefreshSnapshotFailed);
        releaseDetectedTickGuard(message);
        _InterlockedExchange(&g_farRefreshPending,1);
        _InterlockedExchange(&g_farRefreshBusy,0);
        return;
    }

    u32 eligible=0,calls=0,skipped=0,bridges=0,bridgeFailures=0;
    u32 detectedValid=0,detectedInvalid=0,distanceInvalid=0;
    u32 outsideRadius=0,inRadius=0,noBaggage=0,baggageFound=0;
    u32 realizedFound=0,unrealized=0,deferredUnrealized=0,invalidCargo=0;
    u32 claimDuplicate=0,claimFull=0,freshHits=0,freshMisses=0;
    u32 foundCalls=0,markerCalls=0,postCoreBridgeFailed=0;
    float maxDetectedDistanceSquared=0.0f,maxCargoDistanceSquared=0.0f;
    float radiusSquared=target*target;
    for(u32 i=0;i<snapshotCount;i++){
        u8* component=g_farRefreshSnapshot[i];
        u8* owner=0;
        if(!validateDetectedRefreshComponent(component,&owner)){
            detectedInvalid++;
            skipped++;
            continue;
        }
        detectedValid++;
        float distanceSquared=0.0f;
        if(!detectedDistanceSquared(manager,owner,&distanceSquared)){
            distanceInvalid++;
            skipped++;
            continue;
        }
        if(distanceSquared>maxDetectedDistanceSquared){
            maxDetectedDistanceSquared=distanceSquared;
        }
        if(distanceSquared>radiusSquared){
            outsideRadius++;
            skipped++;
            continue;
        }
        inRadius++;
        CargoBridgeObjects cargo;
        CargoBridgeResult cargoReady=resolveCargoBridgeObjects(
            component,owner,&cargo
        );
        if(cargo.baggage){
            baggageFound++;
            if(distanceSquared>maxCargoDistanceSquared){
                maxCargoDistanceSquared=distanceSquared;
            }
        }
        if(cargo.realized)realizedFound++;
        if(cargoReady==CARGO_BRIDGE_NOT_APPLICABLE){
            noBaggage++;
            skipped++;
            continue;
        }
        if(cargoReady==CARGO_BRIDGE_FAILED){
            invalidCargo++;
            bridgeFailures++;
            skipped++;
            continue;
        }
        // Reciprocal links are published before the native Realized init
        // routine has finished. Treat that short window like UNREALIZED: the
        // post-init hook will consume the delayed hit once the object is safe.
        bool realizedReady=cargoReady==CARGO_BRIDGE_READY&&
            !realizedInitInFlight(cargo.realized);
        if(!realizedReady)unrealized++;
        FanoutReserveResult reservation=reserveDetectedFanout(component,message);
        if(reservation!=FANOUT_RESERVE_READY){
            if(reservation==FANOUT_RESERVE_DUPLICATE)claimDuplicate++;
            else claimFull++;
            skipped++;
            continue;
        }
        eligible++;
        if((u32)_InterlockedCompareExchange(
               &g_scanAttemptEpoch,0,0
           )!=scanEpoch||(u32)_InterlockedCompareExchange(
               &g_farRefreshScanEpoch,0,0
           )!=scanEpoch){
            bridgeFailures++;
            postCoreBridgeFailed++;
            skipped++;
            continue;
        }
        original(component,message);
        calls++;
        if(component[OFF_DETECTED_FRESH_HIT]==1u)freshHits++;
        else{
            freshMisses++;
            skipped++;
            continue;
        }
        bool foundCalled=false,markerCalled=false;
        u8* confirmedOwner=0;
        CargoBridgeObjects confirmedCargo;
        CargoBridgeResult confirmedReady=CARGO_BRIDGE_FAILED;
        bool epochCurrent=(u32)_InterlockedCompareExchange(
                &g_scanAttemptEpoch,0,0
            )==scanEpoch&&(u32)_InterlockedCompareExchange(
                &g_farRefreshScanEpoch,0,0
            )==scanEpoch;
        if(epochCurrent&&validateDetectedRefreshComponent(
               component,&confirmedOwner
           )&&confirmedOwner==owner){
            confirmedReady=resolveCargoBridgeObjects(
                component,confirmedOwner,&confirmedCargo
            );
        }
        bool confirmedSameBaggage=
            (confirmedReady==CARGO_BRIDGE_READY||
             confirmedReady==CARGO_BRIDGE_UNREALIZED)&&
            confirmedCargo.baggage==cargo.baggage;
        bool confirmedInitInFlight=
            confirmedReady==CARGO_BRIDGE_READY&&
            realizedInitInFlight(confirmedCargo.realized);
        if(confirmedReady==CARGO_BRIDGE_READY&&
           confirmedSameBaggage&&!confirmedInitInFlight){
            CargoBridgeResult bridged=bridgeFreshCargoHit(
                component,&confirmedCargo,distanceSquared,
                &foundCalled,&markerCalled
            );
            if(bridged==CARGO_BRIDGE_READY){
                bridges++;
            }else{
                clearFreshHitIfStillDetected(component);
                bridgeFailures++;
                postCoreBridgeFailed++;
                skipped++;
            }
        }else if(confirmedSameBaggage&&
                 (confirmedReady==CARGO_BRIDGE_UNREALIZED||
                  confirmedInitInFlight)){
            // Retire the artificial native one-shot before publishing the
            // delayed record. Otherwise a concurrent Init consumer could set
            // it back to one and this sweep could erase that new ownership.
            clearFreshHitIfStillDetected(component);
            PendingRememberResult pending=rememberPendingCargoHit(
                confirmedCargo.baggage,component,confirmedOwner,serial,
                scanEpoch,refreshKey.tick,g_targetScanRadiusBits,
                distanceSquared,scanOrigin
            );
            // Close the narrow race where the native Realized initializer ran
            // after our UNREALIZED check but just before table publication.
            // The Init hook and this catch-up both atomically claim the same
            // record, so at most one path can bridge it.
            if(pending==PENDING_REMEMBER_RECORDED||
               pending==PENDING_REMEMBER_DEDUPE){
                CargoBridgeObjects lateCargo;
                if(resolveCargoBridgeObjects(
                       component,confirmedOwner,&lateCargo
                   )==CARGO_BRIDGE_READY&&
                   lateCargo.baggage==confirmedCargo.baggage&&
                   !realizedInitInFlight(lateCargo.realized)){
                    consumePendingCargoHitKnown(
                        lateCargo.realized,lateCargo.baggage,false,false
                    );
                }
            }
            deferredUnrealized++;
            skipped++;
            if(pending==PENDING_REMEMBER_REJECTED||
               pending==PENDING_REMEMBER_FULL){
                bridgeFailures++;
            }
        }else{
            clearFreshHitIfStillDetected(component);
            bridgeFailures++;
            postCoreBridgeFailed++;
            skipped++;
        }
        if(foundCalled)foundCalls++;
        if(markerCalled)markerCalls++;
    }

    _InterlockedExchange(&g_farRefreshManagerCount,(long)managerCount);
    _InterlockedExchange(&g_farRefreshSnapshotCount,(long)snapshotCount);
    _InterlockedExchange(&g_farRefreshEligible,(long)eligible);
    _InterlockedExchange(&g_farRefreshCalls,(long)calls);
    _InterlockedExchange(&g_farRefreshSkipped,(long)skipped);
    _InterlockedExchange(&g_farRefreshTruncated,truncated?1:0);
    _InterlockedExchange(&g_farRefreshRadiusBits,(long)radiusBits);
    _InterlockedExchange(&g_farRefreshBridges,(long)bridges);
    _InterlockedExchange(&g_farRefreshBridgeFailures,(long)bridgeFailures);
    _InterlockedExchange(&g_farRefreshDetectedValid,(long)detectedValid);
    _InterlockedExchange(&g_farRefreshDetectedInvalid,(long)detectedInvalid);
    _InterlockedExchange(&g_farRefreshDistanceInvalid,(long)distanceInvalid);
    _InterlockedExchange(&g_farRefreshOutsideRadius,(long)outsideRadius);
    _InterlockedExchange(&g_farRefreshInRadius,(long)inRadius);
    _InterlockedExchange(&g_farRefreshNoBaggage,(long)noBaggage);
    _InterlockedExchange(&g_farRefreshBaggageFound,(long)baggageFound);
    _InterlockedExchange(&g_farRefreshRealizedFound,(long)realizedFound);
    _InterlockedExchange(&g_farRefreshUnrealized,(long)unrealized);
    _InterlockedExchange(
        &g_farRefreshDeferredUnrealized,(long)deferredUnrealized
    );
    _InterlockedExchange(&g_farRefreshInvalidCargo,(long)invalidCargo);
    _InterlockedExchange(&g_farRefreshClaimDuplicate,(long)claimDuplicate);
    _InterlockedExchange(&g_farRefreshClaimFull,(long)claimFull);
    _InterlockedExchange(&g_farRefreshFreshHits,(long)freshHits);
    _InterlockedExchange(&g_farRefreshFreshMisses,(long)freshMisses);
    _InterlockedExchange(&g_farRefreshFoundCalls,(long)foundCalls);
    _InterlockedExchange(&g_farRefreshMarkerCalls,(long)markerCalls);
    _InterlockedExchange(
        &g_farRefreshPostCoreBridgeFailed,(long)postCoreBridgeFailed
    );
    _InterlockedExchange(
        &g_farRefreshMaxDetectedDistanceSquaredBits,
        (long)floatBits(maxDetectedDistanceSquared)
    );
    _InterlockedExchange(
        &g_farRefreshMaxCargoDistanceSquaredBits,
        (long)floatBits(maxCargoDistanceSquared)
    );
    _InterlockedExchange(&g_farRefreshCompletedSerial,(long)serial);
    releaseDetectedTickGuard(message);
    _InterlockedExchange(&g_farRefreshBusy,0);
}

typedef void (*EntityManagerGameUpdate)(void*,void*);

[[maybe_unused]] static bool buildPostJoinConstantUpdateMessage(void* context,u8* message){
    if(!context||!message||!rangeReadable(context,0x64u)){
        _InterlockedIncrement(&g_farRefreshContextInvalid);
        return false;
    }
    u8 contextFlag=((u8*)context)[9u];
    _InterlockedExchange(&g_farRefreshLastContextFlag,(long)contextFlag);
    if(contextFlag==0u){
        _InterlockedIncrement(&g_farRefreshContextInactive);
        return false;
    }
    u64 tickValue=0;
    u32 deltaBits=0;
    float delta=0.0f;
    if(!readU64((u8*)context+0x48u,&tickValue)){
        _InterlockedIncrement(&g_farRefreshContextInvalid);
        return false;
    }
    _InterlockedExchange(&g_farRefreshLastTick,(long)(u32)tickValue);
    if(readU32((u8*)context+0x60u,&deltaBits)){
        _InterlockedExchange(&g_farRefreshLastDeltaBits,(long)deltaBits);
    }else{
        _InterlockedIncrement(&g_farRefreshContextDeltaInvalid);
        return false;
    }
    if(!finiteFloat(deltaBits,&delta)||delta<0.0f||delta>1.0f){
        _InterlockedIncrement(&g_farRefreshContextDeltaInvalid);
        return false;
    }
    memset(message,0,0x20u);
    *(u32*)(message+0x18u)=(u32)tickValue;
    *(u32*)(message+0x1Cu)=deltaBits;

    _InterlockedIncrement(&g_farRefreshContextReady);
    return true;
}

extern "C" void entityManagerGameUpdateHook(void* self,void* context){
    EntityManagerGameUpdate original=(EntityManagerGameUpdate)(u64)
        g_originalEntityManagerGameUpdate;
    if(!original)return;
    original(self,context);
    // v0.1.10 deliberately performs no manager snapshot or artificial
    // component fan-out here. The old 4096-entry post-join sweep caused the
    // observed multi-second hitch and could not run any cargo already updated
    // by the natural dispatcher. This wrapper remains pass-through so an
    // already-installed v0.1.8 slot can be upgraded without an inline patch.
}

#include "scanned_cargo_hud.inl"
#include "scanned_cargo_distance.inl"
#include "scanned_cargo_sensor.inl"
#include "survey_alignment.inl"
#include "cargo_meter_label.inl"
#include "cargo_recipient_meter.inl"
#include "visual_wave.inl"

typedef void (*GameplayScanStart)(void*,u64,const void*,const void*);

extern "C" void gameplayScanStartHook(void* self,u64 argument,const void* scanId,const void* origin){
    GameplayScanStart original=(GameplayScanStart)(u64)g_originalGameplayScan;
    if(!original)return;
    long attemptValue=_InterlockedIncrement(&g_scanAttemptEpoch);
    if(attemptValue==0)attemptValue=_InterlockedIncrement(&g_scanAttemptEpoch);
    u32 attemptEpoch=(u32)attemptValue;
    // A scan attempt, successful or not, invalidates delayed hits from every
    // earlier attempt before the native start can alter runtime state.
    purgePendingCargo(false);
    purgeRealizeLeases(true);
    if(_InterlockedCompareExchange(&g_gateTelemetryReady,0,0)==1){
        // Preserve an unfinished measurement when another scan supersedes it.
        if(_InterlockedExchange(&g_gateCaptureActive,0)==1)logGateTelemetry();
        resetGateTelemetry();
    }
    invalidateScannedHudOrigin();
    _InterlockedExchange64(&g_visualWaveInstance,0);
    original(self,argument,scanId,origin);
    // A new start attempt supersedes the preceding result window. v0.1.10 has
    // no post-join manager sweep, so there is no busy fan-out to wait for.
    bool canArmRefresh=true;
    AcquireSRWLockExclusive(&g_farRefreshStateLock);
    clearAnyScanPresentation();
    _InterlockedExchange(&g_farRefreshPending,0);
    _InterlockedExchange(&g_farRefreshPendingSerial,0);
    ReleaseSRWLockExclusive(&g_farRefreshStateLock);
    if(_InterlockedCompareExchange(&g_hooksReady,0,0)!=1)return;

    _InterlockedExchange(&g_gameplayStatus,1);
    GameplayObjects objects;
    if(!captureGameplayObjects(self,&objects)){
        _InterlockedExchange(&g_gameplayStatus,2);
        return;
    }

    u32 beforeDiameter=0,afterDiameter=0,originalOverride=0;
    bool overrideChanged=false;
    bool rangeReady=ensureInstanceDiameter(
        &objects,g_targetEffectDiameterBits,&beforeDiameter,&afterDiameter,
        &originalOverride,&overrideChanged
    );
    u32 beforeGateRadius=0,afterGateRadius=0;
    readU32(objects.manager+OFF_MANAGER_GATE_RADIUS,&beforeGateRadius);
    afterGateRadius=beforeGateRadius;
    bool rangeRollbackFailed=false;
    if(!rangeReady&&overrideChanged){
        u32 observed=0;
        if(!compareExchangeU32(
            objects.instance+OFF_EFFECT_SIZE_OVERRIDE,originalOverride,
            g_targetEffectDiameterBits,&observed
        )||(observed!=g_targetEffectDiameterBits&&observed!=originalOverride)){
            rangeRollbackFailed=true;
        }else{
            afterDiameter=beforeDiameter;
        }
    }
    _InterlockedExchange(&g_gameplayBeforeDiameterBits,(long)beforeDiameter);
    _InterlockedExchange(&g_gameplayAfterDiameterBits,(long)afterDiameter);
    _InterlockedExchange(&g_gameplayBeforeGateRadiusBits,(long)beforeGateRadius);
    _InterlockedExchange(&g_gameplayAfterGateRadiusBits,(long)afterGateRadius);

    bool angleReady=true;
    bool angleRollbackFailed=false;
    u32 beforeInstanceAngle=0,beforeManagerAngle=0;
    if(g_gameplayFullCircle&&!rangeRollbackFailed){
        angleReady=raiseAnglePair(
            objects.instance+OFF_EFFECT_FULL_ANGLE,
            objects.manager+OFF_MANAGER_FULL_ANGLE,
            FULL_CIRCLE_RADIANS_BITS,&beforeInstanceAngle,&beforeManagerAngle,
            &angleRollbackFailed
        );
    }else if(rangeRollbackFailed){
        angleReady=false;
    }
    _InterlockedExchange(&g_gameplayBeforeInstanceAngleBits,(long)beforeInstanceAngle);
    _InterlockedExchange(&g_gameplayBeforeManagerAngleBits,(long)beforeManagerAngle);
    _InterlockedExchange(
        &g_gameplayStatus,
        rangeRollbackFailed?5:(angleRollbackFailed?6:(rangeReady&&angleReady?7:(rangeReady?4:3)))
    );
    if(rangeReady&&angleReady){
        _InterlockedExchange64(&g_visualWaveInstance,(long long)objects.instance);
        publishScannedHudOrigin(objects.manager);
    }
    if(rangeReady&&angleReady&&
       canArmRefresh&&_InterlockedCompareExchange(&g_farRefreshReady,0,0)==1){
        resetFarRefreshDiagnostics();
        AcquireSRWLockExclusive(&g_farRefreshStateLock);
        if(_InterlockedCompareExchange(&g_hooksReady,0,0)==1&&
           _InterlockedCompareExchange(&g_farRefreshReady,0,0)==1){
            long scanSerial=_InterlockedIncrement(&g_farRefreshScanSerial);
            _InterlockedExchange(&g_farRefreshScanEpoch,(long)attemptEpoch);
            publishScanPresentation((u32)scanSerial,true);
        }
        ReleaseSRWLockExclusive(&g_farRefreshStateLock);
    }
}

typedef u8* (*ResourceFactory)(void*,u8*);

extern "C" u8* surveyFactoryHook(void* context,u8* object){
    if(!g_gameBase)return 0;
    ResourceFactory original=(ResourceFactory)(g_gameBase+RVA_SURVEY_FACTORY);
    u8* result=original(context,object);
    if(object)_InterlockedExchange64(&g_surveyResource,(long long)object);
    return result;
}

static bool validateStaticVtable(const GameImage* image,u64 rva,const u64* functions){
    if(!image||!functions||rva+32u>(u64)image->imageSize)return false;
    for(u32 i=0;i<4u;i++){
        u64 entry=0;
        if(!readU64(image->base+rva+(u64)i*8u,&entry)||
           entry!=(u64)(image->base+functions[i])||!pointerInImage(image,entry))return false;
    }
    return true;
}

static bool validateStaticEntityManagerGameRtti(const GameImage* image){
    if(!image||RVA_ENTITY_MANAGER_GAME_VTABLE<8u||
       RVA_ENTITY_MANAGER_GAME_COL+0x10u>(u64)image->imageSize||
       RVA_ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR+0x10u+
               sizeof(EXPECTED_ENTITY_MANAGER_GAME_TYPE_NAME)>
           (u64)image->imageSize)return false;
    u64 completeObjectLocator=0;
    u32 typeDescriptorRva=0;
    return readU64(
               image->base+RVA_ENTITY_MANAGER_GAME_VTABLE-8u,
               &completeObjectLocator
           )&&completeObjectLocator==(u64)(image->base+RVA_ENTITY_MANAGER_GAME_COL)&&
           readU32(
               image->base+RVA_ENTITY_MANAGER_GAME_COL+0x0Cu,
               &typeDescriptorRva
           )&&typeDescriptorRva==(u32)RVA_ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR&&
           bytesEqual(
               image->base+RVA_ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR+0x10u,
               (const u8*)EXPECTED_ENTITY_MANAGER_GAME_TYPE_NAME,
               (u32)sizeof(EXPECTED_ENTITY_MANAGER_GAME_TYPE_NAME)
           );
}

[[maybe_unused]] static bool validateStaticBaggageSymbolsDescriptor(
    const GameImage* image
){
    if(!image||RVA_BAGGAGE_SYMBOLS_RTTI+
           sizeof(EXPECTED_BAGGAGE_SYMBOLS_RTTI)+0x10u>
           (u64)image->imageSize)return false;
    u64 constructor=0u,destructor=0u;
    return bytesEqual(
               image->base+RVA_BAGGAGE_SYMBOLS_RTTI,
               EXPECTED_BAGGAGE_SYMBOLS_RTTI,
               (u32)sizeof(EXPECTED_BAGGAGE_SYMBOLS_RTTI)
           )&&readU64(
               image->base+RVA_BAGGAGE_SYMBOLS_RTTI+0x18u,&constructor
           )&&constructor==(u64)(image->base+0x011FB830ull)&&
           readU64(
               image->base+RVA_BAGGAGE_SYMBOLS_RTTI+0x20u,&destructor
           )&&destructor==(u64)(image->base+0x011FB890ull);
}

static bool validateStaticTargets(const GameImage* image,bool requireCargoHud){
    u64 factoryPointer=0;
    if(!image||RVA_SURVEY_FACTORY+16u>(u64)image->imageSize||
       RVA_SURVEY_CONSTRUCTOR+sizeof(EXPECTED_SURVEY_CONSTRUCTOR)>(u64)image->imageSize||
       RVA_GAMEPLAY_SCAN_START+sizeof(EXPECTED_GAMEPLAY_SCAN_START)>(u64)image->imageSize||
       RVA_SENSOR_GATE_RADIUS_LOAD+sizeof(EXPECTED_SENSOR_GATE_RADIUS_LOAD)>
           (u64)image->imageSize||
       RVA_SENSOR_GATE_CALL_WINDOW+sizeof(EXPECTED_SENSOR_GATE_CALL_WINDOW)>
           (u64)image->imageSize||
       RVA_UI_CACHE_LOAD+sizeof(EXPECTED_UI_CACHE_LOAD)>(u64)image->imageSize||
       RVA_UI_SEARCHED_DISTANCE_LOAD+sizeof(EXPECTED_UI_SEARCHED_DISTANCE_LOAD)>
           (u64)image->imageSize||
       RVA_SENSOR_MANAGER_GLOBAL+8u>(u64)image->imageSize||
       RVA_UI_CACHE_ROOT+8u>(u64)image->imageSize)return false;
    if(!bytesEqual(image->base+RVA_SURVEY_FACTORY,EXPECTED_SURVEY_FACTORY,16u)||
       !bytesEqual(
           image->base+RVA_SURVEY_CONSTRUCTOR,EXPECTED_SURVEY_CONSTRUCTOR,
           (u32)sizeof(EXPECTED_SURVEY_CONSTRUCTOR)
       )||!bytesEqual(
           image->base+RVA_GAMEPLAY_SCAN_START,EXPECTED_GAMEPLAY_SCAN_START,
           (u32)sizeof(EXPECTED_GAMEPLAY_SCAN_START)
       )||!bytesEqual(
           image->base+RVA_SENSOR_GATE_RADIUS_LOAD,EXPECTED_SENSOR_GATE_RADIUS_LOAD,
           (u32)sizeof(EXPECTED_SENSOR_GATE_RADIUS_LOAD)
       )||!bytesEqual(
           image->base+RVA_SENSOR_GATE_CALL_WINDOW,EXPECTED_SENSOR_GATE_CALL_WINDOW,
           (u32)sizeof(EXPECTED_SENSOR_GATE_CALL_WINDOW)
       )||!bytesEqual(
           image->base+RVA_UI_CACHE_LOAD,EXPECTED_UI_CACHE_LOAD,
           (u32)sizeof(EXPECTED_UI_CACHE_LOAD)
       )||!bytesEqual(
           image->base+RVA_UI_SEARCHED_DISTANCE_LOAD,EXPECTED_UI_SEARCHED_DISTANCE_LOAD,
           (u32)sizeof(EXPECTED_UI_SEARCHED_DISTANCE_LOAD)
       )||!validateStaticVtable(image,RVA_SURVEY_VTABLE,SURVEY_VFUNCS)||
       !validateStaticVtable(image,RVA_BODY_SECONDARY_VTABLE,BODY_SECONDARY_VFUNCS)||
       !validateStaticVtable(image,RVA_SENSOR_VTABLE,SENSOR_VFUNCS)||
       !validateStaticVtable(image,RVA_SENSOR_MANAGER_VTABLE,SENSOR_MANAGER_VFUNCS)||
       !validateStaticVtable(image,RVA_EFFECT_INSTANCE_VTABLE,EFFECT_INSTANCE_VFUNCS)||
       !validateStaticVtable(image,RVA_EFFECT_RESOURCE_VTABLE,EFFECT_RESOURCE_VFUNCS)||
       !validateStaticVtable(image,RVA_UI_RESIDENT_VTABLE,UI_RESIDENT_VFUNCS)||
       !validateStaticVtable(image,RVA_UI_GAME_PARAMS_VTABLE,UI_GAME_PARAMS_VFUNCS)||
       !readU64(image->base+RVA_SURVEY_FACTORY_SLOT,&factoryPointer)||
       factoryPointer!=(u64)(image->base+RVA_SURVEY_FACTORY))return false;
    if(!readU64(image->base+RVA_GAMEPLAY_SCAN_SLOT,&factoryPointer)||
       factoryPointer!=(u64)(image->base+RVA_GAMEPLAY_SCAN_START))return false;
    if(!requireCargoHud)return true;
    u64 detectedMessage=0,detectedHandler=0,entityManagerUpdate=0;
    u64 realizedInitMessage=0,realizedInitHandler=0;
    u64 realizedUnlinkMessage=0,realizedUnlinkHandler=0;
    u64 realizedUpdateMessage=0,realizedUpdateHandler=0;
    if(RVA_DETECTED_CONSTANT_UPDATE_MESSAGE_SLOT+8u>(u64)image->imageSize||
       RVA_DETECTED_CONSTANT_UPDATE_HANDLER_SLOT+8u>(u64)image->imageSize||
       RVA_REALIZED_BAGGAGE_INIT_MESSAGE_SLOT+8u>(u64)image->imageSize||
       RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT+8u>(u64)image->imageSize||
       RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT+8u>(u64)image->imageSize||
       RVA_REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT+8u>(u64)image->imageSize||
       RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT+8u>(u64)image->imageSize||
       RVA_REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT+8u>(u64)image->imageSize||
       RVA_ENTITY_MANAGER_GAME_UPDATE_SLOT+8u>(u64)image->imageSize||
       RVA_ENTITY_REQUEST_REALIZE+sizeof(EXPECTED_ENTITY_REQUEST_REALIZE)>
           (u64)image->imageSize||
       RVA_REALIZATION_COORDINATOR_CALL_WINDOW+
               sizeof(EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW)>
           (u64)image->imageSize||
       RVA_REALIZATION_COORDINATOR+1u>(u64)image->imageSize||
       !readU64(
           image->base+RVA_DETECTED_CONSTANT_UPDATE_MESSAGE_SLOT,&detectedMessage
       )||detectedMessage!=(u64)(image->base+RVA_DETECTED_CONSTANT_UPDATE_MESSAGE)||
       !readU64(
           image->base+RVA_DETECTED_CONSTANT_UPDATE_HANDLER_SLOT,&detectedHandler
       )||detectedHandler!=(u64)(image->base+RVA_DETECTED_CONSTANT_UPDATE_THUNK)||
       !readU64(
           image->base+RVA_REALIZED_BAGGAGE_INIT_MESSAGE_SLOT,
           &realizedInitMessage
       )||realizedInitMessage!=(u64)(image->base+RVA_REALIZED_BAGGAGE_INIT_MESSAGE)||
       !readU64(
           image->base+RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT,
           &realizedInitHandler
       )||realizedInitHandler!=(u64)(image->base+RVA_REALIZED_BAGGAGE_INIT_THUNK)||
       !readU64(
           image->base+RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT,
           &realizedUnlinkMessage
       )||realizedUnlinkMessage!=(u64)(image->base+RVA_REALIZED_BAGGAGE_UNLINK_MESSAGE)||
       !readU64(
           image->base+RVA_REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT,
           &realizedUnlinkHandler
       )||realizedUnlinkHandler!=(u64)(image->base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK)||
       !readU64(
           image->base+RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT,
           &realizedUpdateMessage
       )||realizedUpdateMessage!=(u64)(image->base+RVA_REALIZED_BAGGAGE_UPDATE_MESSAGE)||
       !readU64(
           image->base+RVA_REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT,
           &realizedUpdateHandler
       )||realizedUpdateHandler!=(u64)(image->base+RVA_REALIZED_BAGGAGE_UPDATE_THUNK)||
       !readU64(
           image->base+RVA_ENTITY_MANAGER_GAME_UPDATE_SLOT,&entityManagerUpdate
       )||entityManagerUpdate!=(u64)(image->base+RVA_ENTITY_MANAGER_GAME_UPDATE))return false;
    return RVA_BAGGAGE_CONFIG_LOAD+sizeof(EXPECTED_BAGGAGE_CONFIG_LOAD)<=
               (u64)image->imageSize&&
           RVA_BAGGAGE_CONFIG_CACHE_STORES+sizeof(EXPECTED_BAGGAGE_CONFIG_CACHE_STORES)<=
               (u64)image->imageSize&&
           RVA_BAGGAGE_MARKER_UNTOUCHED_LOAD+
                   sizeof(EXPECTED_BAGGAGE_MARKER_UNTOUCHED_LOAD)<=
               (u64)image->imageSize&&
           RVA_BAGGAGE_MARKER_TOUCHED_LOAD+sizeof(EXPECTED_BAGGAGE_MARKER_TOUCHED_LOAD)<=
               (u64)image->imageSize&&
           RVA_BAGGAGE_MARKER_MISSION_LOAD+sizeof(EXPECTED_BAGGAGE_MARKER_MISSION_LOAD)<=
               (u64)image->imageSize&&
           RVA_BAGGAGE_FOUND_DISTANCE_LOAD+sizeof(EXPECTED_BAGGAGE_FOUND_DISTANCE_LOAD)<=
               (u64)image->imageSize&&
           RVA_BAGGAGE_MANAGER_GLOBAL+8u<=(u64)image->imageSize&&
           RVA_PLAYER_RESOURCE_ROOT+8u<=(u64)image->imageSize&&
           RVA_PLAYER_RESOURCE_ROOT_LOAD+
                   sizeof(EXPECTED_PLAYER_RESOURCE_ROOT_LOAD)<=
               (u64)image->imageSize&&
           RVA_PLAYER_RESOURCE_ENUMERATION+
                   sizeof(EXPECTED_PLAYER_RESOURCE_ENUMERATION)<=
               (u64)image->imageSize&&
           RVA_PLAYER_ODRADEK_CONTEXT_PATH+
                   sizeof(EXPECTED_PLAYER_ODRADEK_CONTEXT_PATH)<=
               (u64)image->imageSize&&
           RVA_PLAYER_ODRADEK_CONFIG_GETTER+
                   sizeof(EXPECTED_PLAYER_ODRADEK_CONFIG_GETTER)<=
               (u64)image->imageSize&&
           RVA_DETECTED_CONSTANT_UPDATE_THUNK+
                   sizeof(EXPECTED_DETECTED_CONSTANT_UPDATE_THUNK)<=
               (u64)image->imageSize&&
           RVA_DETECTED_CONSTANT_UPDATE_CORE+
                   sizeof(EXPECTED_DETECTED_CONSTANT_UPDATE_CORE)<=
               (u64)image->imageSize&&
           RVA_FIND_ENTITY_COMPONENT+sizeof(EXPECTED_FIND_ENTITY_COMPONENT)<=
               (u64)image->imageSize&&
           RVA_REALIZED_FOUND_BRIDGE+sizeof(EXPECTED_REALIZED_FOUND_BRIDGE)<=
               (u64)image->imageSize&&
            RVA_REALIZED_MARKER_BRIDGE+sizeof(EXPECTED_REALIZED_MARKER_BRIDGE)<=
                (u64)image->imageSize&&
            RVA_REALIZED_BAGGAGE_INIT_THUNK+
                    sizeof(EXPECTED_REALIZED_BAGGAGE_INIT_THUNK)<=
                (u64)image->imageSize&&
            RVA_REALIZED_BAGGAGE_INIT_CORE+
                    sizeof(EXPECTED_REALIZED_BAGGAGE_INIT_CORE)<=
                (u64)image->imageSize&&
            RVA_REALIZED_BAGGAGE_UNLINK_THUNK+
                    sizeof(EXPECTED_REALIZED_BAGGAGE_UNLINK_THUNK)<=
                (u64)image->imageSize&&
            RVA_REALIZED_BAGGAGE_UNLINK_CORE+
                    sizeof(EXPECTED_REALIZED_BAGGAGE_UNLINK_CORE)<=
                (u64)image->imageSize&&
            RVA_REALIZED_BAGGAGE_UPDATE_THUNK+
                    sizeof(EXPECTED_REALIZED_BAGGAGE_UPDATE_THUNK)<=
                (u64)image->imageSize&&
            RVA_REALIZED_BAGGAGE_UPDATE_CORE+
                    sizeof(EXPECTED_REALIZED_BAGGAGE_UPDATE_CORE)<=
                (u64)image->imageSize&&
            RVA_ENTITY_MANAGER_GAME_UPDATE+
                   sizeof(EXPECTED_ENTITY_MANAGER_GAME_UPDATE)<=
               (u64)image->imageSize&&
           RVA_ENTITY_REQUEST_REALIZE+
                   sizeof(EXPECTED_ENTITY_REQUEST_REALIZE)<=
               (u64)image->imageSize&&
           RVA_REALIZATION_COORDINATOR_CALL_WINDOW+
                   sizeof(EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW)<=
               (u64)image->imageSize&&
           bytesEqual(
               image->base+RVA_BAGGAGE_CONFIG_LOAD,EXPECTED_BAGGAGE_CONFIG_LOAD,
               (u32)sizeof(EXPECTED_BAGGAGE_CONFIG_LOAD)
           )&&bytesEqual(
               image->base+RVA_BAGGAGE_CONFIG_CACHE_STORES,
               EXPECTED_BAGGAGE_CONFIG_CACHE_STORES,
               (u32)sizeof(EXPECTED_BAGGAGE_CONFIG_CACHE_STORES)
           )&&bytesEqual(
               image->base+RVA_BAGGAGE_MARKER_UNTOUCHED_LOAD,
               EXPECTED_BAGGAGE_MARKER_UNTOUCHED_LOAD,
               (u32)sizeof(EXPECTED_BAGGAGE_MARKER_UNTOUCHED_LOAD)
           )&&bytesEqual(
               image->base+RVA_BAGGAGE_MARKER_TOUCHED_LOAD,
               EXPECTED_BAGGAGE_MARKER_TOUCHED_LOAD,
               (u32)sizeof(EXPECTED_BAGGAGE_MARKER_TOUCHED_LOAD)
           )&&bytesEqual(
               image->base+RVA_BAGGAGE_MARKER_MISSION_LOAD,
               EXPECTED_BAGGAGE_MARKER_MISSION_LOAD,
               (u32)sizeof(EXPECTED_BAGGAGE_MARKER_MISSION_LOAD)
           )&&bytesEqual(
               image->base+RVA_BAGGAGE_FOUND_DISTANCE_LOAD,
               EXPECTED_BAGGAGE_FOUND_DISTANCE_LOAD,
               (u32)sizeof(EXPECTED_BAGGAGE_FOUND_DISTANCE_LOAD)
           )&&bytesEqual(
               image->base+RVA_PLAYER_RESOURCE_ROOT_LOAD,
               EXPECTED_PLAYER_RESOURCE_ROOT_LOAD,
               (u32)sizeof(EXPECTED_PLAYER_RESOURCE_ROOT_LOAD)
           )&&bytesEqual(
               image->base+RVA_PLAYER_RESOURCE_ENUMERATION,
               EXPECTED_PLAYER_RESOURCE_ENUMERATION,
               (u32)sizeof(EXPECTED_PLAYER_RESOURCE_ENUMERATION)
           )&&bytesEqual(
               image->base+RVA_PLAYER_ODRADEK_CONTEXT_PATH,
               EXPECTED_PLAYER_ODRADEK_CONTEXT_PATH,
               (u32)sizeof(EXPECTED_PLAYER_ODRADEK_CONTEXT_PATH)
           )&&bytesEqual(
               image->base+RVA_PLAYER_ODRADEK_CONFIG_GETTER,
               EXPECTED_PLAYER_ODRADEK_CONFIG_GETTER,
               (u32)sizeof(EXPECTED_PLAYER_ODRADEK_CONFIG_GETTER)
           )&&bytesEqual(
               image->base+RVA_DETECTED_CONSTANT_UPDATE_THUNK,
               EXPECTED_DETECTED_CONSTANT_UPDATE_THUNK,
               (u32)sizeof(EXPECTED_DETECTED_CONSTANT_UPDATE_THUNK)
           )&&bytesEqual(
               image->base+RVA_DETECTED_CONSTANT_UPDATE_CORE,
               EXPECTED_DETECTED_CONSTANT_UPDATE_CORE,
               (u32)sizeof(EXPECTED_DETECTED_CONSTANT_UPDATE_CORE)
           )&&bytesEqual(
               image->base+RVA_FIND_ENTITY_COMPONENT,
               EXPECTED_FIND_ENTITY_COMPONENT,
               (u32)sizeof(EXPECTED_FIND_ENTITY_COMPONENT)
           )&&bytesEqual(
               image->base+RVA_REALIZED_FOUND_BRIDGE,
               EXPECTED_REALIZED_FOUND_BRIDGE,
               (u32)sizeof(EXPECTED_REALIZED_FOUND_BRIDGE)
            )&&bytesEqual(
                image->base+RVA_REALIZED_MARKER_BRIDGE,
                EXPECTED_REALIZED_MARKER_BRIDGE,
                (u32)sizeof(EXPECTED_REALIZED_MARKER_BRIDGE)
            )&&bytesEqual(
                image->base+RVA_REALIZED_BAGGAGE_INIT_THUNK,
                EXPECTED_REALIZED_BAGGAGE_INIT_THUNK,
                (u32)sizeof(EXPECTED_REALIZED_BAGGAGE_INIT_THUNK)
            )&&bytesEqual(
                image->base+RVA_REALIZED_BAGGAGE_INIT_CORE,
                EXPECTED_REALIZED_BAGGAGE_INIT_CORE,
                (u32)sizeof(EXPECTED_REALIZED_BAGGAGE_INIT_CORE)
            )&&bytesEqual(
                image->base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK,
                EXPECTED_REALIZED_BAGGAGE_UNLINK_THUNK,
                (u32)sizeof(EXPECTED_REALIZED_BAGGAGE_UNLINK_THUNK)
            )&&bytesEqual(
                image->base+RVA_REALIZED_BAGGAGE_UNLINK_CORE,
                EXPECTED_REALIZED_BAGGAGE_UNLINK_CORE,
                (u32)sizeof(EXPECTED_REALIZED_BAGGAGE_UNLINK_CORE)
            )&&bytesEqual(
                image->base+RVA_REALIZED_BAGGAGE_UPDATE_THUNK,
                EXPECTED_REALIZED_BAGGAGE_UPDATE_THUNK,
                (u32)sizeof(EXPECTED_REALIZED_BAGGAGE_UPDATE_THUNK)
            )&&bytesEqual(
                image->base+RVA_REALIZED_BAGGAGE_UPDATE_CORE,
                EXPECTED_REALIZED_BAGGAGE_UPDATE_CORE,
                (u32)sizeof(EXPECTED_REALIZED_BAGGAGE_UPDATE_CORE)
            )&&bytesEqual(
               image->base+RVA_ENTITY_MANAGER_GAME_UPDATE,
               EXPECTED_ENTITY_MANAGER_GAME_UPDATE,
               (u32)sizeof(EXPECTED_ENTITY_MANAGER_GAME_UPDATE)
           )&&bytesEqual(
               image->base+RVA_ENTITY_REQUEST_REALIZE,
               EXPECTED_ENTITY_REQUEST_REALIZE,
               (u32)sizeof(EXPECTED_ENTITY_REQUEST_REALIZE)
           )&&bytesEqual(
               image->base+RVA_REALIZATION_COORDINATOR_CALL_WINDOW,
               EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW,
               (u32)sizeof(EXPECTED_REALIZATION_COORDINATOR_CALL_WINDOW)
           )&&validateStaticVtable(
               image,RVA_BAGGAGE_MANAGER_VTABLE,BAGGAGE_MANAGER_VFUNCS
           )&&validateStaticVtable(
               image,RVA_BAGGAGE_MANAGER_SECONDARY_VTABLE,
               BAGGAGE_MANAGER_SECONDARY_VFUNCS
           )&&validateStaticVtable(
               image,RVA_BAGGAGE_CONFIG_VTABLE,BAGGAGE_CONFIG_VFUNCS
           )&&validateStaticVtable(
               image,RVA_PLAYER_ODRADEK_CONFIG_VTABLE,
               PLAYER_ODRADEK_CONFIG_VFUNCS
           )&&validateStaticVtable(
               image,RVA_DETECTED_COMPONENT_VTABLE,
               DETECTED_COMPONENT_VFUNCS
           )&&validateStaticVtable(
               image,RVA_BAGGAGE_COMPONENT_VTABLE,
               BAGGAGE_COMPONENT_VFUNCS
           )&&validateStaticVtable(
               image,RVA_REALIZED_BAGGAGE_VTABLE,
               REALIZED_BAGGAGE_VFUNCS
           )&&validateStaticVtable(
               image,RVA_ENTITY_MANAGER_GAME_VTABLE,
               ENTITY_MANAGER_GAME_VFUNCS
           )&&validateStaticEntityManagerGameRtti(image);
}

static void logHookFailure(const char* name,ApplyResult result){
    if(result==APPLY_CONFLICT){
        if(name[0]=='g'){
            const char message[]="ERROR: gameplay sensor scan pointer is already modified; no Odradek patch was applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
        }else{
            const char message[]="ERROR: survey RTTI factory pointer is already modified; no Odradek patch was applied.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
        }
    }else if(result==APPLY_CRITICAL){
        const char message[]="CRITICAL: hook rollback or memory-protection restore was incomplete; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
    }else{
        const char message[]="ERROR: Odradek hook installation failed; no resource values were changed.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
    }
}

static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;

    Settings settings;
    SettingsResult settingsResult=loadSettings(&settings);
    if(settingsResult!=SETTINGS_READY){
        closeLog();
        return 0;
    }
    if(g_debugLog)openLog();
    bool scannedHudRequested=disableRejectedCargoLifecycle(&settings);
    {
        const char message[]="CARGO: rejected lifecycle path disabled; no forced realization, lease retention, fresh-hit clearing or replay.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1u));
    }

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
    if(!validateStaticTargets(&image,settings.extendCargoMarkers)||!validateSurveyAlignment(&image)||!validateVisualWave(&image)||
       (scannedHudRequested&&(!validateScannedHudTargets(&image)||!validateHudDistanceTargets(&image)||
                             !validateCargoSensorTargets(&image)||!validateCargoMeter(&image)||!validateRecipientMeter(&image)))){
        const char message[]="ERROR: Odradek constructors, factory signatures, or VTables do not match; no patch applied.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
        closeLog();
        return 0;
    }

    g_gameBase=image.base;
    g_targetEffectDiameterBits=settings.rangeBits;
    g_targetScanRadiusBits=settings.uiRangeBits;
    g_gameplayFullCircle=settings.fullCircle;
    if(g_visualWaveScale<1.0f){
        ApplyResult visualResult=installVisualWave(image.base);
        if(visualResult!=APPLY_READY){logHookFailure("visual-wave",visualResult);closeLog();return 0;}
    }
    ApplyResult alignmentResult=installSurveyAlignment(image.base);
    if(alignmentResult!=APPLY_READY){
        logHookFailure("survey-alignment",alignmentResult);closeLog();return 0;
    }
    _InterlockedExchange64(
        &g_originalGameplayScan,(long long)(u64)(image.base+RVA_GAMEPLAY_SCAN_START)
    );
    if(scannedHudRequested){
        g_scannedHudSort=(ScannedHudSort)(image.base+RVA_SCANNED_HUD_SORT);
        g_scannedHudFind=(FindEntityComponent)(image.base+RVA_FIND_ENTITY_COMPONENT);
        g_scannedHudRecent=(ScannedHudRecent)(image.base+RVA_SCANNED_HUD_RECENT);
        u8* hudRelay=0;
        ApplyResult hudResult=installScannedHud(image.base,image.imageSize,&hudRelay);
        if(hudResult!=APPLY_READY){
            logHookFailure("scanned-cargo-hud",hudResult);
            closeLog();return 0;
        }
        ApplyResult distanceResult=installHudDistanceCalls(image.base);
        if(distanceResult!=APPLY_READY){
            logHookFailure("scanned-cargo-distance",distanceResult);
            closeLog();return 0;
        }
        ApplyResult recipientResult=installRecipientMeter(image.base);
        if(recipientResult!=APPLY_READY){logHookFailure("recipient-meter",recipientResult);closeLog();return 0;}
        ApplyResult meterResult=installCargoMeter(image.base);
        if(meterResult!=APPLY_READY){
            logHookFailure("cargo-meter-label",meterResult);closeLog();return 0;
        }
        _InterlockedExchange(&g_scannedHudReady,1);
        const char message[]="SCANNED-HUD: candidate and scoped HUD distance hooks installed; native expiry and sorting retained.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1u));
    }
    u64 surveyHookPointer=(u64)(const void*)&surveyFactoryHook;
    u64 gameplayHookPointer=(u64)(const void*)&gameplayScanStartHook;
    DWORD surveyProtection=0,gameplayProtection=0;
    bool surveyHookChanged=false,gameplayHookChanged=false;
    logHookState("HOOK-BEFORE:","survey",image.base+RVA_SURVEY_FACTORY_SLOT);
    ApplyResult surveyHook=installPointerHook(
        image.base+RVA_SURVEY_FACTORY_SLOT,(u64)(image.base+RVA_SURVEY_FACTORY),surveyHookPointer,
        &surveyProtection,&surveyHookChanged
    );
    if(surveyHook!=APPLY_READY){
        logHookState("HOOK-FAILED:","survey",image.base+RVA_SURVEY_FACTORY_SLOT);
        logHookFailure("survey",surveyHook);
        closeLog();
        return 0;
    }
    logHookState("HOOK-BEFORE:","gameplay",image.base+RVA_GAMEPLAY_SCAN_SLOT);
    ApplyResult gameplayHook=installPointerHook(
        image.base+RVA_GAMEPLAY_SCAN_SLOT,(u64)(image.base+RVA_GAMEPLAY_SCAN_START),
        gameplayHookPointer,&gameplayProtection,&gameplayHookChanged
    );
    if(gameplayHook!=APPLY_READY){
        logHookState("HOOK-FAILED:","gameplay",image.base+RVA_GAMEPLAY_SCAN_SLOT);
        if(surveyHookChanged){
            ApplyResult rolledBack=rollbackPointerHook(
                image.base+RVA_SURVEY_FACTORY_SLOT,(u64)(image.base+RVA_SURVEY_FACTORY),
                surveyHookPointer,surveyProtection
            );
            if(rolledBack==APPLY_CRITICAL){
                const char message[]="CRITICAL: survey-hook rollback was incomplete after gameplay-hook failure; stop DS2.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
            }
        }
        logHookFailure("gameplay",gameplayHook);
        closeLog();
        return 0;
    }
    u8* gateRelay=0;
    _InterlockedExchange64(
        &g_originalSensorGate,(long long)(u64)(image.base+RVA_SENSOR_GATE)
    );
    _InterlockedExchange64(
        &g_originalDetectedConstantUpdate,
        (long long)(u64)(image.base+RVA_DETECTED_CONSTANT_UPDATE_CORE)
    );
    _InterlockedExchange64(
        &g_originalEntityManagerGameUpdate,
        (long long)(u64)(image.base+RVA_ENTITY_MANAGER_GAME_UPDATE)
    );
    _InterlockedExchange64(
        &g_originalRealizedBaggageInit,
        (long long)(u64)(image.base+RVA_REALIZED_BAGGAGE_INIT_CORE)
    );
    _InterlockedExchange64(
        &g_originalRealizedBaggageUnlink,
        (long long)(u64)(image.base+RVA_REALIZED_BAGGAGE_UNLINK_CORE)
    );
    _InterlockedExchange64(
        &g_originalRealizedBaggageUpdate,
        (long long)(u64)(image.base+RVA_REALIZED_BAGGAGE_UPDATE_CORE)
    );
    _InterlockedExchange64(
        &g_originalRealizationCoordinator,
        (long long)(u64)(image.base+RVA_REALIZATION_COORDINATOR)
    );
    if(g_debugLog||scannedHudRequested){
        ApplyResult gateTelemetry=installGateTelemetry(
            image.base,image.imageSize,&gateRelay
        );
        if(gateTelemetry==APPLY_READY){
            _InterlockedExchange(&g_gateTelemetryReady,1);
            char buffer[192];
            buffer[0]=0;
            u32 position=0;
            position=appendText(buffer,position,192u,"HOOK-ACTIVE: scan-gate telemetry/cargo-cap relay=");
            position=appendHex64(buffer,position,192u,(u64)gateRelay);
            position=appendText(buffer,position,192u,"\r\n");
            logRaw(buffer,position);
        }else if(gateTelemetry==APPLY_CRITICAL){
            const char message[]="CRITICAL: scan-gate telemetry rollback or code protection restore was incomplete; stop DS2.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            closeLog();
            return 0;
        }else{
            const char message[]="WARNING: scan-gate telemetry could not be installed; gameplay patches remain available.\r\n";
            logRaw(message,(DWORD)(sizeof(message)-1u));
        }
    }
    if(settings.extendCargoMarkers){
        DWORD detectedProtection=0,entityManagerProtection=0;
        bool detectedHookChanged=false,entityManagerHookChanged=false;
        u8* detectedSlot=image.base+RVA_DETECTED_CONSTANT_UPDATE_HANDLER_SLOT;
        u8* entityManagerSlot=image.base+RVA_ENTITY_MANAGER_GAME_UPDATE_SLOT;
        logHookState("HOOK-BEFORE:","cargo-seen-tracker",detectedSlot);
        ApplyResult detectedRefresh=installPointerHook(
            detectedSlot,
            (u64)(image.base+RVA_DETECTED_CONSTANT_UPDATE_THUNK),
            (u64)(const void*)&detectedConstantUpdateHook,
            &detectedProtection,&detectedHookChanged
        );
        if(detectedRefresh==APPLY_READY){
            logHookState(
                detectedHookChanged?"HOOK-ACTIVE:":"HOOK-ALREADY-ACTIVE:",
                "cargo-seen-tracker",detectedSlot
            );
            logHookState(
                "HOOK-BEFORE:","cargo-frame-pass-through",entityManagerSlot
            );
            ApplyResult entityManagerRefresh=installPointerHook(
                entityManagerSlot,
                (u64)(image.base+RVA_ENTITY_MANAGER_GAME_UPDATE),
                (u64)(const void*)&entityManagerGameUpdateHook,
                &entityManagerProtection,&entityManagerHookChanged
            );
            if(entityManagerRefresh==APPLY_READY){
                logHookState(
                    entityManagerHookChanged?
                        "HOOK-ACTIVE:":"HOOK-ALREADY-ACTIVE:",
                    "cargo-frame-pass-through",entityManagerSlot
                );
                DWORD unlinkProtection=0,initProtection=0,updateProtection=0;
                bool unlinkChanged=false,initChanged=false,updateChanged=false;
                u8* unlinkSlot=
                    image.base+RVA_REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT;
                u8* initSlot=
                    image.base+RVA_REALIZED_BAGGAGE_INIT_HANDLER_SLOT;
                u8* updateSlot=
                    image.base+RVA_REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT;
                logHookState(
                    "HOOK-BEFORE:","late-cargo-unlink-cleanup",unlinkSlot
                );
                ApplyResult unlinkHook=installPointerHook(
                    unlinkSlot,
                    (u64)(image.base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK),
                    (u64)(const void*)&realizedBaggageUnlinkHook,
                    &unlinkProtection,&unlinkChanged
                );
                ApplyResult initHook=APPLY_NOT_READY;
                ApplyResult updateHook=APPLY_NOT_READY;
                ApplyResult coordinatorHook=APPLY_NOT_READY;
                ApplyResult initRollback=APPLY_READY;
                ApplyResult updateRollback=APPLY_READY;
                ApplyResult unlinkRollback=APPLY_FAILED;
                u8* realizationRelay=0;
                if(unlinkHook==APPLY_READY){
                    logHookState(
                        unlinkChanged?"HOOK-ACTIVE:":"HOOK-ALREADY-ACTIVE:",
                        "late-cargo-unlink-cleanup",unlinkSlot
                    );
                    logHookState(
                        "HOOK-BEFORE:","late-cargo-realized-init",initSlot
                    );
                    initHook=installPointerHook(
                        initSlot,
                        (u64)(image.base+RVA_REALIZED_BAGGAGE_INIT_THUNK),
                        (u64)(const void*)&realizedBaggageInitHook,
                        &initProtection,&initChanged
                    );
                    if(initHook==APPLY_READY){
                        logHookState(
                            initChanged?"HOOK-ACTIVE:":"HOOK-ALREADY-ACTIVE:",
                            "late-cargo-realized-init",initSlot
                        );
                        logHookState(
                            "HOOK-BEFORE:","late-cargo-native-update",updateSlot
                        );
                        updateHook=installPointerHook(
                            updateSlot,
                            (u64)(image.base+RVA_REALIZED_BAGGAGE_UPDATE_THUNK),
                            (u64)(const void*)&realizedBaggageUpdateHook,
                            &updateProtection,&updateChanged
                        );
                        if(updateHook==APPLY_READY){
                            logHookState(
                                updateChanged?"HOOK-ACTIVE:":
                                    "HOOK-ALREADY-ACTIVE:",
                                "late-cargo-native-update",updateSlot
                            );
                            coordinatorHook=installRealizationCoordinatorHook(
                                image.base,image.imageSize,&realizationRelay
                            );
                            if(coordinatorHook==APPLY_READY){
                                _InterlockedExchange(&g_lateCargoReady,1);
                                char buffer[224];
                                buffer[0]=0;
                                u32 position=0;
                                position=appendText(
                                    buffer,position,224u,
                                    "HOOK-ACTIVE: bounded cargo-realization relay="
                                );
                                position=appendHex64(
                                    buffer,position,224u,(u64)realizationRelay
                                );
                                position=appendText(
                                    buffer,position,224u,"\r\n"
                                );
                                logRaw(buffer,position);
                            }else{
                                // No delayed hit may be published unless the
                                // native coordinator can also hold its exact
                                // owner. Roll back every lifecycle hook owned
                                // by this installation attempt.
                                if(updateChanged){
                                    updateRollback=rollbackPointerHook(
                                        updateSlot,
                                        (u64)(image.base+
                                            RVA_REALIZED_BAGGAGE_UPDATE_THUNK),
                                        (u64)(const void*)&
                                            realizedBaggageUpdateHook,
                                        updateProtection
                                    );
                                }
                                if(initChanged){
                                    initRollback=rollbackPointerHook(
                                        initSlot,
                                        (u64)(image.base+
                                            RVA_REALIZED_BAGGAGE_INIT_THUNK),
                                        (u64)(const void*)&
                                            realizedBaggageInitHook,
                                        initProtection
                                    );
                                }
                                if(unlinkChanged){
                                    unlinkRollback=rollbackPointerHook(
                                        unlinkSlot,
                                        (u64)(image.base+
                                            RVA_REALIZED_BAGGAGE_UNLINK_THUNK),
                                        (u64)(const void*)&
                                            realizedBaggageUnlinkHook,
                                        unlinkProtection
                                    );
                                }
                            }
                        }else{
                            if(initChanged){
                                initRollback=rollbackPointerHook(
                                    initSlot,
                                    (u64)(image.base+
                                        RVA_REALIZED_BAGGAGE_INIT_THUNK),
                                    (u64)(const void*)&realizedBaggageInitHook,
                                    initProtection
                                );
                            }
                            if(unlinkChanged){
                                unlinkRollback=rollbackPointerHook(
                                    unlinkSlot,
                                    (u64)(image.base+
                                        RVA_REALIZED_BAGGAGE_UNLINK_THUNK),
                                    (u64)(const void*)&realizedBaggageUnlinkHook,
                                    unlinkProtection
                                );
                            }
                        }
                    }else if(unlinkChanged){
                        unlinkRollback=rollbackPointerHook(
                            unlinkSlot,
                            (u64)(image.base+RVA_REALIZED_BAGGAGE_UNLINK_THUNK),
                            (u64)(const void*)&realizedBaggageUnlinkHook,
                            unlinkProtection
                        );
                    }
                }
                if(unlinkHook==APPLY_CRITICAL||initHook==APPLY_CRITICAL||
                   updateHook==APPLY_CRITICAL||coordinatorHook==APPLY_CRITICAL||
                   initRollback==APPLY_CRITICAL||
                   updateRollback==APPLY_CRITICAL||
                   unlinkRollback==APPLY_CRITICAL){
                    const char message[]="CRITICAL: late-cargo realization/lifecycle hook installation or rollback was incomplete; stop DS2.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1u));
                    closeLog();
                    return 0;
                }
                if(_InterlockedCompareExchange(&g_lateCargoReady,0,0)!=1){
                    const char message[]="WARNING: bounded late-cargo native replay stayed disabled; ordinary native cargo scanning remains available.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                }
                _InterlockedExchange(&g_farRefreshReady,1);
            }else{
                bool rollbackCritical=false;
                if(detectedHookChanged){
                    rollbackCritical=rollbackPointerHook(
                        detectedSlot,
                        (u64)(image.base+RVA_DETECTED_CONSTANT_UPDATE_THUNK),
                        (u64)(const void*)&detectedConstantUpdateHook,
                        detectedProtection
                    )==APPLY_CRITICAL;
                }
                if(entityManagerRefresh==APPLY_CRITICAL||rollbackCritical){
                    const char message[]="CRITICAL: cargo frame pass-through hook or tracker rollback was incomplete; stop DS2.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1u));
                    closeLog();
                    return 0;
                }
                const char message[]="WARNING: cargo frame pass-through hook could not be installed; the per-object tracker was rolled back and other Odradek patches remain available.\r\n";
                logRaw(message,(DWORD)(sizeof(message)-1u));
            }
        }else if(detectedRefresh==APPLY_CRITICAL){
            const char message[]="CRITICAL: detected-component handler rollback or memory-protection restore was incomplete; stop DS2.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            closeLog();
            return 0;
        }else{
            const char message[]="WARNING: per-object cargo scan tracker could not be installed; other Odradek patches remain available.\r\n";
            logRaw(message,(DWORD)(sizeof(message)-1u));
        }
    }
    _InterlockedExchange(&g_hooksReady,1);
    logHookState("HOOK-ACTIVE:","gameplay",image.base+RVA_GAMEPLAY_SCAN_SLOT);
    logHookState("HOOK-ACTIVE:","survey",image.base+RVA_SURVEY_FACTORY_SLOT);
    {
        const char message[]="READY: validated gameplay scan hooks installed; native terrain-survey values are preserved.\r\n";
        logRaw(message,(DWORD)(sizeof(message)-1u));
    }

    u8* surveyAddress=0;
    u32 surveyStaleTicks=0;
    bool surveyWaitingLogged=false;
    bool surveyActiveLogged=false;
    bool gameplayFailureLogged=false;
    bool gameplayActiveLogged=false;
    u8* playerCargoConfigAddress=0;
    bool playerCargoWaitingLogged=false;
    bool playerCargoInvalidLogged=false;
    bool playerCargoConflictLogged=false;
    bool playerCargoFailureLogged=false;
    bool playerCargoActiveLogged=false;
    CargoMarkerObjects cargoMarkerObjects;
    memset(&cargoMarkerObjects,0,sizeof(cargoMarkerObjects));
    CargoMarkerOwnership cargoMarkerOwnership;
    memset(&cargoMarkerOwnership,0,sizeof(cargoMarkerOwnership));
    bool cargoMarkerWaitingLogged=false;
    bool cargoMarkerInvalidLogged=false;
    bool cargoMarkerConflictLogged=false;
    bool cargoMarkerFailureLogged=false;
    bool cargoMarkerActiveLogged=false;
    bool cargoMarkerLoggedState=false;
    u8* uiParametersAddress=0;
    HudMarkerOwnership uiMarkerOwnership;
    memset(&uiMarkerOwnership,0,sizeof(uiMarkerOwnership));
    bool uiWaitingLogged=false;
    bool uiInvalidLogged=false;
    bool uiConflictLogged=false;
    bool uiFailureLogged=false;
    bool uiActiveLogged=false;
    bool uiLoggedState=false;
    u32 telemetrySerial=0u;
    u32 telemetryLastCalls=0xFFFFFFFFu;
    u32 telemetryLastMaxRadius=0xFFFFFFFFu;
    u32 telemetryLoggedCalls=0xFFFFFFFFu;
    u32 telemetryLoggedMaxRadius=0xFFFFFFFFu;
    u32 telemetryStableTicks=0u;
    u32 farRefreshLoggedSerial=0u;
    u32 farRefreshObservedSerial=0u;
    u32 farRefreshPendingTicks=0u;
    bool farRefreshWaitLogged=false;
    u32 pendingActivityLogged=pendingCargoActivitySnapshot();

    for(;;){
        logScannedHudTelemetry();
        logSurveyAlignment();
        logCargoMeter();
        logRecipientMeter();
        logVisualWave();
        failClosedPresentationWatchdog(&image);
        purgePendingCargo(false);
        purgeRealizeLeases(false);
        u32 pendingActivity=pendingCargoActivitySnapshot();
        if(pendingActivity!=pendingActivityLogged){
            logPendingCargoTelemetry();
            pendingActivityLogged=pendingActivity;
        }
        u32 farRefreshSerial=(u32)_InterlockedCompareExchange(
            &g_farRefreshScanSerial,0,0
        );
        u32 farRefreshCompleted=(u32)_InterlockedCompareExchange(
            &g_farRefreshCompletedSerial,0,0
        );
        if(farRefreshSerial!=farRefreshObservedSerial){
            farRefreshObservedSerial=farRefreshSerial;
            farRefreshPendingTicks=0u;
            farRefreshWaitLogged=false;
        }
        if(farRefreshCompleted&&farRefreshCompleted!=farRefreshLoggedSerial){
            logFarRefreshTelemetry();
            farRefreshLoggedSerial=farRefreshCompleted;
        }
        if(farRefreshSerial&&farRefreshCompleted!=farRefreshSerial&&
           _InterlockedCompareExchange(&g_farRefreshPending,0,0)==1){
            if(farRefreshPendingTicks<0xFFFFFFFFu)farRefreshPendingTicks++;
            bool managerInactive=false;
            u64 refreshManagerPointer=0;
            if(readU64(
                   image.base+RVA_SENSOR_MANAGER_GLOBAL,&refreshManagerPointer
               )&&refreshManagerPointer){
                u32 refreshActive=0;
                if(readU32(
                       (u8*)refreshManagerPointer+OFF_MANAGER_ACTIVE,
                       &refreshActive
                   ))managerInactive=(refreshActive&0xFFu)!=1u;
            }
            if(managerInactive&&cancelFarRefresh(
                   farRefreshSerial,"scan became inactive"
               )){
                farRefreshWaitLogged=true;
            }else if(farRefreshPendingTicks>=30u&&cancelFarRefresh(
                         farRefreshSerial,"scan refresh timeout"
                     )){
                farRefreshWaitLogged=true;
            }
            if(farRefreshPendingTicks>=10u&&!farRefreshWaitLogged){
                logFarRefreshWaiting();
                farRefreshWaitLogged=true;
            }
        }else{
            farRefreshPendingTicks=0u;
        }
        if(_InterlockedCompareExchange(&g_gateTelemetryReady,0,0)==1&&
           _InterlockedCompareExchange(&g_gateCaptureActive,0,0)==1){
            u64 managerPointer=0;
            bool managerSampleValid=false;
            u32 sampledActive=0;
            if(readU64(image.base+RVA_SENSOR_MANAGER_GLOBAL,&managerPointer)&&managerPointer){
                u8* manager=(u8*)managerPointer;
                u32 active=0,radius=0,angle=0;
                u64 snapshot=0;
                if(readU32(manager+OFF_MANAGER_ACTIVE,&active)){
                    managerSampleValid=true;
                    sampledActive=active;
                    _InterlockedExchange(&g_gateLatestActive,(long)(active&0xFFu));
                }
                if(readU32(manager+OFF_MANAGER_GATE_RADIUS,&radius)){
                    _InterlockedExchange(&g_gateLatestRadiusBits,(long)radius);
                    if((radius&0x80000000u)==0u&&(radius&0x7F800000u)!=0x7F800000u){
                        long current=g_gateMaxRadiusBits;
                        while((u32)current<radius){
                            long observed=_InterlockedCompareExchange(
                                &g_gateMaxRadiusBits,(long)radius,current
                            );
                            if(observed==current)break;
                            current=observed;
                        }
                    }
                }
                if(readU32(manager+OFF_MANAGER_FULL_ANGLE,&angle)){
                    _InterlockedExchange(&g_gateLatestAngleBits,(long)angle);
                }
                if(readU64(manager+OFF_MANAGER_SCAN_SNAPSHOT,&snapshot)){
                    _InterlockedExchange64(&g_gateLatestSnapshot,(long long)snapshot);
                }
            }
            u32 currentSerial=(u32)_InterlockedCompareExchange(&g_gateScanSerial,0,0);
            if(currentSerial&&currentSerial!=telemetrySerial){
                telemetrySerial=currentSerial;
                telemetryLastCalls=0xFFFFFFFFu;
                telemetryLastMaxRadius=0xFFFFFFFFu;
                telemetryLoggedCalls=0xFFFFFFFFu;
                telemetryLoggedMaxRadius=0xFFFFFFFFu;
                telemetryStableTicks=0u;
            }
            if(currentSerial){
                u32 calls=(u32)_InterlockedCompareExchange(&g_gateCalls,0,0);
                u32 maxRadius=(u32)_InterlockedCompareExchange(&g_gateMaxRadiusBits,0,0);
                if(calls==telemetryLastCalls&&maxRadius==telemetryLastMaxRadius){
                    if(telemetryStableTicks<0xFFFFFFFFu)telemetryStableTicks++;
                }else{
                    telemetryLastCalls=calls;
                    telemetryLastMaxRadius=maxRadius;
                    telemetryStableTicks=0u;
                }
                if(gateTelemetryMayFinish(managerSampleValid,sampledActive,telemetryStableTicks)&&
                   (calls!=telemetryLoggedCalls||maxRadius!=telemetryLoggedMaxRadius)){
                    _InterlockedExchange(&g_gateCaptureActive,0);
                    logGateTelemetry();
                    telemetryLoggedCalls=calls;
                    telemetryLoggedMaxRadius=maxRadius;
                }
            }
        }
        long gameplayStatus=_InterlockedCompareExchange(&g_gameplayStatus,0,0);
        if(gameplayStatus==7&&!gameplayActiveLogged){
            logGameplayValues();
            gameplayActiveLogged=true;
            gameplayFailureLogged=false;
        }else if(gameplayStatus>=2&&gameplayStatus<7&&!gameplayFailureLogged){
            logGameplayFailure((u32)gameplayStatus);
            gameplayFailureLogged=true;
            gameplayActiveLogged=false;
        }else if(gameplayStatus>=2&&gameplayStatus<7){
            gameplayActiveLogged=false;
        }
        u8* capturedSurvey=(u8*)(u64)g_surveyResource;
        bool capturedSurveyUsable=capturedSurvey&&
                                  validateSurveyPatchable(&image,capturedSurvey,&settings);

        if(capturedSurveyUsable&&capturedSurvey!=surveyAddress){
            surveyAddress=capturedSurvey;
            surveyStaleTicks=0u;
            surveyWaitingLogged=false;
            surveyActiveLogged=false;
        }else if(!surveyAddress&&capturedSurvey&&!surveyWaitingLogged){
            const char message[]="STATE: captured survey object is not serialized yet; waiting.\r\n";
            logRaw(message,(DWORD)(sizeof(message)-1u));
            surveyWaitingLogged=true;
        }
        if(surveyAddress){
            // Preserve the native terrain survey values. Cargo/sensor range,
            // gameplay full-circle angle and the visual wave are handled through
            // independent runtime paths.
            if(validateSurveyNative(&image,surveyAddress)){
                surveyStaleTicks=0u;
                if(!surveyActiveLogged){
                    const char message[]="ACTIVE: native terrain survey preserved (no ShapeMode/angle/range writes).\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                    logSurveyValues("NATIVE-TERRAIN:",surveyAddress);
                    surveyActiveLogged=true;
                }
                surveyWaitingLogged=false;
            }else{
                surveyStaleTicks++;
                if(!surveyWaitingLogged){
                    const char message[]="STATE: terrain survey is not in the expected native state; leaving it untouched.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                    logSurveyValues("TERRAIN-UNTOUCHED:",surveyAddress);
                    surveyWaitingLogged=true;
                }
                if(surveyStaleTicks>=5u){
                    surveyAddress=0;
                    surveyStaleTicks=0u;
                    surveyActiveLogged=false;
                }
            }
        }

        if(settings.extendCargoMarkers){
            // The +0x44/+0x48 fields reached through this legacy resource path
            // contained text-like bytes in the live build, not plausible
            // distances. Keep the reverse-engineering code available to the
            // synthetic harness, but never write these unverified fields.
            if(ENABLE_UNVERIFIED_PLAYER_CARGO_FIELDS){
            u8* resolvedPlayerConfig=0;
            ResolveResult playerResult=resolvePlayerOdradekConfig(
                &image,&resolvedPlayerConfig
            );
            if(playerResult==RESOLVE_NOT_READY){
                if(!playerCargoWaitingLogged){
                    const char message[]="STATE: player Lost Cargo sensor config is not loaded yet; waiting.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                    playerCargoWaitingLogged=true;
                }
                playerCargoConfigAddress=0;
                playerCargoActiveLogged=false;
            }else if(playerResult==RESOLVE_INVALID){
                if(!playerCargoInvalidLogged){
                    const char message[]="ERROR: canonical player Odradek config path is invalid; Lost Cargo sensor writes are paused.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1u));
                    playerCargoInvalidLogged=true;
                }
                playerCargoConfigAddress=0;
                playerCargoActiveLogged=false;
            }else{
                if(resolvedPlayerConfig!=playerCargoConfigAddress){
                    playerCargoConfigAddress=resolvedPlayerConfig;
                    playerCargoWaitingLogged=false;
                    playerCargoInvalidLogged=false;
                    playerCargoConflictLogged=false;
                    playerCargoFailureLogged=false;
                    playerCargoActiveLogged=false;
                }
                if(validatePlayerCargoTarget(
                    &image,playerCargoConfigAddress,&settings
                )){
                    if(!playerCargoActiveLogged){
                        const char message[]="ACTIVE: Lost Cargo detect and search ranges are at least RangeMeters.\r\n";
                        logRaw(message,(DWORD)(sizeof(message)-1u));
                        logPlayerCargoRanges(
                            "ACTIVE-PLAYER-CARGO-RANGES:",playerCargoConfigAddress
                        );
                        playerCargoActiveLogged=true;
                    }
                    playerCargoConflictLogged=false;
                    playerCargoFailureLogged=false;
                }else{
                    if(!playerCargoFailureLogged&&!playerCargoConflictLogged){
                        logPlayerCargoRanges(
                            "PLAYER-CARGO-RANGES-BEFORE:",playerCargoConfigAddress
                        );
                    }
                    ApplyResult playerApplied=applyPlayerCargoRanges(
                        &image,playerCargoConfigAddress,&settings
                    );
                    if(playerApplied==APPLY_READY){
                        logPlayerCargoRanges(
                            "PLAYER-CARGO-RANGES-AFTER:",playerCargoConfigAddress
                        );
                        playerCargoWaitingLogged=false;
                        playerCargoConflictLogged=false;
                        playerCargoFailureLogged=false;
                        playerCargoActiveLogged=false;
                    }else if(playerApplied==APPLY_NOT_READY){
                        playerCargoActiveLogged=false;
                    }else if(playerApplied==APPLY_CONFLICT){
                        if(!playerCargoConflictLogged){
                            const char message[]="ERROR: Lost Cargo sensor ranges changed during reconciliation; writes will retry.\r\n";
                            logCritical(message,(DWORD)(sizeof(message)-1u));
                            playerCargoConflictLogged=true;
                        }
                    }else if(playerApplied==APPLY_CRITICAL){
                        const char message[]="CRITICAL: Lost Cargo sensor rollback or protection restore was incomplete; stop DS2.\r\n";
                        logCritical(message,(DWORD)(sizeof(message)-1u));
                        break;
                    }else if(!playerCargoFailureLogged){
                        const char message[]="ERROR: Lost Cargo sensor range write failed and was rolled back; it will retry.\r\n";
                        logCritical(message,(DWORD)(sizeof(message)-1u));
                        playerCargoFailureLogged=true;
                    }
                }
            }
            }

            CargoMarkerObjects resolvedMarkers;
            ResolveResult markerResult=resolveCargoMarkerObjects(&image,&resolvedMarkers);
            const CargoMarkerObjects* markerTarget=0;
            if(markerResult==RESOLVE_NOT_READY){
                if(!cargoMarkerWaitingLogged){
                    const char message[]="STATE: lost-cargo marker config is not loaded yet; waiting.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                    cargoMarkerWaitingLogged=true;
                }
                memset(&cargoMarkerObjects,0,sizeof(cargoMarkerObjects));
                cargoMarkerActiveLogged=false;
            }else if(markerResult==RESOLVE_INVALID){
                if(!cargoMarkerInvalidLogged){
                    const char message[]="ERROR: canonical lost-cargo marker path is invalid; marker writes are paused.\r\n";
                    logCritical(message,(DWORD)(sizeof(message)-1u));
                    cargoMarkerInvalidLogged=true;
                }
                memset(&cargoMarkerObjects,0,sizeof(cargoMarkerObjects));
                cargoMarkerActiveLogged=false;
            }else{
                if(!sameCargoMarkerObjects(&resolvedMarkers,&cargoMarkerObjects)){
                    cargoMarkerObjects=resolvedMarkers;
                    cargoMarkerWaitingLogged=false;
                    cargoMarkerInvalidLogged=false;
                    cargoMarkerConflictLogged=false;
                    cargoMarkerFailureLogged=false;
                    cargoMarkerActiveLogged=false;
                }
                markerTarget=&cargoMarkerObjects;
            }

            bool cargoPresentation=false;
            ApplyResult applied=reconcileCargoMarkerRanges(
                &image,markerTarget,&settings,&cargoMarkerOwnership,
                &cargoPresentation
            );
            if(applied==APPLY_READY){
                if(markerResult==RESOLVE_READY&&(
                       !cargoMarkerActiveLogged||
                       cargoMarkerLoggedState!=cargoPresentation
                   )){
                    if(cargoPresentation){
                        const char message[]="SCAN-PRESENTATION: only untouched Lost Cargo world-marker gates are extended for this scan.\r\n";
                        logRaw(message,(DWORD)(sizeof(message)-1u));
                    }else{
                        const char message[]="IDLE-PRESENTATION: Lost Cargo world-marker gates are native; touched and mission cargo were never extended.\r\n";
                        logRaw(message,(DWORD)(sizeof(message)-1u));
                    }
                    logCargoMarkerValues(
                        "CARGO-MARKER-STATE:",&cargoMarkerObjects
                    );
                    cargoMarkerLoggedState=cargoPresentation;
                    cargoMarkerActiveLogged=true;
                }
                cargoMarkerWaitingLogged=markerResult!=RESOLVE_READY;
                cargoMarkerConflictLogged=false;
                cargoMarkerFailureLogged=false;
            }else if(applied==APPLY_NOT_READY){
                cargoMarkerActiveLogged=false;
            }else if(applied==APPLY_CONFLICT){
                if(!cargoMarkerConflictLogged){
                    const char message[]="WARNING: a Lost Cargo marker field changed externally; that field was left untouched while other owned fields were reconciled.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                    cargoMarkerConflictLogged=true;
                }
                cargoMarkerActiveLogged=false;
            }else if(applied==APPLY_CRITICAL){
                const char message[]="CRITICAL: Lost Cargo marker rollback or protection restore was incomplete; stop DS2.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                break;
            }else if(!cargoMarkerFailureLogged){
                const char message[]="ERROR: Lost Cargo marker write failed; owned fields will be retried without overwriting foreign values.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                cargoMarkerFailureLogged=true;
                cargoMarkerActiveLogged=false;
            }

        }

        u8* resolvedUiParameters=0;
        ResolveResult uiResult=resolveUiGameDesign(&image,&resolvedUiParameters);
        if(uiResult==RESOLVE_NOT_READY){
            if(!uiWaitingLogged){
                const char message[]="STATE: HUD marker parameters are not loaded yet; waiting.\r\n";
                logRaw(message,(DWORD)(sizeof(message)-1u));
                uiWaitingLogged=true;
            }
            uiParametersAddress=0;
            uiActiveLogged=false;
        }else if(uiResult==RESOLVE_INVALID){
            if(!uiInvalidLogged){
                const char message[]="ERROR: canonical HUD marker parameter path is invalid; HUD writes are paused.\r\n";
                logCritical(message,(DWORD)(sizeof(message)-1u));
                uiInvalidLogged=true;
            }
            uiParametersAddress=0;
            uiActiveLogged=false;
        }else{
            if(resolvedUiParameters!=uiParametersAddress){
                uiParametersAddress=resolvedUiParameters;
                uiWaitingLogged=false;
                uiInvalidLogged=false;
                uiConflictLogged=false;
                uiFailureLogged=false;
                uiActiveLogged=false;
            }
        }

        bool uiPresentation=false;
        ApplyResult uiApplied=reconcileHudMarkers(
            &image,uiResult==RESOLVE_READY?uiParametersAddress:0,
            &settings,&uiMarkerOwnership,&uiPresentation
        );
        if(uiApplied==APPLY_READY){
            if(uiResult==RESOLVE_READY&&(
                   !uiActiveLogged||uiLoggedState!=uiPresentation
               )){
                if(uiPresentation){
                    const char message[]="SCAN-PRESENTATION: only the Lost Cargo HUD gate is extended; the shared searched-marker gate stays native.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                }else{
                    const char message[]="IDLE-PRESENTATION: the Lost Cargo HUD gate is native.\r\n";
                    logRaw(message,(DWORD)(sizeof(message)-1u));
                }
                logHudValues("HUD-MARKER-STATE:",uiParametersAddress);
                uiLoggedState=uiPresentation;
                uiActiveLogged=true;
            }
            uiWaitingLogged=uiResult!=RESOLVE_READY;
            uiConflictLogged=false;
            uiFailureLogged=false;
        }else if(uiApplied==APPLY_NOT_READY){
            uiActiveLogged=false;
        }else if(uiApplied==APPLY_CONFLICT){
            if(!uiConflictLogged){
                const char message[]="WARNING: the Lost Cargo HUD distance changed externally; the foreign value was left untouched.\r\n";
                logRaw(message,(DWORD)(sizeof(message)-1u));
                uiConflictLogged=true;
            }
            uiActiveLogged=false;
        }else if(uiApplied==APPLY_CRITICAL){
            const char message[]="CRITICAL: HUD marker rollback or memory-protection restore was incomplete; stop DS2.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            break;
        }else if(!uiFailureLogged){
            const char message[]="ERROR: HUD marker write failed; owned values will be retried without overwriting foreign values.\r\n";
            logCritical(message,(DWORD)(sizeof(message)-1u));
            uiFailureLogged=true;
            uiActiveLogged=false;
        }
        Sleep(RETRY_DELAY_MS);
    }

    _InterlockedExchange(&g_hooksReady,0);
    _InterlockedExchange(&g_farRefreshReady,0);
    _InterlockedExchange(&g_lateCargoReady,0);
    purgePendingCargo(true);
    purgeRealizeLeases(true);
    AcquireSRWLockExclusive(&g_farRefreshStateLock);
    clearAnyScanPresentation();
    if(_InterlockedCompareExchange(&g_farRefreshBusy,0,0)==0){
        _InterlockedExchange(&g_farRefreshPending,0);
        _InterlockedExchange(&g_farRefreshPendingSerial,0);
    }
    ReleaseSRWLockExclusive(&g_farRefreshStateLock);
    ApplyResult cargoCleanup=APPLY_READY;
    ApplyResult uiCleanup=APPLY_READY;
    for(u32 attempt=0u;attempt<3u&&
        cargoMarkerOwnershipPresent(&cargoMarkerOwnership);attempt++){
        cargoCleanup=drainCargoMarkerOwnership(
            &image,&cargoMarkerOwnership
        );
        if(cargoCleanup==APPLY_CRITICAL)break;
    }
    for(u32 attempt=0u;attempt<3u&&uiMarkerOwnership.field.owned;attempt++){
        uiCleanup=drainHudMarkerOwnership(&image,&uiMarkerOwnership);
        if(uiCleanup==APPLY_CRITICAL)break;
    }
    if(cargoCleanup==APPLY_CRITICAL||uiCleanup==APPLY_CRITICAL||
       cargoMarkerOwnershipPresent(&cargoMarkerOwnership)||
       uiMarkerOwnership.field.owned){
        const char message[]="CRITICAL: final temporary marker cleanup was incomplete; stop DS2.\r\n";
        logCritical(message,(DWORD)(sizeof(message)-1u));
    }
    _InterlockedExchange(&g_gateTelemetryReady,0);
    _InterlockedExchange(&g_gateCaptureActive,0);
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
#ifndef ODRADEK_TEST_BUILD
        start();
#endif
    }
    return TRUE;
}
