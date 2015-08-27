#define UNICODE
#include <windows.h>
#include <powrprof.h>
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "powrprof.lib")
void __start__() {
  // program will start from here if `gcc -nostartfiles`
  ExitProcess(WinMain(0, 0, NULL, 0));
}
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  SetSuspendState(FALSE, FALSE, FALSE);
  return 0;
}
