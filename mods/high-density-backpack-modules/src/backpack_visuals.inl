// Visual packing never writes logical module coordinates or save data.
struct VisualLayout {
    u8* info;
    u64 signature;
    backpack::Layout packed;
    u32 positioned;
    u8* models[30];
};
static VisualLayout g_visualLayouts[5]={};
static s32 local_backpack_context(u8* info) {
    u8* manager=*(u8**)(g_imageBase+MANAGER_POINTER_RVA);
    if (manager) for (s32 i=0;i<3;++i) if (*(u8**)(manager+0x45B0+i*8)==info) return i;
    return -1;
}
// The equipment menu owns another DSBackpackComponent copy. Resolve it from
// the live subspace owner, never from a retained pointer to a destroyed preview.
// B413F0 uses the same owner/component lookup; EEF960 uses +2D8/+340 for
// the backpack copied into this preview (the weak component pointer is +20).
static u8* equipment_preview_info() {
    u8* subspace=*(u8**)(g_imageBase+0x623EA00);
    if (!subspace) return 0;
    u8* owner=*(u8**)(subspace+0x38);
    if (!owner) return 0;
    using FindComponent=u8* (FASTCALL *)(u8*,const u8*);
    u8* equipment=((FindComponent)(g_imageBase+0x11FFA0))(owner+0x80,g_imageBase+0x4355C00);
    if (!equipment) return 0;
    u8* backpack=*(u8**)(equipment+0x2D8);
    return backpack?backpack+0x340:0;
}
// B413F0's live player lookup without populating or relying on its menu cache.
// A freshly loaded save may have a player backpack before that cache exists.
static u8* gameplay_backpack_info() {
    u8* players=*(u8**)(g_imageBase+0x623DF40);
    if (!players) return 0;
    const s32 count=*(s32*)(players+0x38);
    u8** entries=*(u8***)(players+0x40);
    if (!entries || count<=0 || count>1024) return 0;
    using FindComponent=u8* (FASTCALL *)(u8*,const u8*);
    auto find=(FindComponent)(g_imageBase+0x11FFA0);
    for (s32 i=0;i<count;++i) {
        u8* player=entries[i];
        if (!player || *(u32*)(player+0x70)!=0) continue;
        u8* actor=*(u8**)(player+0x48);
        if (!actor) return 0;
        u8* equipment=find(actor+0xA0,g_imageBase+0x435ECE0);
        if (!equipment) return 0;
        u8* owner=*(u8**)(equipment+0x58);
        if (!owner) return 0;
        u8* backpack=find(owner+0x80,g_imageBase+0x435D5A0);
        return backpack?backpack+0x360:0;
    }
    return 0;
}
static s32 visual_backpack_context(u8* info) {
    if (!info) return -1;
    const s32 local=local_backpack_context(info);
    if (local>=0) return local;
    if (equipment_preview_info()==info) return 3;
    return gameplay_backpack_info()==info?4:-1;
}
static VisualLayout& visual_layout(u8* info) {
    const s32 context=visual_backpack_context(info);
    VisualLayout* cache=&g_visualLayouts[context<0?4:context];
    backpack::Module modules[30];u64 hash=1469598103934665603ull;
    for (u32 i=0;i<30;++i) {
        auto& m=modules[i];m={};
        u8* c=*(u8**)(info+0x45B0+i*0x60);
        if (c) {
            m.id=*(u32*)(c+4);m.type=*c;m.x=*(s32*)(c+8);m.y=*(s32*)(c+12);
            m.orientation=*(u16*)(c+0x1A);m.active=c[0x18]!=0 && !(c[0x1C]&1);
            if (m.active) get_shape_extents(m.type,&m.a,&m.b);
        }
        const u32 values[]={m.id,m.type,(u32)m.x,(u32)m.y,(u32)m.a,(u32)m.b,m.orientation,(u32)m.active};
        for (u32 j=0;j<8;++j) {hash^=values[j];hash*=1099511628211ull;}
    }
    if (cache->info!=info || cache->signature!=hash) {
        cache->info=info;cache->signature=hash;backpack::pack(modules,cache->packed);cache->positioned=0;
    }
    return *cache;
}
struct BoneVector {s32 count,capacity;u8** data;};
extern "C" long _InterlockedIncrement(volatile long*);
#pragma intrinsic(_InterlockedIncrement)
static u8 FASTCALL visual_reference(BoneVector* out,u8* candidate) {
    using Clear=void (FASTCALL *)(BoneVector*,u8**,u64);
    using Reserve=u8** (FASTCALL *)(BoneVector*,s32);
    ((Clear)(g_imageBase+0xA2F50))(out,out->data,0);out->data=0;
    backpack::Module m={};
    m.type=*candidate;m.x=*(s32*)(candidate+8);m.y=*(s32*)(candidate+12);
    m.orientation=*(u16*)(candidate+0x1A);
    get_shape_extents(m.type,&m.a,&m.b);
    // Only installed local/menu backpacks use the separate layout. Placement previews
    // and other actors still use their original full-footprint reference scan.
    u8* manager=*(u8**)(g_imageBase+MANAGER_POINTER_RVA);
    if (g_ready) {
        for (u32 context=0;context<5;++context) {
            u8* info=context<3?(manager?*(u8**)(manager+0x45B0+context*8):0):
                (context==3?equipment_preview_info():gameplay_backpack_info());
            if (!info) continue;
            bool found=false;
            for (u32 i=0;i<30;++i) if (*(u8**)(info+0x45B0+i*0x60)==candidate) {
                VisualLayout& layout=visual_layout(info);
                if (layout.packed.shown&(1u<<i)) {m.x=layout.packed.x[i];m.y=layout.packed.y[i];}
                found=true;break;
            }
            if (found) break;
        }
    }
    // The one-cell logic permits rotations that cannot fit the visual grid at
    // all. Keep their hidden/preview model transform finite: the native caller
    // otherwise exhausts its fallback search and divides by zero bone count.
    const int width=(m.orientation&1)?m.b:m.a,height=(m.orientation&1)?m.a:m.b;
    if (width>5 || height>6) {
        m.a=1;m.b=1;
        if (m.x<0 || m.x>=5) m.x=0;
        if (m.y<0 || m.y>=6) m.y=0;
    }
    if (!backpack::footprint(m)) return 0;
    const int w=(m.orientation&1)?m.b:m.a,h=(m.orientation&1)?m.a:m.b;
    const int dx=m.orientation>=2?-1:1,dy=(m.orientation==0||m.orientation==3)?-1:1;
    // Same x-major order, native allocator and string reference counting as
    // B3FD10. Its placement callers retain the patched 1x1 behavior.
    for (int x=0;x<w;++x) for (int y=0;y<h;++y) {
        u8** data=((Reserve)(g_imageBase+0xA2E20))(out,out->count+1);
        u8* bone=*(u8**)(g_imageBase+0x6284090+((m.y+y*dy)*5+m.x+x*dx)*8);
        if (bone-0x10!=g_imageBase+0x3B34C90) _InterlockedIncrement((volatile long*)(bone-0x10));
        data[out->count++]=bone;
    }
    return 1;
}
