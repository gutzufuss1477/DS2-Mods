alignas(8)u8 fakeSuitComponent[0x90],fakeRemovals[128*0x10];
SuitParameter fakeSuitTables[2][128];u32 fakeSuitBank=0,suitSets=0,suitRemoves=0,suitQueues=0;
bool suitUnexpectedWrite=false;
void sortSuit(SuitParameter*p,i32 count){
 for(i32 i=1;i<count;++i)for(i32 j=i;j>0&&p[j].key<p[j-1].key;--j){auto tmp=p[j];p[j]=p[j-1];p[j-1]=tmp;}
}
SuitParameter parameter(u32 part,u32 hash,u32 value,u8 count=1){
 SuitParameter e={};e.key=(u64(hash)<<32)|part;e.parameter=hash;e.count=count;
 for(u32 i=0;i<4;++i)e.values[i]=value+i;
 e.controller=part==SuitSourceNames[0]?19:part==SuitSourceNames[1]?20:part==SuitTargetNames[0]?53:part==SuitTargetNames[1]?54:0;
 return e;
}
u64 fakeSuitSet(u64 component,const u32*hash,const void*values,i32 count,const u32*part){
 ++suitSets;if(component!=u64(fakeSuitComponent)||(*part!=SuitTargetNames[0]&&*part!=SuitTargetNames[1]))suitUnexpectedWrite=true;
 // Deliberately move the entire native table on EVERY call and poison the old one.
 i32 n=read<i32>(component+0x50);u32 previous=fakeSuitBank;fakeSuitBank^=1;
 auto*p=fakeSuitTables[fakeSuitBank];copy(p,fakeSuitTables[previous],n*sizeof(*p));
 memset(fakeSuitTables[previous],0xcc,sizeof(fakeSuitTables[previous]));
 at<u64>((void*)component,0x58)=u64(p);u64 key=(u64(*hash)<<32)|*part;i32 index=-1;
 for(i32 i=0;i<n;++i)if(p[i].key==key)index=i;
 if(index<0)index=n++;
 p[index]=parameter(*part,*hash,0,u8(count));copy(p[index].values,values,count*4);
 at<i32>((void*)component,0x50)=n;sortSuit(p,n);return 1;
}
u64 fakeSuitRemove(u64 map,const u64*key,const u32*hash){
 ++suitRemoves;if(map!=u64(fakeSuitComponent)+0x60||(*key>>32)!=*hash||
   (u32(*key)!=SuitTargetNames[0]&&u32(*key)!=SuitTargetNames[1]))suitUnexpectedWrite=true;
 SuitParameter e={};u64 p=suitFind(u64(fakeSuitComponent),*key,e);
 if(!p||!e.removed||e.applied)suitUnexpectedWrite=true;return 1;
}
void fakeSuitQueue(u64 component,const u64*cb){
 ++suitQueues;if(component!=u64(fakeSuitComponent)||cb[0]!=image+SuitDirtyRva||cb[1])suitUnexpectedWrite=true;
}
BodyMap suitFixture(){
 companionMode=false;fixture(false);component[0x51]=AtlasId;BodyMap m={};check(findBody(u64(bodyArt),m));m.active=1;
 memset(fakeSuitComponent,0,sizeof(fakeSuitComponent));memset(fakeSuitTables,0,sizeof(fakeSuitTables));
 at<u64>(bodyArt,0xa0)=u64(fakeSuitComponent);at<u64>(fakeSuitComponent,0x78)=u64(bodyArt);
 at<i32>(fakeSuitComponent,0x54)=128;at<u64>(fakeSuitComponent,0x58)=u64(fakeSuitTables[0]);
 at<i32>(fakeSuitComponent,0x64)=128;at<u64>(fakeSuitComponent,0x68)=u64(fakeRemovals);
 suitSet=fakeSuitSet;suitRemove=fakeSuitRemove;suitQueue=fakeSuitQueue;
 fakeSuitBank=suitSets=suitRemoves=suitQueues=0;suitBusy=0;suitUnexpectedWrite=false;return m;
}
void suitPush(SuitParameter e){i32 n=at<i32>(fakeSuitComponent,0x50);fakeSuitTables[fakeSuitBank][n++]=e;
 at<i32>(fakeSuitComponent,0x50)=n;sortSuit(fakeSuitTables[fakeSuitBank],n);}
SuitParameter suitGet(u32 part,u32 hash){SuitParameter e={};suitFind(u64(fakeSuitComponent),(u64(hash)<<32)|part,e);return e;}
void suitMaterialTests(){
 auto m=suitFixture();const auto color=parameter(SuitSourceNames[0],0x123,0x3f123456,4);
 const auto metal=parameter(SuitSourceNames[1],0x456,0x3f800000);
 const auto global=parameter(0,0x789,0x3eabcdef,3),foreign=parameter(0x6c1a0001,0x123,0x3f000000);
 suitPush(color);suitPush(metal);suitPush(global);suitPush(foreign);
 check(syncSuitMaterialsInner(m)&&suitSets==2&&!suitUnexpectedWrite);
 auto copied=suitGet(SuitTargetNames[0],0x123);
 check(copied.count==4&&same(copied.values,color.values,16)&&copied.controller==53);
 copied=suitGet(SuitTargetNames[1],0x456);check(copied.count==1&&copied.values[0]==metal.values[0]&&copied.controller==54);
 auto source=suitGet(SuitSourceNames[0],0x123);check(same(&source,&color,sizeof(source)));
 source=suitGet(0,0x789);check(same(&source,&global,sizeof(source)));
 source=suitGet(0x6c1a0001,0x123);check(same(&source,&foreign,sizeof(source)));
 check(syncSuitMaterialsInner(m)&&suitSets==2&&suitRemoves==0); // idempotent, no per-frame native writes
 // A palette change: propagate exact metallic/RGB bits, not a guessed color formula.
 SuitParameter e={};u64 p=suitFind(u64(fakeSuitComponent),color.key,e);at<u32>((void*)p,0xc)=0x3f765432;
 check(syncSuitMaterialsInner(m)&&suitSets==3&&suitGet(SuitTargetNames[0],0x123).values[0]==0x3f765432);
 // Native reset and delayed removal must also clear only the corresponding private part.
 p=suitFind(u64(fakeSuitComponent),color.key,e);at<u8>((void*)p,0x25)=1;
 check(syncSuitMaterialsInner(m)&&suitRemoves==1&&suitQueues==1&&!suitUnexpectedWrite);
 check(suitGet(SuitTargetNames[0],0x123).removed&& !suitGet(SuitTargetNames[1],0x456).removed);
 check(syncSuitMaterialsInner(m)&&suitRemoves==1&&suitQueues==1);
 // Re-enable before native removal has drained; native setter must cancel the pending reset.
 p=suitFind(u64(fakeSuitComponent),color.key,e);at<u8>((void*)p,0x25)=0;
 check(syncSuitMaterialsInner(m)&&suitSets==4&&!suitGet(SuitTargetNames[0],0x123).removed);
 // A source already removed from the native table leaves only a stale private override.
 m=suitFixture();suitPush(parameter(SuitTargetNames[0],0x555,0x1234));
 check(syncSuitMaterialsInner(m)&&suitRemoves==1&&suitSets==0&&!suitUnexpectedWrite);
 m=suitFixture();suitPush(color);m.active=0;syncSuitMaterials(m);check(suitSets==0);
 m.active=1;suitBusy=1;syncSuitMaterials(m);check(suitSets==0);suitBusy=0;
 syncSuitMaterials(m);check(suitSets==1&&suitBusy==0);
 m=suitFixture();suitPush(color);at<u64>(fakeSuitComponent,0x78)=u64(entity);
 check(!syncSuitMaterialsInner(m)&&suitSets==0); // foreign owner
 m=suitFixture();suitPush(color);at<u64>(bodyResource,0x10)^=1;
 check(!syncSuitMaterialsInner(m)&&suitSets==0); // another body
 m=suitFixture();suitPush(color);at<i32>(fakeSuitComponent,0x50)=513;
 check(!syncSuitMaterialsInner(m)&&suitSets==0);
 m=suitFixture();suitPush(color);at<u64>(fakeSuitComponent,0x58)=1;
 check(!syncSuitMaterialsInner(m)&&suitSets==0);
 m=suitFixture();e=color;e.count=5;suitPush(e);
 check(!syncSuitMaterialsInner(m)&&suitSets==0); // refuse overflowing value array
 m=suitFixture();e=color;e.parameter^=1;suitPush(e);
 check(!syncSuitMaterialsInner(m)&&suitSets==0);
 m=suitFixture();e=color;e.controller=53;suitPush(e);
 check(!syncSuitMaterialsInner(m)&&suitSets==0);
 m=suitFixture();suitPush(color);suitPush(color);
 check(!syncSuitMaterialsInner(m)&&suitSets==0);
 m=suitFixture();suitPush(color);at<i32>(fakeSuitComponent,0x60)=-1;
 check(!syncSuitMaterialsInner(m)&&suitSets==0);
 m=suitFixture();suitPush(color);suitSet=nullptr;syncSuitMaterials(m);check(suitSets==0);
 // The combined N02 body has 167 controllers; the native material callback must
 // use the same source data even though the boot module itself stays passive.
 m=suitFixture();suitPush(color);companionMode=true;overlayState=0;
 at<i32>(bodyResource,0x88)=167;bodyNames[59]=851113522;bodyNames[64]=0x6e010001;
 companionSuitMaterials(u64(bodyArt),1);
 check(suitSets==1&&suitGet(SuitTargetNames[0],0x123).values[0]==color.values[0]);
 m=suitFixture();suitPush(color);companionMode=true;overlayState=0;
 at<i32>(bodyResource,0x88)=167;bodyNames[59]=851113522;bodyNames[64]=0x6e010001;
 companionSuitMaterials(u64(bodyArt),0);check(suitSets==0);
 overlayState=2;companionSuitMaterials(u64(bodyArt),1);check(suitSets==0); // no second active owner
 overlayState=0;bodyNames[64]=0;companionSuitMaterials(u64(bodyArt),1);check(suitSets==0);
 companionMode=false;suitSet=nullptr;suitRemove=nullptr;suitQueue=nullptr;fixture(false);
 const auto*peer=DS2AtlasBootCompanionQueryV1(1);
 check(peer&&peer->size==56&&peer->nativeLayout==crafting_addon::NativeLayout&&peer->atlasBootId==103);
 check(peer&&sameDigest(peer->graphPayloadSHA256,GraphPayloadSHA)&&peer->storageOpenShim==(void*)&openStorageW);
 check(peer&&peer->prepareMaterials&&peer->syncMaterials&&peer->isPassive);
 check(!DS2AtlasBootCompanionQueryV1(0)&&!DS2AtlasBootCompanionQueryV1(2));
}
