#define UNICODE
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#include <windows.h>
void __start__() {
  // program will start from here if `gcc -nostartfiles`
  ExitProcess(WinMain(0, 0, NULL, 0));
}
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  SendNotifyMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
  return 0;
}
