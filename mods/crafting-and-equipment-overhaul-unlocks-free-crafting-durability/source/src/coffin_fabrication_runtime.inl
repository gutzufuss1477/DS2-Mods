// Exact native SPECIAL vehicle row -> freshly constructed native entity GUID.
// The factory returns before native model initialization. Each object keeps its
// own variant even if initialization finishes on a different worker later.
namespace fabrication_runtime {
constexpr u32 Capacity=coffin_session::Capacity;
struct Context {bool red=false;u64 epoch=0;};
coffin_session::Identity owners[Capacity]={};
u64 cargos[coffin_persistence::Capacity]={};u32 cargoCount=0;
u64 epoch=1;void*lock=nullptr;DWORD tls=0xffffffffu;
volatile long ready=0,selectionLogs=0,ownerLogs=0;
using RowFn=u64(*)(u64,u32);
using CreateFn=u64(*)(u64,u64,u64);
RowFn nativeRow=nullptr;CreateFn nativeCreate=nullptr;
void reset(){
 AcquireSRWLockExclusive(&lock);++epoch;memset(owners,0,sizeof(owners));
 memset(cargos,0,sizeof(cargos));cargoCount=0;ReleaseSRWLockExclusive(&lock);
}
bool isOwner(u64 vehicle){
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const auto current=coffin_session::identity(reader,image,vehicle);if(!current.owner)return false;
 bool found=false;AcquireSRWLockExclusive(&lock);
 for(const auto&e:owners)if(coffin_session::same(e,current)){found=true;break;}
 ReleaseSRWLockExclusive(&lock);return found;
}
void refreshCargo(u64 vehicle){
 if(!isOwner(vehicle))return;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const auto current=coffin_session::identity(reader,image,vehicle);
 u64 manager=0,rows=0,cargo=0;u32 count=0;
 if(!rd(image+0x623ea48,manager)||manager<0x10000||!rd(manager+0x24488,count)||count>Capacity||
    !rd(manager+0x24490,rows)||(!rows&&count))return;
 for(u32 i=0;i<count;++i){u64 own=0,uid=0;const u64 p=rows+0x28ull*i;
  if(!rd(p+8,own)||own!=vehicle)continue;
  if(cargo||!rd(p+0x10,uid)||!coffin_persistence::validCargo(uid))return;
  cargo=uid;
 }
 if(!cargo)return;
 AcquireSRWLockExclusive(&lock);
 // A save load may have reset the map while native registry reads ran.
 bool stillOwned=false;for(const auto&e:owners)if(coffin_session::same(e,current)){stillOwned=true;break;}
 if(!stillOwned){ReleaseSRWLockExclusive(&lock);return;}
 for(u32 i=0;i<cargoCount;++i)if(cargos[i]==cargo){ReleaseSRWLockExclusive(&lock);return;}
 if(cargoCount<coffin_persistence::Capacity)cargos[cargoCount++]=cargo;
 ReleaseSRWLockExclusive(&lock);
}
bool isRed(u64 cargo){
 if(!coffin_persistence::validCargo(cargo))return false;
 bool found=false;AcquireSRWLockExclusive(&lock);
 for(u32 i=0;i<cargoCount;++i)if(cargos[i]==cargo){found=true;break;}
 ReleaseSRWLockExclusive(&lock);if(found)return true;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const u64 vehicle=coffin_session::cargoOwner(reader,image,cargo);
 if(!isOwner(vehicle))return false;
 refreshCargo(vehicle);return true;
}
extern "C" u64 CoffinFabricateVehicle(u64 instance,u32 facility,u64 row){
 Context context;
 const bool on=_InterlockedCompareExchange(&ready,0,0)==1&&_InterlockedCompareExchange(&enabled,0,0)==1;
 void*previous=on?TlsGetValue(tls):nullptr;bool scoped=false;
 if(on){
  auto reader=[](u64 p,auto&v){return rd(p,v);};auto own=[](u64 p){return ownAlias(p);};
  u64 selected=0;u32 kind=0,quantity=0;
  context.red=rd(row,selected)&&selected==instance&&rd(row+8,quantity)&&quantity>0&&
   rd(row+0x14,kind)&&kind==3&&coffin_menu::selectedRow(reader,own,image,row);
  AcquireSRWLockExclusive(&lock);context.epoch=epoch;ReleaseSRWLockExclusive(&lock);
  scoped=TlsSetValue(tls,&context)!=0;
  if(!scoped){_InterlockedExchange(&ready,0);say("RED_COFFIN_BATCH_TLS_SCOPE_FAILED: capture disabled; native fabrication retained.");}
  if(context.red&&scoped){
   if(!prepareDirect())say("RED_COFFIN_BATCH_ASSETS_PENDING: native resource loading will be retried.");
   if(_InterlockedIncrement(&selectionLogs)<=64){Text m;m.add("RED_COFFIN_BATCH_ROW: native vehicle row, quantity=");m.dec(quantity);m.add(" red variant selected independently of other basket rows.");log(m);}
  }
 }
 // Native vehicle fabrication is called ONCE with its original two arguments.
 // The engine owns quantity, cost, placement, callbacks and creation failures.
 const u64 result=nativeRow(instance,facility);
 // Registration creates the native cargo UID after the model. Retain that UID
 // independently of the live vehicle so stored/temporarily unloaded cargo keeps
 // its variant. The native record and save format remain untouched.
 if(on)refreshCargo(result);
 if(scoped&&!TlsSetValue(tls,previous))_InterlockedExchange(&ready,0);
 return result;
}
u64 created(u64 resource,u64 parameters,u64 guid){
 const u64 result=nativeCreate(resource,parameters,guid);
 if(_InterlockedCompareExchange(&ready,0,0)!=1||_InterlockedCompareExchange(&enabled,0,0)!=1)return result;
 const auto*context=(const Context*)TlsGetValue(tls);
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const auto current=coffin_session::identity(reader,image,result);
 if(!current.owner)return result;
 bool added=false;AcquireSRWLockExclusive(&lock);
 // Ordinary jobs must not inherit a marker when an allocation is reused.
 for(auto&e:owners)if(e.owner==result)e={};
 if(context&&context->red&&context->epoch==epoch){
  for(auto&e:owners){
   if(e.owner&&!coffin_session::same(e,coffin_session::identity(reader,image,e.owner)))e={};
   if(!e.owner){e=current;added=true;break;}
  }
 }
 ReleaseSRWLockExclusive(&lock);
 if(context&&context->red&&!added)say("RED_COFFIN_BATCH_OWNER_REFUSED: native entity retained; no unrelated vehicle selected.");
 if(added&&_InterlockedIncrement(&ownerLogs)<=128){Text m;m.add("RED_COFFIN_BATCH_OWNER: owner=");m.hex(result,16);m.add(" GUID=");m.hex(current.lo,16);m.hex(current.hi,16);m.add(" assigned before native graphics initialization.");log(m);}
 return result;
}
bool initialize(){
 tls=TlsAlloc();if(tls==0xffffffffu)return false;
 nativeRow=(RowFn)(image+0xad5e40);nativeCreate=(CreateFn)(image+0x169cb0);return true;
}
}
extern "C" void CoffinFabricateVehicleThunk();
