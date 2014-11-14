@echo off
setlocal
set path=%path%;"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin"
set include="C:\Program Files (x86)\Windows Kits\8.1\Include\um";"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\crt\src";"C:\Program Files (x86)\Windows Kits\8.1\Include\shared"
set lib="C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86";"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib"
nmake /A /f Makefile-vc
endlocal
