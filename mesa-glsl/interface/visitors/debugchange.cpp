/*
 * debugchange.cpp
 *
 *  Created on: 20.10.2013
 */

#include "debugchange.h"
#include "Shader.h"
#include "AstScope.h"
#include "CodeTools.h"
#include "SymbolTable.h"
#include "mesa/glsl/list.h"
#include "glsldb/utils/dbgprint.h"
#include <assert.h>

static void dumpAstChangeableList(exec_list *cl)
{
	if (!cl)
		return;
	dbgPrint(DBGLVL_INFO, "===> ");
	if (cl->is_empty())
		dbgPrint(DBGLVL_INFO, "empty");
	foreach_list(node, cl) {
		changeable_item* item = (changeable_item*) node;
		dumpShChangeable(item->changeable);
	}
	dbgPrint(DBGLVL_INFO, "\n");
}

static ShChangeableIndex* getChangeableIndex(ast_expression* node, void* mem_ctx)
{
	ShChangeableType type;
	long index = -1;

	// FIXME: I cannot understand what is SH_CGB_ARRAY_DIRECT

	if (node->oper == ast_array_index) {
		ast_expression* exp_index = node->subexpressions[1];
		switch (exp_index->oper) {
		case ast_identifier:
			break;
		case ast_int_constant:
		case ast_uint_constant:
			type = SH_CGB_ARRAY_INDIRECT;
			index = exp_index->oper == ast_int_constant ?
					exp_index->primary_expression.int_constant :
					exp_index->primary_expression.uint_constant;
			break;
		default:
			assert(!"Wrong index for array");
			break;

		}
	} else if (node->oper == ast_field_selection) {
		switch (node->debug_selection_type) {
		case ast_fst_swizzle:
			type = SH_CGB_SWIZZLE;
			index = strToSwizzleIdx(node->primary_expression.identifier);
			break;
		case ast_fst_struct:
			type = SH_CGB_STRUCT;
			break;
		default:
			assert(!"not implemented");
			break;
		}
	} else {
		assert(!"Wrong type for indexing");
	}

	return createShChangeableIndexCtx(type, index, mem_ctx);
}

bool ast_debugchange_traverser_visitor::enter(class ast_compound_statement* node)
{
	// Copy all changeables from
	foreach_list_typed (ast_node, ast, link, &node->statements)
		copyChangeables(node, ast);
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_declarator_list* node)
{
	if (node->type)
		node->type->accept(this);
	foreach_list_typed (ast_node, decl, link, &node->declarations)
		copyChangeables(node, decl);
	return false;
}

void ast_debugchange_traverser_visitor::copyChangeables(ast_node* dst, ast_node* src)
{
	if (!dst || !src)
		return;
	src->accept(this);
	copyAstChangeableList(&dst->changeables, &src->changeables, &dst->scope, shader);
	dumpAstChangeableList(&dst->changeables);
}

bool ast_debugchange_traverser_visitor::enter(class ast_expression* node)
{
	switch (node->oper) {
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
	case ast_or_assign: {
		VPRINT(2, "(%s) changeAssigment\n", FormatSourceRange(node->get_location()).c_str());
		// process left branch actively
		if (node->subexpressions[0]) {
			VPRINT(2, "===== left ============================\n");
			this->activate();
			node->subexpressions[0]->accept(this);
			this->deactivate();
			// copy the changeables of left branch
			copyAstChangeableList(&node->changeables, &node->subexpressions[0]->changeables,
					&node->scope, shader);
			//dumpShChangeableList(&node->changeables);
		}

		// process right branch passively
		if (node->subexpressions[1]) {
			VPRINT(2, "===== right ===========================\n");
			this->deactivate();
			// copy the changeables of right branch
			copyChangeables(node, node->subexpressions[1]);
			//dumpShChangeableList(&node->changeables);
		}
		return false;
		break;
	}

	case ast_plus:
	case ast_neg:
	case ast_bit_not:
	case ast_logic_not:
	case ast_pre_inc:
	case ast_pre_dec:
	case ast_post_inc:
	case ast_post_dec: {
		VPRINT(2, "(%s) changeExpression\n", FormatSourceRange(node->get_location()).c_str());
		//dumpShChangeableList(node->getCgbList());
		this->deactivate();
		copyChangeables(node, node->subexpressions[0]);
		copyChangeables(node, node->subexpressions[1]);
		break;
	}

	case ast_field_selection:
	case ast_array_index: {
		VPRINT(2, "(%s) field_selection or index\n",
				FormatSourceRange(node->get_location()).c_str());
		ast_expression* subnode = node->subexpressions[0];

		// process left branch first then copy changeables
		// should be one or zero, otherwise we have a problem
		copyChangeables(node, subnode);

		int count = 0;
		foreach_list(c, &node->changeables)
			count++;

		assert(count <= 1 || !"DebugVar - field_selection or index to more than one changeable?");
		if (node->changeables.is_empty())
			return false;

		ShChangeableIndex *cgbIdx = getChangeableIndex(node, shader);
		ShChangeable* cgb = ((changeable_item*) node->changeables.get_head())->changeable;
		addShIndexToChangeableCtx(cgb, cgbIdx, shader);

		return false;
		break;
	}

	case ast_conditional: {
		this->deactivate();
		copyChangeables(node, node->subexpressions[0]);
		copyChangeables(node, node->subexpressions[1]);
		copyChangeables(node, node->subexpressions[2]);
		return false;
		break;
	}

	case ast_identifier:
		if (this->isActive()) {
			if (node->changeables.is_empty()) {
				VPRINT(2,
						"(%s) changeSymbol -> created %i %s\n", FormatSourceRange(
								node->get_location()).c_str(), node->debug_id,
								node->primary_expression.identifier);
				ShChangeable* cgb = createShChangeableCtx(node->debug_id, shader);
				changeable_item* cgbi = new (shader) changeable_item(cgb);
				node->changeables.push_tail(cgbi);
			} else {
				VPRINT(2, "(%s) changeSymbol -> kept %i %s\n", FormatSourceRange(
						node->get_location()).c_str(), node->debug_id,
						node->primary_expression.identifier);
			}
		}
		return false;
		break;

	case ast_int_constant:
	case ast_uint_constant:
	case ast_float_constant:
	case ast_bool_constant:
		break;

	case ast_sequence: {
		this->deactivate();
		foreach_list_typed (ast_node, ast, link, &node->expressions) {
			VPRINT(2, "(%s) changeChild \n", FormatSourceRange(node->get_location()).c_str());
			copyChangeables(node, ast);
		}
		break;
	}

	case ast_function_call:
	case ast_aggregate:
	default:
		assert(0);
		break;
	}

	return true;
}

bool ast_debugchange_traverser_visitor::enter(class ast_expression_bin* node)
{
	VPRINT(2, "(%s) changeBinExpression\n", FormatSourceRange(node->get_location()).c_str());
	//dumpShChangeableList(&node->changeables);
	this->deactivate();
	copyChangeables(node, node->subexpressions[0]);
	copyChangeables(node, node->subexpressions[1]);
	return true;
}

bool ast_debugchange_traverser_visitor::enter(class ast_function_expression* node)
{
	VPRINT(2, "(%s) change call\n", FormatSourceRange(node->get_location()).c_str());

	// only user defined functions can have out/inout parameters
	if (node->debug_builtin || node->is_constructor())
		return false;

	// This will not work with functions in another file
	// Actually, nothing will work
	ast_function_definition* f = shader->symbols->get_function(
			node->subexpressions[0]->primary_expression.identifier);
	if (!f)
		return false;

	// changed variables are all out/inout parameters and
	// all changes made in the body of this function

	// if function is not already parsed, do it now
	// now the function should be aware of it's changeables
	copyChangeables(node, f);

	// for parameters we need more care iterate over all parameters
	// simultaneously in the function call and the function declaration
	// It's assumed that both sequences have the same amount of parameters.
	foreach_two_lists(pC, &node->expressions, pD, &f->prototype->parameters) {
		// check if parameter is of interest
		ast_node* dast = exec_node_data(ast_node, pD, link);
		if (dast->debug_id < 0)
			continue;
		ShVariable* dvar = findShVariable(dast->debug_id);
		ast_node* cast = exec_node_data(ast_node, pC, link);
		if (dvar->qualifier & SH_OUT)
			this->activate();

		cast->accept(this);
		this->deactivate();

		copyChangeables(node, cast);
		// add these parameters to parameter-list
		if (dvar->qualifier & SH_OUT)
			copyAstChangeableList(&node->changeable_params, &cast->changeables, &node->scope,
					shader);
	}

	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_expression_statement* node)
{
	copyChangeables(node, node->expression);
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_declaration* node)
{
	dumpAstChangeableList(&node->changeables);

	if (node->initializer) {
		VPRINT(2, "(%s) changeDeclaration -> begin\n", FormatSourceRange(node->get_location()).c_str());
		//dumpShChangeableList(&node->changeables);
		// copy the changeables of initialization
		copyChangeables(node, node->initializer);
		VPRINT(2, "(%s) changeDeclaration -> end\n", FormatSourceRange(node->get_location()).c_str());

		//dumpShChangeableList(&node->changeables);
		return false;
	}

	return true;
}

bool ast_debugchange_traverser_visitor::enter(class ast_parameter_declarator* node)
{
	if (node->type->qualifier.flags.q.in) {
		if (node->changeables.is_empty()) {
			VPRINT(2,
					"(%s) changeFuncParam -> created %i %s\n", FormatSourceRange(node->get_location()).c_str(), node->debug_id, node->identifier);
			ShChangeable* cgb = createShChangeableCtx(node->debug_id, shader);
			changeable_item* item = new (shader) changeable_item(cgb);
			node->changeables.push_tail(item);
		} else {
			VPRINT(2,
					"(%s) changeFuncParam -> kept %i %s\n", FormatSourceRange(node->get_location()).c_str(), node->debug_id, node->identifier);
		}
	}
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_function* node)
{
	// do not parse an function if this was already done before
	// again this is due to the (*^@*$ function prototypes
	std::set<ast_node*>::iterator iter = parsed_functions.find(node);
	if (iter != parsed_functions.end())
		return false;
	parsed_functions.insert(node);

	VPRINT(2, "(%s) changeProto %s \n", FormatSourceRange(node->get_location()).c_str(),
				node->identifier);
	foreach_list_typed (ast_node, ast, link, &node->parameters)
		copyChangeables(node, ast);

	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_case_statement* node)
{
	// just traverse and copy changeables all together
	this->deactivate();
	foreach_list_typed (ast_node, ast, link, &node->stmts)
		copyChangeables(node, ast);
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_case_statement_list* node)
{
	++this->depth;
	foreach_list_typed (ast_node, ast, link, &node->cases)
		copyChangeables(node, ast);
	--this->depth;
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_switch_body* node)
{
	this->deactivate();
	copyChangeables(node, node->stmts);
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_selection_statement* node)
{
	// just traverse and copy changeables all together
	this->deactivate();
	copyChangeables(node, node->condition);
	copyChangeables(node, node->then_statement);
	copyChangeables(node, node->else_statement);
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_switch_statement* node)
{
	this->deactivate();
	copyChangeables(node, node->test_expression);
	copyChangeables(node, node->body);
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_iteration_statement* node)
{
	// just traverse and copy changeables all together
	this->deactivate();

	// visit optional initialization
	copyChangeables(node, node->init_statement);

	// visit test, this should not change the changeables, but to be sure
	copyChangeables(node, node->condition);

	// visit optional terminal, this cannot change the changeables either
	copyChangeables(node, node->rest_expression);

	// visit body
	copyChangeables(node, node->body);

	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_jump_statement* node)
{
	if (node->opt_return_value) {
		// simply traverse and copy result
		this->deactivate();
		copyChangeables(node, node->opt_return_value);
	}
	return false;
}

bool ast_debugchange_traverser_visitor::enter(class ast_function_definition* node)
{
	// do not parse an function if this was already done before
	// again this is due to the (*^@*$ function prototypes
	std::set<ast_node*>::iterator iter = parsed_functions.find(node);
	if (iter != parsed_functions.end())
		return false;
	parsed_functions.insert(node);

	VPRINT(2, "(%s) changeFunction \n", FormatSourceRange(node->get_location()).c_str());
	copyChangeables(node, node->prototype);
	copyChangeables(node, node->body);

	// parameters changeables in prototype if needed
	return false;
}
