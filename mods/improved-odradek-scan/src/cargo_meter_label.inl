// The native controller already stores sqrt(distanceSquared) at record+0x74.
// Its renderer formats this with the game's localized meter unit when +0x72=1.
// Enable only that presentation field, after the normal cargo text builder.
// This call runs in the controller's preparation phase, before HUD render jobs.
static bool recipientMeterReady(u8* controller,const u8* record);

typedef u8 (*CargoLabelBuild)(u8*,u32,u8*);
static CargoLabelBuild g_cargoLabelBuild=0;
static HudDistancePatch g_cargoMeterPatch={};
static const HudDistanceCall CARGO_METER_CALL={0x17DFCAFu,0x17DFCB0u,0x17E5980u,
    {0xCC,0x5C,0x00,0x00,0x84,0xC0,0x0F,0x85}};
static volatile long g_cargoMeterCount=0,g_cargoMeterLastBits=0;

static bool enableCargoMeter(u8* controller,u8* position){
    if(!controller||!position||_InterlockedCompareExchange(&g_hooksReady,0,0)!=1||
       _InterlockedCompareExchange(&g_scannedHudReady,0,0)!=1)return false;
    u64 start=(u64)controller+0x1E0u,address=(u64)position;
    if(address<start||address-start>=256u*0x230u||(address-start)%0x230u!=0u)return false;
    u8* record=position-0x30u;
    // Both pointers belong to this exact native call; no entity traversal or
    // VirtualQuery is needed on every marker every frame.
    u32 kind=*(const unsigned short*)(record+8u);
    u32 bits=*(const u32*)(record+0x74u);
    if(kind<0xBDu||kind>0xD0u||record[0x28u]!=1u||record[0x70u]!=0u||
       (bits&0x80000000u)!=0u||(bits&0x7F800000u)==0x7F800000u)return false;
    record[0x72u]=recipientMeterReady(controller,record)?0u:1u;
    _InterlockedExchange(&g_cargoMeterLastBits,(long)bits);
    _InterlockedIncrement(&g_cargoMeterCount);
    return true;
}

extern "C" u8 cargoMeterLabelHook(u8* controller,u32 index,u8* position){
    u8 result=g_cargoLabelBuild(controller,index,position);
    enableCargoMeter(controller,position);
    return result;
}

static bool validateCargoMeter(const GameImage* image){
    static const u8 builder[]={0x40,0x55,0x57,0x41,0x56,0x48,0x8D,0xAC,0x24,0xC0,0xFE,0xFF,0xFF,
        0x48,0x81,0xEC,0x40,0x02,0x00,0x00};
    static const u8 consumer[]={0x44,0x38,0x77,0x72,0x0F,0x84,0xF9,0x03,0x00,0x00};
    return image&&image->imageSize==EXPECTED_IMAGE_SIZE&&image->base[CARGO_METER_CALL.callRva]==0xE8u&&
        bytesEqual(image->base+CARGO_METER_CALL.windowRva,CARGO_METER_CALL.expected,8u)&&
        bytesEqual(image->base+CARGO_METER_CALL.targetRva,builder,sizeof(builder))&&
        bytesEqual(image->base+0x17E34CCu,consumer,sizeof(consumer));
}

static ApplyResult installCargoMeter(u8* base){
    g_cargoLabelBuild=(CargoLabelBuild)(base+CARGO_METER_CALL.targetRva);
    return installExactCall(base,&CARGO_METER_CALL,(u64)&cargoMeterLabelHook,&g_cargoMeterPatch);
}

static void logCargoMeter(){
    static u32 last=0;u32 count=(u32)_InterlockedCompareExchange(&g_cargoMeterCount,0,0);
    if(count==last)return;last=count;
    char buffer[160];u32 pos=appendText(buffer,0,160u,"CARGO-METERS: enabled=");
    pos=appendUnsigned(buffer,pos,160u,count);
    pos=appendText(buffer,pos,160u," lastNativeDistanceBits=");
    pos=appendHex32(buffer,pos,160u,(u32)_InterlockedCompareExchange(&g_cargoMeterLastBits,0,0));
    pos=appendText(buffer,pos,160u,"\r\n");logRaw(buffer,pos);
}
