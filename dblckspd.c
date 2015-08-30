#define UNICODE
#include <windows.h>
void __start__() {
  // program will start from here if `gcc -nostartfiles`
  ExitProcess(WinMain(0, 0, NULL, 0));
}
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  int argc = 0, ms = 0;
  LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argc > 1) {
    LPWSTR ts = args[1];
    while (*ts) {
      ms = (ms * 10) + (*ts++ - '0');
    }
  }
  SystemParametersInfo(SPI_SETDOUBLECLICKTIME,
    ms ? ms : 200, NULL,
    SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
  return 0;
}
