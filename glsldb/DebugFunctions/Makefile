include ../../common.mk

CFLAGS += -I..

LDFLAGS += -L../glenumerants -lglenumerants -L../utils -lutils -ldl -lGL

all: glEnd.so

%.so: %.o
	$(CC) -shared -o $@ -Wl,-soname,$@ $^ $(LDFLAGS)
	mkdir -p ../libs/plugins$(POSTFIX)
	cp $@ ../libs/plugins$(POSTFIX)/

clean: cleandepend
	rm -f *.o *~ *.so

depend:
	makedepend  -- $(CFLAGS) -- *.[ch]*

cleandepend:
	makedepend

tags:
	ctags --recurse=yes *.[ch]

.PHONY: clean depend tags


# DO NOT DELETE
