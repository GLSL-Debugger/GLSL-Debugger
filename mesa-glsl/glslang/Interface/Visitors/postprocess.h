/*
 * sideeffects.h
 *
 *  Created on: 18.10.2013
 */

#ifndef SIDEEFFECTS_H_
#define SIDEEFFECTS_H_

#include "ast_visitor.h"
#include "../ShaderHolder.h"
#include <map>

class ir_variable;


class ast_postprocess_traverser_visitor : public ast_traverse_visitor {
public:
	ast_postprocess_traverser_visitor(AstShader* _sh, struct _mesa_glsl_parse_state* _st) :
			shader(_sh), state(_st)
	{
	}

	virtual ~ast_postprocess_traverser_visitor()
	{
	}

	virtual void visit(exec_list* list) { ast_traverse_visitor::visit(list); }
	virtual void visit(ast_selection_statement *);

	virtual bool enter(ast_declarator_list *);
	virtual bool enter(ast_struct_specifier *);
	virtual bool enter(ast_parameter_declarator *);
	virtual bool enter(ast_compound_statement *);
	virtual bool enter(ast_expression_statement *);
	virtual bool enter(ast_function_definition *);
	virtual bool enter(ast_selection_statement *) { return true; }
	virtual bool enter(ast_iteration_statement *);

	virtual void leave(ast_expression *);
	virtual void leave(ast_expression_bin *);
	virtual void leave(ast_function_expression *);
	virtual void leave(ast_case_statement *);
	virtual void leave(ast_switch_body *);
	virtual void leave(ast_selection_statement *);
	virtual void leave(ast_switch_statement *);
	virtual void leave(ast_iteration_statement *);
	virtual void leave(ast_jump_statement *);
	virtual void leave(ast_function_definition *);
	virtual void leave(ast_gs_input_layout *);

protected:
	AstShader* shader;
	exec_list input_variables;
	struct _mesa_glsl_parse_state* state;
	std::map<ir_variable*,int> variables_id;
};


#endif /* SIDEEFFECTS_H_ */
