alignas(8)u8 bpEntity[0x110],bpArt[0x100],bpResource[0xc0],bpSelection[0x60],bpComponent[0x80];
alignas(8)u8 bpControllers[54*0x30],bpHandles[5][32],bpTable[5*16];
u64 bpComponents[1];u32 bpNames[54];SuitShaderRecord bpRecords[5][16];
bool bpWrong=false;u32 bpCalls=0;
u64 bpGet(u64 model,i32 part){if(model!=u64(txModel)||part<50||part>53)bpWrong=true;u64 h=0;suitModelHandle(model,part,h);return h;}
void bpUpdate(u64 model,SuitShaderId*id,u64 texture,unsigned short sampler){
 ++bpCalls;if(model!=u64(txModel)||id->part<50||id->part>53||
   (id->parameter!=BootPaintKeys[0]&&id->parameter!=BootPaintKeys[1]&&id->parameter!=BootPaintKeys[2]))bpWrong=true;
 u64 h=0;suitModelHandle(model,id->part,h);u64 r=read<u64>(h+0x18)+id->slot*0x48;
 if(read<u32>(r)!=id->parameter||read<u32>(r+0x28)!=5)bpWrong=true;
 at<u64>((void*)r,0x30)=texture;at<unsigned short>((void*)r,0x40)=sampler;
}
void bpDirty(u64 model,u32 flags){++txDirty;if(model!=u64(txModel)||flags!=0x8000)bpWrong=true;}
void bpEntry(u32 which,u32 slot,u32 hash,u64 value,u32 kind=5){
 auto&r=bpRecords[which][slot];memset(&r,0,sizeof(r));at<u32>(r.raw,0)=hash;at<u32>(r.raw,0x28)=kind;
 at<u64>(r.raw,0x30)=value;at<unsigned short>(r.raw,0x40)=1672;
 at<i32>(bpHandles[which],0x10)=slot+1;
}
BodyMap bpFixture(){
 auto m=textureFixture();memset(bootPaintMaps,0,sizeof(bootPaintMaps));memset(bpEntity,0,sizeof(bpEntity));
 memset(bpArt,0,sizeof(bpArt));memset(bpResource,0,sizeof(bpResource));memset(bpSelection,0,sizeof(bpSelection));
 memset(bpComponent,0,sizeof(bpComponent));memset(bpControllers,0,sizeof(bpControllers));
 memset(bpHandles,0,sizeof(bpHandles));memset(bpRecords,0,sizeof(bpRecords));memset(bpNames,0,sizeof(bpNames));
 at<u64>((void*)m.entity,0x88)=u64(bpEntity);at<u64>(bpEntity,0x80)=m.entity;
 at<u64>(bpEntity,0xd8)=u64(bpArt);at<u64>(bpEntity,0x70)=u64(txModel);
 at<u64>(bpArt,0x30)=u64(bpResource);at<u64>(bpArt,0x48)=u64(bpEntity);at<u64>(bpArt,0x58)=u64(bpControllers);
 at<u64>(bpResource,0x10)=0xb4414c0cb2eb5bffull;at<u64>(bpResource,0x18)=0x03b47deefa0cbab7ull;
 at<i32>(bpResource,0x88)=54;at<u64>(bpResource,0x90)=u64(bpNames);
 at<i32>(bpEntity,0xa0)=at<i32>(bpEntity,0xa4)=1;at<u64>(bpEntity,0xa8)=u64(bpComponents);bpComponents[0]=u64(bpComponent);
 at<u64>(bpComponent,0x30)=u64(bpSelection);at<u64>(bpComponent,0x48)=u64(bpEntity);
 bpComponent[0x51]=bpComponent[0x52]=AtlasId;
 at<u64>(bpSelection,0x10)=0x4c41485249434800ull;at<u64>(bpSelection,0x18)=0xa511c0ff20261020ull;
 for(u32 i=0;i<4;++i){bpNames[50+i]=BootPaintNames[i];at<u64>(bpControllers,(50+i)*0x30+8)=u64(bpEntity);at<i32>(bpControllers,(50+i)*0x30+0x28)=50+i;}
 at<i32>(txScene,0x40)=54;at<i32>(txModel,0x1e8)=at<i32>(txModel,0x1ec)=5;at<u64>(txModel,0x1f0)=u64(bpTable);
 for(u32 i=0;i<5;++i){i32 part=i?49+i:-1;at<i32>(bpTable,i*16)=part;at<u64>(bpTable,i*16+8)=u64(bpHandles[i]);
  at<i32>(bpHandles[i],8)=part;at<i32>(bpHandles[i],0x14)=16;at<u64>(bpHandles[i],0x18)=u64(bpRecords[i]);}
 bpEntry(0,0,0x54e8f071,0x3f800000,1);bpEntry(0,1,BootPaintKeys[0],u64(txResources[1]));
 suitGetModel=bpGet;suitUpdateTexture=bpUpdate;suitModelDirty=bpDirty;bpWrong=false;bpCalls=0;bootPaintReady=false;
 return m;
}
void bootPaintTests(){
 auto m=bpFixture();auto global0=bpRecords[0][0],global1=bpRecords[0][1];
 bpEntry(1,0,0x780740de,u64(txResources[2]));auto base=bpRecords[1][0];
 check(syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==12&&txDirty==1&&!bpWrong);
 check(same(&global0,&bpRecords[0][0],sizeof(global0))&&same(&global1,&bpRecords[0][1],sizeof(global1)));
 check(same(&base,&bpRecords[1][0],sizeof(base))); // actual gold/base-color binding survives
 check(suitPinnedCount==1&&read<i32>(u64(txResources[0])+8)==2);
 check(syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==12&&txDirty==1); // no per-frame rewrite
 check(syncBootPaintInner(m,u64(txResources[0]),9)&&bpCalls==36&&txDirty==2); // native sampler update requires reset
 check(syncBootPaintInner(m,u64(txResources[3]),9)&&bpCalls==48&&suitPinnedCount==2&&!bpWrong);
 m=bpFixture();bpComponent[0x52]=1;check(syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==6);
 check(read<i32>(u64(bpHandles[3])+0x10)==0&&read<i32>(u64(bpHandles[4])+0x10)==0); // native right boot untouched
 m=bpFixture();bpComponent[0x51]=1;check(syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==6);
 check(read<i32>(u64(bpHandles[1])+0x10)==0&&read<i32>(u64(bpHandles[2])+0x10)==0);
 m=bpFixture();bpComponent[0x51]=bpComponent[0x52]=1;check(syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();m.active=0;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();check(!syncBootPaintInner(m,0,1672)&&bpCalls==0);
 m=bpFixture();at<i32>(txResources[0],8)=0;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();at<u64>(bpEntity,0x80)=1;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();bpNames[50]=0x6e010001;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0); // N02/other names refused
 m=bpFixture();at<u64>(bpResource,0x10)^=1;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();at<u64>(bpSelection,0x10)|=1;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0); // hung selection refused
 m=bpFixture();at<i32>(bpResource,0x88)=53;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();at<i32>(bpControllers,50*0x30+0x28)=0;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();at<i32>(txScene,0x40)=55;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();at<i32>(bpEntity,0xa0)=25;check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();bpEntry(1,0,BootPaintKeys[0],0x3f800000,1);check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0);
 m=bpFixture();BootPaintMap found={};check(bootPaintFind(m,found));at<u64>(bpEntity,0xd8)=0;
 check(!syncBootPaintInner(m,u64(txResources[0]),1672)&&bpCalls==0); // stale cached owner
 m=bpFixture();at<u64>(bpResource,0x10)=0;at<u64>(bpEntity,0x90)=u64(bpEntity);
 check(!bootPaintFind(m,found)); // bounded cyclic sibling list
 m=bpFixture();check(!prepareBootPaint()); // non-game process cannot enable native path
 at<u64>((void*)m.entity,0x88)=0;bootPaintReady=false;suitGetModel=nullptr;suitUpdateTexture=nullptr;suitModelDirty=nullptr;
 memset(bootPaintMaps,0,sizeof(bootPaintMaps));fixture(false);
}
