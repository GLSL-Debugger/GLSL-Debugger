/*
 * ShaderLang.cpp
 *
 *  Created on: 04.09.2013
 */

#include "glslang/Include/ShaderLang.h"
#include "ShaderHolder.h"
#include "Program.h"
#include "SymbolTable.h"
#include "Visitors/postprocess.h"

// Mesa includes
#include "glsl/standalone_scaffolding.h"
#include "glsl/glsl_parser_extras.h"
#include "glsl/glsl_symbol_table.cpp"
#include "glsl/program.h"
#include "glsl/ralloc.h"
#include "glsl/ast.h"
#include "mesa/main/mtypes.h"

#include "glsldb/utils/notify.h"

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void) x;
#endif

// Variables
int glsl_es = 0;
int glsl_version = 330;


static void initialize_context(struct gl_context *ctx, const TBuiltInResource* resources)
{
	// FIXME: right api
	initialize_context_to_defaults(ctx, API_OPENGL_COMPAT);
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

	ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs = resources->maxVertexAttribs;
	ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents = resources->maxVertexUniformComponents;
	ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents = resources->maxFragmentUniformComponents;
	ctx->Const.MaxVarying = resources->maxVaryingFloats;
	ctx->Const.MaxCombinedTextureImageUnits = resources->maxCombinedTextureImageUnits;
	ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = resources->maxVertexTextureImageUnits;
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

int addShVariableList(ShVariableList *vl, AstShader* shader)
{
	int count = 0;
	foreach_list(node, shader->head) {
		ast_node *ast = exec_node_data(ast_node, node, link);
		ast_declarator_list* dlist = ast->as_declarator_list();
		if (!dlist)
			continue;
		foreach_list_typed (ast_declaration, decl, link, &dlist->declarations){
			ShVariable* var = findShVariable(decl->debug_id);
			addShVariable(vl, var, var->builtin);
			count++;
		}
	}

	return count;
}

void compile_shader_to_ast(struct gl_context *ctx, struct AstShader *shader, int)
{
	struct _mesa_glsl_parse_state *state = new (shader) _mesa_glsl_parse_state(
			ctx, shader->stage, shader);
	const char *source = shader->source;
	state->error = glcpp_preprocess(state, &source, &state->info_log, &ctx->Extensions, ctx);

	if (!state->error) {
		_mesa_glsl_lexer_ctor(state, source);
		_mesa_glsl_parse(state);
		_mesa_glsl_lexer_dtor(state);
	}

	if (!state->error) {
		exec_list instructions;
		/* We need global variables later */
		_mesa_glsl_initialize_variables(&instructions, state);
		state->symbols->push_scope();
		shader->head = &state->translation_unit;
	}

	shader->compile_status = !state->error;
	shader->info_log = state->info_log;
	shader->version = state->language_version;
	shader->is_es = state->es_shader;

	if (!state->error) {
		/* Copy extensions */
		sh_extension* ext = state->explicit_extensions;
		while (ext) {
			sh_extension* sh_ext = ralloc(shader, sh_extension);
			sh_ext->behavior = ext->behavior;
			sh_ext->name = ralloc_strdup(shader, ext->name);
			sh_ext->next = shader->extensions;
			shader->extensions = sh_ext;
			ext = ext->next;
		}

		/* Copy shader qualifiers from state */
		size_t qual_size = sizeof(ast_type_qualifier);
		shader->qualifiers[SQ_DEFAULT_UNIFORM] = new (shader) ast_type_qualifier;
		memcpy(shader->qualifiers[SQ_DEFAULT_UNIFORM], state->default_uniform_qualifier,
				qual_size);
		shader->qualifiers[SQ_GS_OUT] = new (shader) ast_type_qualifier;
		memcpy(shader->qualifiers[SQ_GS_OUT], state->out_qualifier, qual_size);
		shader->qualifiers[SQ_GS_IN] = new (shader) ast_type_qualifier;
		memcpy(shader->qualifiers[SQ_GS_IN], state->in_qualifier, qual_size);

		/* Check side effects, discards, vertex emits */
		ast_postprocess_traverser_visitor ppt(shader, state);
		ppt.visit(shader->head);

		/* Processed in postprocessor instead of ast_to_hir */
		shader->gs_input_prim_type_specified = state->gs_input_prim_type_specified;
	}

	// TODO: steal memory
	//ralloc_free(state);

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
	ShaderHolder* holder = rzalloc(NULL, struct ShaderHolder);
	holder->language = language;
	holder->debug_options = debugOptions;
	holder->ctx = rzalloc(holder, struct gl_context);
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

	if( holder->ctx )
		ralloc_free(holder->ctx);

	ralloc_free(holder);
	holder = NULL;
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

	if (handle == NULL)
		return 0;

	clearTraverseDebugJump();

	vl->numVariables = 0;
	vl->variables = NULL;

	ShaderHolder* holder = reinterpret_cast<ShaderHolder*>(handle);
	initialize_context(holder->ctx, resources);

	bool success = true;
	for (int shnum = 0; numStrings > shnum; shnum++) {
		holder->shaders = reralloc(holder, holder->shaders,
						struct AstShader*, holder->num_shaders + 1);
		AstShader* shader = rzalloc(holder, struct AstShader);
		shader->symbols = new(holder) sh_symbol_table;
		holder->shaders[holder->num_shaders] = shader;
		holder->num_shaders++;

		shader->source = shaderStrings[shnum];
		shader->name = NULL;

		switch (holder->language) {
		case EShLangFragment:
			shader->stage = MESA_SHADER_FRAGMENT;
			break;
		case EShLangGeometry:
			shader->stage = MESA_SHADER_GEOMETRY;
			break;
		case EShLangVertex:
		default:
			shader->stage = MESA_SHADER_VERTEX;
			break;
		}

		char* old_locale = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, "POSIX");
		compile_shader_to_ast(holder->ctx, shader, debugOptions);
		setlocale(LC_NUMERIC, old_locale);

		// TODO: informative names
		if (!shader->compile_status) {
			UT_NOTIFY_VA( LV_ERROR, "Compilation failed, info log:\n%s\n", shader->info_log);
			success = shader->compile_status;
			break;
		}

		// Add global variables from ast tree
		addShVariableList(vl, shader);

		// Traverse tree for scope and variable processing
		// Each node gets data holding list of variables changes with this
		// operation and about scope information
		ShaderVarTraverse(shader, vl);
	}

	return success ? 1 : 0;
}

DbgResult* ShDebugJumpToNext(const ShHandle handle, int debugOptions, int dbgBh)
{
	DbgResult* result = NULL;

	if( handle == NULL )
		return 0;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );
	AstShader* shader = holder->shaders[0];
	result = ShaderTraverse(shader, debugOptions, dbgBh);
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
	AstShader* shader = holder->shaders[0];
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

//	if( holder->program == 0 )
//		return 0;
//
//	link_shaders( holder->ctx, holder->program );
//	int status = ( holder->program->LinkStatus ) ? EXIT_SUCCESS : EXIT_FAILURE;
//
//	if( strlen( holder->program->InfoLog ) > 0 )
//		printf( "Info log for linking:\n%s\n", holder->program->InfoLog );

	return 0; //status;
}

//
// Return any compiler/linker/uniformmap log of messages for the application.
//
const char* ShGetInfoLog(const ShHandle handle)
{
	if( handle == 0 )
		return NULL;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );

//	if( holder->program == 0 )
//		return 0;
//
//	return holder->program->InfoLog;
	return NULL;
}
