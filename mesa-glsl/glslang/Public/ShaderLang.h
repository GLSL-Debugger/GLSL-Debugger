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
#ifndef _COMPILER_INTERFACE_INCLUDED_
#define _COMPILER_INTERFACE_INCLUDED_

#include "ResourceLimits.h"

#ifdef _WIN32
#define C_DECL __cdecl
#ifdef SH_EXPORTING
    #define SH_IMPORT_EXPORT __declspec(dllexport)
#else
    #define SH_IMPORT_EXPORT __declspec(dllimport)
#endif
#else
#define SH_IMPORT_EXPORT
#define __fastcall
#define C_DECL
#endif

#include <stdio.h>

//
// This is the platform independent interface between an OGL driver
// and the shading language compiler/linker.
//

#ifdef __cplusplus
    extern "C" {
#endif
//
// Driver must call this first, once, before doing any other
// compiler/linker operations.
//
SH_IMPORT_EXPORT int ShInitialize();
//
// Driver should call this at shutdown.
//
SH_IMPORT_EXPORT int __fastcall ShFinalize();
//
// Types of languages the compiler can consume.
//
typedef enum {
	EShLangVertex,
	EShLangGeometry,
	EShLangFragment,
	EShLangPack,
    EShLangUnpack,
    EShLangCount,
} EShLanguage;

//
// Types of output the linker will create.
//
typedef enum {
    EShExVertexFragment,
    EShExPackFragment,
    EShExUnpackFragment,
    EShExFragment
} EShExecutable;

//
// Optimization level for the compiler.
//
typedef enum {
    EShOptNoGeneration,
    EShOptNone,
    EShOptSimple,       // Optimizations that can be done quickly
    EShOptFull,         // Optimizations that will take more time
} EShOptimizationLevel;

//
// Build a table for bindings.  This can be used for locating
// attributes, uniforms, globals, etc., as needed.
//
typedef struct {
    char* name;
    int binding;
} ShBinding;

typedef struct {
    int numBindings;
	ShBinding* bindings;  // array of bindings
} ShBindingTable;

//
// Typedefs for changeables
//
typedef enum {
    SH_CGB_STRUCT,
    SH_CGB_ARRAY_DIRECT,
    SH_CGB_ARRAY_INDIRECT,
    SH_CGB_SWIZZLE
} ShChangeableType;

typedef struct {
    ShChangeableType type;
    int index;
} ShChangeableIndex;

typedef struct {
    int id;
    int numIndices;
    ShChangeableIndex **indices;
} ShChangeable;

typedef struct {
    int numChangeables;
    ShChangeable **changeables;
} ShChangeableList;

//
// Some helper Functions for changeable handling
//
SH_IMPORT_EXPORT ShChangeable* createShChangeable(int id);
SH_IMPORT_EXPORT ShChangeableIndex* createShChangeableIndex(ShChangeableType type, int index);
SH_IMPORT_EXPORT void dumpShChangeable(ShChangeable *cbl);
SH_IMPORT_EXPORT void dumpShChangeableList(ShChangeableList *cl);
SH_IMPORT_EXPORT void addShChangeable(ShChangeableList *cl, ShChangeable *c);
SH_IMPORT_EXPORT void copyShChangeableList(ShChangeableList *clout, ShChangeableList *clin);
SH_IMPORT_EXPORT void addShIndexToChangeable(ShChangeable *c, ShChangeableIndex *idx);
SH_IMPORT_EXPORT void addShIndexToChangeableList(ShChangeableList *cl, int s, ShChangeableIndex *idx);
SH_IMPORT_EXPORT void freeShChangeable(ShChangeable **c);

//
// Types for specifiing the behaviour of code generation
//

typedef enum {
    DBG_CG_ORIGINAL_SRC,
    DBG_CG_COVERAGE,
    DBG_CG_GEOMETRY_MAP,
    DBG_CG_GEOMETRY_CHANGEABLE,
    DBG_CG_VERTEX_COUNT,
    DBG_CG_SELECTION_CONDITIONAL,
    DBG_CG_LOOP_CONDITIONAL,
    DBG_CG_CHANGEABLE
} DbgCgOptions;


//
// Type for specifiing the navigation debugger
//
typedef enum {
    DBG_BH_RESET = 1,               // reset debugging to the original program
    DBG_BH_JUMPINTO = 2,            // trace function calls
    DBG_BH_FOLLOW_ELSE = 4,         // evaluate else brance of a conditional
    DBG_BH_SELECTION_JUMP_OVER = 8, // do not debug either branch
    DBG_BH_LOOP_CONTINUE = 16,      // jump out of a loop
    DBG_BH_LOOP_NEXT_ITER = 32      // jump to next iteration without debuggig anything inbetween
} DbgBehaviour;

//
// Debug result information
//
typedef enum {
    DBG_RS_STATUS_UNSET,
    DBG_RS_STATUS_ERROR,
    DBG_RS_STATUS_OK,
    DBG_RS_STATUS_FINISHED
} DbgRsStatus;

typedef enum {
    DBG_RS_POSITION_UNSET,
    DBG_RS_POSITION_ASSIGMENT,
    DBG_RS_POSITION_FUNCTION_CALL,
    DBG_RS_POSITION_UNARY,
    DBG_RS_POSITION_SELECTION_IF,
    DBG_RS_POSITION_SELECTION_IF_ELSE,
    DBG_RS_POSITION_SELECTION_IF_CHOOSE,
    DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE,
    DBG_RS_POSITION_BRANCH,
    DBG_RS_POSITION_LOOP_FOR,
    DBG_RS_POSITION_LOOP_WHILE,
    DBG_RS_POSITION_LOOP_DO,
    DBG_RS_POSITION_LOOP_CHOOSE,
    DBG_RS_POSITION_DUMMY
} DbgRsTargetPosition;

typedef struct {
    int numIds;
    int *ids;
} DbgRsScope;

typedef struct {
    int line;
    int colum;
} DbgRsPos;

typedef struct {
    DbgRsPos left;
    DbgRsPos right;
} DbgRsRange;

typedef struct {
    DbgRsStatus status;
    DbgRsTargetPosition position;
    DbgRsRange range;
    DbgRsScope scope;
    DbgRsScope scopeStack;
    ShChangeableList cgbls;
    int loopIteration;
    bool passedEmitVertex;
    bool passedDiscard;
} DbgResult;

SH_IMPORT_EXPORT const char* getDbgRsStatus(DbgRsStatus s);
SH_IMPORT_EXPORT const char* getDbgRsPosition(DbgRsTargetPosition p);

//
// ShHandle held by but opaque to the driver.  It is allocated,
// managed, and de-allocated by the compiler/linker. It's contents
// are defined by and used by the compiler and linker.  For example,
// symbol table information and object code passed from the compiler
// to the linker can be stored where ShHandle points.
//
// If handle creation fails, 0 will be returned.
//
typedef void* ShHandle;

//
// Driver calls these to create and destroy compiler/linker
// objects.
//
SH_IMPORT_EXPORT ShHandle ShConstructCompiler(const EShLanguage, int debugOptions);  // one per shader
SH_IMPORT_EXPORT ShHandle ShConstructLinker(const EShExecutable, int debugOptions);  // one per shader pair
SH_IMPORT_EXPORT ShHandle ShConstructUniformMap();                 // one per uniform namespace (currently entire program object)
SH_IMPORT_EXPORT void ShDestruct(ShHandle);


//
// Typedefs for struct, variable, and builtin handling
//

typedef enum {
    SH_FLOAT,
    SH_INT,
    SH_UINT,
    SH_BOOL,
    SH_STRUCT,
    SH_ARRAY,
    SH_SAMPLER_GUARD_BEGIN, // no type
    SH_SAMPLER_1D,
    SH_ISAMPLER_1D,
    SH_USAMPLER_1D,
    SH_SAMPLER_2D,
    SH_ISAMPLER_2D,
    SH_USAMPLER_2D,
    SH_SAMPLER_3D,
    SH_ISAMPLER_3D,
    SH_USAMPLER_3D,
    SH_SAMPLER_CUBE,
    SH_ISAMPLER_CUBE,
    SH_USAMPLER_CUBE,
    SH_SAMPLER_1D_SHADOW,
    SH_SAMPLER_2D_SHADOW,
    SH_SAMPLER_2D_RECT,
    SH_ISAMPLER_2D_RECT,
    SH_USAMPLER_2D_RECT,
    SH_SAMPLER_2D_RECT_SHADOW,
    SH_SAMPLER_1D_ARRAY,
    SH_ISAMPLER_1D_ARRAY,
    SH_USAMPLER_1D_ARRAY,
    SH_SAMPLER_2D_ARRAY,
    SH_ISAMPLER_2D_ARRAY,
    SH_USAMPLER_2D_ARRAY,
    SH_SAMPLER_BUFFER,
    SH_ISAMPLER_BUFFER,
    SH_USAMPLER_BUFFER,
    SH_SAMPLER_1D_ARRAY_SHADOW,
    SH_SAMPLER_2D_ARRAY_SHADOW,
    SH_SAMPLER_CUBE_SHADOW,
    SH_SAMPLER_GUARD_END // no type
} variableType;

bool ShIsSampler(variableType v);

typedef enum {
    SH_UNSET,
    SH_TEMPORARY,
    SH_GLOBAL,
    SH_CONST,
    SH_ATTRIBUTE,
    SH_VARYING_IN,
    SH_VARYING_OUT,
    SH_UNIFORM,
    SH_PARAM_IN,
    SH_PARAM_OUT,
    SH_PARAM_INOUT,
    SH_PARAM_CONST,
    SH_BUILTIN_READ,
    SH_BUILTIN_WRITE
} variableQualifier;

typedef enum {
    SH_VM_NONE = 0,
    SH_VM_INVARIANT = 1,
    SH_VM_FLAT = 2,
    SH_VM_CENTROID = 4,
    SH_VM_NOPERSPECTIVE = 8,
    SH_VM_SMOOTH = 16
} variableVMTypes;

typedef int variableVaryingModifier;

typedef struct ShVariable {
    int uniqueId;
    int builtin;
    char *name;
    variableType type;
    variableQualifier qualifier;
    variableVaryingModifier varyingModifier;

    int size;                      // Size of vector

    int isMatrix;
    int matrixSize[2];

    int isArray;
    int arraySize[MAX_ARRAYS];

    char *structName;
    int structSize;
    ShVariable **structSpec;
} XX_SHVARIABLE;


typedef struct ShVariableList {
    int numVariables;
    ShVariable **variables;
} XX_SHVARIABLELIST;

SH_IMPORT_EXPORT char* ShGetTypeString(const ShVariable *v);
SH_IMPORT_EXPORT const char* ShGetQualifierString(const ShVariable *v);

//
// Some helper functions for variable handling
//
SH_IMPORT_EXPORT void ShDumpVariable(ShVariable *v, int depth);
void addShVariable(ShVariableList *vl, ShVariable *v, int builtin);
ShVariable* findShVariableFromId(ShVariableList *vl, int id);
ShVariable* findFirstShVariableFromName(ShVariableList *vl, const char *name);
ShVariable* copyShVariable(ShVariable *src);
void freeShVariable(ShVariable **var);
void freeShVariableList(ShVariableList *vl);

//
// The return value of ShCompile is boolean, indicating
// success or failure.
//
// The info-log should be written by ShCompile into
// ShHandle, so it can answer future queries.
//
SH_IMPORT_EXPORT int ShCompile(
    const ShHandle,
    const char* const shaderStrings[],
    const int numStrings,
    const EShOptimizationLevel,
    const TBuiltInResource *resources,
    int debugOptions,
    ShVariableList *vl
    );

//
// Advancing the debugger
//
SH_IMPORT_EXPORT DbgResult* ShDebugJumpToNext(
        const ShHandle,
        int debugOptions,
        int dgbBh
        );

//
// Get debug program to a given changeable
//
SH_IMPORT_EXPORT char* ShDebugGetProg(
        const ShHandle,
        ShChangeableList *cgbl,
        ShVariableList *vl,
        DbgCgOptions dbgCgOptions
        );

//
// Similar to ShCompile, but accepts an opaque handle to an
// intermediate language structure.
//
SH_IMPORT_EXPORT int ShCompileIntermediate(
    ShHandle compiler,
    ShHandle intermediate,
    const EShOptimizationLevel,
    int debuggable           // boolean
    );

SH_IMPORT_EXPORT int ShLink(
    const ShHandle,               // linker object
    const ShHandle h[],           // compiler objects to link together
    const int numHandles,
    ShHandle uniformMap,          // updated with new uniforms
    short int** uniformsAccessed,  // returned with indexes of uniforms accessed
    int* numUniformsAccessed);

SH_IMPORT_EXPORT int ShLinkExt(
    const ShHandle,               // linker object
    const ShHandle h[],           // compiler objects to link together
    const int numHandles);

//
// ShSetEncrpytionMethod is a place-holder for specifying
// how source code is encrypted.
//
SH_IMPORT_EXPORT void ShSetEncryptionMethod(ShHandle);

//
// All the following return 0 if the information is not
// available in the object passed down, or the object is bad.
//
SH_IMPORT_EXPORT const char* ShGetInfoLog(const ShHandle);
SH_IMPORT_EXPORT const void* ShGetExecutable(const ShHandle);
SH_IMPORT_EXPORT int ShSetVirtualAttributeBindings(const ShHandle, const ShBindingTable*);   // to detect user aliasing
SH_IMPORT_EXPORT int ShSetFixedAttributeBindings(const ShHandle, const ShBindingTable*);     // to force any physical mappings
SH_IMPORT_EXPORT int ShGetPhysicalAttributeBindings(const ShHandle, const ShBindingTable**); // for all attributes
//
// Tell the linker to never assign a vertex attribute to this list of physical attributes
//
SH_IMPORT_EXPORT int ShExcludeAttributes(const ShHandle, int *attributes, int count);

//
// Returns the location ID of the named uniform.
// Returns -1 if error.
//
SH_IMPORT_EXPORT int ShGetUniformLocation(const ShHandle uniformMap, const char* name);

enum TDebugOptions {
	EDebugOpNone               = 0x000,
	EDebugOpIntermediate       = 0x001,
	EDebugOpAssembly           = 0x002,
    EDebugOpObjectCode         = 0x004,
	EDebugOpLinkMaps           = 0x008
};
#ifdef __cplusplus
    }
#endif

#endif // _COMPILER_INTERFACE_INCLUDED_
