/*
 * Shader.cpp
 *
 *  Created on: 20.09.2013
 */

#include "MShader.h"
#include "glsl/list.h"
#include "glsl/glsl_symbol_table.h"


bool containsDiscard( ir_instruction* ir )
{
	switch( ir->ir_type ){
		case ir_type_discard:
			return true;
			break;
		case ir_type_assignment:
		{
			ir_assignment* f = ir->as_assignment();
			return containsDiscard(f->lhs) || containsDiscard(f->rhs);
			break;
		}
		case ir_type_expression:
		{
			ir_expression* e = ir->as_expression();
			int operands = e->get_num_operands();
			for( int i = 0; i < operands; ++i )
				if( e->operands[i] && containsDiscard(e->operands[i]) )
					return true;
			break;
		}
		case ir_type_call:
		{
			ir_call* c = ir->as_call();
			return containsDiscard( &(c->callee->body) );
			break;
		}
		default:
			break;
	}
	return false;
}

bool containsDiscard( exec_list* list )
{
	foreach_iter( exec_list_iterator, iter, *list ) {
		ir_instruction* ir = (ir_instruction*)iter.get();
		if( containsDiscard( ir ) )
			return true;
	}
	return false;
}
