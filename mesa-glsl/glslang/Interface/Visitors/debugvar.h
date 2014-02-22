/*
 * debugvar.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGVAR_H_
#define DEBUGVAR_H_

#include "ShaderLang.h"
#include "glslang/Interface/ShaderHolder.h"
#include "glslang/Interface/ASTScope.h"
#include "ast.h"


class ast_debugvar_traverser_visitor : public ast_traverse_visitor {
public:
	ast_debugvar_traverser_visitor(AstShader* _sh, ShVariableList* _vl) :
			vl(_vl), shader(_sh)
	{
	}

	virtual bool traverse(class ast_expression *);
	virtual bool traverse(class ast_expression_bin *);
	virtual bool traverse(class ast_function_expression *);
	virtual bool traverse(class ast_aggregate_initializer *);
	virtual bool traverse(class ast_declaration *);
	virtual bool traverse(class ast_parameter_declarator *);
	virtual bool traverse(class ast_struct_specifier *);
	virtual bool traverse(class ast_case_statement *);
	virtual bool traverse(class ast_selection_statement *);
	virtual bool traverse(class ast_switch_statement *);
	virtual bool traverse(class ast_iteration_statement *);
	virtual bool traverse(class ast_jump_statement *);
	virtual bool traverse(class ast_function_definition *);

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
