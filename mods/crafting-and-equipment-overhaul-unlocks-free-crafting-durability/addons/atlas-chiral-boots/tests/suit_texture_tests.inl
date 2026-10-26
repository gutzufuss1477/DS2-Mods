alignas(8)u8 txModel[0x210],txScene[0x120],txParts[55*0x1b0],txHandles[4][32],txMap[4*16],txResources[4][64];
SuitShaderRecord txRecords[4][16];u32 txCreates,txUpdates,txDeletes,txDirty,txBindings;bool txWrong;
u32 txReleases=0;bool txReleaseWrong=false;
void mockTxRelease(u64 texture){++txReleases;u32 refs=read<u32>(texture+8);
 if(!(refs&0x3fffff))txReleaseWrong=true;else at<u32>((void*)texture,8)=refs-1;}
bool txDestination(i32 part){return part==read<i32>(u64(txMap)+32)||part==read<i32>(u64(txMap)+48);}
u64 mockTxGet(u64 model,i32 part){if(model!=u64(txModel)||!txDestination(part))txWrong=true;u64 h=0;suitModelHandle(model,part,h);return h;}
i32 mockTxAlloc(u64 h,const u32*hash){++txCreates;u64 data=read<u64>(h+0x18);i32 n=read<i32>(h+0x10),slot=-1;
 for(i32 i=0;i<n;++i)if(!read<u32>(data+i*0x48)){slot=i;break;}
 if(slot<0)slot=n++;at<i32>((void*)h,0x10)=n;at<u32>((void*)(data+slot*0x48),0)=*hash;return slot;}
void mockTxBind(u64,u32,u64){++txBindings;}
void mockTxUpdate(u64 model,SuitShaderId*id,u64 texture,unsigned short sampler){
 ++txUpdates;if(model!=u64(txModel)||!txDestination(id->part))txWrong=true;
 u64 h=0;suitModelHandle(model,id->part,h);u64 r=read<u64>(h+0x18)+id->slot*0x48;
 if(read<u32>(r)!=id->parameter||read<u32>(r+0x28)!=5)txWrong=true;
 at<u64>((void*)r,0x30)=texture;at<unsigned short>((void*)r,0x40)=sampler;
}
void mockTxReset(u64 h,SuitShaderId*id,u8 reset){++txDeletes;if(reset||!txDestination(id->part))txWrong=true;
 u64 r=read<u64>(h+0x18)+id->slot*0x48;at<u32>((void*)r,0)=0;at<u64>((void*)r,0x30)=0;}
void mockTxDirty(u64 model,u32 flags){++txDirty;if(model!=u64(txModel)||flags!=0x8000)txWrong=true;}
void txEntry(u32 part,u32 index,u32 hash,u64 texture,unsigned short sampler=7,u32 kind=5){
 auto&b=txRecords[part][index];memset(&b,0,sizeof(b));at<u32>(b.raw,0)=hash;at<u32>(b.raw,0x28)=kind;
 at<u64>(b.raw,0x30)=texture;at<unsigned short>(b.raw,0x40)=sampler;
 if(read<i32>(u64(txHandles[part])+0x10)<=i32(index))at<i32>(txHandles[part],0x10)=index+1;
}
BodyMap textureFixture(bool liveIndices=false){
 auto m=suitFixture();memset(txModel,0,sizeof(txModel));memset(txScene,0,sizeof(txScene));memset(txParts,0,sizeof(txParts));
 memset(txHandles,0,sizeof(txHandles));memset(txRecords,0,sizeof(txRecords));memset(txResources,0,sizeof(txResources));
 at<u64>((void*)m.entity,0x70)=u64(txModel);at<u64>(txModel,0x90)=u64(txScene);
 at<i32>(txScene,0x40)=55;at<u64>(txScene,0x48)=u64(txParts);
 at<i32>(txModel,0x1e8)=4;at<i32>(txModel,0x1ec)=4;at<u64>(txModel,0x1f0)=u64(txMap);
 const i32 indices[]={liveIndices?17:19,liveIndices?18:20,liveIndices?48:53,liveIndices?49:54};
 if(liveIndices){for(u32 j=0;j<2;++j){m.original[j]=indices[j];m.replacement[j]=indices[j+2];
  at<i32>((void*)m.originalController[j],0x28)=indices[j];at<i32>((void*)m.replacementController[j],0x28)=indices[j+2];}}
 for(u32 i=0;i<4;++i){
  at<i32>(txMap,i*16)=indices[i];at<u64>(txMap,i*16+8)=u64(txHandles[i]);
  at<i32>(txHandles[i],8)=indices[i];at<i32>(txHandles[i],0x14)=16;at<u64>(txHandles[i],0x18)=u64(txRecords[i]);
  at<u64>(txResources[i],0)=u64(txResources);at<i32>(txResources[i],8)=1;
 }
 suitGetModel=mockTxGet;suitAllocOverride=mockTxAlloc;suitBindOverride=mockTxBind;
 suitUpdateTexture=mockTxUpdate;suitResetOverride=mockTxReset;suitModelDirty=mockTxDirty;
 suitPinnedCount=0;memset(suitPinnedTextures,0,sizeof(suitPinnedTextures));memset(textureOwners,0,sizeof(textureOwners));
 textureRelease=mockTxRelease;texturePinsDirty=false;textureLastSweep=0;txReleases=0;txReleaseWrong=false;
 txCreates=txUpdates=txDeletes=txDirty=txBindings=0;txWrong=false;return m;
}
void suitTextureTests(){
 auto m=textureFixture();txEntry(0,0,0x780740de,u64(txResources[0]));txEntry(0,1,0x52bf1c63,u64(txResources[1]));
 txEntry(0,2,0x32d5ae44,0,0,1);txEntry(1,0,0x58e13a70,0,0,3);
 check(syncSuitTexturesInner(m)&&txCreates==2&&txUpdates==2&&txDirty==1&&!txWrong);
 check(read<u64>(u64(txRecords[2][0].raw)+0x30)==u64(txResources[0])&&suitPinnedCount==2);
 check(read<i32>(u64(txResources[0])+8)==2&&read<i32>(u64(txResources[1])+8)==2);
 check(txScene[0x109]==1&&txParts[54*0x1b0+0x1a1]==1&&read<u32>(u64(txModel)+0x1b0)==0x80000);
 check(syncSuitTexturesInner(m)&&txCreates==2&&txUpdates==2&&txDirty==1&&suitPinnedCount==2); // stable frames: zero writes
 check(read<u32>(u64(txRecords[0][0].raw))==0x780740de&&read<u64>(u64(txRecords[0][0].raw)+0x30)==u64(txResources[0]));
 txEntry(0,0,0x780740de,u64(txResources[2]));check(syncSuitTexturesInner(m)&&txCreates==2&&txUpdates==3&&txDirty==2);
 txEntry(0,0,0x780740de,u64(txResources[2]),9);check(syncSuitTexturesInner(m)&&txUpdates==5&&txDirty==3);
 at<u32>(txRecords[0][1].raw,0)=0;check(syncSuitTexturesInner(m)&&txDeletes==1&&txDirty==4);
 check(syncSuitTexturesInner(m)&&txDeletes==1&&txDirty==4);
 // Reuse an erased native slot without confusing record index and part index.
 txEntry(0,1,0x079c2657,u64(txResources[3]));check(syncSuitTexturesInner(m)&&txCreates==3&&!txWrong);
 m=textureFixture();txEntry(0,0,123,1);check(!syncSuitTexturesInner(m)&&txCreates==0&&txDirty==0);
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));suitPinnedCount=MaxSuitPinnedTextures;
 memset(suitPinnedTextures,0,sizeof(suitPinnedTextures));check(!syncSuitTexturesInner(m)&&txCreates==0);
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));at<u64>((void*)m.owner,0x30)=0;
 check(!syncSuitTexturesInner(m)&&txCreates==0); // stale/foreign owner
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));at<i32>(txModel,0x1e8)=129;
 check(!syncSuitTexturesInner(m)&&txCreates==0);
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));txEntry(0,1,123,u64(txResources[1]));
 check(!syncSuitTexturesInner(m)&&txCreates==0); // duplicate shader keys
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));txEntry(2,0,123,0,0,1);
 check(!syncSuitTexturesInner(m)&&txCreates==0&&txDirty==0); // type collision
 m=textureFixture();at<i32>(txScene,0x40)=56;check(!syncSuitTexturesInner(m)&&txCreates==0);
 m=textureFixture();m.replacement[0]=19;check(!syncSuitTexturesInner(m)&&txCreates==0);
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));at<i32>(txResources[0],8)=0;
 check(!syncSuitTexturesInner(m)&&txCreates==0);
 m=textureFixture();txEntry(0,0,123,u64(txResources[0]));m.active=0;syncSuitMaterials(m);check(txCreates==0);
 m.active=1;suitBusy=1;syncSuitMaterials(m);check(txCreates==0);suitBusy=0;
 // Companion reaches exactly the same path while N02 owns the combined model.
 companionMode=true;overlayState=0;at<i32>(bodyResource,0x88)=167;bodyNames[59]=851113522;bodyNames[64]=0x6e010001;
 companionSuitMaterials(u64(bodyArt),1);check(txCreates==1&&!txWrong);
 companionMode=false;m=textureFixture(true);
 // Exact eight missing bindings from the read-only game snapshot, with the
 // actual renderer indices 17/18 -> 48/49 (controller indices remain 19/20 -> 53/54).
 const u32 liveHashes[]={0x780740de,0x52bf1c63,0x079c2657,0x2dd66b76,0x3a825aaa,0x2887aad2,0x72f6401e,0x39ebd013};
 for(u32 i=0;i<8;++i)txEntry(0,i,liveHashes[i],u64(txResources[i%4]),1672);
 check(syncSuitTexturesInner(m)&&txCreates==8&&txUpdates==8&&suitPinnedCount==4&&!txWrong);
 check(syncSuitTexturesInner(m)&&txUpdates==8&&txDirty==1);
 m=textureFixture();txEntry(0,0,123,0,1672);check(syncSuitTexturesInner(m)&&txCreates==1&&suitPinnedCount==0);
 check(read<unsigned short>(u64(txRecords[2][0].raw)+0x40)==1672);
 companionMode=false;suitGetModel=nullptr;suitAllocOverride=nullptr;suitBindOverride=nullptr;
 suitUpdateTexture=nullptr;suitResetOverride=nullptr;suitModelDirty=nullptr;fixture(false);
}
