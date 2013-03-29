include ../../common.mk

CFLAGS += -I.. -I../utils
#LDFLAGS += -ldl -lGL -lpthread
LDFLAGS += -ldl -fOpenGL -lpthread

all: libfunctionList.a types libglsldebug-$(POSTFIX).so libdlsym-$(POSTFIX).so 

libglsldebug-$(POSTFIX).so: libglsldebug.o streamRecorder.o streamRecording.o \
                 replayFunction.o glstate.o readback.o shader.o error.o \
                 memory.o hooks.o functionList.o queries.o preExecution.o \
				 postExecution.o ../utils/libutils.a ../glenumerants/libglenumerants.a
	#$(CC) -shared -o $@ -Wl,-soname,$@ $^ $(LDFLAGS)
	$(CC) -shared -o $@ -Wl $^ $(LDFLAGS)
	mkdir -p ../libs
	cp $@ ../libs/

libdlsym-$(POSTFIX).so: libdlsym.o
	#$(CC) -shared -o $@ -Wl,-soname,$@ $^ -ldl $(LDFLAGS)
	$(CC) -shared -o $@ -Wl $^ -ldl $(LDFLAGS)
	mkdir -p ../libs
	cp $@ ../libs/

hooks.o: hooks.c functionHooks.inc getProcAddressHook.inc

types: functionPointerTypes.inc

preload_hooks: functionHooks.inc getProcAddressHook.inc

libfunctionList.a: functionList.o
	$(AR) $(ARFLAGS) $@ $^
	-$(RANLIB) $@

functionHooks.inc: genFunctionHooks.pl argumentListTools.pl \
                   functionsAllowedInBeginEnd.pl prePostExecuteList.pl \
				   justCopyPointersList.pl \
                   ../GL/gl.h ../GL/glext.h ../GL/glx.h ../GL/glxext.h
	perl genFunctionHooks.pl > functionHooks.inc

getProcAddressHook.inc: genGetProcAddressHook.pl \
                      ../GL/gl.h ../GL/glext.h ../GL/glx.h ../GL/glxext.h
	perl genGetProcAddressHook.pl > getProcAddressHook.inc

functionPointerTypes.inc: genFunctionPointerTypes.pl argumentListTools.pl\
	                      ../GL/gl.h ../GL/glext.h ../GL/glx.h ../GL/glxext.h
	perl genFunctionPointerTypes.pl > functionPointerTypes.inc
	
replayFunction.c: genReplayFunc.pl argumentListTools.pl \
                  justCopyPointersList.pl ../GL/gl.h ../GL/glext.h
	perl genReplayFunc.pl > replayFunction.c

functionList.c: genFunctionList.pl argumentListTools.pl \
                  ../GL/gl.h ../GL/glext.h ../GL/glx.h ../GL/glxext.h
	perl genFunctionList.pl > functionList.c

# merge available lists of functions allowed in begin-end-block
functionsAllowedInBeginEnd.pl: mergeAllowedInBeginEndList.pl
	perl mergeAllowedInBeginEndList.pl > functionsAllowedInBeginEnd.pl 

FUNCTIONSALLOWEDINBEGINEND=functionsAllowedInBeginEnd.pl-`glxinfo | \
						   grep "OpenGL renderer string" | \
						   sed 's/OpenGL renderer string: //' | \
						   sed 's/ \|\/\|(\|)/_/g'`-`glxinfo | \
						   grep "OpenGL version string" | \
						   sed 's/OpenGL version string: //' | \
						   sed 's/ \|\/\|(\|)/_/g'`
						   
# use this target to regenerate functionsAllowedInBeginEnd.pl whenever a new
# extension is supported by any (available) hardware
allowedInBeginEndList: beginEndFunctionTest
	./beginEndFunctionTest > $(FUNCTIONSALLOWEDINBEGINEND)

beginEndFunctionTest: beginEndFunctionTest.o
	$(CC) -o $@ $^ -lGL -lGLU -lglut

beginEndFunctionTest.c: genBeginEndFunctionTest.pl argumentListTools.pl \
                        functionPointerTypes.inc ../GL/gl.h ../GL/glext.h
	perl genBeginEndFunctionTest.pl > beginEndFunctionTest.c

clean: cleandepend
	rm -f *.o *~ *.a *.inc *.so replayFunction.c beginEndFunctionTest.c \
	beginEndFunctionTest functionList.c functionsAllowedInBeginEnd.pl

depend:
	makedepend  -- $(CFLAGS) -- *.[ch]*

cleandepend:
	makedepend

tags:
	ctags --recurse=yes *.[ch]
	
.PHONY: clean cleandepend depend lists preload_hooks types tags


# DO NOT DELETE
