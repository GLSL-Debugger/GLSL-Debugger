//
//Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _COMMON_INCLUDED_
#define _COMMON_INCLUDED_

#ifndef UNUSED_ARG
	#define UNUSED_ARG(x) (void) x;
#endif

#ifdef _WIN32
	#include <basetsd.h>
	#include <cstdlib>
#elif defined (solaris)
	#include <sys/int_types.h>
	#define UINT_PTR uintptr_t
#else
	#include <stdint.h>
	#define UINT_PTR uintptr_t
#endif

/* windows only pragma */
#ifdef _MSC_VER
	#pragma warning(disable : 4786) // Don't warn about too long identifiers
	#pragma warning(disable : 4514) // unused inline method
	#pragma warning(disable : 4201) // nameless union
#endif

#ifdef __GNUC__
	#if __GNUC__ < 3
		#include <hash_map.h>
		namespace Sgi {using ::hash_map;};  // inherit globals
	#elif __GNUC__ < 4
		#include <ext/hash_map>
		#if __GNUC_MINOR__ == 0
			namespace Sgi = std;               // GCC 3.0
		#else
			namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
		#endif
	#else
		#ifdef GLSLDB_OSX
			#include <tr1/unordered_map>
		#else
			#include <unordered_map>
		#endif
		namespace Sgi = std;
	#endif
#else      // ...  there are other compilers, right?
	namespace Sgi = std;
#endif

//
// Doing the push and pop below for warnings does not leave the warning state
// the way it was.  This seems like a defect in the compiler.  We would like
// to do this, but since it does not work correctly right now, it is turned
// off.
//
//??#pragma warning(push, 3)

#include <set>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <stdio.h>

//??#pragma warning(pop)

typedef int TSourceLoc;

typedef struct TSourcePos {
	TSourceLoc line;
	TSourceLoc colum;
} XX_TSOURCEPOS;

typedef struct TSourceRange {
	TSourcePos left;
	TSourcePos right;

	bool operator==(const TSourceRange op) const
	{
		return (left.line == op.left.line && left.colum == op.left.colum
				&& right.line == op.right.line && right.colum == op.right.colum);
	}
	bool operator!=(const TSourceRange op) const
	{
		return !operator==(op);
	}
} XX_TSOURCERANGE;

const TSourceRange TSourceRangeInit = { { 0, 0 }, { 0, 0 } };

inline TSourceRange addRange(TSourceRange a, TSourceRange b)
{
	TSourceRange range;

	range.left.line = a.left.line;
	range.left.colum = a.left.colum;
	range.right.line = b.right.line;
	range.right.colum = b.right.colum;

	return range;
}

#include <assert.h>

#include "PoolAlloc.h"

//
// Put POOL_ALLOCATOR_NEW_DELETE in base classes to make them use this scheme.
//
#define POOL_ALLOCATOR_NEW_DELETE(A)                                  \
    void* operator new(size_t s) { return (A).allocate(s); }          \
    void* operator new(size_t, void *_Where) { return (_Where);	}     \
    void operator delete(void*) { }                                   \
    void operator delete(void *, void *) { }                          \
    void* operator new[](size_t s) { return (A).allocate(s); }        \
    void* operator new[](size_t, void *_Where) { return (_Where);	} \
    void operator delete[](void*) { }                                 \
    void operator delete[](void *, void *) { }

#define TBaseMap std::map
#define TBaseList std::list
#define TBaseSet std::set

//
// Pool version of string.
//
typedef pool_allocator<char> TStringAllocator;
typedef std::basic_string<char, std::char_traits<char>, TStringAllocator> TString;
inline TString* NewPoolTString(const char* s)
{
	void* memory = GlobalPoolAllocator.allocate(sizeof(TString));
	return new (memory) TString(s);
}

//
// Pool allocator versions of vectors, lists, and maps
//
template<class T> class TVector: public std::vector<T, pool_allocator<T> > {
public:
	typedef typename std::vector<T, pool_allocator<T> >::size_type size_type;
	TVector() :
			std::vector<T, pool_allocator<T> >()
	{
	}
	TVector(const pool_allocator<T>& a) :
			std::vector<T, pool_allocator<T> >(a)
	{
	}
	TVector(size_type i) :
			std::vector<T, pool_allocator<T> >(i)
	{
	}
};

template<class T> class TList: public TBaseList<T, pool_allocator<T> > {
public:
	typedef typename TBaseList<T, pool_allocator<T> >::size_type size_type;
	TList() : TBaseList<T, pool_allocator<T> >() {}
	TList(const pool_allocator<T>& a) : TBaseList<T, pool_allocator<T> >(a) {}
	TList(size_type i): TBaseList<T, pool_allocator<T> >(i) {}
};

// This is called TStlSet, because TSet is taken by an existing compiler class.
template<class T, class CMP> class TStlSet: public std::set<T, CMP,
		pool_allocator<T> > {
	// No pool allocator versions of constructors in std::set.
};

template<class K, class D, class CMP = std::less<K> >
class TMap: public TBaseMap<K, D, CMP, pool_allocator<std::pair<const K, D> > > {
public:
	typedef pool_allocator<std::pair <const K, D> > tAllocator;

	TMap() : TBaseMap<K, D, CMP, tAllocator >() {}
	// use correct two-stage name lookup supported in gcc 3.4 and above
	TMap(const tAllocator& a) : TBaseMap<K, D, CMP, tAllocator>(TBaseMap<K, D, CMP, tAllocator >::key_compare(), a) {}
};

//
// Persistent string memory.  Should only be used for strings that survive
// across compiles/links.
//
typedef std::basic_string<char> TPersistString;

//
// templatized min and max functions.
//
template<class T> T Min(const T a, const T b)
{
	return a < b ? a : b;
}
template<class T> T Max(const T a, const T b)
{
	return a > b ? a : b;
}

//
// Create a TString object from an integer.
//
inline const TString String(const int i, const int base = 10)
{
	char text[16];     // 32 bit ints are at most 10 digits in base 10

#ifdef _WIN32
	_itoa(i, text, base);
#else
	UNUSED_ARG(base)
	// we assume base 10 for all cases
	sprintf(text, "%d", i);
#endif

	return text;
}

const unsigned int SourceLocLineMask = 0xffff;
const unsigned int SourceLocStringShift = 16;

__inline TPersistString FormatSourceRange(const TSourceRange range)
{
	char locText[128];

	sprintf(locText, "%4d:%3d - %4d:%3d", range.left.line, range.left.colum,
			range.right.line, range.right.colum);
	/*
	 int string = loc >> SourceLocStringShift;
	 int line = loc & SourceLocLineMask;

	 if (line)
	 sprintf(locText, "%d:%d", string, line);
	 else
	 sprintf(locText, "%d:? ", string);
	 */

	return TPersistString(locText);
}

typedef TMap<TString, TString> TPragmaTable;
typedef TMap<TString, TString>::tAllocator TPragmaTableAllocator;

//
// Extension handling inside intermediate representation
//
typedef enum {
	EBhRequire,
	EBhEnable,
	EBhWarn,
	EBhDisable
} TBehavior;

typedef std::pair<TString, TBehavior> TExtensionPair;
typedef TList<TExtensionPair> TExtensionList;

#endif // _COMMON_INCLUDED_
