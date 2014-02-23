/*
 * Variable.cpp
 *
 *  Created on: 23.02.2014
 */

#include "ShaderLang.h"
#include "AstScope.h"
#include "glsldb/utils/dbgprint.h"
#include <map>
#include "ir.h"
#include "ast.h"


namespace {
	const char* identifierFromNode(ast_node* decl)
	{
		ast_declaration* as_decl = decl->as_declaration();
		ast_parameter_declarator* as_param = decl->as_parameter_declarator();
		ast_struct_specifier* as_struct = decl->as_struct_specifier();

		if (as_decl)
			return as_decl->identifier;
		else if (as_param)
			return as_param->identifier;
		else if (as_struct)
			return as_struct->name;
		else
			return NULL;
	}

	std::map<int, ShVariable*> VariablesById;
	unsigned int VariablesCount = 0;
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
	case SH_ARRAY:
		return "array";
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


variableQualifier qualifierFromAst(const ast_type_qualifier* qualifier, bool is_parameter)
{
	int qual = SH_UNSET;
	qual |= qualifier->flags.q.uniform * SH_UNIFORM;
	qual |= qualifier->flags.q.in * SH_IN;
	qual |= qualifier->flags.q.out * SH_OUT;
	qual |= qualifier->flags.q.varying * SH_VARYING;
	qual |= qualifier->flags.q.attribute * SH_ATTRIBUTE;
	qual |= qualifier->flags.q.constant * SH_CONST;
	qual |= is_parameter * SH_PARAM;
	if (!qual)
		qual = SH_GLOBAL;

	return (variableQualifier)qual;
}

variableQualifier qualifierFromIr(ir_variable* var)
{
//	TODO: not sure about origin
//	SH_CONST,
//	SH_ATTRIBUTE,

	switch (var->data.mode) {
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
		if (var->data.read_only)
			return SH_BUILTIN_READ;
		else
			return SH_BUILTIN_WRITE;
	case ir_var_temporary: /**< Temporary variable generated during compilation. */
		return SH_TEMPORARY;
	default:
		return SH_GLOBAL;
	}
	return SH_GLOBAL;
}

variableVaryingModifier modifierFromAst(const ast_type_qualifier* qualifier)
{
	variableVaryingModifier mod = SH_VM_NONE;
	mod |= qualifier->flags.q.invariant * SH_VM_INVARIANT;
	mod |= qualifier->flags.q.flat * SH_VM_FLAT;
	mod |= qualifier->flags.q.smooth * SH_VM_SMOOTH;
	mod |= qualifier->flags.q.noperspective * SH_VM_NOPERSPECTIVE;
	mod |= qualifier->flags.q.centroid * SH_VM_CENTROID;
	return mod;
}

variableVaryingModifier modifierFromIr(const ir_variable* var)
{
	variableVaryingModifier mod = SH_VM_NONE;
	mod |= var->data.invariant * SH_VM_INVARIANT;
	mod |= var->data.centroid * SH_VM_CENTROID;

	switch (var->data.interpolation) {
	case INTERP_QUALIFIER_SMOOTH:
		mod |= SH_VM_SMOOTH;
		break;
	case INTERP_QUALIFIER_FLAT:
		mod |= SH_VM_FLAT;
		break;
	case INTERP_QUALIFIER_NOPERSPECTIVE:
		mod |= SH_VM_NOPERSPECTIVE;
		break;
	default:
		break;
	}

	return mod;
}

variableVaryingModifier modifierFromGLSL(struct glsl_struct_field* field, int invariant)
{
	variableVaryingModifier mod = SH_VM_NONE;
	mod |= invariant;
	mod |= field->centroid * SH_VM_CENTROID;

	switch (field->interpolation) {
	case INTERP_QUALIFIER_SMOOTH:
		mod |= SH_VM_SMOOTH;
		break;
	case INTERP_QUALIFIER_FLAT:
		mod |= SH_VM_FLAT;
		break;
	case INTERP_QUALIFIER_NOPERSPECTIVE:
		mod |= SH_VM_NOPERSPECTIVE;
		break;
	default:
		break;
	}

	return mod;
}

bool ShIsSampler(variableType v)
{
	if (v < SH_SAMPLER_GUARD_BEGIN || SH_SAMPLER_GUARD_END < v) {
		return false;
	} else {
		return true;
	}
}


void addShVariable(ShVariableList *vl, ShVariable *v, int builtin)
{
	int i;
	ShVariable **vp = vl->variables;

	v->builtin = builtin;

	for (i = 0; i < vl->numVariables; i++) {
		if (strcmp(vp[i]->name, v->name) == 0) {
			vp[i] = v;
			return;
		}
	}

	vl->numVariables++;
	vl->variables = (ShVariable**) realloc(vl->variables,
			vl->numVariables * sizeof(ShVariable*));
	vl->variables[vl->numVariables - 1] = v;
}

ShVariable* findShVariable(int id)
{
	if (id >= 0) {
		std::map<int, ShVariable*>::iterator it = VariablesById.find(id);
		if (it != VariablesById.end())
			return it->second;
	}
	return NULL;
}

ShVariable* findShVariableFromId(ShVariableList *vl, int id)
{
	ShVariable **vp = NULL;
	int i;

	if (!vl)
		return NULL;

	vp = vl->variables;
	for (i = 0; i < vl->numVariables; i++) {
		if (vp[i]->uniqueId == id)
			return vp[i];
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
		// Remove variable from storages
		std::map<int, ShVariable*>::iterator iit = VariablesById.find(
				(*var)->uniqueId);
		if (iit != VariablesById.end())
			VariablesById.erase(iit);

		int i;
		free((*var)->name);
		for (i = 0; i < (*var)->structSize; i++)
			freeShVariable(&(*var)->structSpec[i]);
		free((*var)->structSpec);
		free((*var)->structName);
		free(*var);
		*var = NULL;
	}
}

void freeShVariableList(ShVariableList *vl)
{
	int i;
	for (i = 0; i < vl->numVariables; i++)
		freeShVariable(&vl->variables[i]);
	free(vl->variables);
	vl->numVariables = 0;
}

void glsltypeToShVariable(ShVariable* v, const struct glsl_type* vtype,
		variableQualifier qualifier, variableVaryingModifier modifier,
		void* mem_ctx)
{
#define SET_TYPE(glsl, native) \
    case glsl: \
        v->type = native; \
        break;

	// Type of variable (SH_FLOAT/SH_INT/SH_BOOL/SH_STRUCT)
	switch (vtype->base_type) {
	SET_TYPE( GLSL_TYPE_UINT, SH_UINT)
	SET_TYPE( GLSL_TYPE_INT, SH_INT)
	SET_TYPE( GLSL_TYPE_FLOAT, SH_FLOAT)
	SET_TYPE( GLSL_TYPE_BOOL, SH_BOOL)
	SET_TYPE( GLSL_TYPE_SAMPLER, SH_SAMPLER_GUARD_BEGIN)
	SET_TYPE( GLSL_TYPE_STRUCT, SH_STRUCT)
	SET_TYPE( GLSL_TYPE_ARRAY, SH_ARRAY)
	default:
		dbgPrint( DBGLVL_ERROR, "Type does not defined %x", vtype->gl_type);
		break;
	}

#undef SET_TYPE

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
	for (int i = 0; i < MAX_ARRAYS; i++) {
		v->arraySize[i] = vtype->array_size();
	}

	if (vtype->base_type == GLSL_TYPE_STRUCT) {
		// Add variables recursive
		v->structSize = vtype->length;
		v->structSpec = (ShVariable**) malloc(v->structSize * sizeof(ShVariable*));
		v->structName = strdup(vtype->name);
		for (int i = 0; i < v->structSize; ++i) {
			struct glsl_struct_field* field = &vtype->fields.structure[i];
			ShVariable* var = (ShVariable*)rzalloc(mem_ctx, ShVariable);
			var->uniqueId = -1;
			if (qualifier & SH_BUILTIN)
				var->qualifier = SH_BUILTIN;
			var->name = strdup(field->name);
			int field_mod = modifierFromGLSL(field, modifier & SH_VM_INVARIANT);
			glsltypeToShVariable(var, field->type, qualifier, field_mod, mem_ctx);
			v->structSpec[i] = var;
		}
	} else {
		v->structName = NULL;
		v->structSize = 0;
		v->structSpec = NULL;
	}
}

void addAstShVariable(ast_node* decl, ShVariable* var)
{
	var->uniqueId = VariablesCount++;
	VariablesById[var->uniqueId] = var;
	decl->debug_id = var->uniqueId;
}


ShVariable* astToShVariable(ast_node* decl, variableQualifier qualifier,
		variableVaryingModifier modifier, const struct glsl_type* decl_type,
		void* mem_ctx)
{
	if (!decl)
		return NULL;

	const char* identifier = identifierFromNode(decl);
	if (!identifier)
		return NULL;

	ShVariable* var = findShVariable(decl->debug_id);
	if (!var) {
		var = (ShVariable*)rzalloc(mem_ctx, ShVariable);
		var->uniqueId = -1;
		var->builtin = qualifier & SH_BUILTIN;
		var->name = strdup(identifier);
		glsltypeToShVariable(var, decl_type, qualifier, modifier, mem_ctx);
		addAstShVariable(decl, var);
	}

	return var;
}


void ShDumpVariable(ShVariable *v, int depth)
{
	int i;

	for (i = 0; i < depth; i++)
		dbgPrint(DBGLVL_COMPILERINFO, "    ");

	if (0 <= v->uniqueId)
		dbgPrint(DBGLVL_COMPILERINFO, "<%i> ", v->uniqueId);

	if (v->builtin)
		dbgPrint(DBGLVL_COMPILERINFO, "builtin ");

	dbgPrint(DBGLVL_COMPILERINFO,
			"%s %s", getShQualifierString(v->qualifier), getShTypeString(v));

	if (v->isMatrix) {
		dbgPrint(DBGLVL_COMPILERINFO, "%ix%i", v->size, v->size);
	} else {
		if (1 < v->size)
			dbgPrint(DBGLVL_COMPILERINFO, "%i", v->size);
	}

	dbgPrint(DBGLVL_COMPILERINFO, " %s", v->name);

	if (v->isArray) {
		for (i = 0; i < MAX_ARRAYS; i++) {
			if (v->arraySize[i] < 0)
				break;
			dbgPrint(DBGLVL_COMPILERINFO, "[%i]", v->arraySize[i]);
		}
		dbgPrint(DBGLVL_COMPILERINFO, "\n");
	} else {
		dbgPrint(DBGLVL_COMPILERINFO, "\n");
	}

	if (v->structSize != 0) {
		depth++;
		for (i = 0; i < v->structSize; i++)
			ShDumpVariable(v->structSpec[i], depth);
	}

}


ShVariable* findFirstShVariableFromName(ShVariableList *vl, const char *name)
{
	ShVariable **vp = NULL;
	int i;

	if (!vl)
		return NULL;

	vp = vl->variables;
	for (i = 0; i < vl->numVariables; i++) {
		if (!(strcmp(vp[i]->name, name)))
			return vp[i];
	}

	return NULL;
}
