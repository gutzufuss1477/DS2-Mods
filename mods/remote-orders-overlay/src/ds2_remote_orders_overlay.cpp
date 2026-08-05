// DS2 Remote Orders Overlay v0.2.0
// Passive world-map order reader for the exact DS2.exe build documented below.
//
// Confirmed bridge used by this mod:
// SelectedEntry(type 5/6) -> EntityManager physical table -> facility content object
// -> facility ID at +0x160 -> global MissionSource -> native mission eligibility
// predicate with the selected facility ID -> native mission display formatter.
//
// The mod does not accept orders, alter saves, open the terminal lifecycle, use
// hotkeys, play sounds, attach a debugger, or write DS2 gameplay state.

extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned long long usize;
typedef long long isize;
typedef unsigned long DWORD;
typedef signed long LONG;
typedef unsigned long ULONG_PTR;
typedef long long LONG_PTR;
typedef unsigned long long UINT_PTR;
typedef unsigned int UINT;
typedef unsigned short ATOM;
typedef long BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef wchar_t WCHAR;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HHOOK;
typedef void* HBRUSH;
typedef void* HFONT;
typedef void* HGDIOBJ;
typedef void* HDC;
typedef void* HCURSOR;
typedef void* HICON;
typedef void* HMENU;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef const char* LPCSTR;
typedef const WCHAR* LPCWSTR;
typedef WCHAR* LPWSTR;
typedef LONG_PTR LPARAM;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LRESULT;
typedef void* FARPROC;

#define WINAPI __stdcall
#define CALLBACK __stdcall
#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE ((HANDLE)(isize)-1)

struct POINT { LONG x; LONG y; };
struct RECT { LONG left; LONG top; LONG right; LONG bottom; };
struct MSG { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; DWORD lPrivate; };
struct PAINTSTRUCT { HDC hdc; BOOL fErase; RECT rcPaint; BOOL fRestore; BOOL fIncUpdate; BYTE rgbReserved[32]; };
struct WNDCLASSEXW {
    UINT cbSize; UINT style; LRESULT (CALLBACK *lpfnWndProc)(HWND,UINT,WPARAM,LPARAM);
    int cbClsExtra; int cbWndExtra; HINSTANCE hInstance; HICON hIcon; HCURSOR hCursor;
    HBRUSH hbrBackground; LPCWSTR lpszMenuName; LPCWSTR lpszClassName; HICON hIconSm;
};
struct SYSTEMTIME { u16 year, month, dayOfWeek, day, hour, minute, second, milliseconds; };
struct LARGE_INTEGER { s64 quad; };
struct MEMORY_BASIC_INFORMATION {
    LPVOID BaseAddress; LPVOID AllocationBase; DWORD AllocationProtect; u16 PartitionId;
    usize RegionSize; DWORD State; DWORD Protect; DWORD Type;
};
typedef DWORD (WINAPI *THREADPROC)(LPVOID);
typedef void (__fastcall *MissionDisplayInitFn)(void*);
typedef u8 (__fastcall *MissionDisplayBuildFn)(void*,u64);
typedef void (__fastcall *FacilityIdPrepareFn)(void*);
typedef void (__fastcall *FacilityIdResolveFn)(void*,void*);
typedef void (__fastcall *NativeFormatFn)(u64*,u32,u64,u32);
typedef void (__fastcall *NativeStringAssignFn)(u64*,u64);
typedef void (__fastcall *NativeStringReleaseFn)(u64*);

__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(LPCWSTR);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
__declspec(dllimport) DWORD WINAPI GetCurrentProcessId(void);
__declspec(dllimport) DWORD WINAPI GetCurrentThreadId(void);
__declspec(dllimport) HANDLE WINAPI CreateThread(LPVOID,usize,THREADPROC,LPVOID,DWORD,DWORD*);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE);
__declspec(dllimport) void WINAPI Sleep(DWORD);
__declspec(dllimport) u64 WINAPI GetTickCount64(void);
__declspec(dllimport) usize WINAPI VirtualQuery(LPCVOID,MEMORY_BASIC_INFORMATION*,usize);
__declspec(dllimport) BOOL WINAPI ReadProcessMemory(HANDLE,LPCVOID,LPVOID,usize,usize*);
__declspec(dllimport) void WINAPI GetLocalTime(SYSTEMTIME*);
__declspec(dllimport) HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,DWORD*,LPVOID);
__declspec(dllimport) BOOL WINAPI GetFileSizeEx(HANDLE,LARGE_INTEGER*);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HMODULE WINAPI LoadLibraryW(LPCWSTR);
__declspec(dllimport) FARPROC WINAPI GetProcAddress(HMODULE,LPCSTR);
__declspec(dllimport) int WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,int,LPWSTR,int);
__declspec(dllimport) int WINAPI GetUserDefaultLocaleName(LPWSTR,int);
__declspec(dllimport) DWORD WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,DWORD,LPCWSTR);
}

#define DLL_PROCESS_ATTACH 1u
#define GENERIC_READ 0x80000000u
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001u
#define FILE_SHARE_WRITE 0x00000002u
#define FILE_SHARE_DELETE 0x00000004u
#define CREATE_ALWAYS 2u
#define OPEN_ALWAYS 4u
#define OPEN_EXISTING 3u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define MEM_COMMIT 0x1000u
#define PAGE_READONLY 0x02u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE_READ 0x20u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define CP_UTF8 65001u
#define CP_ACP 0u
#define LOCALE_NAME_MAX_LENGTH 85

#define WM_NULL 0x0000u
#define WM_PAINT 0x000Fu
#define WM_DESTROY 0x0002u
#define WM_MOUSEACTIVATE 0x0021u
#define WM_NCHITTEST 0x0084u
#define WM_ERASEBKGND 0x0014u
#define WM_APP 0x8000u
#define PM_REMOVE 0x0001u
#define HC_ACTION 0
#define WH_GETMESSAGE 3
#define SW_HIDE 0
#define SW_SHOWNOACTIVATE 4
#define SWP_NOSENDCHANGING 0x0400u
#define SWP_NOACTIVATE 0x0010u
#define SWP_SHOWWINDOW 0x0040u
#define HWND_TOPMOST ((HWND)(isize)-1)
#define WS_POPUP 0x80000000u
#define WS_EX_TOPMOST 0x00000008u
#define WS_EX_TRANSPARENT 0x00000020u
#define WS_EX_TOOLWINDOW 0x00000080u
#define WS_EX_LAYERED 0x00080000u
#define WS_EX_NOACTIVATE 0x08000000u
#define LWA_ALPHA 0x00000002u
#define DT_LEFT 0x00000000u
#define DT_RIGHT 0x00000002u
#define DT_TOP 0x00000000u
#define DT_WORDBREAK 0x00000010u
#define DT_NOPREFIX 0x00000800u
#define DT_SINGLELINE 0x00000020u
#define DT_END_ELLIPSIS 0x00008000u
#define DT_RTLREADING 0x00020000u
#define TRANSPARENT 1
#define HTTRANSPARENT ((LRESULT)-1)
#define MA_NOACTIVATE 3
#define FW_NORMAL 400
#define FW_MEDIUM 500
#define DEFAULT_CHARSET 1
#define OUT_DEFAULT_PRECIS 0
#define CLIP_DEFAULT_PRECIS 0
#define CLEARTYPE_QUALITY 5
#define DEFAULT_PITCH 0
#define FF_DONTCARE 0

#define EXPECTED_EXE_SIZE 117851944LL
#define RVA_ROOT_HOLDER_GLOBAL 0x623EBF8ULL
#define ROOT_TABLE_OFFSET 0x19C0ULL
#define MAP_SELECTED_OFFSET 0x3E0ULL
#define SELECTED_TYPE_OFFSET 0x28ULL
#define SELECTED_HANDLE_OFFSET 0x48ULL
#define RVA_ENTITY_MANAGER_GLOBAL 0x623EAD8ULL
#define ENTITY_PHYSICAL_TABLE_OFFSET 0x58ULL
#define RVA_TYPE6_CONTENT 0x0C07650ULL
#define RVA_MISSION_GLOBAL 0x623EB30ULL
#define MISSION_SOURCE_ROOT_OFFSET 0x10ULL
#define MISSION_SOURCE_LIST_OFFSET 0x38ULL
#define RVA_MISSION_AVAILABILITY_EXTRA 0x147F1B0ULL
#define RVA_MISSION_DISPLAY_INIT 0x169F0A0ULL
#define RVA_MISSION_DISPLAY_RESET 0x1734CE0ULL
#define RVA_MISSION_DISPLAY_BUILD 0x1738AA0ULL
#define RVA_FACILITY_ID_PREPARE 0x0C53B30ULL
#define RVA_FACILITY_ID_RESOLVE 0x1472FB0ULL
#define RVA_NATIVE_FORMAT 0x0A42C0ULL
#define RVA_NATIVE_STRING_ASSIGN 0x0A3A20ULL
#define RVA_NATIVE_STRING_RELEASE 0x0A38A0ULL
#define RVA_FACILITY_FORMAT_DATA 0x32B3B28ULL
#define MISSION_CONTROLLER_RECORD_OFFSET 0x5D0ULL
#define RVA_NATIVE_EMPTY_STRING 0x3B34CA0ULL
#define RVA_VERIFY_MAP_ACCEPT 0x18611A0ULL
#define RVA_VERIFY_MISSION_BUILD 0x1738AA0ULL
#define MAX_NATIVE_DISPLAY_SCRATCH 0x1000u
#define MAX_CONTROLLER_ENTRIES 2048u
#define MAX_MISSION_SOURCE 4096u
#define MAX_DISPLAY_ROWS 10u
#define MAX_TEXT_CHARS 384u
#define FILTER_BATCH_SIZE 128u
#define UI_REQUEST_MESSAGE (WM_APP + 0x271u)
#define UI_REFRESH_MS 250u
#define HOVER_STABLE_MS 450u
#define SAME_SELECTION_REFRESH_MS 10000u

// Dynamically resolved USER32/GDI32 functions. Keeping those DLLs out of the
// import table avoids the ASI-loader compatibility problem seen in v13.
typedef BOOL (CALLBACK *WNDENUMPROC)(HWND,LPARAM);
typedef BOOL (WINAPI *PFN_EnumWindows)(WNDENUMPROC,LPARAM);
typedef DWORD (WINAPI *PFN_GetWindowThreadProcessId)(HWND,DWORD*);
typedef BOOL (WINAPI *PFN_IsWindowVisible)(HWND);
typedef HWND (WINAPI *PFN_GetForegroundWindow)(void);
typedef BOOL (WINAPI *PFN_GetClientRect)(HWND,RECT*);
typedef BOOL (WINAPI *PFN_ClientToScreen)(HWND,POINT*);
typedef HHOOK (WINAPI *PFN_SetWindowsHookExW)(int,LRESULT(CALLBACK*)(int,WPARAM,LPARAM),HINSTANCE,DWORD);
typedef BOOL (WINAPI *PFN_UnhookWindowsHookEx)(HHOOK);
typedef LRESULT (WINAPI *PFN_CallNextHookEx)(HHOOK,int,WPARAM,LPARAM);
typedef BOOL (WINAPI *PFN_PostThreadMessageW)(DWORD,UINT,WPARAM,LPARAM);
typedef ATOM (WINAPI *PFN_RegisterClassExW)(const WNDCLASSEXW*);
typedef HWND (WINAPI *PFN_CreateWindowExW)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
typedef LRESULT (WINAPI *PFN_DefWindowProcW)(HWND,UINT,WPARAM,LPARAM);
typedef BOOL (WINAPI *PFN_ShowWindow)(HWND,int);
typedef BOOL (WINAPI *PFN_SetWindowPos)(HWND,HWND,int,int,int,int,UINT);
typedef BOOL (WINAPI *PFN_InvalidateRect)(HWND,const RECT*,BOOL);
typedef BOOL (WINAPI *PFN_UpdateWindow)(HWND);
typedef BOOL (WINAPI *PFN_PeekMessageW)(MSG*,HWND,UINT,UINT,UINT);
typedef BOOL (WINAPI *PFN_TranslateMessage)(const MSG*);
typedef LRESULT (WINAPI *PFN_DispatchMessageW)(const MSG*);
typedef HDC (WINAPI *PFN_BeginPaint)(HWND,PAINTSTRUCT*);
typedef BOOL (WINAPI *PFN_EndPaint)(HWND,const PAINTSTRUCT*);
typedef int (WINAPI *PFN_FillRect)(HDC,const RECT*,HBRUSH);
typedef int (WINAPI *PFN_DrawTextW)(HDC,LPCWSTR,int,RECT*,UINT);
typedef BOOL (WINAPI *PFN_SetLayeredWindowAttributes)(HWND,DWORD,BYTE,DWORD);
typedef BOOL (WINAPI *PFN_DestroyWindow)(HWND);
typedef void (WINAPI *PFN_PostQuitMessage)(int);
typedef HBRUSH (WINAPI *PFN_CreateSolidBrush)(DWORD);
typedef HFONT (WINAPI *PFN_CreateFontW)(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);
typedef HGDIOBJ (WINAPI *PFN_SelectObject)(HDC,HGDIOBJ);
typedef DWORD (WINAPI *PFN_SetTextColor)(HDC,DWORD);
typedef int (WINAPI *PFN_SetBkMode)(HDC,int);
typedef BOOL (WINAPI *PFN_DeleteObject)(HGDIOBJ);

static PFN_EnumWindows pEnumWindows=0;
static PFN_GetWindowThreadProcessId pGetWindowThreadProcessId=0;
static PFN_IsWindowVisible pIsWindowVisible=0;
static PFN_GetForegroundWindow pGetForegroundWindow=0;
static PFN_GetClientRect pGetClientRect=0;
static PFN_ClientToScreen pClientToScreen=0;
static PFN_SetWindowsHookExW pSetWindowsHookExW=0;
static PFN_UnhookWindowsHookEx pUnhookWindowsHookEx=0;
static PFN_CallNextHookEx pCallNextHookEx=0;
static PFN_PostThreadMessageW pPostThreadMessageW=0;
static PFN_RegisterClassExW pRegisterClassExW=0;
static PFN_CreateWindowExW pCreateWindowExW=0;
static PFN_DefWindowProcW pDefWindowProcW=0;
static PFN_ShowWindow pShowWindow=0;
static PFN_SetWindowPos pSetWindowPos=0;
static PFN_InvalidateRect pInvalidateRect=0;
static PFN_UpdateWindow pUpdateWindow=0;
static PFN_PeekMessageW pPeekMessageW=0;
static PFN_TranslateMessage pTranslateMessage=0;
static PFN_DispatchMessageW pDispatchMessageW=0;
static PFN_BeginPaint pBeginPaint=0;
static PFN_EndPaint pEndPaint=0;
static PFN_FillRect pFillRect=0;
static PFN_DrawTextW pDrawTextW=0;
static PFN_SetLayeredWindowAttributes pSetLayeredWindowAttributes=0;
static PFN_DestroyWindow pDestroyWindow=0;
static PFN_PostQuitMessage pPostQuitMessage=0;
static PFN_CreateSolidBrush pCreateSolidBrush=0;
static PFN_CreateFontW pCreateFontW=0;
static PFN_SelectObject pSelectObject=0;
static PFN_SetTextColor pSetTextColor=0;
static PFN_SetBkMode pSetBkMode=0;
static PFN_DeleteObject pDeleteObject=0;

static HMODULE g_self=0;
static HANDLE g_process=0;
static u64 g_imageBase=0;
static u64 g_imageEnd=0;
static u32 g_imageSize=0;
static DWORD g_pid=0;
static HWND g_gameWindow=0;
static DWORD g_uiThreadId=0;
static HHOOK g_uiHook=0;
static HWND g_overlayWindow=0;
static HWND g_bestGameWindow=0;
static u64 g_bestGameWindowArea=0;
static u32 g_overlayPaintCount=0;
static u32 g_lastSetWindowPosOk=0;
static s32 g_lastGameX=0;
static s32 g_lastGameY=0;
static s32 g_lastGameW=0;
static s32 g_lastGameH=0;
static HBRUSH g_backgroundBrush=0;
static HFONT g_headingFont=0;
static HFONT g_rowFont=0;
static HFONT g_smallFont=0;
static volatile u32 g_started=0;
static volatile u32 g_clockTicks=0;
static volatile u32 g_uiRequestPending=0;
static volatile u32 g_snapshotWriteGeneration=0;
static u64 g_cachedMapController=0;
static u64 g_cachedControllerTable=0;
static u32 g_cachedControllerIndex=0;
static u64 g_cachedMissionController=0;
static u64 g_cachedMissionControllerTable=0;
static u32 g_cachedMissionControllerIndex=0;
static WCHAR g_moduleDir[1024];
static WCHAR g_logFile[1200];

struct LocalizedText {
    const WCHAR* remoteOrders;
    const WCHAR* facility;
    const WCHAR* available;
    const WCHAR* noOrders;
    const WCHAR* safeTextMode;
    const WCHAR* checkingOrders;
    const WCHAR* openMapHover;
    const WCHAR* missionSourceUnavailable;
    const WCHAR* moreOrders;
    const WCHAR* orderFallback;
};

static const LocalizedText kTextEnglish={
    L"Remote Orders",L"Facility",L"Available",L"No orders are currently available",
    L"Safe text mode: IDs and order types",L"Checking orders: ",
    L"Open the world map and hover over a facility",L"MissionSource is not available yet",
    L"More orders are available.",L"Order"};
static const LocalizedText kTextFrench={
    L"Commandes à distance",L"Installation",L"Disponibles",L"Aucune commande disponible actuellement",
    L"Mode texte sécurisé : ID et types de commandes",L"Vérification des commandes : ",
    L"Ouvrez la carte du monde et survolez une installation",L"MissionSource n'est pas encore disponible",
    L"D'autres commandes sont disponibles.",L"Commande"};
static const LocalizedText kTextItalian={
    L"Ordini remoti",L"Struttura",L"Disponibili",L"Nessun ordine attualmente disponibile",
    L"Modalità testo sicura: ID e tipi di ordine",L"Verifica degli ordini: ",
    L"Apri la mappa del mondo e passa su una struttura",L"MissionSource non è ancora disponibile",
    L"Sono disponibili altri ordini.",L"Ordine"};
static const LocalizedText kTextGerman={
    L"Remote-Aufträge",L"Einrichtung",L"Verfügbar",L"Keine aktuell verfügbaren Aufträge",
    L"Sicherer Textmodus: IDs und Auftragstypen",L"Aufträge werden geprüft: ",
    L"Weltkarte öffnen und über eine Einrichtung hovern",L"MissionSource noch nicht verfügbar",
    L"Weitere Aufträge sind vorhanden.",L"Auftrag"};
static const LocalizedText kTextSpanishSpain={
    L"Encargos remotos",L"Instalación",L"Disponibles",L"No hay encargos disponibles actualmente",
    L"Modo de texto seguro: ID y tipos de encargo",L"Comprobando encargos: ",
    L"Abre el mapa del mundo y pasa sobre una instalación",L"MissionSource aún no está disponible",
    L"Hay más encargos disponibles.",L"Encargo"};
static const LocalizedText kTextSpanishLatam={
    L"Órdenes remotas",L"Instalación",L"Disponibles",L"No hay órdenes disponibles actualmente",
    L"Modo de texto seguro: ID y tipos de orden",L"Comprobando órdenes: ",
    L"Abre el mapa del mundo y pasa sobre una instalación",L"MissionSource aún no está disponible",
    L"Hay más órdenes disponibles.",L"Orden"};
static const LocalizedText kTextPortugueseBrazil={
    L"Pedidos remotos",L"Instalação",L"Disponíveis",L"Não há pedidos disponíveis no momento",
    L"Modo de texto seguro: IDs e tipos de pedido",L"Verificando pedidos: ",
    L"Abra o mapa-múndi e passe o cursor sobre uma instalação",L"MissionSource ainda não está disponível",
    L"Há mais pedidos disponíveis.",L"Pedido"};
static const LocalizedText kTextPortuguesePortugal={
    L"Pedidos remotos",L"Instalação",L"Disponíveis",L"Não existem pedidos disponíveis neste momento",
    L"Modo de texto seguro: IDs e tipos de pedido",L"A verificar pedidos: ",
    L"Abra o mapa-mundo e passe o cursor sobre uma instalação",L"MissionSource ainda não está disponível",
    L"Existem mais pedidos disponíveis.",L"Pedido"};
static const LocalizedText kTextArabic={
    L"الطلبات البعيدة",L"المنشأة",L"المتاح",L"لا توجد طلبات متاحة حاليًا",
    L"الوضع النصي الآمن: المعرّفات وأنواع الطلبات",L"جارٍ التحقق من الطلبات: ",
    L"افتح خريطة العالم ومرر المؤشر فوق منشأة",L"MissionSource غير متاح بعد",
    L"توجد طلبات إضافية.",L"طلب"};
static const LocalizedText kTextCzech={
    L"Vzdálené zakázky",L"Zařízení",L"Dostupné",L"Momentálně nejsou dostupné žádné zakázky",
    L"Bezpečný textový režim: ID a typy zakázek",L"Kontrola zakázek: ",
    L"Otevřete mapu světa a najeďte na zařízení",L"MissionSource zatím není k dispozici",
    L"Jsou k dispozici další zakázky.",L"Zakázka"};
static const LocalizedText kTextDutch={
    L"Externe opdrachten",L"Faciliteit",L"Beschikbaar",L"Er zijn momenteel geen opdrachten beschikbaar",
    L"Veilige tekstmodus: ID's en opdrachttypen",L"Opdrachten controleren: ",
    L"Open de wereldkaart en beweeg over een faciliteit",L"MissionSource is nog niet beschikbaar",
    L"Er zijn meer opdrachten beschikbaar.",L"Opdracht"};
static const LocalizedText kTextGreek={
    L"Απομακρυσμένες παραγγελίες",L"Εγκατάσταση",L"Διαθέσιμες",L"Δεν υπάρχουν διαθέσιμες παραγγελίες αυτήν τη στιγμή",
    L"Ασφαλής λειτουργία κειμένου: ID και τύποι παραγγελιών",L"Έλεγχος παραγγελιών: ",
    L"Ανοίξτε τον παγκόσμιο χάρτη και περάστε πάνω από μια εγκατάσταση",L"Το MissionSource δεν είναι ακόμη διαθέσιμο",
    L"Υπάρχουν περισσότερες διαθέσιμες παραγγελίες.",L"Παραγγελία"};
static const LocalizedText kTextJapanese={
    L"遠隔依頼",L"施設",L"利用可能",L"現在利用可能な依頼はありません",
    L"セーフテキストモード：IDと依頼タイプ",L"依頼を確認中：",
    L"ワールドマップを開き、施設にカーソルを合わせてください",L"MissionSourceはまだ利用できません",
    L"他にも利用可能な依頼があります。",L"依頼"};
static const LocalizedText kTextKorean={
    L"원격 의뢰",L"시설",L"이용 가능",L"현재 이용 가능한 의뢰가 없습니다",
    L"안전 텍스트 모드: ID 및 의뢰 유형",L"의뢰 확인 중: ",
    L"월드 맵을 열고 시설 위에 커서를 놓으세요",L"MissionSource를 아직 사용할 수 없습니다",
    L"더 많은 의뢰가 있습니다.",L"의뢰"};
static const LocalizedText kTextPolish={
    L"Zdalne zlecenia",L"Placówka",L"Dostępne",L"Brak obecnie dostępnych zleceń",
    L"Bezpieczny tryb tekstowy: identyfikatory i typy zleceń",L"Sprawdzanie zleceń: ",
    L"Otwórz mapę świata i najedź na placówkę",L"MissionSource nie jest jeszcze dostępne",
    L"Dostępne są kolejne zlecenia.",L"Zlecenie"};
static const LocalizedText kTextRussian={
    L"Удалённые заказы",L"Объект",L"Доступно",L"Сейчас нет доступных заказов",
    L"Безопасный текстовый режим: ID и типы заказов",L"Проверка заказов: ",
    L"Откройте карту мира и наведите курсор на объект",L"MissionSource пока недоступен",
    L"Доступны дополнительные заказы.",L"Заказ"};
static const LocalizedText kTextChineseSimplified={
    L"远程订单",L"设施",L"可用",L"当前没有可用订单",
    L"安全文本模式：ID和订单类型",L"正在检查订单：",
    L"打开世界地图并将光标悬停在设施上",L"MissionSource暂不可用",
    L"还有更多可用订单。",L"订单"};
static const LocalizedText kTextChineseTraditional={
    L"遠端訂單",L"設施",L"可用",L"目前沒有可用訂單",
    L"安全文字模式：ID 和訂單類型",L"正在檢查訂單：",
    L"開啟世界地圖並將游標停在設施上",L"MissionSource 尚不可用",
    L"還有更多可用訂單。",L"訂單"};

static const LocalizedText* g_text=&kTextEnglish;
static u64 g_lastLoggedSelection=0;
static s32 g_lastLoggedCount=-9999;
static u32 g_lastLoggedScanned=0;
static u64 g_uiPendingSelection=0;
static u64 g_uiPendingSince=0;
static u64 g_uiLastBuiltSelection=0;
static u64 g_uiLastBuildTick=0;
[[maybe_unused]] static u8 g_nativeDisplayScratch[MAX_NATIVE_DISPLAY_SCRATCH];

struct SelectionState {
    u64 mapController;
    u64 selectedEntry;
    u64 rawHandle;
    u64 physicalWrapper;
    u64 physicalEntity;
    u32 selectionType;
    u32 handleIndex;
    s32 facilityId;
    char selectedType[96];
    char wrapperType[96];
};

struct DisplayRow {
    u64 mission;
    u64 resource;
    u32 missionId;
    u32 missionType;
    s32 destinationId;
    WCHAR title[MAX_TEXT_CHARS];
    WCHAR subtitle[MAX_TEXT_CHARS];
    WCHAR kind[96];
};

struct DisplaySnapshot {
    u32 generation;
    u32 visible;
    u64 tick;
    u64 selectionKey;
    u32 selectionType;
    u32 handleIndex;
    s32 facilityId;
    s32 totalAvailable;
    u32 displayedRows;
    u32 scannedMissions;
    u32 sourceMissions;
    u32 nativeCallFaults;
    WCHAR facilityLabel[128];
    WCHAR status[192];
    DisplayRow rows[MAX_DISPLAY_ROWS];
};

static DisplaySnapshot g_snapshotBuffers[2];
static u64 g_missionPointerScratch[MAX_MISSION_SOURCE];

struct MissionScanState {
    u32 active;
    u32 complete;
    u64 selectionKey;
    s32 facilityId;
    u32 handleIndex;
    u32 selectionType;
    u32 sourceCount;
    u32 nextIndex;
    u32 totalAvailable;
    u32 displayedRows;
    u32 nativeCallFaults;
    WCHAR facilityLabel[128];
    DisplayRow rows[MAX_DISPLAY_ROWS];
};
static MissionScanState g_scan;
static volatile u32 g_nativeCallFaults=0;
static volatile u32 g_nameCalls=0;
static volatile u32 g_nameInitOk=0;
static volatile u32 g_nameInitFaults=0;
static volatile u32 g_nameBuildOk=0;
static volatile u32 g_nameBuildFaults=0;
static volatile u32 g_nameFaults=0;
static volatile u32 g_nameNonEmpty=0;
static u32 g_nameFaultCode=0;static u64 g_nameFaultAddress=0;
struct NameExceptionRecord{u32 code;u32 flags;u64 nested;u64 address;u32 count;u32 pad;u64 info[15];};
struct NameExceptionPointers{NameExceptionRecord*record;void*context;};
extern "C" void* _exception_info(void);
#define NameFaultFilter() (g_nameFaultCode=((NameExceptionPointers*)_exception_info())->record->code,g_nameFaultAddress=((NameExceptionPointers*)_exception_info())->record->address,1)
static u64 g_nameLastPrimary=0,g_nameLastFallback=0,g_nameLastTertiary=0,g_nameLastQuaternary=0;

static void MemSet(void*p,u8 v,usize n){u8*d=(u8*)p;for(usize i=0;i<n;++i)d[i]=v;}
static void MemCopy(void*d0,const void*s0,usize n){u8*d=(u8*)d0;const u8*s=(const u8*)s0;for(usize i=0;i<n;++i)d[i]=s[i];}
static u64 NowMs(void){return (u64)__atomic_add_fetch(&g_clockTicks,1u,__ATOMIC_ACQ_REL)*16ULL;}
static u64 LoadU64Local(const u8*b,u32 off){u64 v=0;MemCopy(&v,b+off,8);return v;}
static u32 LoadU32Local(const u8*b,u32 off){u32 v=0;MemCopy(&v,b+off,4);return v;}
static usize StrLenA(const char*s){usize n=0;if(s)while(s[n])++n;return n;}
static usize StrLenW(const WCHAR*s){usize n=0;if(s)while(s[n])++n;return n;}
static void CopyW(WCHAR*d,usize c,const WCHAR*s){if(!d||!c)return;usize i=0;if(s)while(s[i]&&i+1<c){d[i]=s[i];++i;}d[i]=0;}
static void AppendCharW(WCHAR*d,usize c,WCHAR x){usize n=StrLenW(d);if(n+1<c){d[n]=x;d[n+1]=0;}}
static void AppendDecW(WCHAR*d,usize c,u64 v){WCHAR t[32];u32 n=0;if(!v)t[n++]=L'0';while(v&&n<31){t[n++]=(WCHAR)(L'0'+(v%10));v/=10;}while(n)AppendCharW(d,c,t[--n]);}
static void AppendSignedW(WCHAR*d,usize c,s64 v){if(v<0){AppendCharW(d,c,L'-');AppendDecW(d,c,(u64)(-v));}else AppendDecW(d,c,(u64)v);}
static void AppendW(WCHAR*d,usize c,const WCHAR*s){usize n=StrLenW(d),i=0;if(!s)return;while(s[i]&&n+1<c)d[n++]=s[i++];d[n]=0;}
static bool IsWideSpace(WCHAR c){return c==L' '||c==L'\t'||c==0x00A0||c==0x202F;}
static bool IsInvisibleFormatChar(WCHAR c){return c==0x00AD||c==0x034F||c==0x061C||(c>=0x200B&&c<=0x200F)||(c>=0x202A&&c<=0x202E)||(c>=0x2060&&c<=0x2069)||c==0xFEFF;}
static WCHAR FoldDuplicateChar(WCHAR c){
    if(c>=L'A'&&c<=L'Z')return(WCHAR)(c+0x20);
    if((c>=0x00C0&&c<=0x00D6)||(c>=0x00D8&&c<=0x00DE))return(WCHAR)(c+0x20);
    if(c>=0x0391&&c<=0x03A1)return(WCHAR)(c+0x20);if(c>=0x03A3&&c<=0x03AB)return(WCHAR)(c+0x20);
    if(c>=0x0410&&c<=0x042F)return(WCHAR)(c+0x20);if(c>=0x0400&&c<=0x040F)return(WCHAR)(c+0x50);
    if(c==0x0386)return 0x03AC;if(c==0x0388)return 0x03AD;if(c==0x0389)return 0x03AE;if(c==0x038A)return 0x03AF;
    if(c==0x038C)return 0x03CC;if(c==0x038E)return 0x03CD;if(c==0x038F)return 0x03CE;
    if(c==0x2018||c==0x2019||c==0x02BC)return L'\'';
    return c;
}
static bool CollapseDuplicatedLocalizedText(WCHAR*text){
    if(!text||!text[0])return false;WCHAR normalized[MAX_TEXT_CHARS];u16 sourceIndex[MAX_TEXT_CHARS];u32 count=0;
    for(u32 i=0;text[i]&&count<MAX_TEXT_CHARS;++i){WCHAR c=text[i];if(IsInvisibleFormatChar(c))continue;if(IsWideSpace(c)){if(!count||normalized[count-1]==L' ')continue;c=L' ';}normalized[count]=FoldDuplicateChar(c);sourceIndex[count]=(u16)i;++count;}
    while(count&&normalized[count-1]==L' ')--count;if(count<2)return false;u32 leftCount=count/2,rightStart=leftCount;
    if(count&1u){if(normalized[leftCount]!=L' ')return false;rightStart=leftCount+1u;}
    if(count-rightStart!=leftCount)return false;for(u32 i=0;i<leftCount;++i)if(normalized[i]!=normalized[rightStart+i])return false;
    u32 cut=sourceIndex[rightStart];while(cut&&(IsWideSpace(text[cut-1])||IsInvisibleFormatChar(text[cut-1])))--cut;text[cut]=0;return true;
}
static void AppendA(char*d,usize c,const char*s){usize n=StrLenA(d),i=0;if(!s)return;while(s[i]&&n+1<c)d[n++]=s[i++];d[n]=0;}
static void AppendCharA(char*d,usize c,char x){usize n=StrLenA(d);if(n+1<c){d[n]=x;d[n+1]=0;}}
static void AppendDecA(char*d,usize c,u64 v){char t[32];u32 n=0;if(!v)t[n++]='0';while(v&&n<31){t[n++]=(char)('0'+(v%10));v/=10;}while(n)AppendCharA(d,c,t[--n]);}
static void AppendSignedA(char*d,usize c,s64 v){if(v<0){AppendCharA(d,c,'-');AppendDecA(d,c,(u64)(-v));}else AppendDecA(d,c,(u64)v);}
static void AppendHexA(char*d,usize c,u64 v){static const char*h="0123456789ABCDEF";AppendA(d,c,"0x");bool started=false;for(s32 sh=60;sh>=0;sh-=4){u8 x=(u8)((v>>sh)&15);if(x||started||sh==0){AppendCharA(d,c,h[x]);started=true;}}}
static char LowerAscii(char c){return(c>='A'&&c<='Z')?(char)(c+32):c;}
static bool StartsWithA(const char*s,const char*p){if(!s||!p)return false;for(usize i=0;p[i];++i)if(s[i]!=p[i])return false;return true;}
static bool ContainsInsensitiveA(const char*s,const char*n){if(!s||!n||!n[0])return false;usize sl=StrLenA(s),nl=StrLenA(n);if(nl>sl)return false;for(usize i=0;i+nl<=sl;++i){bool ok=true;for(usize j=0;j<nl;++j)if(LowerAscii(s[i+j])!=LowerAscii(n[j])){ok=false;break;}if(ok)return true;}return false;}
static void JoinPath(WCHAR*out,usize cap,const WCHAR*dir,const WCHAR*name){CopyW(out,cap,dir);usize n=StrLenW(out);if(n&&out[n-1]!=L'\\'&&n+1<cap){out[n++]=L'\\';out[n]=0;}usize i=0;while(name&&name[i]&&n+1<cap)out[n++]=name[i++];out[n]=0;}
static WCHAR LowerAsciiW(WCHAR c){return(c>=L'A'&&c<=L'Z')?(WCHAR)(c+32):c;}
static bool StartsWithInsensitiveW(const WCHAR*s,const WCHAR*p){if(!s||!p)return false;for(usize i=0;p[i];++i)if(!s[i]||LowerAsciiW(s[i])!=LowerAsciiW(p[i]))return false;return true;}
static bool EqualsInsensitiveW(const WCHAR*a,const WCHAR*b){if(!a||!b)return false;usize i=0;while(a[i]&&b[i]){if(LowerAsciiW(a[i])!=LowerAsciiW(b[i]))return false;++i;}return a[i]==0&&b[i]==0;}
static const LocalizedText* SelectLocalizedText(const WCHAR*language){
    if(!language||!language[0])return &kTextEnglish;
    if(StartsWithInsensitiveW(language,L"fr")||EqualsInsensitiveW(language,L"french"))return &kTextFrench;
    if(StartsWithInsensitiveW(language,L"it")||EqualsInsensitiveW(language,L"italian"))return &kTextItalian;
    if(StartsWithInsensitiveW(language,L"de")||EqualsInsensitiveW(language,L"german"))return &kTextGerman;
    if(StartsWithInsensitiveW(language,L"es")){
        if(EqualsInsensitiveW(language,L"es")||StartsWithInsensitiveW(language,L"es-es"))return &kTextSpanishSpain;
        return &kTextSpanishLatam;
    }
    if(EqualsInsensitiveW(language,L"spanish"))return &kTextSpanishSpain;
    if(EqualsInsensitiveW(language,L"latam"))return &kTextSpanishLatam;
    if(StartsWithInsensitiveW(language,L"pt"))return StartsWithInsensitiveW(language,L"pt-br")?&kTextPortugueseBrazil:&kTextPortuguesePortugal;
    if(EqualsInsensitiveW(language,L"brazilian"))return &kTextPortugueseBrazil;
    if(EqualsInsensitiveW(language,L"portuguese"))return &kTextPortuguesePortugal;
    if(StartsWithInsensitiveW(language,L"ar")||EqualsInsensitiveW(language,L"arabic"))return &kTextArabic;
    if(StartsWithInsensitiveW(language,L"cs")||EqualsInsensitiveW(language,L"czech"))return &kTextCzech;
    if(StartsWithInsensitiveW(language,L"nl")||EqualsInsensitiveW(language,L"dutch"))return &kTextDutch;
    if(StartsWithInsensitiveW(language,L"el")||EqualsInsensitiveW(language,L"greek"))return &kTextGreek;
    if(StartsWithInsensitiveW(language,L"ja")||EqualsInsensitiveW(language,L"japanese"))return &kTextJapanese;
    if(StartsWithInsensitiveW(language,L"ko")||EqualsInsensitiveW(language,L"koreana"))return &kTextKorean;
    if(StartsWithInsensitiveW(language,L"pl")||EqualsInsensitiveW(language,L"polish"))return &kTextPolish;
    if(StartsWithInsensitiveW(language,L"ru")||EqualsInsensitiveW(language,L"russian"))return &kTextRussian;
    if(StartsWithInsensitiveW(language,L"zh-cn")||StartsWithInsensitiveW(language,L"zh-sg")||StartsWithInsensitiveW(language,L"zh-hans")||EqualsInsensitiveW(language,L"schinese"))return &kTextChineseSimplified;
    if(StartsWithInsensitiveW(language,L"zh")||EqualsInsensitiveW(language,L"tchinese"))return &kTextChineseTraditional;
    return &kTextEnglish;
}
static void InitializeLocalization(void){
    WCHAR iniPath[1200];JoinPath(iniPath,1200,g_moduleDir,L"DS2_Remote_Orders_Overlay.ini");
    WCHAR configured[64];MemSet(configured,0,sizeof(configured));
    GetPrivateProfileStringW(L"Localization",L"Language",L"auto",configured,64,iniPath);
    if(EqualsInsensitiveW(configured,L"auto")){
        WCHAR localeName[LOCALE_NAME_MAX_LENGTH];MemSet(localeName,0,sizeof(localeName));
        if(GetUserDefaultLocaleName(localeName,LOCALE_NAME_MAX_LENGTH)>0)g_text=SelectLocalizedText(localeName);
        else g_text=&kTextEnglish;
    }else g_text=SelectLocalizedText(configured);
}
static u32 AtomicLoad(volatile u32*v){return __atomic_load_n(v,__ATOMIC_ACQUIRE);}
static void AtomicStore(volatile u32*v,u32 x){__atomic_store_n(v,x,__ATOMIC_RELEASE);}
static bool AtomicStartOnce(void){u32 expected=0;return __atomic_compare_exchange_n(&g_started,&expected,1,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE);}
static u32 AtomicInc(volatile u32*v){return __atomic_add_fetch(v,1u,__ATOMIC_ACQ_REL);}
static u32 Crc32CWord(u32 value){u32 crc=0;for(u32 i=0;i<4;++i){crc^=(value>>(i*8))&0xFFu;for(u32 b=0;b<8;++b)crc=(crc&1u)?((crc>>1)^0x82F63B78u):(crc>>1);}return crc;}

static bool IsCanonicalPointer(u64 v){return v>=0x10000ULL&&v<0x0000800000000000ULL;}
static bool IsReadableProtect(DWORD p){if(p&PAGE_GUARD)return false;DWORD b=p&0xFFu;return b==PAGE_READONLY||b==PAGE_READWRITE||b==PAGE_WRITECOPY||b==PAGE_EXECUTE_READ||b==PAGE_EXECUTE_READWRITE||b==PAGE_EXECUTE_WRITECOPY;}
static usize ReadableBytesAt(u64 a){if(!IsCanonicalPointer(a))return 0;MEMORY_BASIC_INFORMATION mbi;MemSet(&mbi,0,sizeof(mbi));if(VirtualQuery((LPCVOID)(usize)a,&mbi,sizeof(mbi))!=sizeof(mbi))return 0;if(mbi.State!=MEM_COMMIT||!IsReadableProtect(mbi.Protect))return 0;u64 b=(u64)(usize)mbi.BaseAddress,e=b+(u64)mbi.RegionSize;if(a<b||a>=e)return 0;return(usize)(e-a);}
static bool SafeRead(u64 a,void*out,usize n){if(!out||!n||ReadableBytesAt(a)<n)return false;usize got=0;return ReadProcessMemory(g_process,(LPCVOID)(usize)a,out,n,&got)&&got==n;}
static u64 ReadU64(u64 a){u64 v=0;SafeRead(a,&v,8);return v;}
static u32 ReadU32(u64 a){u32 v=0;SafeRead(a,&v,4);return v;}
static s32 ReadS32(u64 a){s32 v=0;SafeRead(a,&v,4);return v;}
static u8 ReadU8(u64 a){u8 v=0;SafeRead(a,&v,1);return v;}

static void DetermineModuleDirectory(void){WCHAR p[1024];MemSet(p,0,sizeof(p));GetModuleFileNameW(g_self,p,1023);CopyW(g_moduleDir,1024,p);usize n=StrLenW(g_moduleDir);while(n&&g_moduleDir[n-1]!=L'\\'&&g_moduleDir[n-1]!=L'/')--n;if(n)g_moduleDir[n-1]=0;CopyW(g_logFile,1200,L"NUL");} // Release build: disable persistent logging.
static void WriteStage(const WCHAR*name,const char*message){HANDLE f=CreateFileW(name,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return;DWORD wrote=0,n=0;while(message&&message[n]&&n<240)++n;WriteFile(f,message,n,&wrote,0);CloseHandle(f);}
static void EarlyLog(const char*message){WriteStage(L"NUL",message);}
static void WriteTextFile(const WCHAR*path,const char*text){HANDLE f=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return;DWORD wrote=0;WriteFile(f,text,(DWORD)StrLenA(text),&wrote,0);CloseHandle(f);}
static void LogSnapshot(const DisplaySnapshot*s){if(!s)return;char text[16384];text[0]=0;AppendA(text,sizeof(text),"DS2 Remote Orders Overlay v0.2.0\r\nSelection=");AppendHexA(text,sizeof(text),s->selectionKey);AppendA(text,sizeof(text),"\r\nHandleIndex=");AppendDecA(text,sizeof(text),s->handleIndex);AppendA(text,sizeof(text),"\r\nFacilityId=");AppendSignedA(text,sizeof(text),s->facilityId);AppendA(text,sizeof(text),"\r\nAvailable=");AppendSignedA(text,sizeof(text),s->totalAvailable);AppendA(text,sizeof(text),"\r\nDisplayed=");AppendDecA(text,sizeof(text),s->displayedRows);AppendA(text,sizeof(text),"\r\nScanned=");AppendDecA(text,sizeof(text),s->scannedMissions);AppendA(text,sizeof(text),"/");AppendDecA(text,sizeof(text),s->sourceMissions);AppendA(text,sizeof(text),"\r\nNativeCallFaults=");AppendDecA(text,sizeof(text),s->nativeCallFaults);AppendA(text,sizeof(text),"\r\nNameCalls=");AppendDecA(text,sizeof(text),g_nameCalls);AppendA(text,sizeof(text)," InitOk=");AppendDecA(text,sizeof(text),g_nameInitOk);AppendA(text,sizeof(text)," InitFaults=");AppendDecA(text,sizeof(text),g_nameInitFaults);AppendA(text,sizeof(text)," BuildOk=");AppendDecA(text,sizeof(text),g_nameBuildOk);AppendA(text,sizeof(text)," BuildFaults=");AppendDecA(text,sizeof(text),g_nameBuildFaults);AppendA(text,sizeof(text)," Faults=");AppendDecA(text,sizeof(text),g_nameFaults);AppendA(text,sizeof(text)," Code=");AppendHexA(text,sizeof(text),g_nameFaultCode);AppendA(text,sizeof(text)," Address=");AppendHexA(text,sizeof(text),g_nameFaultAddress);AppendA(text,sizeof(text)," NonEmpty=");AppendDecA(text,sizeof(text),g_nameNonEmpty);AppendA(text,sizeof(text)," Ptrs=");AppendHexA(text,sizeof(text),g_nameLastPrimary);AppendA(text,sizeof(text),",");AppendHexA(text,sizeof(text),g_nameLastFallback);AppendA(text,sizeof(text),",");AppendHexA(text,sizeof(text),g_nameLastTertiary);AppendA(text,sizeof(text),",");AppendHexA(text,sizeof(text),g_nameLastQuaternary);AppendA(text,sizeof(text),"\r\nOverlayWindow=");AppendHexA(text,sizeof(text),(u64)(usize)g_overlayWindow);AppendA(text,sizeof(text),"\r\nGameWindow=");AppendHexA(text,sizeof(text),(u64)(usize)g_gameWindow);AppendA(text,sizeof(text),"\r\nGameClient=");AppendSignedA(text,sizeof(text),g_lastGameX);AppendA(text,sizeof(text),",");AppendSignedA(text,sizeof(text),g_lastGameY);AppendA(text,sizeof(text)," ");AppendSignedA(text,sizeof(text),g_lastGameW);AppendA(text,sizeof(text),"x");AppendSignedA(text,sizeof(text),g_lastGameH);AppendA(text,sizeof(text),"\r\nSetWindowPosOk=");AppendDecA(text,sizeof(text),g_lastSetWindowPosOk);AppendA(text,sizeof(text),"\r\nPaintCount=");AppendDecA(text,sizeof(text),g_overlayPaintCount);AppendA(text,sizeof(text),"\r\n");for(u32 i=0;i<s->displayedRows;++i){AppendA(text,sizeof(text),"Mission=");AppendHexA(text,sizeof(text),s->rows[i].mission);AppendA(text,sizeof(text)," Resource=");AppendHexA(text,sizeof(text),s->rows[i].resource);AppendA(text,sizeof(text)," DestinationId=");AppendSignedA(text,sizeof(text),s->rows[i].destinationId);AppendA(text,sizeof(text),"\r\n");}WriteTextFile(g_logFile,text);}

static bool GetTypeName(u64 object,char*out,usize cap,u64*outVtable){if(out&&cap)out[0]=0;if(outVtable)*outVtable=0;if(!IsCanonicalPointer(object))return false;u64 vt=ReadU64(object);if(vt<g_imageBase||vt>=g_imageEnd)return false;u64 col=ReadU64(vt-8);if(col<g_imageBase||col+0x18>=g_imageEnd)return false;if(ReadU32(col)!=1)return false;u32 td=ReadU32(col+0x0C);if(!td||td>=g_imageSize)return false;char raw[160];MemSet(raw,0,sizeof(raw));if(!SafeRead(g_imageBase+td+0x10,raw,sizeof(raw)-1))return false;const char*src=raw;if(StartsWithA(src,".?AV")||StartsWithA(src,".?AU"))src+=4;usize i=0;while(src[i]&&i+1<cap){if(src[i]=='@'&&src[i+1]=='@')break;out[i]=src[i];++i;}if(cap)out[i]=0;if(outVtable)*outVtable=vt;return i>0;}

static bool FindMapController(u64*outController){if(outController)*outController=0;u64 slot=ReadU64(g_imageBase+RVA_ROOT_HOLDER_GLOBAL);u64 holder=ReadU64(slot);if(!IsCanonicalPointer(holder))return false;u64 table=ReadU64(holder+ROOT_TABLE_OFFSET);if(!IsCanonicalPointer(table))return false;if(g_cachedMapController&&g_cachedControllerTable==table){u64 current=ReadU64(table+(u64)g_cachedControllerIndex*24ULL);char t[96];MemSet(t,0,sizeof(t));if(current==g_cachedMapController&&GetTypeName(g_cachedMapController,t,sizeof(t),0)&&ContainsInsensitiveA(t,"DSUIMapMenuController")){if(outController)*outController=g_cachedMapController;return true;}g_cachedMapController=0;}usize avail=ReadableBytesAt(table);u32 max=(u32)(avail/24u);if(max>MAX_CONTROLLER_ENTRIES)max=MAX_CONTROLLER_ENTRIES;for(u32 i=0;i<max;++i){u64 obj=ReadU64(table+(u64)i*24ULL);if(!IsCanonicalPointer(obj))continue;char t[96];MemSet(t,0,sizeof(t));if(GetTypeName(obj,t,sizeof(t),0)&&ContainsInsensitiveA(t,"DSUIMapMenuController")){g_cachedMapController=obj;g_cachedControllerTable=table;g_cachedControllerIndex=i;if(outController)*outController=obj;return true;}}return false;}

static bool ResolveSelectedFacility(SelectionState*out){if(!out)return false;MemSet(out,0,sizeof(*out));if(!FindMapController(&out->mapController))return false;out->selectedEntry=ReadU64(out->mapController+MAP_SELECTED_OFFSET);if(!IsCanonicalPointer(out->selectedEntry))return false;out->selectionType=ReadU8(out->selectedEntry+SELECTED_TYPE_OFFSET);GetTypeName(out->selectedEntry,out->selectedType,sizeof(out->selectedType),0);if(out->selectionType==5){out->rawHandle=ReadU64(out->selectedEntry+SELECTED_HANDLE_OFFSET);}else if(out->selectionType==6){typedef void* (__fastcall *Type6Fn)(void*);Type6Fn fn=(Type6Fn)(usize)(g_imageBase+RVA_TYPE6_CONTENT);void* content=0;__try { content=fn((void*)(usize)out->selectedEntry); } __except(1) { AtomicInc(&g_nativeCallFaults);content=0; }u64 c=(u64)(usize)content;if(IsCanonicalPointer(c))out->rawHandle=ReadU64(c+0x08);}else return false;if(!out->rawHandle||out->rawHandle==~0ULL)return false;out->handleIndex=(u32)(((s64)out->rawHandle>>41)&0x7FFF);u64 entityManager=ReadU64(g_imageBase+RVA_ENTITY_MANAGER_GLOBAL);if(!IsCanonicalPointer(entityManager))return false;u64 physicalTable=ReadU64(entityManager+ENTITY_PHYSICAL_TABLE_OFFSET);if(!IsCanonicalPointer(physicalTable))return false;out->physicalWrapper=ReadU64(physicalTable+(u64)out->handleIndex*8ULL);if(!IsCanonicalPointer(out->physicalWrapper))return false;GetTypeName(out->physicalWrapper,out->wrapperType,sizeof(out->wrapperType),0);out->facilityId=ReadS32(out->physicalWrapper+0x160);if(out->facilityId<0||out->facilityId>100000)return false;u64 sub=ReadU64(out->physicalWrapper+0x38);if(IsCanonicalPointer(sub)&&sub>=0x20)out->physicalEntity=sub-0x20;return true;}

static bool IsFunctionInImage(u64 f){return f!=0&&g_imageBase!=0&&g_imageEnd>g_imageBase&&f>=g_imageBase&&f<g_imageEnd;}
typedef u8 (__fastcall *ResourceStateFn)(void*);
typedef u8 (__fastcall *ResourceFacilityFn)(void*,s32);
typedef u8 (__fastcall *MissionExtraAvailabilityFn)(void*);

static bool MissionAvailableAtFacility(u64 mission,s32 facilityId,u64*outResource,s32*outDestination,u32*outMissionId,u32*outMissionType){
    if(outResource)*outResource=0;if(outDestination)*outDestination=-1;if(outMissionId)*outMissionId=0;if(outMissionType)*outMissionType=0;if(!IsCanonicalPointer(mission))return false;
    u8 m[0x30];if(!SafeRead(mission,m,sizeof(m)))return false;
    u64 packed=LoadU64Local(m,0x28);u8 type=(u8)((packed>>32)&0x3F);if(outMissionId)*outMissionId=(u32)packed;if(outMissionType)*outMissionType=type;
    if(type==9||type==6||type==10||type==8||(type>=13&&type<=16))return false;
    u64 resource=LoadU64Local(m,0x10);if(!IsCanonicalPointer(resource))return false;
    u8 r[0x48];if(!SafeRead(resource,r,sizeof(r)))return false;u64 rvt=LoadU64Local(r,0);if(!IsFunctionInImage(rvt))return false;
    u64 stateObj=LoadU64Local(r,0x40);
    if(IsCanonicalPointer(stateObj)){
        u64 svt=ReadU64(stateObj);u64 sf=IsCanonicalPointer(svt)?ReadU64(svt+0x20):0;
        if(IsFunctionInImage(sf)){
            u8 st=0;bool callOk=false;
            __try { ResourceStateFn fn=(ResourceStateFn)(usize)sf;st=fn((void*)(usize)stateObj);callOk=true; }
            __except(1) { AtomicInc(&g_nativeCallFaults); }
            if(!callOk)return false;
            if(st==1||st==6||st==2||st==3||st==7)return false;
        }
    }
    u64 facilityPredicate=ReadU64(rvt+0xB0);if(!IsFunctionInImage(facilityPredicate))return false;
    u8 belongs=0;bool predicateOk=false;
    __try { ResourceFacilityFn pf=(ResourceFacilityFn)(usize)facilityPredicate;belongs=pf((void*)(usize)resource,facilityId);predicateOk=true; }
    __except(1) { AtomicInc(&g_nativeCallFaults); }
    if(!predicateOk||!belongs)return false;
    u8 extraAvailable=0;bool extraOk=false;
    __try { MissionExtraAvailabilityFn extra=(MissionExtraAvailabilityFn)(usize)(g_imageBase+RVA_MISSION_AVAILABILITY_EXTRA);extraAvailable=extra((void*)(usize)mission);extraOk=true; }
    __except(1) { AtomicInc(&g_nativeCallFaults); }
    if(!extraOk||!extraAvailable)return false;
    u32 flags=LoadU32Local(m,0x24);if(((flags>>25)&1u)||((flags>>7)&1u))return false;if(m[0x20]!=1)return false;
    if(outResource)*outResource=resource;return true;
}

static void AsciiTypeToWide(const char*src,WCHAR*out,u32 cap){if(!out||!cap)return;u32 i=0;while(src&&src[i]&&i+1<cap){out[i]=(WCHAR)(u8)src[i];++i;}out[i]=0;}
static bool CopyNativeUtf8ToWide(u64 textPtr,WCHAR*out,u32 cap){
    if(!out||!cap){return false;}out[0]=0;if(!IsCanonicalPointer(textPtr))return false;
    char raw[MAX_TEXT_CHARS*4];MemSet(raw,0,sizeof(raw));usize n=0;
    while(n+1<sizeof(raw)){if(!SafeRead(textPtr+n,&raw[n],1))break;if(!raw[n])break;++n;}
    if(!n)return false;
    u32 oi=0;for(usize i=0;i<n&&oi+1<cap;){
        // DS2 localized strings may contain its rich-text tags, e.g.
        // <letter case=default>. The overlay is plain Win32 text, so remove
        // the complete tag rather than exposing the markup to the user.
        if(raw[i]=='<'){while(i<n&&raw[i]!='>')++i;if(i<n)++i;continue;}
        u8 c=(u8)raw[i++];u32 cp=0;
        if(c<0x80)cp=c;else if((c&0xE0)==0xC0&&i<n)cp=((u32)(c&0x1F)<<6)|((u8)raw[i++]&0x3F);
        else if((c&0xF0)==0xE0&&i+1<n){u8 c1=(u8)raw[i++],c2=(u8)raw[i++];cp=((u32)(c&0x0F)<<12)|((u32)(c1&0x3F)<<6)|(c2&0x3F);}
        else if((c&0xF8)==0xF0&&i+2<n){u8 c1=(u8)raw[i++],c2=(u8)raw[i++],c3=(u8)raw[i++];cp=((u32)(c&7)<<18)|((u32)(c1&0x3F)<<12)|((u32)(c2&0x3F)<<6)|(c3&0x3F);}
        else cp=L'?';if(cp<=0xFFFF)out[oi++]=(WCHAR)cp;else if(oi+2<cap){cp-=0x10000;out[oi++]=(WCHAR)(0xD800+(cp>>10));out[oi++]=(WCHAR)(0xDC00+(cp&0x3FF));}}
    out[oi]=0;return oi!=0;
}

static bool FindMissionController(u64*outController){
    if(outController)*outController=0;u64 slot=ReadU64(g_imageBase+RVA_ROOT_HOLDER_GLOBAL);u64 holder=ReadU64(slot);
    if(!IsCanonicalPointer(holder))return false;u64 table=ReadU64(holder+ROOT_TABLE_OFFSET);if(!IsCanonicalPointer(table))return false;
    if(g_cachedMissionController&&g_cachedMissionControllerTable==table){u64 current=ReadU64(table+(u64)g_cachedMissionControllerIndex*24ULL);char t[96];MemSet(t,0,sizeof(t));if(current==g_cachedMissionController&&GetTypeName(current,t,sizeof(t),0)&&ContainsInsensitiveA(t,"DSUIMissionMenuController")){if(outController)*outController=current;return true;}g_cachedMissionController=0;}
    usize avail=ReadableBytesAt(table);u32 max=(u32)(avail/24u);if(max>MAX_CONTROLLER_ENTRIES)max=MAX_CONTROLLER_ENTRIES;
    for(u32 i=0;i<max;++i){u64 obj=ReadU64(table+(u64)i*24ULL);if(!IsCanonicalPointer(obj))continue;char t[96];MemSet(t,0,sizeof(t));if(GetTypeName(obj,t,sizeof(t),0)&&ContainsInsensitiveA(t,"DSUIMissionMenuController")){g_cachedMissionController=obj;g_cachedMissionControllerTable=table;g_cachedMissionControllerIndex=i;if(outController)*outController=obj;return true;}}
    return false;
}

[[maybe_unused]] static bool ResolveNativeFacilityName(u64 resource,DisplayRow*row){
    if(!IsCanonicalPointer(resource)||!row)return false;u32 id=ReadU32(resource+0x160);u8 key[0x20];MemSet(key,0,sizeof(key));MemCopy(key,&id,4);
    bool called=false;__try{FacilityIdPrepareFn prep=(FacilityIdPrepareFn)(usize)(g_imageBase+RVA_FACILITY_ID_PREPARE);prep(key);u64 root=ReadU64(g_imageBase+RVA_ROOT_HOLDER_GLOBAL);u64 manager=IsCanonicalPointer(root)?ReadU64(root+0xA0):0;if(!IsCanonicalPointer(manager))return false;FacilityIdResolveFn resolve=(FacilityIdResolveFn)(usize)(g_imageBase+RVA_FACILITY_ID_RESOLVE);resolve((void*)(usize)manager,key);called=true;}__except(1){AtomicInc(&g_nativeCallFaults);return false;}
    if(!called)return false;u64 formatted=g_imageBase+RVA_NATIVE_EMPTY_STRING;u32 hash=Crc32CWord(id);NativeFormatFn format=(NativeFormatFn)(usize)(g_imageBase+RVA_NATIVE_FORMAT);bool ok=false;
    __try{format(&formatted,0,g_imageBase+RVA_FACILITY_FORMAT_DATA,hash);WCHAR target[MAX_TEXT_CHARS];MemSet(target,0,sizeof(target));if(CopyNativeUtf8ToWide(formatted,target,MAX_TEXT_CHARS)){CopyW(row->title,MAX_TEXT_CHARS,target);CollapseDuplicatedLocalizedText(row->title);ok=true;}}__except(1){AtomicInc(&g_nativeCallFaults);}
    NativeStringReleaseFn release=(NativeStringReleaseFn)(usize)(g_imageBase+RVA_NATIVE_STRING_RELEASE);__try{release(&formatted);}__except(1){AtomicInc(&g_nativeCallFaults);}return ok;
}

static bool ResolveNativeMissionNames(u64 mission,DisplayRow*row){
    if(!row||!IsCanonicalPointer(mission))return false;u64 controller=0;if(!FindMissionController(&controller))return false;u64 output=controller+MISSION_CONTROLLER_RECORD_OFFSET;
    __atomic_add_fetch(&g_nameCalls,1u,__ATOMIC_ACQ_REL);
    MissionDisplayInitFn init=(MissionDisplayInitFn)(usize)(g_imageBase+RVA_MISSION_DISPLAY_RESET);MissionDisplayBuildFn build=(MissionDisplayBuildFn)(usize)(g_imageBase+RVA_MISSION_DISPLAY_BUILD);bool initOk=false;u8 result=0;
    __try{init((void*)(usize)output);initOk=true;}__except(1){AtomicInc(&g_nativeCallFaults);__atomic_add_fetch(&g_nameFaults,1u,__ATOMIC_ACQ_REL);__atomic_add_fetch(&g_nameInitFaults,1u,__ATOMIC_ACQ_REL);return false;}
    if(!initOk)return false;__atomic_add_fetch(&g_nameInitOk,1u,__ATOMIC_ACQ_REL);bool buildOk=false;
    __try{result=build((void*)(usize)output,mission);buildOk=true;}__except(NameFaultFilter()){AtomicInc(&g_nativeCallFaults);__atomic_add_fetch(&g_nameFaults,1u,__ATOMIC_ACQ_REL);__atomic_add_fetch(&g_nameBuildFaults,1u,__ATOMIC_ACQ_REL);return false;}
    if(!buildOk||!result)return false;__atomic_add_fetch(&g_nameBuildOk,1u,__ATOMIC_ACQ_REL);
    // Copy immediately from the real controller-owned native record.
    u64 primary=ReadU64(output+0x10),fallback=ReadU64(output+0x08),tertiary=ReadU64(output+0x28),quaternary=ReadU64(output+0x30);g_nameLastPrimary=primary;g_nameLastFallback=fallback;g_nameLastTertiary=tertiary;g_nameLastQuaternary=quaternary;
    if(primary||fallback||tertiary||quaternary)__atomic_add_fetch(&g_nameNonEmpty,1u,__ATOMIC_ACQ_REL);
    // The native record fields are: +0x08 resource-derived label,
    // +0x10 recipient/person, +0x28 title and +0x30 description. The
    // resource-derived +0x08 label is the remaining destination candidate.
    if(!CopyNativeUtf8ToWide(fallback,row->title,MAX_TEXT_CHARS))
        if(!CopyNativeUtf8ToWide(primary,row->title,MAX_TEXT_CHARS))
            if(!CopyNativeUtf8ToWide(quaternary,row->title,MAX_TEXT_CHARS))
                if(!CopyNativeUtf8ToWide(tertiary,row->title,MAX_TEXT_CHARS))return false;
    CollapseDuplicatedLocalizedText(row->title);
    row->subtitle[0]=0;row->kind[0]=0;return true;
}

static void MakeMissionRowLabel(DisplayRow*row){
    if(!row)return;
    if(!row->title[0]){CopyW(row->title,MAX_TEXT_CHARS,g_text->orderFallback);}
    row->subtitle[0]=0;
}

static bool BeginMissionScan(const SelectionState*sel){
    if(!sel)return false;MemSet(&g_scan,0,sizeof(g_scan));g_scan.selectionKey=sel->rawHandle;g_scan.facilityId=sel->facilityId;g_scan.handleIndex=sel->handleIndex;g_scan.selectionType=sel->selectionType;AsciiTypeToWide(sel->wrapperType,g_scan.facilityLabel,128);
    u64 globalObj=ReadU64(g_imageBase+RVA_MISSION_GLOBAL);u64 sourceRoot=IsCanonicalPointer(globalObj)?ReadU64(globalObj+MISSION_SOURCE_ROOT_OFFSET):0;u64 list=IsCanonicalPointer(sourceRoot)?sourceRoot+MISSION_SOURCE_LIST_OFFSET:0;s32 count=list?ReadS32(list):-1;u64 data=list?ReadU64(list+8):0;
    if(count<0||count>(s32)MAX_MISSION_SOURCE||!IsCanonicalPointer(data))return false;
    if(count>0&&!SafeRead(data,g_missionPointerScratch,(usize)count*8u))return false;
    g_scan.sourceCount=(u32)count;g_scan.nativeCallFaults=AtomicLoad(&g_nativeCallFaults);g_scan.active=1;return true;
}

static void ProcessMissionScanBatch(void){
    if(!g_scan.active||g_scan.complete)return;u32 stop=g_scan.nextIndex+FILTER_BATCH_SIZE;if(stop>g_scan.sourceCount)stop=g_scan.sourceCount;
    for(u32 i=g_scan.nextIndex;i<stop;++i){u64 mission=g_missionPointerScratch[i];u64 resource=0;s32 dest=-1;u32 missionId=0,missionType=0;if(!MissionAvailableAtFacility(mission,g_scan.facilityId,&resource,&dest,&missionId,&missionType))continue;++g_scan.totalAvailable;if(g_scan.displayedRows<MAX_DISPLAY_ROWS){DisplayRow*row=&g_scan.rows[g_scan.displayedRows++];MemSet(row,0,sizeof(*row));row->mission=mission;row->resource=resource;row->missionId=missionId;row->missionType=missionType;row->destinationId=dest;ResolveNativeMissionNames(mission,row);MakeMissionRowLabel(row);}}
    g_scan.nextIndex=stop;g_scan.nativeCallFaults=AtomicLoad(&g_nativeCallFaults);if(g_scan.nextIndex>=g_scan.sourceCount)g_scan.complete=1;
}

static void CopyScanToSnapshot(DisplaySnapshot*out){
    if(!out)return;out->visible=1;out->selectionKey=g_scan.selectionKey;out->selectionType=g_scan.selectionType;out->handleIndex=g_scan.handleIndex;out->facilityId=g_scan.facilityId;out->totalAvailable=(s32)g_scan.totalAvailable;out->displayedRows=g_scan.displayedRows;out->scannedMissions=g_scan.nextIndex;out->sourceMissions=g_scan.sourceCount;out->nativeCallFaults=g_scan.nativeCallFaults;CopyW(out->facilityLabel,128,g_scan.facilityLabel);for(u32 i=0;i<g_scan.displayedRows&&i<MAX_DISPLAY_ROWS;++i)MemCopy(&out->rows[i],&g_scan.rows[i],sizeof(DisplayRow));
    if(g_scan.complete){if(out->totalAvailable==0)CopyW(out->status,192,g_text->noOrders);else CopyW(out->status,192,g_text->safeTextMode);}
    else{CopyW(out->status,192,g_text->checkingOrders);AppendDecW(out->status,192,g_scan.nextIndex);AppendCharW(out->status,192,L'/');AppendDecW(out->status,192,g_scan.sourceCount);}
}

static void BuildDisplaySnapshot(DisplaySnapshot*out){
    MemSet(out,0,sizeof(*out));u64 now=NowMs();out->tick=now;out->facilityId=-1;SelectionState sel;
    if(!ResolveSelectedFacility(&sel)){g_uiPendingSelection=0;g_uiPendingSince=0;g_uiLastBuiltSelection=0;MemSet(&g_scan,0,sizeof(g_scan));CopyW(out->status,192,g_text->openMapHover);return;}
    if(sel.rawHandle!=g_uiPendingSelection){g_uiPendingSelection=sel.rawHandle;g_uiPendingSince=now;g_uiLastBuiltSelection=0;MemSet(&g_scan,0,sizeof(g_scan));return;}
    if(now-g_uiPendingSince<HOVER_STABLE_MS)return;
    if(!g_scan.active||g_scan.selectionKey!=sel.rawHandle||g_scan.facilityId!=sel.facilityId){if(!BeginMissionScan(&sel)){out->visible=1;out->selectionKey=sel.rawHandle;out->selectionType=sel.selectionType;out->handleIndex=sel.handleIndex;out->facilityId=sel.facilityId;AsciiTypeToWide(sel.wrapperType,out->facilityLabel,128);CopyW(out->status,192,g_text->missionSourceUnavailable);return;}}
    ProcessMissionScanBatch();CopyScanToSnapshot(out);g_uiLastBuiltSelection=sel.rawHandle;g_uiLastBuildTick=now;
}

static LRESULT CALLBACK UiHookProc(int code,WPARAM wParam,LPARAM lParam){if(code==HC_ACTION&&wParam==PM_REMOVE&&lParam){MSG*msg=(MSG*)(usize)lParam;if(msg->message==UI_REQUEST_MESSAGE){msg->message=WM_NULL;DisplaySnapshot local;BuildDisplaySnapshot(&local);u32 next=(AtomicLoad(&g_snapshotWriteGeneration)+1u);local.generation=next;u32 slot=next&1u;MemCopy(&g_snapshotBuffers[slot],&local,sizeof(local));AtomicStore(&g_snapshotWriteGeneration,next);AtomicStore(&g_uiRequestPending,0);}}return pCallNextHookEx?pCallNextHookEx(g_uiHook,code,wParam,lParam):0;}

static DWORD RGBc(u8 r,u8 g,u8 b){return (DWORD)r|((DWORD)g<<8)|((DWORD)b<<16);}
static void DrawOverlay(HWND hwnd){AtomicInc(&g_overlayPaintCount);
    PAINTSTRUCT ps;MemSet(&ps,0,sizeof(ps));HDC dc=pBeginPaint(hwnd,&ps);if(!dc)return;
    RECT client;if(!pGetClientRect(hwnd,&client)){client.left=0;client.top=0;client.right=620;client.bottom=760;}
    pFillRect(dc,&client,g_backgroundBrush);pSetBkMode(dc,TRANSPARENT);
    u32 gen=AtomicLoad(&g_snapshotWriteGeneration);DisplaySnapshot snap;MemSet(&snap,0,sizeof(snap));if(gen)MemCopy(&snap,&g_snapshotBuffers[gen&1u],sizeof(snap));
    int right=(int)client.right-12;if(right<260)right=260;UINT lineAlign=(g_text==&kTextArabic)?(DT_RIGHT|DT_RTLREADING):DT_LEFT;
    pSetTextColor(dc,RGBc(238,241,244));HGDIOBJ old=pSelectObject(dc,g_headingFont);
    WCHAR headingLine[160];CopyW(headingLine,160,g_text->remoteOrders);AppendW(headingLine,160,L"  ·  ");AppendW(headingLine,160,g_text->facility);AppendW(headingLine,160,L": ");AppendSignedW(headingLine,160,snap.facilityId);
    RECT r={12,8,right,32};pDrawTextW(dc,headingLine,-1,&r,lineAlign|DT_TOP|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
    WCHAR countLine[160];CopyW(countLine,160,g_text->available);AppendW(countLine,160,L": ");AppendSignedW(countLine,160,snap.totalAvailable);if(snap.sourceMissions&&snap.scannedMissions<snap.sourceMissions){AppendCharW(countLine,160,L' ');AppendCharW(countLine,160,L'(');AppendDecW(countLine,160,snap.scannedMissions);AppendCharW(countLine,160,L'/');AppendDecW(countLine,160,snap.sourceMissions);AppendCharW(countLine,160,L')');}
    pSelectObject(dc,g_smallFont);pSetTextColor(dc,RGBc(184,202,219));r.left=12;r.top=34;r.right=right;r.bottom=54;pDrawTextW(dc,countLine,-1,&r,lineAlign|DT_TOP|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
    int y=58;int left=12;int gap=10;int width=(right-left-gap)/2;if(width<120)width=120;
    if(snap.displayedRows==0&&snap.status[0]){pSelectObject(dc,g_smallFont);pSetTextColor(dc,RGBc(184,202,219));RECT sr={left,y,right,y+48};pDrawTextW(dc,snap.status,-1,&sr,lineAlign|DT_TOP|DT_WORDBREAK|DT_NOPREFIX);y+=54;}
    else{
        pSelectObject(dc,g_rowFont);pSetTextColor(dc,RGBc(242,244,246));
        u32 shown=snap.displayedRows;if(shown>10)shown=10;u32 rows=(shown+1u)/2u;
        for(u32 i=0;i<shown;++i){u32 col=i/rows;u32 line=i%rows;int x=left+(int)col*(width+gap);int yy=y+(int)line*24;RECT tr={x,yy,x+width,yy+22};pDrawTextW(dc,snap.rows[i].title,-1,&tr,lineAlign|DT_TOP|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);}
        y+=((int)rows)*24;
        if(snap.totalAvailable>(s32)shown){pSetTextColor(dc,RGBc(184,202,219));pSelectObject(dc,g_smallFont);RECT mr={left,y+4,right,y+30};pDrawTextW(dc,g_text->moreOrders,-1,&mr,lineAlign|DT_TOP|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);}
    }
    pSelectObject(dc,old);pEndPaint(hwnd,&ps);
}
static LRESULT CALLBACK OverlayWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){(void)wParam;(void)lParam;if(msg==WM_PAINT){DrawOverlay(hwnd);return 0;}if(msg==WM_NCHITTEST)return HTTRANSPARENT;if(msg==WM_MOUSEACTIVATE)return MA_NOACTIVATE;if(msg==WM_ERASEBKGND)return 1;if(msg==WM_DESTROY){if(pPostQuitMessage)pPostQuitMessage(0);return 0;}return pDefWindowProcW?pDefWindowProcW(hwnd,msg,wParam,lParam):0;}

static bool IsUsableGameWindow(HWND hwnd){if(!hwnd||hwnd==g_overlayWindow||!pIsWindowVisible(hwnd))return false;DWORD pid=0;pGetWindowThreadProcessId(hwnd,&pid);if(pid!=g_pid)return false;RECT rc;if(!pGetClientRect(hwnd,&rc))return false;s64 w=(s64)rc.right-(s64)rc.left,h=(s64)rc.bottom-(s64)rc.top;return w>=640&&h>=360;}
static BOOL CALLBACK FindGameWindowProc(HWND hwnd,LPARAM){if(!IsUsableGameWindow(hwnd))return TRUE;RECT rc;if(!pGetClientRect(hwnd,&rc))return TRUE;u64 w=(u64)((s64)rc.right-(s64)rc.left),h=(u64)((s64)rc.bottom-(s64)rc.top),area=w*h;if(area>g_bestGameWindowArea){g_bestGameWindowArea=area;g_bestGameWindow=hwnd;}return TRUE;}
static bool AcquireGameWindow(void){HWND fg=pGetForegroundWindow?pGetForegroundWindow():0;if(IsUsableGameWindow(fg)){g_gameWindow=fg;DWORD tid=pGetWindowThreadProcessId(fg,0);if(!g_uiHook)g_uiThreadId=tid;return tid!=0;}g_bestGameWindow=0;g_bestGameWindowArea=0;pEnumWindows(FindGameWindowProc,0);if(!g_bestGameWindow)return false;g_gameWindow=g_bestGameWindow;DWORD tid=pGetWindowThreadProcessId(g_gameWindow,0);if(!g_uiHook)g_uiThreadId=tid;return tid!=0;}
static bool LoadUiApis(void){HMODULE u=GetModuleHandleW(L"user32.dll");HMODULE g=GetModuleHandleW(L"gdi32.dll");if(!u||!g)return false;
#define LOADU(name) p##name=(PFN_##name)GetProcAddress(u,#name);if(!p##name)return false
#define LOADG(name) p##name=(PFN_##name)GetProcAddress(g,#name);if(!p##name)return false
    LOADU(EnumWindows);LOADU(GetWindowThreadProcessId);LOADU(IsWindowVisible);LOADU(GetForegroundWindow);LOADU(GetClientRect);LOADU(ClientToScreen);LOADU(SetWindowsHookExW);LOADU(UnhookWindowsHookEx);LOADU(CallNextHookEx);LOADU(PostThreadMessageW);LOADU(RegisterClassExW);LOADU(CreateWindowExW);LOADU(DefWindowProcW);LOADU(ShowWindow);LOADU(SetWindowPos);LOADU(InvalidateRect);LOADU(UpdateWindow);LOADU(PeekMessageW);LOADU(TranslateMessage);LOADU(DispatchMessageW);LOADU(BeginPaint);LOADU(EndPaint);LOADU(FillRect);LOADU(DrawTextW);LOADU(SetLayeredWindowAttributes);LOADU(DestroyWindow);LOADU(PostQuitMessage);
    LOADG(CreateSolidBrush);LOADG(CreateFontW);LOADG(SelectObject);LOADG(SetTextColor);LOADG(SetBkMode);LOADG(DeleteObject);
#undef LOADU
#undef LOADG
    return true;}

static bool VerifyBytesAt(u64 rva,const u8*expected,u32 size){u8 actual[32];if(size>sizeof(actual)||!SafeRead(g_imageBase+rva,actual,size))return false;for(u32 i=0;i<size;++i)if(actual[i]!=expected[i])return false;return true;}
static bool VerifyExecutable(void){WCHAR exe[1024];MemSet(exe,0,sizeof(exe));GetModuleFileNameW((HMODULE)(usize)g_imageBase,exe,1023);HANDLE f=CreateFileW(exe,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;LARGE_INTEGER sz;sz.quad=0;BOOL ok=GetFileSizeEx(f,&sz);CloseHandle(f);if(!ok||sz.quad!=EXPECTED_EXE_SIZE)return false;const u8 mapSig[]={0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xEC,0x20};const u8 type6Sig[]={0x48,0x83,0xEC,0x28,0xE8};const u8 extraSig[]={0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9};return VerifyBytesAt(RVA_VERIFY_MAP_ACCEPT,mapSig,sizeof(mapSig))&&VerifyBytesAt(RVA_TYPE6_CONTENT,type6Sig,sizeof(type6Sig))&&VerifyBytesAt(RVA_MISSION_AVAILABILITY_EXTRA,extraSig,sizeof(extraSig));}

static bool CreateOverlay(void){WNDCLASSEXW wc;MemSet(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=OverlayWndProc;wc.hInstance=g_self;wc.lpszClassName=L"DS2RemoteOrdersOverlayClassV020";if(!pRegisterClassExW(&wc))return false;DWORD exStyle=WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;g_overlayWindow=pCreateWindowExW(exStyle,wc.lpszClassName,L"DS2 Remote Orders Overlay v0.2.0",WS_POPUP,0,0,420,360,g_gameWindow,0,g_self,0);if(!g_overlayWindow)return false;g_backgroundBrush=pCreateSolidBrush(RGBc(17,22,28));g_headingFont=pCreateFontW(-18,0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");g_rowFont=pCreateFontW(-14,0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");g_smallFont=pCreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");return g_backgroundBrush&&g_headingFont&&g_rowFont&&g_smallFont;}
static void UpdateOverlayPositionAndVisibility(void){if(!g_overlayWindow)return;AcquireGameWindow();if(!g_gameWindow)return;u32 gen=AtomicLoad(&g_snapshotWriteGeneration);DisplaySnapshot snap;MemSet(&snap,0,sizeof(snap));if(gen)MemCopy(&snap,&g_snapshotBuffers[gen&1u],sizeof(snap));if(!snap.visible){pShowWindow(g_overlayWindow,SW_HIDE);return;}RECT rc;if(!pGetClientRect(g_gameWindow,&rc)){pShowWindow(g_overlayWindow,SW_HIDE);return;}POINT pt;pt.x=0;pt.y=0;if(!pClientToScreen(g_gameWindow,&pt)){pShowWindow(g_overlayWindow,SW_HIDE);return;}int clientW=(int)(rc.right-rc.left);int clientH=(int)(rc.bottom-rc.top);g_lastGameX=pt.x;g_lastGameY=pt.y;g_lastGameW=clientW;g_lastGameH=clientH;int width=420;if(clientW<680)width=clientW-28;if(width<260)width=260;int height=66+(int)((snap.displayedRows>10?10:snap.displayedRows+1)/2u)*24;if(height>clientH-100)height=clientH-100;if(height<120)height=120;int x=pt.x+clientW-width-28;int y=pt.y+88;g_lastSetWindowPosOk=pSetWindowPos(g_overlayWindow,HWND_TOPMOST,x,y,width,height,SWP_NOACTIVATE|SWP_NOSENDCHANGING|SWP_SHOWWINDOW)?1u:0u;pShowWindow(g_overlayWindow,SW_SHOWNOACTIVATE);pInvalidateRect(g_overlayWindow,0,TRUE);pUpdateWindow(g_overlayWindow);}

static DWORD WINAPI WorkerThread(LPVOID){EarlyLog("WORKER_START\\r\\n");WriteStage(L"NUL","WORKER_START\\r\\n");g_process=GetCurrentProcess();g_pid=GetCurrentProcessId();g_imageBase=(u64)(usize)GetModuleHandleW(0);WriteStage(L"NUL","AFTER_IMAGE\\r\\n");if(!g_process||!g_imageBase)return 1;u32 peOff=ReadU32(g_imageBase+0x3C);g_imageSize=ReadU32(g_imageBase+peOff+0x50);g_imageEnd=g_imageBase+g_imageSize;DetermineModuleDirectory();InitializeLocalization();WriteTextFile(g_logFile,"DS2 Remote Orders Overlay v0.2.0 loaded.\r\n");if(!VerifyExecutable()){WriteStage(L"NUL","VERIFY_FAIL\\r\\n");WriteTextFile(g_logFile,"Unsupported DS2.exe build. Mod disabled.\r\n");return 2;}WriteStage(L"NUL","VERIFY_OK\\r\\n");if(!LoadUiApis()){WriteTextFile(g_logFile,"Could not resolve USER32/GDI32 APIs. Mod disabled.\r\n");return 3;}for(u32 i=0;i<120&&!g_gameWindow;++i){AcquireGameWindow();if(!g_gameWindow)Sleep(250);}if(!g_gameWindow||!g_uiThreadId){WriteTextFile(g_logFile,"DS2 game window/UI thread not found.\r\n");return 4;}g_uiHook=pSetWindowsHookExW(WH_GETMESSAGE,UiHookProc,g_self,g_uiThreadId);if(!g_uiHook){WriteTextFile(g_logFile,"UI-thread message hook could not be installed.\r\n");return 5;}if(!CreateOverlay()){WriteTextFile(g_logFile,"Overlay window could not be created.\r\n");pUnhookWindowsHookEx(g_uiHook);g_uiHook=0;return 6;}u64 lastRequest=0,lastPosition=0;u32 observedGen=0;MSG msg;MemSet(&msg,0,sizeof(msg));for(;;){while(pPeekMessageW(&msg,0,0,0,PM_REMOVE)){pTranslateMessage(&msg);pDispatchMessageW(&msg);}u64 now=NowMs();if(now-lastRequest>=UI_REFRESH_MS&&!AtomicLoad(&g_uiRequestPending)){AtomicStore(&g_uiRequestPending,1);if(!pPostThreadMessageW(g_uiThreadId,UI_REQUEST_MESSAGE,0,0))AtomicStore(&g_uiRequestPending,0);lastRequest=now;}u32 gen=AtomicLoad(&g_snapshotWriteGeneration);if(gen!=observedGen){observedGen=gen;DisplaySnapshot snap;MemCopy(&snap,&g_snapshotBuffers[gen&1u],sizeof(snap));if(snap.visible&&(snap.selectionKey!=g_lastLoggedSelection||snap.totalAvailable!=g_lastLoggedCount||snap.scannedMissions!=g_lastLoggedScanned)){g_lastLoggedSelection=snap.selectionKey;g_lastLoggedCount=snap.totalAvailable;g_lastLoggedScanned=snap.scannedMissions;LogSnapshot(&snap);}UpdateOverlayPositionAndVisibility();}else if(now-lastPosition>=1000){UpdateOverlayPositionAndVisibility();lastPosition=now;}Sleep(16);}return 0;}

static BOOL StartMod(void){WriteStage(L"NUL","STARTMOD_ENTER\\r\\n");if(!AtomicStartOnce()){WriteStage(L"NUL","STARTMOD_ALREADY\\r\\n");return TRUE;}WriteStage(L"NUL","AFTER_ATOMIC\\r\\n");HANDLE t=CreateThread(0,0,WorkerThread,0,0,0);WriteStage(L"NUL",t?"AFTER_CREATE_OK\\r\\n":"AFTER_CREATE_FAIL\\r\\n");if(t)CloseHandle(t);return t?TRUE:FALSE;}
extern "C" __declspec(dllexport) BOOL WINAPI InitializeASI(void){EarlyLog("INITIALIZE_ASI\\r\\n");return TRUE;}
extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){if(reason==DLL_PROCESS_ATTACH){g_self=module;StartMod();}return TRUE;}
