static u32 alignmentTestCalls=0,alignmentTestSeenVariant=0;
static u8 alignmentTestGeometry[0x40];
static void alignmentTestBuild(u8* instance){
    alignmentTestCalls++;alignmentTestSeenVariant=*(u32*)(instance+0x214u);
    static const float multipliers[]={1.0f,1.5f,2.0f,3.0f,4.0f};
    u8* resource=*(u8**)(instance+0x30u);
    *(float*)(alignmentTestGeometry+0x28u)=*(float*)(resource+OFF_SURVEY_RANGE)*multipliers[alignmentTestSeenVariant];
    *(u8**)(instance+0x50u)=alignmentTestGeometry;
}
static u32 testMappedSurveyAlignment(u8* base){
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateSurveyAlignment(&image))return 9500u;
    HudDistancePatch patch={};
    if(installExactCall(base,&SURVEY_ALIGNMENT_CALL,(u64)&surveyAlignmentHook,&patch)!=APPLY_READY)return 9501u;
    HudDistancePatch duplicate={};
    if(installExactCall(base,&SURVEY_ALIGNMENT_CALL,(u64)&surveyAlignmentHook,&duplicate)!=APPLY_CONFLICT)return 9502u;
    u8 resource[0x230]={},instance[0x230]={};
    g_gameBase=base;g_targetEffectDiameterBits=floatBits(2000.0f);g_targetScanRadiusBits=floatBits(1000.0f);
    *(u64*)resource=(u64)(base+RVA_SURVEY_VTABLE);
    *(u32*)(resource+OFF_SURVEY_RANGE)=g_targetEffectDiameterBits;
    *(u8**)(instance+0x30u)=resource;
    _InterlockedExchange64(&g_surveyResource,(long long)(u64)resource);
    _InterlockedExchange(&g_hooksReady,1);g_surveyGeometryBuild=alignmentTestBuild;
    u8 beforeResource[sizeof(resource)];memcpy(beforeResource,resource,sizeof(resource));
    for(u32 variant=0;variant<5u;variant++){
        *(u32*)(instance+0x214u)=variant;alignmentTestCalls=0;
        ((SurveyGeometryBuild)patch.relay)(instance);
        if(alignmentTestCalls!=1u||alignmentTestSeenVariant!=0u||
           g_surveyAlignmentRadius!=(long)floatBits(1000.0f)||g_surveyAlignmentVariant!=(long)variant||
           !bytesEqual(resource,beforeResource,sizeof(resource)))return 9510u+variant;
    }
    // Other resources, shape modes and incomplete activation stay native.
    for(u32 test=0;test<4u;test++){
        *(u32*)(instance+0x214u)=2u;
        if(test==0)_InterlockedExchange(&g_hooksReady,0);
        if(test==1)_InterlockedExchange64(&g_surveyResource,0);
        if(test==2)*(u32*)(resource+OFF_SURVEY_SHAPE)=1u;
        if(test==3)*(float*)(resource+OFF_SURVEY_RANGE)=50.0f;
        alignmentTestCalls=0;((SurveyGeometryBuild)patch.relay)(instance);
        if(alignmentTestCalls!=1u||alignmentTestSeenVariant!=2u)return 9520u+test;
        _InterlockedExchange(&g_hooksReady,1);_InterlockedExchange64(&g_surveyResource,(long long)(u64)resource);
        memcpy(resource,beforeResource,sizeof(resource));
    }
    if(rollbackExactCall(base,&SURVEY_ALIGNMENT_CALL,&patch)!=APPLY_FAILED||!validateSurveyAlignment(&image))return 9530u;
    VirtualFree(patch.relay,0,MEM_RELEASE);
    _InterlockedExchange64(&g_surveyResource,0);

    // Compare multipart condition decisions to the actual native leaf, including
    // empty slots, each intact/destroyed part, terminators and cargo exceptions.
    u8 meta[0x100]={},parts[0x40]={},kind[0x30]={};
    *(float*)(meta+0x88u)=1.0f;*(float*)(meta+0x8Cu)=1.0f;
    *(u8**)(meta+0xD0u)=parts;
    typedef u8 (*NativePartsSpent)(const u8*);
    NativePartsSpent native=(NativePartsSpent)(base+0x12389A0u);
    for(u32 index=0;index<6u;index++)for(u32 mask=0;mask<64u;mask++){
        memset(parts,0,sizeof(parts));*(unsigned short*)(parts+6u+2u*index)=1u;parts[0x36u]=(u8)mask;
        if(spentCargoForExtension(meta)!=(native(parts)!=0u))return 9540u;
        *(unsigned short*)(parts+6u+2u*index)=0u;*(unsigned short*)(parts+0x12u+2u*index)=1u;
        if(spentCargoForExtension(meta)!=(native(parts)!=0u))return 9541u;
        parts[index]=0xFFu;
        if(spentCargoForExtension(meta)!=(native(parts)!=0u))return 9542u;
    }
    memset(parts,0,sizeof(parts));*(u8**)(meta+0xC0u)=kind;*(unsigned short*)(kind+0x20u)=0xCEu;
    if(spentCargoForExtension(meta))return 9543u;
    *(u8**)(meta+0xD0u)=0;*(float*)(meta+0x8Cu)=0.0f;*(u8**)(meta+0xC8u)=kind;kind[0x20u]=0x4Bu;
    if(spentCargoForExtension(meta))return 9544u;
    return 0;
}
