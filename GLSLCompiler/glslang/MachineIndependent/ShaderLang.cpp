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

//
// Implement the top-level of interface to the compiler/linker,
// as defined in ShaderLang.h
//
#include "SymbolTable.h"
#include "ParseHelper.h"
#include "../glsldb/utils/dbgprint.h"
#include "../Include/ShHandle.h"
#include "InitializeDll.h"

#ifdef _WIN32
# include "asprintf.h"
#endif /* _WIN32 */

#ifndef SH_EXPORTING
# define SH_EXPORTING
#endif
#include "../Public/ShaderLang.h"
#include "Initialize.h"
//
// A symbol table for each language.  Each has a different
// set of built-ins, and we want to preserve that from
// compile to compile.
//
TSymbolTable SymbolTables[EShLangCount];

TPoolAllocator* PerProcessGPA = 0;
//
// This is the platform independent interface between an OGL driver
// and the shading language compiler/linker.
//

const char* getDbgRsStatus(DbgRsStatus s)
{
	switch (s) {
	case DBG_RS_STATUS_UNSET:
		return "DBG_RS_STATUS_UNSET";
	case DBG_RS_STATUS_ERROR:
		return "DBG_RS_STATUS_ERROR";
	case DBG_RS_STATUS_OK:
		return "DBG_RS_STATUS_OK";
	case DBG_RS_STATUS_FINISHED:
		return "DBG_RS_STATUS_FINISHED";
	default:
		return "unknown";
	}
}

const char* getDbgRsPosition(DbgRsTargetPosition p)
{
	switch (p) {
	case DBG_RS_POSITION_UNSET:
		return "DBG_RS_POSITION_UNSET";
	case DBG_RS_POSITION_ASSIGMENT:
		return "DBG_RS_POSITION_ASSIGMENT";
	case DBG_RS_POSITION_FUNCTION_CALL:
		return "DBG_RS_POSITION_FUNCTION_CALL";
	case DBG_RS_POSITION_UNARY:
		return "DBG_RS_POSITION_UNARY";
	case DBG_RS_POSITION_SELECTION_IF:
		return "DBG_RS_POSITION_SELECTION_IF";
	case DBG_RS_POSITION_SELECTION_IF_ELSE:
		return "DBG_RS_POSITION_SELECTION_IF_ELSE";
	case DBG_RS_POSITION_SELECTION_IF_CHOOSE:
		return "DBG_RS_POSITION_SELECTION_IF_CHOOSE";
	case DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE:
		return "DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE";
	case DBG_RS_POSITION_BRANCH:
		return "DBG_RS_POSITION_BRANCH";
	case DBG_RS_POSITION_LOOP_FOR:
		return "DBG_RS_POSITION_LOOP_FOR";
	case DBG_RS_POSITION_LOOP_WHILE:
		return "DBG_RS_POSITION_LOOP_WHILE";
	case DBG_RS_POSITION_LOOP_DO:
		return "DBG_RS_POSITION_LOOP_DO";
	case DBG_RS_POSITION_LOOP_CHOOSE:
		return "DBG_RS_POSITION_LOOP_CHOOSE";
	case DBG_RS_POSITION_DUMMY:
		return "DBG_RS_POSITION_DUMMY";
	default:
		return "unkown";
	}
}

static const char* getShTypeString(ShVariable *v)
{
	switch (v->type) {
	case SH_FLOAT:
		return "float";
	case SH_INT:
		return "int";
	case SH_UINT:
		return "unsigned int";
	case SH_BOOL:
		return "bool";
	case SH_STRUCT:
		return v->structName;
	case SH_SAMPLER_1D:
		return "sampler1D";
	case SH_ISAMPLER_1D:
		return "isampler1D";
	case SH_USAMPLER_1D:
		return "usampler1D";
	case SH_SAMPLER_2D:
		return "sampler2D";
	case SH_ISAMPLER_2D:
		return "isampler2D";
	case SH_USAMPLER_2D:
		return "usampler2D";
	case SH_SAMPLER_3D:
		return "sampler3D";
	case SH_ISAMPLER_3D:
		return "isampler3D";
	case SH_USAMPLER_3D:
		return "usampler3D";
	case SH_SAMPLER_CUBE:
		return "samplerCube";
	case SH_ISAMPLER_CUBE:
		return "isamplerCube";
	case SH_USAMPLER_CUBE:
		return "usamplerCube";
	case SH_SAMPLER_1D_SHADOW:
		return "sampler1DShadow";
	case SH_SAMPLER_2D_SHADOW:
		return "sampler2DShadow";
	case SH_SAMPLER_2D_RECT:
		return "sampler2DRect";
	case SH_ISAMPLER_2D_RECT:
		return "isampler2DRect";
	case SH_USAMPLER_2D_RECT:
		return "usampler2DRect";
	case SH_SAMPLER_2D_RECT_SHADOW:
		return "samplerRectShadow";
	case SH_SAMPLER_1D_ARRAY:
		return "sampler1DArray";
	case SH_ISAMPLER_1D_ARRAY:
		return "isampler1DArray";
	case SH_USAMPLER_1D_ARRAY:
		return "usampler1DArray";
	case SH_SAMPLER_2D_ARRAY:
		return "sampler2DArray";
	case SH_ISAMPLER_2D_ARRAY:
		return "isampler2DArray";
	case SH_USAMPLER_2D_ARRAY:
		return "usampler2DArray";
	case SH_SAMPLER_BUFFER:
		return "samplerBuffer";
	case SH_ISAMPLER_BUFFER:
		return "isamplerBuffer";
	case SH_USAMPLER_BUFFER:
		return "usamplerBuffer";
	case SH_SAMPLER_1D_ARRAY_SHADOW:
		return "sampler1DArrayShadow";
	case SH_SAMPLER_2D_ARRAY_SHADOW:
		return "sampler2DArrayShadow";
	case SH_SAMPLER_CUBE_SHADOW:
		return "samplerCubeShadow";
	default:
		return "unknown";
	}
}

bool ShIsSampler(variableType v)
{
	if (v < SH_SAMPLER_GUARD_BEGIN || SH_SAMPLER_GUARD_END < v) {
		return false;
	} else {
		return true;
	}
}

static const char* getShQualifierString(variableQualifier q)
{
	switch (q) {
	case SH_UNSET:
		return "";
	case SH_TEMPORARY:
		return "temporary";
	case SH_GLOBAL:
		return "global";
	case SH_CONST:
		return "const";
	case SH_ATTRIBUTE:
		return "attribute";
	case SH_VARYING_IN:
		return "varying_in";
	case SH_VARYING_OUT:
		return "varying_out";
	case SH_UNIFORM:
		return "uniform";
	case SH_PARAM_IN:
		return "parameter_in";
	case SH_PARAM_OUT:
		return "parameter_out";
	case SH_PARAM_INOUT:
		return "parameter_inout";
	case SH_PARAM_CONST:
		return "parameter_const";
	case SH_BUILTIN_READ:
		return "builtin_read";
	case SH_BUILTIN_WRITE:
		return "builtin_write";
	default:
		return "unknown";
	}
}

//
//
// CHANGEABLES helper functions
//
//

void dumpShChangeable(ShChangeable *cgb)
{
	int j;

	if (cgb) {
		dbgPrint(DBGLVL_INFO, "%i", cgb->id);
		for (j = 0; j < cgb->numIndices; j++) {
			ShChangeableIndex *idx = cgb->indices[j];
			if (idx) {
				switch (idx->type) {
				case SH_CGB_ARRAY_DIRECT:
					dbgPrint(DBGLVL_INFO, "[%i]", idx->index);
					break;
				case SH_CGB_ARRAY_INDIRECT:
					dbgPrint(DBGLVL_INFO, "[(%i)]", idx->index);
					break;
				case SH_CGB_STRUCT:
					dbgPrint(DBGLVL_INFO, ".%i", idx->index);
					break;
				case SH_CGB_SWIZZLE:
					dbgPrint(DBGLVL_INFO, ",%i", idx->index);
					break;
				default:
					break;
				}
			}
		}
		dbgPrint(DBGLVL_INFO, " ");
	}
}

void dumpShChangeableList(ShChangeableList *cl)
{
	int i;

	if (!cl)
		return;
	dbgPrint(DBGLVL_INFO, "===> ");
	if (cl->numChangeables == 0) {
		dbgPrint(DBGLVL_INFO, "empty\n");
		return;
	}

	for (i = 0; i < cl->numChangeables; i++) {
		ShChangeable *cgb = cl->changeables[i];
		dumpShChangeable(cgb);
	}
	dbgPrint(DBGLVL_INFO, "\n");
}

ShChangeable* createShChangeable(int id)
{
	ShChangeable *cgb;
	if (!(cgb = (ShChangeable*) malloc(sizeof(ShChangeable)))) {
		dbgPrint(DBGLVL_ERROR, "not enough memory for cgb\n");
	}
	cgb->id = id;
	cgb->numIndices = 0;
	cgb->indices = NULL;

	return cgb;
}

ShChangeableIndex* createShChangeableIndex(ShChangeableType type, int index)
{
	ShChangeableIndex *idx;
	if (!(idx = (ShChangeableIndex*) malloc(sizeof(ShChangeableIndex)))) {
		dbgPrint(DBGLVL_ERROR, "not enough memory for idx\n");
		exit(1);
	}

	idx->type = type;
	idx->index = index;

	return idx;
}

void addShChangeable(ShChangeableList *cl, ShChangeable *c)
{
	if (!cl || !c)
		return;

	cl->numChangeables++;
	cl->changeables = (ShChangeable**) realloc(cl->changeables,
			cl->numChangeables * sizeof(ShChangeable*));
	cl->changeables[cl->numChangeables - 1] = c;
}

void copyShChangeable(ShChangeableList *cl, ShChangeable *c)
{
	int i;
	ShChangeable *copy;

	if (!cl || !c)
		return;

	cl->numChangeables++;
	cl->changeables = (ShChangeable**) realloc(cl->changeables,
			cl->numChangeables * sizeof(ShChangeable*));

	copy = createShChangeable(c->id);

	// add all indices
	for (i = 0; i < c->numIndices; i++) {
		copy->numIndices++;
		copy->indices = (ShChangeableIndex**) realloc(copy->indices,
				copy->numIndices * sizeof(ShChangeableIndex*));

		copy->indices[copy->numIndices - 1] = (ShChangeableIndex*) malloc(
				sizeof(ShChangeableIndex));

		copy->indices[copy->numIndices - 1]->type = c->indices[i]->type;
		copy->indices[copy->numIndices - 1]->index = c->indices[i]->index;
	}

	cl->changeables[cl->numChangeables - 1] = copy;
}

static bool isEqualShChangeable(ShChangeable *a, ShChangeable *b)
{
	int i;

	if (a->id != b->id)
		return false;
	if (a->numIndices != b->numIndices)
		return false;

	for (i = 0; i < a->numIndices; i++) {
		if (a->indices[i]->type != b->indices[i]->type)
			return false;
		if (a->indices[i]->index != b->indices[i]->index)
			return false;
	}

	return true;
}

void copyShChangeableList(ShChangeableList *clout, ShChangeableList *clin)
{
	int i, j;

	if (!clout || !clin)
		return;

	for (i = 0; i < clin->numChangeables; i++) {
		// copy only if not already in list
		bool alreadyInList = false;
		for (j = 0; j < clout->numChangeables; j++) {
			if (isEqualShChangeable(clout->changeables[j],
					clin->changeables[i])) {
				alreadyInList = true;
				break;
			}
		}
		if (!alreadyInList) {
			copyShChangeable(clout, clin->changeables[i]);
		}
	}
}

void addShIndexToChangeable(ShChangeable *c, ShChangeableIndex *idx)
{
	if (!c)
		return;

	c->numIndices++;
	c->indices = (ShChangeableIndex**) realloc(c->indices,
			c->numIndices * sizeof(ShChangeableIndex*));
	c->indices[c->numIndices - 1] = idx;
}

void addShIndexToChangeableList(ShChangeableList *cl, int s,
		ShChangeableIndex *idx)
{
	if (!cl)
		return;

	if (s < cl->numChangeables) {
		addShIndexToChangeable(cl->changeables[s], idx);
	}
}

void freeShChangeable(ShChangeable **c)
{
	if (c && *c) {
		int i;
		for (i = 0; i < (*c)->numIndices; i++) {
			free((*c)->indices[i]);
		}
		free((*c)->indices);
		free(*c);
		*c = NULL;
	}
}

void freeShChangeableList(ShChangeableList *cl)
{
	int i;
	for (i = 0; i < cl->numChangeables; i++) {
		freeShChangeable(&cl->changeables[i]);
	}
	free(cl->changeables);
	cl->numChangeables = 0;
}

void ShDumpVariable(ShVariable *v, int depth)
{
	int i;

	for (i = 0; i < depth; i++)
		dbgPrint(DBGLVL_COMPILERINFO, "    ");

	if (0 <= v->uniqueId) {
		dbgPrint(DBGLVL_COMPILERINFO, "<%i> ", v->uniqueId);
	}

	if (v->builtin) {
		dbgPrint(DBGLVL_COMPILERINFO, "builtin ");
	}

	dbgPrint(DBGLVL_COMPILERINFO,
			"%s %s", getShQualifierString(v->qualifier), getShTypeString(v));

	if (v->isMatrix) {
		dbgPrint(DBGLVL_COMPILERINFO, "%ix%i", v->size, v->size);
	} else {
		if (1 < v->size) {
			dbgPrint(DBGLVL_COMPILERINFO, "%i", v->size);
		}
	}

	dbgPrint(DBGLVL_COMPILERINFO, " %s", v->name);

	if (v->isArray) {
		for (i = 0; i < MAX_ARRAYS; i++) {
			if (v->arraySize[i] != -1) {
				dbgPrint(DBGLVL_COMPILERINFO, "[%i]", v->arraySize[i]);
			} else {
				break;
			}
		}
		dbgPrint(DBGLVL_COMPILERINFO, "\n");
	} else {
		dbgPrint(DBGLVL_COMPILERINFO, "\n");
	}

	if (v->structSize != 0) {
		depth++;
		for (i = 0; i < v->structSize; i++) {
			ShDumpVariable(v->structSpec[i], depth);
		}
	}

}

void addShVariable(ShVariableList *vl, ShVariable *v, int builtin)
{
	int i;
	ShVariable **vp = vl->variables;

	v->builtin = builtin;

	for (i = 0; i < vl->numVariables; i++) {
		if (vp[i]->uniqueId == v->uniqueId) {
			vp[i] = v;
			return;
		}
	}

	vl->numVariables++;
	vl->variables = (ShVariable**) realloc(vl->variables,
			vl->numVariables * sizeof(ShVariable*));
	vl->variables[vl->numVariables - 1] = v;
}

ShVariable* findShVariableFromId(ShVariableList *vl, int id)
{
	ShVariable **vp = NULL;
	int i;

	if (!vl) {
		return NULL;
	}

	vp = vl->variables;

	for (i = 0; i < vl->numVariables; i++) {
		if (vp[i]->uniqueId == id) {
			return vp[i];
		}
	}

	return NULL;
}

ShVariable* findFirstShVariableFromName(ShVariableList *vl, const char *name)
{
	ShVariable **vp = NULL;
	int i;

	if (!vl) {
		return NULL;
	}

	vp = vl->variables;

	for (i = 0; i < vl->numVariables; i++) {
		if (!(strcmp(vp[i]->name, name))) {
			return vp[i];
		}
	}
	return NULL;
}

char* ShGetTypeString(const ShVariable *v)
{
	char *result;

	if (!v)
		return NULL;

	if (v->isArray) {
		if (v->isMatrix) {
			asprintf(&result, "array of mat[%i][%i]", v->matrixSize[0],
					v->matrixSize[1]);
		} else if (v->size != 1) {
			switch (v->type) {
			case SH_FLOAT:
				asprintf(&result, "array of vec%i", v->size);
				break;
			case SH_INT:
				asprintf(&result, "array of ivec%i", v->size);
				break;
			case SH_UINT:
				asprintf(&result, "array of uvec%i", v->size);
				break;
			case SH_BOOL:
				asprintf(&result, "array of bvec%i", v->size);
				break;
			case SH_STRUCT:
				asprintf(&result, "array of %s", v->structName);
				break;
			default:
				asprintf(&result, "unknown type");
				return result;
			}
		} else {
			asprintf(&result, "array of %s", getShTypeString((ShVariable*) v));
			return result;
		}
	} else {
		if (v->isMatrix) {
			asprintf(&result, "mat[%i][%i]", v->matrixSize[0],
					v->matrixSize[1]);
		} else if (v->size != 1) {
			switch (v->type) {
			case SH_FLOAT:
				asprintf(&result, "vec%i", v->size);
				break;
			case SH_INT:
				asprintf(&result, "ivec%i", v->size);
				break;
			case SH_UINT:
				asprintf(&result, "uvec%i", v->size);
				break;
			case SH_BOOL:
				asprintf(&result, "bvec%i", v->size);
				break;
			case SH_STRUCT:
				asprintf(&result, "%s", v->structName);
				break;
			default:
				asprintf(&result, "unknown type");
				return result;
			}
		} else {
			asprintf(&result, getShTypeString((ShVariable*) v));
			return result;
		}

	}
	return result;
}

const char* ShGetQualifierString(const ShVariable *v)
{
	switch (v->qualifier) {
	case SH_UNSET:
	case SH_TEMPORARY:
	case SH_GLOBAL:
		return "";
	case SH_CONST:
		return "const";
	case SH_ATTRIBUTE:
		return "attribute";
	case SH_VARYING_IN:
		return "varying in";
	case SH_VARYING_OUT:
		return "varying out";
	case SH_UNIFORM:
		return "uniform";
	case SH_PARAM_IN:
		return "in parameter";
	case SH_PARAM_OUT:
		return "out parameter";
	case SH_PARAM_INOUT:
		return "inout parameter";
	case SH_PARAM_CONST:
		return "const parameter";
	case SH_BUILTIN_READ:
		return "builtin read";
	case SH_BUILTIN_WRITE:
		return "builtin write";
	default:
		return "unknown qualifier";

	}
}

ShVariable* copyShVariable(ShVariable *src)
{
	ShVariable *ret;
	int i;

	if (!src)
		return NULL;

	if (!(ret = (ShVariable*) malloc(sizeof(ShVariable)))) {
		dbgPrint(DBGLVL_ERROR, "not enough memory to copy ShVariable\n");
		exit(1);
	}

	ret->uniqueId = src->uniqueId;
	ret->builtin = src->builtin;

	if (src->name) {
		if (!(ret->name = (char*) malloc(strlen(src->name) + 1))) {
			dbgPrint(DBGLVL_ERROR,
					"not enough memory to copy name of ShVariable\n");
			exit(1);
		}
		strcpy(ret->name, src->name);
	} else {
		ret->name = NULL;
	}

	ret->type = src->type;
	ret->qualifier = src->qualifier;
	ret->size = src->size;
	ret->isMatrix = src->isMatrix;
	ret->matrixSize[0] = src->matrixSize[0];
	ret->matrixSize[1] = src->matrixSize[1];
	ret->isArray = src->isArray;
	for (i = 0; i < MAX_ARRAYS; i++) {
		ret->arraySize[i] = src->arraySize[i];
	}

	if (src->structName) {
		if (!(ret->structName = (char*) malloc(strlen(src->structName) + 1))) {
			dbgPrint(DBGLVL_ERROR,
					"not enough memory to copy strctName of ShVariable\n");
			exit(1);
		}
		strcpy(ret->structName, src->structName);
	} else {
		ret->structName = NULL;
	}

	ret->structSize = src->structSize;

	if (!(ret->structSpec = (ShVariable**) malloc(
			sizeof(ShVariable*) * ret->structSize))) {
		dbgPrint(DBGLVL_ERROR,
				"not enough memory to copy structSpec of ShVariable\n");
		exit(1);
	}
	for (i = 0; i < ret->structSize; i++) {
		ret->structSpec[i] = copyShVariable(src->structSpec[i]);
	}

	return ret;
}

void freeShVariable(ShVariable **var)
{
	if (var && *var) {
		int i;
		free((*var)->name);
		for (i = 0; i < (*var)->structSize; i++) {
			freeShVariable(&(*var)->structSpec[i]);
		}
		free((*var)->structSpec);
		free((*var)->structName);
		free(*var);
		*var = NULL;
	}
}

void freeShVariableList(ShVariableList *vl)
{
	int i;
	for (i = 0; i < vl->numVariables; i++) {
		freeShVariable(&vl->variables[i]);
	}
	free(vl->variables);
	vl->numVariables = 0;
}

//
// Driver must call this first, once, before doing any other
// compiler/linker operations.
//
int ShInitialize()
{
	TInfoSink infoSink;
	bool ret = true;

	if (!InitProcess())
		return 0;

	// This method should be called once per process. If its called by multiple threads, then
	// we need to have thread synchronization code around the initialization of per process
	// global pool allocator
	if (!PerProcessGPA) {
		TPoolAllocator *builtInPoolAllocator = new TPoolAllocator(true);
		builtInPoolAllocator->push();
		TPoolAllocator* gPoolAllocator = &GlobalPoolAllocator;
		SetGlobalPoolAllocatorPtr(builtInPoolAllocator);

		TSymbolTable symTables[EShLangCount];

		GenerateBuiltInSymbolTable(0, infoSink, symTables);

		PerProcessGPA = new TPoolAllocator(true);
		PerProcessGPA->push();
		SetGlobalPoolAllocatorPtr(PerProcessGPA);

		SymbolTables[EShLangVertex].copyTable(symTables[EShLangVertex]);
		SymbolTables[EShLangGeometry].copyTable(symTables[EShLangGeometry]);
		SymbolTables[EShLangFragment].copyTable(symTables[EShLangFragment]);

		SetGlobalPoolAllocatorPtr(gPoolAllocator);

		symTables[EShLangVertex].pop();
		symTables[EShLangGeometry].pop();
		symTables[EShLangFragment].pop();

		builtInPoolAllocator->popAll();
		delete builtInPoolAllocator;

	}

	return ret ? 1 : 0;
}

//
// Driver calls these to create and destroy compiler/linker
// objects.
//

ShHandle ShConstructCompiler(const EShLanguage language, int debugOptions)
{
	if (!InitThread())
		return 0;

	TShHandleBase* base = static_cast<TShHandleBase*>(ConstructCompiler(
			language, debugOptions));

	return reinterpret_cast<void*>(base);
}

ShHandle ShConstructLinker(const EShExecutable executable, int debugOptions)
{
	if (!InitThread())
		return 0;

	TShHandleBase* base = static_cast<TShHandleBase*>(ConstructLinker(
			executable, debugOptions));

	return reinterpret_cast<void*>(base);
}

ShHandle ShConstructUniformMap()
{
	if (!InitThread())
		return 0;

	TShHandleBase* base = static_cast<TShHandleBase*>(ConstructUniformMap());

	return reinterpret_cast<void*>(base);
}

TCompiler::~TCompiler()
{
	delete parseContext;
}

void ShDestruct(ShHandle handle)
{
	if (handle == 0)
		return;

	TShHandleBase* base = static_cast<TShHandleBase*>(handle);

	if (base->getAsCompiler()) {
		GlobalPoolAllocator.pop();

		TCompiler *compiler = base->getAsCompiler();

		TIntermediate intermediate(compiler->infoSink);
		TSymbolTable symbolTable(SymbolTables[compiler->getLanguage()]);
		TParseContext* parseContext = new TParseContext(symbolTable,
				intermediate, compiler->getLanguage(), compiler->infoSink,
				NULL);

		intermediate.remove(parseContext->treeRoot);
		parseContext->treeRoot = NULL;

		delete parseContext;

		DeleteCompiler(base->getAsCompiler());

		// Throw away all the temporary memory used by the compilation process.
		// TODO: This assumes the compiler called exactly once and in right order!!!
		// TODO: Try to fix this later, since actual caller assures this
		// HINT: Correct handling would be
		//       1. ShCompile
		//       2. ShDebug.....
		//       3. ShDestruct
		//       4. Next Shader File

	} else if (base->getAsLinker())
		DeleteLinker(base->getAsLinker());
	else if (base->getAsUniformMap())
		DeleteUniformMap(base->getAsUniformMap());
}

//
// Cleanup symbol tables
//
int __fastcall ShFinalize()
{
	if (PerProcessGPA) {
		PerProcessGPA->popAll();
		delete PerProcessGPA;
		PerProcessGPA = 0;
	}
	return 1;
}

bool GenerateBuiltInSymbolTable(const TBuiltInResource* resources,
		TInfoSink& infoSink, TSymbolTable* symbolTables, EShLanguage language)
{

	TBuiltIns builtIns;

	if (resources) {
		builtIns.initialize(*resources);
		InitializeSymbolTable(builtIns.getBuiltInStrings(), language, infoSink,
				resources, symbolTables);
	} else {
		builtIns.initialize();
		dbgPrint(DBGLVL_COMPILERINFO,
				"!!!!!!!! Parse EShLangVertex BuiltIns\n");
		InitializeSymbolTable(builtIns.getBuiltInStrings(), EShLangVertex,
				infoSink, resources, symbolTables);
		dbgPrint(DBGLVL_COMPILERINFO,
				"!!!!!!!! Parse EShLangGeometry BuiltIns\n");
		InitializeSymbolTable(builtIns.getBuiltInStrings(), EShLangGeometry,
				infoSink, resources, symbolTables);
		dbgPrint(DBGLVL_COMPILERINFO,
				"!!!!!!!! Parse EShLangFragment BuiltIns\n");
		InitializeSymbolTable(builtIns.getBuiltInStrings(), EShLangFragment,
				infoSink, resources, symbolTables);
	}

	return true;
}

bool InitializeSymbolTable(TBuiltInStrings* BuiltInStrings,
		EShLanguage language, TInfoSink& infoSink,
		const TBuiltInResource* resources, TSymbolTable* symbolTables)
{
	TIntermediate intermediate(infoSink);
	TSymbolTable* symbolTable;

	if (resources) {
		symbolTable = symbolTables;
	} else {
		symbolTable = &symbolTables[language];
	}

	TParseContext parseContext(*symbolTable, intermediate, language, infoSink);

	GlobalParseContext = &parseContext;

	setInitialState();

	assert(symbolTable->isEmpty() || symbolTable->atSharedBuiltInLevel());

	//
	// Parse the built-ins.  This should only happen once per
	// language symbol table.
	//
	// Push the symbol table to give it an initial scope.  This
	// push should not have a corresponding pop, so that built-ins
	// are preserved, and the test for an empty table fails.
	//

	symbolTable->push();

	//Initialize the Preprocessor
	int ret = InitPreprocessor();
	if (ret) {
		infoSink.info.message(EPrefixInternalError,
				"Unable to intialize the Preprocessor");
		return false;
	}

	int j = 0;
	for (TBuiltInStrings::iterator i =
			BuiltInStrings[parseContext.language].begin();
			i != BuiltInStrings[parseContext.language].end(); ++i, j++) {
		const char* builtInShaders[1];
		int builtInLengths[1];

		builtInShaders[0] = (*i).c_str();
		builtInLengths[0] = (int) (*i).size();

		if (PaParseStrings(const_cast<char**>(builtInShaders), builtInLengths,
				1, parseContext) != 0) {
			infoSink.info.message(EPrefixInternalError,
					"Unable to parse built-ins");
			return false;
		}
	}

	if (resources) {
		IdentifyBuiltIns(parseContext.language, *symbolTable, *resources);
	} else {
		IdentifyBuiltIns(parseContext.language, *symbolTable);
	}

	FinalizePreprocessor();

	return true;
}

//
// Do an actual compile on the given strings.  The result is left
// in the given compile object.
//
// Return:  The return value of ShCompile is really boolean, indicating
// success or failure.
//
int ShCompile(const ShHandle handle, const char* const shaderStrings[],
		const int numStrings, const EShOptimizationLevel optLevel,
		const TBuiltInResource* resources, int debugOptions, ShVariableList *vl)
{
	if (handle == 0)
		return 0;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TCompiler* compiler = base->getAsCompiler();
	if (compiler == 0)
		return 0;

	clearTraverseDebugJump();

	GlobalPoolAllocator.push();
	compiler->infoSink.info.erase();
	compiler->infoSink.debug.erase();

	vl->numVariables = 0;
	vl->variables = NULL;

	if (numStrings == 0)
		return 1;

	TIntermediate intermediate(compiler->infoSink);
	TSymbolTable symbolTable(SymbolTables[compiler->getLanguage()]);

	GenerateBuiltInSymbolTable(resources, compiler->infoSink, &symbolTable,
			compiler->getLanguage());

	TBuiltInResource* rs = new TBuiltInResource;
	memcpy(rs, resources, sizeof(TBuiltInResource));

	TParseContext* parseContext = new TParseContext(symbolTable, intermediate,
			compiler->getLanguage(), compiler->infoSink, rs);
	parseContext->initializeExtensionBehavior();

	GlobalParseContext = parseContext;
	compiler->setParseContext(parseContext);

	setInitialState();

	InitPreprocessor();
	//
	// Parse the application's shaders.  All the following symbol table
	// work will be throw-away, so push a new allocation scope that can
	// be thrown away, then push a scope for the current shader's globals.
	//
	bool success = true;

	symbolTable.push();
	if (!symbolTable.atGlobalLevel())
		parseContext->infoSink.info.message(EPrefixInternalError,
				"Wrong symbol table level");

	if (parseContext->insertBuiltInArrayAtGlobalLevel())
		success = false;

	int ret = PaParseStrings(const_cast<char**>(shaderStrings), 0, numStrings,
			*parseContext);
	if (ret)
		success = false;

	if (success && parseContext->treeRoot) {
		if (optLevel == EShOptNoGeneration)
			parseContext->infoSink.info.message(EPrefixNone,
					"No errors.  No code generation or linking was requested.");
		else {
			success = intermediate.postProcess(parseContext->treeRoot,
					parseContext->language);

			if (success) {

				//TODO
				//if (debugOptions & EDebugOpIntermediate)
				//intermediate.outputTree(parseContext->treeRoot);

				//
				// Call the machine dependent compiler
				//
				if (!compiler->compile(parseContext->treeRoot))
					success = false;
			}
		}
	} else if (!success) {
		parseContext->infoSink.info.prefix(EPrefixError);
		parseContext->infoSink.info << parseContext->numErrors
				<< " compilation errors.  No code generated.\n\n";
		success = false;
		if (debugOptions & EDebugOpIntermediate)
			intermediate.outputTree(parseContext->treeRoot);
	}

	// Do not remove tree for further usage
	// intermediate.remove(parseContext->treeRoot);
	// parseContext->treeRoot = NULL;
	//
	// Ensure symbol table is returned to the built-in level,
	// throwing away all but the built-ins.
	//

	// Processing of builtIns
	// Parse symbol table globals at level 0 and 1
	// return only non const variables
	{
		int i, j;

		for (j = 0; j < 2; j++) {
			TSymbolTableLevel* level = symbolTable.getLevel(j);

			for (i = 0; i < (int) level->getSize(); i++) {
				TString *name;
				TSymbol *symbol;
				level->getEntry(i, &name, &symbol);

				if (symbol->isVariable()
						&& ((TVariable*) symbol)->getType().getQualifier()
								!= EvqConst
						&& ((TVariable*) symbol)->getType().getQualifier()
								!= EvqConstNoValue
						&& ((TVariable*) symbol)->getType().getQualifier()
								!= EvqTemporary) {
					addShVariable(vl, ((TVariable*) symbol)->getShVariable(),
							true);
				}
			}
		}
	}

	// Traverse tree for scope and variable processing
	// Each node gets data holding list of variables changes with this
	// operation and about scope information
	TCompiler* dbgVarTraverser = ConstructCompilerDebugVar(
			compiler->getLanguage(), debugOptions, vl);

	dbgVarTraverser->compile(parseContext->treeRoot);
	delete dbgVarTraverser;

	intermediate.outputTree(parseContext->treeRoot);

	while (!symbolTable.atSharedBuiltInLevel())
		symbolTable.pop();

	FinalizePreprocessor();

	// Used to throw away all the temporary memory used by the compilation process.
	// GlobalPoolAllocator.pop();

	return success ? 1 : 0;
}

DbgResult* ShDebugJumpToNext(const ShHandle handle, int debugOptions, int dbgBh)
{
	DbgResult *result;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TCompiler* compiler = base->getAsCompiler();
	if (compiler == 0)
		return NULL;

	TParseContext* parseContext = compiler->getParseContext();

	compiler->infoSink.info.erase();
	compiler->infoSink.debug.erase();

	TCompiler* traverser = ConstructTraverseDebugJump(compiler->getLanguage(),
			debugOptions, dbgBh);

	result = ((TTraverseDebugJump*) traverser)->process(parseContext->treeRoot);

	TIntermediate intermediate(compiler->infoSink);
	intermediate.outputTree(parseContext->treeRoot);
	dbgPrint(DBGLVL_COMPILERINFO, "%s\n", ShGetInfoLog(compiler));

	delete traverser;

	return result;

}

char* ShDebugGetProg(const ShHandle handle, ShChangeableList *cgbl,
		ShVariableList *vl, DbgCgOptions dbgCgOptions)
{
	char *prog = NULL;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TCompiler* compiler = base->getAsCompiler();
	if (compiler == 0)
		return NULL;
	TParseContext* parseContext = compiler->getParseContext();

	// Generate code
	compiler->compileDbg(parseContext->treeRoot, cgbl, vl, dbgCgOptions, &prog);

	return prog;
}

//
// Do an actual link on the given compile objects.
//
// Return:  The return value of is really boolean, indicating
// success or failure.
//
int ShLink(const ShHandle linkHandle, const ShHandle compHandles[],
		const int numHandles, ShHandle uniformMapHandle,
		short int** uniformsAccessed, int* numUniformsAccessed)

{
	UNUSED_ARG(uniformMapHandle)
	UNUSED_ARG(uniformsAccessed)
	UNUSED_ARG(numUniformsAccessed)

	if (!InitThread())
		return 0;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(linkHandle);
	TLinker* linker = static_cast<TLinker*>(base->getAsLinker());
	if (linker == 0)
		return 0;

	int returnValue;
	GlobalPoolAllocator.push();
	returnValue = ShLinkExt(linkHandle, compHandles, numHandles);
	GlobalPoolAllocator.pop();

	if (returnValue)
		return 1;

	return 0;
}
//
// This link method will be eventually used once the ICD supports the new linker interface
//
int ShLinkExt(const ShHandle linkHandle, const ShHandle compHandles[],
		const int numHandles)
{
	if (linkHandle == 0 || numHandles == 0)
		return 0;

	THandleList cObjects;

	{    // support MSVC++6.0
		for (int i = 0; i < numHandles; ++i) {
			if (compHandles[i] == 0)
				return 0;
			TShHandleBase* base =
					reinterpret_cast<TShHandleBase*>(compHandles[i]);
			if (base->getAsLinker()) {
				cObjects.push_back(base->getAsLinker());
			}
			if (base->getAsCompiler())
				cObjects.push_back(base->getAsCompiler());

			if (cObjects[i] == 0)
				return 0;
		}
	}

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(linkHandle);
	TLinker* linker = static_cast<TLinker*>(base->getAsLinker());

	if (linker == 0)
		return 0;

	linker->infoSink.info.erase();

	{    // support MSVC++6.0
		for (int i = 0; i < numHandles; ++i) {
			if (cObjects[i]->getAsCompiler()) {
				if (!cObjects[i]->getAsCompiler()->linkable()) {
					linker->infoSink.info.message(EPrefixError,
							"Not all shaders have valid object code.");
					return 0;
				}
			}
		}
	}

	bool ret = linker->link(cObjects);

	return ret ? 1 : 0;
}

//
// ShSetEncrpytionMethod is a place-holder for specifying
// how source code is encrypted.
//
void ShSetEncryptionMethod(ShHandle handle)
{
	if (handle == 0)
		return;
}

//
// Return any compiler/linker/uniformmap log of messages for the application.
//
const char* ShGetInfoLog(const ShHandle handle)
{
	if (!InitThread())
		return NULL;

	if (handle == 0)
		return NULL;

	TShHandleBase* base = static_cast<TShHandleBase*>(handle);
	TInfoSink* infoSink = NULL;

	if (base->getAsCompiler())
		infoSink = &(base->getAsCompiler()->getInfoSink());
	else if (base->getAsLinker())
		infoSink = &(base->getAsLinker()->getInfoSink());

	if (infoSink) {
		infoSink->info << infoSink->debug.c_str();
		return infoSink->info.c_str();
	} else {
		return NULL;
	}
}

//
// Return the resulting binary code from the link process.  Structure
// is machine dependent.
//
const void* ShGetExecutable(const ShHandle handle)
{
	if (!InitThread())
		return 0;

	if (handle == 0)
		return 0;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);

	TLinker* linker = static_cast<TLinker*>(base->getAsLinker());
	if (linker == 0)
		return 0;

	return linker->getObjectCode();
}

//
// Let the linker know where the application said it's attributes are bound.
// The linker does not use these values, they are remapped by the ICD or
// hardware.  It just needs them to know what's aliased.
//
// Return:  The return value of is really boolean, indicating
// success or failure.
//
int ShSetVirtualAttributeBindings(const ShHandle handle,
		const ShBindingTable* table)
{
	if (!InitThread())
		return 0;

	if (handle == 0)
		return 0;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TLinker* linker = static_cast<TLinker*>(base->getAsLinker());

	if (linker == 0)
		return 0;

	linker->setAppAttributeBindings(table);

	return 1;
}

//
// Let the linker know where the predefined attributes have to live.
//
int ShSetFixedAttributeBindings(const ShHandle handle,
		const ShBindingTable* table)
{
	if (!InitThread())
		return 0;

	if (handle == 0)
		return 0;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TLinker* linker = static_cast<TLinker*>(base->getAsLinker());

	if (linker == 0)
		return 0;

	linker->setFixedAttributeBindings(table);
	return 1;
}

//
// Some attribute locations are off-limits to the linker...
//
int ShExcludeAttributes(const ShHandle handle, int *attributes, int count)
{
	if (!InitThread())
		return 0;

	if (handle == 0)
		return 0;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TLinker* linker = static_cast<TLinker*>(base->getAsLinker());
	if (linker == 0)
		return 0;

	linker->setExcludedAttributes(attributes, count);

	return 1;
}

//
// Return the index for OpenGL to use for knowing where a uniform lives.
//
// Return:  The return value of is really boolean, indicating
// success or failure.
//
int ShGetUniformLocation(const ShHandle handle, const char* name)
{
	if (!InitThread())
		return 0;

	if (handle == 0)
		return -1;

	TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
	TUniformMap* uniformMap = base->getAsUniformMap();
	if (uniformMap == 0)
		return -1;

	return uniformMap->getLocation(name);
}

