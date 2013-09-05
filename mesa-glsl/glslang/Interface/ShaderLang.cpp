/*
 * ShaderLang.cpp
 *
 *  Created on: 04.09.2013
 */

#include "ShaderLang.h"

// Mesa includes
#include "glsl/standalone_scaffolding.h"
#include "glsl/glsl_parser_extras.h"
#include "glsl/program.h"
#include "glsl/ralloc.h"

#include "glsldb/utils/notify.h"


#ifndef UNUSED_ARG
    #define UNUSED_ARG(x) (void) x;
#endif

#define TO_PROGRAM(name) \
	struct gl_shader_program* program = reinterpret_cast< struct gl_shader_program* >( name );


// Variables
int glsl_es = 0;
int glsl_version = 330; // 150
int dump_ast = 0;
int dump_hir = 0;
int dump_lir = 0;
struct gl_context *main_context;

static void initialize_context( struct gl_context *ctx, gl_api api )
{
	initialize_context_to_defaults( ctx, api );

	/* The standalone compiler needs to claim support for almost
	 * everything in order to compile the built-in functions.
	 */
	ctx->Const.GLSLVersion = glsl_version;
	ctx->Extensions.ARB_ES3_compatibility = true;

	ctx->Const.MaxClipPlanes = 8;
	ctx->Const.MaxDrawBuffers = 2;

	/* More than the 1.10 minimum to appease parser tests taken from
	 * apps that (hopefully) already checked the number of coords.
	 */
	ctx->Const.MaxTextureCoordUnits = 4;

	ctx->Driver.NewShader = _mesa_new_shader;
}

void compile_shader(struct gl_context *ctx, struct gl_shader *shader)
{
   struct _mesa_glsl_parse_state *state =
      new(shader) _mesa_glsl_parse_state(ctx, shader->Type, shader);

   _mesa_glsl_compile_shader(ctx, shader, dump_ast, dump_hir);

   /* Print out the resulting IR */
   if (!state->error && dump_lir) {
      _mesa_print_ir(shader->ir, state);
   }

   return;
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
    	UT_NOTIFY_VA(LV_INFO, "%i", cgb->id);
        for (j=0; j<cgb->numIndices; j++) {
            ShChangeableIndex *idx = cgb->indices[j];
            if (idx) {
                switch (idx->type) {
                    case SH_CGB_ARRAY_DIRECT:
                    	UT_NOTIFY_VA(LV_INFO, "[%i]", idx->index);
                        break;
                    case SH_CGB_ARRAY_INDIRECT:
                    	UT_NOTIFY_VA(LV_INFO, "[(%i)]", idx->index);
                        break;
                    case SH_CGB_STRUCT:
                    	UT_NOTIFY_VA(LV_INFO, ".%i", idx->index);
                        break;
                    case SH_CGB_SWIZZLE:
                    	UT_NOTIFY_VA(LV_INFO, ",%i", idx->index);
                        break;
                    default:
                        break;
                }
            }
        }
        UT_NOTIFY(LV_INFO, " ");
    }
}

void dumpShChangeableList(ShChangeableList *cl)
{
    int i;

    if (!cl) return;
    UT_NOTIFY(LV_INFO, "===> ");
    if (cl->numChangeables == 0) {
    	UT_NOTIFY(LV_INFO, "empty\n");
        return;
    }


    for (i=0; i<cl->numChangeables; i++) {
        ShChangeable *cgb = cl->changeables[i];
        dumpShChangeable(cgb);
    }
    UT_NOTIFY(LV_INFO, "\n");
}


ShChangeable* createShChangeable(int id)
{
    ShChangeable *cgb;
    if (!(cgb = (ShChangeable*) malloc(sizeof(ShChangeable)))) {
        UT_NOTIFY(LV_ERROR, "not enough memory for cgb\n");
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
    	UT_NOTIFY(LV_ERROR, "not enough memory for idx\n");
        exit(1);
    }

    idx->type = type;
    idx->index = index;

    return idx;
}


void addShChangeable(ShChangeableList *cl, ShChangeable *c)
{
    if (!cl || !c) return;

    cl->numChangeables++;
    cl->changeables = (ShChangeable**) realloc(cl->changeables,
            cl->numChangeables*sizeof(ShChangeable*));
    cl->changeables[cl->numChangeables-1] = c;
}


void copyShChangeable(ShChangeableList *cl, ShChangeable *c)
{
    int i;
    ShChangeable *copy;

    if (!cl || !c) return;

    cl->numChangeables++;
    cl->changeables = (ShChangeable**) realloc(cl->changeables,
                cl->numChangeables*sizeof(ShChangeable*));

    copy = createShChangeable(c->id);

    // add all indices
    for (i=0; i<c->numIndices; i++) {
        copy->numIndices++;
        copy->indices = (ShChangeableIndex**) realloc(copy->indices,
                copy->numIndices*sizeof(ShChangeableIndex*));

        copy->indices[copy->numIndices-1] = (ShChangeableIndex*) malloc(
                sizeof(ShChangeableIndex));

        copy->indices[copy->numIndices-1]->type = c->indices[i]->type;
        copy->indices[copy->numIndices-1]->index = c->indices[i]->index;
    }

    cl->changeables[cl->numChangeables-1] = copy;
}


void addShIndexToChangeable(ShChangeable *c, ShChangeableIndex *idx)
{
    if (!c) return;

    c->numIndices++;
    c->indices = (ShChangeableIndex**) realloc(c->indices,
            c->numIndices*sizeof(ShChangeableIndex*));
    c->indices[c->numIndices-1] = idx;
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

void addShVariable(ShVariableList *vl, ShVariable *v, int builtin)
{
    int i;
    ShVariable **vp = vl->variables;

    v->builtin = builtin;

    for (i=0; i<vl->numVariables; i++) {
        if (vp[i]->uniqueId == v->uniqueId) {
            vp[i] = v;
            return;
        }
    }

    vl->numVariables++;
    vl->variables = (ShVariable**) realloc(vl->variables,
            vl->numVariables*sizeof(ShVariable*));
    vl->variables[vl->numVariables-1] = v;
}

char* ShGetTypeString(const ShVariable *v)
{
    char *result;

    if (!v) return NULL;

    if (v->isArray) {
        if (v->isMatrix) {
            asprintf(&result, "array of mat[%i][%i]", v->matrixSize[0], v->matrixSize[1]);
        } else if (v->size != 1) {
            switch(v->type) {
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
            asprintf(&result, "array of %s", getShTypeString((ShVariable*)v));
            return result;
        }
    } else {
        if (v->isMatrix) {
            asprintf(&result, "mat[%i][%i]", v->matrixSize[0], v->matrixSize[1]);
        } else if (v->size != 1) {
            switch(v->type) {
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
            asprintf(&result, getShTypeString((ShVariable*)v));
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

    if (!src) return NULL;

    if (!(ret = (ShVariable*) malloc(sizeof(ShVariable)))) {
        UT_NOTIFY(LV_ERROR, "not enough memory to copy ShVariable\n");
        exit(1);
    }

    ret->uniqueId = src->uniqueId;
    ret->builtin = src->builtin;

    if (src->name) {
        if (!(ret->name = (char*) malloc(strlen(src->name)+1))) {
        	UT_NOTIFY(LV_ERROR, "not enough memory to copy name of ShVariable\n");
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
    for (i=0; i<MAX_ARRAYS; i++) {
        ret->arraySize[i] = src->arraySize[i];
    }

    if (src->structName) {
        if (!(ret->structName = (char*) malloc(strlen(src->structName)+1))) {
        	UT_NOTIFY(LV_ERROR, "not enough memory to copy strctName of ShVariable\n");
            exit(1);
        }
        strcpy(ret->structName, src->structName);
    } else {
        ret->structName = NULL;
    }

    ret->structSize = src->structSize;

    if (!(ret->structSpec = (ShVariable**) malloc(sizeof(ShVariable*)*ret->structSize))) {
    	UT_NOTIFY(LV_ERROR, "not enough memory to copy structSpec of ShVariable\n");
        exit(1);
    }
    for (i=0; i<ret->structSize; i++) {
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
    for (i=0; i<vl->numVariables; i++) {
        freeShVariable(&vl->variables[i]);
    }
    free(vl->variables);
    vl->numVariables = 0;
}

//
// Driver must call this first, once, before doing any other
// compiler/linker operations.
//
int ShInitialize( )
{
	main_context = new struct gl_context;
	initialize_context( main_context, ( glsl_es ) ? API_OPENGLES2 : API_OPENGL_COMPAT );

	/*
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
	 */
	return 1;
}

//
// Driver calls these to create and destroy compiler/linker
// objects.
//

ShHandle ShConstructCompiler(const EShLanguage language, int debugOptions)
{
	/*
    if (!InitThread())
        return 0;

    TShHandleBase* base = static_cast<TShHandleBase*>(ConstructCompiler(language, debugOptions));
    return reinterpret_cast<void*>(base);
    */
	return NULL;
}

ShHandle ShConstructLinker(const EShExecutable executable, int debugOptions)
{
/*
    if (!InitThread())
        return 0;

    TShHandleBase* base = static_cast<TShHandleBase*>(ConstructLinker(executable, debugOptions));

    return reinterpret_cast<void*>(base);
*/
	return NULL;
}


void ShDestruct(ShHandle handle)
{
    if (handle == 0)
        return;

    TO_PROGRAM(handle)

	for (unsigned i = 0; i < MESA_SHADER_TYPES; i++)
		ralloc_free(program->_LinkedShaders[i]);

	ralloc_free(program);
}

//
// Remove shader context
//
int __fastcall ShFinalize()
{
	_mesa_glsl_release_types();
	_mesa_glsl_release_functions();

	if ( main_context ){
		delete main_context;
		main_context = NULL;
	}
	return 1;
}

//
// Do an actual compile on the given strings.  The result is left
// in the given compile object.
//
// Return:  The return value of ShCompile is really boolean, indicating
// success or failure.
//
int ShCompile( const ShHandle handle, const char* const shaderStrings[],
		const int numStrings, const EShOptimizationLevel optLevel,
		const TBuiltInResource* resources, int debugOptions, ShVariableList *vl )
{
	TO_PROGRAM(handle)
	//whole_program = rzalloc(NULL, struct gl_shader_program);
	//assert( whole_program != NULL );
	program->InfoLog = ralloc_strdup( program, "" );

	bool success = true;

	for( int shnum = 0; numStrings > shnum; shnum++ ){
		program->Shaders = reralloc(program, program->Shaders,
				struct gl_shader *, program->NumShaders + 1);
		assert( program->Shaders != NULL );

		struct gl_shader *shader = rzalloc(program, gl_shader);

		program->Shaders[program->NumShaders] = shader;
		program->NumShaders++;

		//const unsigned len = strlen( shaderStrings[shnum] );
		shader->Source = shaderStrings[shnum];

		/* TODO: shader types
		 if( strncmp( ".vert", ext, 5 ) == 0 || strncmp( ".glsl", ext, 5 ) == 0 )
		 shader->Type = GL_VERTEX_SHADER;
		 else if( strncmp( ".geom", ext, 5 ) == 0 )
		 shader->Type = GL_GEOMETRY_SHADER;
		 else if( strncmp( ".frag", ext, 5 ) == 0 )
		 shader->Type = GL_FRAGMENT_SHADER;
		 else
		 usage_fail (argv[0]);
		 */
		shader->Type = GL_VERTEX_SHADER;

		compile_shader( main_context, shader );

		// TODO: informative names
		if( !shader->CompileStatus ){
			printf( "Info log for %d:\n%s\n", shnum, shader->InfoLog );
			success = shader->CompileStatus;
			break;
		}
	}

	/*
	 if (handle == 0)
	 return 0;


	 // Processing of builtIns
	 // Parse symbol table globals at level 0 and 1
	 // return only non const variables
	 {
	 int i, j;

	 for (j=0; j<2; j++) {
	 TSymbolTableLevel* level = symbolTable.getLevel(j);

	 for (i=0; i<(int)level->getSize(); i++) {
	 TString *name;
	 TSymbol *symbol;
	 level->getEntry(i, &name, &symbol);

	 if (symbol->isVariable() &&
	 ((TVariable*)symbol)->getType().getQualifier()!=EvqConst &&
	 ((TVariable*)symbol)->getType().getQualifier()!=EvqConstNoValue &&
	 ((TVariable*)symbol)->getType().getQualifier()!=EvqTemporary ) {
	 addShVariable(vl, ((TVariable*)symbol)->getShVariable(),
	 true);
	 }
	 }
	 }
	 }

	 // Traverse tree for scope and variable processing
	 // Each node gets data holding list of variables changes with this
	 // operation and about scope information
	 TCompiler* dbgVarTraverser = ConstructCompilerDebugVar(
	 compiler->getLanguage(),
	 debugOptions,
	 vl);

	 dbgVarTraverser->compile(parseContext->treeRoot);
	 delete dbgVarTraverser;

	 intermediate.outputTree(parseContext->treeRoot);

	 while (! symbolTable.atSharedBuiltInLevel())
	 symbolTable.pop();

	 FinalizePreprocessor();

	 // Used to throw away all the temporary memory used by the compilation process.
	 // GlobalPoolAllocator.pop();

	 */

	return success ? 1 : 0;
}

DbgResult* ShDebugJumpToNext(
    const ShHandle handle,
    int debugOptions,
    int dbgBh
    )
{
    DbgResult *result = NULL;
    /*
    TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
    TCompiler* compiler = base->getAsCompiler();
    if (compiler == 0)
        return NULL;

    TParseContext* parseContext = compiler->getParseContext();

    compiler->infoSink.info.erase();
    compiler->infoSink.debug.erase();

    TCompiler* traverser =
        ConstructTraverseDebugJump(compiler->getLanguage(),
                                   debugOptions,
                                   dbgBh);

    result = ((TTraverseDebugJump*)traverser)->process(parseContext->treeRoot);

    TIntermediate intermediate(compiler->infoSink);
    intermediate.outputTree(parseContext->treeRoot);
    dbgPrint(DBGLVL_COMPILERINFO, "%s\n", ShGetInfoLog(compiler));

    delete traverser;
*/
    return result;

}

//
// Get debug program to a given changeable
//
char* ShDebugGetProg(const ShHandle handle,
                     ShChangeableList *cgbl,
                     ShVariableList *vl,
                     DbgCgOptions dbgCgOptions)
{
    char *prog = NULL;
    /*
    TO_PROGRAM(handle);

    program->Shaders

    TShHandleBase* base = reinterpret_cast<TShHandleBase*>(handle);
    TCompiler* compiler = base->getAsCompiler();
    if (compiler == 0)
            return NULL;
    TParseContext* parseContext = compiler->getParseContext();

    // Generate code
    compiler->compileDbg(parseContext->treeRoot, cgbl, vl, dbgCgOptions, &prog);
*/
    return prog;
}

//
// Do an actual link on the given compile objects.
//
// Return:  The return value of is really boolean, indicating
// success or failure.
//
int ShLink( const ShHandle linkHandle, const ShHandle compHandles[], const int numHandles,
		ShHandle uniformMapHandle, short int** uniformsAccessed,
		int* numUniformsAccessed )

{
	UNUSED_ARG( uniformMapHandle )
	UNUSED_ARG( uniformsAccessed )
	UNUSED_ARG( numUniformsAccessed )

	TO_PROGRAM(linkHandle)

	if( program == 0 )
		return 0;

	link_shaders(main_context, program);
	int status = (program->LinkStatus) ? EXIT_SUCCESS : EXIT_FAILURE;

	if (strlen(program->InfoLog) > 0)
		 printf("Info log for linking:\n%s\n", program->InfoLog);

	return status;
}

//
// Return any compiler/linker/uniformmap log of messages for the application.
//
const char* ShGetInfoLog(const ShHandle handle)
{
    if (handle == 0)
        return NULL;

    TO_PROGRAM(handle)

    return program->InfoLog;
}
