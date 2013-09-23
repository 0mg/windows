## how to compile

use [gcc](http://gcc.gnu.org/) on command line.

```
gcc *.c
```

add `-mwindows` option to make Windows GUI **(non-console)** application.

```
gcc -mwindows *.c
```

### other options

* `-DUNICODE` for utf-8 source code.
* `-s` to decrease file size.
* `-nostartfiles` to make tiny file. (but it may not work well)

## without compile

* [EXE file archives](https://bitbucket.org/0mg/windows/downloads/)

## another compile

* Visual Studio: `cl *.c /MD`
