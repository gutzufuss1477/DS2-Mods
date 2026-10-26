// Stable gold: isolate native VertexPaintLayer1/2/3 on private ATLAS boots.
// Native 0x124520 uses image+0x6f38b20 when a paint layer has no texture.
// Hashes: CRC32C(seed=0, ASCII name) & 0x7fffffff, native 0x0a4600.
// No Wetness (0x54e8f071), suit, normal-boot or global renderer overrides written.
constexpr u32 BootPaintKeys[]={0x6c5c9cb0,0x7f0c6f44,0x0d67ec47};
constexpr u32 BootPaintNames[]={0x6c1a0001,0x6c1a0003,0x6c1a0002,0x6c1a0004};
constexpr u32 BootPaintFallbackRva=0x6f38b20,BootPaintSamplerRva=0x41bde54;
bool bootPaintReady=false;
struct BootPaintMap {u64 root,entity,art,resource,controllers,component,model; i32 parts[4];};
BootPaintMap bootPaintMaps[64]={};
SuitShaderRecord bootPaintRecords[MaxSuitTextureRecords]; // protected by the existing suitBusy gate
bool prepareBootPaint(){
 const u8 expected[]={0x40,0x56,0x48,0x81,0xec,0x80,0,0,0,0x48,0x83,0x79,0x28,0,0x48,0x8b};
 u8 actual[16];bootPaintReady=readmem(image+0x124520,actual,16)&&same(actual,expected,16);
 if(bootPaintReady)log("CHIRAL_BOOT_PAINT_READY: native empty-paint fallback verified; only private worn boots are diagnostic targets.");
 return bootPaintReady;
}
bool bootPaintValid(const BodyMap&body,const BootPaintMap&m){
 if(!validBody(body)||m.root!=body.entity||!m.entity||read<u64>(m.entity+0x80)!=m.root||
    read<u64>(m.entity+0xd8)!=m.art||read<u64>(m.entity+0x70)!=m.model||
    read<u64>(m.art+0x48)!=m.entity||read<u64>(m.art+0x30)!=m.resource||
    read<u64>(m.art+0x58)!=m.controllers||read<i32>(m.resource+0x88)!=54||
    read<u64>(m.resource+0x10)!=0xb4414c0cb2eb5bffull||read<u64>(m.resource+0x18)!=0x03b47deefa0cbab7ull)return false;
 const u64 selection=read<u64>(m.component+0x30);
 if(read<u64>(m.component+0x48)!=m.entity||
    read<u64>(selection+0x10)!=0x4c41485249434800ull||read<u64>(selection+0x18)!=0xa511c0ff20261020ull)return false;
 // Read the actual names every time: cached addresses alone do not establish ownership.
 const u64 names=read<u64>(m.resource+0x90);
 for(u32 j=0;j<4;++j){const u64 c=m.controllers+(50+j)*0x30;
  if(read<u32>(names+(50+j)*4)!=BootPaintNames[j]||m.parts[j]!=i32(50+j)||
     read<u64>(c+8)!=m.entity||read<i32>(c+0x28)!=m.parts[j])return false;
 }
 return true;
}
bool bootPaintFind(const BodyMap&body,BootPaintMap&out){
 if(!validBody(body))return false;
 for(auto&m:bootPaintMaps)if(m.root==body.entity&&bootPaintValid(body,m)){out=m;return true;}
 // Worn-boot entities are direct children in both tested ownership modes.
 // Unsupported hierarchies are deliberately ignored, never searched across other actors.
 u64 child=read<u64>(body.entity+0x88),seen[64]={};u32 count=0;
 while(child&&count<64){
  for(u32 i=0;i<count;++i)if(seen[i]==child)return false;
  seen[count++]=child;if(read<u64>(child+0x80)!=body.entity)return false;
  BootPaintMap m={};m.root=body.entity;m.entity=child;m.art=read<u64>(child+0xd8);
  m.resource=read<u64>(m.art+0x30);m.controllers=read<u64>(m.art+0x58);m.model=read<u64>(child+0x70);
  if(read<u64>(m.resource+0x10)==0xb4414c0cb2eb5bffull&&read<u64>(m.resource+0x18)==0x03b47deefa0cbab7ull){
   i32 n=0;u64 data=0;if(!suitTextureArray(child,0xa0,8,n,data)||n>24)return false;
   for(i32 i=0;i<n;++i){const u64 c=read<u64>(data+i*8),r=read<u64>(c+0x30);
    if(read<u64>(r+0x10)==0x4c41485249434800ull&&read<u64>(r+0x18)==0xa511c0ff20261020ull){
     if(m.component)return false;m.component=c;
    }
   }
   for(u32 j=0;j<4;++j)m.parts[j]=50+j;
   if(!m.component||!bootPaintValid(body,m))return false;
   for(auto&cached:bootPaintMaps)if(!cached.root||cached.root==body.entity||
       read<u64>(cached.entity+0x80)!=cached.root||read<u64>(cached.entity+0xd8)!=cached.art){cached=m;break;}
   out=m;return true;
  }
  child=read<u64>(child+0x90);
 }
 return false;
}
bool bootPaintModelReady(const BodyMap&body,const BootPaintMap&m,u64&manager,u64&notify,u64&scene,u64&parts){
 if(!bootPaintValid(body,m)||!writable((void*)m.model,0x200))return false;
 scene=read<u64>(m.model+0x90);parts=read<u64>(scene+0x48);
 if(read<i32>(scene+0x40)!=54||!writable((void*)scene,0x110)||!writable((void*)parts,54*0x1b0))return false;
 manager=read<u64>(image+0x6f366a8);notify=manager?read<u64>(read<u64>(manager)+0x88):0;
 return !manager||(notify>=image&&notify<image+0x2d00000);
}
bool syncBootPaintInner(const BodyMap&body,u64 fallback,unsigned short sampler){
 BootPaintMap m={};u64 manager=0,notify=0,scene=0,parts=0;
 if(!body.active||!fallback||!suitGetModel||!suitAllocOverride||!suitBindOverride||!suitUpdateTexture||!suitModelDirty||
    !bootPaintFind(body,m)||!bootPaintModelReady(body,m,manager,notify,scene,parts)||
    !textureTrack(body.entity,m.entity,m.model,m.art,m.resource,m.controllers,m.parts,4,1)||!suitPinTexture(fallback))return false;
 bool changed=false;
 for(u32 j=0;j<4;++j){
  if(read<u8>(m.component+(j<2?0x51:0x52))!=AtlasId)continue;
  u64 handle=0;i32 n=0;auto*records=bootPaintRecords;
  if(!suitModelHandle(m.model,m.parts[j],handle)||!suitShaderSnapshot(handle,records,n))return false;
  for(u32 key:BootPaintKeys){
   i32 slot=suitShaderFind(records,n,key);
   if(slot>=0){if(at<u32>(records[slot].raw,0x28)!=5)return false;
    if(at<u64>(records[slot].raw,0x30)==fallback&&at<unsigned short>(records[slot].raw,0x40)==sampler)continue;}
   if(!bootPaintModelReady(body,m,manager,notify,scene,parts))return false;
   handle=suitGetModel(m.model,m.parts[j]);if(!handle||read<i32>(handle+8)!=m.parts[j])return false;
   texturePending(m.model);
   if(slot<0){slot=suitAllocOverride(handle,&key);i32 count=0;u64 data=0;
    if(!suitTextureArray(handle,0x10,0x48,count,data)||slot<0||slot>=count)return false;
    u64 record=data+u64(slot)*0x48;if(read<u32>(record)!=key)return false;
    const u64 binding=read<u64>(handle);if(binding)suitBindOverride(binding,key,record+8);
    *(u32*)(record+0x28)=5;*(u64*)(record+0x30)=0;*(unsigned short*)(record+0x40)=sampler;
   }
   SuitShaderId id={short(slot),short(m.parts[j]),key};
   // A native setter may reallocate the override vector; never retain its data pointer.
   if(slot<n&&at<u64>(records[slot].raw,0x30)==fallback)suitUpdateTexture(m.model,&id,0,sampler);
   suitUpdateTexture(m.model,&id,fallback,sampler);changed=true;texturePinsDirty=true;
  }
 }
 if(changed){
  suitModelDirty(m.model,0x8000);*(u32*)(m.model+0x1b0)|=0x80000;
  if(manager)((void(*)(u64,u64))notify)(manager,m.model+8);else *(u32*)(m.model+0x34)|=0x1000000;
  *(u8*)(scene+0x109)=1;for(u32 i=0;i<54;++i)*(u8*)(parts+i*0x1b0+0x1a1)=1;
  textureComplete(m.model);
  static u32 logs=0;if(logs++<8)log("CHIRAL_BOOT_PAINT_APPLIED: native empty paint layers bound only to ATLAS worn uppers/soles; Wetness and global overrides retained.");
 }
 return true;
}
void syncBootPaint(const BodyMap&body){
 if(!bootPaintReady)return;
 const u64 fallback=read<u64>(image+BootPaintFallbackRva);const auto sampler=read<unsigned short>(image+BootPaintSamplerRva);
 if(!syncBootPaintInner(body,fallback,sampler)){static u32 waits=0;if(waits++<4)log("CHIRAL_BOOT_PAINT_WAIT: validated worn ATLAS parts or empty-paint resource not ready; retry on native callback.");}
}
