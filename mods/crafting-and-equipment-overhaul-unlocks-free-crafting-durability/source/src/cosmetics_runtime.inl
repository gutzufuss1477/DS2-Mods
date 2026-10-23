#include "cosmetics_sites.hpp"
namespace cosmetics_runtime {
using List=void(*)(u64,u8,Vector*);
using Palette=void(*)(u64);
using Predicate=u8(*)(u64,u64,u64);
List listOriginal=nullptr;Palette paletteOriginal=nullptr;Predicate predicateOriginal=nullptr;
VectorCopyFn copyOriginal=nullptr;
volatile long enabled=0,paletteThread=0;
CosmeticCandidate candidates[CosmeticItemCount];
Instance output[MaxMenu];
u32 menuCount=0;
void augment(u64 manager,Vector*out){
 Vector v={};u64 current=0;
 if(!manager||!out||!rd(image+ManagerRva,current)||current!=manager||
    !rd(u64(out),v)||v.count<0||v.count>i32(MaxMenu)||v.capacity<v.count||(!v.data&&v.count))return;
 const u64 address=u64(out);
 if(address>=manager&&address-manager<0x200)return;
 if(!TryAcquireSRWLockShared((void*)(manager+0x138)))return;
 // The verified caller supplies a temporary stack vector, never a catalogue array.
 u64 master=0;u32 masterCount=0;
 bool ok=rd(manager+0x40,master)&&rd(manager+0x38,masterCount)&&masterCount<=MaxRecipes;
 if(ok&&v.count&&u64(v.data)<master+u64(masterCount)*16&&master<u64(v.data)+u64(v.count)*16)ok=false;
 if(ok&&v.count)ok=readmem(u64(v.data),nativeCopy,u32(v.count)*sizeof(Instance));
 if(ok)ok=scan(manager);
 u32 count=0;
 if(ok)for(u32 i=0;i<recipeCount;++i){
  const Recipe&r=recipes[i];
  if(r.usage!=8||!cosmetic_item(r.key))continue;
  if(count==CosmeticItemCount){ok=false;break;}
  u16 category=0;u64 vt=0;
  if(!rd(r.resource+0x70,category)||!rd(r.resource,vt)){ok=false;break;}
  candidates[count]={r,category};
  if(vt!=image+0x3208868||r.instance.vtable!=image+0x31ff530)candidates[count].recipe.valid=0;
  ++count;
 }
 ReleaseSRWLockShared((void*)(manager+0x138));
 if(!ok){say("COSMETICS_VANILLA: catalogue or menu validation failed.");return;}
 const Build result=build_cosmetics(settings,candidates,count,nativeCopy,u32(v.count),output);
 if(result.ok&&result.added)copyOriginal(out,output,i32(result.count));
 Text t;t.add("COSMETICS_MENU ");t.dec(++menuCount);t.add(" native=");t.dec(u32(v.count));
 t.add(" added=");t.dec(result.added);t.add(" total=");t.dec(result.count);
 t.add(" candidates=");t.dec(count);t.add(" overflow=");t.dec(result.overLimit);
 t.add(result.ok?" status=LIST_READY":" status=VANILLA");log(t);
}
void costumeList(u64 manager,u8 usage,Vector*out){
 listOriginal(manager,usage,out);
 if(_InterlockedCompareExchange(&enabled,0,0)!=1||!settings.cosmeticsEnabled||
    usage!=8||_InterlockedCompareExchange(&busy,1,0)!=0)return;
 augment(manager,out);
 _InterlockedExchange(&busy,0);
}
void paletteInit(u64 menu){
 // Scope only the native costume-menu palette initialization, on this thread.
 // Vehicle, backpack and equipment palette consumers retain their original gates.
 const long tid=long(GetCurrentThreadId());
 const bool owned=_InterlockedCompareExchange(&enabled,0,0)==1&&
     cosmetic_colors_enabled(settings)&&_InterlockedCompareExchange(&paletteThread,tid,0)==0;
 paletteOriginal(menu);
 if(owned)_InterlockedExchange(&paletteThread,0);
}
u8 colorPredicate(u64 context,u64 key,u64 predicate){
 const u8 native=predicateOriginal(context,key,predicate);
 const u32 owner=u32(_InterlockedCompareExchange(&paletteThread,0,0));
 return cosmetic_color_override(_InterlockedCompareExchange(&enabled,0,0)==1&&
     cosmetic_colors_enabled(settings),owner,GetCurrentThreadId())?u8(1):native;
}
bool setup(){
 listOriginal=(List)(image+Hooks[0].target);paletteOriginal=(Palette)(image+Hooks[1].target);
 predicateOriginal=(Predicate)(image+Hooks[2].target);copyOriginal=(VectorCopyFn)(image+VectorCopyRva);
 if(!settings.cosmeticsEnabled){say("COSMETICS_DISABLED: native customization retained.");return true;}
 const u64 targets[]={u64(&costumeList),u64(&paletteInit),u64(&colorPredicate)};
 constexpr u32 count=sizeof(Hooks)/sizeof(Hooks[0]);static_assert(count==3);
 void*relays[count]={};u8 raw[5];
 for(u32 i=0;i<count;++i)if(!readmem(image+Hooks[i].rva,raw,5)||memcmp(raw,Hooks[i].bytes,5)){
  say("COSMETICS_BLOCKED: callsite changed or owned by another mod; native behavior retained.");return false;
 }
 for(u32 i=0;i<count;++i)if(!(relays[i]=relayNear(Hooks[i].rva,targets[i]))){
  for(u32 j=0;j<i;++j)VirtualFree(relays[j],0,0x8000);return false;
 }
 for(u32 i=0;i<count;++i){
  bool ok=false;
  for(u32 retry=0;retry<12;++retry){if(patchOnce(Hooks[i].rva,Hooks[i].bytes,relays[i])){ok=true;break;}Sleep(50);}
  if(!ok){
   for(u32 j=i;j<count;++j)VirtualFree(relays[j],0,0x8000);
   say("COSMETICS_PARTIAL: installed wrappers stay native; restart required.");return false;
  }
 }
 _InterlockedExchange(&enabled,1);
 say("COSMETICS_V1957_READY: 52 native outfit/accessory entries; menu-local copies; scoped native colors; no progression writes.");
 return true;
}
}
