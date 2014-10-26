
#include "Shader.h"
#include "CodeInsertion.h"
#include "CodeTools.h"
#include "AstStack.h"
#include "ShaderHolder.h"
#include "SymbolTable.h"
#include "glsldb/utils/dbgprint.h"

#include <map>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CG_RESULT_PREFIX    "dbgResult"
#define CG_CONDITION_PREFIX "dbgCond"
#define CG_PARAMETER_PREFIX "dbgParam"
#define CG_LOOP_ITER_PREFIX "dbgIter"
#define CG_FUNCTION_POSTFIX "DBG"
#define CG_RANDOMIZED_POSTFIX_SIZE 3


struct ltstr {
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

typedef std::map<const char*, const char*, ltstr> strMap;
typedef std::list<const char*> strList;

static struct {
	strMap nameMap;
	strList loopIters;
	int numLoopIters;
} g;


CodeGen::CodeGen(AstShader* _sh, ShVariableList* _vl, ShChangeableList* _cgbls)
{
	shader = _sh;
	vl = _vl;
	cgbls = _cgbls;
	result = condition = parameter = NULL;
	defined_constructions = 0;
	mem = ralloc_context(NULL);

	g.nameMap.clear();
	g.numLoopIters = 0;
	g.loopIters.clear();
}


CodeGen::~CodeGen()
{
	destruct(CG_TYPE_ALL);
	ralloc_free(mem);
}


static ShVariable* createDefault(cgTypes type, EShLanguage l, void* mem_ctx)
{
	ShVariable* ret = (ShVariable*) rzalloc(mem_ctx, ShVariable);
	ret->uniqueId = -1;
	ret->size = 1;

	switch (type) {
	case CG_TYPE_RESULT:
		ret->type = SH_FLOAT;
		switch (l) {
		case EShLangVertex:
		case EShLangGeometry:
			ret->qualifier = SH_VARYING_OUT;
			break;
		case EShLangFragment:
		default:
			ret->qualifier = SH_TEMPORARY;
			break;
		}
		break;
	case CG_TYPE_CONDITION:
		ret->type = SH_BOOL;
		ret->qualifier = SH_TEMPORARY;
		break;
	case CG_TYPE_PARAMETER:
		dbgPrint(DBGLVL_WARNING, "CodeInsertion - cannot create default of CG_TYPE_PARAMETER\n");
		/* No break */
	default:
		ralloc_free(ret);
		return NULL;
	}

	return ret;
}

static char* getUnusedNameByPrefix(ShVariableList *vl, const char *prefix, void* mem_ctx)
{
	char* new_name = NULL;

	new_name = (char*) rzalloc_size(mem_ctx, strlen(prefix) + 1 + CG_RANDOMIZED_POSTFIX_SIZE);
	assert(new_name || !"CodeInsertion - not enough memory for result name\n");
	strcpy(new_name, prefix);

	if (findFirstShVariableFromName(vl, prefix)) {
		while (findFirstShVariableFromName(vl, new_name)) {
			new_name[strlen(prefix)] = '\0';
			for (int i = 0; i < CG_RANDOMIZED_POSTFIX_SIZE; i++) {
				char ran = (char) ((rand() / (float) RAND_MAX) * ('Z' - 'A') + 'A');
				strncat(new_name, &ran, 1);
			}
		}
	}

	return new_name;
}

void CodeGen::getNewName(char **name, const char *prefix)
{
	*name = getUnusedNameByPrefix(vl, prefix, shader);
}

static char* getNewUnusedName(cgTypes type, ShVariableList *vl, EShLanguage l, void* mem_ctx)
{
	char* new_name = NULL;
	switch (type) {
	case CG_TYPE_RESULT:
		switch (l) {
		case EShLangVertex:
		case EShLangGeometry:
			new_name = (char*) rzalloc_size(mem_ctx, strlen(CG_RESULT_PREFIX) + 1);
			assert(new_name || !"CodeInsertion - not enough memory for result name\n");
			strcpy(new_name, CG_RESULT_PREFIX);
			break;
		case EShLangFragment:
			new_name = getUnusedNameByPrefix(vl, CG_RESULT_PREFIX, mem_ctx);
			break;
		default:
			break;
		}
		break;
	case CG_TYPE_CONDITION:
		new_name = getUnusedNameByPrefix(vl, CG_CONDITION_PREFIX, mem_ctx);
		break;
	case CG_TYPE_PARAMETER:
		new_name = getUnusedNameByPrefix(vl, CG_PARAMETER_PREFIX, mem_ctx);
		break;
	default:
		break;
	}

	return new_name;
}

void CodeGen::init(cgTypes type, ShVariable *src, EShLanguage l)
{
	ShVariable **var = NULL;

	switch (type) {
	case CG_TYPE_RESULT:
		var = &result;
		break;
	case CG_TYPE_CONDITION:
		var = &condition;
		if (src)
			dbgPrint(DBGLVL_WARNING, "CodeInsertion - user defined condition types not supported\n");
		break;
	case CG_TYPE_PARAMETER:
		var = &parameter;
		if (!src)
			dbgPrint(DBGLVL_WARNING, "CodeInsertion - must provide user defined parameter types\n");
		break;
	default:
		return;
	}

	if (*var)
		destruct(type);

	if (!src)
		*var = createDefault(type, l, shader);
	else
		*var = copyShVariableCtx(src, shader);

	/* Assign non-used name */
	(*var)->name = getNewUnusedName(type, vl, l, shader);
	ShDumpVariable(*var, 1);
}

void CodeGen::initTarget(ast_function_expression* node, EShLanguage l, DbgCgOptions)
{
	if (!node)
		return;

	/* Check if an optional parameter register is neccessary */
	const char* func_name = node->subexpressions[0]->primary_expression.identifier;
	ast_function_definition* func = shader->symbols->get_function(func_name);
	/* Check if a parameter is used for code insertion */
	int lastInParameter = getFunctionDebugParameter(func);
	if (lastInParameter >= 0) {
		ast_node* t = getSideEffectsDebugParameter(node, lastInParameter);
		assert((t && t->debug_id >= 0) || !"CodeGen - side effects returned type is invalid");
		ShVariable* var = findShVariable(t->debug_id);
		init(CG_TYPE_PARAMETER, var, l);
	}
}

void CodeGen::initTarget(ast_selection_statement* node, EShLanguage l, DbgCgOptions o)
{
	/* Check if a selection condition needs to be copied */
	if (!node || !(o == DBG_CG_COVERAGE || o == DBG_CG_SELECTION_CONDITIONAL
			|| o == DBG_CG_CHANGEABLE || o == DBG_CG_GEOMETRY_CHANGEABLE))
		return;

	switch (node->debug_state_internal) {
	case ast_dbg_if_condition_passed:
	case ast_dbg_if_then:
	case ast_dbg_if_else:
		init(CG_TYPE_CONDITION, NULL, l);
		break;
	default:
		break;
	}
}

void CodeGen::initTarget(ast_switch_statement* node, EShLanguage l, DbgCgOptions o)
{
	/* Check if a selection condition needs to be copied */
	if (!node || !(o == DBG_CG_COVERAGE  || o == DBG_CG_SWITCH_CONDITIONAL
			|| o == DBG_CG_CHANGEABLE || o == DBG_CG_GEOMETRY_CHANGEABLE))
		return;

	if (node->debug_state_internal == ast_dbg_switch_condition_passed
			|| node->debug_state_internal == ast_dbg_switch_branch) {
		init(CG_TYPE_CONDITION, NULL, l);
		this->condition->type = SH_INT;
	}
}

void CodeGen::initTarget(ast_iteration_statement* node, EShLanguage l, DbgCgOptions o)
{
	if (!node || !(o == DBG_CG_COVERAGE || o == DBG_CG_LOOP_CONDITIONAL
			|| o == DBG_CG_CHANGEABLE || o == DBG_CG_GEOMETRY_CHANGEABLE))
		return;

	/* Add debug temoprary register to copy condition */
	if (node->debug_state_internal == ast_dbg_loop_select_flow)
		init(CG_TYPE_CONDITION, NULL, l);
}

void CodeGen::allocateResult(ast_node* target, EShLanguage language, DbgCgOptions options)
{
	dbgPrint(DBGLVL_COMPILERINFO, "initialize CG_TYPE_RESULT for %i\n", language);
	int size = (options == DBG_CG_GEOMETRY_MAP || options == DBG_CG_VERTEX_COUNT) ? 3 :
				(options == DBG_CG_GEOMETRY_CHANGEABLE) ? 2 : 0;
	ShVariable* ret = NULL;
	if (size) {
		ret = (ShVariable*)rzalloc(shader, ShVariable);
		ret->uniqueId = -1;
		ret->size = size;
		ret->type = SH_FLOAT;
		ret->qualifier = SH_VARYING_OUT;
	}

	init(CG_TYPE_RESULT, ret, language);

	if (ret)
		ralloc_free(ret);

	if (!target)
		return;

	initTarget(target->as_function_expression(), language, options);
	initTarget(target->as_selection_statement(), language, options);
	initTarget(target->as_switch_statement(), language, options);
	initTarget(target->as_iteration_statement(), language, options);
}

static const char* getQualifierCode(ShVariable *v, EShLanguage l, int version)
{
	switch (v->qualifier) {
	case SH_VARYING_OUT:
		if (version > 120 || l == EShLangGeometry)
			return "out ";
		else
			return "varying ";
	case SH_UNIFORM:
		return "uniform ";
	default:
		return "";
	}
}

static std::string getTypeCode(ShVariable *v, bool reduceToScalar = false)
{
	if (v->size == 1 || reduceToScalar) {
		switch (v->type) {
		case SH_FLOAT:
			return "float";
		case SH_INT:
			return "int";
		case SH_UINT:
			return "unsigned int";
		case SH_BOOL:
			return "bool";
		default:
			dbgPrint(DBGLVL_WARNING, "CodeInsertion - queried type of invalid type\n");
			return "";
		}
	} else {
		char out[100];
		switch (v->type) {
		case SH_FLOAT:
			sprintf(out, "vec%i", v->size);
			break;
		case SH_INT:
			sprintf(out, "ivec%i", v->size);
			break;
		case SH_UINT:
			sprintf(out, "uvec%i", v->size);
			return out;
		case SH_BOOL:
			sprintf(out, "bvec%i", v->size);
			break;
		default:
			dbgPrint(DBGLVL_WARNING, "CodeInsertion - queried type of invalid type\n");
			out[0] = '\0';
			break;
		}
		return std::string(out);
	}
}

void CodeGen::addDeclaration(cgTypes type, char** prog, EShLanguage l)
{
	switch (type) {
	case CG_TYPE_RESULT:
		if (result)
			ralloc_asprintf_append(prog, "%s%s %s;\n", getQualifierCode(result, l, shader->version),
					getTypeCode(result).c_str(), result->name);
		break;
	case CG_TYPE_CONDITION:
		if (condition)
			ralloc_asprintf_append(prog, "%s%s %s;\n", getQualifierCode(condition, l, shader->version),
					getTypeCode(condition).c_str(), condition->name);
		break;
	case CG_TYPE_PARAMETER:
		if (parameter) {
			ralloc_asprintf_append(prog, "%s%s %s", getQualifierCode(parameter, l, shader->version),
					getTypeCode(parameter).c_str(), parameter->name);
			if (parameter->isArray)
				ralloc_asprintf_append(prog, "[%i]", parameter->arraySize[0]);
			ralloc_asprintf_append(prog, ";\n");
		}
		break;
	case CG_TYPE_LOOP_ITERS:
		for (strList::iterator it = g.loopIters.begin(); it != g.loopIters.end(); it++)
			ralloc_asprintf_append(prog, "int %s;\n", *it);
		break;
	case CG_TYPE_ALL:
		addDeclaration(CG_TYPE_RESULT, prog, l);
		addDeclaration(CG_TYPE_CONDITION, prog, l);
		addDeclaration(CG_TYPE_PARAMETER, prog, l);
		addDeclaration(CG_TYPE_LOOP_ITERS, prog, l);
		break;
	default:
		break;
	}
}

static const char* getInitializationCode(cgInitialization init)
{
	switch (init) {
	case CG_INIT_BLACK:
		return "0.0";
		break;
	case CG_INIT_WHITE:
		return "1.0";
		break;
	case CG_INIT_CHESS:
		return "(mod(floor(gl_FragCoord.x/8.0), 2.0) == "
				"mod(floor(gl_FragCoord.y/8.0), 2.0)) ? 1.0 : 0.8";
		break;
	case CG_INIT_GEOMAP:
		return "0.0, 0.0, gl_PrimitiveIDIn";
		break;
	}

	return "";
}

void CodeGen::addInitialization(cgTypes type, cgInitialization init, char** prog)
{
	ShVariable* tgt = NULL;
	switch (type) {
	case CG_TYPE_RESULT:
		tgt = result;
		break;
	case CG_TYPE_CONDITION:
		tgt = condition;
		break;
	case CG_TYPE_PARAMETER:
		tgt = parameter;
		break;
	default:
		break;
	}
	if (tgt)
		ralloc_asprintf_append(prog, "%s = %s", tgt->name, getTypeCode(tgt).c_str());
	ralloc_asprintf_append(prog, "(%s)", getInitializationCode(init));
}

static void put_fragment_output(char** prog, char* name, exec_list* instructions)
{
	const char* output_name = NULL;

	foreach_list_typed(ast_node, node, link, instructions) {
		ast_declarator_list* decl_list = node->as_declarator_list();
		if (!decl_list || !decl_list->type->qualifier.flags.q.out)
			continue;
		foreach_list_typed(ast_declaration, decl, link, &decl_list->declarations) {
			if (!strcmp(decl->identifier, "gl_FragColor")
					|| !strcmp(decl->identifier, "gl_FragData")
					|| strncmp(decl->identifier, "gl_", 3) != 0)
				output_name = decl->identifier;
		}
		if (output_name)
			break;
	}

	if (!output_name) {
		dbgPrint(DBGLVL_WARNING, "CodeInsertion - no valid output method set for fragment program.\n");
		dbgPrint(DBGLVL_WARNING, "CodeInsertion - assume gl_FragColor for further usage.\n");
		output_name = "gl_FragColor";
	} else if (!strcmp(output_name, "gl_FragData")) {
		output_name = "gl_FragData[0]";
	}

	ralloc_asprintf_append(prog, "%s.x = %s;\n", output_name, name);
}

void CodeGen::addOutput(cgTypes type, char** prog, EShLanguage l)
{
	/* TODO: fill out other possibilities */
	switch (l) {
	case EShLangVertex:
		break;
	case EShLangGeometry:
		if (type == CG_TYPE_RESULT)
			if (!defined(GS_EMIT_VERTEX | GS_END_PRIMITIVE)) {
				if (!defined(GS_EMIT_VERTEX))
					ralloc_asprintf_append(prog, "EmitVertex();");
				if (!defined(GS_END_PRIMITIVE))
					ralloc_asprintf_append(prog, "EndPrimitive();");
				ralloc_asprintf_append(prog, "\n");
			}
		break;
	case EShLangFragment:
		if (type == CG_TYPE_RESULT)
			put_fragment_output(prog, result->name, shader->head);
		break;
	default:
		break;
	}
}

char* itoSwizzle(int i)
{
	char* swizzle = new char[2];

	switch (i) {
	case 0:
		strcpy(swizzle, "x");
		break;
	case 1:
		strcpy(swizzle, "y");
		break;
	case 2:
		strcpy(swizzle, "z");
		break;
	case 3:
		strcpy(swizzle, "w");
		break;
	}
	return swizzle;
}

char* itoMultiSwizzle(int i)
{
	int k, j;
	char* swizzle = new char[5];
	swizzle[0] = '\0';

	dbgPrint(DBGLVL_COMPILERINFO, "%i\n", i);

	for (k = (int) ceil(log10((float) i)), j = 0; k > 0; k--, j++) {
		int d = (int) (i / pow(10, (float) (k - 1)));

		dbgPrint(DBGLVL_COMPILERINFO, "d:%i k:%i i:%i\n", d, k, i);

		switch (d - 1) {
		case 0:
			swizzle[j] = 'x';
			break;
		case 1:
			swizzle[j] = 'y';
			break;
		case 2:
			swizzle[j] = 'z';
			break;
		case 3:
			swizzle[j] = 'w';
			break;
		}
		i -= (int) (d * (pow(10, (float) (k - 1))));
	}
	swizzle[j] = '\0';

	return swizzle;
}

static void addVariableCode(char** prog, ShChangeable *cgb, ShVariableList *vl)
{
	ShVariable *var;

	assert(cgb && vl || !"CodeInsertion - called getVariableType without valid parameter\n");

	var = findShVariableFromId(vl, cgb->id);
	assert(var || !"CodeInsertion - called getVariableType without valid parameter\n");
	ralloc_asprintf_append(prog, "%s", var->name);

	if (!var->builtin && var->qualifier != SH_VARYING_IN && var->qualifier != SH_VARYING_OUT
			&& var->qualifier != SH_UNIFORM && var->qualifier != SH_ATTRIBUTE)
		ralloc_asprintf_append(prog, "_%i", var->uniqueId);

	int i;
	for (i = 0; i < cgb->numIndices; i++) {
		ShChangeableIndex *idx = cgb->indices[i];

		switch (idx->type) {
		case SH_CGB_ARRAY_INDIRECT:
			ralloc_asprintf_append(prog, "[%i]", idx->index);
			break;
		case SH_CGB_ARRAY_DIRECT:
			// FIXME: Leak ahead
			ralloc_asprintf_append(prog, ".%s", itoSwizzle(idx->index));
			break;
		case SH_CGB_STRUCT:
			assert(idx->index < var->structSize || !"CodeInsertion - struct and changeable do not match\n");
			var = var->structSpec[idx->index];
			ralloc_asprintf_append(prog, ".%s", var->name);
			break;
		case SH_CGB_SWIZZLE:
			// FIXME: Leak ahead
			ralloc_asprintf_append(prog, ".%s", itoMultiSwizzle(idx->index));
			break;
		}
	}
}

static int getVariableSizeByArrayIndices(ShVariable *var, int numOfArrayIndices)
{
	switch (numOfArrayIndices) {
	case 0:
		if (var->isArray)
			return var->arraySize[0] * var->size;
		else if (var->structSpec)
			return var->structSize;
		else if (var->isMatrix)
			return var->matrixSize[0] * var->matrixSize[1];
		else
			return var->size;
	case 1:
		if (var->isArray) {
			if (var->structSpec)
				return var->structSize;
			else
				return var->size;
		} else if (var->isMatrix) {
			/* HINT: glsl1.2 requires change here */
			return var->size;
		} else if (var->size > 1) {
			return 1;
		} else {
			assert(!"CodeInsertion - array subscript to a non-array variable\n");
		}
		break;
	case 2:
		if (var->isArray) {
			if (var->isMatrix) {
				/* HINT: glsl1.2 requires change here */
				return var->size;
			} else if (var->size > 1) {
				return 1;
			} else {
				assert(!"CodeInsertion - array subscript to a non-array variable\n");
			}
		} else if (var->isMatrix) {
			return 1;
		} else {
			assert(!"CodeInsertion - array subscript to a non-array variable\n");
		}
		break;
	case 3:
		assert(var->isArray && var->isMatrix || !"CodeInsertion - array subscript to a non-array variable\n");
		return 1;
	default:
		dbgPrint(DBGLVL_ERROR, "CodeInsertion - too many array subscripts (%i)\n",
				numOfArrayIndices);
		assert(0);
		break;
	}

	return 0;
}

static int getShChangeableSize(ShChangeable *cgb, ShVariableList *vl)
{
	int size, i;
	int arraySub = 0;
	ShVariable *var;

	if (!cgb || !vl)
		return 0;

	var = findShVariableFromId(vl, cgb->id);
	if (!var)
		return 0;

	size = var->size;

	for (i = 0; i < cgb->numIndices; i++) {
		ShChangeableIndex *idx = cgb->indices[i];
		switch (idx->type) {
		case SH_CGB_ARRAY_INDIRECT:
			arraySub++;
			size = getVariableSizeByArrayIndices(var, arraySub);
			break;
		case SH_CGB_ARRAY_DIRECT:
			/* TODO: check assumption that size resolves to '1' */
			size = 1;
			break;
		case SH_CGB_STRUCT:
			assert(idx->index < var->structSize || !"CodeInsertion - struct and changeable do not match\n");
			var = var->structSpec[idx->index];
			arraySub = 0;
			size = getVariableSizeByArrayIndices(var, arraySub);
			break;
		case SH_CGB_SWIZZLE:
			size = (int) ceil(log10((float) idx->index));
			break;
		}
	}
	return size;
}

static void addVariableCodeFromList(char** prog, ShChangeableList* cgbl, ShVariableList *vl,
		int targetSize)
{
	int id;
	int size = 0;

	if (!cgbl) {
		dbgPrint(DBGLVL_WARNING, "CodeInsertion - no changeable list given to generate code\n");
		return;
	}

	for (id = 0; id < cgbl->numChangeables; id++) {
		addVariableCode(prog, cgbl->changeables[id], vl);
		size += getShChangeableSize(cgbl->changeables[id], vl);

		/* Only add seperator if not last item */
		if (id < (cgbl->numChangeables - 1))
			ralloc_asprintf_append(prog, ", ");
	}

	if (size > 4) {
		dbgPrint(DBGLVL_WARNING,
				"CodeInsertion - given changeables exeed single request batch size by %i\n",
				size - 4);
	}

	for (id = size; id < targetSize; id++)
		ralloc_asprintf_append(prog, ", 0.0");
}

static bool hasLoop(AstStack* stack)
{
	foreach_stack(node, stack) {
		ast_iteration_statement* loop = node->as_iteration_statement();
		if (loop && loop->need_dbgiter())
			return true;
	}
	return false;
}

static void addLoopHeader(char** prog, AstStack* stack)
{
	if (!hasLoop(stack))
		return;

	ralloc_asprintf_append(prog, "(");

	/* for each loop node inside stack, e.g. dbgPath add condition */
	foreach_stack(node, stack) {
		ast_iteration_statement* loop = node->as_iteration_statement();
		if (loop && loop->need_dbgiter()) {
			if (loop->debug_iter_name == NULL)
				dbgPrint(DBGLVL_ERROR, "No iter name, crash ahead");
			ralloc_asprintf_append(prog, "%s == %i && ", loop->debug_iter_name, loop->debug_iter);
		}
	}

	ralloc_asprintf_append(prog, "(");
}

static void addLoopFooter(char** prog, AstStack* stack)
{
	if (hasLoop(stack))
		ralloc_asprintf_append(prog, ", true))");
}

/* 'option' semantics:
 *     DBG_CG_SELECTION_CONDITIONAL: branch (true or false)
 *     DBG_CG_GEOMETRY_MAP:          EmitVertex or EndPrimitive
 */
void CodeGen::addDbgCode(cgTypes type, char** prog, DbgCgOptions cgOptions, int option, GLenum outPrimType)
{
	/* TODO: fill out other possibilities */
	switch (type) {
	case CG_TYPE_RESULT: {
		/* Add additional overhead if inside loop */
		if (cgOptions != DBG_CG_GEOMETRY_CHANGEABLE
				|| (option != CG_GEOM_CHANGEABLE_IN_SCOPE
						&& option != CG_GEOM_CHANGEABLE_NO_SCOPE)) {
			addLoopHeader(prog, &shader->path);
		}
		std::string stype = getTypeCode(result);
		const char* type_code = stype.c_str();
		switch (cgOptions) {
		case DBG_CG_COVERAGE:
			ralloc_asprintf_append(prog, "%s = %s(1.0)", result->name, type_code);
			break;
		case DBG_CG_SELECTION_CONDITIONAL:
			ralloc_asprintf_append(prog, "%s = %s(%f)", result->name, type_code,
					option ? 1.0 : 0.5);
			break;
		case DBG_CG_SWITCH_CONDITIONAL: {
			float optf = (float)option / (float)DBG_BH_SWITCH_BRANCH_LAST;
			ralloc_asprintf_append(prog, "%s = %s(%f)", result->name, type_code, optf);
			break;
		}
		case DBG_CG_LOOP_CONDITIONAL:
			ralloc_asprintf_append(prog, "%s = %s(%s)", result->name, type_code, condition->name);
			break;
		case DBG_CG_CHANGEABLE:
			ralloc_asprintf_append(prog, "%s = %s(", result->name, type_code);
			addVariableCodeFromList(prog, cgbls, vl, getVariableSizeByArrayIndices(result, 0));
			ralloc_asprintf_append(prog, ")");
			break;
		case DBG_CG_GEOMETRY_MAP:
			/* option: '0' EmitVertex
			 *         '1' EndPrimitive */
			ralloc_asprintf_append(prog, "%s = %s", result->name, type_code);
			if (option)
				ralloc_asprintf_append(prog, "(dbgResult.x + 1, 0.0, gl_PrimitiveIDIn)");
			else
				ralloc_asprintf_append(prog,
						"(dbgResult.x, dbgResult.y + 1, gl_PrimitiveIDIn)");
			break;
		case DBG_CG_GEOMETRY_CHANGEABLE:
			switch (option) {
			case CG_GEOM_CHANGEABLE_AT_TARGET:
				ralloc_asprintf_append(prog, "%s = %s(0.0)", result->name, type_code);
				break;
			case CG_GEOM_CHANGEABLE_IN_SCOPE:
				ralloc_asprintf_append(prog, "%s = %s(", result->name, type_code);
				addVariableCodeFromList(prog, cgbls, vl, 0);
				ralloc_asprintf_append(prog, ", abs(%s.y))", result->name);
				break;
			case CG_GEOM_CHANGEABLE_NO_SCOPE:
				ralloc_asprintf_append(prog, "%s = %s(0.0, -abs(%s.y))", result->name,
						type_code, result->name);
				break;
			}
			break;
		case DBG_CG_VERTEX_COUNT:
			/* option: '0' EmitVertex
			 *         '1' EndPrimitive */
			ralloc_asprintf_append(prog, "%s = %s", result->name, type_code);
			if (option) {
				switch (outPrimType) {
				case GL_POINTS:
					ralloc_asprintf_append(prog,
							"(dbgResult.y > 0 ? dbgResult.x + 1 : dbgResult.x, 0.0, gl_PrimitiveIDIn)");
					break;
				case GL_LINE_STRIP:
					ralloc_asprintf_append(prog,
							"(dbgResult.y > 1 ? dbgResult.x + dbgResult.y : dbgResult.x, 0.0, gl_PrimitiveIDIn)");
					break;
				case GL_TRIANGLE_STRIP:
					ralloc_asprintf_append(prog,
							"(dbgResult.y > 2 ? dbgResult.x + dbgResult.y : dbgResult.x, 0.0, gl_PrimitiveIDIn)");
					break;
				}
			} else
				ralloc_asprintf_append(prog,
						"(dbgResult.x, dbgResult.y + 1, gl_PrimitiveIDIn)");
			break;
		default:
			break;
		}
		/* Add additional overhead if inside loop */
		if (cgOptions != DBG_CG_GEOMETRY_CHANGEABLE
				|| (option != CG_GEOM_CHANGEABLE_IN_SCOPE
						&& option != CG_GEOM_CHANGEABLE_NO_SCOPE)) {
			addLoopFooter(prog, &shader->path);
		}
		break;
	}
	case CG_TYPE_PARAMETER:
		ralloc_asprintf_append(prog, "%s", parameter->name);
		break;
	case CG_TYPE_CONDITION:
		ralloc_asprintf_append(prog, "%s", condition->name);
		break;
	default:
		break;
	}

}

void CodeGen::destruct(cgTypes type)
{
	switch (type) {
	case CG_TYPE_RESULT:
		freeShVariable(&result);
		break;
	case CG_TYPE_CONDITION:
		freeShVariable(&condition);
		break;
	case CG_TYPE_PARAMETER:
		freeShVariable(&parameter);
		break;
	case CG_TYPE_LOOP_ITERS:
		break;
	case CG_TYPE_ALL:
		destruct(CG_TYPE_RESULT);
		destruct(CG_TYPE_CONDITION);
		destruct(CG_TYPE_PARAMETER);
		break;
	default:
		break;
	}
}

static const char* getNewUnusedFunctionName(const char *input, AstShader* shader)
{
	char *output;
	size_t baseLen;
	int i;
	char* func_name = getFunctionName(input);
	size_t fn_len = strlen(func_name);

	output = (char*) rzalloc_size(shader, fn_len + strlen(CG_FUNCTION_POSTFIX) +
									CG_RANDOMIZED_POSTFIX_SIZE + 1);
	assert(output || !"CodeInsertion - not enough memory to create debug function name");

	strncpy(output, func_name, fn_len);
	strcat(output, CG_FUNCTION_POSTFIX);
	baseLen = strlen(output);
	free(func_name);

    while (shader->symbols->get_function(output)) {
        output[baseLen] = '\0';
        for (i=0; i<CG_RANDOMIZED_POSTFIX_SIZE; i++) {
            output[baseLen+i] = (char)((rand()/(float)RAND_MAX)*('Z'-'A')+'A');
        }
        output[baseLen+i] = '\0';
    }

	return output;
}


const char* CodeGen::getDebugName(const char *input)
{
	strMap::iterator it = g.nameMap.find(input);
	if (it != g.nameMap.end()) {
		/* Object already found */
		return it->second;
	} else {
		/* New object: 1. generate new name
		 *             2. add to map */
		char *name = ralloc_strdup(mem, input);
		g.nameMap[name] = getNewUnusedFunctionName(name, shader);
		return g.nameMap[name];
	}
}

static void setLoopIterName(char **name, ShVariableList *vl, void* mem_ctx)
{
	char prefix[200];
	sprintf(prefix, "%s%i", CG_LOOP_ITER_PREFIX, g.numLoopIters);
	*name = getUnusedNameByPrefix(vl, prefix, mem_ctx);
	g.loopIters.push_back(*name);
	g.numLoopIters++;
}

void CodeGen::setIterNames()
{
	AstStack* stack = &shader->path;

	foreach_stack(node, stack) {
		ast_iteration_statement* loop = node->as_iteration_statement();
		if (!loop)
			continue;
		if (loop->debug_iter_name != NULL) {
			ralloc_free(loop->debug_iter_name);
			loop->debug_iter_name = NULL;
		}

		if (loop->debug_state == ast_dbg_state_target) {
			if (loop->debug_state_internal == ast_dbg_loop_qyr_test
					|| loop->debug_state_internal == ast_dbg_loop_select_flow
					|| loop->debug_state_internal == ast_dbg_loop_qyr_terminal)
				setLoopIterName(&loop->debug_iter_name, vl, shader);
			else
				assert(!"CodeGen - loop target has invalid internal state");
		} else if (loop->debug_state == ast_dbg_state_path) {
			if (loop->debug_state_internal == ast_dbg_loop_wrk_init
					|| loop->debug_state_internal == ast_dbg_loop_wrk_test
					|| loop->debug_state_internal == ast_dbg_loop_wrk_body
					|| loop->debug_state_internal == ast_dbg_loop_wrk_terminal)
				setLoopIterName(&loop->debug_iter_name, vl, shader);
			else
				assert(!"CodeGen - loop path has invalid internal state");
		}
	}
}


void CodeGen::resetLoopIterNames(void)
{
	g.numLoopIters = 0;
	g.loopIters.clear();
}

