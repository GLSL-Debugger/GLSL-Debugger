/*
 * call.h
 *
 *  Created on: 21.02.2014
 */

#ifndef AST_CALL_VISITOR_H_
#define AST_CALL_VISITOR_H_

#include "glsl/ast_visitor.h"
#include "ast_nodes.h"

class CallAcceptor
{
public:
	CallAcceptor() { }
	virtual ~CallAcceptor() { }

	virtual bool accept(int, ast_node*, enum ast_node_type)
	{
		return true;
	}
};

class ast_call_visitor : public ast_traverse_visitor {
public:
	ast_call_visitor(CallAcceptor* _u) :
			unit(_u)
	{
	}

	virtual ~ast_call_visitor()
	{
	}

	virtual bool enter(class ast_node *node);
	virtual bool enter(class ast_expression *node);
	virtual bool enter(class ast_expression_bin *node);
	virtual bool enter(class ast_function_expression *node);
	virtual bool enter(class ast_array_specifier *node);
	virtual bool enter(class ast_aggregate_initializer *node);
	virtual bool enter(class ast_declaration *node);
	virtual bool enter(class ast_struct_specifier *node);
	virtual bool enter(class ast_type_specifier *node);
	virtual bool enter(class ast_fully_specified_type *node);
	virtual bool enter(class ast_parameter_declarator *node);
	virtual bool enter(class ast_function *node);
	virtual bool enter(class ast_case_label *node);
	virtual bool enter(class ast_case_statement *node);
	virtual bool enter(class ast_switch_body *node);
	virtual bool enter(class ast_selection_statement *node);
	virtual bool enter(class ast_switch_statement *node);
	virtual bool enter(class ast_iteration_statement *node);
	virtual bool enter(class ast_jump_statement *node);
	virtual bool enter(class ast_function_definition *node);
	virtual bool enter(class ast_interface_block *node);
	virtual bool enter(class ast_gs_input_layout *node);

	CallAcceptor* unit;
};


#endif /* AST_CALL_VISITOR_H_ */
