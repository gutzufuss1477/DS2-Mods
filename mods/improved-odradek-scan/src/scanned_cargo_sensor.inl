// Native Detected update supplies +0x5C as an optional per-object cap when
// +0x60 is set. Retry only a verified cargo object reached by the current wave.
// The native gate still owns angle, LOS, progression and the final decision.
static bool validateCargoSensorTargets(const GameImage* image){
    static const u8 cap[]={0xC4,0xC1,0x78,0x2F,0xF0,0x76,0x0D,0xC5,0xCA,0x59,0xC6,0xC5,0xF8,
        0x2F,0xE0,0x0F,0x96,0xC0,0xEB,0x06,0xB0,0x01,0xEB,0x02,0x32,0xC0};
    static const u8 source[]={0x80,0x7F,0x60,0x00,0x74,0x1B,0xC5,0xFA,0x10,0x57,0x5C,0xEB,0x19};
    return image&&image->imageSize==EXPECTED_IMAGE_SIZE&&
        bytesEqual(image->base+0xDE6FF8u,cap,sizeof(cap))&&
        bytesEqual(image->base+0x1CE5A94u,source,sizeof(source));
}

static u8 retryCargoSensorGate(SensorGate original,u8* manager,const double* target,
                              float optionalRadius,u8* component){
    if(!original||!manager||!target||!component||!g_scannedHudFind||
       _InterlockedCompareExchange(&g_scannedHudReady,0,0)!=1||
       _InterlockedCompareExchange(&g_hooksReady,0,0)!=1||!manager[0x28u])return 0;
    float limit=0.0f,progress=0.0f;u32 optionalBits=0;
    memcpy(&optionalBits,&optionalRadius,4u);
    if(!finitePositiveFloat(optionalBits,&optionalRadius)||
       !finitePositiveFloat(g_targetScanRadiusBits,&limit)||optionalRadius>=limit||
       !finitePositiveFloat(*(u32*)(manager+0x98u),&progress))return 0;
    double dx=target[0]-*(double*)(manager+0x38u);
    double dy=target[1]-*(double*)(manager+0x40u);
    double dz=target[2]-*(double*)(manager+0x48u);
    double squared=dx*dx+dy*dy+dz*dz;
    if(!(squared>(double)optionalRadius*optionalRadius&&
         squared<=(double)limit*limit&&squared<=(double)progress*progress))return 0;
    _InterlockedIncrement(&g_cargoSensorCounts[0]);
    if(*(u64*)component!=(u64)(g_gameBase+RVA_DETECTED_COMPONENT_VTABLE)||
       component[0x51u]!=1u)return 0;
    u8* owner=*(u8**)(component+0x48u);
    if(!owner)return 0;
    u8* baggage=g_scannedHudFind(owner+0xA0u,g_gameBase+RVA_BAGGAGE_COMPONENT_RTTI);
    if(!baggage||*(u64*)baggage!=(u64)(g_gameBase+RVA_BAGGAGE_COMPONENT_VTABLE)||
       *(u8**)(baggage+0x230u)!=component||*(u8**)(baggage+0xF8u)!=owner+0x20u||
       !eligibleCargoMetadata(baggage))return 0;
    if(spentCargoForExtension(baggage))return 0;
    _InterlockedIncrement(&g_cargoSensorCounts[1]);
    _InterlockedIncrement(&g_cargoSensorCounts[2]);
    u8 result=original(manager,target,limit);
    if(result)_InterlockedIncrement(&g_cargoSensorCounts[3]);
    return result;
}
