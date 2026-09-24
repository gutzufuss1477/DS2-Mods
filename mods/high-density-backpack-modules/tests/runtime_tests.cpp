// Built as a DLL with /noentry: never starts the game worker.
#include "../src/high_density_backpack_modules.cpp"
#include "native_charm_loop.h"
extern "C" int test_adapter(void* function,const void* source,const void* expected,u32 mode);
extern "C" void test_module_relay(void*,u8*,u8,u8*,u64);
extern "C" u32 test_crypto_loop(void* entry,u8* component);
extern "C" int test_menu_read(void* entry,u8* manager,u32 slot,u32 expected);
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
static u8 test_info[0x5700], saved_info[0x5700], shape[0x60];
static u8 entities[30][0xE0], models[30][0xD0], nodes[30][0x30];
static u8 rendered[30], catalogue[0x180], items[9][0x80], entries[9][0x10];
static u8* definitions[16];
static u32 shadowCalls=0,destroyCalls=0;
static u32 positionCalls=0,menuBuildCount=0,nativeSetCount=0,commitCount=0;
static u8 boneStorage[30][32];static u8* bonePointers[30];
static u8 menuRows[12][0x48],controller[0x2740],builtEntries[8][0x38];
static u8 equipmentMenu[0x3D0],subspace[0x40],previewOwner[0x100];
static bool equipmentPresent=true;
static u8 playerRegistry[0x48],playerEntry[0x78],playerActor[0xC0],playerEquipment[0x60],playerOwner[0xA0];
static u8 livePlayerComponent[0x360+sizeof(test_info)];
static u8* playerEntries[2]={0,playerEntry};
static bool playerEquipmentPresent=true,playerBackpackPresent=true;
static u8* FASTCALL fake_find_component(u8* components,const u8* type) {
    if (components==playerActor+0xA0 && type==g_imageBase+0x435ECE0)
        return playerEquipmentPresent?playerEquipment:0;
    if (components==playerOwner+0x80 && type==g_imageBase+0x435D5A0)
        return playerBackpackPresent?livePlayerComponent:0;
    return equipmentPresent && components==previewOwner+0x80 && type==g_imageBase+0x4355C00?equipmentMenu:0;
}
static void FASTCALL fake_position(u8*,u8*) {++positionCalls;}
static void FASTCALL fake_bone_clear(BoneVector* v,u8** data,u64) {
    for(s32 i=0;i<v->count;++i)--*(s32*)(data[i]-0x10);
    v->count=0;v->capacity=0;
}
static u8** FASTCALL fake_bone_reserve(BoneVector* v,s32) {v->data=bonePointers;v->capacity=30;return bonePointers;}
static void FASTCALL fake_menu_build(u8* menu,u8 category,MenuVector* v) {
    menuBuildCount=(u32)v->count;
    if(category!=1 || v->count!=8)return;
    copy_bytes(builtEntries,v->data,sizeof(builtEntries));
    *(s32*)(menu+0x208)=9;*(u8**)(menu+0x210)=menuRows[0];
    *(u32*)menuRows[0]=0;menuRows[0][5]=1;
    for(u32 i=0;i<8;++i){*(u32*)menuRows[i+1]=builtEntries[i][1]?1:2;menuRows[i+1][5]=1;menuRows[i+1][0x30]=builtEntries[i][1];}
}
static void FASTCALL fake_native_set(u8*,u8) {++nativeSetCount;}
static void FASTCALL fake_commit(u8*,u8*) {++commitCount;}
static u8* FASTCALL fake_menu_reserve(MenuVector* v,s32) {
    v->data=menuRows[0];v->capacity=12;return v->data;
}
static u8* FASTCALL fake_shape(void*,u32) { return shape; }
static void FASTCALL fake_shadow(u8*,u8) { ++shadowCalls; }
static void FASTCALL fake_render(u8* output,s32,u8 enabled) { *output=enabled; }
static float FASTCALL fake_capacity(u8* info) {
    if (info[0x56F4]) return 0;
    return 100.f+5.f*(float)((info[0x21C0]==2)+(info[0x21C1]==2));
}
struct TestVector { s32 count; u32 capacity; u8* data; };
static void FASTCALL fake_enumerate(u8*,u8 category,TestVector* out) {
    out->count=category==5?4:0;out->capacity=4;out->data=&entries[0][0];
}
static void FASTCALL fake_destroy(TestVector* v,u64 flags) {
    if (!flags && v->count==4) ++destroyCalls;
    v->count=0;v->capacity=0;v->data=0;
}
static void mock(u32 rva,void* fn) {
    u8* code=g_imageBase+rva;
    code[0]=0x48;code[1]=0xB8;write_u64_le(code+2,(u64)fn);code[10]=0xFF;code[11]=0xE0;
}
extern "C" __declspec(dllexport) int run_tests() {
    u32 ids[30]={};
    for (u32 id=1;id<=30;++id) {
        int i=backpack::anchor(ids,id,0,0);
        CHECK(i>=0 && i<30 && !ids[i]);ids[i]=id;
    }
    CHECK(backpack::anchor(ids,31,0,0)==-1);
    CHECK(backpack::anchor(ids,0,0,0)==-1);
    CHECK(backpack::anchor(ids,0xFFFFFFFF,0,0)==-1);
    CHECK(backpack::anchor(ids,ids[29],0,0)==29);
    ids[17]=0;
    CHECK(backpack::anchor(ids,31,(-2147483647-1),2147483647)==17);
    CHECK(backpack::anchor(ids,31,99,-99)==17);
    backpack::Module modules[30];
    // Exercise every native orientation, anchor and dimensions up to the grid size.
    for (int a=1;a<=6;++a) for (int b=1;b<=6;++b) for (u16 o=0;o<4;++o) {
        for (u32 i=0;i<30;++i) {
            auto& m=modules[i];m.id=i+1;m.type=(u8)(1+i%4);m.x=i%5;m.y=i/5;
            m.a=a;m.b=b;m.orientation=o;m.active=true;
            const u32 f=backpack::footprint(m);
            const int w=(o&1)?b:a,h=(o&1)?a:b;
            const int x2=m.x+((o>=2)?-1:1)*(w-1);
            const int y2=m.y+((o==0||o==3)?-1:1)*(h-1);
            const bool fits=x2>=0 && x2<5 && y2>=0 && y2<6;
            CHECK((f!=0)==fits);
            if (fits) { u32 n=0;for (u32 bit=0;bit<30;++bit) n+=(f>>bit)&1;CHECK(n==(u32)(a*b)); }
        }
        const u32 show=backpack::visible(modules);u32 used=0;
        CHECK((show>>30)==0);
        for (u32 i=0;i<30;++i) if (show&(1u<<i)) {
            u32 f=backpack::footprint(modules[i]);CHECK(f && !(used&f));used|=f;
        }
        backpack::Layout packed;backpack::pack(modules,packed);used=0;
        for(u32 i=0;i<30;++i)if(packed.shown&(1u<<i)) {
            auto m=modules[i];m.x=packed.x[i];m.y=packed.y[i];
            const u32 f=backpack::footprint(m);CHECK(f && !(used&f));used|=f;
        }
    }
    initialize_charm_table();g_ready=1;g_extraCharmMask=255;
    u8 slots[2]={7,7};
    for (u32 mask=0;mask<256;++mask) {
        g_charmMask=mask;const u8* array=virtual_charm_array(slots);u32 seen=0;
        for (u32 i=0;i<8;++i) if (array[i]) {
            CHECK(array[i]>=1 && array[i]<=8);u32 bit=1u<<(array[i]-1);CHECK(!(seen&bit));seen|=bit;
        }
        CHECK(seen==(mask|64));
    }
    g_ready=0;g_charmMask=255;CHECK(virtual_charm_array(slots)==g_charmTable[64]);g_ready=1;
    CHECK(virtual_charm_array(0)==g_charmTable[0]);
    slots[0]=255;slots[1]=0;CHECK(native_charm_mask(slots)==0);
    static u8 snapshot[0x490];snapshot[0x1FC]=1;snapshot[0x1FD]=7;
    snapshot[0x48C]=1;snapshot[0x48D]=7;
    CHECK(test_adapter((void*)&charm_snapshot_rax,snapshot,g_charmTable[255],0)==0);
    CHECK(test_adapter((void*)&charm_snapshot_rcx,snapshot,g_charmTable[255],1)==0);
    CHECK(test_adapter((void*)&charm_full_snapshot_rax,snapshot,g_charmTable[255],0)==0);
    CHECK(test_adapter((void*)&charm_crypto_rax,snapshot+0x1FC,g_charmTable[255],2)==0);
    CHECK(has_virtual_charm(snapshot,8)==1);
    CHECK(has_virtual_charm(snapshot,0)==0);
    CHECK(has_virtual_charm(0,8)==0);
    g_imageBase=(u8*)VirtualAlloc(0,EXPECTED_SIZE_OF_IMAGE,MEM_RESERVE_VALUE|MEM_COMMIT_VALUE,PAGE_EXECUTE_READWRITE_VALUE);
    CHECK(g_imageBase!=0);
    void* adapters[]={(void*)&charm_snapshot_rax,(void*)&charm_snapshot_rcx,
                      (void*)&charm_full_snapshot_rax,(void*)&charm_crypto_rax};
    for (u32 i=0;i<4;++i) {
        u8* relay=allocate_near_relay(g_imageBase+0x1000,(u8*)adapters[i]);
        CHECK(relay!=0);
        CHECK(test_adapter(relay,i==3?snapshot+0x1FC:snapshot,g_charmTable[255],i==1?1:(i==3?2:0))==0);
        VirtualFree(relay,0,MEM_RELEASE_VALUE);
    }
    mock(0x33B480,(void*)&fake_shadow);mock(0x22A8D0,(void*)&fake_render);
    mock(0xB3A860,(void*)&fake_position);
    mock(0x11FFA0,(void*)&fake_find_component);
    mock(0xA2F50,(void*)&fake_bone_clear);mock(0xA2E20,(void*)&fake_bone_reserve);
    mock(0x1528E90,(void*)&fake_menu_build);mock(0x151A5C0,(void*)&fake_native_set);
    mock(0xB393E0,(void*)&fake_commit);
    mock(0xB3E470,(void*)&fake_capacity);mock(0xB6FC20,(void*)&fake_enumerate);mock(0xB7E840,(void*)&fake_destroy);
    FlushInstructionCache(GetCurrentProcess(),g_imageBase,EXPECTED_SIZE_OF_IMAGE);
    static u8 manager[0x4600];
    *(u8**)(g_imageBase+MANAGER_POINTER_RVA)=manager;
    *(u8**)(manager+0x45B8)=test_info;
    *(u8**)(g_imageBase+0x623E540)=catalogue;g_charmCatalogue=catalogue;g_charmRefresh=GetTickCount64();
    *(float*)(g_imageBase+0x3461FEC)=5.f;
    g_shapeLookup=&fake_shape;*(s32*)(shape+0x24)=2;*(s32*)(shape+0x28)=2;
    for (u32 i=0;i<30;++i) {
        u8* c=test_info+i*0x120;*c=1;*(u32*)(c+4)=i+1;*(s32*)(c+8)=i%5;*(s32*)(c+12)=i/5;c[0x18]=1;
        *(u8**)(test_info+0x45B0+i*0x60)=c;*(u8**)(test_info+0x45F8+i*0x60)=entities[i];
        *(u8**)(entities[i]+0xD8)=models[i];*(s32*)(models[i]+0x50)=1;*(u8**)(models[i]+0x58)=nodes[i];
        *(u8**)(nodes[i]+8)=models[i];nodes[i][0x2C]=1;nodes[i][0x2D]=1;
        *(u8**)(models[i]+0xC8)=rendered+i;
    }
    copy_bytes(saved_info,test_info,sizeof(test_info));g_wrapperHits=100;
    for (u32 i=0;i<30;++i) module_visual_update(models[i],1,test_info,i*0x60);
    CHECK(shadowCalls==30);CHECK(bytes_equal(test_info,saved_info,sizeof(test_info)));
    u32 visibleCount=0;for (u32 i=0;i<30;++i) {visibleCount+=rendered[i];CHECK(nodes[i][0x2C]==1 && nodes[i][0x2D]==1);}
    CHECK(visibleCount==6 && rendered[0] && !rendered[6]);
    CHECK(positionCalls==6);
    module_visual_update(models[0],1,test_info,0);CHECK(positionCalls==6);
    test_info[0x18]=0;
    module_visual_update(models[6],1,test_info,6*0x60);CHECK(rendered[6]==1);
    nodes[6][0x2D]=0;module_visual_update(models[6],1,test_info,6*0x60);CHECK(rendered[6]==0);
    // Full-size bone reference scan is independent of the one-cell logic.
    for(u32 i=0;i<30;++i){*(s32*)boneStorage[i]=1;*(u8**)(g_imageBase+0x6284090+i*8)=boneStorage[i]+0x10;}
    BoneVector bones={};
    CHECK(visual_reference(&bones,test_info+0x120)==1 && bones.count==4);
    CHECK(bonePointers[0]==boneStorage[5]+0x10 && bonePointers[1]==boneStorage[0]+0x10);
    CHECK(bonePointers[2]==boneStorage[6]+0x10 && bonePointers[3]==boneStorage[1]+0x10);
    fake_bone_clear(&bones,bones.data,0);bones.data=0;
    for(u32 i=0;i<30;++i)CHECK(*(s32*)boneStorage[i]==1);
    static u8 preview[0x120];preview[0]=1;*(s32*)(preview+8)=4;*(s32*)(preview+12)=0;
    CHECK(visual_reference(&bones,preview)==0 && bones.count==0);
    *(s32*)(preview+8)=2;*(s32*)(preview+12)=3;
    CHECK(visual_reference(&bones,preview)==1 && bones.count==4);
    CHECK(bonePointers[0]==boneStorage[17]+0x10);
    fake_bone_clear(&bones,bones.data,0);bones.data=0;
    *(s32*)(shape+0x24)=5;*(s32*)(shape+0x28)=6;*(u16*)(preview+0x1A)=1;
    CHECK(visual_reference(&bones,preview)==1 && bones.count==1);
    fake_bone_clear(&bones,bones.data,0);bones.data=0;
    *(s32*)(shape+0x24)=2;*(s32*)(shape+0x28)=2;
    // Equipment-menu copy outside all three manager contexts: same mesh packing
    // and full-size bone positions, no logical changes, no stale preview access.
    *(u8**)(manager+0x45B8)=0;
    CHECK(visual_backpack_context(test_info)==-1);
    rendered[6]=1;module_visual_update(models[6],1,test_info,6*0x60);
    CHECK(rendered[6]==1); // unowned actors are untouched
    *(u8**)(g_imageBase+0x623EA00)=subspace;
    CHECK(equipment_preview_info()==0);
    *(u8**)(subspace+0x38)=previewOwner;
    CHECK(equipment_preview_info()==0);
    // Use an allocated component-shaped backing buffer for the weak +20 pointer.
    static u8 previewComponent[0x360+sizeof(test_info)];
    u8* cargoInfo=previewComponent+0x360;
    copy_bytes(cargoInfo,test_info,sizeof(test_info));
    // Rebase each native record pointer into the preview's own data.
    for(u32 i=0;i<30;++i)*(u8**)(cargoInfo+0x45B0+i*0x60)=cargoInfo+i*0x120;
    *(u8**)(equipmentMenu+0x2D8)=previewComponent+0x20;
    CHECK(local_backpack_context(cargoInfo)==-1 && visual_backpack_context(cargoInfo)==3);
    CHECK(visual_backpack_context(test_info)==-1);
    copy_bytes(saved_info,cargoInfo,sizeof(test_info));g_equipmentPreviewReported=true;
    for(u32 i=0;i<30;++i)module_visual_update(models[i],1,cargoInfo,i*0x60);
    CHECK(bytes_equal(saved_info,cargoInfo,sizeof(test_info)));
    CHECK(rendered[1]==1 && rendered[7]==0);
    CHECK(visual_reference(&bones,cargoInfo+0x120)==1 && bones.count==4);
    CHECK(bonePointers[0]==boneStorage[5]+0x10 && bonePointers[3]==boneStorage[1]+0x10);
    fake_bone_clear(&bones,bones.data,0);bones.data=0;
    // Resolve the current preview on each use, including teardown/replacement.
    // Fresh save: no backpack manager cache, but a live player and menu copy.
    *(u8**)(g_imageBase+MANAGER_POINTER_RVA)=0;
    CHECK(gameplay_backpack_info()==0);
    *(u8**)(g_imageBase+0x623DF40)=playerRegistry;
    CHECK(gameplay_backpack_info()==0);
    *(s32*)(playerRegistry+0x38)=2;*(u8***)(playerRegistry+0x40)=playerEntries;
    CHECK(gameplay_backpack_info()==0);
    *(u8**)(playerEntry+0x48)=playerActor;
    CHECK(gameplay_backpack_info()==0);
    *(u8**)(playerEquipment+0x58)=playerOwner;
    u8* liveInfo=livePlayerComponent+0x360;
    copy_bytes(liveInfo,test_info,sizeof(test_info));
    for(u32 i=0;i<30;++i)*(u8**)(liveInfo+0x45B0+i*0x60)=liveInfo+i*0x120;
    CHECK(visual_backpack_context(liveInfo)==4 && visual_backpack_context(cargoInfo)==3);
    CHECK(visual_backpack_context(test_info)==-1);
    // Distinct layouts must survive alternating gameplay/menu renders.
    cargoInfo[0x120+0x18]=0;g_gameplayBackpackReported=true;
    copy_bytes(saved_info,liveInfo,sizeof(test_info));
    for(u32 repeat=0;repeat<3;++repeat) {
        module_visual_update(models[7],1,cargoInfo,7*0x60);CHECK(rendered[7]==1);
        module_visual_update(models[7],1,liveInfo,7*0x60);CHECK(rendered[7]==0);
        CHECK(visual_reference(&bones,liveInfo+0x120)==1 && bones.count==4);
        CHECK(bonePointers[0]==boneStorage[5]+0x10 && bonePointers[3]==boneStorage[1]+0x10);
        fake_bone_clear(&bones,bones.data,0);bones.data=0;
    }
    CHECK(bytes_equal(saved_info,liveInfo,sizeof(test_info)));
    playerEquipmentPresent=false;CHECK(gameplay_backpack_info()==0);playerEquipmentPresent=true;
    playerBackpackPresent=false;CHECK(gameplay_backpack_info()==0);playerBackpackPresent=true;
    *(u32*)(playerEntry+0x70)=1;CHECK(gameplay_backpack_info()==0);*(u32*)(playerEntry+0x70)=0;
    *(s32*)(playerRegistry+0x38)=-1;CHECK(gameplay_backpack_info()==0);
    *(s32*)(playerRegistry+0x38)=1025;CHECK(gameplay_backpack_info()==0);
    *(s32*)(playerRegistry+0x38)=2;
    *(u8**)(playerEquipment+0x58)=0;CHECK(gameplay_backpack_info()==0);
    CHECK(visual_backpack_context(liveInfo)==-1);
    *(u8**)(g_imageBase+0x623DF40)=0;
    *(u8**)(g_imageBase+MANAGER_POINTER_RVA)=manager;
    cargoInfo[0x120+0x18]=1;
    equipmentPresent=false;CHECK(visual_backpack_context(cargoInfo)==-1);
    rendered[7]=1;module_visual_update(models[7],1,cargoInfo,7*0x60);CHECK(rendered[7]==1);
    equipmentPresent=true;
    *(u8**)(equipmentMenu+0x2D8)=0;CHECK(visual_backpack_context(cargoInfo)==-1);
    *(u8**)(subspace+0x38)=0;CHECK(equipment_preview_info()==0);
    *(u8**)(g_imageBase+0x623EA00)=0;CHECK(equipment_preview_info()==0);
    *(u8**)(manager+0x45B8)=test_info;
    for (u32 n=0;n<3;++n) {
        test_info[0x21C0]=(n>=1)?2:1;test_info[0x21C1]=(n==2)?2:7;
        CHECK(battery_capacity(test_info)==105.f);
    }
    test_info[0x56F4]=1;CHECK(battery_capacity(test_info)==0);test_info[0x56F4]=0;
    g_charmMask=0;test_info[0x21C0]=1;test_info[0x21C1]=7;CHECK(battery_capacity(test_info)==100.f);
    // Native ownership enumeration with duplicate records must deduplicate.
    *(u8***)(catalogue+0x10)=definitions;
    const u32 unlocked[4]={1,3,6,6};
    for (u32 i=0;i<9;++i) {definitions[i]=items[i];*(u32*)(items[i]+0x20)=0x12340000+i;
        g_imageBase[0x2DD37D0+i*8]=(u8)i;*(u32*)(g_imageBase+0x2DD37D4+i*8)=0x12340000+i;}
    for (u32 i=0;i<4;++i) *(u32*)(entries[i]+8)=0x8000|unlocked[i];
    g_charmRefresh=0;refresh_charms();CHECK(g_charmMask==0x25 && destroyCalls==1);
    // Menu equipment: eight real rows, reversed native slot mapping, duplicate
    // exclusion, replacing/removing one slot, and preview cancellation.
    *(s32*)(catalogue+8)=9;g_charmMask=255;g_charmRefresh=GetTickCount64();
    const u8 extras[6]={2,3,4,5,6,8};copy_bytes(g_extraCharms,extras,6);update_extra_mask();
    manager[0x4498]=1;manager[0x4499]=7;
    u8 nativeEntries[2][0x38]={};nativeEntries[0][1]=7;nativeEntries[1][1]=1;
    MenuVector nativeMenu={2,2,nativeEntries[0]};
    build_charm_category(controller+0x20,1,&nativeMenu);CHECK(menuBuildCount==8);
    CHECK(builtEntries[0][1]==8 && builtEntries[5][1]==2 && builtEntries[6][1]==7 && builtEntries[7][1]==1);
    for(s32 row=1;row<=8;++row)CHECK(menu_charm_index(controller,row)==8-row);
    CHECK(menu_charm_index(controller,0)==-1 && menu_charm_index(controller,9)==-1);
    *(u32*)(controller+0x2704)=7;
    CHECK(!charm_already_equipped(controller,8) && charm_already_equipped(controller,2));
    CHECK(test_menu_read((void*)&charm_menu_read,manager,7,8)==0);
    CHECK(test_menu_read((void*)&charm_menu_read,manager,0,1)==0);
    CHECK(test_menu_read((void*)&charm_menu_read,manager,99,0)==0);
    set_menu_charm(controller,0);CHECK(g_extraCharms[5]==0 && !(g_extraCharmMask&128));
    set_menu_charm(controller,8);CHECK(g_extraCharms[5]==8); // cancel restores old value
    menuRows[1][0x16]=1;remove_menu_charms(test_info,manager+0x22D8);
    CHECK(g_extraCharms[5]==0 && g_extraCharms[4]==6 && commitCount==1);
    menuRows[1][0x16]=0;
    *(u32*)(controller+0x2704)=1;set_menu_charm(controller,4);CHECK(nativeSetCount==1);
    // Available but unequipped charms must no longer activate themselves.
    for(u32 i=0;i<6;++i)g_extraCharms[i]=0;update_extra_mask();
    CHECK(virtual_charm_array(snapshot+0x1FC)==g_charmTable[65]);
    g_extraCharmMask=255;
    // Exercise real installation bytes, scoped module context and rollback.
    CHECK(!prepare_hooks((HANDLE)(s64)-1));
    copy_bytes(g_imageBase+0x1D14C00,NATIVE_CRYPTO_LOOP,sizeof(NATIVE_CRYPTO_LOOP));
    // Replace only the two exits with observable return values; retain every
    // instruction and branch of the native loop, including its back edge.
    const u8 absentExit[]={0x89,0xC8,0xC3}; // return ECX (8 after all misses)
    const u8 presentExit[]={0x8D,0x81,0x00,0x01,0x00,0x00,0xC3}; // 256+ECX
    copy_bytes(g_imageBase+0x1D14C26,absentExit,sizeof(absentExit));
    copy_bytes(g_imageBase+0x1D14C37,presentExit,sizeof(presentExit));
    for (u32 i=0;i<HOOK_COUNT;++i) {
        Hook& h=HOOKS[i];u8 call[5];build_call_bytes(call,g_imageBase+h.rva,g_imageBase+h.target);
        if (h.tail) call[0]=0xE9;
        copy_bytes(g_imageBase+h.rva,call,5);
    }
    for (u32 i=0;i<INLINE_COUNT;++i) copy_bytes(g_imageBase+INLINE_HOOKS[i].rva,INLINE_HOOKS[i].original,INLINE_HOOKS[i].length);
    for (u32 i=0;i<PATCH_COUNT;++i) copy_bytes(g_imageBase+PATCHES[i].rva,PATCHES[i].original,PATCHES[i].length);
    CHECK(prepare_hooks((HANDLE)(s64)-1));CHECK(install_hooks());
    CHECK(apply_footprint_patches((HANDLE)(s64)-1));
    // Execute the installed eight-byte menu hook and the native tail-jump site.
    g_imageBase[0x152374A]=0xC3;
    FlushInstructionCache(GetCurrentProcess(),g_imageBase+0x1523742,9);
    CHECK(test_menu_read(g_imageBase+0x1523742,manager,0,1)==0);
    *(u32*)(controller+0x2704)=7;
    ((void (FASTCALL *)(u8*,u8))(g_imageBase+0x15272AC))(controller,8);
    CHECK(g_extraCharms[5]==8);
    ((void (FASTCALL *)(u8*,u8))(g_imageBase+0x15272AC))(controller,0);
    CHECK(g_extraCharms[5]==0);
    g_extraCharmMask=255;
    CHECK(bytes_equal(g_imageBase+0x1D14C17,NATIVE_CRYPTO_LOOP+0x17,3));
    FlushInstructionCache(GetCurrentProcess(),g_imageBase+0x1D14C00,0x40);
    static u8 component[0x2502];
    void* loop=g_imageBase+0x1D14C00;
    for (u32 mask=0;mask<256;++mask) for (u32 native=0;native<3;++native) {
        g_charmMask=mask;
        component[0x2500]=native==1?3:1;component[0x2501]=native==2?3:7;
        CHECK(test_crypto_loop(loop,component)==((mask&4)||native?0x102u:8u));
    }
    g_charmMask=255;CHECK(test_crypto_loop(loop,0)==8);
    g_ready=0;component[0x2500]=1;component[0x2501]=7;
    CHECK(test_crypto_loop(loop,component)==8);
    component[0x2501]=3;CHECK(test_crypto_loop(loop,component)==0x102);
    g_ready=1;
    rendered[0]=1;u32 oldShadow=shadowCalls;
    test_module_relay(HOOKS[4].relay,models[0],1,test_info,0);
    CHECK(!rendered[0] && shadowCalls==oldShadow+1);
    CHECK(restore_all());
    for (u32 i=0;i<HOOK_COUNT;++i) CHECK(bytes_equal(g_imageBase+HOOKS[i].rva,HOOKS[i].original,5));
    for (u32 i=0;i<INLINE_COUNT;++i) CHECK(bytes_equal(g_imageBase+INLINE_HOOKS[i].rva,INLINE_HOOKS[i].original,INLINE_HOOKS[i].length));
    for (u32 i=0;i<PATCH_COUNT;++i) CHECK(bytes_equal(g_imageBase+PATCHES[i].rva,PATCHES[i].original,PATCHES[i].length));
    VirtualFree(g_imageBase,0,MEM_RELEASE_VALUE);g_imageBase=0;
    return 0;
}

extern "C" __declspec(dllexport) int test_state_file(const wchar_t* path) {
    u32 i=0;for(;i+1<MAX_PATH_CHARS && path[i];++i)g_charmStatePath[i]=path[i];
    CHECK(!path[i]);g_charmStatePath[i]=0;
    wchar_t valid[16]=L"234568";decode_extra_charms(valid);
    CHECK(g_extraCharmMask==190);g_extraDirty=true;save_extra_charms();CHECK(!g_extraDirty);
    wchar_t loaded[16]={};
    CHECK(GetPrivateProfileStringW(L"Equipment",L"InvisibleSlots",L"",loaded,16,g_charmStatePath)==6);
    for(i=0;i<6;++i)CHECK(loaded[i]==valid[i]);
    wchar_t invalid[16]=L"2239x8";decode_extra_charms(invalid);
    CHECK(g_extraCharms[0]==2 && g_extraCharms[1]==0 && g_extraCharms[2]==3 && g_extraCharms[3]==0 && g_extraCharms[4]==0 && g_extraCharms[5]==8);
    wchar_t empty[16]={};decode_extra_charms(empty);CHECK(g_extraCharmMask==0);
    return 0;
}

// Real DS2 category-builder machine code, supplied from the hash-checked EXE.
// Only its row allocator is substituted; all category/count/row logic executes.
extern "C" __declspec(dllexport) int test_native_menu(const u8* code,u32 size) {
    CHECK(size==0x5A0);
    g_imageBase=(u8*)VirtualAlloc(0,EXPECTED_SIZE_OF_IMAGE,MEM_RESERVE_VALUE|MEM_COMMIT_VALUE,PAGE_EXECUTE_READWRITE_VALUE);
    CHECK(g_imageBase!=0);g_ready=0;g_charmMask=255;
    copy_bytes(g_imageBase+0x1528E90,code,size);g_imageBase[0x1528EDC]=8;
    mock(0x110470,(void*)&fake_menu_reserve);
    *(u32*)(g_imageBase+0x34613AC)=0x3F800000;
    *(u8**)(g_imageBase+0x623E540)=catalogue;
    *(s32*)(catalogue+8)=9;*(u8***)(catalogue+0x10)=definitions;
    for(u32 i=0;i<9;++i){definitions[i]=items[i];*(u32*)(items[i]+0x20)=0x12340000+i;*(u32*)(g_imageBase+0x2DD37D4+i*8)=0x12340000+i;}
    FlushInstructionCache(GetCurrentProcess(),g_imageBase,EXPECTED_SIZE_OF_IMAGE);
    for(u32 mask=0;mask<256;++mask) {
        // The two physical slots use 1/2; six extra slots use 3..8.
        u8 physical[2][0x38]={};physical[0][1]=(mask&2)?2:0;physical[1][1]=(mask&1)?1:0;
        for(u32 i=0;i<6;++i)g_extraCharms[i]=(mask&(1u<<(i+2)))?(u8)(i+3):0;
        MenuVector in={2,2,physical[0]};*(s32*)(controller+0x228)=0;
        build_charm_category(controller+0x20,1,&in);
        CHECK(*(s32*)(controller+0x228)==9 && *(u32*)menuRows[0]==0 && menuRows[0][5]==1);
        u32 equipped=0;
        for(u32 i=0;i<8;++i) {
            const u8 id=(mask&(1u<<(7-i)))?(u8)(8-i):0;
            CHECK(menuRows[i+1][5]==1 && menuRows[i+1][0x30]==id);
            CHECK(*(u32*)menuRows[i+1]==(id?1u:2u));
            equipped+=id?1:0;
        }
        CHECK(*(u32*)(menuRows[0]+0x10)==equipped);
    }
    VirtualFree(g_imageBase,0,MEM_RELEASE_VALUE);g_imageBase=0;
    return 0;
}
