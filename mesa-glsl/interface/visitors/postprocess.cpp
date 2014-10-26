/*
 * sideeffects.cpp
 *
 *  Created on: 27.12.2013
 *
 * There a lot of functions as in ast_to_hir.
 * Primary purpose of it to obtain glsl type for ShVariable
 * Error messages was removed because it is driver work to compile
 * and check for errors, i.e. errors in shader -> no shader for debugging.
 * If you have some problems with this behavior, please report it,
 * it probably some errors in shader receiving phase, not here.
 */

#include "postprocess.h"
#include "Shader.h"
#include "CodeTools.h"
#include "SymbolTable.h"
#include "mesa/glsl/ast.h"
#include "mesa/glsl/ir.h"
#include "mesa/glsl/list.h"
#include "glsldb/utils/dbgprint.h"
#include <assert.h>


extern void validate_identifier(const char *, YYLTYPE, struct _mesa_glsl_parse_state *);
extern const glsl_type* process_array_type(YYLTYPE*, const glsl_type*, ast_array_specifier*,
		struct _mesa_glsl_parse_state*);
extern glsl_interp_qualifier interpret_interpolation_qualifier(
		const struct ast_type_qualifier *, ir_variable_mode, struct _mesa_glsl_parse_state*,
		YYLTYPE*);
extern void validate_matrix_layout_for_type(struct _mesa_glsl_parse_state *, YYLTYPE *,
		const glsl_type *, ir_variable *);
extern unsigned process_parameters(exec_list *instructions, exec_list *actual_parameters,
		exec_list *parameters, struct _mesa_glsl_parse_state *state);
extern void apply_type_qualifier_to_variable(const struct ast_type_qualifier *qual,
		ir_variable *var, struct _mesa_glsl_parse_state *state, YYLTYPE *loc,
		bool is_parameter);
extern void handle_geometry_shader_input_decl(struct _mesa_glsl_parse_state *state,
		YYLTYPE loc, ir_variable *var);
extern ir_variable * get_variable_being_redeclared(ir_variable *var, YYLTYPE loc,
		struct _mesa_glsl_parse_state *state, bool allow_all_redeclarations);


// As in ast_to_hir
unsigned ast_process_structure_or_interface_block(ast_postprocess_traverser_visitor* visitor,
		struct _mesa_glsl_parse_state *state, exec_list *declarations, YYLTYPE &loc,
		glsl_struct_field **fields_ret, bool is_interface, enum glsl_matrix_layout matrix_layout,
		bool allow_reserved_names, ir_variable_mode var_mode)
{
	unsigned decl_count = 0;
	foreach_list_typed (ast_declarator_list, decl_list, link, declarations) {
		decl_count += decl_list->declarations.length();
	}

	glsl_struct_field * const fields = ralloc_array(state, glsl_struct_field,
			decl_count);

	unsigned i = 0;
	foreach_list_typed (ast_declarator_list, decl_list, link, declarations) {
		const char *type_name;
		decl_list->type->specifier->accept(visitor);
		const glsl_type *decl_type = decl_list->type->glsl_type(&type_name, state);
		foreach_list_typed (ast_declaration, decl, link, &decl_list->declarations) {
			if (!allow_reserved_names)
				validate_identifier(decl->identifier, loc, state);

			const struct glsl_type *field_type =
					decl_type != NULL ? decl_type : glsl_type::error_type;

			const struct ast_type_qualifier * const qual = &decl_list->type->qualifier;
			field_type = process_array_type(&loc, decl_type, decl->array_specifier, state);
			fields[i].type = field_type;
			fields[i].name = decl->identifier;
			fields[i].location = -1;
			fields[i].interpolation = interpret_interpolation_qualifier(qual, var_mode,
					state, &loc);
			fields[i].centroid = qual->flags.q.centroid ? 1 : 0;
			fields[i].sample = qual->flags.q.sample ? 1 : 0;

			fields[i].stream = qual->flags.q.explicit_stream ? qual->stream : -1;

			if (qual->flags.q.row_major || qual->flags.q.column_major) {
				if (qual->flags.q.uniform)
					validate_matrix_layout_for_type(state, &loc, field_type, NULL);
			}

			if (field_type->without_array()->is_matrix() ||
				field_type->without_array()->is_record()) {

				fields[i].matrix_layout = matrix_layout;

				if (qual->flags.q.row_major)
					fields[i].matrix_layout = GLSL_MATRIX_LAYOUT_ROW_MAJOR;
				else if (qual->flags.q.column_major)
					fields[i].matrix_layout = GLSL_MATRIX_LAYOUT_COLUMN_MAJOR;

				assert(!is_interface || 
						fields[i].matrix_layout == GLSL_MATRIX_LAYOUT_ROW_MAJOR ||
					   	fields[i].matrix_layout == GLSL_MATRIX_LAYOUT_COLUMN_MAJOR);
			}

			i++;
		}
	}

	assert(i == decl_count);

	*fields_ret = fields;
	return decl_count;
}


ast_postprocess_traverser_visitor::~ast_postprocess_traverser_visitor()
{
	/* Free builtin functions, no need anymore */
	_mesa_glsl_release_builtin_functions();
}

void ast_postprocess_traverser_visitor::visit(ast_selection_statement *node)
{
	/* No enter node
	if (!this->enter(node))
		return;
	 */

	if (node->condition)
		node->condition->accept(this);
	++depth;
	if (node->then_statement){
		state->symbols->push_scope();
		node->then_statement->accept(this);
		state->symbols->pop_scope();
	}
	if (node->else_statement){
		state->symbols->push_scope();
		node->else_statement->accept(this);
		state->symbols->pop_scope();
	}
	--depth;

	this->leave(node);
}

bool ast_postprocess_traverser_visitor::enter(ast_declarator_list* node)
{
	assert(node->type);
	node->type->accept(this);

	const char* type_name;
	const glsl_type * decl_type = node->type->glsl_type(&type_name, this->state);
	assert(decl_type || !"Using of non-declared type");
	shader->symbols->add_type(type_name, decl_type);

	foreach_list_typed (ast_declaration, decl, link, &node->declarations) {
		if (decl->initializer && decl->initializer->oper == ast_aggregate)
			_mesa_ast_set_aggregate_type(decl_type, decl->initializer);
		if ((decl_type == NULL) || decl_type->is_void())
			continue;

		YYLTYPE loc = decl->get_location();
		const struct glsl_type *var_type;
		ir_variable *var;
		ShVariable *shvar;

		decl->accept(this);
		// Register variable
		variableQualifier qual = qualifierFromAst(&node->type->qualifier, false);
		variableVaryingModifier modifier = modifierFromAst(&node->type->qualifier);
		shvar = astToShVariable(decl, qual, modifier, decl_type, shader);
		var_type = process_array_type(&loc, decl_type, decl->array_specifier, state);
		var = new(shader) ir_variable(var_type, decl->identifier, ir_var_auto);
		apply_type_qualifier_to_variable(&node->type->qualifier, var, state, &loc, false);

		variables_id[var] = shvar->uniqueId;
		if (var->data.mode == ir_var_shader_in) {
			var->data.read_only = true;
			if (state->stage == MESA_SHADER_GEOMETRY)
				handle_geometry_shader_input_decl(state, loc, var);
			input_variables.push_tail(var);
		}

		if (!get_variable_being_redeclared(var, loc, state, false)) {
			validate_identifier(decl->identifier, loc, state);
			if (!state->symbols->add_variable(var))
				_mesa_glsl_error(&loc, state, "name `%s' already taken in the "
						"current scope", decl->identifier);
		}
	}

	return false;
}

// As in ast_to_hir
bool ast_postprocess_traverser_visitor::enter(ast_struct_specifier* node)
{
	YYLTYPE loc = node->get_location();
	if (state->language_version != 110 && state->struct_specifier_depth != 0)
		_mesa_glsl_error(&loc, state, "embedded structure declartions are not allowed");

	state->struct_specifier_depth++;

	glsl_struct_field *fields;
	unsigned decl_count = ast_process_structure_or_interface_block(this, state,
		   	&node->declarations, loc, &fields, false, GLSL_MATRIX_LAYOUT_INHERITED,
		   	false, ir_var_auto);

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

	return false;
}

// As in ast_to_hir
bool ast_postprocess_traverser_visitor::enter(ast_parameter_declarator *node)
{
	const struct glsl_type *type;
	const char *name = NULL;
	YYLTYPE loc = node->get_location();

	type = node->type->glsl_type(&name, state);
	shader->symbols->add_type(name, type);

	if (type == NULL)
		type = glsl_type::error_type;

	if (node->formal_parameter && (node->identifier == NULL))
		return false;

	node->is_void = false;
	if (type->is_void()) {
		node->is_void = true;
		return false;
	}

	type = process_array_type(&loc, type, node->array_specifier, state);
	if (!type->is_error() && type->is_unsized_array())
		type = glsl_type::error_type;

	if (type == glsl_type::error_type)
		_mesa_glsl_error(&loc, state, "Error while processing parameter.");

	variableQualifier qual = qualifierFromAst(&node->type->qualifier, true);
	variableVaryingModifier modifier = modifierFromAst(&node->type->qualifier);
	ShVariable* shvar = astToShVariable(node, qual, modifier, type, shader);

	if (!type->is_error() && type->is_unsized_array()) {
		_mesa_glsl_error(&loc, state, "arrays passed as parameters must have "
				"a declared size");
		type = glsl_type::error_type;
	}

	ir_variable *var = new(shader) ir_variable(type, node->identifier, ir_var_function_in);
	variables_id[var] = shvar->uniqueId;
	apply_type_qualifier_to_variable(&node->type->qualifier, var, state, &loc, true);
	if (!get_variable_being_redeclared(var, loc, state, false)) {
		validate_identifier(node->identifier, loc, state);
		if (!state->symbols->add_variable(var))
			_mesa_glsl_error(&loc, state, "name `%s' already taken in the "
							"current scope", node->identifier);
	}

	return false;
}

bool ast_postprocess_traverser_visitor::enter(ast_compound_statement* node)
{
	if (node->new_scope){
		state->symbols->push_scope();
		depth++;
	}

	foreach_list_typed (ast_node, ast, link, &node->statements) {
		ast->accept(this);
		node->debug_sideeffects |= ast->debug_sideeffects;
	}

	if (node->new_scope){
		state->symbols->pop_scope();
		depth--;
	}

	return false;
}

bool ast_postprocess_traverser_visitor::enter(ast_expression_statement* node)
{
	if (node->expression) {
		node->expression->accept(this);
		node->debug_sideeffects |= node->expression->debug_sideeffects;
	}

	return false;
}

bool ast_postprocess_traverser_visitor::enter(ast_function_definition *)
{
	state->symbols->push_scope();
	return true;
}

bool ast_postprocess_traverser_visitor::enter(ast_iteration_statement *node)
{
	// Push scope for loop
	if (node->mode != ast_iteration_statement::ast_do_while)
		state->symbols->push_scope();
	return true;
}

void ast_postprocess_traverser_visitor::leave(ast_expression* node)
{
	for (int i = 0; i < 3; ++i) {
		if (!node->subexpressions[i])
			continue;
		node->debug_sideeffects |= node->subexpressions[i]->debug_sideeffects;
	}

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
	case ast_pre_inc:
	case ast_pre_dec:
	case ast_post_inc:
	case ast_post_dec:
		node->debug_sideeffects |= ast_dbg_se_general;
		break;

	case ast_sequence:
	case ast_function_call:
	case ast_aggregate:
		assert(!"not implemented");
		break;

	case ast_identifier:
		// It may be deref or global variable
		if (node->debug_id < 0){
			ir_variable* var = state->symbols->get_variable(
					node->primary_expression.identifier);
			assert(var || !"Undeclared identifier");
			std::map<ir_variable*, int>::iterator it = variables_id.find(var);
			// If found - it is deref, unregistered global otherwise
			if (it != variables_id.end()){
				node->debug_id = it->second;
			} else {
				// Maybe I need to convert it to the ast_declaration and prepend the list?
				const glsl_type* type = var->type;
				int qual = qualifierFromIr(var);
				qual |= SH_BUILTIN;
				variableVaryingModifier modifier = modifierFromIr(var);
				ShVariable *shvar = astToShVariable(node, (variableQualifier) qual,
														modifier, type, shader);
				variables_id[var] = shvar->uniqueId;
				addShVariableCtx(&shader->globals, shvar, shvar->builtin, shader);
			}
		}
		break;
	case ast_field_selection: {
		// We need to know is it struct or swizzle
		// In mesa it obtained in some wierd way.
		exec_list instructions;
		ir_rvalue* op = node->subexpressions[0]->hir(&instructions, state);
		if (op->type->base_type == GLSL_TYPE_STRUCT
		              || op->type->base_type == GLSL_TYPE_INTERFACE) {
			node->debug_selection_type = ast_fst_struct;
		} else if (node->subexpressions[1] != NULL) {
			node->debug_selection_type = ast_fst_method;
			// This is method. Make function_expression built-in
			ast_function_expression* exp = node->subexpressions[1]->as_function_expression();
			assert(exp || !"Unexpected method type");
			exp->debug_builtin = true;
		} else if (op->type->is_vector() ||
	              (state->ARB_shading_language_420pack_enable &&
	               op->type->is_scalar())) {
			node->debug_selection_type = ast_fst_swizzle;
		} else {
			assert(!"wrong type for field selection");
		}
		break;
	}
	default:
		break;
	}

}

void ast_postprocess_traverser_visitor::leave(ast_expression_bin* node)
{
	node->debug_sideeffects |= node->subexpressions[0]->debug_sideeffects;
	node->debug_sideeffects |= node->subexpressions[1]->debug_sideeffects;
}

void ast_postprocess_traverser_visitor::leave(ast_function_expression* node)
{
	// Not sure about it
	foreach_list_const(n, &node->expressions) {
		ast_node *ast = exec_node_data(ast_node, n, link);
		node->debug_sideeffects |= ast->debug_sideeffects;
	}

	node->debug_builtin = false;
	if (node->is_constructor()) {
		node->debug_builtin = true;
		node->debug_void_builtin = false;
	} else {
		ast_expression* id = node->subexpressions[0];
		const char *func_name = id->primary_expression.identifier;
		if (!strcmp(func_name, EMIT_VERTEX_SIGNATURE))
			node->debug_sideeffects |= ast_dbg_se_emit_vertex;

		// Probably leak
		exec_list actual_parameters;
		exec_list instructions;
		process_parameters(&instructions, &actual_parameters, &node->expressions, state);

		/* Check the built-ins. */
		_mesa_glsl_initialize_builtin_functions();
		ir_function_signature* func = _mesa_glsl_find_builtin_function(state, func_name,
				&actual_parameters);
		if (func) {
			node->debug_builtin = true;
			node->debug_void_builtin = func->return_type->is_void();
		}
	}
}

void ast_postprocess_traverser_visitor::leave(ast_case_statement* node)
{
	foreach_list_typed (ast_node, ast, link, &node->stmts)
		node->debug_sideeffects |= ast->debug_sideeffects;
}

void ast_postprocess_traverser_visitor::leave(ast_case_statement_list* node)
{
	int index = DBG_BH_SWITCH_BRANCH_FIRST;
	foreach_list_typed (ast_node, ast, link, &node->cases) {
		ast_case_statement* stmt = ast->as_case_statement();
		if (stmt)
			stmt->debug_branch_index = index++;
	}

	if (index > DBG_BH_SWITCH_BRANCH_LAST)
		dbgPrint(DBGLVL_COMPILERINFO, "Only %i switch branches supported, but %i defined.\n",
				DBG_BH_SWITCH_BRANCH_LAST, index);
}

void ast_postprocess_traverser_visitor::leave(ast_switch_body* node)
{
	if (node->stmts) // Break affects only case
		foreach_list_typed (ast_node, ast, link, &node->stmts->cases)
			node->debug_sideeffects |= ast->debug_sideeffects & ~ast_dbg_se_break;
}

void ast_postprocess_traverser_visitor::leave(ast_selection_statement* node)
{
	if (node->condition)
		node->debug_sideeffects |= node->condition->debug_sideeffects;
	if (node->then_statement)
		node->debug_sideeffects |= node->then_statement->debug_sideeffects;
	if (node->else_statement)
		node->debug_sideeffects |= node->else_statement->debug_sideeffects;
}

void ast_postprocess_traverser_visitor::leave(ast_switch_statement* node)
{
	if (node->test_expression)
		node->debug_sideeffects |= node->test_expression->debug_sideeffects;
	if (node->body)
		node->debug_sideeffects |= node->body->debug_sideeffects;
}

void ast_postprocess_traverser_visitor::leave(ast_iteration_statement* node)
{
	if (node->init_statement)
		node->debug_sideeffects |= node->init_statement->debug_sideeffects;
	if (node->condition)
		node->debug_sideeffects |= node->condition->debug_sideeffects;
	if (node->body) // Break affects only loop
		node->debug_sideeffects |= node->body->debug_sideeffects & ~ast_dbg_se_break;
	if (node->rest_expression)
		node->debug_sideeffects |= node->rest_expression->debug_sideeffects;


	node->debug_sideeffects &= ~ast_dbg_se_break;

	// We pushed scope in enter()
	if (node->mode != ast_iteration_statement::ast_do_while)
		state->symbols->pop_scope();
}

void ast_postprocess_traverser_visitor::leave(ast_jump_statement* node)
{
	if (node->mode == ast_jump_statement::ast_discard)
		node->debug_sideeffects |= ast_dbg_se_discard;
	else if (node->mode == ast_jump_statement::ast_return)
		node->debug_sideeffects |= ast_dbg_se_return;
	else if (node->mode == ast_jump_statement::ast_break)
		node->debug_sideeffects |= ast_dbg_se_break;
}

void ast_postprocess_traverser_visitor::leave(ast_function_definition* node)
{
	node->debug_sideeffects |= ast_dbg_se_general;
	if (node->body) // Return takes place only inside function
		node->debug_sideeffects |= node->body->debug_sideeffects & ~ast_dbg_se_return;
	shader->symbols->add_function(node);
	state->symbols->pop_scope();
}

void ast_postprocess_traverser_visitor::leave(ast_gs_input_layout* node)
{
	YYLTYPE loc = node->get_location();
	state->gs_input_prim_type_specified = true;
	unsigned num_vertices = vertices_per_prim(node->prim_type);

	/* If any shader inputs occurred before this declaration and did not
	 * specify an array size, their size is determined now. */
	foreach_in_list(ir_instruction, ir, &input_variables) {
		ir_variable *var = ir->as_variable();
		if (var == NULL || var->data.mode != ir_var_shader_in)
			continue;

		if (var->type->is_unsized_array()) {
			if (var->data.max_array_access >= num_vertices)
				_mesa_glsl_error(&loc, state, "this geometry shader input layout implies %u"
						" vertices, but an access to element %u of input"
						" `%s' already exists", num_vertices, var->data.max_array_access,
						var->name);
			else
				var->type = glsl_type::get_array_instance(var->type->fields.array,
						num_vertices);
		}
	}
}
