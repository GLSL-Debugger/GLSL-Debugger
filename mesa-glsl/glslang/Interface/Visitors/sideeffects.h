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
	ir_sideeffects_traverser_visitor() :
		hasSideEffects(false)
	{
	}

	virtual ~ir_sideeffects_traverser_visitor()
	{
	}

	virtual bool visitIr(ir_function *)
	{
		hasSideEffects = true;
		return false;
	}

	virtual bool visitIr(ir_assignment *)
	{
		hasSideEffects = true;
		return false;
	}

	virtual bool visitIr(ir_call *ir)
	{
		ir->callee->accept(this);
		return true;
	}

	bool hasSideEffects;
};


#endif /* SIDEEFFECTS_H_ */
