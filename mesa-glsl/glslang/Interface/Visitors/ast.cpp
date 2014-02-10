/*
 * ast.cpp
 *
 *  Created on: 05.02.2014
 */

#include "Visitors/ast.h"
#include "glsl/list.h"


void ast_traverse_visitor::visit(class ast_expression* node)
{
	if (flags & (traverse_previsit | traverse_debugvisit))
		if (this->traverse(node) != traverse_continue)
			return;

	++this->depth;

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
	case ast_or_assign:
	case ast_plus:
	case ast_neg:
	case ast_bit_not:
	case ast_logic_not:
	case ast_pre_inc:
	case ast_pre_dec:
	case ast_post_inc:
	case ast_post_dec:
	case ast_field_selection:
	case ast_array_index:
	case ast_conditional: // TODO: ast_selection_statement
		for (int i = 0; i < 2; ++i) {
			if (!node->subexpressions[0])
				break;
			this->visit(node->subexpressions[0]);
		}
		break;

	case ast_identifier:
	case ast_int_constant:
	case ast_uint_constant:
	case ast_float_constant:
	case ast_bool_constant:
		break;

	case ast_function_call:
	case ast_sequence:
	case ast_aggregate: {
		foreach_list_const(n, &node->expressions) {
			ast_node *ast = exec_node_data(ast_node, n, link);
			this->visit(ast);
		}
		break;
	}
	default:
		assert(0);
		break;
	}

	--this->depth;
	if (flags & traverse_postvisit)
		this->traverse(node);
}

void ast_traverse_visitor::visit(class ast_expression_bin* node)
{
	if (flags & (traverse_previsit | traverse_debugvisit))
			if (this->traverse(node) != traverse_continue)
				return;

	++this->depth;
	this->visit(node->subexpressions[0]);
	this->visit(node->subexpressions[1]);
	--this->depth;

	if (flags & traverse_postvisit)
		this->traverse(node);
}

void ast_traverse_visitor::visit(class ast_function_expression* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_array_specifier* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_aggregate_initializer* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_compound_statement* node)
{
	if (node->new_scope)
		depth++;

	foreach_list_typed (ast_node, ast, link, &node->statements)
		this->visit(ast);

	if (node->new_scope)
		depth--;
}

void ast_traverse_visitor::visit(class ast_declaration* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_struct_specifier* node)
{
	if (flags & (traverse_previsit | traverse_debugvisit))
		if (this->traverse(node) != traverse_continue)
			return;

	++this->depth;

	foreach_list_typed (ast_declarator_list, decl_list, link, &node->declarations)
		this->visit(decl_list);

	--this->depth;

	if (flags & traverse_postvisit)
		this->traverse(node);
}

void ast_traverse_visitor::visit(class ast_type_specifier* node)
{
	if (flags & (traverse_previsit | traverse_debugvisit))
		if (this->traverse(node) != traverse_continue)
			return;

	if (node->structure != NULL && node->structure->is_declaration)
		this->visit(node->structure);

	if (flags & traverse_postvisit)
		this->traverse(node);
}

void ast_traverse_visitor::visit(class ast_fully_specified_type* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_declarator_list* node)
{
	foreach_list_typed (ast_declaration, decl, link, &node->declarations)
		this->visit(decl);
}

void ast_traverse_visitor::visit(class ast_parameter_declarator* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_function* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_expression_statement* node)
{
	if (node->expression)
		this->visit(node->expression);
}

void ast_traverse_visitor::visit(class ast_case_label* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_case_label_list* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_case_statement* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_case_statement_list* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_switch_body* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_selection_statement* node)
{
	if (flags & traverse_debugvisit) {
		ast_traverser_status visit;
		/* Visit node for optional check of condition */
		if (node->debug_state_internal == ast_dbg_if_unset
			|| node->debug_state_internal == ast_dbg_if_init
			|| node->debug_state_internal == ast_dbg_if_condition_passed)
			visit = this->traverse(node);

		if (visit == traverse_continue &&
				node->debug_state_internal == ast_dbg_if_condition)
			this->visit(node->condition);

		/* Visit node again for choosing debugged branch */
		if (node->debug_state_internal == ast_dbg_if_condition)
			visit = this->traverse(node);

		if (visit == traverse_continue) {
			if (node->debug_state_internal == ast_dbg_if_then)
				this->visit(node->then_statement);
			if (node->debug_state_internal == ast_dbg_if_else)
				this->visit(node->else_statement);
		}

		/* Visit node again for preparation of pass */
		if (node->debug_state_internal == ast_dbg_if_then
				|| node->debug_state_internal == ast_dbg_if_else)
			visit = this->traverse(node);

	} else {
		if (flags & (traverse_previsit | traverse_debugvisit))
			if (this->traverse(node) != traverse_continue)
				return;

		this->visit(node->condition);
		this->visit(node->then_statement);
		this->visit(node->else_statement);

		if (flags & (traverse_postvisit | traverse_debugvisit))
			this->traverse(node);
	}
}

void ast_traverse_visitor::visit(class ast_switch_statement* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_iteration_statement* node)
{
	assert(!"not implemented");
}


void ast_traverse_visitor::visit(class ast_jump_statement* node)
{
	if (flags & traverse_previsit)
		if (this->traverse(node) != traverse_continue)
			return;

    if (node->opt_return_value)
    	this->visit(node->opt_return_value);

    if (flags & (traverse_postvisit | traverse_debugvisit))
    	this->traverse(node);
}

void ast_traverse_visitor::visit(class ast_function_definition* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_interface_block* node)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_gs_input_layout* node)
{
	assert(!"not implemented");
}
