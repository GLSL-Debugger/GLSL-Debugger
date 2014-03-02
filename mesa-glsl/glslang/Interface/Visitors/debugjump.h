/*
 * debugjump.h
 *
 *  Created on: 10.10.2013
 */

#ifndef __DEBUGJUMP_TRAVERSER_H_
#define __DEBUGJUMP_TRAVERSER_H_

#include "glsl/ir.h"
#include "glsl/ast_visitor.h"
#include "traverse.h"
#include "ShaderLang.h"
#include "glslang/Interface/AstStack.h"

enum OTOperation {
    OTOpTargetUnset,         // Invalidate actual target
    OTOpTargetSet,           // Look for new target
    OTOpDone,                // Do no harm, i.e. don't change anything anymore
    OTOpFinished             // Reached end of program, stop debugging
};


class ast_debugjump_traverser_visitor : public ast_traverse_visitor {
public:
	ast_debugjump_traverser_visitor(DbgResult& _result) :
		finishedDbgFunction(false), discardPassed(false), vertexEmited(false),
		shader(NULL), result(_result)
	{

		dbgBehaviour = OTOpTargetUnset;
	}

	virtual ~ast_debugjump_traverser_visitor()
	{
	}

	void setGobalScope(exec_list*);
	void processDebugable(ast_node*, OTOperation*);
	void addShChangeables(ast_node* node);
	void checkReturns(ast_node*);

	virtual void visit(exec_list* list) { ast_traverse_visitor::visit(list); }
	virtual void visit(class ast_selection_statement*);
	virtual void visit(class ast_iteration_statement*);

	virtual void leave(class ast_expression *);
	virtual void leave(class ast_expression_bin *);
	virtual void leave(class ast_function_expression *);
	virtual void leave(class ast_expression_statement *);
	virtual void leave(class ast_compound_statement *);
	virtual void leave(class ast_declaration*);
	virtual void leave(class ast_declarator_list *);
	virtual void leave(class ast_parameter_declarator *);
	virtual void leave(class ast_case_statement *);
	virtual void leave(class ast_case_statement_list *);
	virtual void leave(class ast_switch_body *);
	virtual void leave(class ast_switch_statement *);
	virtual void leave(class ast_function_definition *);
	virtual void leave(class ast_jump_statement*);

	virtual bool enter(class ast_selection_statement *);
	virtual bool enter(class ast_iteration_statement *);

	OTOperation operation;
    AstStack parseStack;
    int dbgBehaviour;
    bool finishedDbgFunction;
    bool discardPassed;
    bool vertexEmited;
    AstShader* shader;
    DbgResult& result;
};

class scopeList;


class ir_debugjump_traverser_visitor : public ir_traverse_visitor {
public:
	ir_debugjump_traverser_visitor(DbgResult& _result) :
		finishedDbgFunction(false), discardPassed(false), vertexEmited(false),
		result(_result)
	{
		dbgBehaviour = OTOpTargetUnset;
	}

	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_swizzle *ir);
	virtual bool visitIr(ir_assignment *ir);

	OTOperation operation;
    // Keeps track of function call order
    //ir_instruction *root;
    //IRStack parseStack;
    int dbgBehaviour;
    bool finishedDbgFunction;
    bool discardPassed;
    bool vertexEmited;
    DbgResult& result;
};


#endif /* __DEBUGJUMP_TRAVERSER_H_ */
