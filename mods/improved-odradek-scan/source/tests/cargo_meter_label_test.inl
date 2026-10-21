static u8 meterTestController[0x231B0];
static u32 meterTestCalls=0,meterTestIndex=0;
static u8* meterTestPosition=0;
static u8 meterTestBuild(u8* controller,u32 index,u8* position){
    meterTestCalls++;meterTestIndex=index;meterTestPosition=position;
    return controller==meterTestController?0xA5u:0u;
}
static u32 testMappedCargoMeter(u8* base){
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateCargoMeter(&image))return 9600u;
    HudDistancePatch patch={};
    DWORD before=0;
    if(!queryProtection(base+CARGO_METER_CALL.windowRva,&before))return 9601u;
    if(installExactCall(base,&CARGO_METER_CALL,(u64)&cargoMeterLabelHook,&patch)!=APPLY_READY)return 9602u;
    if(!protectionMatchesAfterOwnedWrite(base+CARGO_METER_CALL.windowRva,before))return 9603u;
    HudDistancePatch duplicate={};
    if(installExactCall(base,&CARGO_METER_CALL,(u64)&cargoMeterLabelHook,&duplicate)!=APPLY_CONFLICT)return 9604u;
    g_cargoLabelBuild=meterTestBuild;
    _InterlockedExchange(&g_hooksReady,1);_InterlockedExchange(&g_scannedHudReady,1);
    CargoLabelBuild relay=(CargoLabelBuild)patch.relay;
    // Every native type: change exactly one presentation byte for cargo only.
    for(u32 type=0;type<0x130u;type++){
        u8* record=meterTestController+0x1B0u+(type%256u)*0x230u;
        memset(record,0,0x230u);*(unsigned short*)(record+8u)=(unsigned short)type;
        record[0x28u]=1u;*(float*)(record+0x74u)=649.43f;
        u8 expected[0x230];memcpy(expected,record,sizeof(expected));
        if(type>=0xBDu&&type<=0xD0u)expected[0x72u]=1u;
        meterTestCalls=0;
        if(relay(meterTestController,123u,record+0x30u)!=0xA5u||meterTestCalls!=1u||
           meterTestIndex!=123u||meterTestPosition!=record+0x30u||
           !bytesEqual(record,expected,sizeof(expected)))return 9610u;
    }
    // Native distance remains unchanged, including zero and long distances.
    u8* record=meterTestController+0x1B0u;
    const u32 distances[]={0u,0x3F800000u,0x447A0000u,0x461C4000u,0xBF800000u,0x7F800000u,0x7FC00000u};
    for(u32 i=0;i<7u;i++){
        memset(record,0,0x230u);*(unsigned short*)(record+8u)=0xBDu;
        record[0x28u]=1u;*(u32*)(record+0x74u)=distances[i];
        relay(meterTestController,0u,record+0x30u);
        if(record[0x72u]!=(i<4u?1u:0u)||*(u32*)(record+0x74u)!=distances[i])return 9620u+i;
    }
    for(u32 i=0;i<6u;i++){
        memset(record,0,0x230u);*(unsigned short*)(record+8u)=0xBDu;record[0x28u]=1u;
        if(i==0)_InterlockedExchange(&g_hooksReady,0);
        if(i==1)_InterlockedExchange(&g_scannedHudReady,0);
        if(i==2)record[0x70u]=1u;
        if(i==3)record[0x28u]=0u;
        u8* position=record+0x30u;
        if(i==4)position++;
        if(i==5)position=meterTestController+0x231E0u;
        relay(meterTestController,0u,position);
        if(record[0x72u])return 9630u+i;
        _InterlockedExchange(&g_hooksReady,1);_InterlockedExchange(&g_scannedHudReady,1);
    }
    if(rollbackExactCall(base,&CARGO_METER_CALL,&patch)!=APPLY_FAILED||!validateCargoMeter(&image)||
       !protectionMatchesAfterOwnedWrite(base+CARGO_METER_CALL.windowRva,before))return 9640u;
    VirtualFree(patch.relay,0,MEM_RELEASE);
    return 0;
}
