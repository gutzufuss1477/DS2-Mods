static u8 recipientTestController[0x2B4B0];
static u8 recipientTestBaggage[0x128],recipientTestResource[0x50];
static u8 recipientTestString[528],recipientTestResult[528];
static u32 recipientTestRenderCalls=0,recipientTestGetCalls=0,recipientTestSetCalls=0;
static u8* recipientTestControllerArg=0;
static u8* recipientTestDetailArg=0;
static float recipientTestElapsed=0;
static const char* recipientTestName="Mechaniker";
static u32 recipientTestNameLength=10u;
static u32 recipientTestFormatCalls=0,recipientTestFormatIndex=0;
static char** recipientTestFormatOut=0;
static const void* recipientTestFormatArg=0;
static void recipientTestFormat(char** out,u32 index,const void* argument){
    recipientTestFormatCalls++;recipientTestFormatOut=out;
    recipientTestFormatIndex=index;recipientTestFormatArg=argument;
}
static char** recipientTestGet(char** out,u8*){
    recipientTestGetCalls++;
    *(u32*)(recipientTestString+8u)=recipientTestNameLength;
    memcpy(recipientTestString+16u,recipientTestName,recipientTestNameLength);
    recipientTestString[16u+recipientTestNameLength]=0;
    *out=(char*)recipientTestString+16u;return out;
}
static void recipientTestSet(char** out,const char* value,u32 length){
    recipientTestSetCalls++;*(u32*)(recipientTestResult+8u)=length;
    memcpy(recipientTestResult+16u,value,length);recipientTestResult[16u+length]=0;
    *out=(char*)recipientTestResult+16u;
}
static void recipientTestRender(u8* controller,u8* detail,float elapsed){
    recipientTestRenderCalls++;recipientTestControllerArg=controller;
    recipientTestDetailArg=detail;recipientTestElapsed=elapsed;
    if(detail[0x31u]){char* out=0;recipientNameHook(&out,recipientTestBaggage);detail[0x31u]=0u;}
}
static void recipientTestFixture(){
    memset(recipientTestController,0,sizeof(recipientTestController));
    memset(recipientTestBaggage,0,sizeof(recipientTestBaggage));
    memset(recipientTestResource,0,sizeof(recipientTestResource));
    memset(g_recipientCache,0,sizeof(g_recipientCache));
    u8* detail=recipientTestController+0x231B0u;
    u8* icon=recipientTestController+0x1B0u;
    *(u64*)detail=*(u64*)icon=123u;
    *(unsigned short*)(detail+8u)=*(unsigned short*)(icon+8u)=0xBDu;
    detail[0x30u]=1u;icon[0x28u]=1u;*(float*)(icon+0x74u)=291.75f;
    *(u64*)(recipientTestBaggage+0x18u)=123u;
    recipientTestName="Mechaniker";recipientTestNameLength=10u;
    g_recipientRender=recipientTestRender;g_recipientGet=recipientTestGet;
    g_recipientStringSet=recipientTestSet;
    g_recipientFormat=recipientTestFormat;recipientTestFormatCalls=0;
    _InterlockedExchange(&g_recipientReady,1);_InterlockedExchange(&g_hooksReady,1);
    _InterlockedExchange(&g_scannedHudReady,1);
    recipientTestRenderCalls=recipientTestGetCalls=recipientTestSetCalls=0;
}

static u32 testRecipientLikes(u8* base){
    recipientTestFixture();
    typedef u8 (*NativeRewardPredicate)(u8*);
    NativeRewardPredicate native=(NativeRewardPredicate)(base+0x122F430u);
    // Compare the row-presence decision with actual game machine code.
    for(u32 flags=0;flags<4u;flags++)for(u32 state=0;state<3u;state++)
    for(u32 type=0;type<7u;type++)for(u32 id=0;id<2u;id++)for(u32 res=0;res<3u;res++){
        recipientTestBaggage[0xECu]=(u8)(flags&1u);
        *(u32*)(recipientTestBaggage+0xE8u)=(flags&2u)?0x8000u:0u;
        *(u32*)(recipientTestBaggage+0x120u)=state;
        *(u64*)(recipientTestBaggage+0x28u)=((u64)type<<32u)|id;
        *(u8**)(recipientTestBaggage+0x38u)=recipientTestResource;
        recipientTestResource[0x48u]=(u8)(res==1u);
        bool expected=(native(recipientTestBaggage)!=0u||(flags&2u))&&res==0u;
        // The original predicate can dereference resource for type 5, so
        // test null only after evaluating it with the safe fixture resource.
        if(res==2u)*(u8**)(recipientTestBaggage+0x38u)=0;
        if(recipientHasLikes(recipientTestBaggage)!=expected)return 9850u;
    }
    recipientTestFixture();
    *(u8**)(recipientTestBaggage+0x38u)=recipientTestResource;
    *(u32*)(recipientTestBaggage+0xE8u)=0x8000u;
    RecipientScope scope={123u,700u,false,false};TlsSetValue(g_recipientTls,&scope);
    char* name=0;recipientNameHook(&name,recipientTestBaggage);
    if(!scope.useLikes||scope.appended||recipientTestSetCalls||
       !bytesEqual((u8*)name,(const u8*)"Mechaniker",11u))return 9851u;
    // The real localized icon markup remains byte-identical at the front.
    const char likes[]="<icon=like>122";
    *(u32*)(recipientTestString+8u)=sizeof(likes)-1u;
    memcpy(recipientTestString+16u,likes,sizeof(likes));
    char* out=(char*)recipientTestString+16u;u8 argument[24]={};
    RecipientFormat relay=(RecipientFormat)g_recipientPatches[2].relay;
    relay(&out,2u,argument);
    const char expected[]="<icon=like>122 | 700 m";
    if(recipientTestFormatCalls!=1u||recipientTestFormatOut!=&out||recipientTestFormatIndex!=2u||
       recipientTestFormatArg!=argument||recipientTestSetCalls!=1u||!scope.appended||
       !bytesEqual((u8*)out,(const u8*)expected,sizeof(expected)))return 9852u;
    relay(&out,2u,argument);
    if(recipientTestSetCalls!=1u||recipientTestFormatCalls!=2u)return 9853u;
    for(u32 guard=0;guard<4u;guard++){
        scope.appended=false;scope.useLikes=guard!=1u;
        TlsSetValue(g_recipientTls,guard==0u?0:&scope);
        _InterlockedExchange(&g_hooksReady,guard==2u?0:1);
        relay(&out,guard==3u?3u:2u,argument);
        if(scope.appended||recipientTestSetCalls!=1u)return 9854u;
    }
    _InterlockedExchange(&g_hooksReady,1);scope.useLikes=true;scope.appended=false;
    TlsSetValue(g_recipientTls,&scope);
    // Execute the game's formatter at type=0 for both remaining slots.
    // This verifies that slot 3 cannot remove/change the appended suffix.
    g_recipientFormat=(RecipientFormat)(base+0x147A960u);
    out=(char*)recipientTestString+16u;
    relay(&out,2u,argument);
    char* final=out;g_recipientFormat(&out,3u,argument);
    if(out!=final||!bytesEqual((u8*)out,(const u8*)expected,sizeof(expected)))return 9855u;
    TlsSetValue(g_recipientTls,0);return 0;
}

static u32 testMappedRecipient(u8* base){
    GameImage image={base,EXPECTED_TIMESTAMP,EXPECTED_IMAGE_SIZE};
    if(!validateRecipientMeter(&image))return 9800u;
    if(installRecipientMeter(base)!=APPLY_READY)return 9801u;
    recipientTestFixture();
    RecipientRender render=(RecipientRender)g_recipientPatches[0].relay;
    RecipientGet getter=(RecipientGet)g_recipientPatches[1].relay;
    u8* detail=recipientTestController+0x231B0u;
    u8* icon=recipientTestController+0x1B0u;
    RecipientScope enclosing={999u,1u,false,false};TlsSetValue(g_recipientTls,&enclosing);
    u8 beforeIcon[0x230];memcpy(beforeIcon,icon,sizeof(beforeIcon));
    render(recipientTestController,detail,0.125f);
    const char expected[]="Mechaniker | 291 m";
    if(recipientTestRenderCalls!=1u||recipientTestGetCalls!=1u||recipientTestSetCalls!=1u||
       recipientTestControllerArg!=recipientTestController||recipientTestDetailArg!=detail||
       recipientTestElapsed!=0.125f||TlsGetValue(g_recipientTls)!=&enclosing||
       !bytesEqual(recipientTestResult+16u,(const u8*)expected,sizeof(expected))||
       !bytesEqual(beforeIcon,icon,sizeof(beforeIcon))||!recipientMeterReady(recipientTestController,icon))return 9802u;
    // Cached text is not rebuilt every frame; integer changes do rebuild from
    // the original name, never appending duplicate suffixes.
    render(recipientTestController,detail,0.25f);
    if(recipientTestSetCalls!=1u||recipientTestRenderCalls!=2u)return 9803u;
    *(float*)(icon+0x74u)=292.1f;render(recipientTestController,detail,0.25f);
    const char expected2[]="Mechaniker | 292 m";
    if(recipientTestSetCalls!=2u||!bytesEqual(recipientTestResult+16u,(const u8*)expected2,sizeof(expected2)))return 9804u;
    // Native dirty state must refresh a changed recipient even at the same metre.
    recipientTestName="Empfaenger";recipientTestNameLength=10u;detail[0x31u]=1u;
    render(recipientTestController,detail,0.25f);
    const char expected3[]="Empfaenger | 292 m";
    if(recipientTestSetCalls!=3u||!bytesEqual(recipientTestResult+16u,(const u8*)expected3,sizeof(expected3)))return 9807u;
    // The icon meter is hidden only after a successful detail update.
    if(!enableCargoMeter(recipientTestController,icon+0x30u)||icon[0x72u]!=0u)return 9805u;
    g_recipientCache[0].time=GetTickCount64()-1001u;
    if(!enableCargoMeter(recipientTestController,icon+0x30u)||icon[0x72u]!=1u)return 9806u;
    for(u32 condition=0;condition<9u;condition++){
        recipientTestFixture();
        if(condition==0)*(u64*)icon=321u;
        if(condition==1)icon[0x70u]=1u;
        if(condition==2)*(u32*)(icon+0x74u)=0x7FC00000u;
        if(condition==3)*(float*)(icon+0x74u)=-1.0f;
        if(condition==4)detail[0x30u]=0u;
        if(condition==5)*(unsigned short*)(detail+8u)=0xBCu;
        if(condition==6)*(u64*)(recipientTestBaggage+0x18u)=321u;
        if(condition==7)recipientTestNameLength=0u;
        if(condition==8)_InterlockedExchange(&g_hooksReady,0);
        render(recipientTestController,detail,0.125f);
        if(recipientTestRenderCalls!=1u||recipientTestSetCalls||recipientMeterReady(recipientTestController,icon))return 9810u+condition;
    }
    // Getter relay preserves pointer return/native args, UTF-8 and bounds;
    // no append outside the scoped detail job or for another baggage ID.
    recipientTestFixture();RecipientScope scope={123u,300u,false,false};
    const char unicodeName[]="Chronobiologin \xC3\xA4";
    recipientTestName=unicodeName;recipientTestNameLength=sizeof(unicodeName)-1u;
    TlsSetValue(g_recipientTls,&scope);char* out=0;
    if(getter(&out,recipientTestBaggage)!=&out||!scope.appended||
       !bytesEqual((u8*)out,(const u8*)unicodeName,sizeof(unicodeName)-1u))return 9830u;
    TlsSetValue(g_recipientTls,0);recipientTestSetCalls=0;
    if(getter(&out,recipientTestBaggage)!=&out||recipientTestSetCalls)return 9831u;
    char longName[482];memset(longName,'x',481u);longName[481]=0;
    recipientTestName=longName;recipientTestNameLength=481u;scope.appended=false;
    TlsSetValue(g_recipientTls,&scope);
    getter(&out,recipientTestBaggage);
    if(recipientTestSetCalls)return 9832u;
    TlsSetValue(g_recipientTls,0);
    u32 likesResult=testRecipientLikes(base);if(likesResult)return likesResult;
    for(u32 i=0;i<3u;i++){
        HudDistancePatch duplicate={};
        if(installExactCall(base,&RECIPIENT_CALLS[i],(u64)&recipientNameHook,&duplicate)!=APPLY_CONFLICT)return 9840u;
        if(rollbackExactCall(base,&RECIPIENT_CALLS[i],&g_recipientPatches[i])!=APPLY_FAILED||
           !protectionMatchesAfterOwnedWrite(base+RECIPIENT_CALLS[i].windowRva,g_recipientPatches[i].protection))return 9841u;
        VirtualFree(g_recipientPatches[i].relay,0,MEM_RELEASE);
    }
    if(!validateRecipientMeter(&image))return 9842u;
    TlsFree(g_recipientTls);g_recipientTls=0xFFFFFFFFu;_InterlockedExchange(&g_recipientReady,0);
    memset(g_recipientPatches,0,sizeof(g_recipientPatches));memset(g_recipientCache,0,sizeof(g_recipientCache));
    // Collision at site three restores both preceding sites, readiness off.
    HudDistancePatch conflict={};
    if(installExactCall(base,&RECIPIENT_CALLS[2],(u64)&recipientLikesHook,&conflict)!=APPLY_READY)return 9843u;
    if(installRecipientMeter(base)!=APPLY_CONFLICT||g_recipientReady||
       !bytesEqual(base+RECIPIENT_CALLS[0].windowRva,RECIPIENT_CALLS[0].expected,8u)||
       !bytesEqual(base+RECIPIENT_CALLS[1].windowRva,RECIPIENT_CALLS[1].expected,8u))return 9844u;
    if(rollbackExactCall(base,&RECIPIENT_CALLS[2],&conflict)!=APPLY_FAILED||!validateRecipientMeter(&image))return 9845u;
    VirtualFree(conflict.relay,0,MEM_RELEASE);VirtualFree(g_recipientPatches[0].relay,0,MEM_RELEASE);
    VirtualFree(g_recipientPatches[1].relay,0,MEM_RELEASE);
    TlsFree(g_recipientTls);g_recipientTls=0xFFFFFFFFu;
    memset(g_recipientPatches,0,sizeof(g_recipientPatches));
    return 0;
}
