// Six extra equipment slots live outside the two-byte native save structure.
extern "C" __declspec(dllimport) DWORD WINAPI GetPrivateProfileStringW(const wchar_t*,const wchar_t*,const wchar_t*,wchar_t*,DWORD,const wchar_t*);
extern "C" __declspec(dllimport) BOOL WINAPI WritePrivateProfileStringW(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
static wchar_t g_charmStatePath[MAX_PATH_CHARS];
static u8* g_menuController=0;
static bool g_extraDirty=false;
static void update_extra_mask() {
    u32 mask=0;
    for (u32 i=0;i<6;++i) if (g_extraCharms[i]>=1 && g_extraCharms[i]<=8) mask|=1u<<(g_extraCharms[i]-1);
    _InterlockedExchange(&g_extraCharmMask,(long)mask);
}
static void decode_extra_charms(const wchar_t slots[16]) {
    u32 used=0;
    for (u32 i=0;i<6;++i) {
        const u32 id=(u32)(slots[i]-L'0');
        if (id>=1 && id<=8 && !(used&(1u<<(id-1)))) {g_extraCharms[i]=(u8)id;used|=1u<<(id-1);}
        else g_extraCharms[i]=0;
    }
    update_extra_mask();
}
static void load_extra_charms() {
    DWORD n=GetModuleFileNameW(0,g_charmStatePath,MAX_PATH_CHARS);
    if (!n || n>=MAX_PATH_CHARS) { g_charmStatePath[0]=0; return; }
    u32 slash=0;
    for (u32 i=0;i<n;++i) if (g_charmStatePath[i]==L'\\' || g_charmStatePath[i]==L'/') slash=i+1;
    const wchar_t name[]=L"DS2_HighDensityBackpackModules.charms.ini";
    if (slash+sizeof(name)/sizeof(wchar_t)>MAX_PATH_CHARS) { g_charmStatePath[0]=0; return; }
    for (u32 i=0;i<sizeof(name)/sizeof(wchar_t);++i) g_charmStatePath[slash+i]=name[i];
    wchar_t slots[16]={};
    GetPrivateProfileStringW(L"Equipment",L"InvisibleSlots",L"000000",slots,16,g_charmStatePath);
    decode_extra_charms(slots);
}
static void save_extra_charms() {
    if (!g_extraDirty || !g_charmStatePath[0]) return;
    wchar_t slots[7];
    for (u32 i=0;i<6;++i) slots[i]=(wchar_t)(L'0'+g_extraCharms[i]);
    slots[6]=0;
    if (WritePrivateProfileStringW(L"Equipment",L"InvisibleSlots",slots,g_charmStatePath)) g_extraDirty=false;
    else {
        HANDLE log=open_log_append();write_text(log,"EXTRA_CHARM_SAVE_FAILED\r\n");
        if (log!=(HANDLE)(s64)-1) CloseHandle(log);
    }
}
extern "C" u32 menu_charm_value(u8* manager,u32 slot) {
    if (slot<2) return manager?manager[0x4498+slot]:0;
    if (slot>=8) return 0;
    const u8 id=g_extraCharms[slot-2];
    return id>=1 && id<=8 && (g_charmMask&(1u<<(id-1)))?id:0;
}
static u8* charm_definition(u8 id) {
    if (id<1 || id>8) return 0;
    u8* catalogue=*(u8**)(g_imageBase+0x623E540);
    if (!catalogue) return 0;
    const s32 n=*(s32*)(catalogue+8);
    u8** definitions=*(u8***)(catalogue+0x10);
    if (!definitions || n<0 || n>65536) return 0;
    const u32 hash=*(u32*)(g_imageBase+0x2DD37D4+id*8);
    for (s32 i=0;i<n;++i) if (definitions[i] && *(u32*)(definitions[i]+0x20)==hash) return definitions[i];
    return 0;
}
struct MenuVector {s32 count,capacity;u8* data;};
static void FASTCALL build_charm_category(u8* menu,u8 category,MenuVector* native) {
    using Build=void (FASTCALL *)(u8*,u8,MenuVector*);
    Build build=(Build)(g_imageBase+0x1528E90);
    if (category!=1 || !native || native->count!=2 || !native->data) {build(menu,category,native);return;}
    refresh_charms();
    if (g_menuController!=menu-0x20) {
        HANDLE log=open_log_append();
        write_text(log,"ACCESSORY_MENU_SLOTS=8; last_two_rows_visible; first_six_rows_invisible\r\n");
        if (log!=(HANDLE)(s64)-1) CloseHandle(log);
    }
    g_menuController=menu-0x20;
    u8 entries[8][0x38];
    // The native list indexes slots in reverse order: header + count - row.
    // Last two rows are the original visible slots (1, 0).
    for (u32 i=0;i<6;++i) {
        copy_bytes(entries[i],native->data,0x38);
        const u8 id=(u8)menu_charm_value(0,7-i);
        u8* definition=charm_definition(id);
        entries[i][1]=definition?id:0;
        *(u8**)(entries[i]+8)=definition;
        entries[i][0x24]=0;
    }
    copy_bytes(entries[6],native->data,0x70);
    MenuVector extended={8,8,&entries[0][0]};
    build(menu,category,&extended);
}
static s32 charm_header(u8* controller) {
    if (!controller) return -1;
    const s32 count=*(s32*)(controller+0x228);
    u8* rows=*(u8**)(controller+0x230);
    if (!rows || count<0 || count>256) return -1;
    for (s32 i=0;i<count;++i) if (*(u32*)(rows+i*0x48)==0 && rows[i*0x48+5]==1) return i;
    return -1;
}
static s32 FASTCALL menu_charm_index(u8* controller,s32 row) {
    const s32 header=charm_header(controller);
    const s32 index=header+8-row;
    return header>=0 && row>header && index>=0 && index<8?index:-1;
}
static u8 FASTCALL charm_already_equipped(u8* controller,u8 id) {
    if (id<1 || id>8) return 0;
    const s32 header=charm_header(controller);
    if (header<0) return 0;
    const s32 count=*(s32*)(controller+0x228);
    u8* rows=*(u8**)(controller+0x230);
    const u32 selected=*(u32*)(controller+0x2704);
    for (s32 i=header+1;i<count && i<=header+8;++i)
        if (rows[i*0x48+5]==1 && rows[i*0x48+0x30]==id && (u32)(header+8-i)!=selected) return 1;
    return 0;
}
static void set_extra_slot(u32 slot,u8 id) {
    if (slot<2 || slot>=8) return;
    if (id>8) id=0;
    if (g_extraCharms[slot-2]==id) return;
    g_extraCharms[slot-2]=id;g_extraDirty=true;update_extra_mask();
}
static void FASTCALL set_menu_charm(u8* controller,u8 id) {
    const u32 slot=*(u32*)(controller+0x2704);
    if (slot<2) {
        using Set=void (FASTCALL *)(u8*,u8);
        ((Set)(g_imageBase+0x151A5C0))(controller,id);
    } else set_extra_slot(slot,id);
}
static void FASTCALL remove_menu_charms(u8* info,u8* staged) {
    const s32 header=charm_header(g_menuController);
    if (header>=0) {
        const s32 count=*(s32*)(g_menuController+0x228);
        u8* rows=*(u8**)(g_menuController+0x230);
        for (s32 i=header+1;i<count && i<=header+8;++i)
            if (rows[i*0x48+5]==1 && rows[i*0x48+0x16]) set_extra_slot((u32)(header+8-i),0);
    }
    using Commit=void (FASTCALL *)(u8*,u8*);
    ((Commit)(g_imageBase+0xB393E0))(info,staged);
}
static void FASTCALL close_charm_menu() {
    save_extra_charms();g_menuController=0;
    using Close=void (FASTCALL *)();
    ((Close)(g_imageBase+0xB41A60))();
}
extern "C" void charm_menu_read();
