"""Execute the actual production fabrication gate/reset with mock native callbacks."""
from pathlib import Path
import subprocess
R=Path(__file__).resolve().parents[1]
s=(R/'src/coffin_red_runtime.inl').read_text()
gate=s[s.index('void manufactured(u64 menu){'):s.index('\n#include "coffin_direct_model_runtime.inl"')]
s=(R/'src/coffin_direct_model_runtime.inl').read_text()
reset=s[s.index('void beginFabrication(){'):s.index('\nu64 parts[2]')]
prefix=r'''
#include "../src/core.hpp"
#include "../src/coffin_direct_model_policy.hpp"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using namespace craft;
#include <intrin.h>
volatile long phase=0,appliedMask=0,verification=0,observations=0;
volatile long long owner=0;u32 renderIndices[2]={};
'''
middle=r'''
std::unordered_map<u64,u64> memory;
template<class T>bool rd(u64 p,T&v){auto i=memory.find(p);if(i==memory.end())return false;std::memcpy(&v,&i->second,sizeof(v));return true;}
volatile long enabled=1,arm=0,finished=0,tickCount=0,guardLogs=0,manufactureLogs=0;
volatile long storyEnabled=1,storyPending=0,storyApplied=0;
u64 manufactureTick=0,sourceMenu=0,finishedOwner=0,storyManufactureTime=0;
u32 previousCount=0;u64 previousOwners[2]={};u64 now=1000;unsigned nativeCalls=0,resets=0;
u64 GetTickCount64(){return now;}
bool within(u64 a,u64 b){return b-a<=redcoffin::RedManufactureWindowMs;}
bool ownAlias(u64 a){return a==0x200000;}
bool saveExisting(){return true;}
bool prepareDirect(){return true;}
void beginDirect(){++resets;beginFabrication();}
struct Text{void add(const char*){}void dec(u64){}};
void say(const char*){}void log(const Text&){}
void originalManufacture(u64){++nativeCalls;}
'''
suffix=r'''
int main(){unsigned n=0,failed=0;auto ck=[&](bool b,const char*s){++n;if(!b){++failed;std::printf("FAIL %s\n",s);}};
 constexpr u64 menu=0x100000,vehicle=0x300000;
 auto rows=[&](u32 qty=1,bool red=true){memory={{menu+0x41f0,1},{menu+0x1f0,red?0x200000ull:0x200010ull},{menu+0x1f8,qty},{menu+0x1fc,0x0184cf05}};};
 auto finish=[&](){owner=vehicle;phase=2;appliedMask=3;verification=1;observations=120;renderIndices[0]=1;renderIndices[1]=5;arm=0;};
 rows();manufactured(menu);ck(arm==1&&resets==1&&nativeCalls==1,"first red manufacture starts and calls native once");finish();
 now+=1000;manufactured(menu);ck(arm==1&&resets==2&&nativeCalls==2,"second red manufacture in same process is allowed");
 ck(owner==0&&phase==0&&appliedMask==0&&verification==0&&observations==0,"all prior fabrication state reset");
 ck(renderIndices[0]==0xffffffffu&&renderIndices[1]==0xffffffffu,"old model row indices cleared");
 ck(coffin_direct::selected(arm==1,now,manufactureTick,vehicle+0x1000,owner,previousOwners,0),"new world owner accepted");finish();
 // Loading a pre-craft save leaves the old mod globals, exactly as reported.
 previousOwners[0]=vehicle;previousCount=1;now+=1000;rows();manufactured(menu);
 ck(arm==1&&owner==0&&resets==3&&nativeCalls==3,"manufacture after loading earlier save re-arms without process restart");
 ck(!coffin_direct::selected(true,now,manufactureTick,vehicle,owner,previousOwners,1),"existing native board cannot be selected");
 ck(coffin_direct::selected(true,now,manufactureTick,vehicle+0x2000,owner,previousOwners,1),"newly made red board after reload selected");finish();
 rows(1,false);manufactured(menu);ck(resets==3&&nativeCalls==4&&!arm,"normal recipe keeps native appearance");
 rows(2,true);manufactured(menu);ck(resets==3&&nativeCalls==5&&!arm,"unsupported batch does not select an arbitrary board");
 rows();memory[menu+0x41f0]=2;memory[menu+0x210]=0x200010;memory[menu+0x218]=1;memory[menu+0x21c]=0x0184cf05;
 manufactured(menu);ck(resets==3&&nativeCalls==6&&!arm,"mixed fabrication remains explicitly excluded");
 rows();memory.erase(menu+0x1f8);manufactured(menu);ck(resets==3&&nativeCalls==7,"unreadable row falls back and calls original exactly once");
 std::printf("ACTUAL_FABRICATION_GATE_ASSERTIONS=%u FAILURES=%u\n",n,failed);return failed?1:0;
}
'''
out=R/'build/repeat_fabrication_test.cpp';out.write_text(prefix+reset+middle+gate+suffix)
exe=R/'build/repeat_fabrication_test.exe'
subprocess.run([r'C:\Program Files\LLVM\bin\clang-cl.exe','/nologo','/std:c++17','/EHsc','/MT','/O2',str(out),'/Fe'+str(exe)],cwd=R,check=True)
subprocess.run([str(exe)],cwd=R,check=True)
