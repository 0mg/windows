CL = gcc $(@:.exe=.c) -o $@ -mwindows -s -nostartfiles
DL = gcc $(@:.dll=.c) -o $@ -shared -s

.PHONY: all
all: dblckspd.exe dispoff.exe suspend.exe sustimer.exe

dblckspd.exe:
	$(CL)

dispoff.exe:
	$(CL)

suspend.exe:
	$(CL) -lpowrprof

sustimer.exe:
	$(CL) -lole32 -lpowrprof -lwinmm
