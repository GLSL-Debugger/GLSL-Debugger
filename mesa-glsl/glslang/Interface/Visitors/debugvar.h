/*
 * debugvar.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGVAR_H_
#define DEBUGVAR_H_

#include "traverse.h"
#include "ShaderLang.h"
#include "glslang/Interface/IRScope.h"


class ir_debugvar_traverser_visitor : public ir_traverse_visitor {
public:
	ir_debugvar_traverser_visitor(ShVariableList *_vl) : vl(_vl)
	{
	}

	virtual ~ir_debugvar_traverser_visitor()
	{
	}

	virtual bool visitIr(ir_variable *ir);
	virtual bool visitIr(ir_function *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_return *ir);
	virtual bool visitIr(ir_discard *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);
	virtual bool visitIr(ir_loop_jump *ir);

	ShVariableList *getVariableList() { return vl; }
    void addToScope(int id);
    void dumpScope(void);
    scopeList& getScope(void) { return scope; }
    scopeList* getCopyOfScope(void);
private:
    bool nameIsAlreadyInList(scopeList *l, const char *name);
    ShVariableList *vl;
    scopeList scope;
};



#endif /* DEBUGVAR_H_ */
