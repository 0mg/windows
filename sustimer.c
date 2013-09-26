#define UNICODE
#include <ctype.h>
#include <time.h>
#include <windows.h>
/* #include <shobjidl.h>
  gcc's shobjidl.h:
    STDMETHOD(SetProgressValue)(THIS_ ULONGLONG,ULONGLONG) PURE;
    > ..is incorrect definition
  correct:
    STDMETHOD(SetProgressValue)(THIS_ HWND,ULONGLONG,ULONGLONG) PURE;
*/
#undef INTERFACE
#define INTERFACE ITaskbarList3
DECLARE_INTERFACE(INTERFACE) {
  void *qa[2];
  STDMETHOD_(ULONG, Release)(THIS);
  void *ha[6];
  STDMETHOD(SetProgressValue)(THIS_ HWND, ULONGLONG, ULONGLONG);
};
#undef INTERFACE
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#define WND_WIDTH 320
#define WND_HEIGHT 240
#define WND_BG RGB(30, 90, 200)
#define TEXT_COLOR RGB(255, 255, 255)
#define PRG_BORDER 2
#define WTIMER_ID 0
#define WTIMER_OUT 500
#define ATIMEOUT_DEFAULT 60

void __start__() {
  // program will start from here if `gcc -nostartfiles`
  ExitProcess(WinMain(GetModuleHandle(NULL), 0, "", 0));
}

BOOL atimeover;

void suspendSystem() {
  HMODULE lib = LoadLibrary(TEXT("powrprof.dll"));
  FARPROC SetSuspendState = GetProcAddress(lib, "SetSuspendState");
  SetSuspendState(FALSE, FALSE, FALSE);
  FreeLibrary(lib);
}

void startMouseTrack(HWND hwnd) {
  TRACKMOUSEEVENT tme;
  tme.cbSize = sizeof(TRACKMOUSEEVENT);
  tme.dwFlags = TME_LEAVE;
  tme.hwndTrack = hwnd;
  tme.dwHoverTime = 1;
  TrackMouseEvent(&tme);
}

void setTBProgress(HWND hwnd, int now, int max) {
  ITaskbarList3* ppv;
  CLSID tbclsid;
  IID tbiid;
  HRESULT result;
  CLSIDFromString(OLESTR("{56FDF344-FD6D-11d0-958A-006097C9A090}"), &tbclsid);
  IIDFromString(OLESTR("{EA1AFB91-9E28-4B86-90E9-9E9F8A5EEFAF}"), &tbiid);
  CoInitialize(NULL);
  result = CoCreateInstance(&tbclsid, NULL, CLSCTX_ALL, &tbiid, (LPVOID *)&ppv);
  if (result == S_OK) {
    ppv->lpVtbl->SetProgressValue(ppv, hwnd, now, max);
    ppv->lpVtbl->Release(ppv);
  }
  CoUninitialize();
}

void quitApp(HWND hwnd) {
  PostMessage(hwnd, WM_CLOSE, 0, 0);
  PostQuitMessage(0);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  static BOOL hover;
  static RECT canvas;
  static RECT progvas;

  static struct {
    int out;
    int rest;
    int fixed;
    int fixedPrev;
  } atimer;

  static struct {
    HFONT font;
    HPEN pen;
    LOGBRUSH logbrush;
    HBRUSH brush;
    TCHAR text[8];
  } counter, closer, logo, progress, progbar;

  if (!atimer.out) {
    LPWSTR *cmdarr;
    int cmdlen;
    cmdarr = CommandLineToArgvW(GetCommandLineW(), &cmdlen);
    atimer.out = (
      cmdlen >= 2 && iswdigit(cmdarr[1][0]) ?
        _wtoi(cmdarr[1]) : ATIMEOUT_DEFAULT
    ) * 1000;
  }

  atimer.rest = atimer.out - clock();
  atimer.fixed = atimer.rest / 1000 + 1;

  if (atimer.rest <= 0) {
    quitApp(hwnd);
    atimeover = TRUE;
  } else switch (msg) {
  case WM_CREATE:
    SetTimer(hwnd, WTIMER_ID, WTIMER_OUT, NULL);
    // init pens
    counter.font = CreateFont(90, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
      DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
    closer.pen = CreatePen(PS_SOLID, 10, TEXT_COLOR);
    logo.font = counter.font;
    closer.brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    closer.font = CreateFont(90, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
      SYMBOL_CHARSET, 0, 0, 0, 0, NULL);
    progress.pen = CreatePen(PS_SOLID, PRG_BORDER, TEXT_COLOR);
    progress.brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    progbar.pen = (HPEN)GetStockObject(NULL_PEN);
    progbar.brush = (HBRUSH)CreateSolidBrush(TEXT_COLOR);
    // Set: client area
    GetClientRect(hwnd, &canvas);
    // Set: progressbar area
    progvas.left = canvas.left + 20;
    progvas.top = canvas.bottom - 40;
    progvas.right = canvas.right - 20;
    progvas.bottom = canvas.bottom - 20;
    // repaint
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
  case WM_TIMER:
    // repaint
    if (atimer.fixed != atimer.fixedPrev) {
      InvalidateRect(hwnd, NULL, TRUE);
      atimer.fixedPrev = atimer.fixed;
    } else {
      InvalidateRect(hwnd, &progvas, TRUE);
    }
    return 0;
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, TEXT_COLOR);
    // logo
    SelectObject(hdc, logo.font);
    DrawText(hdc, TEXT("\u263e"), -1, &canvas,
      DT_LEFT);
    // count down
    SelectObject(hdc, counter.font);
    wsprintf(counter.text, TEXT("%d"), atimer.fixed);
    DrawText(hdc, counter.text, -1, &canvas,
      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // progress frame
    SelectObject(hdc, progress.pen);
    SelectObject(hdc, progress.brush);
    Rectangle(hdc,
      progvas.left,
      progvas.top,
      progvas.right,
      progvas.bottom);
    // progress content
    SelectObject(hdc, progbar.pen);
    SelectObject(hdc, progbar.brush);
    Rectangle(hdc,
      progvas.left,
      progvas.top,
      progvas.left +
        (progvas.right - progvas.left) / ((float)atimer.out / atimer.rest),
      progvas.bottom);
    // close charm
    if (hover) {
      SelectObject(hdc, closer.pen);
      SelectObject(hdc, closer.brush);
      SelectObject(hdc, closer.font);
      Rectangle(hdc, canvas.left, canvas.top, canvas.right, canvas.bottom);
      DrawText(hdc, TEXT("x"), -1, &canvas, DT_RIGHT);
    }
    EndPaint(hwnd, &ps);
    setTBProgress(hwnd, atimer.rest, atimer.out);
    return 0;
  }
  case WM_DESTROY:
    quitApp(hwnd);
    return 0;
  case WM_MOUSELEAVE:
    hover = FALSE;
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
  case WM_MOUSEMOVE:
    if (!hover) {
      hover = TRUE;
      startMouseTrack(hwnd);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;
  case WM_LBUTTONUP:
    quitApp(hwnd);
    return 0;
  case WM_CHAR:
    switch (wp) {
    case VK_ESCAPE:
    case VK_SPACE:
    case VK_RETURN:
      quitApp(hwnd);
      break;
    }
    return 0;
  }
  return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  WNDCLASS wc;
  HWND hwnd;
  MSG msg;

  // Main Window: Settings
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = MainWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hi;
  wc.hIcon =
    (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
  wc.hCursor =
    (HCURSOR)LoadImage(NULL, IDC_HAND, IMAGE_CURSOR, 0, 0, LR_SHARED);
  wc.hbrBackground = CreateSolidBrush(WND_BG);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = TEXT("Suspend PC Timer Window Class");

  // Main Window: Create, Show
  hwnd = CreateWindowEx(
    WS_EX_TOPMOST,
    (LPCTSTR)MAKELONG(RegisterClass(&wc), 0), TEXT("Suspend PC Timer"),
    WS_VISIBLE | WS_SYSMENU,
    GetSystemMetrics(SM_CXSCREEN) / 2 - WND_WIDTH / 2,
    GetSystemMetrics(SM_CYSCREEN) / 2 - WND_HEIGHT / 2,
    WND_WIDTH,
    WND_HEIGHT,
    NULL, NULL, hi, NULL
  );
  if (hwnd == NULL) return 1;

  // While msg.message != WM_QUIT
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  // Suspend if time over
  if (atimeover) suspendSystem();
  return msg.wParam;
}
