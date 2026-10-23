#include "../src/coffin_menu_policy.hpp"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using namespace coffin_menu;
int main(){unsigned n=0,failed=0;auto ck=[&](bool b,const char*s){++n;if(!b){++failed;std::printf("FAIL %s\n",s);}};
 constexpr u64 image=0x140000000ull,instance=0x200000,row=0x300000,manager=0x400000,rows=0x500000,recipe=0x600000,bag=0x700000;
 std::unordered_map<u64,u64> mem;auto read=[&](u64 p,auto&v){auto it=mem.find(p);if(it==mem.end())return false;std::memcpy(&v,&it->second,sizeof(v));return true;};
 auto own=[&](u64 p){return p==instance;};
 auto setup=[&](){mem={{instance,image+0x31ff530},{instance+8,0x80ff},{image+0x623e540,manager},{manager+8,910},{manager+0x10,rows},{rows+255*8,recipe},{recipe+0x20,RecipeKey},{recipe+0x52,9},{recipe+0x28,bag},{bag+0x44,BaggageKey},{bag+0x49,18},{row,instance},{row+12,RecipeKey}};};
 setup();ck(selectedRow(read,own,image,row),"exact alias presentation allowed");auto before=mem;
 ck(selectedInstance(read,own,image,instance),"exact icon instance allowed");ck(mem==before,"no catalogue/gameplay writes");
 ck(!selectedInstance(read,own,image,instance+16),"byte-identical native neighbor excluded");
 for(u64 p:{instance,manager+8,recipe+0x20,recipe+0x52,bag+0x44,bag+0x49,row+12}){setup();mem[p]=0;ck(!selectedRow(read,own,image,row),"invalid identity fails closed");}
 setup();mem[manager+8]=255;ck(!selectedRow(read,own,image,row),"resource index bound");
 setup();mem.erase(recipe+0x28);ck(!selectedRow(read,own,image,row),"unreadable memory fallback");
 IconGraph g{},source{};field<u64>(g.ui,0)=field<u64>(source.ui,0)=1;field<u64>(g.texture,0)=field<u64>(source.texture,0)=2;field<u64>(g.gpu,0)=field<u64>(source.gpu,0)=3;
 field<u64>(source.ui,16)=0x464e26649a54d138ull;field<u64>(source.ui,24)=0x73899fb820edf499ull;
 field<u32>(source.ui,0x24)=256;field<u32>(source.ui,0x28)=160;source.gpu[0x29]=0x4b;
 auto original=source;ck(iconGraph(g,source.ui,source.texture,source.gpu,0x900000,0xa00000),"correct native icon graph");
 ck(!std::memcmp(&source,&original,sizeof source),"original donor graph unchanged");
 ck(field<u64>(g.ui,0x30)==u64(g.texture)&&field<u64>(g.texture,0x20)==u64(g.gpu),"private graph links");
 ck(field<u64>(g.gpu,0x80)==0x900000&&field<u64>(g.gpu,0x90)==0xa00000,"own GPU handles");
 ck(field<u32>(g.gpu,0x2c)==(0x20000000u|512|(320<<15)),"physical image dimensions");
 ck(field<u32>(g.ui,0x24)==256&&field<u32>(g.ui,0x28)==160,"native layout dimensions");
 ck(field<u32>(g.ui,12)==0&&field<u32>(g.texture,12)==0,"no catalogue group identity");
 source.ui[16]^=1;ck(!iconGraph(g,source.ui,source.texture,source.gpu,1,1),"unrelated item icon rejected");source=original;
 source.gpu[0x29]=0x47;ck(!iconGraph(g,source.ui,source.texture,source.gpu,1,1),"wrong format rejected");source=original;
 ck(!iconGraph(g,source.ui,source.texture,source.gpu,0,1),"missing GPU upload fallback");
 std::printf("COFFIN_MENU_ASSERTIONS=%u FAILURES=%u\n",n,failed);return failed?1:0;}
