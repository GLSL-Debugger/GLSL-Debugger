include ../../../common.mk

JUMPOBJECTS = DebugJump.o IntermStack.o
VAROBJECTS = DebugVar.o
SRCS = DebugJump.cpp IntermStack.cpp DebugVar.cpp

INCLUDES = -I. -I.. -I../../../glsldb/utils/

CXXFLAGS += $(INCLUDES)

default : all
all : libDebugJump.a libDebugVar.a

libDebugJump.a : $(JUMPOBJECTS)
	$(AR) $(ARFLAGS) $@ $(JUMPOBJECTS)
	$(RANLIB) $@

libDebugVar.a: $(VAROBJECTS)
	$(AR) $(ARFLAGS) $@ $(VAROBJECTS)
	$(RANLIB) $@

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

#
# Cleanup
#
.PHONY : clean cleandepend depend tags
	
clean: cleandepend
	$(RM) *.o *.a

cleandepend:
	makedepend

tags :
	ctags *.cpp *.h

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCS)
	
# DO NOT DELETE
