#include "pcc_structures.hpp"
#include "pcc_sites.generated.hpp"
extern "C" void PccCatalogueThunk();
using PccFactFn=u8(*)(u64,u64,u64);
PccFactFn pccOriginal;
volatile long pccReported=0;
extern "C" u8 PccCatalogueCheck(u64 manager,u64 context,u64 fact,u64 ammo,u64 behavior){
 const u8 native=pccOriginal(manager,context,fact);
 if(native||!settings.enabled||!settings.pccUnlockAll)return native;
 auto reader=[](u64 p,void*out,u32 n){return readmem(p,out,n);};
 const i32 identity=pcc::eligible(reader,image,manager,context,fact,ammo,behavior);
 if(identity<0)return native;
 const long bit=1L<<identity;
 long seen=_InterlockedCompareExchange(&pccReported,0,0);
 while(!(seen&bit)){
  const long old=_InterlockedCompareExchange(&pccReported,seen|bit,seen);
  if(old==seen){Text t;t.add("PCC_OVERRIDE: ammo=");t.dec(pcc::Ammo[identity].id);t.add(" category=");t.dec(pcc::Ammo[identity].category);t.add(" catalogue result only; native tier and placement retained.");log(t);break;}
  seen=old;
 }
 return 1;
}
bool pccSignaturesMatch(){
 u8 bytes[sizeof(pcc::AvailabilityBytes)];u64 slot=0;
 return readmem(image+pcc::AvailabilityRva,bytes,sizeof(pcc::AvailabilityBytes))&&
  !memcmp(bytes,pcc::AvailabilityBytes,sizeof(pcc::AvailabilityBytes))&&
  readmem(image+pcc::FactReaderRva,bytes,sizeof(pcc::FactReaderBytes))&&
  !memcmp(bytes,pcc::FactReaderBytes,sizeof(pcc::FactReaderBytes))&&
  rd(image+pcc::BehaviorVtableRva+0x208,slot)&&slot==image+pcc::AvailabilityRva;
}
void installPccStructures(){
 if(!settings.pccUnlockAll){say("PCC_OFF: UnlockAll=0; no PCC hook installed.");return;}
 if(!pccSignaturesMatch()){say("PCC_BLOCKED: native eligibility/fact reader/vtable differs or is owned by another mod.");return;}
 pccOriginal=(PccFactFn)(image+pcc::OriginalRva);
 void*relay=relayNear(pcc::SiteRva,(u64)&PccCatalogueThunk);
 if(!relay){say("PCC_BLOCKED: near relay allocation failed.");return;}
 bool installed=false;
 for(u32 i=0;i<12;++i){if(!pccSignaturesMatch())break;if(patchOnce(pcc::SiteRva,pcc::CallBytes,relay)){installed=true;break;}Sleep(50);}
 if(!installed){VirtualFree(relay,0,0x8000);say("PCC_BLOCKED: safe callsite installation failed; native eligibility retained.");return;}
 say("PCC_ON: RVA 0x02000ABE; 13 standard builder variants; PCC tiers native; Hot Spring Digger excluded. Standard construction unlocks; native placement retained.");
}
