# Library names
LIBS = kernel32 shell32 user32 gdi32 ole32 powrprof

# GCC
GCCFLAGS = $(AR:%=-o $@ -mwindows -nostartfiles -s ${LIBS:%=-l%})

# VC
TEMP1 = $(LIBS) %
VCLIBS = $(TEMP1: =.lib )
TEMP2 = /MD /link /ENTRY:__start__ $(VCLIBS)
VCFLAGS = $(TEMP2:%=)

# GCC or VC
CFLAGS = $(GCCFLAGS) $(VCFLAGS)

# main
.PHONY: all clean
all: dblckspd.exe dispoff.exe suspend.exe

.SUFFIXES: .c .exe

.c.exe:
	$(CC) $< $(CFLAGS)

clean:
	del *.obj
