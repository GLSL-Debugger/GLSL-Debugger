/*
 * traverse.h
 *
 *  Created on: 12.10.2013
 */

#ifndef __BASE_TRAVERSER_H
#define __BASE_TRAVERSER_H

#include <assert.h>
#include "glsl/ir_visitor.h"

class exec_list;


class ir_traverse_visitor : public ir_visitor {
public:
	ir_traverse_visitor() :
		depth(0), preVisit(true), postVisit(false), debugVisit(false),
		skipInternal(true)
	{
	}

	virtual ~ir_traverse_visitor()
	{
		/* empty */
	}


	virtual void visit(ir_variable *ir);
	virtual void visit(ir_function_signature *ir);
	virtual void visit(ir_function *ir);
	virtual void visit(ir_expression *ir);
	virtual void visit(ir_texture *ir);
	virtual void visit(ir_swizzle *ir);
	virtual void visit(ir_dereference_variable *ir);
	virtual void visit(ir_dereference_array *ir);
	virtual void visit(ir_dereference_record *ir);
	virtual void visit(ir_assignment *ir);
	virtual void visit(ir_constant *ir);
	virtual void visit(ir_call *ir);
	virtual void visit(ir_return *ir);
	virtual void visit(ir_discard *ir);
	virtual void visit(ir_if *ir);
	virtual void visit(ir_loop *ir);
	virtual void visit(ir_loop_jump *ir);
	virtual void visit(ir_list_dummy *ir);
	virtual void visit(exec_list* instructions);

	// Subclasses must implement this
	virtual bool visitIr(ir_variable *ir) { return ir; };
	virtual bool visitIr(ir_function_signature *ir) { return ir; };
	virtual bool visitIr(ir_function *ir) { return ir; };
	virtual bool visitIr(ir_expression *ir) { return ir; };
	virtual bool visitIr(ir_texture *ir) { return ir; };
	virtual bool visitIr(ir_swizzle *ir) { return ir; };
	virtual bool visitIr(ir_dereference_variable *ir) { return ir; };
	virtual bool visitIr(ir_dereference_array *ir) { return ir; };
	virtual bool visitIr(ir_dereference_record *ir) { return ir; };
	virtual bool visitIr(ir_assignment *ir) { return ir; };
	virtual bool visitIr(ir_constant *ir) { return ir; };
	virtual bool visitIr(ir_call *ir) { return ir; };
	virtual bool visitIr(ir_return *ir) { return ir; };
	virtual bool visitIr(ir_discard *ir) { return ir; };
	virtual bool visitIr(ir_if *ir) { return ir; };
	virtual bool visitIr(ir_loop *ir) { return ir; };
	virtual bool visitIr(ir_loop_jump *ir) { return ir; };

	// Dummy node
	virtual bool visitIr(ir_list_dummy* ir) { return ir; };

    int  depth;
    bool preVisit;
    bool postVisit;
    bool debugVisit;
    bool skipInternal;
};

#endif /* __BASE_TRAVERSER_H */
