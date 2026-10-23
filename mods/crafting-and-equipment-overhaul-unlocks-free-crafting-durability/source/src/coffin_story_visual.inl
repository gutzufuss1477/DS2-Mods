// Private, visual-only story coffin baggage model probe.
// This is never used by the native baggage manager's world/spawn selector.
// It overrides only the single callsite in the native BAGGAGE-VIEW factory
// FUN_141236CA0+0x26, which calls FUN_1411DCB00 to select an EntityResource.
// User's canonical Coffinboard baggage/commodity/vehicle IDs never change.
extern "C" unsigned long long CoffinStoryOriginalResolver=0;
extern "C" unsigned long long CoffinStoryVisualThunk();
volatile long storyEnabled=0,storyPending=0,storyApplied=0,storyRefusedLogs=0,storyMatchLogs=0;
u64 storyManufactureTime=0,storyViewer=0;
u64 loadedStoryBag=0,loadedStoryEntity=0,loadedStoryGroup=0,loadedNormalGroup=0;
volatile long loadedStoryLookupAttempted=0;
struct StoryMemoryInfo {
 void*base;void*allocationBase;u32 allocationProtect,alignment;
 u64 regionSize;u32 state,protect,type,padding;
};
static_assert(sizeof(StoryMemoryInfo)==48,"x64 MEMORY_BASIC_INFORMATION layout");
bool storyWritable(u64 p){
 if(p<0x10000ull || p>0x00007FFFFFFFFFFFull-12)return false;
 StoryMemoryInfo m={};
 if(VirtualQuery((void*)(p+8),&m,sizeof(m))!=sizeof(m) ||
    m.state!=0x1000u ||
    (m.protect!=4u && m.protect!=8u &&
     m.protect!=0x40u && m.protect!=0x80u))return false;
 const u64 addr=p+8,begin=u64(m.base);
 return addr>=begin && addr+4>=addr && addr+4<=begin+m.regionSize;
}
bool primeStoryBeforeVehicle(u64 knownBag){
 if(loadedStoryBag&&loadedStoryEntity&&loadedStoryGroup&&loadedNormalGroup)return true;
 if(_InterlockedCompareExchange(&loadedStoryLookupAttempted,2,0)!=0)return false;
 u64 nativeBag=knownBag;
 if(!nativeBag){
  u64 manager=0,rows=0; i32 count=0;
  if(rd(image+0x623EA48,manager)&&manager>0x10000 &&
     rd(manager+0x20,count)&&count>0&&count<=128 &&
     rd(manager+0x28,rows)&&rows>0x10000){
   for(i32 i=0;i<count;++i){
    u64 bag=0,model=0,lo=0,hi=0;u8 type=0;
    if(!rd(rows+u64(i)*8,bag)||bag<0x10000 ||
       !rd(bag+0x20,type)||type!=coffin_story::CoffinboardModelType||
       !rd(bag+0x28,model)||model<0x10000||
       !rd(bag+0x10,lo)||!rd(bag+0x18,hi)||
       lo!=coffin_story::NormalBaggageUUID[0]||
       hi!=coffin_story::NormalBaggageUUID[1])continue;
    if(nativeBag&&nativeBag!=bag){nativeBag=0;break;} // ambiguous canonical bag
    nativeBag=bag;
   }
  }
 }
 bool valid=false;
 if(nativeBag){
  auto nativeReader=[](u64 address,auto&value)->bool{return rd(address,value);};
  const auto result=coffin_story::findLoaded(nativeReader,image,nativeBag);
  if(result.ready() && storyWritable(result.bag) && storyWritable(result.entity)){
   _InterlockedIncrement((long*)(result.bag+8));
   _InterlockedIncrement((long*)(result.entity+8));
   loadedStoryBag=result.bag;loadedStoryEntity=result.entity;
   loadedStoryGroup=result.storyGroup;loadedNormalGroup=result.normalGroup;
   Text t;t.add("RED_COFFIN_STORY_MODEL_RESOURCES_PRELOADED: native original Story asset group available BEFORE world model init. storyGroup=");
   t.hex(loadedStoryGroup,16);t.add(" bag=");t.hex(loadedStoryBag,16);log(t);
   valid=true;
  }
 }
 _InterlockedExchange(&loadedStoryLookupAttempted,valid?1:0);
 return valid;
}
extern "C" u64 CoffinStoryAfterResolve(u64 nativeVisual,u64,u64){return nativeVisual;}
