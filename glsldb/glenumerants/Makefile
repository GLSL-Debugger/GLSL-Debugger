include ../../common.mk

CFLAGS += -I..

libglenumerants.a: glenumerants.o glxenumerants.o
	$(AR) $(ARFLAGS) $@ $^
	-$(RANLIB) $@

glenumerants.o: glenumerants.c enumerants.h glxenumerants.h
	$(CC) $(CFLAGS) -c -o $@ glenumerants.c

glxenumerants.o: glxenumerants.c glxenumerants.h
	$(CC) $(CFLAGS) -c -o $@ glxenumerants.c
	
enumerants.h: genEnumerants.pl ../GL/gl.h ../GL/glext.h
	perl genEnumerants.pl ../GL/gl.h ../GL/glext.h > enumerants.h
	
glxenumerants.h: genGLXEnumerants.pl ../GL/glx.h ../GL/glxext.h
	perl genGLXEnumerants.pl ../GL/glx.h ../GL/glxext.h > glxenumerants.h
	
clean: cleandepend
	rm -f *.o *~ enumerants.h glxenumerants.h libglenumerants.a 

depend:
	makedepend  -- $(CFLAGS) -- *.[ch]*

cleandepend:
	makedepend
	
tags:
	ctags --recurse=yes *.[ch] 
	
.PHONY: clean cleandepend depend tags


# DO NOT DELETE
