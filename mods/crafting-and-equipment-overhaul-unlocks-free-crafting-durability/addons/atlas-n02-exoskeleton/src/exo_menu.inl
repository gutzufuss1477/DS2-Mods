// Source-compared native UI path from Crafting 1.9.22. Our two entry hooks chain
// through the host. Only the exact host-owned ATLAS list receives a private name.
extern "C" int ExoIconGpuPoll(void*,u64*,u64*);
extern "C" unsigned long ExoIconGpuError();
constexpr u32 IconRva=0x17BE390,CatalogueRva=0xB6CB20;
using IconLookup=u64(*)(u64,const char*const*);


constexpr u32 iconCode(const char*s){
 u32 n=0;while(*s){n^=u8(*s++);for(u32 i=0;i<8;++i)n=(n>>1)^((n&1)?0x82F63B78u:0);}
 return n&0x7fffffffu;
}
struct IconString {u32 refs,hash,length,capacity;char data[64];};
template<u32 N>constexpr IconString iconString(const char(&s)[N]){
 IconString v={1,iconCode(s),N-1,63,{}};
 for(u32 i=0;i<N;++i)v.data[i]=s[i];return v;
}
IconString chiralIconName=iconString("ut_ui_icon_atlas_n02_exoskeleton");
const char* iconFallbackName=nullptr;
u64 iconBoundList=0;
u64 iconType(u64 p){
 u64 fn=read<u64>(read<u64>(p));
 return fn>=image&&fn<image+0x2b00000?((u64(*)(u64))fn)(p)-image:0;
}
bool isAtlasIconList(const void*p){
 return p&&at<u64>(p,0x10)==0x4A634CBDCE790268ull&&
  at<u64>(p,0x18)==0xB7AE21D89446A581ull&&at<u32>(p,0x40)==1000104&&
  at<u32>(p,0x44)==iconCode("DS2Mod.Atlas.Skeleton.Equipment.v1");
}
bool privateIconName(u64 p){
 char data[64]={};
 return p>=0x10000&&read<u32>(p-8)==chiralIconName.length&&
  (read<u32>(p-12)==chiralIconName.hash||read<u32>(p-12)==0xffffffffu)&&
  readmem(p,data,chiralIconName.length+1)&&same(data,chiralIconName.data,chiralIconName.length+1);
}
// Two bounded depot traversals finish before one pointer is changed.
struct IconSurvey {u64 visited[512]={},list=0;u32 count=0;};
bool findAtlasIcon(u64 depot,IconSurvey&s,u32 depth=0){
 if(depth>24||iconType(depot)!=0x605f3a0)return false;
 for(u32 j=0;j<s.count;++j)if(s.visited[j]==depot)return true;
 if(s.count==512)return false;s.visited[s.count++]=depot;
 i32 n=read<i32>(depot+0x38);u64 rows=read<u64>(depot+0x40);
 if(n<0||n>200000||(!rows&&n))return false;
 for(i32 i=0;i<n;++i){
  u64 p=read<u64>(rows+8*i);if(!p)continue;
  u64 type=iconType(p);
  if(type==0x605f3a0){if(!findAtlasIcon(p,s,depth+1))return false;}
  else if(type==0x4333710){ // DSGameEquipmentListItem; 0x4333A00 is weapon-only
   u8 b[0x60];if(!readmem(p,b,sizeof(b)))return false;
   if(isAtlasIconList(b)){if(s.list&&s.list!=p)return false;s.list=p;}
  }
 }
 return true;
}
bool bindAtlasIconList(u64 list){
 u8 b[0x60];if(!readmem(list,b,sizeof(b))||!isAtlasIconList(b)||!writable((void*)(list+0x50),8))return false;
 u64 source=at<u64>(b,0x50);
 if(source==u64(chiralIconName.data))return list==iconBoundList;
 u32 length=read<u32>(source-8);char name[64]={};
 if(source<0x10000||length<1||length>=64||!readmem(source,name,length+1)||name[length]||
    length<11||!same(name,"ut_ui_icon_",11))return false;
 // The host already pins its native donor. Its original reference stays alive.
 iconFallbackName=(const char*)source;iconBoundList=list;
 at<u64>((void*)list,0x50)=u64(chiralIconName.data);
 logValue("EXO_ICON_SOURCE: ",name);
 log("EXO_ICON_BOUND: private rendered menu image assigned only to ATLAS list 1000104.");
 return true;
}
void* catalogueWithChiralIcon(void*p,crafting_addon::CatalogueNext chainCatalogue){
 if(!iconBoundList){
  u64 game=read<u64>(image+0x623e310),items=read<u64>(game+0x280),catalogue=read<u64>(game+0x270);
  static IconSurvey s; // Native constructor owns this work; keep traversal off its stack.
  s.count=0;s.list=0;
  if(!game||!findAtlasIcon(catalogue,s))
   log("EXO_ICON_BIND_REFUSED: catalogue depot traversal failed; host image retained.");
  else if(items!=catalogue&&!findAtlasIcon(items,s))
   log("EXO_ICON_BIND_REFUSED: item depot traversal failed; host image retained.");
  else if(!s.list)
   log("EXO_ICON_BIND_REFUSED: exact ATLAS equipment list not found; host image retained.");
  else if(!bindAtlasIconList(s.list))
   log("EXO_ICON_BIND_REFUSED: ATLAS icon string validation failed; host image retained.");
 }
 return chainCatalogue(p);
}
struct alignas(16) IconGraph {alignas(8)u8 ui[0x38],texture[0x70],gpu[0x188];};
IconGraph* iconGraph=nullptr;volatile long iconBusy=0;bool iconFailed=false;u64 iconRetry=0;
bool fillIconGraph(IconGraph&g,const void*ui,const void*gpu,u64 resource,u64 descriptor){
 if(!ui||!gpu||!resource||!descriptor||!at<u64>(g.ui,0)||!at<u64>(g.texture,0)||!at<u64>(g.gpu,0)||
    at<u8>(ui,0x20)||at<u32>(ui,0x24)!=256||at<u32>(ui,0x28)!=160||at<u8>(gpu,0x29)!=75)return false;
 for(u32 i=0;i<2;++i){void*p=i?(void*)g.texture:(void*)g.ui;
  at<u32>(p,8)=1;at<u32>(p,12)=0;
  at<u64>(p,16)=0x4e3032534b454c10ull+i;at<u64>(p,24)=0xa511c0ff20261020ull;
 }
 copy(g.ui+0x24,(const u8*)ui+0x24,8);at<u64>(g.ui,0x30)=u64(g.texture);
 at<u64>(g.texture,0x20)=u64(g.gpu);
 copy(g.gpu+0x28,(const u8*)gpu+0x28,12);
 g.gpu[0x29]=68; // EPixelFormat::BC3, same dimensions/layout as confirmed Test13.
 at<u32>(g.gpu,8)=1;at<u32>(g.gpu,0x10)=2;
 at<u32>(g.gpu,0x2c)=0x20000000u|256|(160<<15);
 at<u32>(g.gpu,0x30)=1;at<u32>(g.gpu,0x34)=0;g.gpu[0x58]=2;
 at<u64>(g.gpu,0x80)=resource;at<u64>(g.gpu,0x90)=descriptor;
 return true;
}
bool prepareChiralIcon(u64 source){
 u64 tex=read<u64>(source+0x30),gpu=read<u64>(tex+0x20);
 u8 src[0x38],gpuData[0x188];
 if(iconType(source)!=0x6065810||iconType(tex)!=0x5e1f780||!readmem(source,src,sizeof(src))||
    !readmem(gpu,gpuData,sizeof(gpuData))||at<u64>(gpuData,0)!=image+0x33e5d48||
    !writable((void*)(source+8),4))return false;
 u64 device=read<u64>(image+0x63e0e00),resource=0,descriptor=0;
 if(!device)return false;
 int status=ExoIconGpuPoll((void*)device,&resource,&descriptor);
 for(u32 i=0;i<50&&status==0;++i){Sleep(1);status=ExoIconGpuPoll((void*)device,&resource,&descriptor);}
 if(status<0){iconFailed=true;log("EXO_ICON_GPU_REFUSED: native icon retained.");return false;}
 if(!status)return false;
 auto*g=(IconGraph*)VirtualAlloc(nullptr,sizeof(IconGraph),0x3000,4);if(!g)return false;
 ((void(*)(u64,void*))(image+0x270d4d0))(0,g->ui);
 ((void(*)(u64,void*))(image+0x24e5b20))(0,g->texture);
 ((void*(*)(void*))(image+0x2112ee0))(g->gpu);
 if(!fillIconGraph(*g,src,gpuData,resource,descriptor)){
  VirtualFree(g,0,0x8000);iconFailed=true;log("EXO_ICON_GRAPH_REFUSED: unexpected donor; native icon retained.");return false;
 }
 u64 manager=read<u64>(image+0x623fbc8);if(!manager){VirtualFree(g,0,0x8000);return false;}
 void*lock=(void*)(manager+0xbaa398);AcquireSRWLockExclusive(lock);
 u64 pool=manager+0xaaa378;u32 head=read<u32>(pool),capacity=read<u32>(pool+0x14);
 bool valid=head<capacity&&capacity<=262144;
 if(valid){
  i32 index=((i32(*)(u64))(image+0x2071df0))(pool);valid=index>0&&u32(index)<capacity;
  if(valid){at<u32>(g->gpu,0x10)=(u32(index)<<2)|2;
   at<u64>((void*)manager,0x86a378+u32(index)*8)=u64(g->gpu);
   at<u8>((void*)manager,0xa6a378+u32(index))=0;
  }
 }
 ReleaseSRWLockExclusive(lock);
 if(!valid){VirtualFree(g,0,0x8000);iconFailed=true;log("EXO_ICON_REGISTRY_REFUSED: no native descriptor slot.");return false;}
 _InterlockedIncrement((long*)(source+8));iconGraph=g;
 log("EXO_RENDERED_ICON_READY: 256x160 BC3 N02 skeleton model render, transparent background.");
 return true;
}
u64 chiralCatalogueIcon(u64 catalogue,const char*const*name,IconLookup chainIcon){
 u64 str=read<u64>(u64(name));
 if(!iconBoundList||!privateIconName(str))return chainIcon(catalogue,name);
 u64 fallback=chainIcon(catalogue,&iconFallbackName);
 if(!fallback||iconFailed||iconType(catalogue)!=0x4408340)return fallback;
 if(_InterlockedCompareExchange(&iconBusy,1,0))return fallback;
 u64 now=GetTickCount64();
 if(!iconGraph&&now>=iconRetry){iconRetry=now+1000;prepareChiralIcon(fallback);}
 u64 result=iconGraph?u64(iconGraph->ui):fallback;
 _InterlockedExchange(&iconBusy,0);return result;
}
