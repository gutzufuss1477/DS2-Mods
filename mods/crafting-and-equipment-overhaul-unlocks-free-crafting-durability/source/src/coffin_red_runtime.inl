// DS2 v1.10.89.0 direct Story graphics on the native vehicle, v1.9.53.
namespace redcoffin_runtime {
using u8=craft::u8; using u32=craft::u32; using u64=craft::u64; using i32=craft::i32;
constexpr u64 ManufactureRva=0x1829E53ull;
constexpr u8 ManufactureBytes[5]={0xE8,0xE8,0x47,0xEF,0xFF};
constexpr u64 NativeVehicleTickRva=0x1F79699ull;
constexpr u8 NativeVehicleTickBytes[5]={0xE8,0x72,0xD7,0xFE,0xFF};
constexpr u64 NativeVehicleUpdate=0x1F66E10ull;
constexpr u64 NativeShaderUpdate=0x33BD40ull;
using ManufactureFn=void(*)(u64);
using VehicleUpdateFn=void(*)(u64,float);
using ShaderFn=void(*)(u64,const u32*,const float*,u32,const u32*);
ManufactureFn originalManufacture=nullptr;
VehicleUpdateFn originalVehicleUpdate=nullptr;
ShaderFn shader=nullptr;
volatile long enabled=0,arm=0,finished=0,tickCount=0,guardLogs=0;
volatile long manufactureLogs=0;
u64 manufactureTick=0,sourceMenu=0,finishedOwner=0;
u64 previousOwners[redcoffin::MaxPreviousCoffins]={};
u32 previousCount=0;
bool within(u64 start,u64 now){return start&&now>=start&&now-start<=redcoffin::RedManufactureWindowMs;}
#include "coffin_story_visual.inl"
bool ownAlias(u64 instance){
 return instance&&snapshotCount>0&&snapshotCount<=MaxSnapshots&&
    redcoffin::selectedAlias(instance,redAliasAddresses,snapshotCount,MaxSnapshots);
}
bool saveExisting(){
 u64 mgr=0;
 if(!rd(image+0x623EA48,mgr)||mgr<0x10000)return false;
 i32 count=-1;u64 records=0;
 if(!rd(mgr+0x24488,count)||count<0||count>i32(redcoffin::MaxPreviousCoffins))return false;
 if(count&&!rd(mgr+0x24490,records))return false;
 if(count&&records<0x10000)return false;
 u64 tmp[redcoffin::MaxPreviousCoffins]={};
 for(i32 i=0;i<count;++i){
  if(!rd(records+u64(i)*0x28ull+8,tmp[i])||tmp[i]<0x10000)return false;
 }
 for(i32 i=0;i<count;++i)previousOwners[i]=tmp[i];
 previousCount=u32(count);
 return true;
}
bool registered(u64 owner){
 u64 mgr=0;if(!rd(image+0x623EA48,mgr)||mgr<0x10000)return false;
 i32 count=-1;u64 records=0;
 if(!rd(mgr+0x24488,count)||count<1||count>i32(redcoffin::MaximumWorldCoffins)||
    !rd(mgr+0x24490,records)||records<0x10000)return false;
 for(i32 i=0;i<count;++i){
  u64 other=0;
  if(rd(records+u64(i)*0x28ull+8,other)&&other==owner)return true;
 }
 return false;
}
void beginDirect();
bool prepareDirect();
void manufactured(u64 menu){
 // The engine's copied 0x20-byte per-fabrication row is the only safe
 // discriminator. A separate recipe/baggage id caused crashes previously.
 i32 n=0;bool safe=menu>=0x10000;
 u32 red=0,other=0;
 if(!safe||!rd(menu+0x41F0,n)||n<1||n>i32(MaxMenu))safe=false;
 if(safe){
  for(i32 i=0;i<n;++i){
   const u64 row=menu+0x1F0ull+u64(i)*0x20ull;
   u64 instance=0;i32 qty=0;u32 item=0;
   if(!rd(row,instance)||!rd(row+8,qty)||!rd(row+12,item)||
      qty<0||qty>32){safe=false;break;}
   if(!qty)continue;
   if(ownAlias(instance)&&item==0x0184CF05u&&qty==1)red++;
   else other++;
  }
 }
 if(_InterlockedCompareExchange(&enabled,0,0)){
  if(safe&&red==1&&!other&&
     _InterlockedCompareExchange(&arm,2,0)==0){
    if(saveExisting()){
     beginDirect();
     sourceMenu=menu;manufactureTick=GetTickCount64();
     _InterlockedExchange(&tickCount,0);
     _InterlockedExchange(&guardLogs,0);
     _InterlockedExchange(&finished,0);
     finishedOwner=0;
     _InterlockedExchange(&arm,1);
     if(_InterlockedCompareExchange(&storyEnabled,0,0)==1){
      storyManufactureTime=manufactureTick;
      _InterlockedExchange(&storyPending,1);
      _InterlockedExchange(&storyApplied,0);
      // Preload the original 8542 resource group BEFORE native vehicle model
      // initialization. Waiting for the later baggage-view callback is too late.
      if(!prepareDirect())
       say("RED_COFFIN_STORY_MODEL_PRELOAD_PENDING: Story group unavailable at fabrication; normal model preserved.");
      say("RED_COFFIN_V1955_DIRECT_MODEL_ARMED: fresh per-fabrication state; repeated crafting and crafting after reload allowed.");
     }
     Text t;t.add("RED_COFFIN_MANUFACTURE_ARMED: exact second recipe, previously present boards=");
     t.dec(previousCount);t.add(" validRows=");t.dec(n);log(t);
    }else{
     _InterlockedExchange(&arm,0);
     say("RED_COFFIN_MANUFACTURE_BLOCKED: native world registry snapshot invalid.");
    }
  }else if(safe&&red&&other){
   say("RED_COFFIN_MIXED_FABRICATION_UNSUPPORTED: both menu rows have the same native baggage identity; manufacture one at a time for distinct color.");
   _InterlockedExchange(&arm,0);
   _InterlockedExchange(&storyPending,0);
  }else if(safe&&other&&_InterlockedCompareExchange(&arm,0,0)==1){
   _InterlockedExchange(&arm,0);
   _InterlockedExchange(&storyPending,0);
   say("RED_COFFIN_MANUFACTURE_CANCELLED: another original item was fabricated.");
  }else if(_InterlockedIncrement(&manufactureLogs)<=5){
   Text t;t.add("RED_COFFIN_MANUFACTURE_CHECK: valid=");
   t.dec(safe?1:0);t.add(" red=");t.dec(red);t.add(" others=");t.dec(other);log(t);
  }
 }
 originalManufacture(menu);
}

#include "coffin_direct_model_runtime.inl"
#include "coffin_persistence_runtime.inl"
void beginDirect(){coffin_direct_runtime::beginFabrication();}
bool prepareDirect(){return coffin_direct_runtime::prepare();}
void onCraftingMenuReady(){
 if(_InterlockedCompareExchange(&enabled,0,0)!=1)return;
 if(coffin_direct_runtime::prepare())
  say("RED_COFFIN_V1953_DIRECT_ASSETS_READY: registered original Story geometry/materials, native 19-joint representation, two native-named parts; no Entity spawned.");
 else
  say("RED_COFFIN_V1953_DIRECT_ASSETS_PENDING: exact private graphics resources unavailable; native model fallback.");
}
void tick(u64 vehicle,float state){
 originalVehicleUpdate(vehicle,state);
 coffin_direct_runtime::observe(vehicle); // read-only diagnostics
 if(_InterlockedCompareExchange(&arm,0,0)==1&&!within(manufactureTick,GetTickCount64())){
  _InterlockedExchange(&arm,0);
  say("RED_COFFIN_V1953_DIRECT_SELECTION_EXPIRED: selected vehicle graphics not created within window; no late replacement.");
 }
}
bool setupHooks(){
 originalManufacture=(ManufactureFn)(image+0x171E640);
 originalVehicleUpdate=(VehicleUpdateFn)(image+NativeVehicleUpdate);
 const u64 sites[3]={ManufactureRva,NativeVehicleTickRva,coffin_direct::AddRowSite};
 const u8*expected[3]={ManufactureBytes,NativeVehicleTickBytes,coffin_direct::AddRowBytes};
 const u64 targets[3]={u64(&manufactured),u64(&tick),u64(&coffin_direct_runtime::addRow)};
 u8 bytes[5]={};void*relays[3]={};
 for(u32 i=0;i<3;++i)if(!readmem(image+sites[i],bytes,5)||memcmp(bytes,expected[i],5)){
  say("RED_COFFIN_V1953_HOOK_REFUSED: original manufacture/update/native add-row callsite mismatch.");return false;
 }
 for(u32 i=0;i<3;++i){
  relays[i]=relayNear(sites[i],targets[i]);
  if(!relays[i]){for(u32 j=0;j<i;++j)VirtualFree(relays[j],0,0x8000);return false;}
 }
 bool all=true;
 for(u32 i=0;i<3;++i)if(!patchOnce(sites[i],expected[i],relays[i])){
  VirtualFree(relays[i],0,0x8000);all=false;
 }
 if(!all){say("RED_COFFIN_V1953_PARTIAL_HOOKS_DISABLED: wrappers retain native behavior.");return false;}
 persistence_runtime::setup();
 _InterlockedExchange(&storyEnabled,1);_InterlockedExchange(&enabled,1);
 say("RED_COFFIN_V1953_DIRECT_NATIVE_HOOK_READY: three verified callsites; no Entity creation, GPU forcing, world-transform following, render-world correction or visibility override.");
 return true;
}
}
