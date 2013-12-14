/*
 * debugjump.h
 *
 *  Created on: 10.10.2013
 */

#ifndef __DEBUGJUMP_TRAVERSER_H_
#define __DEBUGJUMP_TRAVERSER_H_

#include "glsl/ir.h"
#include "traverse.h"
#include "ShaderLang.h"
#include "glslang/Interface/IRStack.h"
#include "glslang/Interface/IRScope.h"


enum OTOperation {
    OTOpTargetUnset,         // Invalidate actual target
    OTOpTargetSet,           // Look for new target
    OTOpPathClear,           // Clear all path nodes, but not targets
    OTOpPathBuild,           // Construct path from root to targets
    OTOpReset,               // Reconstruct initial debug state
    OTOpDone,                // Do no harm, i.e. don't change anything anymore
    OTOpFinished             // Reached end of program, stop debugging
};


class ir_debugjump_traverser_visitor : public ir_traverse_visitor {
public:
	ir_debugjump_traverser_visitor(DbgResult& _result) :
		finishedDbgFunction(false), discardPassed(false), vertexEmited(false),
		result(_result)
	{
		dbgBehaviour = OTOpTargetUnset;
	}

	virtual ~ir_debugjump_traverser_visitor()
	{
	}

	void setGobalScope(scopeList *s);
	void processDebugable(ir_instruction *node, OTOperation *op);

	virtual bool visitIr(ir_variable *ir);
	virtual bool visitIr(ir_function_signature *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_texture *ir);
	virtual bool visitIr(ir_swizzle *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_constant *ir);
	virtual bool visitIr(ir_call *ir);
	virtual bool visitIr(ir_return *ir);
	virtual bool visitIr(ir_discard *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);
	virtual bool visitIr(ir_dummy* list);

	OTOperation operation;
    // Keeps track of function call order
    //ir_instruction *root;
    IRStack parseStack;
    int dbgBehaviour;
    bool finishedDbgFunction;
    bool discardPassed;
    bool vertexEmited;
    DbgResult& result;
};


#endif /* __DEBUGJUMP_TRAVERSER_H_ */
