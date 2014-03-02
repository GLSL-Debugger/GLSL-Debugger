/*
 * call_visitor.cpp
 *
 *  Created on: 22.02.2014
 */

#include "glsl/ast.h"
#include "call_visitor.h"
#include "units/Base.h"

bool ast_call_visitor::enter(class ast_node *node)
{
	return unit->accept(depth, node, AST_NODE);
}

bool ast_call_visitor::enter(class ast_expression *node)
{
	return unit->accept(depth, node, AST_EXPRESSION);
}

bool ast_call_visitor::enter(class ast_expression_bin *node)
{
	return unit->accept(depth, node, AST_EXPRESSION_BIN);
}

bool ast_call_visitor::enter(class ast_function_expression *node)
{
	return unit->accept(depth, node, AST_FUNCTION_EXPRESSION);
}

bool ast_call_visitor::enter(class ast_array_specifier *node)
{
	return unit->accept(depth, node, AST_ARRAY_SPECIFIER);
}

bool ast_call_visitor::enter(class ast_aggregate_initializer *node)
{
	return unit->accept(depth, node, AST_AGGREGATE_INITIALIZER);
}

bool ast_call_visitor::enter(class ast_declaration *node)
{
	return unit->accept(depth, node, AST_DECLARATION);
}

bool ast_call_visitor::enter(class ast_struct_specifier *node)
{
	return unit->accept(depth, node, AST_STRUCT_SPECIFIER);
}

bool ast_call_visitor::enter(class ast_type_specifier *node)
{
	return unit->accept(depth, node, AST_TYPE_SPECIFIER);
}

bool ast_call_visitor::enter(class ast_fully_specified_type *node)
{
	return unit->accept(depth, node, AST_FULLY_SPECIFIED_TYPE);
}

bool ast_call_visitor::enter(class ast_parameter_declarator *node)
{
	return unit->accept(depth, node, AST_PARAMETER_DECLARATOR);
}

bool ast_call_visitor::enter(class ast_function *node)
{
	return unit->accept(depth, node, AST_FUNCTION);
}

bool ast_call_visitor::enter(class ast_case_label *node)
{
	return unit->accept(depth, node, AST_CASE_LABEL);
}

bool ast_call_visitor::enter(class ast_case_statement *node)
{
	return unit->accept(depth, node, AST_CASE_STATEMENT);
}

bool ast_call_visitor::enter(class ast_switch_body *node)
{
	return unit->accept(depth, node, AST_SWITCH_BODY);
}

bool ast_call_visitor::enter(class ast_selection_statement *node)
{
	return unit->accept(depth, node, AST_SELECTION_STATEMENT);
}

bool ast_call_visitor::enter(class ast_switch_statement *node)
{
	return unit->accept(depth, node, AST_SWITCH_STATEMENT);
}

bool ast_call_visitor::enter(class ast_iteration_statement *node)
{
	return unit->accept(depth, node, AST_ITERATION_STATEMENT);
}

bool ast_call_visitor::enter(class ast_jump_statement *node)
{
	return unit->accept(depth, node, AST_JUMP_STATEMENT);
}

bool ast_call_visitor::enter(class ast_function_definition *node)
{
	return unit->accept(depth, node, AST_FUNCTION_DEFINITION);
}

bool ast_call_visitor::enter(class ast_interface_block *node)
{
	return unit->accept(depth, node, AST_INTERFACE_BLOCK);
}

bool ast_call_visitor::enter(class ast_gs_input_layout *node)
{
	return unit->accept(depth, node, AST_GS_INPUT_LAYOUT);
}
