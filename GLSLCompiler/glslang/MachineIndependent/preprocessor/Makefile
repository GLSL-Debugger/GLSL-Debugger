include ../../../../common.mk

OBJECTS = atom.o cpp.o cppstruct.o memory.o scanner.o symbols.o tokens.o
SRCS = scanner.c atom.c memory.c tokens. cpp.c cppstruct.c symbols.c

default: all
all : libPreprocessor.a
libPreprocessor.a : $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)
	$(RANLIB) $@

%.o : %.c
	$(CC) $(CFLAGS) -c $<

#
# Cleanup
#
.PHONY: clean cleandepend

clean: cleandepend
	$(RM) *.o *.a

cleandepend:
	makedepend

depend:
	makedepend  -Y -- $(SRCS)

# DO NOT DELETE
