/*
 * sideeffects.h
 *
 *  Created on: 18.10.2013
 */

#ifndef SIDEEFFECTS_H_
#define SIDEEFFECTS_H_

#include "ast_visitor.h"
#include "traverse.h"

class ast_sideeffects_traverser_visitor : public ast_traverse_visitor {
public:
	ast_sideeffects_traverser_visitor(struct _mesa_glsl_parse_state* _st) :
			state(_st)
	{
		flags = traverse_postvisit;
	}

	virtual ~ir_sideeffects_traverser_visitor()
	{
	}

	virtual bool traverse(ir_function_signature *ir);
	virtual bool traverse(ir_function *ir);
	virtual bool traverse(ir_expression *ir);
	virtual bool traverse(ir_assignment *ir);
	virtual bool traverse(ir_call *ir);
	virtual bool traverse(ir_discard *ir);
	virtual bool traverse(ir_if *ir);
	virtual bool traverse(ir_loop *ir);
	virtual bool traverse(ir_emit_vertex *ir);
	virtual bool traverse(ir_end_primitive *ir);

protected:
	struct _mesa_glsl_parse_state* state;
};

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
