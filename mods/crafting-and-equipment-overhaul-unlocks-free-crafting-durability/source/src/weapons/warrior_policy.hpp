#pragma once
namespace warrior {
inline unsigned cap(unsigned native,unsigned multiplier,unsigned explicitRounds){
 if(native<1||native>512||multiplier<1||multiplier>10||explicitRounds>300)return 0;
 return explicitRounds?explicitRounds:native*multiplier;
}
inline unsigned boundedStock(unsigned available,unsigned magazine,unsigned reserveMagazines){
 if(reserveMagazines==0)return available;
 if(magazine<1||magazine>300||reserveMagazines>30)return available;
 const unsigned limit=magazine*reserveMagazines;
 return available>limit?limit:available;
}
}

namespace warrior {
// The game saves ammo-loader stock by category (AR, shotgun, grenades).
// It must never receive mod-provided base ammo; that would give the same rounds
// to normal weapons and double-credit Warrior weapons when switched.
struct ReserveLedger {
 unsigned remaining=0, lastTotal=0, lastNative=0;
 bool seeded=false;
};
inline void seed(ReserveLedger& s,unsigned magazine,unsigned spareMagazines){
 if(s.seeded)return;
 if(magazine<1||magazine>300||spareMagazines<1||spareMagazines>30)return;
 s.remaining=magazine*spareMagazines;s.seeded=true;
}
inline unsigned afterNativeInit(ReserveLedger& s,unsigned nativeReserve){
 if(!s.seeded)return nativeReserve;
 unsigned total=nativeReserve+s.remaining;
 if(total<nativeReserve)return nativeReserve;
 s.lastNative=nativeReserve;s.lastTotal=total;
 return total;
}
inline void afterReload(ReserveLedger& s,unsigned before,unsigned after){
 if(!s.seeded)return;
 if(after<before){
  unsigned consumed=before-after;
  s.remaining-=consumed>s.remaining?s.remaining:consumed;
 }
 s.lastTotal=after;
}
inline unsigned beforeNativeSync(ReserveLedger& s,unsigned observedTotal){
 if(!s.seeded)return observedTotal;
 if(observedTotal<s.lastTotal){
  unsigned consumed=s.lastTotal-observedTotal;
  s.remaining-=consumed>s.remaining?s.remaining:consumed;
 }
 unsigned virtualAmount=s.remaining>observedTotal?observedTotal:s.remaining;
 s.lastNative=observedTotal-virtualAmount;
 s.lastTotal=observedTotal;
 return s.lastNative;
}

} // namespace warrior

namespace warrior {
struct NativeGrantSet {
 unsigned long long tables[384]{};
 unsigned count=0;
 bool contains(unsigned long long table) const {
  for(unsigned i=0;i<count;++i)if(tables[i]==table)return true;
  return false;
 }
 bool mark(unsigned long long table){
  if(table<0x10000 || contains(table)||count>=384)return false;
  tables[count++]=table;return true;
 }
};
inline unsigned totalStock(unsigned original,unsigned magazine,unsigned spareMagazines){
 if(magazine<1||magazine>300||spareMagazines<1||spareMagazines>30||original>100000)return 0;
 unsigned extra=magazine*spareMagazines;
 if(extra>100000-original)return 0;
 return original+extra;
}
}

namespace warrior {
inline int findNativeCategoryIndex(const unsigned char*cells,unsigned count,unsigned wanted){
 if(!cells||count<1||count>64||wanted>255)return -1;
 for(unsigned i=0;i<count;++i)if(cells[i*8]==wanted)return (int)i;
 return -1;
}
inline unsigned reserveFloor(unsigned existing,unsigned magazine,unsigned reserveMagazines){
 if(existing>100000||magazine<1||magazine>300||reserveMagazines<1||reserveMagazines>30)return existing;
 unsigned target=magazine*reserveMagazines;
 return existing>target?existing:target;
}
}

namespace warrior {
inline unsigned fabricationTotal(unsigned mag,unsigned reserveMagazines){
 if(mag<1||mag>300||reserveMagazines<1||reserveMagazines>30)return 0;
 return mag*(reserveMagazines+1);
}
inline unsigned reconciledNativeReserve(unsigned native,unsigned mag,unsigned reserveMagazines,unsigned oldReserve){
 if(mag<1||mag>300||reserveMagazines<1||reserveMagazines>30||native>100000)return 0;
 unsigned required=mag*reserveMagazines;
 if(native>=required)return native; // newly crafted items already include reserve
 unsigned accessoryBonus=native>oldReserve?native-oldReserve:0;
 if(required+accessoryBonus>100000)return native;
 return required+accessoryBonus;
}
}
