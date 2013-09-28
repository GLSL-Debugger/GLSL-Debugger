/*
 * ShaderLang.cpp
 *
 *  Created on: 04.09.2013
 */

#include "ShaderLang.h"
#include "ShaderHolder.h"
#include "Program.h"

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
struct gl_context *main_context;


static struct {
	DbgResult result;
	unsigned int position;
} g;


variableQualifier qualifierToNative(unsigned mode)
{
	switch( mode ){
		case ir_var_uniform:
			return SH_UNIFORM;
		case ir_var_shader_in:
			return SH_VARYING_IN;
		case ir_var_shader_out:
			return SH_VARYING_OUT;
		case ir_var_function_in:
			return SH_PARAM_IN;
		case ir_var_function_out:
			return SH_PARAM_OUT;
		case ir_var_function_inout:
			return SH_PARAM_INOUT;
		case ir_var_const_in: /**< "in" param that must be a constant expression */
			return SH_PARAM_CONST;
		case ir_var_system_value: /**< Ex: front-face, instance-id, etc. */
			return SH_BUILTIN_READ;
		case ir_var_temporary: /**< Temporary variable generated during compilation. */
			return SH_TEMPORARY;
			/* TODO: not sure about origin
			 ir_var_auto = 0,     // < Function local variables and globals.
			 SH_UNSET,
			 SH_GLOBAL,
			 SH_CONST,
			 SH_ATTRIBUTE,
			 SH_BUILTIN_WRITE
			 */
		default:
			return SH_GLOBAL;
	}
	return SH_GLOBAL;
}

ShVariable* glsltypeToShVariable(const struct glsl_type* vtype, const char* name,
		variableQualifier qualifier, unsigned modifier = SH_VM_NONE)
{
	ShVariable *v = NULL;
	v = (ShVariable*)malloc( sizeof(ShVariable) );

	// Type has no identifier! To be filled in later by TVariable
	v->uniqueId = -1;
	v->builtin = false;
	v->name = strdup( name );

#define SET_TYPE(glsl, native) \
    case glsl: \
        v->type = native; \
        break;

	// Type of variable (SH_FLOAT/SH_INT/SH_BOOL/SH_STRUCT)
	switch( vtype->base_type ){
		SET_TYPE( GLSL_TYPE_UINT, SH_UINT )
		SET_TYPE( GLSL_TYPE_INT, SH_INT )
		SET_TYPE( GLSL_TYPE_FLOAT, SH_FLOAT )
		SET_TYPE( GLSL_TYPE_BOOL, SH_BOOL )
		SET_TYPE( GLSL_TYPE_SAMPLER, SH_SAMPLER_GUARD_BEGIN )
		SET_TYPE( GLSL_TYPE_STRUCT, SH_STRUCT )
		SET_TYPE( GLSL_TYPE_ARRAY, SH_ARRAY )
		default:
			UT_NOTIFY_VA( LV_ERROR, "Type does not defined %x", vtype->gl_type );
			break;
	}

	// Qualifier of variable
	v->qualifier = qualifier;

	// Varying modifier
	v->varyingModifier = modifier;

	// Scalar/Vector size
	v->size = vtype->components();

	// Matrix handling
	v->isMatrix = vtype->is_matrix();
	v->matrixSize[0] = vtype->vector_elements;
	v->matrixSize[1] = vtype->matrix_columns;

	// Array handling
	v->isArray = vtype->is_array();
	for( int i = 0; i < MAX_ARRAYS; i++ ){
		v->arraySize[i] = vtype->array_size();
	}

	if( vtype->base_type == GLSL_TYPE_STRUCT ){
		//
		// Append structure to ShVariable
		//
		v->structSize = vtype->length;
		v->structSpec = (ShVariable**)malloc( v->structSize * sizeof(ShVariable*) );
		v->structName = strdup( vtype->name );
		for( int i = 0; i < v->structSize; ++i ){
			struct glsl_struct_field* field = &vtype->fields.structure[i];
			v->structSpec[i] = glsltypeToShVariable( field->type, field->name,
					qualifier );
		}
	}else{
		v->structName = NULL;
		v->structSize = 0;
		v->structSpec = NULL;
	}

	return v;
}

ShVariable* symbolToShVariable(_mesa_symbol_desc* symbol)
{
	symbol_table_entry* data = (symbol_table_entry*)symbol->data;
	ir_variable* variable = data->v;
	if( !variable ) // This is not a variable
		return NULL;
	const struct glsl_type* vtype = variable->type;
	variableQualifier qualifier = qualifierToNative( variable->mode );
	unsigned modifier = 0;
	modifier |= variable->invariant ? SH_VM_INVARIANT : SH_VM_NONE;
	modifier |=
			( variable->interpolation & INTERP_QUALIFIER_FLAT ) ? SH_VM_FLAT : SH_VM_NONE;
	modifier |=
			( variable->interpolation & INTERP_QUALIFIER_SMOOTH ) ?
					SH_VM_SMOOTH : SH_VM_NONE;
	modifier |=
			( variable->interpolation & INTERP_QUALIFIER_NOPERSPECTIVE ) ?
					SH_VM_NOPERSPECTIVE : SH_VM_NONE;
	modifier |= variable->centroid ? SH_VM_CENTROID : SH_VM_NONE;
	return glsltypeToShVariable( vtype, variable->name, qualifier, modifier );
}

static void initialize_context(struct gl_context *ctx, gl_api api)
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

void compile_shader(struct gl_context *ctx, struct gl_shader *shader, int debug)
{
	/* TODO: debug options
	 * 	EDebugOpIntermediate       = 0x001,
	 * 	EDebugOpAssembly           = 0x002,
	 * 	EDebugOpObjectCode         = 0x004,
	 * 	EDebugOpLinkMaps           = 0x008
	 */
	UNUSED_ARG(debug)
	int dump_ast = 1;
	int dump_hir = 1;
	int dump_lir = 1;
	struct _mesa_glsl_parse_state *state = new ( shader ) _mesa_glsl_parse_state( ctx,
			shader->Type, shader );

	_mesa_glsl_compile_shader( ctx, shader, dump_ast, dump_hir );

	/* Print out the resulting IR */
	if( !state->error && dump_lir ){
		_mesa_print_ir( shader->ir, state );
	}

	return;
}

//
// Driver must call this first, once, before doing any other
// compiler/linker operations.
//
int ShInitialize( )
{
	main_context = new struct gl_context;
	initialize_context( main_context, ( glsl_es ) ? API_OPENGLES2 : API_OPENGL_COMPAT );
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
	//holder->parse_context = NULL;
	return reinterpret_cast< void* >( holder );
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
	if( handle == 0 )
		return;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );
	if( holder->program ){
		for( unsigned i = 0; i < MESA_SHADER_TYPES; i++ )
			ralloc_free( holder->program->_LinkedShaders[i] );
		ralloc_free( holder->program );
		holder->program = NULL;
	}
	delete holder;
	holder = NULL;
}

//
// Remove shader context
//
int __fastcall ShFinalize( )
{
	_mesa_glsl_release_types();
	_mesa_glsl_release_functions();

	if( main_context ){
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
int ShCompile(const ShHandle handle, const char* const shaderStrings[],
		const int numStrings, const EShOptimizationLevel optLevel,
		const TBuiltInResource* resources, int debugOptions, ShVariableList *vl)
{
	// TODO: may be added to context.
	UNUSED_ARG(resources)
	// Also do not see something like in mesa yet.
	UNUSED_ARG(optLevel)

	if( handle == NULL )
		return 0;

	vl->numVariables = 0;
	vl->variables = NULL;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( handle );
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

		compile_shader( main_context, shader, debugOptions );

		// TODO: informative names
		if( !shader->CompileStatus ){
			UT_NOTIFY_VA( LV_ERROR,
					"Info log for %d:\n%s\n", shader->Name, shader->InfoLog );
			success = shader->CompileStatus;
			break;
		}

		_mesa_symbol_desc* symbols = shader->symbols->get_descs( 2 );
		while( symbols != NULL ){
			_mesa_symbol_desc* old = symbols;
			ShVariable* sh = symbolToShVariable( symbols );
			if( sh && sh->qualifier != SH_TEMPORARY )
				addShVariable( vl, sh, strncmp( sh->name, "gl_", 3 ) == 0 );
			symbols = symbols->next;
			free( old );
		}
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
//
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



/*
	s->

	struct gl_program_machine* machine = new_machine();
	_mesa_execute_program_steps(main_context, holder->program, machine, g.position, g.position + 1);
	g.position++;
*/
	/*
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
char* ShDebugGetProg(const ShHandle handle, ShChangeableList *cgbl, ShVariableList *vl,
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
int ShLink(const ShHandle linkHandle, const ShHandle compHandles[], const int numHandles,
		ShHandle uniformMapHandle, short int** uniformsAccessed, int* numUniformsAccessed)

{
	UNUSED_ARG( uniformMapHandle )
	UNUSED_ARG( uniformsAccessed )
	UNUSED_ARG( numUniformsAccessed )

	if( linkHandle == NULL )
		return 0;

	ShaderHolder* holder = reinterpret_cast< ShaderHolder* >( linkHandle );

	if( holder->program == 0 )
		return 0;

	link_shaders( main_context, holder->program );
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
