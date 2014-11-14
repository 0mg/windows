#define UNICODE
#include <windows.h>
#pragma comment(lib, "user32.lib")

__declspec(dllexport) void SetSuspendState() {
  MessageBox(NULL, TEXT("powrprof!!"), TEXT(""), MB_OK);
}
