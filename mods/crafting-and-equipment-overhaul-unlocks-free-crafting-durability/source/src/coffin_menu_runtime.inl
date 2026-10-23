// Presentation scoped to the exact menu-local alias. No persistent item IDs.
#include "coffin_menu_policy.hpp"
#include "coffin_menu_sites.hpp"
extern "C" void CoffinCargoDetailIconThunk(),CoffinCargoHudIconThunk(),CoffinCargoFocusIconThunk();
extern "C" int StoryCoffinIconGpuPoll(void*,u64*,u64*);
extern "C" void CoffinMenuGridIconThunk(),CoffinMenuDetailIconThunk(),CoffinMenuBasketIconThunk(),CoffinMenuSummaryIconThunk();
extern "C" void CoffinMenuGridNameThunk(),CoffinMenuDetailNameThunk(),CoffinMenuSelectedNameThunk(),CoffinMenuSelectedContentNameThunk(),CoffinMenuBasketNameThunk(),CoffinMenuSummaryNameThunk();
namespace coffin_menu_runtime {
volatile long enabled=0,iconBusy=0,nameLogged=0,iconLogged=0;
coffin_menu::IconGraph*icon=nullptr;bool iconFailed=false;u64 retry=0;
template<class T>T get(u64 a){T value{};rd(a,value);return value;}
bool selected(u64 value,bool row){
 if(_InterlockedCompareExchange(&enabled,0,0)!=1)return false;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 auto own=[](u64 p){return redcoffin_runtime::ownAlias(p);};
 return row?coffin_menu::selectedRow(reader,own,image,value):coffin_menu::selectedInstance(reader,own,image,value);
}
const char*title(u64 nativeString){
 if(settings.silenced.language==1)return "Surfsarg (Story - Rot)";
 if(settings.silenced.language==2)return "Coffin Board (Story - Red)";
 char prefix[8]={};u64 data=get<u64>(nativeString);
 if(data&&readmem(data,prefix,8)&&!memcmp(prefix,"Surfsarg",8))return "Surfsarg (Story - Rot)";
 return "Coffin Board (Story - Red)";
}
u64 makeName(u64 original){
 const char*text=title(original);u32 n=0;while(text[n])++n;u64 temporary=0;
 ((void(*)(u64*,const char*,u32))(image+0xA4100))(&temporary,text,n);
 return temporary;
}
void nameLog(){if(!_InterlockedExchange(&nameLogged,1))say("RED_COFFIN_V1954_MENU_NAME: exact Story row named; original catalogue strings retained.");}
bool prepareIcon(u64 source){
 if(icon)return true;
 u64 tex=get<u64>(source+0x30),gpu=get<u64>(tex+0x20);
 u8 sourceBytes[0x38]={},texBytes[0x70]={},gpuBytes[0x188]={};
 if(!readmem(source,sourceBytes,sizeof(sourceBytes))||!readmem(tex,texBytes,sizeof(texBytes))||
    !readmem(gpu,gpuBytes,sizeof(gpuBytes))||get<u64>(gpu)!=image+0x33E5D48)return false;
 u64 device=get<u64>(image+0x63E0E00),resource=0,descriptor=0;if(!device)return false;
 int status=StoryCoffinIconGpuPoll((void*)device,&resource,&descriptor);
 for(u32 i=0;i<50&&!status;++i){Sleep(1);status=StoryCoffinIconGpuPoll((void*)device,&resource,&descriptor);}
 if(status<0){iconFailed=true;say("RED_COFFIN_V1954_ICON_GPU_REFUSED");return false;}
 if(!status)return false;
 auto*g=(coffin_menu::IconGraph*)VirtualAlloc(nullptr,sizeof(coffin_menu::IconGraph),0x3000,4);if(!g)return false;
 ((void(*)(u64,void*))(image+0x270D4D0))(0,g->ui);
 ((void(*)(u64,void*))(image+0x24E5B20))(0,g->texture);
 ((void*(*)(void*))(image+0x2112EE0))(g->gpu);
 if(!coffin_menu::iconGraph(*g,sourceBytes,texBytes,gpuBytes,resource,descriptor)){
  VirtualFree(g,0,0x8000);iconFailed=true;say("RED_COFFIN_V1954_ICON_GRAPH_REFUSED");return false;
 }
 u64 manager=get<u64>(image+0x623FBC8);if(!manager){VirtualFree(g,0,0x8000);return false;}
 void*lock=(void*)(manager+0xBAA398);AcquireSRWLockExclusive(lock);
 u64 pool=manager+0xAAA378;u32 head=get<u32>(pool),capacity=get<u32>(pool+0x14);
 bool valid=head<capacity&&capacity<=262144;
 if(valid){i32 index=((i32(*)(u64))(image+0x2071DF0))(pool);valid=index>0&&u32(index)<capacity;
  if(valid){coffin_menu::field<u32>(g->gpu,0x10)=(u32(index)<<2)|2;
   *(u64*)(manager+0x86A378+u32(index)*8)=u64(g->gpu);*(u8*)(manager+0xA6A378+u32(index))=0;}
 }
 ReleaseSRWLockExclusive(lock);
 if(!valid){VirtualFree(g,0,0x8000);iconFailed=true;say("RED_COFFIN_V1954_ICON_REGISTRY_REFUSED");return false;}
 _InterlockedIncrement((long*)(source+8));icon=g;
 say("RED_COFFIN_V1954_ORIGINAL_STORY_ICON_READY: original 61050:0 BC7 pixels, 512x320, native colors and alpha.");return true;
}
u64 storyIcon(u64 fallback){
 if(!fallback||iconFailed)return fallback;
 if(_InterlockedCompareExchange(&iconBusy,1,0))return fallback;
 if(!icon&&GetTickCount64()>=retry){retry=GetTickCount64()+1000;prepareIcon(fallback);}
 u64 result=icon?u64(icon->ui):fallback;
 _InterlockedExchange(&iconBusy,0);return result;
}
extern "C" u64 CoffinMenuIcon(u64 catalogue,u64 name,u64 instance){
 u64 fallback=((u64(*)(u64,u64))(image+0x17BE390))(catalogue,name);
 return selected(instance,false)?storyIcon(fallback):fallback;
}
extern "C" u64 CoffinCargoIconById(u64 catalogue,u64 name,u64 cargo){
 // Preserve the existing weapon and public shoe-addon icon dispatcher.
 u64 fallback=((u64(*)(u64,u64))(image+0x17BE390))(catalogue,name);
 if(_InterlockedCompareExchange(&enabled,0,0)!=1||
    !redcoffin_runtime::coffin_direct_runtime::isStoryCargo(cargo))return fallback;
 u64 result=storyIcon(fallback);
 if(result!=fallback&&!_InterlockedExchange(&iconLogged,1))
  say("RED_COFFIN_V1955_CARGO_DETAIL_ICON: exact live Story cargo; original red menu pixels.");
 return result;
}
extern "C" u64 CoffinCargoIcon(u64 catalogue,u64 name,u64 record){
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 return CoffinCargoIconById(catalogue,name,coffin_session::recordCargo(reader,image,record));
}
void pickupIcon(u64 context,u64 parent,u64 record){
 // Frachtabholung uses 1569190 (56db8a50), independently of 16BB8A0.
 // Run the complete native update first, including its other resource branch.
 ((void(*)(u64,u64,u64))(image+0x1569190))(context,parent,record);
 if(_InterlockedCompareExchange(&enabled,0,0)!=1)return;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const u64 cargo=coffin_session::recordCargo(reader,image,record);
 if(!redcoffin_runtime::coffin_direct_runtime::isStoryCargo(cargo))return;
 u64 root=get<u64>(get<u64>(image+0x623ebf8)),bag=get<u64>(record+0x38),commodity=get<u64>(bag+0x50);
 if(!root||!commodity)return;
 u64 catalogue=((u64(*)(u64))(image+0x14a01a0))(root);
 u64 fallback=((u64(*)(u64,u64))(image+0x17be390))(catalogue,commodity+0x50);
 u64 red=storyIcon(fallback);if(!red||red==fallback)return;
 u32 key=0x56db8a50;u64 node=((u64(*)(u64,u64,const u32*,u64))(image+0x232b3d0))(context+0x20,parent,&key,0);
 if(!node)return;
 ((u64(*)(u64,u64,u64))(image+0x67da30))(context+0x20,node,red);
 if(!_InterlockedExchange(&iconLogged,1))say("RED_COFFIN_V1956_PICKUP_DETAIL_ICON: red Story image bound to cargo pickup/detail node 56DB8A50.");
}
extern "C" u64 CoffinMenuName(u64 ctx,u64 node,u64 value,u64 row){
 using Fn=u64(*)(u64,u64,u64);auto native=(Fn)(image+0x1489DE0);
 if(!selected(row,true))return native(ctx,node,value);
 u64 tmp=makeName(value);u64 result=native(ctx,node,u64(&tmp));
 ((void(*)(u64*))(image+0xA38A0))(&tmp);nameLog();return result;
}
extern "C" u64 CoffinMenuCopyName(u64 dst,u64 src,u64 row){
 // A3A20 is MOVE assignment: preserve its source-consuming semantics first.
 u64 result=((u64(*)(u64,u64))(image+0xA3A20))(dst,src);
 if(selected(row,true)){
  u64 tmp=makeName(dst);((u64(*)(u64,u64))(image+0xA3940))(dst,u64(&tmp));
  ((void(*)(u64*))(image+0xA38A0))(&tmp);nameLog();
 }
 return result;
}
bool setup(){
 const u64 targets[]={u64(&CoffinMenuGridIconThunk),u64(&CoffinMenuDetailIconThunk),u64(&CoffinMenuBasketIconThunk),u64(&CoffinMenuSummaryIconThunk),u64(&CoffinMenuGridNameThunk),u64(&CoffinMenuDetailNameThunk),u64(&CoffinMenuSelectedNameThunk),u64(&CoffinMenuSelectedContentNameThunk),u64(&CoffinMenuBasketNameThunk),u64(&CoffinMenuSummaryNameThunk),u64(&CoffinCargoDetailIconThunk),u64(&CoffinCargoHudIconThunk),u64(&CoffinCargoFocusIconThunk),u64(&pickupIcon),u64(&pickupIcon),u64(&pickupIcon)};
 static_assert(sizeof(targets)/sizeof(targets[0])==sizeof(coffin_menu::Hooks)/sizeof(coffin_menu::Hooks[0]));
 constexpr u32 hookCount=sizeof(targets)/sizeof(targets[0]);
 u8 bytes[5];void*relays[hookCount]={};
 for(u32 i=0;i<hookCount;++i){auto&h=coffin_menu::Hooks[i];
  if(!readmem(image+h.rva,bytes,5)||memcmp(bytes,h.bytes,5)){say("RED_COFFIN_V1954_MENU_HOOK_REFUSED: callsite differs; presentation disabled.");return false;}
 }
 for(u32 i=0;i<hookCount;++i)if(!(relays[i]=relayNear(coffin_menu::Hooks[i].rva,targets[i]))){
  for(u32 j=0;j<i;++j)VirtualFree(relays[j],0,0x8000);return false;
 }
 for(u32 i=0;i<hookCount;++i){auto&h=coffin_menu::Hooks[i];bool ok=false;
  for(u32 attempt=0;attempt<12;++attempt){if(patchOnce(h.rva,h.bytes,relays[i])){ok=true;break;}Sleep(50);}
  if(!ok){for(u32 j=i;j<hookCount;++j)VirtualFree(relays[j],0,0x8000);say("RED_COFFIN_V1954_MENU_PARTIAL: wrappers stay native; restart required.");return false;}
 }
 _InterlockedExchange(&enabled,1);say("RED_COFFIN_V1956_MENU_READY: 10 crafting, 3 HUD and 3 cargo-pickup callsites; addon icon dispatcher retained.");return true;
}
}
