/*
 * scopestack.h
 *
 *  Created on: 27.10.2013
 */

#ifndef SCOPESTACK_H_
#define SCOPESTACK_H_

#include "glsl/ir.h"
#include "traverse.h"
#include "ShaderLang.h"


class ir_scopestack_traverse_visitor : public ir_traverse_visitor {
public:
	ir_scopestack_traverse_visitor(DbgResult& _result) :
		passedTarget(false), result(_result)
	{
	}

	virtual ~ir_scopestack_traverse_visitor()
	{
	}

	virtual bool visitIr(ir_variable *ir);
//	virtual bool visitIr(ir_function *ir);
	virtual bool visitIr(ir_function_signature *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_swizzle *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_call *ir);
	virtual bool visitIr(ir_return *ir);
	virtual bool visitIr(ir_discard *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);
	virtual bool visitIr(ir_loop_jump *ir);

	bool passedTarget;
	DbgResult& result;
};


#endif /* SCOPESTACK_H_ */
