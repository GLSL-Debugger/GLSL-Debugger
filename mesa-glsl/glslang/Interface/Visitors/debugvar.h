/*
 * debugvar.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGVAR_H_
#define DEBUGVAR_H_

#include "ShaderLang.h"
#include "glslang/Interface/ASTScope.h"
#include "ast.h"


class ast_debugvar_traverser_visitor : public ast_traverse_visitor {
public:
	ast_debugvar_traverser_visitor(ShVariableList *_vl) : vl(_vl)
	{
		reset();
	}

	virtual bool traverse(class ast_expression *);
	virtual bool traverse(class ast_expression_bin *);
	virtual bool traverse(class ast_aggregate_initializer *);
	virtual bool traverse(class ast_declaration *);
	virtual bool traverse(class ast_parameter_declarator *);
	virtual bool traverse(class ast_case_statement *);
	virtual bool traverse(class ast_selection_statement *);
	virtual bool traverse(class ast_switch_statement *);
	virtual bool traverse(class ast_iteration_statement *);
	virtual bool traverse(class ast_jump_statement *);
	virtual bool traverse(class ast_function_definition *);

	ShVariableList *getVariableList() { return vl; }
	void addToScope(int id);
	void dumpScope(void);
	scopeList& getScope(void) { return scope; }
	scopeList* getCopyOfScope(void);
	void reset() { scope.clear(); }

private:
	bool nameIsAlreadyInList(scopeList *l, const char *name);
	ShVariableList *vl;
	scopeList scope;
};

#endif /* DEBUGVAR_H_ */
