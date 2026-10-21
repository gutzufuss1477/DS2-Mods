// Execute production x64 MASM repair gate, including whitelist behavior.
// No DS2 game process or memory changes.
#include <cstdint>
#include <cstring>
#include <cstdio>
extern "C" {
 struct SamGateRegistration { std::uintptr_t source,resource,owner; };
 extern std::uintptr_t SamGateComponentVT;
 extern std::uintptr_t SamGateResourceVT;
 extern std::uintptr_t SamGateOwnerVT;
 extern std::uint32_t SamGateRadiusBits;
 extern volatile long SamGateRegistrationCount;
 extern SamGateRegistration SamGateRegistrations[32];
 extern std::uintptr_t SamGateContinue;
 extern std::uintptr_t SamGateSkip;
 int SamGateTestInvoke(void*);
}
static_assert(sizeof(SamGateRegistration)==24u,"Record layout must match MASM");
extern "C" __declspec(noinline) int GateMockContinue(){return 1;}
extern "C" __declspec(noinline) int GateMockSkip(){return 0;}
static unsigned tested=0;
static unsigned failed=0;
static void verify(bool actual,const char* scenario){
 if(actual)std::printf("PASS %s\n",scenario);
 else {std::printf("FAIL %s\n",scenario);++failed;}
 ++tested;
}
static void u64(void* obj,unsigned off,std::uintptr_t val) {
 std::memcpy((unsigned char*)obj+off,&val,sizeof(val));
}
static void clear(){
 SamGateRegistrationCount=0;
 for(auto& entry:SamGateRegistrations){entry.source=0;entry.resource=0;entry.owner=0;}
}
int main(){
 alignas(16) unsigned char source[0x100]{};
 alignas(16) unsigned char resource[0x60]{};
 alignas(16) unsigned char owner[0xC0]{};
 alignas(16) unsigned char untrusted[0x100]{};
 SamGateComponentVT=0x00007FF600329208ULL;
 SamGateResourceVT=0x00007FF600329670ULL;
 SamGateOwnerVT=0x00007FF600311BC8ULL;
 SamGateRadiusBits=0x41000000u;
 SamGateContinue=(std::uintptr_t)&GateMockContinue;
 SamGateSkip=(std::uintptr_t)&GateMockSkip;
 u64(source,0,SamGateComponentVT);
 u64(source,0x30,(std::uintptr_t)resource);
 u64(source,0x48,(std::uintptr_t)owner);
 source[0x70]=0;
 clear();
 verify(SamGateTestInvoke(source)==0,"no registered source preserves vanilla inactive gate");
 source[0x70]=1;
 verify(SamGateTestInvoke(source)==1,"no registered source preserves vanilla active gate");
 source[0x70]=0;
 SamGateRegistrations[0]={ (std::uintptr_t)source,(std::uintptr_t)resource,(std::uintptr_t)owner };
 SamGateRegistrationCount=1;
 verify(SamGateTestInvoke(source)==1,"registered source bypasses inactive contact");
 source[0x70]=1;
 verify(SamGateTestInvoke(source)==1,"registered active source passes");
 source[0x70]=0;
 u64(source,0,SamGateComponentVT+8u);
 verify(SamGateTestInvoke(source)==0,"source vtable mismatch fails closed");
 u64(source,0,SamGateComponentVT);
 u64(source,0x30,0);
 verify(SamGateTestInvoke(source)==0,"resource pointer not matching known entry fails closed");
 u64(source,0x30,(std::uintptr_t)resource);
 u64(source,0x48,0);
 verify(SamGateTestInvoke(source)==0,"owner pointer not matching known entry fails closed");
 u64(source,0x48,(std::uintptr_t)owner);
 SamGateRegistrations[0].source=0;
 verify(SamGateTestInvoke(source)==0,"expired cache entry fails closed");
 SamGateRegistrations[0].source=(std::uintptr_t)source;
 SamGateRegistrations[0].resource=0;
 verify(SamGateTestInvoke(source)==0,"unregistered resource pointer fails closed");
 SamGateRegistrations[0].resource=(std::uintptr_t)resource;
 SamGateRegistrations[0].owner=0;
 verify(SamGateTestInvoke(source)==0,"unregistered owner pointer fails closed");
 SamGateRegistrations[0].owner=(std::uintptr_t)owner;
 SamGateRegistrationCount=33;
 verify(SamGateTestInvoke(source)==0,"invalid registration count uses vanilla");
 SamGateRegistrationCount=1;
 verify(SamGateTestInvoke(source)==1,"valid count again enables only registered source");
 SamGateRegistrations[0]={(std::uintptr_t)untrusted,(std::uintptr_t)resource,(std::uintptr_t)owner};
 SamGateRegistrations[1]={(std::uintptr_t)source,(std::uintptr_t)resource,(std::uintptr_t)owner};
 SamGateRegistrationCount=2;
 verify(SamGateTestInvoke(source)==1,"multiple entries find correct source");
 SamGateRegistrationCount=1;
 verify(SamGateTestInvoke(source)==0,"unpublished cache record is never consulted");
 SamGateRegistrationCount=2;
 u64(untrusted,0,SamGateComponentVT+8u);
 untrusted[0x70]=0;
 verify(SamGateTestInvoke(untrusted)==0,"registered pointer wrong vtable fails closed");
 untrusted[0x70]=1;
 verify(SamGateTestInvoke(untrusted)==1,"wrong vtable and active vanilla contact passes");
 // Crucially, a dangling owner/resource address is NEVER dereferenced:
 // native source keeps mismatched pointer values, but can safely fall back.
 u64(source,0x30,0x8888888800000000ULL);
 verify(SamGateTestInvoke(source)==0,"nonsense nonnull resource pointer never dereferenced");
 u64(source,0x30,(std::uintptr_t)resource);
 u64(source,0x48,0x7777777700000000ULL);
 verify(SamGateTestInvoke(source)==0,"nonsense nonnull owner pointer never dereferenced");
 u64(source,0x48,(std::uintptr_t)owner);
 clear();
 std::printf("SCOPED_NATIVE_REPAIR_GATE_ABI_TESTS %s (%u cases)\n",failed?"FAILED":"PASSED",tested);
 return failed?1:0;
}
