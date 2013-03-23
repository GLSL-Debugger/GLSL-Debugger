CC = cl.exe
LINK = link.exe
LIBRARIAN = lib.exe
COPYDEST = ..\..\Debug
CFLAGS = /I .. /I "..\detoursExpress\include" /I "..\..\GLSLCompiler\glslang\OSDependent\Windows" /TC
CFLAGS = $(CFLAGS) /D "_DEBUG" /MDd /Fdvc80.pdb
LDFLAGS = /LIBPATH:../../Release /LIBPATH:"..\detoursExpress\lib" /LTCG /DLL /SUBSYSTEM:WINDOWS /NODEFAULTLIB:msvcrt
LDFLAGS = $(LDFLAGS) /DEBUG

all: libfunctionList.lib types debuglib.dll copy

copy: debuglib.dll
	copy debuglib.dll $(COPYDEST)
	
debuglib.dll: libglsldebug.obj streamRecorder.obj streamRecording.obj \
                 replayFunction.obj glstate.obj readback.obj shader.obj error.obj \
                 memory.obj hooks.obj functionList.obj
	$(LINK) $(LDFLAGS) /OUT:$@ $** /DEF:"debuglib.def" opengl32.lib detours.lib detoured.lib glenumerants.lib
	
debuglib.def: genTrampolines.pl ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genTrampolines.pl exp > $@

hooks.obj: hooks.c functionHooks.inc getProcAddressHook.inc trampolines.inc

libglsldebug.obj: trampolines.h debuglib.def

types: functionPointerTypes.inc

preload_hooks: functionHooks.inc getProcAddressHook.inc

libfunctionList.lib: functionList.obj
	$(LIBRARIAN) /OUT:$@ $**

getProcAddressHook.inc: genGetProcAddressHook.pl \
                      ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genGetProcAddressHook.pl > getProcAddressHook.inc

trampolines.h: genTrampolines.pl argumentListTools.pl\
	                      ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	echo #ifndef __TRAMPOLINES_H > $@
	echo #define __TRAMPOLINES_H >> $@
	perl genTrampolines.pl decl >> $@
	echo void initExtensionTrampolines(); >> $@
	echo void attachTrampolines(); >> $@
	echo void detachTrampolines(); >> $@
	echo #endif /* __TRAMPOLINES_H */ >> $@

trampolines.inc: genTrampolines.pl argumentListTools.pl\
	                      ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genTrampolines.pl def > $@
	
replayFunction.c: genReplayFunc.pl argumentListTools.pl ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genReplayFunc.pl > replayFunction.c

functionList.c: genFunctionList.pl argumentListTools.pl \
                  ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genFunctionList.pl > functionList.c

functionHooks.inc: genFunctionHooks.pl argumentListTools.pl \
                   functionsAllowedInBeginEnd.pl \
                   ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genFunctionHooks.pl > functionHooks.inc

functionsAllowedInBeginEnd.pl: beginEndFunctionTest.exe
	echo @allowedInBeginEnd = ( > functionsAllowedInBeginEnd.pl
	beginEndFunctionTest.exe >> functionsAllowedInBeginEnd.pl
	echo ); >> functionsAllowedInBeginEnd.pl

beginEndFunctionTest.exe: beginEndFunctionTest.obj
	$(LINK) /OUT:$@ $** OPENGL32.lib GLU32.lib glut32.lib

beginEndFunctionTest.c: genBeginEndFunctionTest.pl argumentListTools.pl \
                        functionPointerTypes.inc ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genBeginEndFunctionTest.pl > beginEndFunctionTest.c

functionPointerTypes.inc: genFunctionPointerTypes.pl argumentListTools.pl\
	                      ../GL/gl.h ../GL/glext.h ../GL/wglext.h ../GL/WinGDI.h
	perl genFunctionPointerTypes.pl > functionPointerTypes.inc

clean:
	del *.obj *.lib *.inc *.dll replayFunction.c beginEndFunctionTest.c \
	beginEndFunctionTest.exe functionsAllowedInBeginEnd.pl functionList.c trampolines.h 

tags:
	ctags --recurse=yes *.[ch]
	
	
