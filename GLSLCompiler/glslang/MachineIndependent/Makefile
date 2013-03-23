include ../../../common.mk

INCLUDE = -I. -I.. -I../.. -I../Include -I../Public -I../OSDependent/Linux \
          -I../../compiler
INCPREPROCESSOR = ./preprocessor
INCCODEGEN = ./../GenericCodeGen
INCDEBUGCODEGEN = ./../DebugCodeGen
INCOSDEPENDENT = ./../OSDependent/Linux/
INCINITIALISATION = ./../../compiler/
LIBPREPROCESSOR = ./preprocessor/libPreprocessor.a
LIBOSDEPENDENT = ./../OSDependent/Linux/libOssource.a
LIBINITIALISATION = ./../../compiler/libInitializeDll.a
LIBCODEGEN = ./../GenericCodeGen/libCodeGen.a
LIBDEBUGJUMP = ./../DebugCodeGen/libDebugJump.a
LIBDEBUGVAR = ./../DebugCodeGen/libDebugVar.a
LIBS = $(LIBPREPROCESSOR) $(LIBCODEGEN) $(LIBDEBUGJUMP) $(LIBDEBUGVAR) \
       $(LIBOSDEPENDENT) $(LIBINITIALISATION)
OBJECTS = Initialize.o IntermTraverse.o Intermediate.o ParseHelper.o \
          PoolAlloc.o QualifierAlive.o RemoveTree.o ShaderLang.o intermOut.o \
          parseConst.o SymbolTable.o InfoSink.o Gen_glslang.o Gen_glslang_tab.o
SRCS= Gen_glslang.cpp Gen_glslang_tab.cpp Initialize.cpp IntermTraverse.cpp \
	Intermediate.cpp ParseHelper.cpp PoolAlloc.cp QualifierAlive.cpp \
	RemoveTree.cpp ShaderLang.cpp SymbolTable.cpp intermOut.cpp \
	parseConst.cpp InfoSink.cpp
CXXFLAGS += $(DEFINE) $(INCLUDE)
STATICLIB=./lib/libglslang.a

default: all

all: $(STATICLIB)

$(STATICLIB): $(OBJECTS) $(LIBS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)
	(TMPFILE=tmp.0815$$$$; mkdir $$TMPFILE; cd $$TMPFILE; \
	for a in $(LIBS); do objs=`ar t ../$$a`; ar xv ../$$a; ar rcs ../$@ $$objs; rm $$objs; done; \
	cd ..; rmdir $$TMPFILE)
	-$(RANLIB) $@

Gen_glslang.o : Gen_glslang.cpp glslang_tab.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c Gen_glslang.cpp -o $@

Gen_glslang_tab.o : Gen_glslang_tab.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c Gen_glslang_tab.cpp -o $@

Gen_glslang.cpp: glslang.l
	@echo Generating Gen_glslang.cpp
	@dos2unix glslang.l
	flex glslang.l

Gen_glslang_tab.cpp glslang_tab.h: glslang.y
	@echo Generating Gen_glslang_tab.cpp
	@dos2unix glslang.y
	bison -t -v -d glslang.y
	echo "#include \"../../../glsldb/utils/dbgprint.h\"" > Gen_glslang_tab.cpp
	perl -ne "s/(fprintf|YYFPRINTF)\s*\([^,]*\s*/dbgPrint\(DBGLVL_COMPILERINFO/;print;" glslang.tab.c >> Gen_glslang_tab.cpp
	rm glslang.tab.c
	mv glslang.tab.h glslang_tab.h

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

$(LIBPREPROCESSOR):
	cd $(INCPREPROCESSOR); make 

$(LIBCODEGEN):
	cd $(INCCODEGEN); make 

$(LIBDEBUGJUMP):
	cd $(INCDEBUGCODEGEN); make libDebugJump.a
	
$(LIBDEBUGVAR):
	cd $(INCDEBUGCODEGEN); make libDebugVar.a

$(LIBINITIALISATION):
	cd $(INCINITIALISATION); make 

$(LIBOSDEPENDENT):
	cd $(INCOSDEPENDENT); make 

depend:
	cd $(INCPREPROCESSOR); make depend
	cd $(INCCODEGEN); make depend
	cd $(INCDEBUGCODEGEN); make depend
	cd $(INCINITIALISATION); make depend
	cd $(INCOSDEPENDENT); make depend
	makedepend -Y -- $(CXXFLAGS) -- $(SRCS) 
#
# Cleanup
#
.PHONY : clean cleandepend
	
clean : cleandepend
	$(RM) *.o *.a Gen_glslang_tab.cpp Gen_glslang.cpp glslang_tab.h glslang.output
	$(RM) ./lib/*.so ./lib/*.a
	cd $(INCPREPROCESSOR); make clean
	cd $(INCCODEGEN); make clean
	cd $(INCINITIALISATION); make clean
	cd $(INCOSDEPENDENT); make clean
	cd $(INCDEBUGCODEGEN); make clean

cleandepend:
	cd $(INCPREPROCESSOR); make cleandepend
	cd $(INCCODEGEN); make cleandepend
	cd $(INCINITIALISATION); make cleandepend
	cd $(INCOSDEPENDENT); make cleandepend
	cd $(INCDEBUGCODEGEN); make cleandepend
	makedepend

# DO NOT DELETE
