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
extern "C" s64 _InterlockedCompareExchange64(volatile s64*,s64,s64);
extern "C" void* _ReturnAddress();
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_ReturnAddress)

#define MOD_VERSION "1.0.0"
#define EXPECTED_TIMESTAMP 0x6A3DAE46u
#define EXPECTED_IMAGE_SIZE 0x0B292000u

static const u64 RVA_GLOBAL_ROOT=0x0623EAD8ull;
static const u64 RVA_MANAGER_VTABLE=0x03280940ull;
static const u64 RVA_FACILITY_VTABLE=0x032827C8ull;
static const u64 RVA_MATERIAL_LEVEL_CONTEXT=0x012809EBull;
static const u64 RVA_MATERIAL_LEVEL_CALL=0x01280A06ull;
static const u64 RVA_MATERIAL_LEVEL_RETURN=0x01280A0Dull;
static const u64 RVA_CONSTRUCTION_INIT=0x0127B340ull;
static const u64 RVA_INIT_DISPATCH_CONTEXT_A=0x01260FD0ull;
static const u64 RVA_INIT_DISPATCH_CONTEXT_B=0x01262044ull;
static const u64 RVA_CONSTRUCTION_UPDATE=0x01280950ull;
static const u64 RVA_UPDATE_DISPATCH_CONTEXT=0x01281272ull;
static const u64 RVA_UPDATE_DISPATCH_RETURN=0x01281294ull;
static const u64 RVA_UPDATE_DISPATCH_CONTEXT_SECONDARY=0x012825E3ull;
static const u64 RVA_UPDATE_DISPATCH_RETURN_SECONDARY=0x012825F6ull;
static const u64 RVA_EFFECTIVE_MAX_LEVEL=0x012D81D0ull;
static const u64 RVA_LEVEL_ASSIGN_CONTEXT=0x012874F8ull;
static const u64 RVA_COMPLETION_COMPARE=0x012875F5ull;
static const u64 RVA_CUSTOMIZABLE_COMPARE=0x012876A8ull;
static const u64 RVA_INIT_PENDING_COPIES=0x0127B6AAull;
static const u64 RVA_PCC_FOUNDATION_PENDING_DESCRIPTOR=0x00C96356ull;
static const u64 RVA_FOUNDATION_STATE_ENTRY=0x01262F8Full;
static const u64 RVA_PLAYER_FOUNDATION_STATE_SELECT=0x01266E44ull;
static const u64 RVA_FOUNDATION_PENDING_TIMER_GATE=0x012828BCull;
static const u64 RVA_FOUNDATION_DWELL_SECONDS=0x03460EC8ull;
static const u64 RVA_STREAMING_ACTIVE_ATTACH=0x01376219ull;
static const u64 RVA_SECONDARY_UPDATE_OBJECT_GATE=0x0137648Dull;
static const u64 RVA_LIFECYCLE_ACTIVE_TRANSITION=0x012805F3ull;
static const u64 RVA_UPDATE_LEVEL_DIRTY=0x0128098Aull;
static const u64 RVA_UPDATE_MATERIAL_DIRTY=0x012809BEull;
static const u64 RVA_MATERIAL_DIRTY_REARM=0x01280A0Full;
static const u64 RVA_SETLEVEL_PENDING_CHECK=0x012874BEull;
static const u64 RVA_SETLEVEL_DIRTY_SET=0x012875B4ull;
static const u64 RVA_LEVEL_TERMINAL_ACK=0x01288D74ull;
static const u64 RVA_DEFERRED_CALLBACK_QUEUE=0x01287954ull;
static const u64 RVA_DEFERRED_CALLBACK_DRAIN_CHECK=0x01281BF7ull;
static const u64 RVA_DEFERRED_CALLBACK_DRAIN_REMOVE=0x01281CB8ull;
static const u64 RVA_MANAGER_CALLBACK_OBJECT_QUEUE=0x01287162ull;
static const u64 RVA_MANAGER_CALLBACK_ENQUEUE_PRIMARY=0x012871E2ull;
static const u64 RVA_MANAGER_CALLBACK_ENQUEUE_SECONDARY=0x0128727Bull;
static const u64 RVA_APPLIED_ACK_CALLBACK_PRIMARY=0x01299B6Dull;
static const u64 RVA_APPLIED_ACK_CALLBACK_SECONDARY=0x01299D60ull;

static const u32 OFF_ROOT_MANAGER=0x28u;
static const u32 OFF_FACILITY_CONFIG=0x30u;
static const u32 OFF_MAX_LEVEL=0x2Cu;
static const u32 OFF_COMPLETION_LEVEL=0x4E8u;
static const u32 OFF_CUSTOMIZABLE_LEVEL=0x4ECu;
static const u32 OFF_OBJECT_TYPE=0x28u;
static const u32 OFF_OBJECT_SUBTYPE=0x29u;
static const u32 OFF_OBJECT_CONSTRUCTION_ID=0x2Cu;
static const u32 OFF_OBJECT_NET_TYPE=0x30u;
static const u32 OFF_OBJECT_LIFECYCLE_STATE=0x34u;
static const u32 OFF_OBJECT_FOUNDATION_TIMER=0x224u;
static const u32 OFF_OBJECT_DEFERRED_CALLBACK_COUNT=0x2E8u;
static const u32 OFF_OBJECT_SECONDARY_UPDATE_BLOCKED=0x3C8u;
static const u32 OFF_OBJECT_MATERIAL_DIRTY=0x3CBu;
static const u32 OFF_OBJECT_BUILD_PENDING=0x3D1u;
static const u32 OFF_OBJECT_LEVEL_DIRTY=0x3E6u;
static const u32 OFF_OBJECT_BUILD_AUX_PENDING=0x3EEu;
static const u32 OFF_OBJECT_LEVEL=0x3F5u;
static const u32 OFF_OBJECT_APPLIED_LEVEL=0x3F6u;
static const u32 OFF_DESCRIPTOR_TYPE=0x10u;
static const u32 OFF_DESCRIPTOR_SUBTYPE=0x60u;
static const u32 OFF_DESCRIPTOR_CONSTRUCTION_ID=0x64u;
static const u32 OFF_DESCRIPTOR_NET_TYPE=0x68u;
static const u32 OFF_DESCRIPTOR_LEVEL=0x70u;
static const u32 OFF_VTABLE_INIT=0xE0u;
static const u32 OFF_VTABLE_UPDATE=0x140u;
static const u32 OFF_VTABLE_SET_LEVEL=0x238u;
static const u8 CONSTRUCTION_NET_TYPE_PLAYER=1u;
static const u32 FOUNDATION_DWELL_BITS=0x3DCCCCCDu;

static const u8 EXPECTED_MATERIAL_LEVEL_CONTEXT[36]={
    0x48,0x8B,0xCF,0xE8,0xDD,0xD1,0x00,0x00,0x0F,0xB6,0x8F,0xF5,0x03,0x00,0x00,0x3B,
    0xC1,0x74,0x1C,0x4C,0x8B,0x07,0x8B,0xD0,0x48,0x8B,0xCF,0x41,0xFF,0x90,0x38,0x02,
    0x00,0x00,0xEB,0x0B
};
static const u8 EXPECTED_CONSTRUCTION_INIT[29]={
    0x4C,0x8B,0xDC,0x56,0x48,0x81,0xEC,0xD0,0x02,0x00,0x00,0x48,0x8B,0x05,0xAE,0xE4,
    0x73,0x02,0x48,0x33,0xC4,0x48,0x89,0x84,0x24,0x20,0x02,0x00,0x00
};
static const u8 EXPECTED_CONSTRUCTION_UPDATE[26]={
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x55,0x57,0x41,0x56,0x48,
    0x8D,0x68,0xA1,0x48,0x81,0xEC,0x00,0x01,0x00,0x00
};
static const u8 EXPECTED_UPDATE_DISPATCH_CONTEXT[34]={
    0x48,0x8B,0x46,0x38,0x41,0xB0,0x01,0x48,0x85,0xC0,0x48,0x8B,0xCE,0xC5,0xF8,0x28,
    0xCE,0x4C,0x8D,0x78,0xE0,0x48,0x8B,0x06,0x4D,0x0F,0x44,0xFD,0xFF,0x90,0x40,0x01,
    0x00,0x00
};
static const u8 EXPECTED_UPDATE_DISPATCH_CONTEXT_SECONDARY[27]={
    0x48,0x8B,0x03,0x45,0x33,0xC0,0xC5,0xF8,0x28,0xCE,0x48,0x8B,0xCB,0xFF,0x90,0x40,
    0x01,0x00,0x00,0x48,0x8B,0xCB,0xE8,0x22,0x02,0x00,0x00
};
static const u8 EXPECTED_INIT_DISPATCH_CONTEXT_A[41]={
    0xE8,0x6B,0xEC,0xFF,0xFF,0x48,0x8B,0xF0,0x48,0x85,0xC0,0x74,0x7E,0x4C,0x8B,0x00,
    0x49,0x8B,0xD6,0x48,0x8B,0xC8,0x41,0xFF,0x90,0xE0,0x00,0x00,0x00,0x48,0x8D,0x55,
    0xE0,0x48,0x8B,0xCF,0xE8,0x17,0x1F,0x01,0x00
};
static const u8 EXPECTED_INIT_DISPATCH_CONTEXT_B[45]={
    0xE8,0xF7,0xDB,0xFF,0xFF,0x4C,0x8B,0xF8,0x48,0x85,0xC0,0x0F,0x84,0x9C,0x00,0x00,
    0x00,0x4C,0x8B,0x00,0x48,0x8B,0xD7,0x48,0x8B,0xC8,0x41,0xFF,0x90,0xE0,0x00,0x00,
    0x00,0x48,0x8D,0x55,0xE0,0x48,0x8B,0xCE,0xE8,0x9F,0x0E,0x01,0x00
};
static const u8 EXPECTED_MATERIAL_LEVEL_CALL[7]={
    0x41,0xFF,0x90,0x38,0x02,0x00,0x00
};
static const u8 EXPECTED_EFFECTIVE_MAX_LEVEL[49]={
    0x40,0x53,0x48,0x83,0xEC,0x20,0x80,0x79,0x28,0x06,0x48,0x8B,0xD9,0x75,0x19,0xB9,
    0x30,0x00,0x00,0x00,0xE8,0x97,0xE6,0x1A,0x00,0x84,0xC0,0x75,0x0B,0x8B,0x43,0x2C,
    0xFF,0xC8,0x48,0x83,0xC4,0x20,0x5B,0xC3,0x8B,0x43,0x2C,0x48,0x83,0xC4,0x20,0x5B,
    0xC3
};
static const u8 EXPECTED_LEVEL_ASSIGN_CONTEXT[31]={
    0x41,0x0F,0xB6,0x86,0xF5,0x03,0x00,0x00,0xB9,0x10,0x00,0x00,0x00,0x44,0x3B,0xE0,
    0x45,0x0F,0xB6,0xCC,0x49,0x8B,0x46,0x38,0x45,0x88,0x8E,0xF5,0x03,0x00,0x00
};
static const u8 EXPECTED_COMPLETION_COMPARE[16]={
    0x41,0x0F,0xB6,0x86,0xF5,0x03,0x00,0x00,0x3B,0x81,0xE8,0x04,0x00,0x00,0x75,0x33
};
static const u8 EXPECTED_CUSTOMIZABLE_COMPARE[16]={
    0x41,0x0F,0xB6,0x86,0xF5,0x03,0x00,0x00,0x41,0x3B,0x80,0xEC,0x04,0x00,0x00,0x75
};
static const u8 EXPECTED_SPAWN_LEVEL_SEED[49]={
    0xC4,0xC1,0x78,0x10,0x45,0x74,0xC4,0xC1,0x78,0x11,0x46,0x58,0x41,0x8B,0x45,0x6C,
    0x41,0x89,0x46,0x70,0x41,0x0F,0xB6,0x45,0x70,0x41,0x88,0x86,0xF6,0x03,0x00,0x00,
    0x41,0x88,0x86,0xF5,0x03,0x00,0x00,0x41,0x0F,0xB7,0x45,0x72,0x66,0x41,0x89,0x46,
    0x32
};
static const u8 EXPECTED_NET_TYPE_COPY[9]={
    0x41,0x0F,0xB6,0x45,0x68,0x41,0x88,0x46,0x30
};
static const u8 EXPECTED_INIT_PENDING_COPIES[30]={
    0x41,0x0F,0xB6,0x85,0x95,0x00,0x00,0x00,0x41,0x88,0x86,0xD1,0x03,0x00,0x00,
    0x41,0x0F,0xB6,0x85,0xB2,0x00,0x00,0x00,0x41,0x88,0x86,0xEE,0x03,0x00,0x00
};
static const u8 EXPECTED_PCC_FOUNDATION_PENDING_DESCRIPTOR[9]={
    0x66,0xC7,0x86,0x94,0x00,0x00,0x00,0x01,0x01
};
static const u8 EXPECTED_FOUNDATION_STATE_ENTRY[22]={
    0x48,0x8B,0x06,0x48,0x8B,0xCE,0xFF,0x90,0x48,0x01,0x00,0x00,0xB2,0x02,0x48,0x8B,
    0xCE,0xE8,0xEB,0xD5,0x01,0x00
};
static const u8 EXPECTED_PLAYER_FOUNDATION_STATE_SELECT[47]={
    0x48,0x8B,0x43,0x38,0x48,0x85,0xC0,0x48,0x8D,0x48,0x78,0x49,0x0F,0x44,0xCE,0x48,
    0x8B,0x01,0x48,0xC1,0xE8,0x30,0xA8,0x01,0x74,0x0B,0xB2,0x02,0x44,0x38,0xA7,0xA4,
    0x00,0x00,0x00,0x74,0x02,0xB2,0x03,0x48,0x8B,0xCB,0xE8,0x1D,0x97,0x01,0x00
};
static const u8 EXPECTED_FOUNDATION_PENDING_TIMER_GATE[31]={
    0x80,0xBF,0xD1,0x03,0x00,0x00,0x00,0x74,0x16,0xC5,0xFA,0x10,0x05,0xFB,0xE5,
    0x1D,0x02,0xC5,0xF8,0x2F,0x87,0x24,0x02,0x00,0x00,0x0F,0x87,0xF2,0x02,0x00,0x00
};
static const u8 EXPECTED_FOUNDATION_DWELL_SECONDS[4]={
    0xCD,0xCC,0xCC,0x3D
};
static const u8 EXPECTED_STREAMING_ACTIVE_ATTACH[22]={
    0x48,0x8B,0x49,0x58,0x48,0x85,0xC9,0x74,0x5F,0x80,0x79,0x34,0x02,0x75,0x07,0xB2,
    0x03,0xE8,0x61,0xA3,0xF0,0xFF
};
static const u8 EXPECTED_SECONDARY_UPDATE_OBJECT_GATE[17]={
    0x48,0x8B,0x49,0x58,0x80,0xB9,0xC8,0x03,0x00,0x00,0x00,0x0F,0x85,0xFF,0x00,0x00,
    0x00
};
static const u8 EXPECTED_LIFECYCLE_ACTIVE_TRANSITION[35]={
    0x48,0x8B,0x03,0x48,0x8B,0xCB,0x40,0x3A,0xF7,0x73,0x0C,0xC6,0x43,0x34,0x03,
    0xFF,0x90,0xC8,0x01,0x00,0x00,0xEB,0x2D,0xC6,0x43,0x34,0x01,0xFF,0x90,0xC0,
    0x01,0x00,0x00,0xEB,0x21
};
static const u8 EXPECTED_UPDATE_LEVEL_DIRTY[22]={
    0x80,0xB9,0xE6,0x03,0x00,0x00,0x00,0x74,0x0E,0x33,0xD2,0xE8,0xE6,0x77,0x00,
    0x00,0xC6,0x87,0xE6,0x03,0x00,0x00
};
static const u8 EXPECTED_UPDATE_MATERIAL_DIRTY[12]={
    0x0F,0xB6,0xB7,0xCB,0x03,0x00,0x00,0xC6,0x87,0xCB,0x03,0x00
};
static const u8 EXPECTED_MATERIAL_DIRTY_REARM[11]={
    0x3C,0x04,0x75,0x07,0xC6,0x87,0xCB,0x03,0x00,0x00,0x01
};
static const u8 EXPECTED_SETLEVEL_PENDING_CHECK[21]={
    0x45,0x38,0xBE,0xD1,0x03,0x00,0x00,0x75,0x19,0x49,0x8B,0xCE,0xE8,0x31,0xEE,
    0x97,0xFF,0x84,0xC0,0x74,0x0D
};
static const u8 EXPECTED_SETLEVEL_DIRTY_SET[15]={
    0x41,0xC6,0x86,0xE6,0x03,0x00,0x00,0x01,0x48,0x8B,0x48,0x28,0x48,0x85,0xC9
};
static const u8 EXPECTED_LEVEL_TERMINAL_ACK[27]={
    0x0F,0xB6,0x86,0xF5,0x03,0x00,0x00,0x88,0x86,0xF6,0x03,0x00,0x00,0xC6,0x86,
    0xD1,0x03,0x00,0x00,0x00,0xC6,0x86,0xEE,0x03,0x00,0x00,0x00
};
static const u8 EXPECTED_DEFERRED_CALLBACK_QUEUE[22]={
    0x48,0x8D,0x54,0x24,0x28,0x48,0x89,0x44,0x24,0x28,0x49,0x8D,0x8E,0xE8,0x02,
    0x00,0x00,0xE8,0xF6,0x23,0xE9,0xFE
};
static const u8 EXPECTED_DEFERRED_CALLBACK_DRAIN_CHECK[22]={
    0x45,0x8B,0xF5,0x44,0x39,0xAE,0xE8,0x02,0x00,0x00,0x0F,0x8E,0x0B,0x01,0x00,
    0x00,0x41,0xBF,0x01,0x00,0x00,0x00
};
static const u8 EXPECTED_DEFERRED_CALLBACK_DRAIN_REMOVE[50]={
    0x8B,0x86,0xE8,0x02,0x00,0x00,0x48,0x8B,0x9E,0xF0,0x02,0x00,0x00,0x41,0x2B,
    0xC6,0xFF,0xC8,0x4C,0x63,0xC0,0x49,0x63,0xC7,0x49,0xC1,0xE0,0x03,0x48,0x8D,
    0x14,0xC3,0x49,0x63,0xC6,0x48,0x8D,0x0C,0xC3,0xE8,0xD6,0x86,0x84,0x01,0xFF,
    0x8E,0xE8,0x02,0x00,0x00
};
static const u8 EXPECTED_MANAGER_CALLBACK_OBJECT_QUEUE[18]={
    0x48,0x8B,0x01,0x4C,0x8B,0xF1,0x48,0x89,0x44,0x24,0x20,0x48,0x8D,0xA8,0xE8,
    0x02,0x00,0x00
};
static const u8 EXPECTED_MANAGER_CALLBACK_ENQUEUE_PRIMARY[18]={
    0x48,0x8D,0x54,0x24,0x50,0x48,0x89,0x44,0x24,0x50,0x48,0x8B,0xCD,0xE8,0x6C,
    0x2B,0xE9,0xFE
};
static const u8 EXPECTED_MANAGER_CALLBACK_ENQUEUE_SECONDARY[22]={
    0x48,0x8D,0x54,0x24,0x50,0x48,0x89,0x44,0x24,0x50,0x48,0x8D,0x8B,0xE8,0x02,
    0x00,0x00,0xE8,0xCF,0x2A,0xE9,0xFE
};
static const u8 EXPECTED_APPLIED_ACK_CALLBACK_PRIMARY[17]={
    0x48,0x8B,0x4B,0x10,0x0F,0xB6,0x81,0xF5,0x03,0x00,0x00,0x88,0x81,0xF6,0x03,
    0x00,0x00
};
static const u8 EXPECTED_APPLIED_ACK_CALLBACK_SECONDARY[17]={
    0x48,0x8B,0x4F,0x10,0x0F,0xB6,0x81,0xF5,0x03,0x00,0x00,0x88,0x81,0xF6,0x03,
    0x00,0x00
};

static const u64 MANAGER_VFUNCS[4]={
    0x012426B0ull,0x01274280ull,0x00109DE0ull,0x00109EB0ull
};
static const u64 FACILITY_VFUNCS[4]={
    0x01242690ull,0x00291C50ull,0x00109DE0ull,0x00109EB0ull
};

struct TargetDefinition{
    const char* label;
    u32 managerOffset;
    u32 alternateManagerOffset;
    u64 configVtableRva;
    u64 vfuncs[4];
    u32 nativeMaxLevel;
    u32 nativeCompletionLevel;
    u32 nativeCustomizableLevel;
};

static const TargetDefinition TARGETS[]={
    {"Postbox",0x38u,0u,0x03291028ull,{0x01305660ull,0x01305670ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"SafeHouse",0x30u,0u,0x032913C0ull,{0x01307B90ull,0x01307C60ull,0x00109DE0ull,0x00109EB0ull},4u,2u,3u},
    {"Watchtower",0x40u,0u,0x0327F8F8ull,{0x01245220ull,0x01245230ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"CargoCatapult",0x48u,0u,0x03293488ull,{0x01311E70ull,0x01311FF0ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"Generator",0x50u,0u,0x032938E8ull,{0x0131A0F0ull,0x0131A220ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"RainShelter",0x58u,0u,0x03291EC0ull,{0x01306030ull,0x013060D0ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"Zipline",0x68u,0u,0x032863A8ull,{0x01243560ull,0x01243720ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"Bridge30m",0x80u,0x138u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull},4u,2u,3u},
    {"Bridge45m",0x140u,0u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull},4u,2u,3u},
    {"Bridge80m",0x148u,0u,0x03292F68ull,{0x0130D8F0ull,0x0130DA00ull,0x00109DE0ull,0x00109EB0ull},4u,2u,3u},
    {"ChiralBridge",0x98u,0u,0x03292870ull,{0x0131CEC0ull,0x0131CED0ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"JumpRamp",0xA0u,0u,0x03291CF0ull,{0x012EE520ull,0x012EE610ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u},
    {"FastTravelStation",0xC0u,0u,0x03291068ull,{0x012EABB0ull,0x01247010ull,0x00109DE0ull,0x00109EB0ull},3u,1u,2u}
};
static const u32 TARGET_COUNT=(u32)(sizeof(TARGETS)/sizeof(TARGETS[0]));

struct HookClassDefinition{
    const char* label;
    u64 objectVtableRva;
    u64 originalInitRva;
    u64 originalSetLevelRva;
};
static const HookClassDefinition HOOK_CLASSES[]={
    {"DSPost",0x0327F920ull,0x012EACC0ull,0x01287400ull},
    {"DSSafetyHouse",0x0327F460ull,0x01307E10ull,0x013084A0ull},
    {"DSWatchTower",0x032820F8ull,0x012455B0ull,0x01287400ull},
    {"DSCatapult",0x03283E08ull,0x013123F0ull,0x01287400ull},
    {"DSCharger",0x03284108ull,0x012EACC0ull,0x01287400ull},
    {"DSRainShelter",0x032851C8ull,0x012EACC0ull,0x01287400ull},
    {"DSZipline",0x032824A8ull,0x0124AAC0ull,0x01287400ull},
    {"DSBridge",0x032838E8ull,0x0130F3F0ull,0x0130E3C0ull},
    {"DSChiralBridge",0x03285978ull,0x0131D460ull,0x01287400ull},
    {"DSJumpStand",0x03285580ull,0x012EEB10ull,0x012EEE10ull},
    {"DSFastTravelStation",0x032816D8ull,0x012EACC0ull,0x01287400ull}
};
static const u32 HOOK_CLASS_COUNT=(u32)(sizeof(HOOK_CLASSES)/sizeof(HOOK_CLASSES[0]));
static const u8 TARGET_HOOK_CLASS[]={0u,1u,2u,3u,4u,5u,6u,7u,7u,7u,8u,9u,10u};
static const u32 GIFT_MARKER_COUNT=4096u;
static const u32 GIFT_MARKER_PROBES=8u;
static_assert(sizeof(TARGET_HOOK_CLASS)/sizeof(TARGET_HOOK_CLASS[0])==TARGET_COUNT,"target/class map mismatch");
static_assert((GIFT_MARKER_COUNT&(GIFT_MARKER_COUNT-1u))==0u,"gift marker count must be a power of two");

enum ResolveResult{RESOLVE_NOT_READY,RESOLVE_INVALID,RESOLVE_READY};

struct Observation{
    u8* manager;
    u8* facility;
    u8* config;
    u32 stablePolls;
};

typedef void (__fastcall* SetLevelFn)(void*,u32);
typedef void (__fastcall* ConstructionInitFn)(void*,void*);
typedef void (__fastcall* ConstructionUpdateFn)(void*,float,u8);
typedef u32 (__fastcall* EffectiveMaxLevelFn)(void*);

enum GiftPhase{
    GIFT_PHASE_NONE=0,
    GIFT_PHASE_CANDIDATE=1,
    GIFT_PHASE_ARMED=2,
    GIFT_PHASE_WAIT_ACK=3,
    GIFT_PHASE_COMPLETE=4
};
static const u32 GIFT_FLAG_FOUNDATION_COMPLETION_PRESERVED=1u;

struct GiftMarker{
    volatile long lock;
    volatile long nativeCallInProgress;
    u32 constructionId;
    u32 targetIndex;
    u32 phase;
    u32 expectedLevel;
    u32 waitPolls;
    u32 reserved;
    u64 object;
};

static const u32 FOUNDATION_DIAGNOSTIC_SAMPLE_COUNT=8u;
struct FoundationDiagnosticSample{
    u64 callerRva;
    u64 objectVtableRva;
    u32 constructionId;
    u32 liveConstructionId;
    u32 deferredBefore;
    u32 deferredAfter;
    u32 foundationTimerBits;
    u32 markerFlags;
    u32 blockers;
    u32 hits;
    u8 updateFlags;
    u8 dispatch;
    u8 samplePoint;
    u8 phase;
    u8 lifecycle;
    u8 currentLevel;
    u8 appliedLevel;
    u8 secondaryBlocked;
    u8 materialDirty;
    u8 buildPending;
    u8 levelDirty;
    u8 buildAuxPending;
    u8 reported;
};
struct FoundationDiagnosticBank{
    volatile long lock;
    u32 used;
    u32 overflow;
    u32 overflowReported;
    FoundationDiagnosticSample samples[FOUNDATION_DIAGNOSTIC_SAMPLE_COUNT];
};

static HMODULE g_self=0;
static u8* g_gameBase=0;
static HANDLE g_log=INVALID_HANDLE_VALUE;
static bool g_debugLog=false;
static volatile long g_started=0;
static volatile u32 g_installedSetLevelHookMask=0;
static volatile u32 g_installedInitHookMask=0;
static volatile u32 g_installedUpdateHookMask=0;
static volatile u64 g_allowedConfigs[TARGET_COUNT];
static volatile long g_configBatchPublished=0;
static volatile long g_candidatesTracked[TARGET_COUNT];
static volatile long g_levelStepsStarted[TARGET_COUNT];
static volatile long g_foundationStepsStarted[TARGET_COUNT];
static volatile long g_promotions[TARGET_COUNT];
static volatile long g_downgradesPrevented[TARGET_COUNT];
static volatile long g_promotionFailures[TARGET_COUNT];
static volatile long g_initHookObserved=0;
static volatile long g_updateHookObserved=0;
static volatile long g_vtableProtectionRestoreFailed=0;
__declspec(align(64)) static GiftMarker g_giftMarkers[GIFT_MARKER_COUNT];
__declspec(align(64)) static FoundationDiagnosticBank g_foundationDiagnostics[TARGET_COUNT];
#ifdef DS2_TESTING
static u8* g_testResolvedConfig=0;
static SetLevelFn g_testSetLevel=0;
static ConstructionInitFn g_testConstructionInit=0;
static ConstructionUpdateFn g_testConstructionUpdate=0;
static EffectiveMaxLevelFn g_testEffectiveMaxLevel=0;
static bool g_testMaterialCaller=true;
static s32 g_testHookClass=0;
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
static bool equalsIgnoreCase(const WCHAR* left,const WCHAR* right){
    if(!left||!right)return left==right;
    while(*left&&*right){
        if(lowerAscii(*left)!=lowerAscii(*right))return false;
        left++;right++;
    }
    return *left==0&&*right==0;
}
static void modulePath(WCHAR* out,const WCHAR* name){
    if(!out||!name)return;
    out[0]=0;
    DWORD length=GetModuleFileNameW(g_self,out,520);
    if(!length||length>=520){out[0]=0;return;}
    DWORD slash=0;
    for(DWORD i=0;i<length;i++)if(out[i]==L'\\'||out[i]==L'/')slash=i+1u;
    DWORD index=0;
    while(name[index]&&slash+index+1u<520u){out[slash+index]=name[index];index++;}
    if(name[index]){out[0]=0;return;}
    out[slash+index]=0;
}
static bool isDs2Process(HMODULE game){
    WCHAR path[520];
    DWORD length=GetModuleFileNameW(game,path,520);
    if(!length||length>=520)return false;
    DWORD start=0;
    for(DWORD i=0;i<length;i++)if(path[i]==L'\\'||path[i]==L'/')start=i+1u;
    return equalsIgnoreCase(path+start,L"DS2.exe");
}
static bool readBoolSetting(const WCHAR* key,bool fallback){
    WCHAR path[520];
    modulePath(path,L"ds2_construction_max_level_on_build.ini");
    if(!path[0])return fallback;
    WCHAR value[32];
    DWORD length=GetPrivateProfileStringW(
        L"ConstructionMaxLevelOnBuild",key,fallback?L"1":L"0",value,32,path
    );
    if(!length)return fallback;
    if(equalsIgnoreCase(value,L"1")||equalsIgnoreCase(value,L"true")||
       equalsIgnoreCase(value,L"yes")||equalsIgnoreCase(value,L"on"))return true;
    if(equalsIgnoreCase(value,L"0")||equalsIgnoreCase(value,L"false")||
       equalsIgnoreCase(value,L"no")||equalsIgnoreCase(value,L"off"))return false;
    return fallback;
}

static void openLog(){
    if(g_log!=INVALID_HANDLE_VALUE)return;
    WCHAR path[520];
    modulePath(path,L"ds2_construction_max_level_on_build.log");
    if(!path[0])return;
    g_log=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(g_log!=INVALID_HANDLE_VALUE){
        const char header[]=
            "DS2 Construction Max Level on Build " MOD_VERSION "\r\n"
            "Expected target: Steam PC 1.10.89.0\r\n";
        DWORD written=0;
        WriteFile(g_log,header,(DWORD)(sizeof(header)-1u),&written,0);
    }
}
static void closeLog(){
    if(g_log==INVALID_HANDLE_VALUE)return;
    FlushFileBuffers(g_log);
    CloseHandle(g_log);
    g_log=INVALID_HANDLE_VALUE;
}
static void logRaw(const char* text,DWORD length){
    if(!text||!length)return;
    openLog();
    if(g_log==INVALID_HANDLE_VALUE)return;
    DWORD written=0;
    WriteFile(g_log,text,length,&written,0);
    FlushFileBuffers(g_log);
}
static void logCritical(const char* text){
    DWORD length=0;
    while(text&&text[length])length++;
    logRaw(text,length);
}
static void logDebug(const char* text){if(g_debugLog)logCritical(text);}
static u32 appendText(char* out,u32 position,u32 capacity,const char* text){
    if(!out||!text||!capacity)return position;
    while(*text&&position+1u<capacity)out[position++]=*text++;
    return position;
}
static u32 appendU32Decimal(char* out,u32 position,u32 capacity,u32 value){
    char digits[10];
    u32 count=0;
    do{
        digits[count++]=(char)('0'+(value%10u));
        value/=10u;
    }while(value&&count<(u32)sizeof(digits));
    while(count&&position+1u<capacity)out[position++]=digits[--count];
    return position;
}
static u32 appendU64Hex(char* out,u32 position,u32 capacity,u64 value){
    static const char HEX[]="0123456789ABCDEF";
    char digits[16];
    u32 count=0;
    do{
        digits[count++]=HEX[(u32)(value&0xFu)];
        value>>=4u;
    }while(value&&count<(u32)sizeof(digits));
    while(count&&position+1u<capacity)out[position++]=digits[--count];
    return position;
}
static void logTarget(const char* prefix,u32 targetIndex,const char* suffix){
    if(targetIndex>=TARGET_COUNT)return;
    char line[320];
    u32 position=appendText(line,0u,(u32)sizeof(line),prefix);
    position=appendText(line,position,(u32)sizeof(line),TARGETS[targetIndex].label);
    position=appendText(line,position,(u32)sizeof(line),suffix);
    line[position]=0;
    logRaw(line,position);
}
static void logFoundationDiagnostic(
    u32 targetIndex,const FoundationDiagnosticSample* sample
){
    if(targetIndex>=TARGET_COUNT||!sample)return;
    char line[640];
    u32 p=appendText(line,0u,(u32)sizeof(line),"DIAG: ");
    p=appendText(line,p,(u32)sizeof(line),TARGETS[targetIndex].label);
    p=appendText(line,p,(u32)sizeof(line)," id=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->constructionId);
    p=appendText(line,p,(u32)sizeof(line)," liveId=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->liveConstructionId);
    p=appendText(line,p,(u32)sizeof(line)," vtable=0x");
    p=appendU64Hex(line,p,(u32)sizeof(line),sample->objectVtableRva);
    p=appendText(line,p,(u32)sizeof(line)," flags=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->updateFlags);
    p=appendText(line,p,(u32)sizeof(line)," caller=0x");
    p=appendU64Hex(line,p,(u32)sizeof(line),sample->callerRva);
    p=appendText(line,p,(u32)sizeof(line)," dispatch=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->dispatch);
    p=appendText(line,p,(u32)sizeof(line)," point=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->samplePoint);
    p=appendText(line,p,(u32)sizeof(line)," phase=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->phase);
    p=appendText(line,p,(u32)sizeof(line)," markerFlags=0x");
    p=appendU64Hex(line,p,(u32)sizeof(line),sample->markerFlags);
    p=appendText(line,p,(u32)sizeof(line)," life=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->lifecycle);
    p=appendText(line,p,(u32)sizeof(line)," level=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->currentLevel);
    p=appendText(line,p,(u32)sizeof(line)," applied=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->appliedLevel);
    p=appendText(line,p,(u32)sizeof(line)," cb=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->deferredBefore);
    p=appendText(line,p,(u32)sizeof(line),"/");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->deferredAfter);
    p=appendText(line,p,(u32)sizeof(line)," secblk=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->secondaryBlocked);
    p=appendText(line,p,(u32)sizeof(line)," dirty[M=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->materialDirty);
    p=appendText(line,p,(u32)sizeof(line)," B=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->buildPending);
    p=appendText(line,p,(u32)sizeof(line)," L=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->levelDirty);
    p=appendText(line,p,(u32)sizeof(line)," A=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->buildAuxPending);
    p=appendText(line,p,(u32)sizeof(line),"] timer=0x");
    p=appendU64Hex(line,p,(u32)sizeof(line),sample->foundationTimerBits);
    p=appendText(line,p,(u32)sizeof(line)," blockers=0x");
    p=appendU64Hex(line,p,(u32)sizeof(line),sample->blockers);
    p=appendText(line,p,(u32)sizeof(line)," hits=");
    p=appendU32Decimal(line,p,(u32)sizeof(line),sample->hits);
    p=appendText(line,p,(u32)sizeof(line),"\r\n");
    line[p]=0;
    logRaw(line,p);
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
static bool readU8(const void* pointer,u8* value){
    if(!value||!rangeReadable(pointer,1u))return false;
    *value=*(volatile const u8*)pointer;
    return true;
}
static bool readU32(const void* pointer,u32* value){
    if(!value||!rangeReadable(pointer,4u))return false;
    *value=*(volatile const u32*)pointer;
    return true;
}
static bool readU64(const void* pointer,u64* value){
    if(!value||!rangeReadable(pointer,8u))return false;
    *value=*(volatile const u64*)pointer;
    return true;
}
static void incrementPromotion(volatile long* value){
    for(;;){
        long before=_InterlockedCompareExchange(value,0,0);
        if(_InterlockedCompareExchange(value,before+1,before)==before)return;
    }
}
static void atomicStoreLong(volatile long* target,long value){
    for(;;){
        long before=_InterlockedCompareExchange(target,0,0);
        if(_InterlockedCompareExchange(target,value,before)==before)return;
    }
}
static u64 atomicLoadU64(volatile u64* target){
    return (u64)_InterlockedCompareExchange64((volatile s64*)target,0,0);
}
static void atomicStoreU64(volatile u64* target,u64 value){
    for(;;){
        u64 before=atomicLoadU64(target);
        if((u64)_InterlockedCompareExchange64(
            (volatile s64*)target,(s64)value,(s64)before
        )==before)return;
    }
}

static bool readBuildMetadata(const u8* base,u32* timestamp,u32* imageSize){
    if(!base||!timestamp||!imageSize||!rangeReadable(base,0x1000u))return false;
    if(readU16Raw(base)!=0x5A4Du)return false;
    u32 pe=readU32Raw(base+0x3C);
    if(pe<0x40u||pe>=0x2000u||readU32Raw(base+pe)!=0x00004550u)return false;
    if(readU16Raw(base+pe+4u)!=0x8664u)return false;
    if(readU16Raw(base+pe+20u)<0x00F0u||readU16Raw(base+pe+24u)!=0x020Bu)return false;
    *timestamp=readU32Raw(base+pe+8u);
    *imageSize=readU32Raw(base+pe+0x50u);
    return true;
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
    for(u32 i=0;i<TARGET_COUNT;i++){
        if(!validateVtableEntries(base,TARGETS[i].configVtableRva,TARGETS[i].vfuncs,4u))return false;
    }
    return true;
}
static bool validateObjectHookSlots(const u8* base){
    if(!base)return false;
    for(u32 i=0;i<HOOK_CLASS_COUNT;i++){
        u8* initSlot=(u8*)base+HOOK_CLASSES[i].objectVtableRva+OFF_VTABLE_INIT;
        u8* updateSlot=(u8*)base+HOOK_CLASSES[i].objectVtableRva+OFF_VTABLE_UPDATE;
        u8* setLevelSlot=(u8*)base+HOOK_CLASSES[i].objectVtableRva+OFF_VTABLE_SET_LEVEL;
        u64 initValue=0,updateValue=0,setLevelValue=0;
        if(((u64)initSlot&7u)!=0u||((u64)updateSlot&7u)!=0u||
           ((u64)setLevelSlot&7u)!=0u||!readU64(initSlot,&initValue)||
           !readU64(updateSlot,&updateValue)||!readU64(setLevelSlot,&setLevelValue)||
           initValue!=(u64)(base+HOOK_CLASSES[i].originalInitRva)||
           updateValue!=(u64)(base+RVA_CONSTRUCTION_UPDATE)||
           setLevelValue!=(u64)(base+HOOK_CLASSES[i].originalSetLevelRva))return false;
    }
    return true;
}
static bool validateObjectVtable(const void* object,const u8* base,u64 expectedRva){
    u64 vtable=0;
    return readU64(object,&vtable)&&vtable==(u64)(base+expectedRva);
}
static bool validateRuntimeCode(const u8* base){
    if(!base)return false;
    return bytesEqual(base+RVA_CONSTRUCTION_INIT,EXPECTED_CONSTRUCTION_INIT,(u32)sizeof(EXPECTED_CONSTRUCTION_INIT))&&
           bytesEqual(base+RVA_INIT_DISPATCH_CONTEXT_A,EXPECTED_INIT_DISPATCH_CONTEXT_A,(u32)sizeof(EXPECTED_INIT_DISPATCH_CONTEXT_A))&&
           bytesEqual(base+RVA_INIT_DISPATCH_CONTEXT_B,EXPECTED_INIT_DISPATCH_CONTEXT_B,(u32)sizeof(EXPECTED_INIT_DISPATCH_CONTEXT_B))&&
           bytesEqual(base+RVA_CONSTRUCTION_UPDATE,EXPECTED_CONSTRUCTION_UPDATE,(u32)sizeof(EXPECTED_CONSTRUCTION_UPDATE))&&
           bytesEqual(base+RVA_UPDATE_DISPATCH_CONTEXT,EXPECTED_UPDATE_DISPATCH_CONTEXT,(u32)sizeof(EXPECTED_UPDATE_DISPATCH_CONTEXT))&&
           bytesEqual(base+RVA_UPDATE_DISPATCH_CONTEXT_SECONDARY,EXPECTED_UPDATE_DISPATCH_CONTEXT_SECONDARY,(u32)sizeof(EXPECTED_UPDATE_DISPATCH_CONTEXT_SECONDARY))&&
           bytesEqual(base+RVA_MATERIAL_LEVEL_CONTEXT,EXPECTED_MATERIAL_LEVEL_CONTEXT,(u32)sizeof(EXPECTED_MATERIAL_LEVEL_CONTEXT))&&
           bytesEqual(base+RVA_MATERIAL_LEVEL_CALL,EXPECTED_MATERIAL_LEVEL_CALL,(u32)sizeof(EXPECTED_MATERIAL_LEVEL_CALL))&&
           bytesEqual(base+RVA_EFFECTIVE_MAX_LEVEL,EXPECTED_EFFECTIVE_MAX_LEVEL,(u32)sizeof(EXPECTED_EFFECTIVE_MAX_LEVEL))&&
           bytesEqual(base+RVA_LEVEL_ASSIGN_CONTEXT,EXPECTED_LEVEL_ASSIGN_CONTEXT,(u32)sizeof(EXPECTED_LEVEL_ASSIGN_CONTEXT))&&
           bytesEqual(base+RVA_COMPLETION_COMPARE,EXPECTED_COMPLETION_COMPARE,(u32)sizeof(EXPECTED_COMPLETION_COMPARE))&&
           bytesEqual(base+RVA_CUSTOMIZABLE_COMPARE,EXPECTED_CUSTOMIZABLE_COMPARE,(u32)sizeof(EXPECTED_CUSTOMIZABLE_COMPARE))&&
           bytesEqual(base+0x0127B4F0ull,EXPECTED_SPAWN_LEVEL_SEED,(u32)sizeof(EXPECTED_SPAWN_LEVEL_SEED))&&
           bytesEqual(base+0x0127B537ull,EXPECTED_NET_TYPE_COPY,(u32)sizeof(EXPECTED_NET_TYPE_COPY))&&
           bytesEqual(base+RVA_INIT_PENDING_COPIES,EXPECTED_INIT_PENDING_COPIES,(u32)sizeof(EXPECTED_INIT_PENDING_COPIES))&&
           bytesEqual(base+RVA_PCC_FOUNDATION_PENDING_DESCRIPTOR,EXPECTED_PCC_FOUNDATION_PENDING_DESCRIPTOR,(u32)sizeof(EXPECTED_PCC_FOUNDATION_PENDING_DESCRIPTOR))&&
           bytesEqual(base+RVA_FOUNDATION_STATE_ENTRY,EXPECTED_FOUNDATION_STATE_ENTRY,(u32)sizeof(EXPECTED_FOUNDATION_STATE_ENTRY))&&
           bytesEqual(base+RVA_PLAYER_FOUNDATION_STATE_SELECT,EXPECTED_PLAYER_FOUNDATION_STATE_SELECT,(u32)sizeof(EXPECTED_PLAYER_FOUNDATION_STATE_SELECT))&&
           bytesEqual(base+RVA_FOUNDATION_PENDING_TIMER_GATE,EXPECTED_FOUNDATION_PENDING_TIMER_GATE,(u32)sizeof(EXPECTED_FOUNDATION_PENDING_TIMER_GATE))&&
           bytesEqual(base+RVA_FOUNDATION_DWELL_SECONDS,EXPECTED_FOUNDATION_DWELL_SECONDS,(u32)sizeof(EXPECTED_FOUNDATION_DWELL_SECONDS))&&
           bytesEqual(base+RVA_STREAMING_ACTIVE_ATTACH,EXPECTED_STREAMING_ACTIVE_ATTACH,(u32)sizeof(EXPECTED_STREAMING_ACTIVE_ATTACH))&&
           bytesEqual(base+RVA_SECONDARY_UPDATE_OBJECT_GATE,EXPECTED_SECONDARY_UPDATE_OBJECT_GATE,(u32)sizeof(EXPECTED_SECONDARY_UPDATE_OBJECT_GATE))&&
           bytesEqual(base+RVA_LIFECYCLE_ACTIVE_TRANSITION,EXPECTED_LIFECYCLE_ACTIVE_TRANSITION,(u32)sizeof(EXPECTED_LIFECYCLE_ACTIVE_TRANSITION))&&
           bytesEqual(base+RVA_UPDATE_LEVEL_DIRTY,EXPECTED_UPDATE_LEVEL_DIRTY,(u32)sizeof(EXPECTED_UPDATE_LEVEL_DIRTY))&&
           bytesEqual(base+RVA_UPDATE_MATERIAL_DIRTY,EXPECTED_UPDATE_MATERIAL_DIRTY,(u32)sizeof(EXPECTED_UPDATE_MATERIAL_DIRTY))&&
           bytesEqual(base+RVA_MATERIAL_DIRTY_REARM,EXPECTED_MATERIAL_DIRTY_REARM,(u32)sizeof(EXPECTED_MATERIAL_DIRTY_REARM))&&
           bytesEqual(base+RVA_SETLEVEL_PENDING_CHECK,EXPECTED_SETLEVEL_PENDING_CHECK,(u32)sizeof(EXPECTED_SETLEVEL_PENDING_CHECK))&&
           bytesEqual(base+RVA_SETLEVEL_DIRTY_SET,EXPECTED_SETLEVEL_DIRTY_SET,(u32)sizeof(EXPECTED_SETLEVEL_DIRTY_SET))&&
           bytesEqual(base+RVA_LEVEL_TERMINAL_ACK,EXPECTED_LEVEL_TERMINAL_ACK,(u32)sizeof(EXPECTED_LEVEL_TERMINAL_ACK))&&
           bytesEqual(base+RVA_DEFERRED_CALLBACK_QUEUE,EXPECTED_DEFERRED_CALLBACK_QUEUE,(u32)sizeof(EXPECTED_DEFERRED_CALLBACK_QUEUE))&&
           bytesEqual(base+RVA_DEFERRED_CALLBACK_DRAIN_CHECK,EXPECTED_DEFERRED_CALLBACK_DRAIN_CHECK,(u32)sizeof(EXPECTED_DEFERRED_CALLBACK_DRAIN_CHECK))&&
           bytesEqual(base+RVA_DEFERRED_CALLBACK_DRAIN_REMOVE,EXPECTED_DEFERRED_CALLBACK_DRAIN_REMOVE,(u32)sizeof(EXPECTED_DEFERRED_CALLBACK_DRAIN_REMOVE))&&
           bytesEqual(base+RVA_MANAGER_CALLBACK_OBJECT_QUEUE,EXPECTED_MANAGER_CALLBACK_OBJECT_QUEUE,(u32)sizeof(EXPECTED_MANAGER_CALLBACK_OBJECT_QUEUE))&&
           bytesEqual(base+RVA_MANAGER_CALLBACK_ENQUEUE_PRIMARY,EXPECTED_MANAGER_CALLBACK_ENQUEUE_PRIMARY,(u32)sizeof(EXPECTED_MANAGER_CALLBACK_ENQUEUE_PRIMARY))&&
           bytesEqual(base+RVA_MANAGER_CALLBACK_ENQUEUE_SECONDARY,EXPECTED_MANAGER_CALLBACK_ENQUEUE_SECONDARY,(u32)sizeof(EXPECTED_MANAGER_CALLBACK_ENQUEUE_SECONDARY))&&
           bytesEqual(base+RVA_APPLIED_ACK_CALLBACK_PRIMARY,EXPECTED_APPLIED_ACK_CALLBACK_PRIMARY,(u32)sizeof(EXPECTED_APPLIED_ACK_CALLBACK_PRIMARY))&&
           bytesEqual(base+RVA_APPLIED_ACK_CALLBACK_SECONDARY,EXPECTED_APPLIED_ACK_CALLBACK_SECONDARY,(u32)sizeof(EXPECTED_APPLIED_ACK_CALLBACK_SECONDARY));
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
    if(!base||!manager||targetIndex>=TARGET_COUNT||!managerOffset||!facilityOut||!configOut){
        return RESOLVE_INVALID;
    }
    *facilityOut=0;*configOut=0;
    u64 facility=0,config=0;
    if(!readU64(manager+managerOffset,&facility))return RESOLVE_INVALID;
    if(!facility)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)facility,base,RVA_FACILITY_VTABLE))return RESOLVE_INVALID;
    if(!readU64((u8*)facility+OFF_FACILITY_CONFIG,&config))return RESOLVE_INVALID;
    if(!config)return RESOLVE_NOT_READY;
    if(!validateObjectVtable((void*)config,base,TARGETS[targetIndex].configVtableRva))return RESOLVE_INVALID;
    if(!rangeReadable((u8*)config+OFF_MAX_LEVEL,4u)||
       !rangeReadable((u8*)config+OFF_COMPLETION_LEVEL,8u))return RESOLVE_INVALID;
    *facilityOut=(u8*)facility;
    *configOut=(u8*)config;
    return RESOLVE_READY;
}
static ResolveResult resolveTarget(
    u8* base,u8* manager,u32 targetIndex,u8** facilityOut,u8** configOut
){
    ResolveResult primary=resolveTargetAtOffset(
        base,manager,targetIndex,TARGETS[targetIndex].managerOffset,facilityOut,configOut
    );
    if(primary!=RESOLVE_NOT_READY||!TARGETS[targetIndex].alternateManagerOffset)return primary;
    return resolveTargetAtOffset(
        base,manager,targetIndex,TARGETS[targetIndex].alternateManagerOffset,
        facilityOut,configOut
    );
}

static void clearAllowedConfigs(){
    atomicStoreLong(&g_configBatchPublished,0);
    for(u32 i=0;i<TARGET_COUNT;i++)atomicStoreU64(&g_allowedConfigs[i],0);
}
static bool validateObservationBatch(const Observation* observations){
    if(!observations)return false;
    for(u32 i=0;i<TARGET_COUNT;i++){
        if(observations[i].stablePolls<2u||!observations[i].config)return false;
        u32 maxLevel=0,completionLevel=0,customizableLevel=0;
        if(!readU32(observations[i].config+OFF_MAX_LEVEL,&maxLevel)||
           !readU32(observations[i].config+OFF_COMPLETION_LEVEL,&completionLevel)||
           !readU32(observations[i].config+OFF_CUSTOMIZABLE_LEVEL,&customizableLevel))return false;
        const TargetDefinition* definition=&TARGETS[i];
        if(maxLevel!=definition->nativeMaxLevel||
           completionLevel!=definition->nativeCompletionLevel||
           customizableLevel!=definition->nativeCustomizableLevel)return false;
        for(u32 earlier=0;earlier<i;earlier++){
            if(observations[earlier].config==observations[i].config)return false;
        }
    }
    return true;
}
static void publishAllowedConfigs(const Observation* observations){
    if(!observations)return;
    atomicStoreLong(&g_configBatchPublished,0);
    for(u32 i=0;i<TARGET_COUNT;i++){
        atomicStoreU64(&g_allowedConfigs[i],(u64)observations[i].config);
    }
    atomicStoreLong(&g_configBatchPublished,1);
}

static bool resolveConfigForObject(void* object,u8** configOut){
    if(!object||!configOut||!g_gameBase)return false;
    *configOut=0;
#ifdef DS2_TESTING
    if(g_testResolvedConfig){
        *configOut=g_testResolvedConfig;
        return true;
    }
#endif
    u8 type=0,subtype=0;
    if(!readU8((u8*)object+OFF_OBJECT_TYPE,&type)||
       !readU8((u8*)object+OFF_OBJECT_SUBTYPE,&subtype))return false;
    u8* manager=0;
    if(resolveManager(g_gameBase,&manager)!=RESOLVE_READY)return false;
    u64 facility=0,config=0;
    if(subtype==0x0Eu){
        if(!readU64(manager+0x20u+(u64)type*8u,&facility))return false;
    }else{
        if(!readU64(manager+0x138u+(u64)subtype*8u,&facility))return false;
    }
    if(!facility||!readU64((u8*)facility+OFF_FACILITY_CONFIG,&config)||!config)return false;
    *configOut=(u8*)config;
    return true;
}
static bool resolveConfigForDescriptor(void* descriptor,u8** configOut){
    if(!descriptor||!configOut||!g_gameBase)return false;
    *configOut=0;
#ifdef DS2_TESTING
    if(g_testResolvedConfig){
        *configOut=g_testResolvedConfig;
        return true;
    }
#endif
    u8 type=0,subtype=0;
    if(!readU8((u8*)descriptor+OFF_DESCRIPTOR_TYPE,&type)||
       !readU8((u8*)descriptor+OFF_DESCRIPTOR_SUBTYPE,&subtype))return false;
    u8* manager=0;
    if(resolveManager(g_gameBase,&manager)!=RESOLVE_READY)return false;
    u64 facility=0,config=0;
    if(subtype==0x0Eu){
        if(!readU64(manager+0x20u+(u64)type*8u,&facility))return false;
    }else{
        if(!readU64(manager+0x138u+(u64)subtype*8u,&facility))return false;
    }
    if(!facility||!readU64((u8*)facility+OFF_FACILITY_CONFIG,&config)||!config)return false;
    *configOut=(u8*)config;
    return true;
}
static s32 findAllowedConfig(u8* config){
    if(!config||_InterlockedCompareExchange(&g_configBatchPublished,0,0)!=1)return -1;
    for(u32 i=0;i<TARGET_COUNT;i++){
        if(atomicLoadU64(&g_allowedConfigs[i])==(u64)config)return (s32)i;
    }
    return -1;
}

static bool addressIsExecutable(const void* pointer){
    if(!pointer)return false;
    MEMORY_BASIC_INFORMATION_X64 info;
    if(VirtualQuery(pointer,&info,sizeof(info))<sizeof(info)||info.State!=MEM_COMMIT)return false;
    DWORD protection=info.Protect&0xFFu;
    return !(info.Protect&PAGE_GUARD)&&
           (protection==PAGE_EXECUTE||protection==PAGE_EXECUTE_READ||
            protection==PAGE_EXECUTE_READWRITE||protection==PAGE_EXECUTE_WRITECOPY);
}
static s32 findHookClassForObject(void* object){
#ifdef DS2_TESTING
    if(g_testHookClass>=0)return g_testHookClass;
#endif
    u64 vtable=0;
    if(!object||!g_gameBase||!readU64(object,&vtable))return -1;
    for(u32 i=0;i<HOOK_CLASS_COUNT;i++){
        if(vtable==(u64)(g_gameBase+HOOK_CLASSES[i].objectVtableRva))return (s32)i;
    }
    return -1;
}
static bool callOriginalSetLevel(void* object,u32 level){
#ifdef DS2_TESTING
    if(g_testSetLevel){g_testSetLevel(object,level);return true;}
#endif
    s32 classIndex=findHookClassForObject(object);
    if(classIndex<0||!g_gameBase)return false;
    SetLevelFn function=(SetLevelFn)(g_gameBase+HOOK_CLASSES[(u32)classIndex].originalSetLevelRva);
    if(!addressIsExecutable((void*)function))return false;
    function(object,level);
    return true;
}
static bool callOriginalConstructionInit(void* object,void* descriptor){
#ifdef DS2_TESTING
    if(g_testConstructionInit){
        g_testConstructionInit(object,descriptor);
        return true;
    }
#endif
    s32 classIndex=findHookClassForObject(object);
    if(classIndex<0||!g_gameBase)return false;
    ConstructionInitFn function=(ConstructionInitFn)(
        g_gameBase+HOOK_CLASSES[(u32)classIndex].originalInitRva
    );
    if(!addressIsExecutable((void*)function))return false;
    function(object,descriptor);
    return true;
}
static bool callOriginalConstructionUpdate(void* object,float elapsedSeconds,u8 updateFlags){
#ifdef DS2_TESTING
    if(g_testConstructionUpdate){
        g_testConstructionUpdate(object,elapsedSeconds,updateFlags);
        return true;
    }
#endif
    if(!g_gameBase)return false;
    ConstructionUpdateFn function=(ConstructionUpdateFn)(g_gameBase+RVA_CONSTRUCTION_UPDATE);
    function(object,elapsedSeconds,updateFlags);
    return true;
}
static bool readEffectiveMaxLevel(u8* config,u32* levelOut){
    if(!config||!levelOut)return false;
#ifdef DS2_TESTING
    if(g_testEffectiveMaxLevel){
        *levelOut=g_testEffectiveMaxLevel(config);
        return *levelOut>0u&&*levelOut<32u;
    }
#endif
    if(!g_gameBase)return false;
    EffectiveMaxLevelFn function=(EffectiveMaxLevelFn)(g_gameBase+RVA_EFFECTIVE_MAX_LEVEL);
    if(!addressIsExecutable((void*)function))return false;
    *levelOut=function(config);
    return *levelOut>0u&&*levelOut<32u;
}

static u32 giftMarkerStart(void* object){
    u64 value=(u64)object;
    return (u32)(((value>>4u)^(value>>13u)^(value>>22u))&(GIFT_MARKER_COUNT-1u));
}
static bool tryLockGiftMarker(GiftMarker* marker){
    return marker&&_InterlockedCompareExchange(&marker->lock,1,0)==0;
}
static void unlockGiftMarker(GiftMarker* marker){
    if(marker)_InterlockedCompareExchange(&marker->lock,0,1);
}
static void resetLockedGiftMarker(GiftMarker* marker){
    if(!marker)return;
    atomicStoreLong(&marker->nativeCallInProgress,0);
    marker->object=0;
    marker->constructionId=0;
    marker->targetIndex=0;
    marker->phase=GIFT_PHASE_NONE;
    marker->expectedLevel=0;
    marker->waitPolls=0;
    marker->reserved=0;
}
static void clearGiftMarkerForObject(void* object){
    if(!object)return;
    u32 start=giftMarkerStart(object);
    for(u32 probe=0;probe<GIFT_MARKER_PROBES;probe++){
        GiftMarker* marker=&g_giftMarkers[(start+probe)&(GIFT_MARKER_COUNT-1u)];
        if(!tryLockGiftMarker(marker))continue;
        if(marker->object==(u64)object)resetLockedGiftMarker(marker);
        unlockGiftMarker(marker);
    }
}
static bool claimGiftMarker(
    void* object,u32 constructionId,u32 targetIndex,u32 phase,u32 expectedLevel,u32 flags
){
    if(!object||targetIndex>=TARGET_COUNT)return false;
    u32 start=giftMarkerStart(object);
    for(u32 probe=0;probe<GIFT_MARKER_PROBES;probe++){
        GiftMarker* marker=&g_giftMarkers[(start+probe)&(GIFT_MARKER_COUNT-1u)];
        if(!tryLockGiftMarker(marker))return false;
        if(!marker->object||marker->object==(u64)object){
            marker->constructionId=constructionId;
            marker->targetIndex=targetIndex;
            marker->phase=phase;
            marker->expectedLevel=expectedLevel;
            marker->waitPolls=0;
            marker->reserved=flags;
            atomicStoreLong(&marker->nativeCallInProgress,0);
            marker->object=(u64)object;
            unlockGiftMarker(marker);
            return true;
        }
        unlockGiftMarker(marker);
    }
    return false;
}
static GiftMarker* lockMatchingGiftMarker(
    void* object,u32 constructionId,u32 targetIndex
){
    if(!object||targetIndex>=TARGET_COUNT)return 0;
    u32 start=giftMarkerStart(object);
    for(u32 probe=0;probe<GIFT_MARKER_PROBES;probe++){
        GiftMarker* marker=&g_giftMarkers[(start+probe)&(GIFT_MARKER_COUNT-1u)];
        if(!tryLockGiftMarker(marker))return 0;
        if(marker->object==(u64)object&&marker->constructionId==constructionId&&
           marker->targetIndex==targetIndex)return marker;
        unlockGiftMarker(marker);
    }
    return 0;
}
static bool readGiftMarkerState(
    void* object,u32 constructionId,u32 targetIndex,u32* phaseOut,u32* flagsOut
){
    if(!phaseOut||!flagsOut)return false;
    *phaseOut=GIFT_PHASE_NONE;
    *flagsOut=0u;
    GiftMarker* marker=lockMatchingGiftMarker(object,constructionId,targetIndex);
    if(!marker)return false;
    *phaseOut=marker->phase;
    *flagsOut=marker->reserved;
    unlockGiftMarker(marker);
    return true;
}

static bool giftPromotionCallInProgress(
    void* object,u32 constructionId,u32 targetIndex
){
    if(!object||targetIndex>=TARGET_COUNT)return false;
    u32 start=giftMarkerStart(object);
    for(u32 probe=0;probe<GIFT_MARKER_PROBES;probe++){
        GiftMarker* marker=&g_giftMarkers[(start+probe)&(GIFT_MARKER_COUNT-1u)];
        if(_InterlockedCompareExchange(&marker->nativeCallInProgress,0,0)==0)continue;
        u64 markerObject=atomicLoadU64((volatile u64*)&marker->object);
        u32 markerConstruction=(u32)_InterlockedCompareExchange(
            (volatile long*)&marker->constructionId,0,0
        );
        u32 markerTarget=(u32)_InterlockedCompareExchange(
            (volatile long*)&marker->targetIndex,0,0
        );
        if(markerObject==(u64)object&&markerConstruction==constructionId&&
           markerTarget==targetIndex&&
           _InterlockedCompareExchange(&marker->nativeCallInProgress,0,0)!=0){
            return true;
        }
    }
    return false;
}

struct ValidatedTargetMetadata{
    u32 targetIndex;
    u32 completionLevel;
    u32 effectiveMaxLevel;
};
static bool readValidatedTargetMetadata(
    void* object,u8* config,ValidatedTargetMetadata* stateOut
){
    if(!object||!config||!stateOut)return false;
    s32 targetIndex=findAllowedConfig(config);
    if(targetIndex<0)return false;
    const TargetDefinition* definition=&TARGETS[(u32)targetIndex];
    s32 hookClass=findHookClassForObject(object);
    u32 rawMax=0,effectiveMax=0,completionLevel=0,customizableLevel=0;
    if(hookClass!=(s32)TARGET_HOOK_CLASS[(u32)targetIndex]||
       !readU32(config+OFF_MAX_LEVEL,&rawMax)||
       !readU32(config+OFF_COMPLETION_LEVEL,&completionLevel)||
       !readU32(config+OFF_CUSTOMIZABLE_LEVEL,&customizableLevel)||
       !readEffectiveMaxLevel(config,&effectiveMax)||
       rawMax!=definition->nativeMaxLevel||
       effectiveMax!=definition->nativeMaxLevel||
       completionLevel!=definition->nativeCompletionLevel||
       customizableLevel!=definition->nativeCustomizableLevel||
       completionLevel>=effectiveMax)return false;
    stateOut->targetIndex=(u32)targetIndex;
    stateOut->completionLevel=completionLevel;
    stateOut->effectiveMaxLevel=effectiveMax;
    return true;
}

struct TargetInitState{
    ValidatedTargetMetadata metadata;
    u32 constructionId;
    u8 initialLevel;
};
static bool readTargetInitState(void* object,void* descriptor,TargetInitState* stateOut){
    if(!object||!descriptor||!stateOut)return false;
    u8* config=0;
    u8 netType=0,initialLevel=0;
    u32 constructionId=0;
    if(!resolveConfigForDescriptor(descriptor,&config)||
       !readU8((u8*)descriptor+OFF_DESCRIPTOR_NET_TYPE,&netType)||
       !readU8((u8*)descriptor+OFF_DESCRIPTOR_LEVEL,&initialLevel)||
       !readU32((u8*)descriptor+OFF_DESCRIPTOR_CONSTRUCTION_ID,&constructionId)||
       netType!=CONSTRUCTION_NET_TYPE_PLAYER||
       !readValidatedTargetMetadata(object,config,&stateOut->metadata))return false;
    stateOut->constructionId=constructionId;
    stateOut->initialLevel=initialLevel;
    return true;
}

struct TargetRuntimeState{
    ValidatedTargetMetadata metadata;
    u32 constructionId;
    u8 currentLevel;
};
static bool readTargetRuntimeState(void* object,TargetRuntimeState* stateOut){
    if(!object||!stateOut)return false;
    u8* config=0;
    u8 currentLevel=0,netType=0;
    u32 constructionId=0;
    if(!resolveConfigForObject(object,&config)||
       !readU8((u8*)object+OFF_OBJECT_LEVEL,&currentLevel)||
       !readU8((u8*)object+OFF_OBJECT_NET_TYPE,&netType)||
       !readU32((u8*)object+OFF_OBJECT_CONSTRUCTION_ID,&constructionId)||
       netType!=CONSTRUCTION_NET_TYPE_PLAYER||
       !readValidatedTargetMetadata(object,config,&stateOut->metadata))return false;
    stateOut->constructionId=constructionId;
    stateOut->currentLevel=currentLevel;
    return true;
}

static bool validateInitializedLevel(
    void* object,const TargetInitState* state,u32 expectedLevel
){
    if(!object||!state)return false;
    u8 netType=0,currentLevel=0,appliedLevel=0;
    u32 constructionId=0;
    return readU8((u8*)object+OFF_OBJECT_NET_TYPE,&netType)&&
           readU8((u8*)object+OFF_OBJECT_LEVEL,&currentLevel)&&
           readU8((u8*)object+OFF_OBJECT_APPLIED_LEVEL,&appliedLevel)&&
           readU32((u8*)object+OFF_OBJECT_CONSTRUCTION_ID,&constructionId)&&
           netType==CONSTRUCTION_NET_TYPE_PLAYER&&
           constructionId==state->constructionId&&
           currentLevel==expectedLevel&&appliedLevel==expectedLevel;
}

static void __fastcall hookedConstructionInit(void* object,void* descriptor){
    if(g_initHookObserved==0)_InterlockedCompareExchange(&g_initHookObserved,1,0);
    clearGiftMarkerForObject(object);
    TargetInitState state;
    bool eligible=readTargetInitState(object,descriptor,&state)&&
                  state.initialLevel>0u&&
                  state.initialLevel<=state.metadata.effectiveMaxLevel;
    bool called=callOriginalConstructionInit(object,descriptor);
    if(!eligible||!called||!validateInitializedLevel(object,&state,state.initialLevel))return;
    u32 phase=state.initialLevel==state.metadata.effectiveMaxLevel?
        GIFT_PHASE_COMPLETE:GIFT_PHASE_CANDIDATE;
    u32 flags=state.metadata.completionLevel==2u&&
              state.initialLevel>=state.metadata.completionLevel?
        GIFT_FLAG_FOUNDATION_COMPLETION_PRESERVED:0u;
    if(claimGiftMarker(
        object,state.constructionId,state.metadata.targetIndex,
        phase,state.initialLevel,flags
    ))incrementPromotion(&g_candidatesTracked[state.metadata.targetIndex]);
}

struct ConstructionGateSnapshot{
    u32 deferredCallbackCountBefore;
    u32 deferredCallbackCountAfter;
    u32 foundationTimerBits;
    u8 lifecycleState;
    u8 currentLevel;
    u8 appliedLevel;
    u8 secondaryUpdateBlocked;
    u8 materialDirty;
    u8 buildPending;
    u8 levelDirty;
    u8 buildAuxPending;
};
static bool readConstructionGate(void* object,ConstructionGateSnapshot* gate){
    if(!object||!gate)return false;
    return readU32((u8*)object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT,
                   &gate->deferredCallbackCountBefore)&&
           readU8((u8*)object+OFF_OBJECT_LIFECYCLE_STATE,&gate->lifecycleState)&&
           readU8((u8*)object+OFF_OBJECT_LEVEL,&gate->currentLevel)&&
           readU8((u8*)object+OFF_OBJECT_APPLIED_LEVEL,&gate->appliedLevel)&&
           readU8((u8*)object+OFF_OBJECT_SECONDARY_UPDATE_BLOCKED,
                  &gate->secondaryUpdateBlocked)&&
           readU8((u8*)object+OFF_OBJECT_MATERIAL_DIRTY,&gate->materialDirty)&&
           readU8((u8*)object+OFF_OBJECT_BUILD_PENDING,&gate->buildPending)&&
           readU8((u8*)object+OFF_OBJECT_LEVEL_DIRTY,&gate->levelDirty)&&
           readU8((u8*)object+OFF_OBJECT_BUILD_AUX_PENDING,&gate->buildAuxPending)&&
           readU32((u8*)object+OFF_OBJECT_FOUNDATION_TIMER,
                   &gate->foundationTimerBits)&&
           readU32((u8*)object+OFF_OBJECT_DEFERRED_CALLBACK_COUNT,
                   &gate->deferredCallbackCountAfter);
}
static bool constructionGateIsStable(const ConstructionGateSnapshot* gate){
    return gate&&gate->deferredCallbackCountBefore==0u&&
           gate->deferredCallbackCountAfter==0u&&gate->lifecycleState==3u&&
           gate->currentLevel==gate->appliedLevel&&
           gate->materialDirty==0u&&gate->buildPending==0u&&
           gate->levelDirty==0u&&gate->buildAuxPending==0u;
}

enum TrackedUpdateDispatch{
    TRACKED_UPDATE_NONE=0,
    TRACKED_UPDATE_PRIMARY=1,
    TRACKED_UPDATE_FOUNDATION_SECONDARY=2
};

static bool isFoundationTarget(u32 targetIndex){
    return targetIndex==1u||(targetIndex>=7u&&targetIndex<=9u);
}

static bool foundationDwellComplete(const ConstructionGateSnapshot* gate){
    if(!gate)return false;
    bool finiteNonnegativeAtLeastPointOne=
        gate->foundationTimerBits>=FOUNDATION_DWELL_BITS&&
        gate->foundationTimerBits<=0x7F7FFFFFu;
    return gate->buildPending==0u||finiteNonnegativeAtLeastPointOne;
}

static bool foundationGateIsStable(
    const ConstructionGateSnapshot* gate,const ValidatedTargetMetadata* metadata,
    TrackedUpdateDispatch dispatch
){
    bool supportedTarget=metadata&&isFoundationTarget(metadata->targetIndex);
    bool matchingLifecycle=gate&&(
        (dispatch==TRACKED_UPDATE_FOUNDATION_SECONDARY&&
         gate->lifecycleState==2u&&gate->secondaryUpdateBlocked==0u)||
        (dispatch==TRACKED_UPDATE_PRIMARY&&gate->lifecycleState==3u)
    );
    return gate&&supportedTarget&&metadata->completionLevel==2u&&
           metadata->effectiveMaxLevel==4u&&matchingLifecycle&&
           gate->deferredCallbackCountBefore==0u&&
           gate->deferredCallbackCountAfter==0u&&
           gate->currentLevel==1u&&gate->appliedLevel==1u&&
           gate->materialDirty==0u&&gate->levelDirty==0u&&
           gate->buildAuxPending==0u&&foundationDwellComplete(gate);
}

static bool sameFoundationDiagnostic(
    const FoundationDiagnosticSample* left,const FoundationDiagnosticSample* right
){
    return left&&right&&left->callerRva==right->callerRva&&
           left->objectVtableRva==right->objectVtableRva&&
           left->constructionId==right->constructionId&&
           left->liveConstructionId==right->liveConstructionId&&
           left->deferredBefore==right->deferredBefore&&
           left->deferredAfter==right->deferredAfter&&
           left->markerFlags==right->markerFlags&&
           left->blockers==right->blockers&&
           left->updateFlags==right->updateFlags&&left->dispatch==right->dispatch&&
           left->samplePoint==right->samplePoint&&
           left->phase==right->phase&&left->lifecycle==right->lifecycle&&
           left->currentLevel==right->currentLevel&&
           left->appliedLevel==right->appliedLevel&&
           left->secondaryBlocked==right->secondaryBlocked&&
           left->materialDirty==right->materialDirty&&
           left->buildPending==right->buildPending&&
           left->levelDirty==right->levelDirty&&
           left->buildAuxPending==right->buildAuxPending;
}

static void recordFoundationDiagnostic(
    void* object,u8 updateFlags,u64 callerRva,TrackedUpdateDispatch dispatch,
    u8 samplePoint
){
    if(!object||!g_debugLog)return;
#ifndef DS2_TESTING
    if(!g_gameBase)return;
    u64 quickVtable=*(volatile const u64*)object;
    if(quickVtable!=(u64)(g_gameBase+HOOK_CLASSES[1].objectVtableRva)&&
       quickVtable!=(u64)(g_gameBase+HOOK_CLASSES[7].objectVtableRva))return;
#endif
    u32 targetIndex=TARGET_COUNT;
    u32 phase=GIFT_PHASE_NONE;
    u32 markerFlags=0u;
    u32 markerConstructionId=0u;
    u32 start=giftMarkerStart(object);
    for(u32 probe=0;probe<GIFT_MARKER_PROBES;probe++){
        GiftMarker* marker=&g_giftMarkers[(start+probe)&(GIFT_MARKER_COUNT-1u)];
        if(!tryLockGiftMarker(marker))return;
        if(marker->object==(u64)object&&marker->phase!=GIFT_PHASE_NONE){
            targetIndex=marker->targetIndex;
            phase=marker->phase;
            markerFlags=marker->reserved;
            markerConstructionId=marker->constructionId;
        }
        unlockGiftMarker(marker);
        if(targetIndex<TARGET_COUNT)break;
    }
    if(!isFoundationTarget(targetIndex))return;

    ConstructionGateSnapshot gate;
    if(!readConstructionGate(object,&gate))return;
    u32 liveConstructionId=0u;
    u64 objectVtable=0u;
    bool liveIdValid=readU32(
        (u8*)object+OFF_OBJECT_CONSTRUCTION_ID,&liveConstructionId
    );
    bool objectVtableValid=readU64(object,&objectVtable);
    TargetRuntimeState state;
    bool runtimeStateValid=readTargetRuntimeState(object,&state)&&
        state.metadata.targetIndex==targetIndex&&
        state.constructionId==markerConstructionId;

    bool matchingLifecycle=
        (dispatch==TRACKED_UPDATE_FOUNDATION_SECONDARY&&gate.lifecycleState==2u)||
        (dispatch==TRACKED_UPDATE_PRIMARY&&gate.lifecycleState==3u);
    u32 blockers=0u;
    if(dispatch==TRACKED_UPDATE_NONE)blockers|=1u<<0;
    if(!matchingLifecycle)blockers|=1u<<1;
    if(dispatch==TRACKED_UPDATE_FOUNDATION_SECONDARY&&
       gate.secondaryUpdateBlocked!=0u)blockers|=1u<<2;
    if(gate.deferredCallbackCountBefore!=0u)blockers|=1u<<3;
    if(gate.deferredCallbackCountAfter!=0u)blockers|=1u<<4;
    if(gate.currentLevel!=1u)blockers|=1u<<5;
    if(gate.appliedLevel!=1u)blockers|=1u<<6;
    if(gate.materialDirty!=0u)blockers|=1u<<7;
    if(gate.levelDirty!=0u)blockers|=1u<<8;
    if(gate.buildAuxPending!=0u)blockers|=1u<<9;
    if(!foundationDwellComplete(&gate))blockers|=1u<<10;
    if(!runtimeStateValid||!liveIdValid||
       liveConstructionId!=markerConstructionId||!objectVtableValid){
        blockers|=1u<<11;
    }
    FoundationDiagnosticSample sample;
    memset(&sample,0,sizeof(sample));
    sample.callerRva=callerRva;
    sample.objectVtableRva=objectVtableValid&&g_gameBase&&
        objectVtable>=(u64)g_gameBase&&
        objectVtable<(u64)g_gameBase+EXPECTED_IMAGE_SIZE?
        objectVtable-(u64)g_gameBase:objectVtable;
    sample.constructionId=markerConstructionId;
    sample.liveConstructionId=liveConstructionId;
    sample.deferredBefore=gate.deferredCallbackCountBefore;
    sample.deferredAfter=gate.deferredCallbackCountAfter;
    sample.foundationTimerBits=gate.foundationTimerBits;
    sample.markerFlags=markerFlags;
    sample.blockers=blockers;
    sample.hits=1u;
    sample.updateFlags=updateFlags;
    sample.dispatch=(u8)dispatch;
    sample.samplePoint=samplePoint;
    sample.phase=(u8)phase;
    sample.lifecycle=gate.lifecycleState;
    sample.currentLevel=gate.currentLevel;
    sample.appliedLevel=gate.appliedLevel;
    sample.secondaryBlocked=gate.secondaryUpdateBlocked;
    sample.materialDirty=gate.materialDirty;
    sample.buildPending=gate.buildPending;
    sample.levelDirty=gate.levelDirty;
    sample.buildAuxPending=gate.buildAuxPending;

    FoundationDiagnosticBank* bank=&g_foundationDiagnostics[targetIndex];
    if(_InterlockedCompareExchange(&bank->lock,1,0)!=0)return;
    for(u32 i=0;i<bank->used;i++){
        if(sameFoundationDiagnostic(&bank->samples[i],&sample)){
            if(bank->samples[i].hits<0xFFFFFFFFu)bank->samples[i].hits++;
            _InterlockedCompareExchange(&bank->lock,0,1);
            return;
        }
    }
    if(bank->used<FOUNDATION_DIAGNOSTIC_SAMPLE_COUNT){
        bank->samples[bank->used]=sample;
        bank->used++;
    }else if(bank->overflow<0xFFFFFFFFu){
        bank->overflow++;
    }
    _InterlockedCompareExchange(&bank->lock,0,1);
}

static void drainFoundationDiagnostics(){
    FoundationDiagnosticSample pending[FOUNDATION_DIAGNOSTIC_SAMPLE_COUNT];
    for(u32 targetIndex=0;targetIndex<TARGET_COUNT;targetIndex++){
        if(!isFoundationTarget(targetIndex))continue;
        u32 pendingCount=0u;
        bool reportOverflow=false;
        FoundationDiagnosticBank* bank=&g_foundationDiagnostics[targetIndex];
        if(_InterlockedCompareExchange(&bank->lock,1,0)!=0)continue;
        for(u32 i=0;i<bank->used;i++){
            if(!bank->samples[i].reported){
                pending[pendingCount++]=bank->samples[i];
                bank->samples[i].reported=1u;
            }
        }
        if(bank->overflow!=0u&&bank->overflowReported==0u){
            bank->overflowReported=1u;
            reportOverflow=true;
        }
        _InterlockedCompareExchange(&bank->lock,0,1);
        for(u32 i=0;i<pendingCount;i++){
            logFoundationDiagnostic(targetIndex,&pending[i]);
        }
        if(reportOverflow){
            logTarget(
                "DIAG: ",targetIndex,
                " reached the bounded eight-state diagnostic limit; later distinct states may be omitted.\r\n"
            );
        }
    }
}

static void advanceTrackedConstruction(void* object,TrackedUpdateDispatch dispatch){
    if(!object)return;
    u32 quickConstructionId=*(volatile const u32*)((u8*)object+OFF_OBJECT_CONSTRUCTION_ID);
    u32 start=giftMarkerStart(object);
    u32 quickTarget=TARGET_COUNT;
    for(u32 probe=0;probe<GIFT_MARKER_PROBES;probe++){
        GiftMarker* marker=&g_giftMarkers[(start+probe)&(GIFT_MARKER_COUNT-1u)];
        if(!tryLockGiftMarker(marker))continue;
        if(marker->object==(u64)object&&marker->constructionId==quickConstructionId&&
           marker->phase!=GIFT_PHASE_NONE)quickTarget=marker->targetIndex;
        unlockGiftMarker(marker);
        if(quickTarget<TARGET_COUNT)break;
    }
    if(quickTarget>=TARGET_COUNT)return;

    TargetRuntimeState state;
    ConstructionGateSnapshot gate;
    if(!readTargetRuntimeState(object,&state)||
       state.metadata.targetIndex!=quickTarget||
       !readConstructionGate(object,&gate)){
        clearGiftMarkerForObject(object);
        return;
    }
    GiftMarker* marker=lockMatchingGiftMarker(
        object,state.constructionId,state.metadata.targetIndex
    );
    if(!marker)return;

    bool callNextLevel=false;
    u32 requestedLevel=0;
    bool clearMarker=false;
    bool completed=false;
    bool failed=false;
    bool foundationStep=false;
    if(gate.lifecycleState==0u){
        clearMarker=true;
    }else if(marker->phase==GIFT_PHASE_CANDIDATE){
        if(gate.currentLevel>state.metadata.effectiveMaxLevel){
            clearMarker=true;
        }else if(dispatch==TRACKED_UPDATE_PRIMARY&&
                 constructionGateIsStable(&gate)&&
                 gate.currentLevel>=state.metadata.completionLevel){
            if(gate.currentLevel==state.metadata.effectiveMaxLevel){
                clearMarker=true;
            }else{
                marker->phase=GIFT_PHASE_ARMED;
                marker->expectedLevel=gate.currentLevel;
                marker->waitPolls=0;
            }
        }else if(foundationGateIsStable(&gate,&state.metadata,dispatch)){
            marker->phase=GIFT_PHASE_ARMED;
            marker->expectedLevel=gate.currentLevel;
            marker->waitPolls=0;
        }
    }else if(marker->phase==GIFT_PHASE_ARMED){
        if(marker->expectedLevel<state.metadata.completionLevel){
            if(gate.currentLevel!=marker->expectedLevel){
                clearMarker=true;
            }else if(foundationGateIsStable(&gate,&state.metadata,dispatch)){
                requestedLevel=state.metadata.completionLevel;
                marker->phase=GIFT_PHASE_WAIT_ACK;
                marker->expectedLevel=requestedLevel;
                marker->waitPolls=0;
                marker->reserved|=GIFT_FLAG_FOUNDATION_COMPLETION_PRESERVED;
                callNextLevel=true;
                foundationStep=true;
            }
        }else if(dispatch==TRACKED_UPDATE_PRIMARY&&
           constructionGateIsStable(&gate)&&
           gate.currentLevel==marker->expectedLevel&&
           gate.currentLevel>=state.metadata.completionLevel&&
           gate.currentLevel<state.metadata.effectiveMaxLevel){
            // The native material recompute path can request the highest
            // cumulatively funded level in one SetLevel call. Preserve the
            // required Completion milestone, then use that same native shape
            // to avoid replaying a full construction presentation per level.
            requestedLevel=state.metadata.effectiveMaxLevel;
            marker->phase=GIFT_PHASE_WAIT_ACK;
            marker->expectedLevel=requestedLevel;
            marker->waitPolls=0;
            callNextLevel=true;
        }else if(gate.currentLevel!=marker->expectedLevel){
            clearMarker=true;
        }
    }else if(marker->phase==GIFT_PHASE_WAIT_ACK){
        if(gate.currentLevel!=marker->expectedLevel){
            clearMarker=true;
            failed=true;
        }else if(constructionGateIsStable(&gate)){
            if(gate.currentLevel==state.metadata.effectiveMaxLevel){
                marker->phase=GIFT_PHASE_COMPLETE;
                marker->waitPolls=0;
                completed=true;
            }else{
                marker->phase=GIFT_PHASE_CANDIDATE;
                marker->expectedLevel=gate.currentLevel;
                marker->waitPolls=0;
            }
        }else if(gate.lifecycleState==3u&&marker->waitPolls<0xFFFFFFFFu){
            marker->waitPolls++;
        }
    }else if(marker->phase==GIFT_PHASE_COMPLETE&&
             gate.currentLevel!=state.metadata.effectiveMaxLevel){
        clearMarker=true;
    }
    if(clearMarker)resetLockedGiftMarker(marker);
    if(callNextLevel){
        atomicStoreLong(&marker->nativeCallInProgress,1);
        bool called=callOriginalSetLevel(object,requestedLevel);
        u8 currentLevel=0;
        if(!called||!readU8((u8*)object+OFF_OBJECT_LEVEL,&currentLevel)||
           currentLevel!=requestedLevel){
            resetLockedGiftMarker(marker);
            failed=true;
        }else{
            atomicStoreLong(&marker->nativeCallInProgress,0);
        }
        unlockGiftMarker(marker);
        if(failed)incrementPromotion(&g_promotionFailures[state.metadata.targetIndex]);
        else{
            incrementPromotion(&g_levelStepsStarted[state.metadata.targetIndex]);
            if(foundationStep){
                incrementPromotion(&g_foundationStepsStarted[state.metadata.targetIndex]);
            }
        }
        return;
    }
    unlockGiftMarker(marker);

    if(failed)incrementPromotion(&g_promotionFailures[state.metadata.targetIndex]);
    if(completed)incrementPromotion(&g_promotions[state.metadata.targetIndex]);
}

static void __fastcall hookedConstructionUpdate(
    void* object,float elapsedSeconds,u8 updateFlags
){
    if(g_updateHookObserved==0)_InterlockedCompareExchange(&g_updateHookObserved,1,0);
    TrackedUpdateDispatch dispatch=TRACKED_UPDATE_NONE;
    u64 callerRva=0u;
#ifdef DS2_TESTING
    if(updateFlags==1u){
        dispatch=TRACKED_UPDATE_PRIMARY;
        callerRva=RVA_UPDATE_DISPATCH_RETURN;
    }else if(updateFlags==0u){
        dispatch=TRACKED_UPDATE_FOUNDATION_SECONDARY;
        callerRva=RVA_UPDATE_DISPATCH_RETURN_SECONDARY;
    }
#else
    void* caller=_ReturnAddress();
    u64 callerAddress=(u64)caller;
    u64 gameAddress=(u64)g_gameBase;
    callerRva=callerAddress>=gameAddress&&
              callerAddress<gameAddress+EXPECTED_IMAGE_SIZE?
        callerAddress-gameAddress:0xFFFFFFFFFFFFFFFFull;
    if(updateFlags==1u&&caller==(void*)(g_gameBase+RVA_UPDATE_DISPATCH_RETURN)){
        dispatch=TRACKED_UPDATE_PRIMARY;
    }else if(updateFlags==0u&&
             caller==(void*)(g_gameBase+RVA_UPDATE_DISPATCH_RETURN_SECONDARY)){
        dispatch=TRACKED_UPDATE_FOUNDATION_SECONDARY;
    }
#endif
    recordFoundationDiagnostic(object,updateFlags,callerRva,dispatch,0u);
    if(!callOriginalConstructionUpdate(object,elapsedSeconds,updateFlags))return;
    recordFoundationDiagnostic(object,updateFlags,callerRva,dispatch,1u);
    if(dispatch!=TRACKED_UPDATE_NONE)advanceTrackedConstruction(object,dispatch);
}

static void __fastcall hookedSetLevel(void* object,u32 requestedLevel){
    bool materialCaller=false;
#ifdef DS2_TESTING
    materialCaller=g_testMaterialCaller;
#else
    materialCaller=_ReturnAddress()==(void*)(g_gameBase+RVA_MATERIAL_LEVEL_RETURN);
#endif
    bool handled=false;
    bool clearAfterNative=false;
    TargetRuntimeState state;
    if(readTargetRuntimeState(object,&state)){
        u32 phase=GIFT_PHASE_NONE,markerFlags=0u;
        bool markerStateAvailable=readGiftMarkerState(
            object,state.constructionId,state.metadata.targetIndex,
            &phase,&markerFlags
        );
        bool promotionCallInProgress=!markerStateAvailable&&
            giftPromotionCallInProgress(
                object,state.constructionId,state.metadata.targetIndex
            );
        u8 lifecycleState=0,appliedLevel=0;
        bool lifecycleStateAvailable=readU8(
            (u8*)object+OFF_OBJECT_LIFECYCLE_STATE,&lifecycleState
        );
        bool completeAndConfirmed=phase==GIFT_PHASE_COMPLETE&&
            state.currentLevel==state.metadata.effectiveMaxLevel&&
            lifecycleStateAvailable&&
            readU8((u8*)object+OFF_OBJECT_APPLIED_LEVEL,&appliedLevel)&&
            lifecycleState==3u&&appliedLevel==state.metadata.effectiveMaxLevel;
        bool preservedFoundationCompletion=
            (markerFlags&GIFT_FLAG_FOUNDATION_COMPLETION_PRESERVED)!=0u&&
            state.currentLevel>=state.metadata.completionLevel;
        bool protectInProgress=promotionCallInProgress&&
            state.currentLevel>=state.metadata.completionLevel;
        bool foundationSetupRefresh=materialCaller&&requestedLevel>0u&&
            requestedLevel<state.metadata.completionLevel&&
            phase==GIFT_PHASE_CANDIDATE&&
            isFoundationTarget(state.metadata.targetIndex)&&
            lifecycleStateAvailable&&lifecycleState==2u;
        if(materialCaller&&requestedLevel>0u&&
           requestedLevel<state.currentLevel&&
           (completeAndConfirmed||preservedFoundationCompletion||protectInProgress)){
            handled=true;
            incrementPromotion(&g_downgradesPrevented[state.metadata.targetIndex]);
        }else if(phase!=GIFT_PHASE_NONE&&!foundationSetupRefresh&&
                  requestedLevel<state.currentLevel&&
                  requestedLevel<state.metadata.completionLevel){
            clearAfterNative=true;
        }
    }
    if(!handled)callOriginalSetLevel(object,requestedLevel);
    if(clearAfterNative)clearGiftMarkerForObject(object);
}

static bool atomicReplaceVtableSlot(
    u32 classIndex,u32 slotOffset,u64 expected,u64 replacement
){
    if(!g_gameBase||classIndex>=HOOK_CLASS_COUNT)return false;
    volatile s64* slot=(volatile s64*)(
        g_gameBase+HOOK_CLASSES[classIndex].objectVtableRva+slotOffset
    );
    if(((u64)slot&7u)!=0u)return false;
    DWORD oldProtection=0;
    if(!VirtualProtect((void*)slot,8u,PAGE_READWRITE,&oldProtection))return false;
    s64 observed=_InterlockedCompareExchange64(slot,(s64)replacement,(s64)expected);
    DWORD ignored=0;
    if(!VirtualProtect((void*)slot,8u,oldProtection,&ignored)){
        // Retry once, then surface the exceptional protection state in the
        // runtime log instead of silently leaving the shared VTable page RW.
        if(!VirtualProtect((void*)slot,8u,oldProtection,&ignored)){
            _InterlockedCompareExchange(&g_vtableProtectionRestoreFailed,1,0);
        }
    }
    return observed==(s64)expected;
}
static void rollbackObjectVtableHooks(u32 setLevelMask,u32 initMask,u32 updateMask){
    for(u32 i=0;i<HOOK_CLASS_COUNT;i++){
        u32 bit=1u<<i;
        if(updateMask&bit){
            if(atomicReplaceVtableSlot(
                i,OFF_VTABLE_UPDATE,(u64)&hookedConstructionUpdate,
                (u64)(g_gameBase+RVA_CONSTRUCTION_UPDATE)
            ))g_installedUpdateHookMask&=~bit;
        }
        if(initMask&bit){
            if(atomicReplaceVtableSlot(
                i,OFF_VTABLE_INIT,(u64)&hookedConstructionInit,
                (u64)(g_gameBase+HOOK_CLASSES[i].originalInitRva)
            ))g_installedInitHookMask&=~bit;
        }
        if(setLevelMask&bit){
            if(atomicReplaceVtableSlot(
                i,OFF_VTABLE_SET_LEVEL,(u64)&hookedSetLevel,
                (u64)(g_gameBase+HOOK_CLASSES[i].originalSetLevelRva)
            ))g_installedSetLevelHookMask&=~bit;
        }
    }
}
static bool installObjectVtableHooks(){
    if(!g_gameBase||g_installedSetLevelHookMask||g_installedInitHookMask||
       g_installedUpdateHookMask)return false;
    u32 setLevelInstalled=0,initInstalled=0,updateInstalled=0;
    for(u32 i=0;i<HOOK_CLASS_COUNT;i++){
        u64 original=(u64)(g_gameBase+HOOK_CLASSES[i].originalSetLevelRva);
        if(!atomicReplaceVtableSlot(
            i,OFF_VTABLE_SET_LEVEL,original,(u64)&hookedSetLevel
        )){
            rollbackObjectVtableHooks(setLevelInstalled,initInstalled,updateInstalled);
            return false;
        }
        setLevelInstalled|=1u<<i;
        g_installedSetLevelHookMask=setLevelInstalled;
    }
    for(u32 i=0;i<HOOK_CLASS_COUNT;i++){
        if(!atomicReplaceVtableSlot(
            i,OFF_VTABLE_INIT,(u64)(g_gameBase+HOOK_CLASSES[i].originalInitRva),
            (u64)&hookedConstructionInit
        )){
            rollbackObjectVtableHooks(setLevelInstalled,initInstalled,updateInstalled);
            return false;
        }
        initInstalled|=1u<<i;
        g_installedInitHookMask=initInstalled;
    }
    for(u32 i=0;i<HOOK_CLASS_COUNT;i++){
        if(!atomicReplaceVtableSlot(
            i,OFF_VTABLE_UPDATE,(u64)(g_gameBase+RVA_CONSTRUCTION_UPDATE),
            (u64)&hookedConstructionUpdate
        )){
            rollbackObjectVtableHooks(setLevelInstalled,initInstalled,updateInstalled);
            return false;
        }
        updateInstalled|=1u<<i;
        g_installedUpdateHookMask=updateInstalled;
    }
    return true;
}

static DWORD WINAPI worker(LPVOID){
    HMODULE game=GetModuleHandleW(0);
    if(!game||!isDs2Process(game))return 0;
    Sleep(1000u);
    bool enabled=readBoolSetting(L"Enabled",true);
    g_debugLog=readBoolSetting(L"DebugLog",false);
    openLog();
    if(!enabled){
        logCritical("STATE: disabled in INI; no VTable hook was installed.\r\n");
        closeLog();
        return 0;
    }

    g_gameBase=(u8*)game;
    u32 timestamp=0,imageSize=0;
    if(!readBuildMetadata(g_gameBase,&timestamp,&imageSize)||
       timestamp!=EXPECTED_TIMESTAMP||imageSize!=EXPECTED_IMAGE_SIZE){
        logCritical("ERROR: unsupported DS2.exe build; no VTable hook was installed.\r\n");
        closeLog();
        return 0;
    }
    if(!validateRuntimeCode(g_gameBase)){
        logCritical("ERROR: construction Init/Update/SetLevel semantic signatures do not match; no VTable hook was installed.\r\n");
        closeLog();
        return 0;
    }
    if(!validateAllVtables(g_gameBase)||!validateObjectHookSlots(g_gameBase)){
        logCritical("ERROR: construction manager/facility/config/object hook slots do not match; no hook was installed.\r\n");
        closeLog();
        return 0;
    }
    if(!installObjectVtableHooks()){
        logCritical("ERROR: atomic construction VTable hook installation failed or conflicts with another mod.\r\n");
        if(g_vtableProtectionRestoreFailed!=0){
            logCritical("WARNING: Windows did not restore a construction VTable page protection; fully restart the game.\r\n");
        }
        closeLog();
        return 0;
    }
    logCritical("Atomic construction Init, Update, and SetLevel VTable hooks installed. Each native level transition waits for visual acknowledgement before the next one.\r\n");
    if(g_vtableProtectionRestoreFailed!=0){
        logCritical("WARNING: Windows did not restore a construction VTable page protection; restart the game before changing this mod.\r\n");
    }

    Observation observations[TARGET_COUNT];
    long reportedCandidates[TARGET_COUNT];
    long reportedSteps[TARGET_COUNT];
    long reportedFoundationSteps[TARGET_COUNT];
    long reportedPromotions[TARGET_COUNT];
    long reportedDowngrades[TARGET_COUNT];
    long reportedFailures[TARGET_COUNT];
    memset(observations,0,sizeof(observations));
    memset(reportedCandidates,0,sizeof(reportedCandidates));
    memset(reportedSteps,0,sizeof(reportedSteps));
    memset(reportedFoundationSteps,0,sizeof(reportedFoundationSteps));
    memset(reportedPromotions,0,sizeof(reportedPromotions));
    memset(reportedDowngrades,0,sizeof(reportedDowngrades));
    memset(reportedFailures,0,sizeof(reportedFailures));
    bool batchPublished=false;
    bool conflictLogged=false;
    bool initActivityLogged=false;
    bool updateActivityLogged=false;

    for(;;){
        u8* manager=0;
        ResolveResult managerResult=resolveManager(g_gameBase,&manager);
        bool allStable=managerResult==RESOLVE_READY;
        if(managerResult!=RESOLVE_READY){
            memset(observations,0,sizeof(observations));
        }else{
            for(u32 i=0;i<TARGET_COUNT;i++){
                u8* facility=0;
                u8* config=0;
                ResolveResult result=resolveTarget(g_gameBase,manager,i,&facility,&config);
                if(result!=RESOLVE_READY){
                    observations[i].manager=0;
                    observations[i].facility=0;
                    observations[i].config=0;
                    observations[i].stablePolls=0;
                    allStable=false;
                    continue;
                }
                if(observations[i].manager==manager&&observations[i].facility==facility&&
                   observations[i].config==config){
                    if(observations[i].stablePolls<2u)observations[i].stablePolls++;
                }else{
                    observations[i].manager=manager;
                    observations[i].facility=facility;
                    observations[i].config=config;
                    observations[i].stablePolls=1u;
                }
                if(observations[i].stablePolls<2u)allStable=false;
            }
        }

        if(!allStable){
            if(batchPublished){
                clearAllowedConfigs();
                batchPublished=false;
                logCritical("STATE: construction resources changed; promotion paused until the full batch is stable again.\r\n");
            }
        }else if(validateObservationBatch(observations)){
            if(!batchPublished){
                publishAllowedConfigs(observations);
                batchPublished=true;
                conflictLogged=false;
                for(u32 i=0;i<TARGET_COUNT;i++){
                    logTarget(
                        "READY: ",i,
                        " exact config armed for acknowledged milestone promotion.\r\n"
                    );
                }
                logCritical("READY: all 13 canonical player construction configs are armed.\r\n");
            }
        }else{
            clearAllowedConfigs();
            batchPublished=false;
            if(!conflictLogged){
                logCritical("ERROR: canonical PCC level metadata is unexpected; promotion remains disarmed.\r\n");
                conflictLogged=true;
            }
        }

        for(u32 i=0;i<TARGET_COUNT;i++){
            long candidates=_InterlockedCompareExchange(&g_candidatesTracked[i],0,0);
            if(candidates!=reportedCandidates[i]){
                reportedCandidates[i]=candidates;
                logTarget(
                    "TRACKED: ",i,
                    " entered native initialization and is waiting for a stable native milestone.\r\n"
                );
            }
            long steps=_InterlockedCompareExchange(&g_levelStepsStarted[i],0,0);
            if(steps!=reportedSteps[i]){
                reportedSteps[i]=steps;
                logTarget(
                    "STEP: ",i,
                    " started one native milestone transition; waiting for visual acknowledgement.\r\n"
                );
            }
            long foundationSteps=_InterlockedCompareExchange(
                &g_foundationStepsStarted[i],0,0
            );
            if(foundationSteps!=reportedFoundationSteps[i]){
                reportedFoundationSteps[i]=foundationSteps;
                logTarget(
                    "FOUNDATION: ",i,
                    " started its native level-1 to Completion transition.\r\n"
                );
            }
            long current=_InterlockedCompareExchange(&g_promotions[i],0,0);
            if(current!=reportedPromotions[i]){
                reportedPromotions[i]=current;
                logTarget("COMPLETE: ",i," reached effective MaxLevel after every native visual transition was acknowledged.\r\n");
            }
            long prevented=_InterlockedCompareExchange(&g_downgradesPrevented[i],0,0);
            if(prevented!=reportedDowngrades[i]){
                reportedDowngrades[i]=prevented;
                logTarget("PRESERVED: ",i," remained at effective MaxLevel after a material recompute.\r\n");
            }
            long failures=_InterlockedCompareExchange(&g_promotionFailures[i],0,0);
            if(failures!=reportedFailures[i]){
                reportedFailures[i]=failures;
                logTarget(
                    "ERROR: ",i,
                    " an acknowledged milestone promotion left the expected native lifecycle; tracking was stopped.\r\n"
                );
            }
        }
        if(!initActivityLogged&&_InterlockedCompareExchange(&g_initHookObserved,0,0)==1){
            initActivityLogged=true;
            logCritical("ACTIVE: construction Init hook observed runtime objects.\r\n");
        }
        if(!updateActivityLogged&&_InterlockedCompareExchange(&g_updateHookObserved,0,0)==1){
            updateActivityLogged=true;
            logCritical("ACTIVE: construction Update hook observed runtime objects.\r\n");
        }
        drainFoundationDiagnostics();
        if(g_debugLog&&!batchPublished)logDebug("Waiting for a complete stable PCC config batch.\r\n");
        Sleep(1000u);
    }
}

static void start(){
    if(_InterlockedCompareExchange(&g_started,1,0)!=0)return;
    HANDLE thread=CreateThread(0,0,worker,0,0,0);
    if(thread)CloseHandle(thread);
    else _InterlockedCompareExchange(&g_started,0,1);
}

extern "C" __declspec(dllexport) void InitializeASI(){start();}
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
