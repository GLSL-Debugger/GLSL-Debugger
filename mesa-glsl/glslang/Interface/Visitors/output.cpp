/*
 * output.cpp
 *
 *  Created on: 10.03.2014.
 */

#include "output.h"
#include "glsl/ast.h"
#include "glslang/Interface/CodeTools.h"
#include "glslang/Interface/SymbolTable.h"
#include "glsldb/utils/dbgprint.h"

#include <assert.h>

static void print_variable(char **buffer, ast_node* node, const char* name, ShVariableList* vl)
{
	ralloc_asprintf_append(buffer, "%s", name);
	int postfix = node->debug_id;
	ShVariable* var = findShVariableFromId(vl, postfix);
	/* Add postfix to all non-builtin symbols due to !@$#^$% scope hiding */
	if (var && !var->builtin && var->qualifier != SH_VARYING_IN
				&& var->qualifier != SH_VARYING_OUT && var->qualifier != SH_UNIFORM
				&& var->qualifier != SH_ATTRIBUTE)
		ralloc_asprintf_append(buffer, "_%i", postfix);
}

void ast_output_traverser_visitor::dump()
{
	dbgPrint(DBGLVL_COMPILERINFO, "\n"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"
			"%s\n"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", buffer);
}


void ast_output_traverser_visitor::get_code(char** dst)
{
	*dst = strdup(buffer);
}

void ast_output_traverser_visitor::append_header()
{
	if(shader && shader->version)
		ralloc_asprintf_append(&buffer, "#version %i\n", shader->version);

	ralloc_asprintf_append(&buffer, "\n");

	if (shader->stage == MESA_SHADER_GEOMETRY){
		if (shader->gs_input_prim_type_specified) {
			output_qualifier(shader->qualifiers[SQ_GS_IN]);
			ralloc_asprintf_append(&buffer, "in;\n");
		}
		output_qualifier(shader->qualifiers[SQ_GS_OUT]);
		ralloc_asprintf_append(&buffer, "out;\n\n");
	}

	/* Add declaration of all neccessary types */
	if (cgOptions != DBG_CG_ORIGINAL_SRC)
		cg.addDeclaration(CG_TYPE_ALL, &buffer, mode);

	ralloc_asprintf_append(&buffer, "\n");
}


void ast_output_traverser_visitor::indent(void)
{
   for (int i = 0; i < depth; i++)
      ralloc_asprintf_append (&buffer, "  ");
}

void ast_output_traverser_visitor::visit(class ast_node*)
{
	ralloc_asprintf_append(&buffer, "unhandled node ");
}

void ast_output_traverser_visitor::visit(class ast_expression* node)
{
	if (node->debug_target() && cgOptions != DBG_CG_ORIGINAL_SRC) {
		dbgTargetProcessed = true;
		cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
		// FIXME: WAT?
		ralloc_asprintf_append(&buffer, ", ");
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
		node->subexpressions[0]->accept(this);
		ralloc_asprintf_append(&buffer, " %s ", node->operator_string(node->oper));
		node->subexpressions[1]->accept(this);
		break;

	case ast_field_selection:
		node->subexpressions[0]->accept(this);
		ralloc_asprintf_append(&buffer, ".");
		if (node->debug_selection_type == ast_fst_method)
			node->subexpressions[1]->accept(this);
		else
			ralloc_asprintf_append(&buffer, "%s", node->primary_expression.identifier);
		break;

	case ast_plus:
	case ast_neg:
	case ast_bit_not:
	case ast_logic_not:
	case ast_pre_inc:
	case ast_pre_dec:
		ralloc_asprintf_append(&buffer, "%s ", node->operator_string(node->oper));
		node->subexpressions[0]->accept(this);
		break;

	case ast_post_inc:
	case ast_post_dec:
		node->subexpressions[0]->accept(this);
		ralloc_asprintf_append(&buffer, "%s", node->operator_string(node->oper));
		break;

	case ast_conditional:
		assert(!"Not implemented");
//		node->subexpressions[0]->print();
//		printf("? ");
//		node->subexpressions[1]->print();
//		printf(": ");
//		node->subexpressions[2]->print();
		break;

	case ast_array_index:
		node->subexpressions[0]->accept(this);
		ralloc_asprintf_append(&buffer, "[");
		node->subexpressions[1]->accept(this);
		ralloc_asprintf_append(&buffer, "]");
		break;

	case ast_function_call:
		assert(!"Must be ast_function_expression");
		break;

	case ast_identifier:
		print_variable(&buffer, node, node->primary_expression.identifier, vl);
		break;

	case ast_int_constant:
		ralloc_asprintf_append(&buffer, "%d", node->primary_expression.int_constant);
		break;
	case ast_uint_constant:
		ralloc_asprintf_append(&buffer, "%u", node->primary_expression.uint_constant);
		break;
	case ast_float_constant:
		ralloc_asprintf_append(&buffer, "%f", node->primary_expression.float_constant);
		break;
	case ast_bool_constant:
		ralloc_asprintf_append(&buffer, "%s",
				node->primary_expression.bool_constant ? "true" : "false");
		break;

	case ast_sequence:
		output_sequence(&node->expressions, "(", ", ", ")");
		break;
	case ast_aggregate:
		output_sequence(&node->expressions, "{", ", ", "}");
		break;

	default:
		assert(0);
		break;
	}
}

void ast_output_traverser_visitor::visit(class ast_expression_bin* node)
{
	if (node->debug_target() && cgOptions != DBG_CG_ORIGINAL_SRC) {
		dbgTargetProcessed = true;
		cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
		// FIXME: WAT?
		ralloc_asprintf_append(&buffer, ", ");
	}

	node->subexpressions[0]->accept(this);
	ralloc_asprintf_append(&buffer, " %s ", node->operator_string(node->oper));
	node->subexpressions[1]->accept(this);
}

void ast_output_traverser_visitor::visit(class ast_function_expression* node)
{
	if (!enter(node))
		return;

	node->subexpressions[0]->accept(this);
	output_sequence(&node->expressions, "(", ", ", ")");
}

void ast_output_traverser_visitor::visit(class ast_array_specifier* node)
{
	if (node->is_unsized_array)
		ralloc_asprintf_append(&buffer, "[]");

	foreach_list_typed(ast_expression, expr, link, &node->array_dimensions)
		if (expr->oper == ast_int_constant)
			ralloc_asprintf_append(&buffer, "[%i]", expr->primary_expression.int_constant);
}

void ast_output_traverser_visitor::visit(class ast_aggregate_initializer* node)
{
	output_sequence(&node->expressions, "{", ", ", "}\n");
}

void ast_output_traverser_visitor::visit(class ast_compound_statement *node)
{
	ast_function_definition* main = shader->symbols->get_function(MAIN_FUNC_SIGNATURE);
	bool main_child = (main->body == node);
	bool skip_brakets = no_brakets;
	no_brakets = false;

	if (!skip_brakets)
		ralloc_asprintf_append(&buffer, "{\n");

	/* If in debug mode add initialization to main function */
	if (cgOptions != DBG_CG_ORIGINAL_SRC && main_child) {
		depth++;
		indent();
		switch (cgOptions) {
		case DBG_CG_CHANGEABLE:
		case DBG_CG_SELECTION_CONDITIONAL:
		case DBG_CG_LOOP_CONDITIONAL:
		case DBG_CG_COVERAGE:
			cg.addInitialization(CG_TYPE_RESULT, CG_INIT_BLACK, &buffer);
			break;
		case DBG_CG_VERTEX_COUNT:
		case DBG_CG_GEOMETRY_MAP:
			cg.addInitialization(CG_TYPE_RESULT, CG_INIT_GEOMAP, &buffer);
			break;
		case DBG_CG_GEOMETRY_CHANGEABLE:
			cg.addInitialization(CG_TYPE_RESULT, CG_INIT_WHITE, &buffer);
			break;
		default:
			break;
		}
		ralloc_asprintf_append(&buffer, ";\n");
		depth--;
	}

	output_sequence(&node->statements, "", "\n", "\n", true);

	/* And also add debug output at the end */
	if (cgOptions != DBG_CG_ORIGINAL_SRC && main_child) {
		if (mode == EShLangGeometry) {
			if (cgOptions == DBG_CG_CHANGEABLE
					|| cgOptions == DBG_CG_COVERAGE
					|| cgOptions == DBG_CG_SELECTION_CONDITIONAL
					|| cgOptions == DBG_CG_LOOP_CONDITIONAL
					|| cgOptions == DBG_CG_VERTEX_COUNT) {
				if (!cg.defined(CodeGen::GS_EMIT_VERTEX | CodeGen::GS_END_PRIMITIVE)) {
					indent();
					cg.addOutput(CG_TYPE_RESULT, &buffer, mode);
				}
			}
		} else if (mode == EShLangFragment) {
			indent();
			cg.addOutput(CG_TYPE_RESULT, &buffer, mode);
		}
	}

	if (!skip_brakets)
		ralloc_asprintf_append(&buffer, "}\n");

}

void ast_output_traverser_visitor::visit(class ast_declaration* node)
{
	print_variable(&buffer, node, node->identifier, vl);

	if (node->array_specifier)
		node->array_specifier->accept(this);

	if (node->initializer) {
		ralloc_asprintf_append(&buffer, " = ");
		node->initializer->accept(this);
		indent();
	}
}

void ast_output_traverser_visitor::visit(class ast_struct_specifier* node)
{
	ralloc_asprintf_append(&buffer, "struct %s ", node->name);
	output_sequence(&node->declarations, "{\n", "\n", "} ", true);
}

void ast_output_traverser_visitor::visit(class ast_type_specifier* node)
{
	if (node->structure)
		node->structure->accept(this);
	else
		ralloc_asprintf_append(&buffer, "%s ", node->type_name);

	if (node->array_specifier)
		node->array_specifier->accept(this);
}

void ast_output_traverser_visitor::visit(class ast_fully_specified_type* node)
{
	output_qualifier(&node->qualifier);
	if (node->specifier)
		node->specifier->accept(this);
}

void ast_output_traverser_visitor::visit(class ast_declarator_list* node)
{
	foreach_list_typed(ast_declaration, decl, link, &node->declarations) {
		if (decl->initializer && decl->initializer->debug_target()) {
			dbgTargetProcessed = true;
			cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
			ralloc_asprintf_append(&buffer, ";\n");
			indent();
		}
	}

	node->type->accept(this);
	depth++;
	output_sequence(&node->declarations, "", ", ", ";");
	depth--;
}

void ast_output_traverser_visitor::visit(class ast_parameter_declarator* node)
{
	node->type->accept(this);
	ralloc_asprintf_append(&buffer, " ");
	print_variable(&buffer, node, node->identifier, vl);
	if (node->array_specifier)
		node->array_specifier->accept(this);
}

void ast_output_traverser_visitor::visit(class ast_expression_statement* node)
{
	if (node->expression)
		node->expression->accept(this);
	ralloc_asprintf_append(&buffer, ";");
}

void ast_output_traverser_visitor::visit(class ast_case_label* node)
{
	indent();
	if (node->test_value) {
		ralloc_asprintf_append(&buffer, "case ");
		node->test_value->accept(this);
		ralloc_asprintf_append(&buffer, ": ");
	} else {
		ralloc_asprintf_append(&buffer, "default: ");
	}
}

void ast_output_traverser_visitor::visit(class ast_case_label_list* node)
{
	output_sequence(&node->labels, "", "\n", "", true);
}

void ast_output_traverser_visitor::visit(class ast_case_statement* node)
{
	node->labels->accept(this);
	output_sequence(&node->stmts, " {", "\n", "}", true);
}

void ast_output_traverser_visitor::visit(class ast_case_statement_list* node)
{
	output_sequence(&node->cases, "", "", "");
}

void ast_output_traverser_visitor::visit(class ast_switch_body* node)
{
	ralloc_asprintf_append(&buffer, "{\n");
	indent();
	if (node->stmts)
		node->stmts->accept(this);
	ralloc_asprintf_append(&buffer, "}\n");
}

void ast_output_traverser_visitor::visit(class ast_selection_statement* node)
{
	bool copyCondition = false;
	/* Add debug code */
	if (node->debug_target()) {
		this->dbgTargetProcessed = true;
		if (cgOptions == DBG_CG_COVERAGE || cgOptions == DBG_CG_CHANGEABLE
				|| cgOptions == DBG_CG_GEOMETRY_CHANGEABLE) {
			switch (node->debug_state_internal) {
			case ast_dbg_if_unset:
				assert(!"CodeGen - selection status is unset\n");
				break;
			case ast_dbg_if_init:
			case ast_dbg_if_condition:
				/* Add debug code prior to selection */
				cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
				ralloc_asprintf_append(&buffer, ";\n");
				indent();
				break;
			case ast_dbg_if_condition_passed:
			case ast_dbg_if_then:
			case ast_dbg_if_else:
				/* Add temporary register for condition */
				/* Fix: trigraph initialized condition register
				 *      even if trigraph is part of another
				 *      statement
				 *
				 cgInit(CG_TYPE_CONDITION, NULL, oit->vl, oit->language);
				 cgAddDeclaration(CG_TYPE_CONDITION, oit->debugProgram, oit->language);
				 outputIndentation(oit, oit->depth);
				 */
				if (node->condition)
					copyCondition = true;
				break;
			default:
				break;
			}
		}
	}

	ralloc_asprintf_append(&buffer, "if (");

	/* Add condition */
	if (copyCondition) {
		cg.addDbgCode(CG_TYPE_CONDITION, &buffer, cgOptions, 0);
		ralloc_asprintf_append(&buffer, " = (");
		node->condition->accept(this);
		ralloc_asprintf_append(&buffer, "), ");

		/* Add debug code */
		cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
		ralloc_asprintf_append(&buffer, ", ");
		cg.addDbgCode(CG_TYPE_CONDITION, &buffer, cgOptions, 0);
	} else
		if (node->condition)
			node->condition->accept(this);

	ralloc_asprintf_append(&buffer, ") {\n");
	selection_body(node, node->then_statement, true);
	ralloc_asprintf_append(&buffer, "}");

	if (!node->else_statement) {
		ralloc_asprintf_append(&buffer, " else {\n");
		selection_body(node, node->else_statement, false);
		ralloc_asprintf_append(&buffer, "}");
	}
}

void ast_output_traverser_visitor::visit(class ast_switch_statement* node)
{
	indent();
	ralloc_asprintf_append(&buffer, "switch ( ");
	node->test_expression->accept(this);
	ralloc_asprintf_append(&buffer, ")");
	node->body->accept(this);
}

void ast_output_traverser_visitor::visit(class ast_iteration_statement* node)
{
	if (this->cgOptions != DBG_CG_ORIGINAL_SRC && node->debug_state == ast_dbg_state_target)
		this->dbgTargetProcessed = true;

	loop_debug_prepare(node);

	if (node->mode == ast_iteration_statement::ast_for) {
		ralloc_asprintf_append(&buffer, "for (");
		if (node->init_statement)
			node->init_statement->accept(this);
		if (!node->init_statement || !node->init_statement->as_expression_statement())
			ralloc_asprintf_append(&buffer, ";");
		ralloc_asprintf_append(&buffer, " ");
		loop_debug_condition(node);
		ralloc_asprintf_append(&buffer, "; ");
		loop_debug_terminal(node);
		ralloc_asprintf_append(&buffer, ") ");
	} else if (node->mode == ast_iteration_statement::ast_while) {
		ralloc_asprintf_append(&buffer, "while (");
		loop_debug_condition(node);
		ralloc_asprintf_append(&buffer, ") ");
	} else {
		ralloc_asprintf_append(&buffer, "do ");
	}

	// Add one more depth level to insert debug iteration
	if (cgOptions != DBG_CG_ORIGINAL_SRC){
		ralloc_asprintf_append(&buffer, "{\n");
		no_brakets = true;
	}
	node->body->accept(this);
	loop_debug_end(node);
	if (cgOptions != DBG_CG_ORIGINAL_SRC) {
		indent();
		ralloc_asprintf_append(&buffer, "} ");
	}

	if (node->mode == ast_iteration_statement::ast_do_while) {
		ralloc_asprintf_append(&buffer, "while (");
		loop_debug_condition(node);
		ralloc_asprintf_append(&buffer, ")");
	}
}

void ast_output_traverser_visitor::visit(class ast_jump_statement* node)
{
	/* Add debug code before statement if not return */
	if (node->mode != ast_jump_statement::ast_jump_modes::ast_return &&
			node->debug_target() && cgOptions != DBG_CG_ORIGINAL_SRC) {
		dbgTargetProcessed = true;
		cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
		ralloc_asprintf_append (&buffer, ";");
	}

	switch (node->mode) {
	case ast_jump_statement::ast_jump_modes::ast_discard:
		if (!dbgTargetProcessed || cgOptions == DBG_CG_ORIGINAL_SRC)
			ralloc_asprintf_append (&buffer, "discard");
		break;
	case ast_jump_statement::ast_jump_modes::ast_break:
		ralloc_asprintf_append (&buffer, "break");
		break;
	case ast_jump_statement::ast_jump_modes::ast_continue:
		ralloc_asprintf_append (&buffer, "continue");
		break;
	case ast_jump_statement::ast_jump_modes::ast_return:
		char *tmpRegister = NULL;

		if (node->debug_target() && cgOptions != DBG_CG_ORIGINAL_SRC) {
			dbgTargetProcessed = true;
			if (node->opt_return_value) {
				/* Declaraion: type name = expression; */
				/* TODO: type */
//				oit->debugProgram +=
//						node->getExpression()->getType().getCodeString(true,
//								oit->language);
				ralloc_asprintf_append (&buffer, " ");

				cg.getNewName(&tmpRegister, "dbgBranch");
				ralloc_asprintf_append (&buffer, "%s = ", tmpRegister);
				node->opt_return_value->accept(this);
				ralloc_asprintf_append (&buffer, ";\n");
				indent();
			}

			cg.addDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, 0);
			ralloc_asprintf_append (&buffer, "; ");
		}

		/* If control flow ends program execution append code
		 * for emitting vertex and primitive.
		 */
		if ((mode == EShLangGeometry && (cgOptions == DBG_CG_CHANGEABLE
						|| cgOptions == DBG_CG_VERTEX_COUNT
						|| cgOptions == DBG_CG_COVERAGE
						|| cgOptions == DBG_CG_SELECTION_CONDITIONAL
						|| cgOptions == DBG_CG_LOOP_CONDITIONAL))
				|| ((mode == EShLangVertex || mode == EShLangFragment)
						&& cgOptions != DBG_CG_ORIGINAL_SRC)) {
			/* Node must be direct/indirect child of main function call */
			ast_function_definition* main = shader->symbols->get_function(MAIN_FUNC_SIGNATURE);
			assert(main || !"CodeGen - Cannot find main function");
			if (partof(node, main->body)) {
				cg.addOutput(CG_TYPE_RESULT, &buffer, mode);
				indent();
			}
		}

		ralloc_asprintf_append (&buffer, "return");

		if (node->opt_return_value) {
			if (node->debug_target() && cgOptions != DBG_CG_ORIGINAL_SRC)
				ralloc_asprintf_append (&buffer, " %s", tmpRegister);
			else
				node->opt_return_value->accept(this);
		}

		break;
	}
}

void ast_output_traverser_visitor::visit(class ast_function_definition* node)
{
	node->prototype->return_type->accept(this);
	bool is_main = !strcmp(node->prototype->identifier, MAIN_FUNC_SIGNATURE);

	const char* fname;
	// Use debug name if we print debuged function's clone
	if (this->cgOptions != DBG_CG_ORIGINAL_SRC && !is_main
			&& node->debug_state == ast_dbg_state_path
			&& node->debug_overwrite != ast_dbg_ow_original) {
		std::string mangled = getMangledName(node, shader);
		const char* mname = mangled.c_str();
		fname = cg.getDebugName(mname);
		/* Double function if this is on path.
		 * This is done to make sure that only the debugged path is calling
		 * a function with inserted debug code */
		DbgCgOptions option = cgOptions;
		cgOptions = DBG_CG_ORIGINAL_SRC;
		node->accept(this);
		cgOptions = option;
	} else {
		fname = node->prototype->identifier;
	}

	ralloc_asprintf_append(&buffer, " %s", fname);
	output_sequence(&node->prototype->parameters, "(", ", ", ")");
	if (!node->body || node->body->statements.is_empty()) {
		ralloc_asprintf_append(&buffer, ";\n");
		return;
	}

	ralloc_asprintf_append(&buffer, "\n");
	node->body->accept(this);
}

void ast_output_traverser_visitor::visit(class ast_interface_block* node)
{
	indent();
	output_qualifier(&node->layout);
	ralloc_asprintf_append(&buffer, "%s ", node->block_name);
	output_sequence(&node->declarations, "{", "\n", "}", true);

	if (node->instance_name)
		ralloc_asprintf_append(&buffer, " %s", node->instance_name);
	if (node->array_specifier)
		node->array_specifier->accept(this);

	ralloc_asprintf_append(&buffer, ";\n");
}

void ast_output_traverser_visitor::visit(class ast_gs_input_layout* node)
{
	(void)node;
}
