/*
 * stacktraverser.h
 *
 *  Created on: 09.10.2013
 */

#ifndef STACKTRAVERSER_H_
#define STACKTRAVERSER_H_

#include "traverse.h"
#include "ShaderLang.h"
#include "glslang/Interface/CodeInsertion.h"


class ir_stack_traverser_visitor : public ir_traverse_visitor {
public:
	ir_stack_traverser_visitor(ShVariableList* list) : vl(list)
	{
		dbgStack.clear();
	}

	virtual ~ir_stack_traverser_visitor()
	{
	}

	virtual bool visitIr(ir_function_signature *ir);
	virtual bool visitIr(ir_function *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_call *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);

	ShVariableList *vl;
    IRGenStack dbgStack;
};


#endif /* STACKTRAVERSER_H_ */
