/*
 * position.cpp
 *
 *  Created on: 26.12.2013
 */

#include "position_output.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"
#include "mesa/main/macros.h"

#define TRUE_SIGN "*"
#define FALSE_SIGN " "


static void print_type(const glsl_type *t, int lvl)
{
	if (t->base_type == GLSL_TYPE_ARRAY) {
		dbgPrint(lvl, "(array ");
		print_type(t->fields.array, lvl);
		dbgPrint(lvl, " %u)", t->length);
	} else if ((t->base_type == GLSL_TYPE_STRUCT)
			&& (strncmp("gl_", t->name, 3) != 0)) {
		dbgPrint(lvl, "%s@%p", t->name, (void *) t);
	} else {
		dbgPrint(lvl, "%s", t->name);
	}
}

void ir_position_output_visitor::run(exec_list* instructions)
{
	int skip_pair = -1;
	bool first = true;
	foreach_list(node, instructions) {
		ir_instruction * const inst = (ir_instruction *) node;
		if (!list_iter_check(inst, skip_pair))
			continue;
		if (!first)
			dbgPrint(output_level, "\n");
		inst->accept(this);
		first = false;
	}
}

void ir_position_output_visitor::run(ir_dummy* first)
{
	if (!first || !first->next)
		return;

	int end_token = ir_dummy::pair_type(first->dummy_type);
	if (end_token < 0)
		return;

	foreach_node_safe(node, first->next) {
		ir_instruction * const inst = (ir_instruction *) node;
		ir_dummy * const dm = inst->as_dummy();
		if (dm && end_token == dm->dummy_type)
			return;
		inst->accept(this);
	}
}

void ir_position_output_visitor::print_header()
{
	dbgPrint(output_level,
			"       Discard   ───────┐\n"
			"     Emit vertex ─────┐│ │\n"
			"    Side effects ───┐│ │ │\n"
			"      Position     │ │ │ │    Instruction\n"
			"───────────────────┴─┴─┴─┴───────────────────────────\n");
}

void ir_position_output_visitor::print_range(ir_instruction* ir)
{
	if (!ir){
		for (int i = 0; i < 27; ++i)
			dbgPrint(output_level, "  ");
		return;
	}

	const char * const side_effects = (ir->debug_sideeffects & ir_dbg_se_general) ?
										TRUE_SIGN : FALSE_SIGN;
	const char * const vertex = (ir->debug_sideeffects & ir_dbg_se_emit_vertex) ?
										TRUE_SIGN : FALSE_SIGN;
	const char * const discard = (ir->debug_sideeffects & ir_dbg_se_discard) ?
										TRUE_SIGN : FALSE_SIGN;

	dbgPrint(output_level, "%s %s %s %s ", FormatSourceRange(ir->yy_location).c_str(),
						side_effects, vertex, discard);

	for (int i = 0; i < indentation; ++i)
		dbgPrint(output_level, "  ");
}

void ir_position_output_visitor::visit(ir_variable* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(declare ");

	const char * const cent = (ir->data.centroid) ? "centroid " : "";
	const char * const inv = (ir->data.invariant) ? "invariant " : "";
	const char * const mode[] = { "", "uniform ", "shader_in ", "shader_out ",
					"in ", "out ", "inout ", "const_in ", "sys ", "temporary " };
	STATIC_ASSERT(ARRAY_SIZE(mode) == ir_var_mode_count);
	const char * const interp[] = { "", "smooth", "flat", "noperspective" };
	STATIC_ASSERT(ARRAY_SIZE(interp) == INTERP_QUALIFIER_COUNT);

	dbgPrint(output_level, "(%s%s%s%s) ", cent, inv, mode[ir->data.mode], interp[ir->data.interpolation]);

	print_type(ir->type, output_level);
	dbgPrint(output_level, " %s)", unique_name(ir));
}

void ir_position_output_visitor::visit(ir_function_signature* ir)
{
	_mesa_symbol_table_push_scope(symbols);
	print_range(ir);
	dbgPrint(output_level, "(signature ");
	indentation++;
	print_type(ir->return_type, output_level);
	dbgPrint(output_level, " (parameters");
	if (!ir->parameters.is_empty()) {
		dbgPrint(output_level, "\n");
		indentation++;
		this->run(&ir->parameters);
		indentation--;
	}
	dbgPrint(output_level, ")(\n");
	indentation++;
	this->run(&ir->body);
	indentation--;
	dbgPrint(output_level, "))");
	indentation--;
	_mesa_symbol_table_pop_scope(symbols);
}

void ir_position_output_visitor::visit(ir_function* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(function %s\n", ir->name);
	indentation++;
	this->run(&ir->signatures);
	indentation--;
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_expression* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(expression ");
	print_type(ir->type, output_level);
	dbgPrint(output_level, " %s", ir->operator_string());
	indentation++;
	for (unsigned i = 0; i < ir->get_num_operands(); i++) {
		dbgPrint(output_level, "\n");
		ir->operands[i]->accept(this);
	}
	indentation--;
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_texture* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(%s ", ir->opcode_string());
	print_type(ir->type, output_level);
	dbgPrint(output_level, " ");
	ir->sampler->accept(this);
	dbgPrint(output_level, " ");

	if (ir->op != ir_txs && ir->op != ir_query_levels) {
		ir->coordinate->accept(this);
		dbgPrint(output_level, " ");
		if (ir->offset != NULL)
			ir->offset->accept(this);
		else
			dbgPrint(output_level, "0");
		dbgPrint(output_level, " ");
	}

	if (ir->op != ir_txf && ir->op != ir_txf_ms && ir->op != ir_txs
			&& ir->op != ir_tg4 && ir->op != ir_query_levels) {
		if (ir->projector)
			ir->projector->accept(this);
		else
			dbgPrint(output_level, "1");

		if (ir->shadow_comparitor) {
			dbgPrint(output_level, " ");
			ir->shadow_comparitor->accept(this);
		} else {
			dbgPrint(output_level, " ()");
		}
	}

	dbgPrint(output_level, " ");
	switch (ir->op) {
	case ir_tex:
	case ir_lod:
	case ir_query_levels:
		break;
	case ir_txb:
		ir->lod_info.bias->accept(this);
		break;
	case ir_txl:
	case ir_txf:
	case ir_txs:
		ir->lod_info.lod->accept(this);
		break;
	case ir_txf_ms:
		ir->lod_info.sample_index->accept(this);
		break;
	case ir_txd:
		dbgPrint(output_level, "(");
		ir->lod_info.grad.dPdx->accept(this);
		dbgPrint(output_level, " ");
		ir->lod_info.grad.dPdy->accept(this);
		dbgPrint(output_level, ")");
		break;
	case ir_tg4:
		ir->lod_info.component->accept(this);
		break;
	};
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_swizzle* ir)
{
	print_range(ir);
	const unsigned swiz[4] = {
		ir->mask.x,
		ir->mask.y,
		ir->mask.z,
		ir->mask.w,
	};

	dbgPrint(output_level, "(swiz ");
	for (unsigned i = 0; i < ir->mask.num_components; i++)
		dbgPrint(output_level, "%c", "xyzw"[swiz[i]]);
	dbgPrint(output_level, "\n");
	indentation++;
	ir->val->accept(this);
	indentation--;
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_dereference_variable* ir)
{
	print_range(ir);
	ir_variable *var = ir->variable_referenced();
	dbgPrint(output_level, "(var_ref %s)", unique_name(var));
}

void ir_position_output_visitor::visit(ir_dereference_array* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(array_ref\n");
	ir->array->accept(this);
	dbgPrint(output_level, "\n");
	ir->array_index->accept(this);
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_dereference_record* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(record_ref\n");
	ir->record->accept(this);
	dbgPrint(output_level, " %s)", ir->field);
}

void ir_position_output_visitor::visit(ir_assignment* ir)
{
	print_range(ir);

	char mask[5];
	unsigned j = 0;

	for (unsigned i = 0; i < 4; i++) {
		if ((ir->write_mask & (1 << i)) != 0) {
			mask[j] = "xyzw"[i];
			j++;
		}
	}
	mask[j] = '\0';

	dbgPrint(output_level, "(assign (%s)\n", mask);

	if (ir->condition)
		ir->condition->accept(this);
	indentation++;
	ir->lhs->accept(this);
	dbgPrint(output_level, "\n");
	ir->rhs->accept(this);
	indentation--;
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_constant* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(constant ");
	print_type(ir->type, output_level);
	dbgPrint(output_level, " (");

	if (ir->type->is_array()) {
		for (unsigned i = 0; i < ir->type->length; i++)
			ir->get_array_element(i)->accept(this);
	} else if (ir->type->is_record()) {
		ir_constant *value = (ir_constant *) ir->components.get_head();
		for (unsigned i = 0; i < ir->type->length; i++) {
			dbgPrint(output_level, "(%s ", ir->type->fields.structure[i].name);
			value->accept(this);
			dbgPrint(output_level, ")");
			value = (ir_constant *) value->next;
		}
	} else {
		for (unsigned i = 0; i < ir->type->components(); i++) {
			if (i != 0)
				dbgPrint(output_level, " ");
			switch (ir->type->base_type) {
			case GLSL_TYPE_UINT:
				dbgPrint(output_level, "%u", ir->value.u[i]);
				break;
			case GLSL_TYPE_INT:
				dbgPrint(output_level, "%d", ir->value.i[i]);
				break;
			case GLSL_TYPE_FLOAT:
				if (ir->value.f[i] == 0.0f)
					/* 0.0 == -0.0, so print with %f to get the proper sign. */
					dbgPrint(output_level, "%.1f", ir->value.f[i]);
				else if (abs(ir->value.f[i]) < 0.000001f)
					dbgPrint(output_level, "%a", ir->value.f[i]);
				else if (abs(ir->value.f[i]) > 1000000.0f)
					dbgPrint(output_level, "%e", ir->value.f[i]);
				else
					dbgPrint(output_level, "%f", ir->value.f[i]);
				break;
			case GLSL_TYPE_BOOL:
				dbgPrint(output_level, "%d", ir->value.b[i]);
				break;
			default:
				assert(0);
				break;
			}
		}
	}
	dbgPrint(output_level, "))");
}

void ir_position_output_visitor::visit(ir_call* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(call %s", ir->callee_name());
	if (ir->return_deref){
		indentation++;
		dbgPrint(output_level, "\n");
		ir->return_deref->accept(this);
		indentation--;
	}
	dbgPrint(output_level, " (\n");
	indentation++;
	this->run(&ir->actual_parameters);
	indentation--;
	dbgPrint(output_level, "))");
}

void ir_position_output_visitor::visit(ir_return* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(return");
	ir_rvalue * const value = ir->get_value();
	if (value) {
		dbgPrint(output_level, "\n");
		value->accept(this);
	}
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_discard* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(discard ");
	if (ir->condition != NULL) {
		dbgPrint(output_level, "\n");
		ir->condition->accept(this);
	}
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_if* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(if\n");
	indentation++;
	ir->condition->accept(this);
	indentation--;
	dbgPrint(output_level, "(then\n");
	indentation++;
	this->run(&ir->then_instructions);
	indentation--;
	dbgPrint(output_level, ")");

	if (!ir->else_instructions.is_empty()) {
		dbgPrint(output_level, "(else\n");
		indentation++;
		this->run(&ir->else_instructions);
		indentation--;
		indent();
		dbgPrint(output_level, "))");
	} else {
		dbgPrint(output_level, ")");
	}
}

void ir_position_output_visitor::visit(ir_loop* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(loop (");
	indentation++;
	if (!ir->debug_init->block_empty()) {
		dbgPrint(output_level, "\n");
		this->run(ir->debug_init);
	}
	dbgPrint(output_level, ") (");
	if (!ir->debug_check->block_empty()) {
		dbgPrint(output_level, "\n");
		this->run(ir->debug_check);
	}
	dbgPrint(output_level, ") (");
	if (!ir->debug_terminal->block_empty()){
		dbgPrint(output_level, "\n");
		this->run(ir->debug_terminal);
	}
	indentation--;
	dbgPrint(output_level, ") (\n");
	indentation++;
	this->run(&ir->body_instructions);
	indentation--;
	dbgPrint(output_level, "))");
}

void ir_position_output_visitor::visit(ir_loop_jump* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(loop %s)", ir->is_break() ? "break" : "continue");
}

void ir_position_output_visitor::visit(ir_typedecl_statement* ir)
{
	const glsl_type *const s = ir->type_decl;
	print_range(ir);
	dbgPrint(output_level, "(");
	print_type(s, output_level);
	for (unsigned j = 0; j < s->length; j++) {
		dbgPrint(output_level, " (");
		print_type(s->fields.structure[j].type, false);
		dbgPrint(output_level, "%s)", s->fields.structure[j].name);
	}
	dbgPrint(output_level, ")");
}

void ir_position_output_visitor::visit(ir_emit_vertex* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(emit-vertex)");
}

void ir_position_output_visitor::visit(ir_end_primitive* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(end-primitive)");
}

void ir_position_output_visitor::visit(ir_dummy* ir)
{
	print_range(ir);
	dbgPrint(output_level, "(dummy)");
}



