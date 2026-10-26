// Stress the actual override copier and lifetime collector with changing texture
// resources. Fake native release records ownership; no game is launched here.
alignas(8)u8 lifeResources[600][64];SuitShaderRecord lifeMovedRecords[16];
void lifeBootReset(u64 handle,SuitShaderId*id,u8 reset){
 if(reset||id->part<50||id->part>53||(id->parameter!=BootPaintKeys[0]&&id->parameter!=BootPaintKeys[1]&&id->parameter!=BootPaintKeys[2]))bpWrong=true;
 u64 r=read<u64>(handle+0x18)+id->slot*0x48;at<u32>((void*)r,0)=0;at<u64>((void*)r,0x30)=0;++txDeletes;
}
void textureLifetimeTests(){
 auto m=textureFixture(true);memset(lifeResources,0,sizeof(lifeResources));bool ok=true;
 for(u32 i=0;i<600;++i){
  at<u64>(lifeResources[i],0)=u64(lifeResources);at<u32>(lifeResources[i],8)=0x400001;
  txEntry(0,0,0x780740de,u64(lifeResources[i]));
  if(!syncSuitTexturesInner(m)||!sweepTexturePins(true)||suitPinnedCount!=1||
    read<u32>(u64(lifeResources[i])+8)!=0x400002)ok=false;
  if(i&&read<u32>(u64(lifeResources[i-1])+8)!=0x400001)ok=false;
 }
 check(ok&&txReleases==599&&!txReleaseWrong&&!txWrong); // old 256-pin lifetime cap exceeded safely
 // A native vector can move without invalidating resource ownership.
 copy(lifeMovedRecords,txRecords[2],sizeof(lifeMovedRecords));at<u64>(txHandles[2],0x18)=u64(lifeMovedRecords);
 check(sweepTexturePins(true)&&suitPinnedCount==1&&txReleases==599);
 m.active=0;syncSuitMaterials(m);
 check(suitPinnedCount==0&&txReleases==600&&read<u32>(u64(lifeResources[599])+8)==0x400001);
 check(at<u64>(txRecords[0][0].raw,0x30)==u64(lifeResources[599])&&!txWrong); // original source survives
 m=textureFixture();txEntry(0,0,1,u64(txResources[0]));txEntry(1,0,2,u64(txResources[0]));
 check(syncSuitTexturesInner(m)&&sweepTexturePins(true)&&suitPinnedCount==1&&txReleases==0);
 at<u32>(txRecords[0][0].raw,0)=0;
 check(syncSuitTexturesInner(m)&&sweepTexturePins(true)&&suitPinnedCount==1&&txReleases==0); // still used by second part
 at<u32>(txRecords[1][0].raw,0)=0;
 check(syncSuitTexturesInner(m)&&sweepTexturePins(true)&&suitPinnedCount==0&&txReleases==1);
 m=textureFixture();txEntry(0,0,1,u64(txResources[0]));check(syncSuitTexturesInner(m));
 at<i32>(txHandles[2],0x10)=129;check(!sweepTexturePins(true)&&txReleases==0); // uncertain table retains refs
 at<i32>(txHandles[2],0x10)=1;check(sweepTexturePins(true)&&suitPinnedCount==1);
 at<u64>((void*)m.entity,0x18)^=1;
 check(sweepTexturePins(true)&&suitPinnedCount==0&&txReleases==1); // owner identity replaced
 m=textureFixture();txEntry(0,0,1,u64(txResources[0]));check(syncSuitTexturesInner(m));
 at<u32>(txRecords[2][0].raw,0)=0;texturePending(u64(txModel));at<i32>(txScene,0x40)=99;
 check(!sweepTexturePins(true)&&suitPinnedCount==1&&txReleases==0); // notification must precede release
 at<i32>(txScene,0x40)=55;check(sweepTexturePins(true)&&txReleases==1&&suitPinnedCount==0);
 m=textureFixture();txEntry(0,0,1,u64(txResources[0]));syncSuitTexturesInner(m);
 companionMode=true;overlayState=0;at<i32>(bodyResource,0x88)=167;bodyNames[59]=851113522;bodyNames[64]=0x6e010001;
 companionSuitMaterials(u64(bodyArt),0);
 check(suitPinnedCount==0&&txReleases==1&&!txWrong); // inactive signal in N02 ownership mode
 companionMode=false;m=bpFixture();suitResetOverride=lifeBootReset;
 bpEntry(1,0,0x780740de,u64(txResources[2]));auto gold=bpRecords[1][0],global=bpRecords[0][0];
 check(syncBootPaintInner(m,u64(txResources[0]),1672)&&sweepTexturePins(true)&&suitPinnedCount==1);
 m.active=0;syncSuitMaterials(m);
 check(txDeletes==12&&txReleases==1&&suitPinnedCount==0&&!bpWrong);
 check(same(&gold,&bpRecords[1][0],sizeof(gold))&&same(&global,&bpRecords[0][0],sizeof(global)));
 m=textureFixture();check(!prepareTextureLifetime());
 textureRelease=nullptr;suitGetModel=nullptr;suitUpdateTexture=nullptr;suitModelDirty=nullptr;fixture(false);
}
