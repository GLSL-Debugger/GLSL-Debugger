/*
 * ShaderLang.cpp
 *
 *  Created on: 04.09.2013
 */

#include "ShaderLang.h"
#include "ShaderHolder.h"
#include "Program.h"
#include "Visitors/sideeffects.h"

// Mesa includes
#include "glsl/standalone_scaffolding.h"
#include "glsl/glsl_parser_extras.h"
#include "glsl/glsl_symbol_table.cpp"
#include "glsl/program.h"
#include "glsl/ralloc.h"
#include "mesa/main/mtypes.h"

#include "glsldb/utils/notify.h"

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void) x;
#endif

// Variables
int glsl_es = 0;
int glsl_version = 150;


static void initialize_context(struct gl_context *ctx, const TBuiltInResource* resources)
{
	/* The standalone compiler needs to claim support for almost
	 * everything in order to compile the built-in functions.
	 */
	ctx->Const.GLSLVersion = glsl_version;
	ctx->Extensions.ARB_ES3_compatibility = true;

	/* 1.20 minimums. */
	ctx->Const.MaxLights = resources->maxLights;
	ctx->Const.MaxClipPlanes = resources->maxClipPlanes;
	ctx->Const.MaxTextureUnits = resources->maxTextureUnits;

	/* allow high amount */
	ctx->Const.MaxTextureCoordUnits = resources->maxTextureCoords;

	ctx->Const.VertexProgram.MaxAttribs = resources->maxVertexAttribs;
	ctx->Const.VertexProgram.MaxUniformComponents = resources->maxVertexUniformComponents;
	ctx->Const.FragmentProgram.MaxUniformComponents = resources->maxFragmentUniformComponents;
	ctx->Const.MaxVarying = resources->maxVaryingFloats;
	ctx->Const.MaxCombinedTextureImageUnits = resources->maxCombinedTextureImageUnits;
	ctx->Const.VertexProgram.MaxTextureImageUnits = resources->maxVertexTextureImageUnits;
//	ctx->Const.FragmentProgram.MaxTextureImageUnits = 16;
//	ctx->Const.GeometryProgram.MaxTextureImageUnits = 16;

	ctx->Const.MaxDrawBuffers = resources->maxDrawBuffers;
	ctx->Const.MaxGeometryOutputVertices = resources->geoVerticesOut;
	// I saw it in mailing list in the latest mesa git, I think. Or not.
	// But anyway I cannot find anything in current headers and we using only stable mesa now.
	//	int geoVerticesIn;
	//	int geoInputType;
	//	int geoOutputType;

	ctx->Driver.NewShader = _mesa_new_shader;

	// Enable required extensions
	ctx->Extensions.ARB_framebuffer_object = (GLboolean)resources->framebufferObjectsSupported;
	ctx->Extensions.EXT_transform_feedback = (GLboolean)resources->transformFeedbackSupported;
	ctx->Extensions.ARB_geometry_shader4 = (GLboolean)resources->geoShaderSupported;
}


void compile_shader(struct gl_context *ctx, struct gl_shader *shader, int debug)
{
	/* TODO: debug options
	 * 	EDebugOpIntermediate       = 0x001,
	 * 	EDebugOpAssembly           = 0x002,
	 * 	EDebugOpObjectCode         = 0x004,
	 * 	EDebugOpLinkMaps           = 0x008
	 */
	int dump_ast = debug & EDebugOpAssembly;
	int dump_hir = debug & EDebugOpIntermediate;
	int dump_lir = debug & EDebugOpObjectCode;
	struct _mesa_glsl_parse_state *state = new (shader) _mesa_glsl_parse_state(
			ctx, shader->Type, shader);

	_mesa_glsl_compile_shader(ctx, shader, dump_ast, dump_hir);

	/* Print out the resulting IR */
	if (!state->error && dump_lir)
		_mesa_print_ir(shader->ir, state);

	/* Check side effects, discards, vertex emits */
	ir_sideeffects_traverser_visitor sideeffects;
	sideeffects.visit(shader->ir);

	return;
}

//
// Driver must call this first, once, before doing any other
// compiler/linker operations.
//
int ShInitialize( )
{
	return 1;
}

//
// Driver calls these to create and destroy compiler/linker
// objects.
//
ShHandle ShConstructCompiler(const EShLanguage language, int debugOptions)
{
	ShaderHolder* holder = new ShaderHolder;
	holder->language = language;
	holder->debugOptions = debugOptions;
	holder->program = NULL;
	holder->ctx = new struct gl_context;
	initialize_context_to_defaults( holder->ctx,
					( glsl_es ) ? API_OPENGLES2 : API_OPENGL_COMPAT );
	return reinterpret_cast< void* >( holder );
}

ShHandle ShConstructLinker(const EShExecutable executable, int debugOptions)
{
	UNUSED_ARG(executable)
	UNUSED_ARG(debugOptions)
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
	if( handle == 0 )
		return;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );

	if( holder->program ){
		for( unsigned i = 0; i < MESA_SHADER_TYPES; i++ )
			ralloc_free( holder->program->_LinkedShaders[i] );
		ralloc_free( holder->program );
		holder->program = NULL;
	}

	if( holder->ctx )
		delete holder->ctx, holder->ctx = NULL;

	delete holder, holder = NULL;
}

//
// Remove shader context
//
int __fastcall ShFinalize( )
{
	_mesa_glsl_release_types();
	// Lol, mesa just lost it.
	//_mesa_glsl_release_functions();
	return 1;
}


int addShVariableList( ShVariableList *vl, exec_list* list, bool globals_only )
{
	int count = 0;
	foreach_list( node, list ){
		ir_instruction* ir = (ir_instruction *)node;
		if( globals_only && ir->ir_type != ir_type_variable )
			continue;
		count += addShVariableIr( vl, ir );
	}

	return count;
}


int addShVariableIr( ShVariableList *vl, ir_instruction* ir )
{
	switch( ir->ir_type ){
		case ir_type_variable:
		{
			ShVariable* sh = irToShVariable( ir->as_variable() );
			if( sh && sh->qualifier != SH_TEMPORARY ){
				addShVariable( vl, sh, strncmp( sh->name, "gl_", 3 ) == 0 );
				return 1;
			}
			break;
		}
		case ir_type_function:
		{
			ir_function* f = ir->as_function();
			return addShVariableList( vl, &f->signatures );
			break;
		}
		case ir_type_function_signature:
		{
			ir_function_signature* fs = ir->as_function_signature();
			int count = 0;
			foreach_list( node, &fs->parameters ) {
				ir_instruction* ir = (ir_instruction *)node;
				count += addShVariableIr( vl, ir );
			}
			return count + addShVariableList( vl, &fs->body );
			break;
		}
		default:
			break;
	}
	return 0;
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
	// TODO: Can we use something like in mesa?
	UNUSED_ARG(optLevel)

	if( handle == NULL )
		return 0;

	clearTraverseDebugJump();

	vl->numVariables = 0;
	vl->variables = NULL;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );
	initialize_context(holder->ctx, resources);

	holder->program = rzalloc(NULL, struct gl_shader_program);
	assert( holder->program != NULL );
	holder->program->InfoLog = ralloc_strdup( holder->program, "" );

	bool success = true;

	for( int shnum = 0; numStrings > shnum; shnum++ ){
		holder->program->Shaders = reralloc(holder->program, holder->program->Shaders,
				struct gl_shader *, holder->program->NumShaders + 1);
		assert( holder->program->Shaders != NULL );

		struct gl_shader *shader = rzalloc(holder->program, gl_shader);

		holder->program->Shaders[holder->program->NumShaders] = shader;
		holder->program->NumShaders++;

		//const unsigned len = strlen( shaderStrings[shnum] );
		shader->Source = shaderStrings[shnum];

		switch( holder->language ){
			case EShLangFragment:
				shader->Type = GL_FRAGMENT_SHADER;
				break;
			case EShLangGeometry:
				shader->Type = GL_GEOMETRY_SHADER;
				break;
			case EShLangVertex:
			default:
				shader->Type = GL_VERTEX_SHADER;
				break;
		}

		char* old_locale = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, "POSIX");
		compile_shader( holder->ctx, shader, debugOptions );
		setlocale(LC_NUMERIC, old_locale);

		// TODO: informative names
		if( !shader->CompileStatus ){
			UT_NOTIFY_VA( LV_ERROR,
					"Info log for %d:\n%s\n", shader->Name, shader->InfoLog );
			success = shader->CompileStatus;
			break;
		}

		printShaderIr(shader);

		// Recursively add global variables from ir tree
		addShVariableList( vl, shader->ir, true );

		// Traverse tree for scope and variable processing
		// Each node gets data holding list of variables changes with this
		// operation and about scope information
		ShaderVarTraverse( shader, vl );
	}

	return success ? 1 : 0;
}

DbgResult* ShDebugJumpToNext(const ShHandle handle, int debugOptions, int dbgBh)
{
	DbgResult* result = NULL;

	if( handle == NULL )
		return 0;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );
	struct gl_shader* shader = holder->program->Shaders[0];


//	switch( holder->language ){
//		case EShLangVertex:
//			shader =  holder->program->_LinkedShaders[MESA_SHADER_VERTEX];
//			break;
//		case EShLangFragment:
//			shader = holder->program->_LinkedShaders[MESA_SHADER_FRAGMENT];
//			break;
//		case EShLangGeometry:
//			shader = holder->program->_LinkedShaders[MESA_SHADER_GEOMETRY];
//			break;
//	}

	result = ShaderTraverse( shader, debugOptions, dbgBh );
	return result;

}

//
// Get debug program to a given changeable
//
char* ShDebugGetProg(const ShHandle handle, ShChangeableList *cgbl, ShVariableList *vl,
		DbgCgOptions dbgCgOptions)
{
	if( handle == NULL )
		return 0;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );
	struct gl_shader* shader = holder->program->Shaders[0];
	char* prog = NULL;

	// Generate code
	compileDbgShaderCode(shader, cgbl, vl, dbgCgOptions, &prog);
	return prog;
}

//
// Do an actual link on the given compile objects.
//
// Return:  The return value of is really boolean, indicating
// success or failure.
//
int ShLink(const ShHandle linkHandle, const ShHandle compHandles[], const int numHandles,
		ShHandle uniformMapHandle, short int** uniformsAccessed, int* numUniformsAccessed)

{
	UNUSED_ARG( compHandles )
	UNUSED_ARG( uniformMapHandle )
	UNUSED_ARG( uniformsAccessed )
	UNUSED_ARG( numUniformsAccessed )
	UNUSED_ARG( numHandles )

	if( linkHandle == NULL )
		return 0;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( linkHandle );

	if( holder->program == 0 )
		return 0;

	link_shaders( holder->ctx, holder->program );
	int status = ( holder->program->LinkStatus ) ? EXIT_SUCCESS : EXIT_FAILURE;

	if( strlen( holder->program->InfoLog ) > 0 )
		printf( "Info log for linking:\n%s\n", holder->program->InfoLog );

	return status;
}

//
// Return any compiler/linker/uniformmap log of messages for the application.
//
const char* ShGetInfoLog(const ShHandle handle)
{
	if( handle == 0 )
		return NULL;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );

	if( holder->program == 0 )
		return 0;

	return holder->program->InfoLog;
}
