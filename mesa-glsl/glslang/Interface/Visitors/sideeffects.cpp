/*
 * sideeffects.cpp
 *
 *  Created on: 27.12.2013
 */

#include "sideeffects.h"
#include "glsl/ast.h"
#include "glsl/ir.h"
#include "glsl/list.h"
#include "glslang/Interface/CodeTools.h"
#undef NDEBUG
#include <assert.h>


extern void validate_identifier(const char *, YYLTYPE, struct _mesa_glsl_parse_state *);
extern const glsl_type* process_array_type(YYLTYPE*, const glsl_type*, ast_array_specifier*,
		struct _mesa_glsl_parse_state*);
extern glsl_interp_qualifier interpret_interpolation_qualifier(
		const struct ast_type_qualifier *, ir_variable_mode, struct _mesa_glsl_parse_state*,
		YYLTYPE*);
extern void validate_matrix_layout_for_type(struct _mesa_glsl_parse_state *, YYLTYPE *,
		const glsl_type *, ir_variable *);


// As in ast_to_hir
unsigned ast_process_structure_or_interface_block(ast_sideeffects_traverser_visitor* visitor,
		struct _mesa_glsl_parse_state *state, exec_list *declarations, YYLTYPE &loc,
		glsl_struct_field **fields_ret, bool block_row_major)
{
	unsigned decl_count = 0;
	foreach_list_typed (ast_declarator_list, decl_list, link, declarations) {
		foreach_list_const (decl_ptr, & decl_list->declarations) {
			decl_count++;
		}
	}

	glsl_struct_field * const fields = ralloc_array(state, glsl_struct_field,
			decl_count);

	unsigned i = 0;
	foreach_list_typed (ast_declarator_list, decl_list, link, declarations) {
		const char *type_name;
		decl_list->type->specifier->accept(visitor);
		const glsl_type *decl_type = decl_list->type->glsl_type(&type_name, state);
		foreach_list_typed (ast_declaration, decl, link, &decl_list->declarations) {
			validate_identifier(decl->identifier, loc, state);

			const struct glsl_type *field_type =
					decl_type != NULL ? decl_type : glsl_type::error_type;

			const struct ast_type_qualifier * const qual = &decl_list->type->qualifier;
			field_type = process_array_type(&loc, decl_type, decl->array_specifier, state);
			fields[i].type = field_type;
			fields[i].name = decl->identifier;
			fields[i].location = -1;
			fields[i].interpolation = interpret_interpolation_qualifier(qual, ir_var_auto,
					state, &loc);
			fields[i].centroid = qual->flags.q.centroid ? 1 : 0;
			fields[i].sample = qual->flags.q.sample ? 1 : 0;

			if (qual->flags.q.row_major || qual->flags.q.column_major) {
				if (qual->flags.q.uniform)
					validate_matrix_layout_for_type(state, &loc, field_type, NULL);
			}


			if (field_type->is_matrix()
					|| (field_type->is_array() && field_type->fields.array->is_matrix())) {
				fields[i].row_major = block_row_major;
				if (qual->flags.q.row_major)
					fields[i].row_major = true;
				else if (qual->flags.q.column_major)
					fields[i].row_major = false;
			}

			i++;
		}
	}

	assert(i == decl_count);

	*fields_ret = fields;
	return decl_count;
}

void ast_sideeffects_traverser_visitor::visit(ast_declarator_list* node)
{
	assert(node->type);
	node->type->accept(this);

	const char* type_name;
	const glsl_type * type = node->type->glsl_type(&type_name, this->state);
	assert(type || !"Using of non-declared type");
	foreach_list_typed (ast_node, decl, link, &node->declarations) {
		// Check if we need to set initializer type
		ast_declaration* d = decl->as_declaration();
		if (d &&  d->initializer && d->initializer->oper == ast_aggregate)
			_mesa_ast_set_aggregate_type(type, d->initializer);

		decl->accept(this);
		// Register variable
		astToShVariable(decl, &node->type->qualifier, type);
	}
}

// As in ast_to_hir
void ast_sideeffects_traverser_visitor::visit(ast_struct_specifier* node)
{
	YYLTYPE loc = node->get_location();
	if (state->language_version != 110 && state->struct_specifier_depth != 0)
		_mesa_glsl_error(&loc, state, "embedded structure declartions are not allowed");

	state->struct_specifier_depth++;

	glsl_struct_field *fields;
	unsigned decl_count = ast_process_structure_or_interface_block(this, state,
			&node->declarations, loc, &fields, false);

	validate_identifier(node->name, loc, state);
	const glsl_type *t = glsl_type::get_record_instance(fields, decl_count, node->name);

	if (!state->symbols->add_type(node->name, t)) {
		_mesa_glsl_error(&loc, state, "struct `%s' previously defined", node->name);
	} else {
		const glsl_type **s = reralloc(state, state->user_structures,
				const glsl_type *, state->num_user_structures + 1);
		if (s != NULL) {
			s[state->num_user_structures] = t;
			state->user_structures = s;
			state->num_user_structures++;
		}
	}

	state->struct_specifier_depth--;
}

void ast_sideeffects_traverser_visitor::visit(ast_compound_statement* node)
{
	if (node->new_scope)
		depth++;

	foreach_list_typed (ast_node, ast, link, &node->statements) {
		ast->accept(this);
		node->debug_sideeffects |= ast->debug_sideeffects;
	}

	if (node->new_scope)
		depth--;
}

void ast_sideeffects_traverser_visitor::visit(ast_expression_statement* node)
{
	if (node->expression) {
		node->expression->accept(this);
		node->debug_sideeffects |= node->expression->debug_sideeffects;
	}
}

bool ast_sideeffects_traverser_visitor::traverse(ast_expression* node)
{
	for (int i = 0; i < 3; ++i) {
		if (!node->subexpressions[i])
			continue;
		node->debug_sideeffects |= node->subexpressions[i]->debug_sideeffects;
	}

	switch (node->oper) {
	case ast_assign:
		node->debug_sideeffects |= ast_dbg_se_general;
		break;
	default:
		assert(!"not implemented");
	}
	return true;
}

bool ast_sideeffects_traverser_visitor::traverse(ast_expression_bin* node)
{
	assert(!"not implemented");
}

bool ast_sideeffects_traverser_visitor::traverse(ast_function_expression* node)
{
	assert(!"not implemented");
//	node->
//	ir->debug_sideeffects = ir->callee->debug_sideeffects;
//	ir->debug_sideeffects |= list_sideeffects(&ir->actual_parameters);
//	return true;

//ir->debug_sideeffects |= ir_dbg_se_emit_vertex;
}

bool ast_sideeffects_traverser_visitor::traverse(ast_case_statement* node)
{
	assert(!"not implemented");
}

bool ast_sideeffects_traverser_visitor::traverse(ast_case_statement_list* node)
{
	assert(!"not implemented");
}

bool ast_sideeffects_traverser_visitor::traverse(ast_switch_body* node)
{
	assert(!"not implemented");
}

bool ast_sideeffects_traverser_visitor::traverse(ast_selection_statement* node)
{
	if (node->condition)
		node->debug_sideeffects = node->condition->debug_sideeffects;
	if (node->then_statement)
		node->debug_sideeffects = node->then_statement->debug_sideeffects;
	if (node->else_statement)
		node->debug_sideeffects = node->else_statement->debug_sideeffects;
	return true;
}

bool ast_sideeffects_traverser_visitor::traverse(ast_switch_statement* node)
{
	assert(!"not implemented");
}

bool ast_sideeffects_traverser_visitor::traverse(ast_iteration_statement* node)
{
	if (node->init_statement)
		node->debug_sideeffects |= node->init_statement->debug_sideeffects;
	if (node->condition)
		node->debug_sideeffects |= node->condition->debug_sideeffects;
	if (node->body)
		node->debug_sideeffects |= node->body->debug_sideeffects;
	if (node->rest_expression)
		node->debug_sideeffects |= node->rest_expression->debug_sideeffects;
	return true;
}

bool ast_sideeffects_traverser_visitor::traverse(ast_jump_statement* node)
{
	if (node->mode == ast_jump_statement::ast_discard)
		node->debug_sideeffects |= ir_dbg_se_discard;
	return true;
}

bool ast_sideeffects_traverser_visitor::traverse(ast_function_definition* node)
{
	node->debug_sideeffects |= ir_dbg_se_general;
	return true;
}

bool ast_sideeffects_traverser_visitor::traverse(ast_gs_input_layout* node)
{
	assert(!"not implemented");
}

