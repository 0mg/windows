#define UNICODE
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#include <windows.h>
void __start__() {
  // program will start from here if `gcc -nostartfiles`
  ExitProcess(WinMain(0, 0, NULL, 0));
}
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  int n, ms = _wtoi(CommandLineToArgvW(GetCommandLineW(), &n)[1]);
  SystemParametersInfo(SPI_SETDOUBLECLICKTIME,
    ms ? ms : 200, NULL,
    SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
  return 0;
}