extern "C" void* get_peb();
extern "C" void HookEntry();
extern "C" int _fltused = 0;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using usize = unsigned long long;
using BOOL = int;
using DWORD = unsigned long;
using HANDLE = void*;
using HMODULE = void*;

extern "C" {
__declspec(dllexport) u64 g_deliveryReturn = 0;
__declspec(dllexport) u32 g_enabledMask = 0x1F;
__declspec(dllexport) double g_multiplier = 2.0;
__declspec(dllexport) double g_half = 0.5;
__declspec(dllexport) u64 g_extraPoints = 0;
__declspec(dllexport) u32 g_mode = 0; // 0 multiplier, 1 additive, 2 both
__declspec(dllexport) void* g_trampoline = nullptr;
}

static int g_enabled = 1;
static char g_iniBuf[8192];
static char g_pathBuf[1024];

struct LIST_ENTRY { LIST_ENTRY* Flink; LIST_ENTRY* Blink; };
struct UNICODE_STRING { u16 Length; u16 MaximumLength; wchar_t* Buffer; };

static u8 lower8(u8 c) { return (c >= 'A' && c <= 'Z') ? (u8)(c + 32) : c; }
static wchar_t lowerw(wchar_t c) { return (c >= L'A' && c <= L'Z') ? (wchar_t)(c + 32) : c; }

static bool wends_eq(const wchar_t* s, u16 byteLen, const wchar_t* lit) {
    if (!s || !lit) return false;
    usize n = byteLen / 2;
    usize m = 0; while (lit[m]) ++m;
    if (n != m) return false;
    for (usize i=0;i<n;++i) if (lowerw(s[i]) != lowerw(lit[i])) return false;
    return true;
}

static void* find_module(const wchar_t* name) {
    u8* peb = (u8*)get_peb();
    if (!peb) return nullptr;
    u8* ldr = *(u8**)(peb + 0x18);
    if (!ldr) return nullptr;
    LIST_ENTRY* head = (LIST_ENTRY*)(ldr + 0x20);
    for (LIST_ENTRY* n=head->Flink; n && n!=head; n=n->Flink) {
        u8* e = (u8*)n - 0x10;
        UNICODE_STRING* baseName = (UNICODE_STRING*)(e + 0x58);
        if (wends_eq(baseName->Buffer, baseName->Length, name)) return *(void**)(e + 0x30);
    }
    return nullptr;
}

static int streqi(const char* a, const char* b) {
    while (*a && *b) { if (lower8((u8)*a) != lower8((u8)*b)) return 0; ++a; ++b; }
    return *a == *b;
}

static usize cstrlen(const char* s) { usize n=0; while (s && s[n]) ++n; return n; }

static void* resolve_export_recursive(void* module, const char* wanted, int depth) {
    if (!module || !wanted || depth>4) return nullptr;
    u8* b=(u8*)module;
    u32 peoff=*(u32*)(b+0x3C);
    u8* nt=b+peoff;
    if (*(u32*)nt != 0x00004550) return nullptr;
    u32 expRva=*(u32*)(nt+0x88);
    u32 expSize=*(u32*)(nt+0x8C);
    if (!expRva) return nullptr;
    u8* e=b+expRva;
    u32 nNames=*(u32*)(e+0x18);
    u32 funcsRva=*(u32*)(e+0x1C);
    u32 namesRva=*(u32*)(e+0x20);
    u32 ordRva=*(u32*)(e+0x24);
    u32* names=(u32*)(b+namesRva);
    u16* ords=(u16*)(b+ordRva);
    u32* funcs=(u32*)(b+funcsRva);
    for (u32 i=0;i<nNames;++i) {
        const char* nm=(const char*)(b+names[i]);
        if (!streqi(nm,wanted)) continue;
        u32 rva=funcs[ords[i]];
        if (!(rva >= expRva && rva < expRva + expSize)) return b+rva;

        const char* fw=(const char*)(b+rva);
        char mod[96]; char fn[128]; usize mi=0,fi=0;
        while (*fw && *fw!='.' && mi<88) mod[mi++]=*fw++;
        if (*fw!='.') return nullptr;
        ++fw;
        while (*fw && fi<120) fn[fi++]=*fw++;
        mod[mi]=0; fn[fi]=0;
        if (fn[0]=='#') return nullptr;
        const char dll[]=".dll";
        if (mi+4<95) { for (int j=0;j<5;++j) mod[mi+j]=dll[j]; }
        wchar_t wmod[96]; usize wn=0;
        while (mod[wn] && wn<95) { wmod[wn]=(wchar_t)(u8)mod[wn]; ++wn; }
        wmod[wn]=0;
        void* next=find_module(wmod);
        if (!next) next=find_module(L"kernelbase.dll");
        return resolve_export_recursive(next,fn,depth+1);
    }
    return nullptr;
}

static void* resolve_export(void* module, const char* wanted) {
    return resolve_export_recursive(module,wanted,0);
}

using VirtualProtect_t = BOOL(*)(void*, usize, DWORD, DWORD*);
using VirtualAlloc_t = void*(*)(void*, usize, DWORD, DWORD);
using FlushInstructionCache_t = BOOL(*)(HANDLE, const void*, usize);
using GetModuleFileNameA_t = DWORD(*)(HMODULE, char*, DWORD);
using CreateFileA_t = HANDLE(*)(const char*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
using ReadFile_t = BOOL(*)(HANDLE, void*, DWORD, DWORD*, void*);
using CloseHandle_t = BOOL(*)(HANDLE);

static VirtualProtect_t pVirtualProtect;
static VirtualAlloc_t pVirtualAlloc;
static FlushInstructionCache_t pFlushInstructionCache;
static GetModuleFileNameA_t pGetModuleFileNameA;
static CreateFileA_t pCreateFileA;
static ReadFile_t pReadFile;
static CloseHandle_t pCloseHandle;

static bool resolve_api() {
    void* k32=find_module(L"kernel32.dll");
    void* kb=find_module(L"kernelbase.dll");
    void* root=k32?k32:kb;
    if (!root) return false;
    pVirtualProtect=(VirtualProtect_t)resolve_export(root,"VirtualProtect");
    pVirtualAlloc=(VirtualAlloc_t)resolve_export(root,"VirtualAlloc");
    pFlushInstructionCache=(FlushInstructionCache_t)resolve_export(root,"FlushInstructionCache");
    pGetModuleFileNameA=(GetModuleFileNameA_t)resolve_export(root,"GetModuleFileNameA");
    pCreateFileA=(CreateFileA_t)resolve_export(root,"CreateFileA");
    pReadFile=(ReadFile_t)resolve_export(root,"ReadFile");
    pCloseHandle=(CloseHandle_t)resolve_export(root,"CloseHandle");
    return pVirtualProtect && pVirtualAlloc && pFlushInstructionCache;
}

static char* trim(char* s) {
    while (*s==' ' || *s=='\t' || *s=='\r') ++s;
    char* e=s; while (*e && *e!='\n') ++e;
    while (e>s && (e[-1]==' ' || e[-1]=='\t' || e[-1]=='\r')) --e;
    *e=0; return s;
}
static char* find_char(char* s, char c) { while (*s) { if (*s==c) return s; ++s; } return nullptr; }
static int parse_bool(const char* s, int defv) {
    if (streqi(s,"1")||streqi(s,"true")||streqi(s,"yes")||streqi(s,"on")) return 1;
    if (streqi(s,"0")||streqi(s,"false")||streqi(s,"no")||streqi(s,"off")) return 0;
    return defv;
}
static u64 parse_u64(const char* s, u64 defv) {
    u64 v=0; int any=0;
    while (*s==' '||*s=='\t') ++s;
    while (*s>='0'&&*s<='9') { any=1; u64 d=(u64)(*s-'0'); if (v>1000000000000000000ULL) return defv; v=v*10+d; ++s; }
    return any?v:defv;
}
static double parse_double(const char* s, double defv) {
    while (*s==' '||*s=='\t') ++s;
    double v=0.0; int any=0;
    while (*s>='0'&&*s<='9') { any=1; v=v*10.0+(double)(*s-'0'); ++s; }
    if (*s=='.'||*s==',') { ++s; double place=0.1; while (*s>='0'&&*s<='9') { any=1; v += (double)(*s-'0')*place; place*=0.1; ++s; } }
    if (!any || v<0.0 || v>1000.0) return defv;
    return v;
}

static void set_category_bit(u32 bit, int on) { if (on) g_enabledMask |= (1u<<bit); else g_enabledMask &= ~(1u<<bit); }

static void parse_ini(char* buf) {
    char* p=buf;
    while (*p) {
        char* line=p;
        while (*p && *p!='\n') ++p;
        if (*p=='\n') { *p=0; ++p; }
        line=trim(line);
        if (!*line || *line==';' || *line=='#' || *line=='[') continue;
        char* eq=find_char(line,'='); if (!eq) continue;
        *eq=0; char* key=trim(line); char* val=trim(eq+1);
        if (streqi(key,"Enabled")) g_enabled=parse_bool(val,g_enabled);
        else if (streqi(key,"Mode")) { if(streqi(val,"Additive")) g_mode=1; else if(streqi(val,"Both")) g_mode=2; else g_mode=0; }
        else if (streqi(key,"Multiplier")) g_multiplier=parse_double(val,g_multiplier);
        else if (streqi(key,"ExtraPoints")) g_extraPoints=parse_u64(val,g_extraPoints);
        else if (streqi(key,"Porter")) set_category_bit(0,parse_bool(val,1));
        else if (streqi(key,"Combat")) set_category_bit(1,parse_bool(val,1));
        else if (streqi(key,"Stealth")) set_category_bit(2,parse_bool(val,1));
        else if (streqi(key,"Service")) set_category_bit(3,parse_bool(val,1));
        else if (streqi(key,"BridgeLink")) set_category_bit(4,parse_bool(val,1));
    }
}

static void load_ini(HMODULE self) {
    if (!pGetModuleFileNameA || !pCreateFileA || !pReadFile || !pCloseHandle) return;
    if (!pGetModuleFileNameA(self,g_pathBuf,1000)) return;
    int len=0; while (g_pathBuf[len] && len<1000) ++len;
    int dot=-1; for (int i=len-1;i>=0;--i) { if (g_pathBuf[i]=='.') { dot=i; break; } if (g_pathBuf[i]=='\\'||g_pathBuf[i]=='/') break; }
    if (dot<0) dot=len;
    const char ext[]=".ini";
    for (int i=0;i<5 && dot+i<1023;++i) g_pathBuf[dot+i]=ext[i];
    g_pathBuf[dot+4]=0;
    HANDLE h=pCreateFileA(g_pathBuf,0x80000000u,1u,nullptr,3u,0x80u,nullptr);
    if ((usize)h == ~(usize)0) return;
    DWORD got=0;
    if (pReadFile(h,g_iniBuf,8191,&got,nullptr) && got<8192) { g_iniBuf[got]=0; parse_ini(g_iniBuf); }
    pCloseHandle(h);
}

static usize image_size(void* base) {
    u8* b=(u8*)base; u32 peoff=*(u32*)(b+0x3C); u8* nt=b+peoff; return (usize)*(u32*)(nt+0x50);
}

static bool match_at(const u8* p, const u8* pat, const char* mask, usize n) {
    for (usize i=0;i<n;++i) if (mask[i]=='x' && p[i]!=pat[i]) return false; return true;
}

static u8* find_mutator(void* exeBase) {
    static const u8 pat[]={0x48,0x89,0x6C,0x24,0x18,0x56,0x57,0x41,0x56,0x48,0x83,0xEC,0x20,0x48,0x8B,0x35,0,0,0,0,0x4D,0x8B,0xF1,0x0F,0xB7,0xFA,0x49,0x8B,0xE8,0x83,0xFF,0x09};
    static const char mask[]="xxxxxxxxxxxxxxxx????xxxxxxxxxxxx";
    const usize n=sizeof(pat);
    u8* b=(u8*)exeBase; usize sz=image_size(exeBase); u8* found=nullptr;
    if (sz<n) return nullptr;
    for (usize i=0;i+n<=sz;++i) {
        if (match_at(b+i,pat,mask,n)) { if (found) return nullptr; found=b+i; }
    }
    return found;
}

static bool install_hook(void* exeBase) {
    u8* target=find_mutator(exeBase); if (!target) return false;
    const usize stolen=13;
    u8* tramp=(u8*)pVirtualAlloc(nullptr,64,0x3000u,0x40u); if (!tramp) return false;
    for (usize i=0;i<stolen;++i) tramp[i]=target[i];
    u8* q=tramp+stolen;
    q[0]=0x48; q[1]=0xB8; *(u64*)(q+2)=(u64)(target+stolen); q[10]=0xFF; q[11]=0xE0;
    g_trampoline=tramp;
    g_deliveryReturn=(u64)((u8*)exeBase+0xC6BF82ULL);
    DWORD oldp=0;
    if (!pVirtualProtect(target,stolen,0x40u,&oldp)) return false;
    u8 patch[13];
    patch[0]=0x48; patch[1]=0xB8; *(u64*)(patch+2)=(u64)&HookEntry; patch[10]=0xFF; patch[11]=0xE0; patch[12]=0x90;
    for (usize i=0;i<stolen;++i) target[i]=patch[i];
    pFlushInstructionCache((HANDLE)(usize)-1,target,stolen);
    DWORD dummy=0; pVirtualProtect(target,stolen,oldp,&dummy);
    return true;
}

extern "C" BOOL DllMain(HMODULE self, DWORD reason, void*) {
    if (reason!=1) return 1;
    if (!resolve_api()) return 1;
    load_ini(self);
    if (!g_enabled) return 1;
    void* exe=find_module(L"ds2.exe");
    if (!exe) return 1;
    install_hook(exe);
    return 1;
}
