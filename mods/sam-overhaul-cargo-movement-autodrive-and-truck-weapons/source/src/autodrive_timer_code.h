#pragma once
// Same verified DSPlayerVehicleDriving timer as test33; fractional dt scaling.
// Shared with isolated native CPU tests. No global time or input changes.
namespace sam_autodrive {
typedef unsigned char Byte;
typedef unsigned int UInt;
typedef unsigned long long Address;
static const UInt TimerRva=0x01F4F917u;
static const UInt MaxCodeBytes=40u;
static const Byte NativeContext[29]={
    0xC5,0x82,0x58,0x83,0x80,0x00,0x00,0x00,
    0xC4,0xC1,0x7A,0x5D,0xCD,
    0xC5,0xFA,0x11,0x8B,0x80,0x00,0x00,0x00,
    0x41,0xC6,0x83,0x52,0x0F,0x00,0x00,0x01
};
inline UInt BuildTimerCode(Byte* output, UInt capacity, float multiplier,
                          Address codeAddress, Address returnAddress,
                          Address executedFlagAddress) {
    if (!output || !(multiplier>=1.0f && multiplier<=10.0f) ||
        !executedFlagAddress || capacity<MaxCodeBytes) return 0u;
    if (codeAddress>~(Address)0-MaxCodeBytes) return 0u;
    const long long distance=(long long)(returnAddress-(codeAddress+36u));
    if (distance < -2147483648LL || distance > 2147483647LL) return 0u;
    const Byte code[MaxCodeBytes]={
        0xC5,0x82,0x59,0x05,0x1C,0x00,0x00,0x00, // xmm0=xmm15*literal
        0xC5,0xFA,0x58,0x83,0x80,0x00,0x00,0x00, // xmm0+=timer
        0x50,0x48,0xB8,0,0,0,0,0,0,0,0,            // save rax; flag address
        0xC6,0x00,0x01,0x58,                       // flag=1; restore rax
        0xE9,0,0,0,0,                              // native clamp/store
        0,0,0,0                                   // float multiplier
    };
    for (UInt i=0u;i<MaxCodeBytes;++i) output[i]=code[i];
    for (UInt i=0u;i<8u;++i)
        output[19u+i]=(Byte)(executedFlagAddress>>(i*8u));
    const UInt relative=(UInt)(int)distance;
    for (UInt i=0u;i<4u;++i) output[32u+i]=(Byte)(relative>>(i*8u));
    const Byte* value=(const Byte*)&multiplier;
    for (UInt i=0u;i<4u;++i) output[36u+i]=value[i];
    // XMM15, RAX and RFLAGS preserved; no calls. Literal is after the jump.
    // VEX scalar operations preserve the original XMM15 upper-lane source.
    return MaxCodeBytes;
}
} // namespace sam_autodrive
