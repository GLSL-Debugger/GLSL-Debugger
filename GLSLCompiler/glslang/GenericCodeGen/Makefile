include ../../../common.mk

OBJECTS = CodeGen.o Link.o CodeInsertion.o CodeTools.o

SRCS = CodeGen.cpp Link.cpp CodeInsertion.cpp CodeTools.cpp

INCLUDES = -I.. -I../../../glsldb/utils/

CXXFLAGS += $(INCLUDES)

default : all
all : libCodeGen.a

libCodeGen.a : $(OBJECTS)
	$(AR) rvu $@ $(OBJECTS)
	ranlib $@

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

#
# Cleanup
#
.PHONY : clean cleandepend depend
	
clean : cleandepend
	$(RM) *.o *.a

cleandepend:
	makedepend

depend:
	makedepend -- $(CXXFLAGS) -- $(SRCS)
	
# DO NOT DELETE
