include ../../common.mk

libutils.a: hash.o dlutils.o p2pcopy.o dbgprint.o pfm.o sync.o
	$(AR) $(ARFLAGS) $@ $^
	-$(RANLIB) $@

clean: cleandepend
	rm -f *.o *~ libutils.a

depend:
	makedepend  -- $(CFLAGS) -- *.[ch]*

cleandepend:
	makedepend

tags:
	ctags --recurse=yes *.[ch]
	
.PHONY: clean cleandepend depend tags


# DO NOT DELETE
