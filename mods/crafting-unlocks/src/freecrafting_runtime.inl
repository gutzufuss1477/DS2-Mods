// Included after the existing startup patch utilities, inside the private namespace.
// No ingredient arrays, recipe state, material balances or game files are written.
constexpr u32 CostSiteCount=sizeof(CostSites)/sizeof(CostSites[0]);
constexpr u32 LimitSiteCount=sizeof(MaterialLimitSites)/sizeof(MaterialLimitSites[0]);
constexpr u32 FreePatchCount=CostSiteCount+LimitSiteCount;
struct FreePatch {const CostSite* spec;void* relay;u8 replacement[8];DWORD protection;bool changed;};
FreePatch freePatches[FreePatchCount];
using MaterialMaximumFn=i32(*)(u64,u64);
using ItemMaximumFn=i32(*)(u64);
extern "C" i32 FreeMaterialMaximum(u64 menu,u64 entry) {
 u32 key=0;
 if(entry&&rd(entry+0xC,key)&&free_recipe(key)) {
  // Retain the native per-item limit (coffin, blood bags and other restrictions).
  // The original callers still enforce total cargo, placement and basket limits.
  return ((ItemMaximumFn)(image+0x171BEB0))(entry);
 }
 return ((MaterialMaximumFn)(image+0x171CC50))(menu,entry);
}
bool freeSignaturesMatch() {
 for(u32 i=0;i<FreePatchCount;++i){const CostSite&s=i<CostSiteCount?CostSites[i]:MaterialLimitSites[i-CostSiteCount];u8 raw[8];
  if(s.length<5||s.length>8||(s.rva&4095)+s.length>4096||!readmem(image+s.rva,raw,s.length)||memcmp(raw,s.expected,s.length)) {
   Text t;t.add("FREECRAFTING_BLOCKED: instruction mismatch at ");t.hex(s.rva);t.add(". No cost patches installed.");log(t);return false;
  }
 }
 return true;
}
void discardFreeRelays(){for(auto&p:freePatches)if(p.relay){VirtualFree(p.relay,0,0x8000);p.relay=nullptr;}}
bool prepareFreeRelays() {
 for(u32 i=0;i<FreePatchCount;++i){FreePatch&p=freePatches[i];
  p.spec=i<CostSiteCount?&CostSites[i]:&MaterialLimitSites[i-CostSiteCount];
  u64 destination=i<CostSiteCount?(u64)CostThunks[i]:(u64)&FreeMaterialMaximum;
  p.relay=relayNear(p.spec->rva,destination);
  if(!p.relay){discardFreeRelays();return false;}
  long long delta=(long long)(u64)p.relay-(long long)(image+p.spec->rva+5);
  if(delta<(-2147483647LL-1)||delta>2147483647LL){discardFreeRelays();return false;}
  for(u32 j=0;j<8;++j)p.replacement[j]=0x90;
  p.replacement[0]=0xE8; // All relays are reached by CALL; real return-address/unwind chain.
  i32 relative=(i32)delta;memcpy(p.replacement+1,&relative,4);p.changed=false;
 }
 return true;
}
bool restoreFreePatches() {
 bool all=true;
 for(u32 i=FreePatchCount;i;--i){FreePatch&p=freePatches[i-1];if(!p.changed)continue;
  void*site=(void*)(image+p.spec->rva);DWORD ignored=0;
  if(!VirtualProtect(site,p.spec->length,0x40,&ignored)){all=false;continue;}
  memcpy(site,p.spec->expected,p.spec->length);
  bool flushed=FlushInstructionCache(GetCurrentProcess(),site,p.spec->length)!=0;
  bool protectedAgain=VirtualProtect(site,p.spec->length,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain)all=false;
  if(flushed&&protectedAgain)p.changed=false;
 }
 return all;
}
// 0=unchanged/retryable, 1=installed, 2=critical restore/resume failure.
int installFreeGroupOnce() {
 if(!collectThreads())return 0;
 bool ok=true;Context context={};context.flags=0x00100001;
 for(u32 i=0;i<threadCount&&ok;++i){Thread&t=threads[i];
  if(SuspendThread(t.h)==0xFFFFFFFF){ok=false;break;}t.suspended=true;
  if(!GetThreadContext(t.h,&context)){ok=false;break;}
  for(const auto&p:freePatches){u64 start=image+p.spec->rva;
   if(context.rip>start&&context.rip<start+p.spec->length){ok=false;break;}
  }
 }
 // Check every site before the first write: no partially enabled cost policy.
 if(ok)for(const auto&p:freePatches)if(memcmp((void*)(image+p.spec->rva),p.spec->expected,p.spec->length)){ok=false;break;}
 if(ok)for(auto&p:freePatches){void*site=(void*)(image+p.spec->rva);DWORD ignored=0;
  if(!VirtualProtect(site,p.spec->length,0x40,&p.protection)){ok=false;break;}
  memcpy(site,p.replacement,p.spec->length);p.changed=true;
  bool flushed=FlushInstructionCache(GetCurrentProcess(),site,p.spec->length)!=0;
  bool protectedAgain=VirtualProtect(site,p.spec->length,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain){ok=false;break;}
 }
 bool restored=ok?true:restoreFreePatches();
 bool resumed=resumeThreads();closeThreads();
 if(!restored||!resumed)return 2;
 return ok?1:0;
}
void installFreeCrafting() {
 if(!settings.freeCrafting){say("FREECRAFTING_OFF: native material costs; no cost patches installed.");return;}
 if(!freeSignaturesMatch())return;
 if(!prepareFreeRelays()){say("FREECRAFTING_BLOCKED: relay allocation failed. Native costs retained.");return;}
 int result=0;
 for(u32 attempt=0;attempt<12&&result==0;++attempt){result=installFreeGroupOnce();if(!result)Sleep(50);}
 if(result==2){say("CRITICAL: cost patch rollback or thread resume failed. Close/restart the game; relays retained for safety.");return;}
 if(!result){discardFreeRelays();say("FREECRAFTING_BLOCKED: group not installed; native costs retained. Send this log.");return;}
 Text t;t.add("FREECRAFTING_ON: ");t.dec(CostSiteCount);t.add(" scoped ingredient readers + ");t.dec(LimitSiteCount);
 t.add(" material-limit calls installed. 120 supported keys; no resource arrays, balances or unlock flags changed.");log(t);
}
