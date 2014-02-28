/*
 * debugchange.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGCHANGE_H_
#define DEBUGCHANGE_H_

#include "../ShaderHolder.h"
#include <set>
#include "ast.h"


class ast_debugchange_traverser_visitor: public ast_traverse_visitor {
public:
	ast_debugchange_traverser_visitor(AstShader* _sh) :
			shader(_sh)
	{
		flags = traverse_previsit;
	}

	virtual ~ast_debugchange_traverser_visitor()
	{
	}

	enum debugchange_ext_flags {
		ext_flag_active = traverse_flag_last
	};

	virtual void visit(exec_list *l) { ast_traverse_visitor::visit(l); }
	virtual void visit(class ast_compound_statement *);
	virtual void visit(class ast_expression_statement *);

	virtual bool traverse(class ast_expression *);
	virtual bool traverse(class ast_expression_bin *);
	virtual bool traverse(class ast_function_expression *);
	virtual bool traverse(class ast_declaration *);
	virtual bool traverse(class ast_parameter_declarator *);
	virtual bool traverse(class ast_function *);
	virtual bool traverse(class ast_case_statement *);
	virtual bool traverse(class ast_switch_body *);
	virtual bool traverse(class ast_selection_statement *);
	virtual bool traverse(class ast_switch_statement *);
	virtual bool traverse(class ast_iteration_statement *);
	virtual bool traverse(class ast_jump_statement *);
	virtual bool traverse(class ast_function_definition *);


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
