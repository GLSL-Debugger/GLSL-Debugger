/*
 * debugpath.cpp
 *
 *  Created on: 02 марта 2014 г.
 */

#include "debugpath.h"
#include "glsl/list.h"
#include "glsldb/utils/dbgprint.h"

void ast_debugpath_traverser_visitor::run(exec_list* node, enum DPOperation op)
{
	if (op == DPOpPathClear) {
		VPRINT(1, "********* clear path traverse **********\n");
	} else if (op == DPOpPathBuild) {
		VPRINT(1, "********* create path traverse **********\n");
	} else if (op == DPOpReset) {
		VPRINT(1, "********* reset traverse **********\n");
	} else {
		assert(!"Wrong operation type");
	}

	this->action = op;
	this->visit(node);
}

void ast_debugpath_traverser_visitor::processDebugable(ast_node* node)
{
	VPRINT(3, "path Debugable L:%s Op:%i DbgSt:%i\n",
		FormatSourceRange(node->get_location()).c_str(), action, node->debug_state);

	if (action == DPOpPathClear) {
		if (node->debug_state == ast_dbg_state_path)
			node->debug_state = ast_dbg_state_unset;
	} else if (action == DPOpReset) {
		node->debug_state = ast_dbg_state_unset;
	}
}

void ast_debugpath_traverser_visitor::leave(class ast_expression* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		for (int i = 0; i < 3; ++i) {
			if (!node->subexpressions[i])
				continue;
			if (node->subexpressions[i]->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
		}
}

void ast_debugpath_traverser_visitor::leave(class ast_expression_bin* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		for (int i = 0; i < 2; ++i) {
			if (!node->subexpressions[i])
				continue;
			if (node->subexpressions[i]->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
		}
}

void ast_debugpath_traverser_visitor::leave(class ast_function_expression* node)
{
	processDebugable(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_expression_statement* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		if (node->expression && node->expression->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_compound_statement* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		foreach_list_typed(ast_node, ast, link, &node->statements)
			if (ast->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_declaration* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		if (node->initializer && node->initializer->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_declarator_list* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		foreach_list_typed(ast_node, ast, link, &node->declarations)
			if (ast->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_case_statement* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild){
		if (node->labels->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;

		foreach_list_typed(ast_node, ast, link, &node->stmts)
			if (ast->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
	}
}

void ast_debugpath_traverser_visitor::leave(class ast_case_statement_list* node)
{
	processDebugable(node);

	if (action == OTOpPathBuild)
		foreach_list_typed(ast_node, ast, link, &node->cases)
			if (ast->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_switch_body* node)
{
	processDebugable(node);
	if (action == OTOpPathBuild && node->stmts)
		if (node->stmts != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_switch_statement* node)
{
	VPRINT(2, "path Switch L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	processDebugable(node);

	if (action == DPOpReset)
		node->debug_state_internal = ast_dbg_switch_unset;

	if (action == DPOpPathBuild && node->debug_state == ast_dbg_state_unset)
		if (dbg_state_not_match(node->test_expression, ast_dbg_state_unset)
				|| dbg_state_not_match(node->body, ast_dbg_state_unset))
			node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_selection_statement* node)
{
	VPRINT(2, "path Selection L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	processDebugable(node);

	if (action == DPOpReset)
		node->debug_state_internal = ast_dbg_if_unset;

	/* Check conditional and branches */
	if (action == DPOpPathBuild && node->debug_state == ast_dbg_state_unset)
		if (dbg_state_not_match(node->then_statement, ast_dbg_state_unset)
				|| dbg_state_not_match(node->else_statement, ast_dbg_state_unset)
				|| node->condition->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::leave(class ast_iteration_statement* node)
{
	VPRINT(2, "path Loop L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	processDebugable(node);

	if (action == DPOpReset) {
		node->debug_state_internal = ast_dbg_loop_unset;
		node->debug_iter = 0;
	}

	/* Check init, test, terminal, and body */
	if (action == DPOpPathBuild && node->debug_state == ast_dbg_state_unset)
		if (dbg_state_not_match(node->init_statement, ast_dbg_state_unset)
				|| dbg_state_not_match(node->condition, ast_dbg_state_unset)
				|| dbg_state_not_match(node->rest_expression, ast_dbg_state_unset)
				|| dbg_state_not_match(node->body, ast_dbg_state_unset))
			node->debug_state = ir_dbg_state_path;

}

void ast_debugpath_traverser_visitor::leave(class ast_function_definition* node)
{
	VPRINT(2, "process function definition L:%s N:%s Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(),
			node->prototype->identifier, action, node->debug_state);

	processDebugable(node);
	if (action == OTOpPathBuild && node->body) {
		VPRINT(6, "getDebugState: %i\n", node->body->debug_state);
		if (node->body->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
	}
}

void ast_debugpath_traverser_visitor::leave(class ast_jump_statement* node)
{
	processDebugable(node);
	if (action == OTOpPathBuild)
		if (node->opt_return_value
				&& node->opt_return_value->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
}
