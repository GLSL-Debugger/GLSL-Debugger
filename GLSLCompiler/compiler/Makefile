include ../../common.mk

INCLUDE = -I. -I../glslang -I../glslang/Include -I../glslang/OSDependent/Linux \
          -I../glslang/MachineIndependent
WARNINGS = -Wwrite-strings -Wpointer-arith -Wcast-align -Wno-deprecated
DEFINE = -Dlinux -D__i386__ -D_ALT_NS=1

CXXFLAGS += $(WARNINGS) $(DEFINE) $(INCLUDE)

#
# Object file variables are defined here.
#
OSSRCS = InitializeDll.cpp
OSOBJS = InitializeDll.o

LIBNAME = libInitializeDll.a

all : $(LIBNAME)

$(LIBNAME) : $(OSOBJS)
	 $(AR) $(ARFLAGS) $(LIBNAME) $(OSOBJS) 

#
# Dependency
#
depend:
	makedepend -- $(CPPFLAGS) -- $(OSSRCS)

cleandepend:
	makedepend

#
# Cleanup
#

clean: cleandepend
	$(RM) *.o *.a
	
.PHONY : clean cleandepend

# DO NOT DELETE
