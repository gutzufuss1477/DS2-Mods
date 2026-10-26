u32 peerSyncCalls=0,peerClearCalls=0;u64 peerArt=0;
int peerPrepare(){return 1;}int peerPassive(){return 1;}
void peerSync(u64 art,int active){if(active){++peerSyncCalls;peerArt=art;}else ++peerClearCalls;}
void companionTests(){
 AtlasBootCompanionV1 valid={56,1,crafting_addon::NativeLayout,103,GraphPayloadSHA,(void*)&peerSync,&peerPrepare,&peerSync,&peerPassive};
 check(bootCompanionShape(valid,GraphPayloadSHA));
 auto bad=valid;bad.version=2;check(!bootCompanionShape(bad,GraphPayloadSHA));
 bad=valid;bad.size=48;check(!bootCompanionShape(bad,GraphPayloadSHA));
 bad=valid;bad.nativeLayout++;check(!bootCompanionShape(bad,GraphPayloadSHA));
 bad=valid;bad.atlasBootId=104;check(!bootCompanionShape(bad,GraphPayloadSHA));
 bad=valid;bad.syncMaterials=nullptr;check(!bootCompanionShape(bad,GraphPayloadSHA));
 check(!bootCompanionShape(valid,GameSHA));
 check(!compatibleBootModule(nullptr));
 bootsEnabled=true;bootCompanion=valid;bootCompanionReady=true;peerSyncCalls=peerClearCalls=0;
 fixture(false);component[0x51]=AtlasId;
 check(syncBody(component,true)&&peerSyncCalls==1&&peerArt==u64(bodyArt));
 component[0x51]=11;check(syncBody(component,true)&&peerSyncCalls==1&&peerClearCalls==1);
 bootsEnabled=false;syncBootCompanion(u64(bodyArt),true);check(peerSyncCalls==1);
 bootsEnabled=true;bootCompanionReady=false;syncBootCompanion(u64(bodyArt),true);check(peerSyncCalls==1);
 bootCompanion={};fixture(false);
}
