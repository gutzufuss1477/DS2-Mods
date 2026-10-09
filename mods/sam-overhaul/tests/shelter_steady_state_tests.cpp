// Deterministic exact-identity and geometry tests for dev28 hot cache.
// Production native memory reads additionally check Jolt vtables, body
// ownership and generation. No DS2 game process or save is accessed.
#include <cstdio>
#include <cstring>
#include "../src/shelter_steady_state.h"
using shelter_steady_state::Key;
using shelter_steady_state::identity;
using shelter_steady_state::dimensions;
static unsigned count=0,failed=0;
static void check(bool ok,const char* name) {
 ++count;
 if(!ok)++failed;
 std::printf("%s %s\n",ok?"PASS":"FAIL",name);
}
static Key valid(){
 Key out{};
 out.shelter=0x10001000;
 out.owner=0x10002000;
 out.members=0x10003000;
 out.odradek=0x10004000;
 out.odradekResource=0x10005000;
 out.renderer=0x10006000;
 out.repair=0x10007000;
 out.repairResource=0x10008000;
 out.triggers[0]=0x10009000;
 out.triggers[1]=0x1000a000;
 out.cylinders[0]=0x1000b000;
 out.cylinders[1]=0x1000c000;
 out.ids[0]=0x80000019u;
 out.ids[1]=0x8000002au;
 return out;
}
int main(){
 const Key original=valid();
 const Key clean{};
 check(!identity(clean,clean),"empty unvalidated cache is NEVER eligible");
 check(identity(original,original),"exact verified shelter/native identities match");
 Key m=original;
 m.shelter+=0x1000;
 check(!identity(original,m),"new shelter object invalidates fast path");
 m=original;m.owner+=0x1000;
 check(!identity(original,m),"new shelter owner invalidates fast path");
 m=original;m.members+=0x1000;
 check(!identity(original,m),"new native member array invalidates fast path");
 m=original;m.repair+=0x1000;
 check(!identity(original,m),"reloaded RepairSpray component invalidates fast path");
 m=original;m.repairResource+=0x1000;
 check(!identity(original,m),"reloaded RepairSpray resource invalidates fast path");
 m=original;m.odradek+=0x1000;
 check(!identity(original,m),"reloaded Odradek component invalidates fast path");
 m=original;m.odradekResource+=0x1000;
 check(!identity(original,m),"changed Odradek resource invalidates fast path");
 m=original;m.renderer+=0x1000;
 check(!identity(original,m),"recreated render instance invalidates fast path");
 m=original;m.triggers[0]+=0x1000;
 check(!identity(original,m),"first native trigger replacement invalidates cache");
 m=original;m.triggers[1]+=0x1000;
 check(!identity(original,m),"second native trigger replacement invalidates cache");
 m=original;m.cylinders[0]+=0x1000;
 check(!identity(original,m),"first Jolt cylinder replacement invalidates cache");
 m=original;m.cylinders[1]+=0x1000;
 check(!identity(original,m),"second Jolt cylinder replacement invalidates cache");
 m=original;m.ids[0]++;
 check(!identity(original,m),"first Jolt body generation change invalidates cache");
 m=original;m.ids[1]++;
 check(!identity(original,m),"second Jolt body generation change invalidates cache");
 m=original;m.ids[1]=0xffffffff;
 check(!identity(m,m),"invalid BodyID cannot arm cache");
 m=original;m.renderer=0;
 check(!identity(m,m),"null native renderer cannot arm cache");
 m=original;m.repairResource=0;
 check(!identity(m,m),"null repair resource cannot arm cache");
 const float coat=8.6f,rain=8.0f;
 check(dimensions(rain,coat,16,16,16,8,6,8,6,8,8.6f),
       "original dev27 ring8, repair8.6, height6 remain eligible");
 check(!dimensions(rain,coat,32,16,16,8,6,8,6,8,8.6f),
       "oversized 32m draw diameter invalidates cache");
 check(!dimensions(rain,coat,16,32,16,8,6,8,6,8,8.6f),
       "changed Odradek resource size forces sync");
 check(!dimensions(rain,coat,16,16,8,8,6,8,6,8,8.6f),
       "changed Odradek component size forces sync");
 check(!dimensions(rain,8,16,16,16,8,6,8,6,8,8.6f),
       "reset 8m native coating instead of 8.6 forces re-registration");
 check(!dimensions(rain,coat,16,16,16,4,6,8,6,8,8.6f),
       "first native Jolt cylinder reset to 4m requires full scan");
 check(!dimensions(rain,coat,16,16,16,8,6,8,3,8,8.6f),
       "second Jolt vertical halfheight reset forces full scan");
 check(!dimensions(4,coat,16,16,16,8,6,8,6,8,8.6f),
       "native protection radius reset must not be cached");
 check(dimensions(8,8.6f,16.03f,16.02f,15.98f,
                  8.01f,6.01f,7.99f,5.98f,8,8.6f),
       "float noise within the original native epsilon accepted");
 check(!dimensions(8,8.6f,16.1f,16,16,8,6,8,6,8,8.6f),
       "unknown value > native epsilon denied");
 std::printf("SHELTER_STEADY_STATE_NATIVE_POLICY %s (%u tests)\n",
             failed?"FAILED":"PASSED",count);
 return failed?1:0;
}
