/*
 * debugpath.cpp
 *
 *  Created on: 02 марта 2014 г.
 */

#include "debugpath.h"
#include "glsl/ast.h"
#include "glsl/list.h"
#include "glsldb/utils/dbgprint.h"
#include "glslang/Interface/CodeTools.h"

static inline void childPath(ast_node* node, ast_node* child)
{
	if (node->debug_state == ast_dbg_state_unset)
		if (child && child->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
}

void ast_debugpath_traverser_visitor::getPath(DbgRsScope& scope, AstShader* shader)
{
	shader->path.clear();
	path.copy(&shader->path);
	while(!path.empty()) {
		ast_node* top = path.top();
		addScopeToScopeStack(scope, &top->scope, shader);
		path.pop();
	}
}

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

	if (action != DPOpPathBuild)
		return;

	for (int i = 0; i < 3; ++i)
		childPath(node, node->subexpressions[i]);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_expression_bin* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	for (int i = 0; i < 2; ++i)
		childPath(node, node->subexpressions[i]);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_function_expression* node)
{
	processDebugable(node);
	if (action == DPOpPathBuild && node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_expression_statement* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	childPath(node, node->expression);
	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_compound_statement* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	foreach_list_typed(ast_node, ast, link, &node->statements)
		childPath(node, ast);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_declaration* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	childPath(node, node->initializer);
	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_declarator_list* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	foreach_list_typed(ast_node, ast, link, &node->declarations)
		childPath(node, ast);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_case_statement* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	childPath(node, node->labels);

	foreach_list_typed(ast_node, ast, link, &node->stmts)
		childPath(node, ast);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_case_statement_list* node)
{
	processDebugable(node);

	if (action != DPOpPathBuild)
		return;

	foreach_list_typed(ast_node, ast, link, &node->cases)
		childPath(node, ast);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_switch_body* node)
{
	processDebugable(node);
	if (action != DPOpPathBuild)
		return;

	childPath(node, node->stmts);
	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_switch_statement* node)
{
	VPRINT(2, "path Switch L:%s DbgSt:%i\n", FormatSourceRange(node->get_location()).c_str(),
			node->debug_state);

	processDebugable(node);

	if (action == DPOpReset)
		node->debug_state_internal = ast_dbg_switch_unset;

	if (action != DPOpPathBuild)
		return;

	childPath(node, node->test_expression);
	childPath(node, node->body);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_selection_statement* node)
{
	VPRINT(2, "path Selection L:%s DbgSt:%i\n", FormatSourceRange(node->get_location()).c_str(),
			node->debug_state);

	processDebugable(node);

	if (action == DPOpReset)
		node->debug_state_internal = ast_dbg_if_unset;


	if (action != DPOpPathBuild)
		return;

	childPath(node, node->then_statement);
	childPath(node, node->else_statement);
	childPath(node, node->condition);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_iteration_statement* node)
{
	VPRINT(2, "path Loop L:%s DbgSt:%i\n", FormatSourceRange(node->get_location()).c_str(),
			node->debug_state);

	processDebugable(node);

	if (action == DPOpReset) {
		node->debug_state_internal = ast_dbg_loop_unset;
		node->debug_iter = 0;
	}

	/* Check init, test, terminal, and body */
	if (action != DPOpPathBuild)
		return;

	childPath(node, node->init_statement);
	childPath(node, node->condition);
	childPath(node, node->rest_expression);
	childPath(node, node->body);

	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_function_definition* node)
{
	VPRINT(2, "process function definition L:%s N:%s Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->prototype->identifier,
			action, node->debug_state);

	processDebugable(node);
	if (action != DPOpPathBuild)
		return;

	VPRINT(6, "getDebugState: %i\n", node->body->debug_state);
	childPath(node, node->body);
	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}

void ast_debugpath_traverser_visitor::leave(class ast_jump_statement* node)
{
	processDebugable(node);
	if (action != DPOpPathBuild)
		return;

	childPath(node, node->opt_return_value);
	if (node->debug_state != ast_dbg_state_unset)
		path.push(node);
}
