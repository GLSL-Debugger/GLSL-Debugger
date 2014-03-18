/*
 * debugjump.h
 *
 *  Created on: 10.10.2013
 */

#ifndef __DEBUGJUMP_TRAVERSER_H_
#define __DEBUGJUMP_TRAVERSER_H_

#include "ShaderLang.h"
#include "glsl/ast_visitor.h"
#include "glslang/Interface/AstStack.h"
#include "glslang/Interface/ShaderHolder.h"

enum OTOperation {
    OTOpTargetUnset,         // Invalidate actual target
    OTOpTargetSet,           // Look for new target
    OTOpDone,                // Do no harm, i.e. don't change anything anymore
    OTOpFinished             // Reached end of program, stop debugging
};


class ast_debugjump_traverser_visitor : public ast_traverse_visitor {
public:
	ast_debugjump_traverser_visitor(DbgResult& _result) :
		finishedDbgFunction(false), shader(NULL), result(_result)
	{
		operation = OTOpTargetUnset;
		dbgBehaviour = DBG_BH_JUMPINTO;
	}

	virtual ~ast_debugjump_traverser_visitor()
	{
	}

	bool finished()
	{
		return operation == OTOpFinished;
	}

	void setUp(AstShader* sh, int behaviour)
	{
		shader = sh;
		dbgBehaviour = behaviour;
		result.passedDiscard = false;
		result.passedEmitVertex = false;
	}

	bool step(const char* name);
	void setGobalScope(exec_list*);
	void addShChangeables(ast_node* node);
	void checkReturns(ast_node*);
	void processDebugable(ast_node*);

	virtual void setTarget(class ast_expression*);
	virtual void setTarget(class ast_declaration*);
	virtual void setTarget(class ast_jump_statement*);

	virtual void visit(exec_list* list) { ast_traverse_visitor::visit(list); }
	virtual void visit(class ast_selection_statement*);
	virtual void visit(class ast_switch_statement*);
	virtual void visit(class ast_iteration_statement*);

	virtual bool enter(class ast_expression *);
	virtual bool enter(class ast_declaration *);
	virtual void leave(class ast_expression *);
	virtual void leave(class ast_function_expression *);
	virtual void leave(class ast_function_definition *);
	virtual void leave(class ast_jump_statement*);

	virtual bool enter(class ast_selection_statement *);
	virtual bool enter(class ast_switch_statement *);
	virtual bool enter(class ast_case_statement_list*);
	virtual bool enter(class ast_iteration_statement *);


	OTOperation operation;
    AstStack parseStack;
    int dbgBehaviour;
    bool finishedDbgFunction;
    AstShader* shader;
    DbgResult& result;
};


#endif /* __DEBUGJUMP_TRAVERSER_H_ */
