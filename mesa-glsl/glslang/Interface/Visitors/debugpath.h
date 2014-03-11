/*
 * debugpath.h
 *
 *  Created on: 02 марта 2014 г.
 */

#ifndef DEBUGPATH_H_
#define DEBUGPATH_H_

#include "glsl/ast_visitor.h"
#include "glslang/Interface/AstStack.h"
#include "ShaderLang.h"
#include "glslang/Interface/ShaderHolder.h"


enum DPOperation {
    DPOpPathClear,           // Clear all path nodes, but not targets
    DPOpPathBuild,           // Construct path from root to targets
    DPOpReset,               // Reconstruct initial debug state
};


class ast_debugpath_traverser_visitor : public ast_traverse_visitor {
public:
	ast_debugpath_traverser_visitor()
	{
		passedTarget = false;
		action = DPOpReset;
	}

	virtual ~ast_debugpath_traverser_visitor()
	{
	}

	void getPath(DbgRsScope&, AstShader*);
	void run(exec_list*, enum DPOperation);
	void processDebugable(ast_node*);

	virtual void leave(class ast_expression *);
	virtual void leave(class ast_expression_bin *);
	virtual void leave(class ast_function_expression *);
	virtual void leave(class ast_expression_statement *);
	virtual void leave(class ast_compound_statement *);
	virtual void leave(class ast_declaration*);
	virtual void leave(class ast_declarator_list *);
	virtual void leave(class ast_case_statement *);
	virtual void leave(class ast_case_statement_list *);
	virtual void leave(class ast_switch_body *);
	virtual void leave(class ast_switch_statement *);
	virtual void leave(class ast_selection_statement *);
	virtual void leave(class ast_iteration_statement *);
	virtual void leave(class ast_function_definition *);
	virtual void leave(class ast_jump_statement*);

protected:
	AstStack path;
	bool passedTarget;
	enum DPOperation action;
};



#endif /* DEBUGPATH_H_ */
