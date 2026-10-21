// Controlled shader-only experiment. CPU radius/progression is never modified.
typedef u64 (*VisualWaveWrite)(u8*,u64,u32,const float*);
static VisualWaveWrite g_visualWaveWrite=0;
static HudDistancePatch g_visualWavePatches[2]={};
static const HudDistanceCall VISUAL_WAVE_CALLS[2]={
    {0x1D72989u,0x1D72988u,0x2119630u,{0x38,0xE8,0xA2,0x6C,0x3A,0x00,0x48,0x8B}},
    {0x1D729CBu,0x1D729C8u,0x2119630u,{0x44,0x24,0x38,0xE8,0x60,0x6C,0x3A,0x00}}
};
static volatile long g_visualWaveCount=0,g_visualWaveBefore=0,g_visualWaveAfter=0;
#include "visual_wave_anchors.inl"

extern "C" u64 visualWaveHook(u8* binding,u64 handle,u32 count,const float* value,u8* subobject){
    // RDI is the current native effect+0x60 at these two exact call sites.
    // The retained identity is compared only, never dereferenced separately.
    u64 identity=(u64)_InterlockedCompareExchange64(&g_visualWaveInstance,0,0);
    if(_InterlockedCompareExchange(&g_hooksReady,0,0)!=1||
       !(g_visualWaveScale>=0.25f&&g_visualWaveScale<1.0f)||
       !identity||(u64)subobject!=identity+0x60u||!value||count!=4u)
        return g_visualWaveWrite(binding,handle,count,value);
    u8* instance=subobject-0x60u;
    // These fields have just been consumed by the native caller. Avoid a
    // VirtualQuery/entity traversal in its per-frame rendering path.
    float diameter=value[3];
    u32 bits=floatBits(diameter);
    if(*(const u64*)instance!=(u64)(g_gameBase+RVA_EFFECT_INSTANCE_VTABLE)||
       *(const u32*)(instance+OFF_EFFECT_SIZE_OVERRIDE)!=g_targetEffectDiameterBits||
       *(const u32*)(instance+0x294u)!=bits||!(diameter>0.0f)||
       (bits&0x7F800000u)==0x7F800000u)
        return g_visualWaveWrite(binding,handle,count,value);
    __declspec(align(16)) float scaled[4];
    memcpy(scaled,value,sizeof(scaled));scaled[3]=diameter*g_visualWaveScale;
    _InterlockedExchange(&g_visualWaveBefore,(long)bits);
    _InterlockedExchange(&g_visualWaveAfter,(long)floatBits(scaled[3]));
    _InterlockedIncrement(&g_visualWaveCount);
    return g_visualWaveWrite(binding,handle,count,scaled);
}

static ApplyResult installVisualWave(u8* base){
    g_visualWaveWrite=(VisualWaveWrite)(base+0x2119630u);
    for(u32 i=0;i<2u;i++){
        ApplyResult result=installExactCall(base,&VISUAL_WAVE_CALLS[i],
            (u64)&visualWaveHook,&g_visualWavePatches[i],2u);
        if(result==APPLY_READY)continue;
        if(i&&rollbackExactCall(base,&VISUAL_WAVE_CALLS[0],&g_visualWavePatches[0])==APPLY_CRITICAL)
            result=APPLY_CRITICAL;
        return result;
    }
    return APPLY_READY;
}

static void logVisualWave(){
    static u32 last=0;u32 count=(u32)_InterlockedCompareExchange(&g_visualWaveCount,0,0);
    if(count==last)return;last=count;
    char buffer[200];u32 pos=appendText(buffer,0,200u,"VISUAL-WAVE: writes=");
    pos=appendUnsigned(buffer,pos,200u,count);
    pos=appendText(buffer,pos,200u," scaleBits=");pos=appendHex32(buffer,pos,200u,floatBits(g_visualWaveScale));
    pos=appendText(buffer,pos,200u," nativeDiameterBits=");
    pos=appendHex32(buffer,pos,200u,(u32)_InterlockedCompareExchange(&g_visualWaveBefore,0,0));
    pos=appendText(buffer,pos,200u," shaderDiameterBits=");
    pos=appendHex32(buffer,pos,200u,(u32)_InterlockedCompareExchange(&g_visualWaveAfter,0,0));
    pos=appendText(buffer,pos,200u,"\r\n");logRaw(buffer,pos);
}
