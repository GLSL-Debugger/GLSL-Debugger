#include "CodeTools.h"

#include "glsl/list.h"
#include "Visitors/postprocess.h"
#include "glsldb/utils/dbgprint.h"
#include "AstScope.h"
#include "SymbolTable.h"
#include <map>

#define X 1
#define R 5
#define S 9
#define I 13

long strToSwizzleIdx(const char* str)
{
	/* See ir.cpp ir_swizzle::create */

	/* For each possible swizzle character, this table encodes the value in
	 * \c idx_map that represents the 0th element of the vector.  For invalid
	 * swizzle characters (e.g., 'k'), a special value is used that will allow
	 * detection of errors.
	 */
	static const unsigned char base_idx[26] = {
		/* a  b  c  d  e  f  g  h  i  j  k  l  m */
		   R, R, I, I, I, I, R, I, I, I, I, I, I,
		/* n  o  p  q  r  s  t  u  v  w  x  y  z */
		   I, I, S, S, R, S, S, I, I, X, X, X, X };

	/* Each valid swizzle character has an entry in the previous table.  This
	 * table encodes the base index encoded in the previous table plus the actual
	 * index of the swizzle character.  When processing swizzles, the first
	 * character in the string is indexed in the previous table.  Each character
	 * in the string is indexed in this table, and the value found there has the
	 * value form the first table subtracted.  The result must be on the range
	 * [0,3].
	 *
	 * For example, the string "wzyx" will get X from the first table.  Each of
	 * the charcaters will get X+3, X+2, X+1, and X+0 from this table.  After
	 * subtraction, the swizzle values are { 3, 2, 1, 0 }.
	 *
	 * The string "wzrg" will get X from the first table.  Each of the characters
	 * will get X+3, X+2, R+0, and R+1 from this table.  After subtraction, the
	 * swizzle values are { 3, 2, 4, 5 }.  Since 4 and 5 are outside the range
	 * [0,3], the error is detected.
	 */
	static const unsigned char idx_map[26] = {
		/* a    b    c    d    e    f    g    h    i    j    k    l    m */
	      R+3, R+2,  0,   0,   0,   0,  R+1,  0,   0,   0,   0,   0,   0,
		/* n    o    p    q    r    s    t    u    v    w    x    y    z */
		   0, 	0,  S+2, S+3, R+0, S+0, S+1,  0,   0,  X+3, X+0, X+1, X+2 };

	/* Validate the first character in the swizzle string and look up the base
	 * index value as described above.
	 */
	if ((str[0] < 'a') || (str[0] > 'z'))
		return 0;

	long swiz_idx = 0;
	const unsigned base = base_idx[str[0] - 'a'];
	for (unsigned i = 0; (i < 4) && (str[i] != '\0'); i++) {
		if ((str[i] < 'a') || (str[i] > 'z'))
			return 0;
		int val = (idx_map[str[i] - 'a'] - base);
		// Driver must care about it, not debugger.
		if (val < 0)
			return 0;
		// shift it to next power of two for number of table elements
		//swiz_idx += val << (5*i);
		// It something like this in original. I think, order will be lost here.
		swiz_idx += 1 << val;
	}
	return swiz_idx;
}

bool partof(ast_node* target, ast_node* node)
{
	if (!target || !node)
		return false;

    if (node == target)
    	return true;

    ast_iteration_statement* loop = node->as_iteration_statement();
    ast_selection_statement* sels = node->as_selection_statement();
    ast_compound_statement* cmpd = node->as_compound_statement();
    ast_switch_statement* swtc =  node->as_switch_statement();
    ast_expression* expr = node->as_expression();
    // TODO: switch things

    if (loop) {
    	if (partof(target, loop->init_statement)
    			|| partof(target, loop->condition)
    			|| partof(target, loop->rest_expression)
    			|| partof(target, loop->body))
    		return true;
    } else if (sels) {
    	if (partof(target, sels->condition)
    			|| partof(target, sels->then_statement)
    			|| partof(target, sels->else_statement))
    		return true;
    } else if (cmpd) {
    	foreach_list_typed(ast_node, stmt, link, &cmpd->statements)
    		if (partof(target, stmt))
    			return true;
    } else if (swtc) {
    	if (partof(target, swtc->test_expression) ||
    			partof(target, swtc->body))
    		return true;
    } else if (expr) {
    	for (int i = 0; i < 3; ++i)
    		if (partof(target, expr->subexpressions[i]))
    			return true;
    	foreach_list_typed(ast_node, stmt, link, &expr->expressions)
    		if (partof(target, stmt))
    	    	return true;
    }
    return false;
}

void dumpNodeInfo(ast_node* node)
{
	dbgPrint(DBGLVL_COMPILERINFO, "(%s) ", FormatSourceRange(node->get_location()).c_str());
	ast_function_expression* call = node->as_function_expression();
	ast_function_definition* func = node->as_function_definition();
	ast_selection_statement* sels = node->as_selection_statement();
	ast_iteration_statement* loop = node->as_iteration_statement();
	ast_jump_statement* jump = node->as_jump_statement();
	ast_expression* expr = node->as_expression();
	if (call) {
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION CALL %s",
				call->subexpressions[0]->primary_expression.identifier);
	} else if (func) {
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION SIGNATURE %s",
				func->prototype->identifier);
	} else if (sels) {
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "IF");
	} else if (loop) {
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "LOOP");
	} else if (jump) {
		switch (jump->mode) {
		case ast_jump_statement::ast_jump_modes::ast_break:
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "BREAK");
			break;
		case ast_jump_statement::ast_jump_modes::ast_continue:
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "CONTINUE");
			break;
		case ast_jump_statement::ast_jump_modes::ast_discard:
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "DISCARD");
			break;
		case ast_jump_statement::ast_jump_modes::ast_return:
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "RETURN");
			break;
		}
	} else if (expr) {
		switch (expr->oper) {
		case ast_assign:
		case ast_mul_assign:
		case ast_div_assign:
		case ast_mod_assign:
		case ast_add_assign:
		case ast_sub_assign:
		case ast_ls_assign:
		case ast_rs_assign:
		case ast_and_assign:
		case ast_xor_assign:
		case ast_or_assign:
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "ASSIGNMENT");
			break;
		default:
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "EXPRESSION");
			break;
		}
	} else {
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "unknown");
	}
	dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
}

void dumpDbgStack(AstStack *stack)
{
	dbgPrint(DBGLVL_COMPILERINFO, "## STACK #####################################\n");

	for (ast_node* node = stack->head(); node != NULL; node = stack->perv()) {
		dumpNodeInfo(node);
		switch (node->debug_overwrite) {
		case ast_dbg_ow_debug:
			dbgPrint(DBGLVL_COMPILERINFO, " <OwDebug> ");
			break;
		case ast_dbg_ow_original:
			dbgPrint(DBGLVL_COMPILERINFO, " <OwOriginal> ");
			break;
		default:
			dbgPrint(DBGLVL_COMPILERINFO, " <OwUnset> ");
			break;
		}
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
		foreach_list(item, &node->scope){
			scope_item* sc = (scope_item*)item;
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "<%i,%s> ", sc->id, sc->name);
		}
		if (node->scope.is_empty())
			dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "no scope");
		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
	}

	dbgPrint(DBGLVL_COMPILERINFO, "###############################################\n");
}


static void makeMangledName(const struct glsl_type* type, std::string& mangledName)
{
	if (!type)
		return;

	if (type->is_matrix())
		mangledName += 'm';
	else if (type->is_vector())
		mangledName += 'v';

	switch (type->base_type) {
	case GLSL_TYPE_UINT:
		mangledName += 'u';
		break;
	case GLSL_TYPE_INT:
		mangledName += 'i';
		break;
	case GLSL_TYPE_FLOAT:
		mangledName += 'f';
		break;
	case GLSL_TYPE_BOOL:
		mangledName += 'b';
		break;
	case GLSL_TYPE_SAMPLER: {
		mangledName += 's';
		if (type->sampler_type == GLSL_TYPE_INT)
			mangledName += 'I';
		else if (type->sampler_type == GLSL_TYPE_UINT)
			mangledName += 'U';

		if (type->sampler_shadow)
			mangledName += 'S';

		switch (type->sampler_dimensionality) {
		case GLSL_SAMPLER_DIM_1D:
			mangledName += '1';
			break;
		case GLSL_SAMPLER_DIM_2D:
			mangledName += '2';
			break;
		case GLSL_SAMPLER_DIM_3D:
			mangledName += '3';
			break;
		case GLSL_SAMPLER_DIM_CUBE:
			mangledName += 'C';
			break;
		case GLSL_SAMPLER_DIM_RECT:
			mangledName += 'R';
			break;
		case GLSL_SAMPLER_DIM_BUF:
			mangledName += 'B';
			break;
		default:
			mangledName += "Unknown";
			break;

		}
//    		   GLSL_SAMPLER_DIM_EXTERNAL,
//    		   GLSL_SAMPLER_DIM_MS
//    	    case EbtSampler1DArray:     mangledName += "sA1";    break;  // EXT_gpu_shader4
//    	    case EbtISampler1DArray:    mangledName += "sIA1";   break;  // EXT_gpu_shader4
//    	    case EbtUSampler1DArray:    mangledName += "sUA1";   break;  // EXT_gpu_shader4
//    	    case EbtSampler2DArray:     mangledName += "sA2";    break;  // EXT_gpu_shader4
//    	    case EbtISampler2DArray:    mangledName += "sIA2";   break;  // EXT_gpu_shader4
//    	    case EbtUSampler2DArray:    mangledName += "sUA2";   break;  // EXT_gpu_shader4
//    	    case EbtSampler1DArrayShadow:     mangledName += "sAS1";    break;  // EXT_gpu_shader4
//    	    case EbtSampler2DArrayShadow:     mangledName += "sAS2";    break;  // EXT_gpu_shader4
		break;
	}
	case GLSL_TYPE_STRUCT: {
		mangledName += "struct-" + std::string(type->name);
		for (unsigned int i = 0; i < type->length; ++i) {
			mangledName += '-';
			makeMangledName(type->fields.structure[i].type, mangledName);
		}
		break;
	}
	case GLSL_TYPE_VOID:
		mangledName += "void";
		break;
		/* GLSL_TYPE_INTERFACE, GLSL_TYPE_ARRAY, GLSL_TYPE_ERROR */
	default:
		break;
	}

	if (type->is_matrix()) {
		mangledName += static_cast<char>('0' + type->vector_elements);
		mangledName += 'x';
		mangledName += static_cast<char>('0' + type->matrix_columns);
	} else {
		mangledName += static_cast<char>('0' + type->length);
	}

	if (type->is_array()) {
		char buf[100];
		sprintf(buf, "[%d]", type->length);
		mangledName += buf;
	}
}

std::string getMangledName(ast_function_definition* fs, AstShader* shader)
{
	std::string mname = "";

	if (!fs)
		return mname;

	mname += std::string(fs->prototype->identifier) + "(";
	foreach_list_typed(ast_parameter_declarator, param, link, &fs->prototype->parameters) {
		const struct glsl_type* type =
				shader->symbols->get_type(param->type->specifier->type_name);
		assert(type || !"Type was not saved");
		makeMangledName(type, mname);
	}

	return mname;
}

char* getFunctionName(const char* manglName)
{
	size_t namelength;
	char *name;

	if (strchr(manglName, '(')) {
		namelength = strchr(manglName, '(') - manglName;
	} else {
		namelength = strlen(manglName);
	}

	if (!(name = (char*) malloc((namelength + 1) * sizeof(char)))) {
		dbgPrint(DBGLVL_ERROR, "CodeTools - Not enough memory for name in getFunctionName %s\n",
				manglName);
		exit(1);
	}
	strncpy(name, manglName, namelength);
	name[namelength] = '\0';
	return name;
}

int getFunctionDebugParameter(ast_function_definition* node)
{
	int result = -1;

	if (!node)
		return result;

	int i = 0;
	foreach_list_typed(ast_parameter_declarator, param, link, &node->prototype->parameters)	{
		if (param->type->qualifier.flags.q.in)
			result = i;
		++i;
	}

	return result;
}

ast_node* getSideEffectsDebugParameter(ast_function_expression *node, int pnum)
{
	if (!node)
		return NULL;

	int i = 0;
	foreach_list_typed(ast_node, param, link, &node->expressions) {
		if (i == pnum)
			return param->debug_sideeffects & ast_dbg_se_general ? param : NULL;
		++i;
	}

	assert(!"CodeTools - function does not have this much parameter");
	return NULL;
}

bool dbg_state_not_match(ast_node* node, enum ast_dbg_state state)
{
	if (node && node->debug_state != state)
		return true;
	return false;
}
