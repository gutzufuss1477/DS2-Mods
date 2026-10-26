constexpr u32 VisibilityRva=0x22A8D0;
void(*chainVisibility)(u64,i32,u8)=nullptr;
void(*setControllerVisibility)(u64,u8)=nullptr;
struct BodyMap {
 u64 owner,entity,resource,controllers,render,originalController[2],replacementController[2];
 i32 original[2],replacement[2];volatile long active;
};
BodyMap bodyMaps[64]={};
void* bodyMapLock=nullptr;
bool validBody(const BodyMap&m){
 return m.owner&&read<u64>(m.owner+0x30)==m.resource&&read<u64>(m.owner+0x58)==m.controllers&&
        read<u64>(m.owner+0x48)==m.entity&&read<u64>(m.entity+0xd8)==m.owner&&
        read<u64>(m.entity+0xc8)==m.render&&read<u64>(m.resource+0x10)==0x99427894a809558dull&&
        read<u64>(m.resource+0x18)==0xd1d869139c5e09b4ull;
}
bool exoVisibility(u64,i32,u8);
void visibility(u64 model,i32 part,u8 enabled){
 if(exoVisibility(model,part,enabled))return;
 BodyMap snapshot={};bool found=false;
 AcquireSRWLockShared(&bodyMapLock);
 for(auto&m:bodyMaps)if(m.render==model&&m.active){snapshot=m;found=true;break;}
 ReleaseSRWLockShared(&bodyMapLock);
 if(found&&validBody(snapshot)){
  for(u32 j=0;j<2;++j)if(part==snapshot.original[j]){
   setControllerVisibility(snapshot.replacementController[j],enabled);chainVisibility(model,part,0);return;
  }
 }
 chainVisibility(model,part,enabled);
}
bool findBody(u64 art,BodyMap&out){
 u64 resource=read<u64>(art+0x30);
 if(read<u64>(resource+0x10)!=0x99427894a809558dull||read<u64>(resource+0x18)!=0xd1d869139c5e09b4ull)return false;
 i32 count=read<i32>(resource+0x88);u64 names=read<u64>(resource+0x90);
 if(count!=(exoEnabled?167:166))return false;
 u32 hashes[167];if(!readmem(names,hashes,count*4))return false;
 i32 indices[4]={-1,-1,-1,-1};const u32 wanted[]={1838528733,718898234,0x6c1a0005,0x6c1a0006};
 for(i32 i=0;i<count;++i)for(u32 j=0;j<4;++j)if(hashes[i]==wanted[j]){if(indices[j]>=0)return false;indices[j]=i;}
 for(auto i:indices)if(i<0)return false;
 // RVA 0x33dc40 stores Entity* at controller+8; 0x3478e0 obtains
 // its render component from Entity+0xc8, not ArtPartsDataComponent+0xc8.
 u64 ownerEntity=read<u64>(art+0x48),controllers=read<u64>(art+0x58);
 u64 render=read<u64>(ownerEntity+0xc8);
 if(!ownerEntity||!controllers||!render||read<u64>(ownerEntity+0xd8)!=art)return false;
 out.owner=art;out.entity=ownerEntity;out.resource=resource;out.controllers=controllers;out.render=render;
 for(u32 j=0;j<4;++j){
  u64 c=controllers+u64(indices[j])*0x30;
  i32 index=read<i32>(c+0x28);
  if(read<u64>(c+8)!=ownerEntity||index<0||index>=55)return false;
  if(j<2){out.original[j]=index;out.originalController[j]=c;}
  else{out.replacement[j-2]=index;out.replacementController[j-2]=c;}
 }
 return validBody(out);
}
bool syncBody(void*component,bool apply){
 const u64 entity=read<u64>(u64(component)+0x48);
 u64 parent=read<u64>(entity+0x80);BodyMap found={};bool success=false;
 for(u32 depth=0;parent&&depth<4;++depth){
  if(findBody(read<u64>(parent+0xd8),found)){success=true;break;}
  parent=read<u64>(parent+0x80);
 }
 if(!success)return false;
 AcquireSRWLockExclusive(&bodyMapLock);
 BodyMap* target=nullptr;
 for(auto&m:bodyMaps)if(m.owner==found.owner&&m.render==found.render){target=&m;break;}
 if(!target)for(auto&m:bodyMaps)if(!m.owner||!validBody(m)){target=&m;break;}
 if(!target){ReleaseSRWLockExclusive(&bodyMapLock);return false;}
 _InterlockedExchange(&target->active,0);
 target->owner=found.owner;target->entity=found.entity;target->resource=found.resource;target->controllers=found.controllers;
 for(u32 j=0;j<2;++j){target->original[j]=found.original[j];target->replacement[j]=found.replacement[j];
  target->originalController[j]=found.originalController[j];target->replacementController[j]=found.replacementController[j];}
 target->render=found.render;
 bool active=apply&&(read<u8>(u64(component)+0x51)==AtlasId||read<u8>(u64(component)+0x52)==AtlasId);
 _InterlockedExchange(&target->active,active?1:0);
 BodyMap snapshot=*target;
 ReleaseSRWLockExclusive(&bodyMapLock);
 syncBootCompanion(snapshot.owner,active);
 if(chainVisibility&&setControllerVisibility)for(u32 j=0;j<2;++j){
  const u8 enabled=read<u8>(snapshot.originalController[j]+0x2d)?1:0;
  chainVisibility(snapshot.render,snapshot.original[j],active?0:enabled);
  setControllerVisibility(snapshot.replacementController[j],active?enabled:0);
 }
 return true;
}
