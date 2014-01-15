/*
 * sideeffects.cpp
 *
 *  Created on: 27.12.2013
 */

#include "sideeffects.h"
#include "glsl/ir.h"
#include "glsl/list.h"
#include "glslang/Interface/CodeTools.h"


int ir_sideeffects_traverser_visitor::list_sideeffects(exec_list* instructions)
{
	int skip_pair = -1;
	int effects = ir_dbg_se_unset;
	if (!instructions)
		return effects;

	foreach_list(node, instructions) {
		ir_instruction * const inst = (ir_instruction *) node;
		if (!list_iter_check(inst, skip_pair))
			continue;
		effects |= inst->debug_sideeffects;
	}
	return effects;
}

int ir_sideeffects_traverser_visitor::block_sideeffects(ir_dummy* first)
{
	int effects = ir_dbg_se_unset;
	if (!first || !first->next)
		return effects;

	int end_token = ir_dummy::pair_type(first->dummy_type);

	// Skip non-blocks
	if (end_token < 0)
		return effects;

	foreach_node_safe(node, first->next){
		ir_instruction * const inst = (ir_instruction *)node;
		ir_dummy * const dm = inst->as_dummy();
		// End traverse
		if ( dm && end_token == dm->dummy_type )
			return effects;
		effects |= inst->debug_sideeffects;
	}

	return effects;
}


bool ir_sideeffects_traverser_visitor::visitIr(ir_function_signature* ir)
{
	ir->debug_sideeffects = list_sideeffects(&ir->body);
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_function* ir)
{
	ir->debug_sideeffects |= ir_dbg_se_general;
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_expression* ir)
{
	int count = ir->get_num_operands();
	ir->debug_sideeffects = ir_dbg_se_unset;
	for (int i = 0; i < count; ++i)
		ir->debug_sideeffects |= ir->operands[i]->debug_sideeffects;
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_assignment* ir)
{
	ir->debug_sideeffects = ir->lhs->debug_sideeffects |
							ir->rhs->debug_sideeffects;
	ir->debug_sideeffects |= ir_dbg_se_general;
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_call* ir)
{
	ir->debug_sideeffects = ir->callee->debug_sideeffects;
	ir->debug_sideeffects |= list_sideeffects(&ir->actual_parameters);
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_discard* ir)
{
	ir->debug_sideeffects |= ir_dbg_se_discard;
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_if* ir)
{
	ir->debug_sideeffects = ir->condition->debug_sideeffects;
	ir->debug_sideeffects_then = list_sideeffects(&ir->then_instructions);
	ir->debug_sideeffects_else = list_sideeffects(&ir->else_instructions);
	ir->debug_sideeffects |= ir->debug_sideeffects_then | ir->debug_sideeffects_else;
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_loop* ir)
{
	ir->debug_sideeffects = block_sideeffects(ir->debug_init) |
							block_sideeffects(ir->debug_check) |
							block_sideeffects(ir->debug_terminal) |
							list_sideeffects(&ir->body_instructions);
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_emit_vertex* ir)
{
	ir->debug_sideeffects |= ir_dbg_se_emit_vertex;
	return true;
}

bool ir_sideeffects_traverser_visitor::visitIr(ir_end_primitive* ir)
{
	return true;
}
