#define UNICODE
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#define WND_CLASS TEXT("Suspend PC Timer Window Class")
#define WND_WIDTH 320
#define WND_HEIGHT 240
#define WND_BG RGB(30, 90, 200)
#define WTIMER_ID 0
#define WTIMER_OUT 500
#define ATIMEOUT_DEFAULT 60

void __start__() {
  // program will start from here if `gcc -nostartfiles`
  ExitProcess(WinMain(GetModuleHandle(NULL), 0, "", 0));
}

struct {
  int atimeout;
} gdata;

void suspendSystem() {
  HMODULE lib = LoadLibrary(TEXT("powrprof.dll"));
  FARPROC SetSuspendState = GetProcAddress(lib, "SetSuspendState");
  SetSuspendState(FALSE, FALSE, FALSE);
  FreeLibrary(lib);
}
void nop() {}
void (*onATimeout)() = suspendSystem;

void startMouseTrack(HWND hwnd) {
  TRACKMOUSEEVENT tme;
  tme.cbSize = sizeof(TRACKMOUSEEVENT);
  tme.dwFlags = TME_LEAVE;
  tme.hwndTrack = hwnd;
  tme.dwHoverTime = 1;
  TrackMouseEvent(&tme);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  static BOOL hover;

  static struct {
    int rest;
    int fixed;
    int fixedPrev;
    BOOL over;
  } atimer;

  static struct {
    HFONT font;
    HPEN pen;
    LOGBRUSH logbrush;
    HBRUSH brush;
    TCHAR text[8];
  } counter, closer;

  void initPainter() {
    counter.pen = CreatePen(PS_SOLID, 1, WND_BG);
    counter.brush = CreateSolidBrush(WND_BG);
    counter.font = CreateFont(90, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
      DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
    closer.pen = CreatePen(PS_SOLID, 10, RGB(255, 255, 255));
    closer.brush = GetStockObject(NULL_BRUSH);
    closer.font = CreateFont(90, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
      SYMBOL_CHARSET, 0, 0, 0, 0, NULL);
  }

  void freePainter() {
    DeleteObject(counter.pen);
    DeleteObject(counter.brush);
    DeleteObject(counter.font);
    DeleteObject(closer.pen);
    DeleteObject(closer.brush);
    DeleteObject(closer.font);
  }

  void paintContent() {
    RECT canvas;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    GetClientRect(hwnd, &canvas);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, counter.pen);
    SelectObject(hdc, counter.brush);
    SelectObject(hdc, counter.font);
    SetTextColor(hdc, RGB(255, 255, 255));
    Rectangle(hdc, canvas.left, canvas.top, canvas.right, canvas.bottom);
    wsprintf(counter.text, TEXT("%d"), atimer.fixed);
    DrawText(hdc, counter.text, -1, &canvas,
      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (hover) {
      // ウィンドウマウスホバー時
      SelectObject(hdc, closer.pen);
      SelectObject(hdc, closer.brush);
      SelectObject(hdc, closer.font);
      SetTextColor(hdc, RGB(255, 255, 255));
      Rectangle(hdc, canvas.left, canvas.top, canvas.right, canvas.bottom);
      DrawText(hdc, TEXT("x"), -1, &canvas, DT_RIGHT);
    }
    EndPaint(hwnd, &ps);
  }

  void repaint() {
    if (atimer.fixed != atimer.fixedPrev) {
      InvalidateRect(hwnd, NULL, TRUE);
      atimer.fixedPrev = atimer.fixed;
    }
  }

  void quitApp(HWND hwnd) {
    DestroyWindow(hwnd);
    freePainter();
    PostQuitMessage(0);
  }

  atimer.rest = gdata.atimeout - clock();
  atimer.fixed = atimer.rest / 1000 + 1;

  if (atimer.over) {
    quitApp(hwnd);
  } else if (atimer.rest <= 0) {
    atimer.over = TRUE;
    KillTimer(hwnd, WTIMER_ID);
    quitApp(hwnd);
    onATimeout(hwnd);
    onATimeout = nop;
  }

  switch (msg) {
  case WM_CREATE:
    SetTimer(hwnd, WTIMER_ID, WTIMER_OUT, NULL);
    initPainter();
    repaint();
    return 0;
  case WM_TIMER:
    repaint();
    return 0;
  case WM_PAINT:
    paintContent();
    return 0;
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
  MSG msg;
  WNDCLASS wc;

  // タイムアウト(msec) 設定
  gdata.atimeout = (isdigit(cl[0]) ? atoi(cl) : ATIMEOUT_DEFAULT) * 1000;

  // メインウィンドウ 設定
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = MainWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hi;
  wc.hIcon = LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
  wc.hCursor = LoadImage(NULL, IDC_HAND, IMAGE_CURSOR, 0, 0, LR_SHARED);
  wc.hbrBackground = GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = WND_CLASS;
  RegisterClass(&wc);

  // メインウィンドウ 生成・表示
  CreateWindowEx(
    WS_EX_TOPMOST,
    WND_CLASS, TEXT("Suspend PC Timer"),
    WS_VISIBLE | WS_SYSMENU,
    GetSystemMetrics(SM_CXSCREEN) / 2 - WND_WIDTH / 2,
    GetSystemMetrics(SM_CYSCREEN) / 2 - WND_HEIGHT / 2,
    WND_WIDTH,
    WND_HEIGHT,
    NULL, NULL, hi, NULL
  );

  // While msg.message != WM_QUIT
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}
