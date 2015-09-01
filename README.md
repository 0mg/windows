## this software is

simple applications for Windows.

* **dblckspd**

change mouse-DoubleClickSpeed to 200ms. command `dblckspd.exe 400` to 400ms.

* **dispoff**

turn PC display off.

* **suspend**

suspend PC.

### and

**sustimer** has gone to [other project](https://github.com/0mg/sustimer).

## how to compile

use [gcc](http://gcc.gnu.org/) on command line.

```
mingw32-make
```

or

```
gcc *.c -mwindows -lole32 -lpowrprof
```

### important options

* `-mwindows` to make Windows GUI **(non-console)** application.
* `-l*` to link library
  * -lpowrprof = link "libpowrprof.a" to use SetSuspendState(..).

### other options

* `-DUNICODE` for utf-8 source code.
* `-s` to decrease file size.
* `-nostartfiles` to make more tiny. (but it may not work well)

## without compile

* [Released EXEs](https://github.com/0mg/windows/releases)
* [EXE old archives](https://bitbucket.org/0mg/windows/downloads/)

## another compile

### Visual Studio

```
nmake
```

or

```
cl *.c /MD /link /ENTRY:__start__ kernel32.lib shell32.lib user32.lib gdi32.lib ole32.lib powrprof.lib
```
