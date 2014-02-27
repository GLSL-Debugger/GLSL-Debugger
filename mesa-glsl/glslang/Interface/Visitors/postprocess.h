/*
 * sideeffects.h
 *
 *  Created on: 18.10.2013
 */

#ifndef SIDEEFFECTS_H_
#define SIDEEFFECTS_H_

#include "ast_visitor.h"
#include "../ShaderHolder.h"
#include "traverse.h"
#include <map>

class ir_variable;


class ast_postprocess_traverser_visitor : public ast_traverse_visitor {
public:
	ast_postprocess_traverser_visitor(AstShader* _sh, struct _mesa_glsl_parse_state* _st) :
			shader(_sh), state(_st)
	{
		flags = traverse_postvisit;
	}

	virtual ~ast_postprocess_traverser_visitor()
	{
	}

	virtual void visit(exec_list *l) { ast_traverse_visitor::visit(l); }
	virtual void visit(ast_declarator_list *);
	virtual void visit(ast_struct_specifier *);
	virtual void visit(ast_parameter_declarator *);
	virtual void visit(ast_compound_statement *);
	virtual void visit(ast_expression_statement *);
	virtual void visit(ast_function_definition *);
	virtual void visit(ast_selection_statement *);
	virtual void visit(ast_iteration_statement *);

	virtual bool traverse(ast_expression *);
	virtual bool traverse(ast_expression_bin *);
	virtual bool traverse(ast_function_expression *);
	virtual bool traverse(ast_case_statement *);
	virtual bool traverse(ast_switch_body *);
	virtual bool traverse(ast_selection_statement *);
	virtual bool traverse(ast_switch_statement *);
	virtual bool traverse(ast_iteration_statement *);
	virtual bool traverse(ast_jump_statement *);
	virtual bool traverse(ast_function_definition *);
	virtual bool traverse(ast_gs_input_layout *);

protected:
	AstShader* shader;
	exec_list input_variables;
	struct _mesa_glsl_parse_state* state;
	std::map<ir_variable*,int> variables_id;
};


#endif /* SIDEEFFECTS_H_ */
