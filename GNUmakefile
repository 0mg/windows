.PHONY: all
all: dblckspd.exe dispoff.exe suspend.exe sustimer.exe

.SUFFIXES: .c .exe

.c.exe:
	$(CC) $*.c -o $@ -mwindows -s -nostartfiles -lpowrprof -lole32
