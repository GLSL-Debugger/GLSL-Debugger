/*
 * Shader.cpp
 *
 *  Created on: 20.09.2013
 */

#include "MShader.h"
#include "glsl/list.h"

bool dbg_state_not_match(exec_list* list, enum ir_dbg_state state)
{
	int skip_pair = -1;
	foreach_iter(exec_list_iterator, iter, *list) {
		ir_instruction * const inst = (ir_instruction *) iter.get();
		if (inst->ir_type == ir_type_dummy) {
			ir_dummy * const dm = inst->as_dummy();
			if (skip_pair < 0) {
				skip_pair = ir_dummy::pair_type(dm->dummy_type);
			} else if (skip_pair == dm->dummy_type) {
				skip_pair = -1;
				continue;
			}
		}
		if (skip_pair >= 0)
			continue;
		if (inst->debug_state != state)
			return true;
	}
	return false;
}

bool dbg_state_not_match(ir_dummy* first, enum ir_dbg_state state)
{
	if (!first || !first->next)
		return false;

	int end_token = ir_dummy::pair_type(first->dummy_type);
	if (end_token >= 0) {
		foreach_node_safe(node, first->next) {
			ir_instruction * const inst = (ir_instruction *) node;
			ir_dummy * const dm = inst->as_dummy();
			if (dm && end_token == dm->dummy_type)
				break;
			if (inst->debug_state != state)
				return true;
		}
	}

	return false;
}

