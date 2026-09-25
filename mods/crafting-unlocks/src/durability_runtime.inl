// Optional durability-loss scaling. Included inside the private namespace after relayNear().
extern "C" void DurabilityContentsThunk();
extern "C" void DurabilityBootsWearThunk();

struct DurabilitySite {u32 rva;u8 expected[8];void* destination;};
const DurabilitySite DurabilitySites[] = {
 {0x0123508B,{0xC5,0xFA,0x11,0x8F,0x88,0x00,0x00,0x00},(void*)&DurabilityContentsThunk},
 {0x00E70C56,{0xC5,0xFA,0x10,0xB5,0x00,0x02,0x00,0x00},(void*)&DurabilityBootsWearThunk},
};
constexpr u32 DurabilitySiteCount=sizeof(DurabilitySites)/sizeof(DurabilitySites[0]);
struct DurabilityPatch {const DurabilitySite* spec;void* relay;u8 replacement[8];DWORD protection;bool changed;};
DurabilityPatch durabilityPatches[DurabilitySiteCount];

bool durabilitySignaturesMatch(){
 for(u32 i=0;i<DurabilitySiteCount;++i){u8 raw[8];
  if(!readmem(image+DurabilitySites[i].rva,raw,8)||memcmp(raw,DurabilitySites[i].expected,8)){
   Text t;t.add("DURABILITY_BLOCKED: instruction mismatch at ");t.hex(DurabilitySites[i].rva);
   t.add(". Native durability retained.");log(t);return false;
  }
 }
 return true;
}
void discardDurabilityRelays(){
 for(auto&p:durabilityPatches)if(p.relay){VirtualFree(p.relay,0,0x8000);p.relay=nullptr;}
}
bool prepareDurabilityRelays(){
 for(u32 i=0;i<DurabilitySiteCount;++i){auto&p=durabilityPatches[i];p.spec=&DurabilitySites[i];
  p.relay=relayNear(p.spec->rva,(u64)p.spec->destination);if(!p.relay){discardDurabilityRelays();return false;}
  long long delta=(long long)(u64)p.relay-(long long)(image+p.spec->rva+5);
  if(delta<(-2147483647LL-1)||delta>2147483647LL){discardDurabilityRelays();return false;}
  for(u32 j=0;j<8;++j)p.replacement[j]=0x90;p.replacement[0]=0xE8;
  i32 relative=(i32)delta;memcpy(p.replacement+1,&relative,4);p.changed=false;
 }
 return true;
}
bool restoreDurabilityPatches(){
 bool all=true;
 for(u32 i=DurabilitySiteCount;i;--i){auto&p=durabilityPatches[i-1];if(!p.changed)continue;
  void*site=(void*)(image+p.spec->rva);DWORD ignored=0;
  if(!VirtualProtect(site,8,0x40,&ignored)){all=false;continue;}
  memcpy(site,p.spec->expected,8);bool flushed=FlushInstructionCache(GetCurrentProcess(),site,8)!=0;
  bool protectedAgain=VirtualProtect(site,8,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain)all=false;if(flushed&&protectedAgain)p.changed=false;
 }
 return all;
}
// 0=retryable, 1=installed, 2=critical rollback/resume failure.
int installDurabilityGroupOnce(){
 if(!collectThreads())return 0;bool ok=true;Context context={};context.flags=0x00100001;
 for(u32 i=0;i<threadCount&&ok;++i){Thread&t=threads[i];
  if(SuspendThread(t.h)==0xFFFFFFFF){ok=false;break;}t.suspended=true;
  if(!GetThreadContext(t.h,&context)){ok=false;break;}
  for(const auto&p:durabilityPatches){u64 start=image+p.spec->rva;
   if(context.rip>start&&context.rip<start+8){ok=false;break;}
  }
 }
 if(ok)for(const auto&p:durabilityPatches)if(memcmp((void*)(image+p.spec->rva),p.spec->expected,8)){ok=false;break;}
 if(ok)for(auto&p:durabilityPatches){void*site=(void*)(image+p.spec->rva);DWORD ignored=0;
  if(!VirtualProtect(site,8,0x40,&p.protection)){ok=false;break;}
  memcpy(site,p.replacement,8);p.changed=true;
  bool flushed=FlushInstructionCache(GetCurrentProcess(),site,8)!=0;
  bool protectedAgain=VirtualProtect(site,8,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain){ok=false;break;}
 }
 bool restored=ok?true:restoreDurabilityPatches();
 bool resumed=resumeThreads();closeThreads();
 if(!restored||!resumed)return 2;return ok?1:0;
}
void installDurability(){
 if(!settings.durabilityEnabled){say("DURABILITY_OFF: native durability loss; no durability patches installed.");return;}
 DurabilityMultiplierBits=milliFloatBits(settings.durabilityMilli);
 DurabilityUnbreakableValue=settings.durabilityUnbreakable?1u:0u;
 if(!settings.durabilityUnbreakable&&settings.durabilityMilli==1000){
  say("DURABILITY_NATIVE: enabled with Multiplier=1.0; native durability retained.");return;
 }
 if(!durabilitySignaturesMatch())return;
 if(!prepareDurabilityRelays()){say("DURABILITY_BLOCKED: relay allocation failed. Native durability retained.");return;}
 int result=0;for(u32 attempt=0;attempt<12&&result==0;++attempt){result=installDurabilityGroupOnce();if(!result)Sleep(50);}
 if(result==2){say("CRITICAL: durability rollback or thread resume failed. Close/restart the game.");return;}
 if(!result){discardDurabilityRelays();say("DURABILITY_BLOCKED: group not installed. Native durability retained.");return;}
 Text t;t.add("DURABILITY_ON: supported crafted items + crafted boots wear; cargo/container durability remains native. Multiplier=");
 t.dec(settings.durabilityMilli/1000);t.ch('.');u32 f=settings.durabilityMilli%1000;
 t.ch(char('0'+(f/100)%10));t.ch(char('0'+(f/10)%10));t.ch(char('0'+f%10));
 t.add(" unbreakable=");t.dec(settings.durabilityUnbreakable?1:0);
 t.add(". 120 crafted baggage IDs + 7 boot IDs scoped; cargo case +0x84 untouched; repair paths untouched.");log(t);
}
