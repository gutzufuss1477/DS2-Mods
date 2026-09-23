// The native scan-start selects survey variants 0..4. Their terrain diameter
// multipliers are 1, 1.5, 2, 3, 4, separate from the gameplay sensor's radius.
// In our configured Sphere scan, use the base variant so the geometry diameter
// remains exactly twice RangeMeters. The original builder still runs once.
typedef void (*SurveyGeometryBuild)(u8*);
static SurveyGeometryBuild g_surveyGeometryBuild=0;
static HudDistancePatch g_surveyAlignmentPatch={};
static const HudDistanceCall SURVEY_ALIGNMENT_CALL={0xDE5DF9u,0xDE5DF8u,0x1D773F0u,
    {0x00,0xE8,0xF2,0x15,0xF9,0x00,0x48,0x8B}};
static volatile long g_surveyAlignmentCalls=0,g_surveyAlignmentApplied=0;
static volatile long g_surveyAlignmentVariant=0,g_surveyAlignmentRadius=0;

static bool normalizeSurveyVariant(u8* instance,u32* before){
    if(!instance||!before||_InterlockedCompareExchange(&g_hooksReady,0,0)!=1)return false;
    u8* resource=0;u32 diameter=0,shape=0,variant=0;
    if(!readObjectPointer(instance,0x30u,&resource)||
       resource!=(u8*)(u64)_InterlockedCompareExchange64(&g_surveyResource,0,0)||
       !hasExactVtable(resource,RVA_SURVEY_VTABLE)||
       !readU32(resource+OFF_SURVEY_RANGE,&diameter)||diameter!=g_targetEffectDiameterBits||
       !readU32(resource+OFF_SURVEY_SHAPE,&shape)||shape!=0u||
       !readU32(instance+0x214u,&variant)||variant>4u)return false;
    *before=variant;
    u32 observed=0;
    return variant==0u||(compareExchangeU32(instance+0x214u,0u,variant,&observed)&&observed==variant);
}

extern "C" void surveyAlignmentHook(u8* instance){
    u32 before=0;
    bool normalized=normalizeSurveyVariant(instance,&before);
    g_surveyGeometryBuild(instance);
    if(normalized){
        _InterlockedExchange(&g_surveyAlignmentVariant,(long)before);
        u8* geometry=0;u32 diameterBits=0;float diameter=0.0f;
        if(readObjectPointer(instance,0x50u,&geometry)&&readU32(geometry+0x28u,&diameterBits)&&
           finitePositiveFloat(diameterBits,&diameter)){
            _InterlockedExchange(&g_surveyAlignmentRadius,(long)floatBits(diameter*0.5f));
        }else _InterlockedExchange(&g_surveyAlignmentRadius,0);
        _InterlockedIncrement(&g_surveyAlignmentApplied);
    }
    _InterlockedIncrement(&g_surveyAlignmentCalls);
}

static bool validateSurveyAlignment(const GameImage* image){
    static const u8 build[]={0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,
        0x24,0x18,0x57,0x41,0x56,0x41,0x57,0x48,0x83,0xEC,0x30,0x48,0x8B,0x69,0x30,0x48,0x8B,0xD9,0x48};
    static const float scales[]={1.0f,1.5f,2.0f,3.0f,4.0f};
    return image&&image->imageSize==EXPECTED_IMAGE_SIZE&&
        bytesEqual(image->base+SURVEY_ALIGNMENT_CALL.windowRva,SURVEY_ALIGNMENT_CALL.expected,8u)&&
        bytesEqual(image->base+SURVEY_ALIGNMENT_CALL.targetRva,build,sizeof(build))&&
        bytesEqual(image->base+0x2E6D5F0u,(const u8*)scales,sizeof(scales));
}

static ApplyResult installSurveyAlignment(u8* base){
    g_surveyGeometryBuild=(SurveyGeometryBuild)(base+SURVEY_ALIGNMENT_CALL.targetRva);
    return installExactCall(base,&SURVEY_ALIGNMENT_CALL,(u64)&surveyAlignmentHook,&g_surveyAlignmentPatch);
}

static void logSurveyAlignment(){
    static u32 last=0;
    u32 calls=(u32)_InterlockedCompareExchange(&g_surveyAlignmentCalls,0,0);
    if(calls==last)return;last=calls;
    char buffer[256];u32 pos=appendText(buffer,0,256u,"SCAN-ALIGNMENT: calls=");
    pos=appendUnsigned(buffer,pos,256u,calls);
    pos=appendText(buffer,pos,256u," normalized=");
    pos=appendUnsigned(buffer,pos,256u,(u32)_InterlockedCompareExchange(&g_surveyAlignmentApplied,0,0));
    pos=appendText(buffer,pos,256u," nativeVariant=");
    pos=appendUnsigned(buffer,pos,256u,(u32)_InterlockedCompareExchange(&g_surveyAlignmentVariant,0,0));
    pos=appendText(buffer,pos,256u," terrainRadiusBits=");
    pos=appendHex32(buffer,pos,256u,(u32)_InterlockedCompareExchange(&g_surveyAlignmentRadius,0,0));
    pos=appendText(buffer,pos,256u," sensorTargetRadiusBits=");
    pos=appendHex32(buffer,pos,256u,g_targetScanRadiusBits);
    pos=appendText(buffer,pos,256u,"\r\n");logRaw(buffer,pos);
}
