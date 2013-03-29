OPTIMIZATIONS = ON
#OPTIMIZATIONS = OFF

#CROSS_BUILD = ON

# determine presence of Intel C/C++ compiler
ifeq ($(shell which icc >& /dev/null && echo exists),exists)
	CC = icc
	CXX = icc
else
	ifeq ($(shell uname),Darwin)
		CC = clang
		CXX = clang++
	else
		CC = gcc
		CXX = g++
	endif	
endif

# build architecture
ARCH := $(shell uname -m | sed 's/i.86/i386/')

# cross build?
ifeq ($(CROSS_BUILD),ON)
    ifeq ($(ARCH),x86_64)
	    ARCH = i386
		CROSS_COMPILE_FLAG = -m32
    else 
	    ARCH = x86_64
		CROSS_COMPILE_FLAG = -m64
    endif
endif

ifeq ($(ARCH),x86_64)
	POSTFIX = 64
	ARCHLIB = lib64
else
	POSTFIX = 32
	ARCHLIB = lib
endif

# library tools
AR = ar
ARFLAGS = rcs
RANLIB = ranlib

# other tools
PERL = perl
RM = rm -f
MV = mv -f

# Qt tools
MOC = $(QTDIR)/bin/moc
RCC = $(QTDIR)/bin/rcc
UIC = $(QTDIR)/bin/uic

# compiler and linker flags
CFLAGS = -fPIC $(CROSS_COMPILE_FLAG) 
CXXFLAGS = -fPIC -DUNIX $(CROSS_COMPILE_FLAG)
LDFLAGS = -fPIC $(CROSS_COMPILE_FLAG)

ifeq ($(shell uname),Darwin)
	CFLAGS += -I/usr/X11/include
	CXXFLAGS += -std=c++11 -stdlib=libc++ 
else
	CXXFLAGS += -std=c++0x
endif

ifeq ($(CC),icc)
	# icc options (NOTE: requires sse2 and creates alternative codepaths for
	# modern Intel processors)
	LDFLAGS += -static-intel
	DBGCFLAGS = -ggdb -O0
	OPTCFLAGS = -O3 -xW -axPT -no-prec-div -finline-functions -unroll -parallel \
	            -par-runtime-control
	# needed to force icc to add .note.GNU-stack ELF sections :-/
	OPTCFLAGS += -use-asm
	OPTLDFLAGS = $(OPTCFLAGS)
else
	# gcc options
	DBGCFLAGS = -DDEBUG -ggdb -O0
	OPTCFLAGS = -O3 -finline-functions -funroll-loops -fprefetch-loop-arrays -ffast-math
	OPTLDFLAGS =
endif
		   
ifeq ($(OPTIMIZATIONS),ON)
	CFLAGS += $(OPTCFLAGS)
	CXXFLAGS += $(OPTCFLAGS)
	LDFLAGS += $(OPTLDFLAGS)
else
	CFLAGS += $(DBGCFLAGS)
	CXXFLAGS += $(DBGCFLAGS)
endif

DEFINE = -Dlinux

