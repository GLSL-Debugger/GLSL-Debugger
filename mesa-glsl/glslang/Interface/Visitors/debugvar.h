/*
 * debugvar.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGVAR_H_
#define DEBUGVAR_H_

#include "ShaderLang.h"
#include "glslang/Interface/ShaderHolder.h"
#include "ast.h"


class ast_debugvar_traverser_visitor : public ast_traverse_visitor {
public:
	ast_debugvar_traverser_visitor(AstShader* _sh, ShVariableList* _vl);

	virtual bool enter(class ast_expression *);
	virtual bool enter(class ast_expression_bin *);
	virtual bool enter(class ast_function_expression *);
	virtual bool enter(class ast_aggregate_initializer *);
	virtual bool enter(class ast_compound_statement *);
	virtual bool enter(class ast_declaration *);
	virtual bool enter(class ast_parameter_declarator *);
	virtual bool enter(class ast_struct_specifier *);
	virtual bool enter(class ast_case_statement *);
	virtual bool enter(class ast_case_statement_list *);
	virtual bool enter(class ast_switch_body *);
	virtual bool enter(class ast_selection_statement *);
	virtual bool enter(class ast_switch_statement *);
	virtual bool enter(class ast_iteration_statement *);
	virtual bool enter(class ast_jump_statement *);
	virtual bool enter(class ast_function_definition *);

	virtual void leave(class ast_expression_statement *);

	void addToScope(ShVariable*);
	void dumpScope() { dumpScope(&scope); }
	void copyScopeTo(ast_node*);

	static void dumpScope(exec_list*);
private:
	ShVariableList *vl;
	exec_list scope;
	AstShader* shader;
};

#endif /* DEBUGVAR_H_ */
