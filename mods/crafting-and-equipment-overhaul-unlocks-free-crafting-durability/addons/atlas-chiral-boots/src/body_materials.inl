// Test17: mirror only the native suit overrides belonging to the two grafted parts.
// Evidence: DS2 1.10.89.0 / SHA in addon.cpp; see evidence/ghidra-materials.json.
// This runs on the existing native worn-component callback, never a worker thread.
constexpr u32 SuitSourceNames[2]={1838528733,718898234};
constexpr u32 SuitTargetNames[2]={0x6c1a0005,0x6c1a0006};
constexpr u32 SuitParameterRva=0x347d30, SuitRemovalRva=0x34e060, SuitQueueRva=0x34dd40;
constexpr u32 SuitDirtyRva=0x348790, MaxSuitParameters=512;
struct SuitParameter {
 u64 key;u32 parameter,values[4];u8 count,padding[3];i32 controller;
 u8 applied,removed,state,pad;
};
static_assert(sizeof(SuitParameter)==0x28&&__builtin_offsetof(SuitParameter,count)==0x1c&&
 __builtin_offsetof(SuitParameter,removed)==0x25,"Native shader override layout");
using SuitSet=u64(*)(u64,const u32*,const void*,i32,const u32*);
using SuitRemove=u64(*)(u64,const u64*,const u32*);
using SuitQueue=void(*)(u64,const u64*);
SuitSet suitSet=nullptr;SuitRemove suitRemove=nullptr;SuitQueue suitQueue=nullptr;
volatile long suitBusy=0;
SuitParameter suitSnapshot[MaxSuitParameters]={};
u32 suitLogLines=0;
void suitLog(const char* action,u64 owner,u32 source,const SuitParameter&entry){
 if(suitLogLines>=192)return;++suitLogLines;
 char line[256]="CHIRAL_SUIT ";u32 n=12;
 auto append=[&](const char*s){while(*s&&n<250)line[n++]=*s++;};
 auto hex=[&](u64 x,u32 digits){constexpr char h[]="0123456789abcdef";for(u32 j=digits;j;--j)line[n++]=h[(x>>((j-1)*4))&15];};
 append(action);append(" owner=");hex(owner,16);append(" source=");hex(source,8);
 append(" part=");hex(u32(entry.key),8);append(" parameter=");hex(entry.parameter,8);
 append(" count=");hex(entry.count,2);append(" bits=");
 for(u32 i=0;i<entry.count&&i<4;++i){hex(entry.values[i],8);line[n++]=' ';}
 line[n]=0;log(line);
}
bool suitSignatures(){
 const u8 a[]={0x40,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x48,0x8d,0x6c,0x24,0xd9,0x48};
 const u8 b[]={0x40,0x55,0x56,0x57,0x41,0x54,0x41,0x57,0x48,0x83,0xec,0x20,0x8b,0x29,0x4c,0x8b};
 const u8 c[]={0x41,0x56,0x48,0x83,0xec,0x40,0x4c,0x8b,0xca,0x4c,0x8b,0xf1,0x48,0x8b,0x51,0x40};
 const u8 d[]={0x48,0x8b,0x51,0x28,0x8b,0x82,0xb0,0x01,0,0,0x0f,0xba,0xf0,0x1c,0x0f,0xba};
 const u32 rvas[]={SuitParameterRva,SuitRemovalRva,SuitQueueRva,SuitDirtyRva};
 const u8*expected[]={a,b,c,d};u8 bytes[16];
 for(u32 i=0;i<4;++i)if(!readmem(image+rvas[i],bytes,16)||!same(bytes,expected[i],16))return false;
 return true;
}
bool prepareSuitTextures();
bool prepareBootPaint();
bool prepareSuitMaterials(){
 if(!suitSignatures())return false;
 suitSet=(SuitSet)(image+SuitParameterRva);suitRemove=(SuitRemove)(image+SuitRemovalRva);
 suitQueue=(SuitQueue)(image+SuitQueueRva);return prepareSuitTextures()&&prepareBootPaint();
}
bool suitArray(u64 component,u32 offset,u32 stride,i32&count,u64&data){
 u8 h[16];if(!readmem(component+offset,h,16))return false;
 count=at<i32>(h,0);i32 capacity=at<i32>(h,4);data=at<u64>(h,8);
 return count>=0&&count<=i32(MaxSuitParameters)&&capacity>=count&&capacity<=4096&&
  (!count||(data>=0x10000&&data+u64(count)*stride>=data&&writable((void*)data,u64(count)*stride)));
}
bool suitOwner(const BodyMap&m,u64 parameters){
 return validBody(m)&&parameters>=0x10000&&read<u64>(m.owner+0xa0)==parameters&&
  read<u64>(parameters+0x78)==m.owner&&writable((void*)parameters,0x90);
}
bool suitRecord(const SuitParameter&e){
 return u32(e.key>>32)==e.parameter&&e.count>=1&&e.count<=4&&e.removed<=1;
}
// Re-resolve after EVERY native call: native setters can reallocate/sort the vector.
u64 suitFind(u64 parameters,u64 key,SuitParameter&out){
 i32 count=0;u64 data=0;if(!suitArray(parameters,0x50,0x28,count,data))return 0;
 i32 lo=0,hi=count;
 while(lo<hi){i32 mid=lo+(hi-lo)/2;SuitParameter e={};
  if(!readmem(data+u64(mid)*0x28,&e,sizeof(e)))return 0;
  if(e.key<key)lo=mid+1;else hi=mid;
 }
 if(lo==count||!readmem(data+u64(lo)*0x28,&out,sizeof(out))||out.key!=key)return 0;
 return data+u64(lo)*0x28;
}
const SuitParameter* suitSnapshotFind(i32 count,u64 key){
 for(i32 i=0;i<count;++i)if(suitSnapshot[i].key==key)return &suitSnapshot[i];return nullptr;
}
bool syncSuitMaterialsInner(const BodyMap&m){
 const u64 parameters=read<u64>(m.owner+0xa0);
 if(!suitOwner(m,parameters))return false;
 i32 count=0,removedCount=0;u64 data=0,removedData=0;
 if(!suitArray(parameters,0x50,0x28,count,data)||!suitArray(parameters,0x60,0x10,removedCount,removedData)||
  (count&&!readmem(data,suitSnapshot,u64(count)*sizeof(SuitParameter))))return false;
 // Reject malformed, duplicate or unsorted tables before making any native call.
 for(i32 i=0;i<count;++i){const auto&e=suitSnapshot[i];
  if(i&&e.key<=suitSnapshot[i-1].key)return false;
  for(u32 j=0;j<2;++j)if(u32(e.key)==SuitSourceNames[j]||u32(e.key)==SuitTargetNames[j]){
   u64 controller=u32(e.key)==SuitSourceNames[j]?m.originalController[j]:m.replacementController[j];
   if(!suitRecord(e)||e.controller!=i32((controller-m.controllers)/0x30))return false;
  }
 }
 if(read<i32>(parameters+0x50)!=count||read<u64>(parameters+0x58)!=data||!suitOwner(m,parameters))return false;
 // Capture a bounded initial sample even if this costume uses unexpected part names.
 // This is diagnostic evidence, not proof that the reported green tint is reproduced.
 static bool sampled=false;
 if(count&&!sampled){sampled=true;
  for(i32 i=0;i<count&&i<24;++i)if(suitRecord(suitSnapshot[i]))suitLog("OBSERVED",m.owner,0,suitSnapshot[i]);
 }
 for(i32 i=0;i<count;++i){const auto&e=suitSnapshot[i];
  for(u32 j=0;j<2;++j){
   if(u32(e.key)!=SuitSourceNames[j]||e.removed)continue;
   const u64 key=(e.key&0xffffffff00000000ull)|SuitTargetNames[j];
   const auto*old=suitSnapshotFind(count,key);
   if(old&&!old->removed&&old->count==e.count&&same(old->values,e.values,e.count*4))continue;
   if(!suitOwner(m,parameters))return false;
   const u32 part=SuitTargetNames[j];
   if(!suitSet(parameters,&e.parameter,e.values,e.count,&part))return false;
   SuitParameter after={};
   if(!suitFind(parameters,key,after)||after.removed||after.count!=e.count||!same(after.values,e.values,e.count*4))return false;
   suitLog("COPIED",m.owner,SuitSourceNames[j],after);
  }
 }
 // Native reset behavior (0x348280): mark the private entry for removal, insert its
 // key into the native deferred-removal map and dispatch the native dirty callback.
 // Never erase vectors ourselves, never reset the entire component or original part.
 for(i32 i=0;i<count;++i){const auto&e=suitSnapshot[i];
  for(u32 j=0;j<2;++j){
   if(u32(e.key)!=SuitTargetNames[j]||e.removed)continue;
   const auto*source=suitSnapshotFind(count,(e.key&0xffffffff00000000ull)|SuitSourceNames[j]);
   if(source&&!source->removed)continue;
   if(!suitOwner(m,parameters))return false;
   SuitParameter current={};const u64 address=suitFind(parameters,e.key,current);
   if(!address||current.removed)continue;
   if(!writable((void*)address,sizeof(SuitParameter)))return false;
   *((u8*)address+0x25)=1;*((u8*)address+0x24)=0;
   suitRemove(parameters+0x60,&e.key,&e.parameter);
   const u64 callback[]={image+SuitDirtyRva,0};suitQueue(parameters,callback);
   suitLog("REMOVED",m.owner,SuitSourceNames[j],e);
  }
 }
 return true;
}
#include "body_textures.inl"
#include "boot_paint.inl"
void syncSuitMaterials(const BodyMap&m){
 if(!suitSet||!suitRemove||!suitQueue||_InterlockedCompareExchange(&suitBusy,1,0))return;
 if(!m.active){clearInactiveTextures(m);sweepTexturePins();_InterlockedExchange(&suitBusy,0);return;}
 bool ok=syncSuitMaterialsInner(m);
 if(suitGetModel){const bool texturesOk=syncSuitTexturesInner(m);if(!texturesOk){static bool noted=false;if(!noted){noted=true;log("CHIRAL_SUIT_TEXTURE_WAIT: renderer textures unavailable or validation refused; retry on worn callback.");}}}
 syncBootPaint(m);
 static bool reported=false;
 if(!ok&&!reported){reported=true;log("CHIRAL_SUIT_WAIT: native parameter component unavailable or validation refused; retry on worn callback.");}
 if(ok){static bool seen=false;if(!seen){seen=true;log("CHIRAL_SUIT_READY: native suit parameters inspected; only two private graft parts may receive copies.");}}
 sweepTexturePins();_InterlockedExchange(&suitBusy,0);
}
