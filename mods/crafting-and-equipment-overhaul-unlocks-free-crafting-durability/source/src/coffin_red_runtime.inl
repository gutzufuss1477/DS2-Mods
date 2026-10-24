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
#include "coffin_fabrication_runtime.inl"
void manufactured(u64 menu){
 // Per-item identity is captured by the native row callsite below. The whole
 // basket always reaches the native implementation once, without exclusions.
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
 fabrication_runtime::refreshCargo(vehicle);
 coffin_direct_runtime::observe(vehicle); // read-only diagnostics
 if(_InterlockedCompareExchange(&arm,0,0)==1&&!within(manufactureTick,GetTickCount64())){
  _InterlockedExchange(&arm,0);
  say("RED_COFFIN_V1953_DIRECT_SELECTION_EXPIRED: selected vehicle graphics not created within window; no late replacement.");
 }
}
bool setupHooks(){
 originalManufacture=(ManufactureFn)(image+0x171E640);
 originalVehicleUpdate=(VehicleUpdateFn)(image+NativeVehicleUpdate);
 if(!fabrication_runtime::initialize()){say("RED_COFFIN_BATCH_TLS_UNAVAILABLE");return false;}
 constexpr u32 hookCount=5;
 const u64 sites[hookCount]={ManufactureRva,NativeVehicleTickRva,coffin_direct::AddRowSite,0x171e72e,0x1f26d66};
 const u8 extraBytes[2][5]={{0xe8,0x0d,0x77,0x3b,0xff},{0xe8,0x45,0x2f,0x24,0xfe}};
 const u8*expected[hookCount]={ManufactureBytes,NativeVehicleTickBytes,coffin_direct::AddRowBytes,extraBytes[0],extraBytes[1]};
 const u64 targets[hookCount]={u64(&manufactured),u64(&tick),u64(&coffin_direct_runtime::addRow),u64(&CoffinFabricateVehicleThunk),u64(&fabrication_runtime::created)};
 u8 bytes[5]={};void*relays[hookCount]={};
 for(u32 i=0;i<hookCount;++i)if(!readmem(image+sites[i],bytes,5)||memcmp(bytes,expected[i],5)){
  say("RED_COFFIN_V1953_HOOK_REFUSED: original manufacture/update/native add-row callsite mismatch.");return false;
 }
 for(u32 i=0;i<hookCount;++i){
  relays[i]=relayNear(sites[i],targets[i]);
  if(!relays[i]){for(u32 j=0;j<i;++j)VirtualFree(relays[j],0,0x8000);return false;}
 }
 bool all=true;
 for(u32 i=0;i<hookCount;++i)if(!patchOnce(sites[i],expected[i],relays[i])){
  VirtualFree(relays[i],0,0x8000);all=false;
 }
 if(!all){say("RED_COFFIN_V1953_PARTIAL_HOOKS_DISABLED: wrappers retain native behavior.");return false;}
 persistence_runtime::setup();
 _InterlockedExchange(&storyEnabled,1);_InterlockedExchange(&enabled,1);
 _InterlockedExchange(&fabrication_runtime::ready,1);
 say("RED_COFFIN_V1953_DIRECT_NATIVE_HOOK_READY: five verified callsites with exact native vehicle row and entity GUID; no Entity creation, GPU forcing, world-transform following, render-world correction or visibility override.");
 return true;
}
}
