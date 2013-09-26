## how to compile

use [gcc](http://gcc.gnu.org/) on command line.

```
gcc *.c -mwindows -lpowrprof -lole32
```

### important options

* `-mwindows` to make Windows GUI **(non-console)** application.
* `-l*` to link library
  * -lpowrprof = link "libpowrprof.a" to use SetSuspendState(..).

### other options

* `-DUNICODE` for utf-8 source code.
* `-s` to decrease file size.
* `-nostartfiles` to make tiny file. (but it may not work well)

## without compile

* [EXE file archives](https://bitbucket.org/0mg/windows/downloads/)

## another compile

* Visual Studio: `cl *.c /MD`
