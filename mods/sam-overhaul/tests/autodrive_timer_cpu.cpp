// Execute the exact production timer-code builder in THIS test process only.
// No DS2 process, game files, injection, or external-memory access is used.
#include <windows.h>
#include <cstdio>
#include <cstring>
#include "../src/autodrive_timer_code.h"
struct Registers {
    unsigned long long rax;
    float delta;
    unsigned int padding;
    unsigned long long flagsAfter, flagsBefore;
    unsigned int deltaLanes[4],resultLanes[4];
    unsigned long long rbx;
};
extern "C" float RunTimer(void* code,void* object,float delta,Registers* registers);
static unsigned int checks=0;
static void require(bool ok,const char* message) {
    ++checks;
    if (!ok) { std::printf("FAIL: %s\n",message); ExitProcess(2); }
}
#include "autodrive_settings_tests.inl"
int main() {
    test_settings();
    using namespace sam_autodrive;
    Byte scratch[128]={};
    require(BuildTimerCode(scratch,128,0,0x1000,0x2000,1)==0,"reject multiplier zero");
    require(BuildTimerCode(scratch,128,11,0x1000,0x2000,1)==0,"reject multiplier eleven");
    require(BuildTimerCode(scratch,1,10,0x1000,0x2000,1)==0,"reject short buffer");
    require(BuildTimerCode(scratch,128,10,0x1000,0x2000,0)==0,"reject null flag");
    require(BuildTimerCode(scratch,128,10,0x1000,0x800000000ULL,1)==0,"reject far return");
    const float initial[]={0.0f,0.125f,4.99f,5.0f,9.99f};
    const float deltas[]={0.0f,1.0f/30.0f,1.0f/60.0f,1.0f/120.0f,0.25f};
    const unsigned int frameRates[]={30,60,120,240};
    for (UInt hundredths=50u; hundredths<=500u; ++hundredths) {
        const float seconds=(float)hundredths/100.0f;
        const float multiplier=5.0f/seconds;
        Byte* code=(Byte*)VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
        require(code!=nullptr,"allocate private test page");
        volatile Byte executed=0;
        UInt size=BuildTimerCode(code,128,multiplier,(Address)code,(Address)(code+128),
                                (Address)&executed);
        require(size==40u && size==MaxCodeBytes,"exact code length");
        // Run the unchanged native min(10.0,timer) and store after the code jump.
        std::memcpy(code+128,NativeContext+8,13);
        const Byte finish[]={0xC5,0xF8,0x28,0xC1,0xC3}; // vmovaps xmm0,xmm1; ret
        std::memcpy(code+141,finish,sizeof(finish));
        DWORD previous=0;
        require(VirtualProtect(code,4096,PAGE_EXECUTE_READ,&previous)!=0,"make test page RX");
        require(FlushInstructionCache(GetCurrentProcess(),code,146)!=0,"flush test page");
        for (float before:initial) for (float delta:deltas) {
            alignas(16) Byte object[160],expectedObject[160];
            std::memset(object,0xA5,sizeof(object));
            std::memcpy(object+0x80,&before,sizeof(before));
            std::memcpy(expectedObject,object,sizeof(object));
            volatile float scaled=delta*multiplier;
            volatile float accumulated=before+scaled;
            const float expected=accumulated>10.0f ? 10.0f : accumulated;
            std::memcpy(expectedObject+0x80,&expected,sizeof(expected));
            Registers registers={}; executed=0;
            const float result=RunTimer(code,object,delta,&registers);
            require(result==expected,"exact single-precision timer result");
            require(std::memcmp(object,expectedObject,sizeof(object))==0,"only timer field changed");
            require(registers.rax==0x1122334455667788ULL,"RAX preserved");
            require(registers.delta==delta,"XMM15 dt preserved");
            require(((registers.flagsAfter^registers.flagsBefore)&0xCD5ULL)==0,"RFLAGS preserved");
            require(executed==1,"production execution marker written");
            const unsigned int lanes[]={0x11223344u,0x55667788u,0x99AABBCCu};
            require(std::memcmp(registers.deltaLanes+1,lanes,12)==0,"XMM15 upper lanes preserved");
            require(std::memcmp(registers.resultLanes+1,lanes,12)==0,"XMM0 native upper lanes preserved");
            require(registers.rbx==(unsigned long long)object,"RBX object pointer preserved");
        }
        for (UInt fps:frameRates) {
            alignas(16) Byte object[160]={}; Registers registers={};
            float value=0.0f; UInt frames=0;
            while (value<=5.0f && frames<fps*7) {
                value=RunTimer(code,object,1.0f/(float)fps,&registers); ++frames;
            }
            const float measured=(float)frames/(float)fps, target=seconds;
            require(measured>=target-0.001f && measured<=target+2.0f/(float)fps,
                    "readiness threshold scales without altering frame dt");
            if (hundredths%50u==0u || hundredths==125u || hundredths==175u || hundredths==225u)
                std::printf("requested=%.2f factor=%.6f fps=%u measured=%.4f frames=%u\n",
                            seconds,multiplier,fps,measured,frames);
        }
        require(VirtualFree(code,0,MEM_RELEASE)!=0,"release private test page");
    }
    std::printf("PASS: %u checks; 451 durations, 4 frame rates, INI and timer/ABI regression.\n",checks);
    return 0;
}
