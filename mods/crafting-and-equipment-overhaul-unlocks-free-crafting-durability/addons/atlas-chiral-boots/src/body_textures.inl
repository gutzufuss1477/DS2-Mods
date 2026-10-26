// Native renderer texture overrides are separate from ArtParts numeric values.
// Evidence: live-3000-overrides.json; native-texture-bindings.json.
// Only the two validated suit grafts are destinations. No global/boot/skeleton writes.
constexpr u32 SuitGetModelRva=0x1662d0,SuitAllocOverrideRva=0x26ff450;
constexpr u32 SuitBindOverrideRva=0x24b7300,SuitUpdateTextureRva=0x166c70;
constexpr u32 SuitResetOverrideRva=0x26ff9b0,SuitModelDirtyRva=0x166470;
constexpr u32 MaxSuitTextureRecords=128,MaxSuitPinnedTextures=2048;
struct SuitShaderRecord {u8 raw[0x48];};
struct SuitShaderId {short slot,part;u32 parameter;};
static_assert(sizeof(SuitShaderId)==8,"Native renderer handle");
using SuitGetModel=u64(*)(u64,i32);
using SuitAllocOverride=i32(*)(u64,const u32*);
using SuitBindOverride=void(*)(u64,u32,u64);
using SuitUpdateTexture=void(*)(u64,SuitShaderId*,u64,unsigned short);
using SuitResetOverride=void(*)(u64,SuitShaderId*,u8);
using SuitModelDirty=void(*)(u64,u32);
SuitGetModel suitGetModel=nullptr;SuitAllocOverride suitAllocOverride=nullptr;
SuitBindOverride suitBindOverride=nullptr;SuitUpdateTexture suitUpdateTexture=nullptr;
SuitResetOverride suitResetOverride=nullptr;SuitModelDirty suitModelDirty=nullptr;
SuitShaderRecord suitSourceTextures[MaxSuitTextureRecords],suitTargetTextures[MaxSuitTextureRecords];
u32 suitTextureLogLines=0;
bool prepareTextureLifetime();
bool prepareSuitTextures(){
 const u32 rvas[]={SuitGetModelRva,SuitAllocOverrideRva,SuitBindOverrideRva,SuitUpdateTextureRva,SuitResetOverrideRva,SuitModelDirtyRva};
 const u8 expected[][16]={
  {0x89,0x54,0x24,0x10,0x53,0x56,0x41,0x56,0x48,0x83,0xec,0x20,0x48,0x63,0xf2,0x4c},
  {0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x8b,0x3a,0x33,0xc0,0x48,0x63},
  {0x40,0x55,0x53,0x56,0x57,0x41,0x56,0x48,0x8d,0x6c,0x24,0xc9,0x48,0x81,0xec,0x00},
  {0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57},
  {0x40,0x56,0x48,0x83,0xec,0x30,0x48,0x8b,0xf2,0x45,0x0f,0xb6,0xd0,0x48,0x0f,0xbf},
  {0x40,0x53,0x48,0x83,0xec,0x20,0x83,0x89,0xb0,0x01,0x00,0x00,0x10,0x48,0x8b,0xd9}};
 u8 bytes[16];for(u32 i=0;i<6;++i)if(!readmem(image+rvas[i],bytes,16)||!same(bytes,expected[i],16))return false;
 suitGetModel=(SuitGetModel)(image+SuitGetModelRva);suitAllocOverride=(SuitAllocOverride)(image+SuitAllocOverrideRva);
 suitBindOverride=(SuitBindOverride)(image+SuitBindOverrideRva);suitUpdateTexture=(SuitUpdateTexture)(image+SuitUpdateTextureRva);
 suitResetOverride=(SuitResetOverride)(image+SuitResetOverrideRva);suitModelDirty=(SuitModelDirty)(image+SuitModelDirtyRva);
 if(!prepareTextureLifetime())return false;
 static bool announced=false;if(!announced){announced=true;log("CHIRAL_SUIT_TEXTURES_READY: native signatures verified; owner-scoped texture forwarding enabled.");}return true;
}
bool suitTextureArray(u64 base,u32 offset,u32 stride,i32&n,u64&p){
 u8 h[16];if(!readmem(base+offset,h,16))return false;n=at<i32>(h,0);i32 cap=at<i32>(h,4);p=at<u64>(h,8);
 return n>=0&&n<=i32(MaxSuitTextureRecords)&&cap>=n&&cap<=4096&&(!n||(p>=0x10000&&writable((void*)p,u64(n)*stride)));
}
bool suitModelHandle(u64 model,i32 part,u64&handle){
 handle=0;i32 n=0;u64 p=0;if(!suitTextureArray(model,0x1e8,16,n,p))return false;
 i32 last=-2147483647;for(i32 i=0;i<n;++i){u8 b[16];if(!readmem(p+u64(i)*16,b,16))return false;
  i32 index=at<i32>(b,0);if(index<=last||index< -1||index>=55)return false;last=index;
  if(index==part)handle=at<u64>(b,8);
 }
 return !handle||(read<i32>(handle+8)==part&&writable((void*)handle,32));
}
bool suitShaderSnapshot(u64 handle,SuitShaderRecord*out,i32&count){
 count=0;if(!handle)return true;u64 p=0;
 if(!suitTextureArray(handle,0x10,0x48,count,p)||(count&&!readmem(p,out,u64(count)*0x48)))return false;
 for(i32 i=0;i<count;++i){u32 hash=at<u32>(out[i].raw,0);if(!hash)continue;
  const u32 kind=at<u32>(out[i].raw,0x28);if(kind<1||kind>5)return false;
  for(i32 j=0;j<i;++j)if(at<u32>(out[j].raw,0)==hash)return false;
 }
 return read<i32>(handle+0x10)==count&&read<u64>(handle+0x18)==p;
}
i32 suitShaderFind(const SuitShaderRecord*p,i32 n,u32 hash){for(i32 i=0;i<n;++i)if(at<u32>(p[i].raw,0)==hash)return i;return -1;}
#include "texture_lifetime.inl"
void suitTextureLog(u64 owner,u32 part,u32 hash,const char*action){
 if(suitTextureLogLines>=96)return;++suitTextureLogLines;SuitParameter e={};e.key=part;e.parameter=hash;
 suitLog(action,owner,SuitSourceNames[0],e);
}
bool suitTextureModelReady(const BodyMap&m,u64 model,u64&manager,u64&notify,u64&scene,u64&parts,i32&partCount){
 if(!validBody(m)||read<u64>(m.entity+0x70)!=model||!writable((void*)model,0x200))return false;
 scene=read<u64>(model+0x90);partCount=read<i32>(scene+0x40);parts=read<u64>(scene+0x48);
 if(partCount<1||partCount>55||!writable((void*)scene,0x110)||!writable((void*)parts,u64(partCount)*0x1b0))return false;
 for(u32 j=0;j<2;++j)if(m.original[j]<0||m.replacement[j]<0||m.original[j]>=partCount||m.replacement[j]>=partCount||m.original[j]==m.replacement[j])return false;
 manager=read<u64>(image+0x6f366a8);notify=manager?read<u64>(read<u64>(manager)+0x88):0;
 return !manager||(notify>=image&&notify<image+0x2d00000);
}
bool syncSuitTexturesInner(const BodyMap&m){
 if(!suitGetModel||!suitAllocOverride||!suitBindOverride||!suitUpdateTexture||!suitResetOverride||!suitModelDirty)return false;
 const u64 model=read<u64>(m.entity+0x70);u64 manager=0,notify=0,scene=0,parts=0;i32 partCount=0;
 if(!suitTextureModelReady(m,model,manager,notify,scene,parts,partCount))return false;
 if(!textureTrack(m.entity,m.entity,model,m.owner,m.resource,m.controllers,m.replacement,2,0))return false;
 bool changed=false,ok=true;
 for(u32 j=0;j<2&&ok;++j){
  u64 source=0,target=0;i32 sn=0,tn=0;
  if(!suitModelHandle(model,m.original[j],source)||!source||!suitModelHandle(model,m.replacement[j],target)||
     !suitShaderSnapshot(source,suitSourceTextures,sn)||!suitShaderSnapshot(target,suitTargetTextures,tn)){ok=false;break;}
  // Validate/pin source resources before creating any override. Float/color entries remain
  // owned by the existing numeric synchronization, not this texture path.
  for(i32 i=0;i<sn;++i){auto&b=suitSourceTextures[i];if(at<u32>(b.raw,0)&&at<u32>(b.raw,0x28)==5&&!suitPinTexture(at<u64>(b.raw,0x30))){ok=false;break;}}
  for(i32 i=0;i<sn&&ok;++i){const auto&b=suitSourceTextures[i];const u32 hash=at<u32>(b.raw,0);
   if(!hash||at<u32>(b.raw,0x28)!=5)continue;
   const u64 texture=at<u64>(b.raw,0x30);const auto sampler=at<unsigned short>(b.raw,0x40);
   i32 slot=suitShaderFind(suitTargetTextures,tn,hash);
   if(slot>=0){const auto&old=suitTargetTextures[slot];if(at<u32>(old.raw,0x28)!=5){ok=false;break;}
    if(at<u64>(old.raw,0x30)==texture&&at<unsigned short>(old.raw,0x40)==sampler)continue;
   }
   if(!suitTextureModelReady(m,model,manager,notify,scene,parts,partCount)){ok=false;break;}
   target=suitGetModel(model,m.replacement[j]);
   if(!target||read<i32>(target+8)!=m.replacement[j]){ok=false;break;}
   texturePending(model);
   if(slot<0){slot=suitAllocOverride(target,&hash);i32 n=0;u64 data=0;
    if(!suitTextureArray(target,0x10,0x48,n,data)||slot<0||slot>=n){ok=false;break;}
    u64 rec=data+u64(slot)*0x48;if(read<u32>(rec)!=hash){ok=false;break;}
    const u64 binding=read<u64>(target);if(binding)suitBindOverride(binding,hash,rec+8);
    *(u32*)(rec+0x28)=5;*(u64*)(rec+0x30)=0;*(unsigned short*)(rec+0x40)=sampler;
   }
   SuitShaderId id={short(slot),short(m.replacement[j]),hash};
   // The native updater compares pointer only. A sampler change needs a null reset first.
   if(slot<tn&&at<u64>(suitTargetTextures[slot].raw,0x30)==texture&&texture)suitUpdateTexture(model,&id,0,sampler);
   suitUpdateTexture(model,&id,texture,sampler);changed=true;texturePinsDirty=true;
   suitTextureLog(m.owner,SuitTargetNames[j],hash,"TEXTURE_COPIED");
   if(!suitShaderSnapshot(target,suitTargetTextures,tn)){ok=false;break;}
  }
  for(i32 i=0;i<tn&&ok;++i){const auto&b=suitTargetTextures[i];const u32 hash=at<u32>(b.raw,0);
   if(!hash||at<u32>(b.raw,0x28)!=5||suitShaderFind(suitSourceTextures,sn,hash)>=0)continue;
   if(!suitTextureModelReady(m,model,manager,notify,scene,parts,partCount)){ok=false;break;}
   texturePending(model);SuitShaderId id={short(i),short(m.replacement[j]),hash};suitResetOverride(target,&id,0);changed=true;texturePinsDirty=true;
   suitTextureLog(m.owner,SuitTargetNames[j],hash,"TEXTURE_REMOVED");
  }
 }
 if(changed&&suitTextureModelReady(m,model,manager,notify,scene,parts,partCount)){
  // Exact native texture-batch completion from 0x166e10. No GPU pointers copied.
  suitModelDirty(model,0x8000);*(u32*)(model+0x1b0)|=0x80000;
  if(manager)((void(*)(u64,u64))notify)(manager,model+8);else *(u32*)(model+0x34)|=0x1000000;
  *(u8*)(scene+0x109)=1;for(i32 i=0;i<partCount;++i)*(u8*)(parts+u64(i)*0x1b0+0x1a1)=1;
  textureComplete(model);
 }
 return ok;
}
