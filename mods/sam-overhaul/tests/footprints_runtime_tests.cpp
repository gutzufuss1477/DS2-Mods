#include "../src/footprints/footprint_core.h"
extern "C" { int _fltused=0; }
namespace test {
uintptr_t fakeRootSlot=0;
std::atomic<uint64_t> calls{0},badArgs{0};
__declspec(noinline) void __fastcall Target(uintptr_t,uintptr_t a2,const void* a3,const void* a4,
 const void* a5,float a6,const void* a7,int a8,int a9,uint8_t a10,uint32_t a11,float a12){
 calls.fetch_add(1);
 if(a2!=0x1122334455667788ULL || a3!=(void*)0x1020304050607080ULL || a4!=(void*)0x2020304050607080ULL ||
  a5!=(void*)0x3020304050607080ULL || a6!=1.25f || a7!=(void*)0x4020304050607080ULL ||
  a8!=-1234567 || a9!=7654321 || a10!=0xA5 || a11!=0xDEADBEEF || a12!=-3.5f)badArgs.fetch_add(1);
}
footprint::Append volatile target=&Target;
void Call(uintptr_t i){target(i,0x1122334455667788ULL,(void*)0x1020304050607080ULL,(void*)0x2020304050607080ULL,
 (void*)0x3020304050607080ULL,1.25f,(void*)0x4020304050607080ULL,-1234567,7654321,0xA5,0xDEADBEEF,-3.5f);}
void Emit(const char* s){DWORD n=0,w=0;while(s[n])++n;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),s,n,&w,nullptr);}
void Check(bool good,const char* s){Emit(good?"PASS ":"FAIL ");Emit(s);Emit("\r\n");if(!good)ExitProcess(1);}
bool SameBytes(const void* a,const void* b,size_t count){auto x=(const unsigned char*)a;auto y=(const unsigned char*)b;for(size_t i=0;i<count;++i)if(x[i]!=y[i])return false;return true;}
struct WorkerState{uintptr_t a,b;int iterations;};
DWORD WINAPI ConcurrentThread(void* context){auto w=(const WorkerState*)context;for(int i=0;i<w->iterations;++i){Call(w->a);Call(w->b);}return 0;}

template<class T>void Put(unsigned char* a,size_t off,T value){memcpy(a+off,&value,sizeof value);}
}
extern "C" void WINAPI TestEntry(){
 using namespace footprint;using namespace test;
 base=reinterpret_cast<uintptr_t>(&fakeRootSlot)-0x623F9E8;
 alignas(16) unsigned char resources[4][0xB0]{};
 alignas(16) unsigned char instances[4][0x120]{};
 for(int i=0;i<4;++i){Put(resources[i],0,base+ResourceVtRva);Put(resources[i],0x10,i<3?Targets[i]:Id{0xABCD,0x1234});
  Put(instances[i],0,base+InstanceVtRva);Put(instances[i],0xA8,reinterpret_cast<uintptr_t>(resources[i]));}
 uintptr_t a=reinterpret_cast<uintptr_t>(instances[0]),b=reinterpret_cast<uintptr_t>(instances[3]);
 unsigned char before[32];memcpy(before,reinterpret_cast<void*>(&Target),sizeof before);
 Call(a);Check(calls==1 && badArgs==0,"direct baseline preserves all 12 arguments");
 Check(MH_Initialize()==MH_OK,"MinHook initialization");
 Check(MH_CreateHook(reinterpret_cast<void*>(&Target),reinterpret_cast<void*>(&OnAppend),reinterpret_cast<void**>(&original))==MH_OK,"hook and trampoline creation");
 Check(MH_EnableHook(reinterpret_cast<void*>(&Target))==MH_OK,"hook enable");
 suppress=false;Call(a);Call(b);
 Check(calls==3 && badArgs==0 && matched==1 && blocked==0,"observation mode forwards target/non-target arguments");
 suppress=true;for(int i=0;i<3;++i)Call(reinterpret_cast<uintptr_t>(instances[i]));
 Check(calls==3 && blocked==3,"all three target identities selectively suppressed");
 Call(b);Call(0);Call(1);
 Check(calls==6 && badArgs==0 && invalid==2,"unknown/null/invalid context fail open");
 Put(instances[2],0,uintptr_t(0));Call(reinterpret_cast<uintptr_t>(instances[2]));
 Check(calls==7 && badArgs==0,"wrong instance vtable fails open");
 constexpr int workers=4,iterations=1000;
 WorkerState work={a,b,iterations};HANDLE threads[workers]{};
 for(int t=0;t<workers;++t){threads[t]=CreateThread(nullptr,0,ConcurrentThread,&work,0,nullptr);Check(threads[t]!=nullptr,"create parallel test worker");}
 for(auto t:threads){Check(WaitForSingleObject(t,10000)==WAIT_OBJECT_0,"parallel test worker completed");CloseHandle(t);}
 Check(calls==7+workers*iterations && blocked==3+workers*iterations && badArgs==0,"8000 concurrent calls preserve arguments and selective counts");
 // A previously unknown UUID is eligible only while referenced by verified engine metadata.
 alignas(16) unsigned char fakeManager[0x50]{},fakeMetadata[0x68]{};
 uintptr_t newArray[1]={reinterpret_cast<uintptr_t>(resources[3])};
 Put(fakeManager,0,base+0x33A6638);Put(fakeManager,0x38,reinterpret_cast<uintptr_t>(fakeMetadata));
 Put(fakeMetadata,0,base+0x33A6D20);Put(fakeMetadata,0x20,reinterpret_cast<uintptr_t>(resources[0]));
 Put(fakeMetadata,0x28,uint32_t(1));Put(fakeMetadata,0x30,reinterpret_cast<uintptr_t>(newArray));
 fakeRootSlot=reinterpret_cast<uintptr_t>(fakeManager);
 uint64_t oldCalls=calls.load(),oldBlocked=blocked.load();
 Call(b);Check(calls==oldCalls && blocked==oldBlocked+1,"new UUID accepted only through verified footprint resource membership");
 fakeRootSlot=0;
 SetLastError(0x1234);Call(0x123450001000ULL);
 Check(calls==oldCalls+1 && GetLastError()==0x1234 && badArgs==0,"inaccessible memory fails open without changing caller LastError");
 Check(MH_DisableHook(reinterpret_cast<void*>(&Target))==MH_OK,"disable hook");
 Check(SameBytes(before,reinterpret_cast<void*>(&Target),sizeof before),"original code bytes restored exactly");
 Call(a);Check(calls==9+workers*iterations && badArgs==0,"post-unhook original behavior");
 Check(MH_RemoveHook(reinterpret_cast<void*>(&Target))==MH_OK && MH_Uninitialize()==MH_OK,"hook cleanup");
 Emit("SELFTEST_COMPLETE: production no-CRT build flags, primitives and hooks; synthetic tests, not game visual validation.\r\n");ExitProcess(0);
}