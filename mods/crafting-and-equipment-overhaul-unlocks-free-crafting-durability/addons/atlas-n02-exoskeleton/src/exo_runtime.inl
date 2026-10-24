// Native evidence: DS2 1.10.89.0, EC7010 caches true equipment ID at +2de8.
// All controller and mesh mutations are restricted to the current Sam instance.
constexpr u32 ExoUpdateRva=0xEC7010,ExoOriginal=851113522,ExoPrivate=0x6e010001;
void(*nextExoUpdate)(u64,u64,u8)=nullptr;
struct ExoMap {u64 control,art,resource,controllers,entity,render,oldController,newController;i32 oldPart,newPart;};
ExoMap exoMaps[32]={};void*exoLock=nullptr;
void exoDiagnostic(const char*label,u64 value){
 char hex[19]="0x";const char digits[]="0123456789abcdef";
 for(u32 i=0;i<16;++i)hex[2+i]=digits[(value>>(60-i*4))&15];hex[18]=0;logValue(label,hex);
}
bool validExo(const ExoMap&m){
 return m.control&&read<u64>(m.art+0x30)==m.resource&&read<u64>(m.art+0x58)==m.controllers&&
  read<u64>(m.oldController+8)==m.entity&&read<u64>(m.newController+8)==m.entity&&
  read<u64>(m.entity+0xc8)==m.render&&read<i32>(m.oldController+0x28)==m.oldPart&&
  read<i32>(m.newController+0x28)==m.newPart;
}
bool exoVisibility(u64 render,i32 part,u8 enabled){
 if(!exoEnabled||overlayState!=2)return false;
 ExoMap m={};AcquireSRWLockShared(&exoLock);
 for(const auto&row:exoMaps)if(row.render==render&&row.oldPart==part){m=row;break;}
 ReleaseSRWLockShared(&exoLock);
 if(!validExo(m)||read<u8>(m.control+0x2de8)!=104)return false;
 setControllerVisibility(m.newController,enabled);chainVisibility(render,part,0);return true;
}
void updateExo(u64 control,u64 param,u8 force){
 nextExoUpdate(control,param,force);
 if(!exoEnabled||overlayState!=2)return;
 static volatile long entered=0;if(!_InterlockedExchange(&entered,1))log("N02_UPDATE_ENTERED: original native skeleton update chained.");
 const u64 sam=read<u64>(control+0x48),art=read<u64>(sam+0xd8),resource=read<u64>(art+0x30);
 if(read<u64>(resource+0x10)!=0x99427894a809558dull||read<u64>(resource+0x18)!=0xd1d869139c5e09b4ull||
    read<i32>(resource+0x88)!=167)return;
 const u64 names=read<u64>(resource+0x90),controllers=read<u64>(art+0x58);
 if(!names||!controllers)return;
 i32 indices[2]={-1,-1};
 for(i32 i=0;i<167;++i){const u32 name=read<u32>(names+4ull*i);if(name==ExoOriginal)indices[0]=i;else if(name==ExoPrivate)indices[1]=i;}
 if(indices[0]!=59||indices[1]!=64)return;
 ExoMap m={};m.control=control;m.art=art;m.resource=resource;m.controllers=controllers;
 m.oldController=controllers+48ull*indices[0];m.newController=controllers+48ull*indices[1];
 m.entity=read<u64>(m.oldController+8);m.render=read<u64>(m.entity+0xc8);
 m.oldPart=read<i32>(m.oldController+0x28);m.newPart=read<i32>(m.newController+0x28);
 if(!m.entity||!m.render||m.oldPart!=4||m.newPart!=9||!validExo(m)){
  static volatile long diagnostic=0;if(_InterlockedIncrement(&diagnostic)<=3){exoDiagnostic("N02_WAIT_OLD_PART=",u32(m.oldPart));exoDiagnostic("N02_WAIT_NEW_PART=",u32(m.newPart));}return;
 }
 bool stored=false;AcquireSRWLockExclusive(&exoLock);
 for(auto&row:exoMaps)if(row.control==control||!row.control||!validExo(row)){row=m;stored=true;break;}
 ReleaseSRWLockExclusive(&exoLock);if(!stored)return;
 const bool atlas=read<u8>(control+0x2de8)==104;
 const u8 enabled=read<u8>(m.oldController+0x2d)?1:0;
 chainVisibility(m.render,m.oldPart,atlas?0:enabled);
 setControllerVisibility(m.newController,atlas?enabled:0);
 static bool reported=false;if(atlas&&!reported){reported=true;log("N02_BOUND: ATLAS 104 uses private 54-joint N02 geometry; original Boost remains available.");}
}
