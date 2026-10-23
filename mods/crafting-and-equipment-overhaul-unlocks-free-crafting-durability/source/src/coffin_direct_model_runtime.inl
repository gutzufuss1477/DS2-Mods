// Native model-row construction only. No second Entity, world transforms,
// visibility toggles, fabricated live resources, or manual GPU initialization.
namespace persistence_runtime {bool isLoaded(u64 cargo);}
namespace coffin_direct_runtime {
volatile long phase=0,appliedMask=0,observations=0,verification=0;
volatile long long owner=0;
volatile long previewBindings=0,previewChecks=0;
// Each successful fabrication is retained separately. World addresses are
// checked with their native 128-bit object identity to reject address reuse.
coffin_session::Identity storyOwners[coffin_session::Capacity]={};
void*storyOwnersLock=nullptr;
bool isStoryCargo(u64 cargo){
 if(persistence_runtime::isLoaded(cargo))return true;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const u64 vehicle=coffin_session::cargoOwner(reader,image,cargo);
 const auto current=coffin_session::identity(reader,image,vehicle);if(!current.owner)return false;
 bool found=false;AcquireSRWLockExclusive(&storyOwnersLock);
 for(const auto&e:storyOwners)if(coffin_session::same(e,current)){found=true;break;}
 ReleaseSRWLockExclusive(&storyOwnersLock);return found;
}
void remember(u64 vehicle){
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const auto current=coffin_session::identity(reader,image,vehicle);if(!current.owner)return;
 bool added=false;AcquireSRWLockExclusive(&storyOwnersLock);
 for(const auto&e:storyOwners)if(coffin_session::same(e,current)){ReleaseSRWLockExclusive(&storyOwnersLock);return;}
 for(auto&e:storyOwners){
  // Reclaim expired slots only when the native registry no longer associates
  // the same live object. This does not touch any native registry or save.
  if(e.owner){const auto live=coffin_session::identity(reader,image,e.owner);
   if(!coffin_session::same(e,live)||!coffin_preview::cargoIdentity(reader,image,e.owner))e={};}
  if(!e.owner){e=current;added=true;break;}
 }
 ReleaseSRWLockExclusive(&storyOwnersLock);
 if(added)say("RED_COFFIN_V1955_SESSION_OWNER: additional Story coffin registered for cargo preview and detail icon.");
 else say("RED_COFFIN_V1955_SESSION_FULL: world model retained; preview association unavailable.");
}
u32 renderIndices[2]={0xffffffffu,0xffffffffu};
void beginFabrication(){
 _InterlockedCompareExchange64(&owner,0,_InterlockedCompareExchange64(&owner,0,0));_InterlockedExchange(&phase,0);
 _InterlockedExchange(&appliedMask,0);_InterlockedExchange(&verification,0);_InterlockedExchange(&observations,0);
 renderIndices[0]=renderIndices[1]=0xffffffffu;
}
u64 parts[2]={};
u64 previewParts[2]={};volatile long previewPrepareBusy=0;
bool preparePreview(u64 skin){
 if(previewParts[0]&&previewParts[1])return true;
 if(_InterlockedCompareExchange(&previewPrepareBusy,1,0))return false;
 auto reader=[](u64 p,auto&v){return rd(p,v);};u64 own=0,art=0,res=0,model=0,list=0,originals[2]={};u32 count=0;
 bool ok=rd(skin+0x48,own)&&own>=0x10000&&rd(own+0xD8,art)&&art>=0x10000&&
  rd(art+0x30,res)&&coffin_direct::id(reader,res,coffin_preview::ArtLo,coffin_preview::ArtHi)&&
  rd(res+0x20,model)&&coffin_direct::id(reader,model,coffin_preview::ModelLo,coffin_preview::ModelHi)&&
  rd(model+0x38,count)&&count==6&&rd(model+0x40,list)&&list>=0x10000;
 coffin_preview::PartCopy native[2]={},story[2]={},copies[2]={};
 for(u32 i=0;ok&&i<2;++i)ok=rd(list+16ull*i,originals[i])&&storyWritable(originals[i])&&storyWritable(parts[i])&&
  readmem(originals[i],native[i].bytes,0xe0)&&readmem(parts[i],story[i].bytes,0xe0)&&
  coffin_preview::copyGraphics(copies[i],native[i].bytes,story[i].bytes,i);
 if(ok){auto*allocated=(coffin_preview::PartCopy*)VirtualAlloc(nullptr,sizeof(copies),0x3000,4);ok=allocated!=nullptr;
  if(ok){memcpy(allocated,copies,sizeof(copies));
   // Keep source resource graphs alive; our two root references are retained
   // until process exit, exactly like the host's existing attachment graphs.
   for(u32 i=0;i<2;++i){_InterlockedIncrement((long*)(originals[i]+8));_InterlockedIncrement((long*)(parts[i]+8));previewParts[i]=u64(allocated[i].bytes);}
   say("RED_COFFIN_V1954_PREVIEW_GRAPH_READY: Story geometry and bounds, original baggage collision and all other part fields retained.");
  }
 }
 _InterlockedExchange(&previewPrepareBusy,0);return ok;
}
using AddFn=u64(*)(u64,u64,u8);
using BindFn=void(*)(u64,u64);
bool prepare(){
 if(_InterlockedCompareExchange(&vgraph_runtime::registeredGraphState,0,0)!=2||
    !coffin_archive_storage::ready()||!primeStoryBeforeVehicle(0))return false;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 u64 found[2]={};
 if(!coffin_direct::privateParts(reader,loadedStoryGroup,found))return false;
 parts[0]=found[0];parts[1]=found[1];return true;
}
u64 addRow(u64 skin,u64 row,u8 initialize){
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 coffin_direct::Candidate choice{};
 bool restored=false;
 if(_InterlockedCompareExchange(&enabled,0,0)==1){
  u64 vehicle=0,vt=0;
  if(rd(skin+0x48,vehicle)&&rd(vehicle,vt)&&vt==image+redcoffin::CoffinVehicleVtableRva){
   const u64 cargo=coffin_persistence::worldCargo(reader,image,vehicle);
   if(persistence_runtime::isLoaded(cargo)&&((parts[0]&&parts[1])||prepare())){
    choice=coffin_direct::inspect(reader,image,skin,row,parts);restored=choice.replacement!=0;
   }
  }
 }
 if(_InterlockedCompareExchange(&enabled,0,0)==1&&
    !restored&&_InterlockedCompareExchange(&arm,0,0)==1&&parts[0]&&parts[1]){
  choice=coffin_direct::inspect(reader,image,skin,row,parts);
  const u64 tracked=u64(_InterlockedCompareExchange64(&owner,0,0));
  if(!choice.replacement||!coffin_direct::selected(true,GetTickCount64(),manufactureTick,
       choice.owner,tracked,previousOwners,previousCount))choice={};
  if(choice.replacement){
   const u64 prior=u64(_InterlockedCompareExchange64(&owner,(long long)choice.owner,0));
   if(prior&&prior!=choice.owner)choice={};
  }
 }
 bool preview=false;
 u64 previewResource=0;
 if(!choice.replacement&&_InterlockedCompareExchange(&enabled,0,0)==1&&parts[0]&&parts[1]&&
    rd(skin+0x30,previewResource)&&coffin_direct::id(reader,previewResource,coffin_preview::SkinLo,coffin_preview::SkinHi)){
  u64 previewOwner=0;rd(skin+0x48,previewOwner);
  const u64 cargo=coffin_session::previewCargo(reader,image,previewOwner);
  const u64 vehicle=coffin_session::cargoOwner(reader,image,cargo);
  if(isStoryCargo(cargo)&&preparePreview(skin))choice=coffin_preview::inspect(reader,image,skin,row,previewParts,cargo);preview=choice.replacement!=0;
  if(!preview&&_InterlockedIncrement(&previewChecks)<=12){Text m;m.add("RED_COFFIN_V1954_PREVIEW_CHECK: vehicle=");m.hex(vehicle,16);m.add(" cargo=");m.hex(cargo,16);m.add(" skin=");m.hex(skin,16);m.add(" native row retained (accessory or unmatched identity).");log(m);}
 }
 auto bind=[](u64 r,u64 part){((BindFn)(image+coffin_direct::BindRowTarget))(r,part);};
 auto native=[](u64 s,u64 r,u8 flag)->u64{return ((AddFn)(image+coffin_direct::AddRowTarget))(s,r,flag);};
 const u64 result=coffin_direct::dispatch(choice,skin,row,initialize,bind,native);
 if(restored){
  // This is the same native construction-time binding as fresh fabrication.
  // Do not touch the fresh-fabrication arm or its two-part verification state.
  u64 rows=0,bound=0;u32 count=0;const u32 index=u32(result);
  bool ok=index<redcoffin::NativeCoffinArtPartCount&&rd(skin+0x58,count)&&index<count&&
   rd(skin+0x60,rows)&&rows>=0x10000&&rd(rows+0x140ull*index+0xc8,bound)&&bound==choice.replacement;
  Text m;m.add(ok?"RED_COFFIN_V1956_RESTORED_PART: owner=":"RED_COFFIN_V1956_RESTORE_BIND_INCOMPLETE: owner=");
  m.hex(choice.owner,16);m.add(" slot=");m.dec(choice.slot);log(m);
  return result;
 }
 if(preview){
  if(_InterlockedIncrement(&previewBindings)<=24){Text m;m.add("RED_COFFIN_V1954_PREVIEW_PART_BOUND: owner=");m.hex(choice.owner,16);
   m.add(" slot=");m.dec(choice.slot);m.add(" index=");m.dec(u32(result));m.add(" exact red cargo UID; native baggage components retained.");log(m);}
  return result;
 }
 if(choice.replacement){
  u64 rows=0,bound=0;u32 count=0;
  const u32 index=u32(result);
  bool ok=index<redcoffin::NativeCoffinArtPartCount&&rd(skin+0x58,count)&&index<count&&
          rd(skin+0x60,rows)&&rows>=0x10000ull&&
          rd(rows+0x140ull*index+0xC8,bound)&&bound==choice.replacement;
  if(ok){
   renderIndices[choice.slot]=index;
   long before=0;
   do {before=_InterlockedCompareExchange(&appliedMask,0,0);}
   while(_InterlockedCompareExchange(&appliedMask,before|(1L<<choice.slot),before)!=before);
   _InterlockedExchange(&phase,1);
   Text m;m.add("RED_COFFIN_V1953_DIRECT_PART_BOUND: owner=");m.hex(choice.owner,16);
   m.add(" slot=");m.dec(choice.slot);m.add(" renderIndex=");m.dec(index);
   m.add(" resource=");m.hex(bound,16);m.add(" native add called exactly once; original component resources retained.");log(m);
   if(_InterlockedCompareExchange(&appliedMask,0,0)==3){
    remember(choice.owner);
    _InterlockedExchange(&phase,2);_InterlockedExchange(&arm,0);
   }
  }else{
   _InterlockedExchange(&phase,-1);_InterlockedExchange(&arm,0);
   say("RED_COFFIN_V1953_DIRECT_BIND_INCOMPLETE: native row did not retain requested resource; no retry or extra model callback.");
  }
 }
 return result;
}
void observe(u64 vehicle){
 const u64 target=u64(_InterlockedCompareExchange64(&owner,0,0));
 if(vehicle!=target||!target||_InterlockedCompareExchange(&phase,0,0)!=2||
    _InterlockedCompareExchange(&verification,0,0)!=0)return;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 u64 skin=0,art=0,res=0,rows=0;u32 count=0;
 bool ok=rd(vehicle+0xC8,skin)&&skin>=0x10000ull&&rd(skin+0x60,rows)&&rows>=0x10000ull&&
         rd(skin+0x58,count)&&count>=2&&count<=redcoffin::NativeCoffinArtPartCount&&
         rd(skin+0x30,res)&&coffin_direct::id(reader,res,redcoffin::NativeCoffinSkinned.lo,redcoffin::NativeCoffinSkinned.hi)&&
         rd(vehicle+0xD8,art)&&art>=0x10000ull&&rd(art+0x30,res)&&
         coffin_direct::id(reader,res,redcoffin::NativeCoffinArt.lo,redcoffin::NativeCoffinArt.hi);
 for(u32 i=0;ok&&i<2;++i){
  u64 part=0;ok=renderIndices[i]<count&&rd(rows+0x140ull*renderIndices[i]+0xC8,part)&&part==parts[i];
 }
 if(ok&&registered(vehicle)){
  _InterlockedExchange(&verification,1);
  say("RED_COFFIN_V1953_DIRECT_NATIVE_RENDER_VERIFIED: both Story parts belong to the original vehicle SkinnedModel; original ArtPartsData/SkinnedModel and gameplay identity retained; no follower or shell-visibility override.");
 }else if(_InterlockedIncrement(&observations)==120){
  say("RED_COFFIN_V1953_DIRECT_VERIFY_PENDING: original registry/component/render-row verification still incomplete.");
 }
}
}
