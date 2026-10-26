// Release ownership for renderer texture overrides. Native TextureResource references
// use the low 22 bits at +8; release must use 0x109d80 (including its flag dispatch).
// Keep each resource while any validated private renderer override still uses it.
// A replacement/reset is notified to the native renderer before old references drop.
constexpr u32 TextureReleaseRva=0x109d80,MaxTextureOwners=128;
using TextureRelease=void(*)(u64);
TextureRelease textureRelease=nullptr;
u64 suitPinnedTextures[MaxSuitPinnedTextures]={};u32 suitPinnedCount=0;
u8 textureUsed[MaxSuitPinnedTextures]={};bool texturePinsDirty=false;u64 textureLastSweep=0;
struct TextureOwner {
 u64 root,rootArt,entity,model,art,resource,controllers,identity[2];
 i32 parts[4];u32 count,kind;bool pending;
};
TextureOwner textureOwners[MaxTextureOwners]={};
SuitShaderRecord textureGcRecords[MaxSuitTextureRecords];
bool prepareTextureLifetime(){
 const u8 expected[]={0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0xb8,0xff,0xff,0xff,0xff,0xf0,0x0f};
 u8 actual[16];if(!readmem(image+TextureReleaseRva,actual,16)||!same(actual,expected,16))return false;
 textureRelease=(TextureRelease)(image+TextureReleaseRva);return true;
}
bool textureOwnerValid(const TextureOwner&o){
 if(!o.entity||o.count<1||o.count>4||o.kind>1||read<u64>(o.root+0xd8)!=o.rootArt||
    read<u64>(o.entity+0x70)!=o.model||read<u64>(o.entity+0xd8)!=o.art||
    read<u64>(o.entity+0x10)!=o.identity[0]||read<u64>(o.entity+0x18)!=o.identity[1]||
    read<u64>(o.art+0x48)!=o.entity||read<u64>(o.art+0x30)!=o.resource||read<u64>(o.art+0x58)!=o.controllers)return false;
 const u64 rootResource=read<u64>(o.rootArt+0x30);
 if(read<u64>(rootResource+0x10)!=0x99427894a809558dull||read<u64>(rootResource+0x18)!=0xd1d869139c5e09b4ull)return false;
 if(o.kind){
  if(o.root==o.entity||read<u64>(o.entity+0x80)!=o.root||o.count!=4||read<i32>(o.resource+0x88)!=54||
     read<u64>(o.resource+0x10)!=0xb4414c0cb2eb5bffull||read<u64>(o.resource+0x18)!=0x03b47deefa0cbab7ull)return false;
  const u32 names[]={0x6c1a0001,0x6c1a0003,0x6c1a0002,0x6c1a0004};
  const u64 table=read<u64>(o.resource+0x90);
  for(u32 j=0;j<4;++j)if(o.parts[j]!=i32(50+j)||read<u32>(table+(50+j)*4)!=names[j]||
    read<u64>(o.controllers+(50+j)*0x30+8)!=o.entity||read<i32>(o.controllers+(50+j)*0x30+0x28)!=o.parts[j])return false;
 }else{
  BodyMap m={};if(o.root!=o.entity||o.count!=2||!findBody(o.art,m))return false;
  for(u32 j=0;j<2;++j)if(o.parts[j]!=m.replacement[j])return false;
 }
 return true;
}
bool textureTrack(u64 root,u64 entity,u64 model,u64 art,u64 resource,u64 controllers,const i32*parts,u32 count,u32 kind){
 if(!textureRelease||count<1||count>4)return false;
 TextureOwner next={};next.root=root;next.rootArt=read<u64>(root+0xd8);next.entity=entity;next.model=model;
 next.art=art;next.resource=resource;next.controllers=controllers;next.count=count;next.kind=kind;
 next.identity[0]=read<u64>(entity+0x10);next.identity[1]=read<u64>(entity+0x18);
 for(u32 j=0;j<count;++j)next.parts[j]=parts[j];
 if(!textureOwnerValid(next))return false;
 TextureOwner*slot=nullptr;
 for(auto&o:textureOwners){
  if(o.model==model&&o.entity==entity&&textureOwnerValid(o)){next.pending=o.pending;o=next;return true;}
  if(!slot&&(!o.entity||!textureOwnerValid(o)))slot=&o;
 }
 if(!slot)return false;*slot=next;texturePinsDirty=true;return true;
}
bool suitPinTexture(u64 texture){
 if(!texture)return true;
 for(u32 i=0;i<MaxSuitPinnedTextures;++i)if(suitPinnedTextures[i]==texture)return true;
 if(!textureRelease||suitPinnedCount>=MaxSuitPinnedTextures||!writable((void*)texture,0x28))return false;
 const u32 refs=read<u32>(texture+8)&0x3fffff;
 if(!refs||refs==0x3fffff||!read<u64>(texture))return false;
 for(u32 i=0;i<MaxSuitPinnedTextures;++i)if(!suitPinnedTextures[i]){
  _InterlockedIncrement((volatile long*)(texture+8));suitPinnedTextures[i]=texture;++suitPinnedCount;texturePinsDirty=true;return true;
 }
 return false;
}
bool textureNotify(const TextureOwner&o);
void texturePending(u64 model){for(auto&o:textureOwners)if(o.model==model&&o.entity)o.pending=true;}
void textureComplete(u64 model){for(auto&o:textureOwners)if(o.model==model&&o.entity)o.pending=false;}
bool sweepTexturePins(bool force=false){
 if(!textureRelease)return false;
 const u64 now=GetTickCount64();if(!force&&!texturePinsDirty&&now-textureLastSweep<1000)return true;
 memset(textureUsed,0,sizeof(textureUsed));
 for(auto&o:textureOwners){
  if(!o.entity)continue;
  if(!textureOwnerValid(o)){o={};continue;}
  // Do not drop resources from a partially completed renderer update. Finish the
  // native dirty notification first, or retain all pins until a later callback.
  if(o.pending){if(!textureNotify(o))return false;o.pending=false;}
  for(u32 j=0;j<o.count;++j){u64 handle=0;i32 n=0;
   // A transient/unreadable table never permits releasing possibly live references.
   if(!suitModelHandle(o.model,o.parts[j],handle)||!suitShaderSnapshot(handle,textureGcRecords,n))return false;
   for(i32 k=0;k<n;++k){const auto&r=textureGcRecords[k];
    if(!at<u32>(r.raw,0)||at<u32>(r.raw,0x28)!=5)continue;const u64 texture=at<u64>(r.raw,0x30);
    for(u32 t=0;t<MaxSuitPinnedTextures;++t)if(texture&&suitPinnedTextures[t]==texture)textureUsed[t]=1;
   }
  }
 }
 for(u32 i=0;i<MaxSuitPinnedTextures;++i)if(suitPinnedTextures[i]&&!textureUsed[i]){
  const u64 texture=suitPinnedTextures[i];suitPinnedTextures[i]=0;--suitPinnedCount;
  textureRelease(texture); // native flagged/deferred final-release path, not a guessed destructor
 }
 texturePinsDirty=false;textureLastSweep=now;return true;
}
bool textureNotify(const TextureOwner&o){
 if(!textureOwnerValid(o)||!suitModelDirty)return false;
 const u64 scene=read<u64>(o.model+0x90),parts=read<u64>(scene+0x48);
 const i32 n=read<i32>(scene+0x40);
 if(n<1||n>55||!writable((void*)o.model,0x200)||!writable((void*)scene,0x110)||!writable((void*)parts,u64(n)*0x1b0))return false;
 const u64 manager=read<u64>(image+0x6f366a8),notify=manager?read<u64>(read<u64>(manager)+0x88):0;
 if(manager&&(notify<image||notify>=image+0x2d00000))return false;
 suitModelDirty(o.model,0x8000);*(u32*)(o.model+0x1b0)|=0x80000;
 if(manager)((void(*)(u64,u64))notify)(manager,o.model+8);else *(u32*)(o.model+0x34)|=0x1000000;
 *(u8*)(scene+0x109)=1;for(i32 i=0;i<n;++i)*(u8*)(parts+u64(i)*0x1b0+0x1a1)=1;
 return true;
}
void clearInactiveTextures(const BodyMap&body){
 if(!validBody(body)||!suitResetOverride)return;
 for(auto&o:textureOwners)if(o.root==body.entity&&textureOwnerValid(o)){
  bool changed=false;
  for(u32 j=0;j<o.count;++j){u64 handle=0;i32 n=0;
   if(!suitModelHandle(o.model,o.parts[j],handle)||!suitShaderSnapshot(handle,textureGcRecords,n))continue;
   for(i32 k=0;k<n;++k){const auto&r=textureGcRecords[k];const u32 key=at<u32>(r.raw,0);
    if(!key||at<u32>(r.raw,0x28)!=5)continue;
    if(o.kind&&key!=0x6c5c9cb0&&key!=0x7f0c6f44&&key!=0x0d67ec47)continue;
    if(!textureOwnerValid(o))return;
    texturePending(o.model);SuitShaderId id={short(k),short(o.parts[j]),key};suitResetOverride(handle,&id,0);changed=true;
   }
  }
  if(changed){if(textureNotify(o))o.pending=false;texturePinsDirty=true;}
 }
}
