/*
 * Shader.cpp
 *
 *  Created on: 20.09.2013
 */

#include "MShader.h"
#include "glsl/list.h"
#include "BaseTypes.h"

std::string getCodeArraySize(const struct glsl_type* type)
{
	std::string out = "";
	if (type->is_array()) {
		char buffer[30];
		for (unsigned i = 0; i < type->length; ++i) {
			// FIXME? if (v->getType().getArraySize(i) != 0) {
			sprintf(buffer, "[%i]", type->fields.array[i].length);
			out += buffer;
		}
	}
	return out;
}

std::string getCodeString(ir_variable* var, bool withQualifier, EShLanguage l)
{
	if (var->invariant)
		return "invariant";

	std::string out = "";

	if (withQualifier) {
		if (var->interpolation)
			out += std::string(interpolation_string(var->interpolation)) + " ";
		if (var->centroid)
			out += "centroid ";
		if (var->mode > ir_var_auto && var->mode < ir_var_temporary)
			out += std::string(getQualifierString(var->mode, l)) + " ";
	}

	return out + getCodeString(var->type);
}

std::string getCodeString(const struct glsl_type* type)
{
	std::string out = "";
	char buf[100];

	if (type->is_array()) {
		/* ASSUMPTION: arrays are processed after calling this function!!!
		 *  p += sprintf(p, "array of ");
		 */
	}
	if (type->is_record()) {  // Struct
		// FIXME: How the unnamed struct named in mesa? Typename cannot be null
		out += std::string(type->name);
	} else if (type->is_matrix()) {
		/* For compability stick to older spec for square matrixes */
		if (type->vector_elements == type->matrix_columns)
			sprintf(buf, "mat%i", type->vector_elements);
		else
			sprintf(buf, "mat%ix%i", type->vector_elements,
					type->matrix_columns);
		out += std::string(buf);
	} else if (type->is_vector()) {
		if (type->base_type == GLSL_TYPE_INT)
			out += "i";
		else if (type->base_type == GLSL_TYPE_UINT)
			out += "u";
		else if (type->base_type == GLSL_TYPE_BOOL)
			out += "b";
		sprintf(buf, "vec%i", type->length);
		out += std::string(buf);
	} else {
		out += std::string(type->name);
	}

	return out;
}

//bool containsDiscard(ir_instruction* ir)
//{
//	switch (ir->ir_type) {
//	case ir_type_discard:
//		return true;
//		break;
//	case ir_type_assignment: {
//		ir_assignment* f = ir->as_assignment();
//		return containsDiscard(f->lhs) || containsDiscard(f->rhs);
//		break;
//	}
//	case ir_type_expression: {
//		ir_expression* e = ir->as_expression();
//		int operands = e->get_num_operands();
//		for (int i = 0; i < operands; ++i)
//			if (e->operands[i] && containsDiscard(e->operands[i]))
//				return true;
//		break;
//	}
//	case ir_type_call: {
//		ir_call* c = ir->as_call();
//		return containsDiscard(&(c->callee->body));
//		break;
//	}
//	default:
//		break;
//	}
//	return false;
//}
//
//bool containsDiscard(exec_list* list)
//{
//	foreach_iter( exec_list_iterator, iter, *list ) {
//		ir_instruction* ir = (ir_instruction*) iter.get();
//		if (containsDiscard(ir))
//			return true;
//	}
//	return false;
//}
//
//bool containsEmitVertex(ir_instruction* ir)
//{
//	switch (ir->ir_type) {
//	case ir_type_emit_vertex:
//		return true;
//		break;
//	case ir_type_assignment: {
//		ir_assignment* f = ir->as_assignment();
//		return containsEmitVertex(f->lhs) || containsEmitVertex(f->rhs);
//		break;
//	}
//	case ir_type_expression: {
//		ir_expression* e = ir->as_expression();
//		int operands = e->get_num_operands();
//		for (int i = 0; i < operands; ++i)
//			if (e->operands[i] && containsEmitVertex(e->operands[i]))
//				return true;
//		break;
//	}
//	case ir_type_call: {
//		ir_call* c = ir->as_call();
//		return containsEmitVertex(&(c->callee->body));
//		break;
//	}
//	default:
//		break;
//	}
//	return false;
//}
//
//bool containsEmitVertex(exec_list* list)
//{
//	foreach_iter( exec_list_iterator, iter, *list ) {
//		ir_instruction* ir = (ir_instruction*) iter.get();
//		if (containsEmitVertex(ir))
//			return true;
//	}
//	return false;
//}

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

void init_shader()
{
}

void clean_shader()
{
}


