/*
 * call.h
 *
 *  Created on: 21.02.2014
 */

#ifndef AST_CALL_VISITOR_H_
#define AST_CALL_VISITOR_H_

#include "glsl/ast_visitor.h"
#include "ast_nodes.h"

class BaseUnitTest;


class ast_call_visitor : public ast_traverse_visitor {
public:
	ast_call_visitor(BaseUnitTest* _u) :
			unit(_u)
	{
	}

	virtual ~ast_call_visitor()
	{
	}

	virtual bool traverse(class ast_node *node);
	virtual bool traverse(class ast_expression *node);
	virtual bool traverse(class ast_expression_bin *node);
	virtual bool traverse(class ast_function_expression *node);
	virtual bool traverse(class ast_array_specifier *node);
	virtual bool traverse(class ast_aggregate_initializer *node);
	virtual bool traverse(class ast_declaration *node);
	virtual bool traverse(class ast_struct_specifier *node);
	virtual bool traverse(class ast_type_specifier *node);
	virtual bool traverse(class ast_fully_specified_type *node);
	virtual bool traverse(class ast_parameter_declarator *node);
	virtual bool traverse(class ast_function *node);
	virtual bool traverse(class ast_case_label *node);
	virtual bool traverse(class ast_case_statement *node);
	virtual bool traverse(class ast_switch_body *node);
	virtual bool traverse(class ast_selection_statement *node);
	virtual bool traverse(class ast_switch_statement *node);
	virtual bool traverse(class ast_iteration_statement *node);
	virtual bool traverse(class ast_jump_statement *node);
	virtual bool traverse(class ast_function_definition *node);
	virtual bool traverse(class ast_interface_block *node);
	virtual bool traverse(class ast_gs_input_layout *node);

	BaseUnitTest* unit;
};


#endif /* AST_CALL_VISITOR_H_ */
