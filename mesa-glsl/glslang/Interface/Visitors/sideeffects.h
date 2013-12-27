/*
 * sideeffects.h
 *
 *  Created on: 18.10.2013
 */

#ifndef SIDEEFFECTS_H_
#define SIDEEFFECTS_H_

#include "traverse.h"

class ir_sideeffects_traverser_visitor : public ir_traverse_visitor {
public:
	ir_sideeffects_traverser_visitor()
	{
		preVisit = false;
		postVisit = true;
	}

	virtual ~ir_sideeffects_traverser_visitor()
	{
	}

	virtual bool visitIr(ir_function_signature *ir);
	virtual bool visitIr(ir_function *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_call *ir);
	virtual bool visitIr(ir_discard *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);
	virtual bool visitIr(ir_emit_vertex *ir);
	virtual bool visitIr(ir_end_primitive *ir);

	int list_sideeffects(exec_list* instructions);
	int block_sideeffects(ir_dummy* first);

};


#endif /* SIDEEFFECTS_H_ */
