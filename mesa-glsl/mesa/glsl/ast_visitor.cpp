/*
 * ast_visitor.cpp
 *
 *  Created on: 05.02.2014
 */

#include "ast_visitor.h"
#include "ast.h"
#include "list.h"

void ast_traverse_visitor::visit(exec_list *list)
{
	foreach_list_typed (ast_node, ast, link, list)
		ast->accept(this);
}

void ast_traverse_visitor::visit(class ast_node *)
{
	assert(!"unhandled error_type");
}

void ast_traverse_visitor::visit(class ast_expression* node)
{
	if (!this->enter(node))
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
	case ast_conditional:
		for (int i = 0; i < 3; ++i) {
			if (!node->subexpressions[i])
				break;
			node->subexpressions[i]->accept(this);
		}
		break;

	case ast_identifier:
	case ast_int_constant:
	case ast_uint_constant:
	case ast_float_constant:
	case ast_bool_constant:
		break;

	case ast_sequence:
	case ast_aggregate: {
		foreach_list_const(n, &node->expressions) {
			ast_node *ast = exec_node_data(ast_node, n, link);
			ast->accept(this);
		}
		break;
	}

	case ast_function_call:
	default:
		assert(0);
		break;
	}

	--this->depth;
	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_expression_bin* node)
{
	if (!this->enter(node))
		return;

	++this->depth;
	node->subexpressions[0]->accept(this);
	node->subexpressions[1]->accept(this);
	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_function_expression* node)
{
	if (!this->enter(node))
		return;

	// Type
	if (node->is_constructor())
		node->subexpressions[0]->accept(this);

	++this->depth;
	foreach_list_const(n, &node->expressions) {
		ast_node *ast = exec_node_data(ast_node, n, link);
		ast->accept(this);
	}
	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_array_specifier* node)
{
	if (!this->enter(node))
		return;

	this->depth++;
	foreach_list_const(n, &node->array_dimensions) {
		ast_node *dimension = exec_node_data(ast_node, n, link);
		dimension->accept(this);
	}
	this->depth--;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_aggregate_initializer* node)
{
	if (!this->enter(node))
		return;
	// Nothing to do here
	// TODO: Or not? If you have problems with some kind
	//       of calls in initializer, it is here
	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_compound_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->new_scope)
		depth++;

	foreach_list_typed (ast_node, ast, link, &node->statements)
		ast->accept(this);

	if (node->new_scope)
		depth--;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_declaration* node)
{
	if (!this->enter(node))
		return;

	++this->depth;
	if (node->array_specifier)
		node->array_specifier->accept(this);
	if (node->initializer)
		node->initializer->accept(this);

	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_struct_specifier* node)
{
	if (!this->enter(node))
		return;

	++this->depth;

	foreach_list_typed (ast_declarator_list, decl_list, link, &node->declarations)
		decl_list->accept(this);

	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_type_specifier* node)
{
	if (!this->enter(node))
		return;

	if (node->structure != NULL && node->structure->is_declaration)
		node->structure->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_fully_specified_type* node)
{
	if (node->specifier)
		node->specifier->accept(this);
}

void ast_traverse_visitor::visit(class ast_declarator_list* node)
{
	if (!this->enter(node))
		return;

	if (node->type)
		node->type->accept(this);
	foreach_list_typed (ast_node, decl, link, &node->declarations)
		decl->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_parameter_declarator* node)
{
	if (!this->enter(node))
		return;

	if (node->array_specifier)
		node->array_specifier->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_function* node)
{
	if (!this->enter(node))
		return;

	foreach_list_typed (ast_node, decl, link, &node->parameters)
		decl->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_expression_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->expression)
		node->expression->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_case_label* node)
{
	if (!this->enter(node))
		return;

	if (node->test_value)
		node->test_value->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_case_label_list* node)
{
	foreach_list_typed (ast_node, ast, link, &node->labels)
		ast->accept(this);
}

void ast_traverse_visitor::visit(class ast_case_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->labels)
		node->labels->accept(this);

	++this->depth;
	foreach_list_typed (ast_node, ast, link, &node->stmts)
		ast->accept(this);
	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_case_statement_list* node)
{
	if (!this->enter(node))
		return;

	++this->depth;
	foreach_list_typed (ast_node, ast, link, &node->cases)
		ast->accept(this);
	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_switch_body* node)
{
	if (!this->enter(node))
		return;

	if (node->stmts)
		node->stmts->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_selection_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->condition)
		node->condition->accept(this);
	++depth;
	if (node->then_statement)
		node->then_statement->accept(this);
	if (node->else_statement)
		node->else_statement->accept(this);
	--depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_switch_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->test_expression)
		node->test_expression->accept(this);

	++this->depth;
	if (node->body)
		node->body->accept(this);
	--this->depth;

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_iteration_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->init_statement)
		node->init_statement->accept(this);
	if (node->condition)
		node->condition->accept(this);
	++depth;
	if (node->body)
		node->body->accept(this);
	--depth;
	if (node->rest_expression)
		node->rest_expression->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_jump_statement* node)
{
	if (!this->enter(node))
		return;

	if (node->opt_return_value)
		node->opt_return_value->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_function_definition* node)
{
	if (!this->enter(node))
		return;

	if (node->prototype)
		node->prototype->accept(this);

	if (node->body)
		node->body->accept(this);

	this->leave(node);
}

void ast_traverse_visitor::visit(class ast_interface_block*)
{
	assert(!"not implemented");
}

void ast_traverse_visitor::visit(class ast_gs_input_layout* node)
{
	// It can be used to setup variable size for inputs preceded this declaration
	if (!this->enter(node))
		return;
	this->leave(node);
}
