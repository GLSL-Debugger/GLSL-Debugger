####
#### This Makefile should be used in the Linux Compiler build.  This Makefile build the OSDependent 
#### component as a static library.
####

include ../../../../common.mk

#
# Compilation related
#
INCLUDE = -I. -I../.. -I../../Include -I../../../compiler 
WARNINGS = -Wall -Wwrite-strings -Wpointer-arith -Wcast-align -Wno-deprecated
DEFINE = -Dlinux -D__i386__ -D_ALT_NS=1
  
CXXFLAGS += $(DEFINE) $(WARNINGS) $(INCLUDE)

#
# Object file variables are defined here.
#
OSSRCS = ossource.cpp
OSOBJS = ossource.o

LIBNAME = libOssource.a

all : $(LIBNAME)

$(LIBNAME) : $(OSOBJS)
	 $(AR) $(ARFLAGS) $(LIBNAME) $(OSOBJS) 

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

#
# Dependency
#
depend:
	makedepend -- $(CXXFLAGS) -- $(OSSRCS)

#
# Cleanup
#
.PHONY: clean cleandepend depend

clean: cleandepend
	$(RM) *.o *.a

cleandepend:
	makedepend

# DO NOT DELETE
