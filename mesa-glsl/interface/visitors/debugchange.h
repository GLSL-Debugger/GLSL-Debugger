/*
 * debugchange.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGCHANGE_H_
#define DEBUGCHANGE_H_

#include "ShaderHolder.h"
#include "mesa/glsl/ast.h"
#include <set>


class ast_debugchange_traverser_visitor: public ast_traverse_visitor {
public:
	ast_debugchange_traverser_visitor(AstShader* _sh) :
			shader(_sh)
	{
	}

	virtual ~ast_debugchange_traverser_visitor()
	{
	}

	enum debugchange_ext_flags {
		ext_flag_active = 8
	};

	virtual bool enter(class ast_expression *);
	virtual bool enter(class ast_expression_bin *);
	virtual bool enter(class ast_function_expression *);
	virtual bool enter(class ast_expression_statement *);
	virtual bool enter(class ast_compound_statement *);
	virtual bool enter(class ast_declaration *);
	virtual bool enter(class ast_declarator_list *);
	virtual bool enter(class ast_parameter_declarator *);
	virtual bool enter(class ast_function *);
	virtual bool enter(class ast_case_statement *);
	virtual bool enter(class ast_case_statement_list *);
	virtual bool enter(class ast_switch_body *);
	virtual bool enter(class ast_selection_statement *);
	virtual bool enter(class ast_switch_statement *);
	virtual bool enter(class ast_iteration_statement *);
	virtual bool enter(class ast_jump_statement *);
	virtual bool enter(class ast_function_definition *);



	bool isActive(void) { return flags & ext_flag_active; }
	// active:  all coming symbols are being changed
	void activate(void) { flags |= ext_flag_active; }
	// passive: coming symbols act as input and are not changed
	void deactivate(void) { flags &= ~ext_flag_active; }

	void copyChangeables(ast_node* dst, ast_node* src);
protected:
	AstShader* shader;
	std::set<ast_node*> parsed_functions;
};

#endif /* DEBUGCHANGE_H_ */
