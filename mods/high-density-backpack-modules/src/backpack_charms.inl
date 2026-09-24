// Native available-item enumeration, not menu navigation, determines effects.
extern "C" __declspec(dllimport) u64 WINAPI GetTickCount64();
extern "C" long _InterlockedCompareExchange(volatile long*,long,long);
extern "C" long _InterlockedExchange(volatile long*,long);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
static volatile long g_charmMask=0; // Available, not automatically equipped.
static volatile long g_extraCharmMask=0;
static u8 g_extraCharms[6]={};
static volatile long g_refreshLock=0;
static u64 g_charmRefresh=0;
static u8* g_charmCatalogue=0;
static u8 g_charmTable[256][8];
static void initialize_charm_table() {
    for (u32 mask=0;mask<256;++mask)
        for (u32 id=1;id<=8;++id) g_charmTable[mask][id-1]=(mask&(1u<<(id-1)))?(u8)id:0;
}
static u32 native_charm_mask(const u8* slots) {
    u32 mask=0;
    if (slots) for (u32 i=0;i<2;++i)
        if (slots[i]>=1 && slots[i]<=8) mask|=1u<<(slots[i]-1);
    return mask;
}
// Called by the assembly pointer adapters; returned storage is immutable.
extern "C" const u8* virtual_charm_array(const u8* nativeSlots) {
    if (!nativeSlots) return g_charmTable[0];
    const u32 mask=native_charm_mask(nativeSlots) | (g_ready?((u32)g_charmMask&(u32)g_extraCharmMask):0);
    return g_charmTable[mask&255];
}
static void refresh_charms() {
    if (!g_ready || _InterlockedCompareExchange(&g_refreshLock,1,0)!=0) return;
    u8* catalogue=*(u8**)(g_imageBase+0x623E540);
    const u64 now=GetTickCount64();
    if (catalogue!=g_charmCatalogue) {
        g_charmCatalogue=catalogue; g_charmRefresh=0;
        _InterlockedExchange(&g_charmMask,0);
    }
    if (catalogue && (!g_charmRefresh || now-g_charmRefresh>=1000)) {
        const bool firstRefresh=g_charmRefresh==0;
        g_charmRefresh=now;
        struct Vector { s32 count; u32 capacity; u8* data; } available={0,0,0};
        using Enumerate=void (FASTCALL *)(u8*,u8,Vector*);
        using Destroy=void (FASTCALL *)(Vector*,u64);
        ((Enumerate)(g_imageBase+0xB6FC20))(catalogue,5,&available);
        u32 mask=0;
        u8** definitions=*(u8***)(catalogue+0x10);
        if (definitions && available.data && available.count>0 && available.count<=4096) {
            for (s32 i=0;i<available.count;++i) {
                const u32 key=*(u32*)(available.data+i*0x10+8)&0xFFF;
                u8* item=definitions[key];
                if (!item) continue;
                const u32 hash=*(u32*)(item+0x20);
                for (u32 j=0;j<9;++j) {
                    const u8* mapping=g_imageBase+0x2DD37D0+j*8;
                    const u8 id=mapping[0];
                    if (id>=1 && id<=8 && *(const u32*)(mapping+4)==hash) mask|=1u<<(id-1);
                }
            }
        }
        const s32 availableCount=available.count;
        ((Destroy)(g_imageBase+0xB7E840))(&available,0);
        const u32 before=(u32)_InterlockedExchange(&g_charmMask,(long)mask);
        if (firstRefresh || before!=mask) {
            char line[128]; u32 n=0;
            append_text(line,sizeof(line),&n,"UNLOCKED_CHARM_MASK=");
            append_hex_u32(line,sizeof(line),&n,mask);
            append_text(line,sizeof(line),&n,"; available_items=");
            append_s32_decimal(line,sizeof(line),&n,availableCount);
            append_text(line,sizeof(line),&n,"; two native visual slots\r\n");
            HANDLE log=open_log_append(); write_text(log,line);
            if (log!=(HANDLE)(s64)-1) CloseHandle(log);
        }
    }
    _InterlockedExchange(&g_refreshLock,0);
}
static void FASTCALL backpack_preview_update(u8* info,float dt) {
    refresh_charms();
    using Native=void (FASTCALL *)(u8*,float);
    ((Native)(g_imageBase+0xB39FE0))(info,dt);
}
static u8 FASTCALL has_virtual_charm(u8* snapshot,u8 type) {
    if (!snapshot) return 0;
    const u8* slots=snapshot+0x1FC;
    if (!g_ready || type<1 || type>8) return slots[0]==type || slots[1]==type;
    return (virtual_charm_array(slots)[type-1]==type)?1:0;
}
static float FASTCALL battery_capacity(u8* info) {
    refresh_charms();
    using Native=float (FASTCALL *)(u8*);
    float result=((Native)(g_imageBase+0xB3E470))(info);
    if (!g_ready || info[0x56F4] || !(g_charmMask&g_extraCharmMask&2)) return result;
    u8* manager=*(u8**)(g_imageBase+MANAGER_POINTER_RVA);
    if (!manager || (info!=*(u8**)(manager+0x45B0) && info!=*(u8**)(manager+0x45B8) &&
                     info!=*(u8**)(manager+0x45C0))) return result;
    const u32 nativeCount=(info[0x21C0]==2?1u:0u)+(info[0x21C1]==2?1u:0u);
    const float bonus=*(float*)(g_imageBase+0x3461FEC);
    if (!nativeCount) result+=bonus;
    else if (nativeCount==2) result-=bonus;
    return result;
}
extern "C" void charm_snapshot_rax();
extern "C" void charm_snapshot_rcx();
extern "C" void charm_crypto_rax();

extern "C" void charm_full_snapshot_rax();
