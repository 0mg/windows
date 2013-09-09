#include <windows.h>
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  HMODULE lib = LoadLibrary("powrprof.dll");
  FARPROC SetSuspendState = GetProcAddress(lib, "SetSuspendState");
  SetSuspendState(FALSE, FALSE, FALSE);
  return 0;
}
