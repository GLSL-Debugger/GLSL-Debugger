/*
 * call.h
 *
 *  Created on: 21.02.2014
 */

#ifndef AST_CALL_VISITOR_H_
#define AST_CALL_VISITOR_H_

#include "glsl/ast_visitor.h"
#include "ast_nodes.h"
#include "units/Base.h"


class ast_call_visitor : public ast_traverse_visitor {
public:
	ast_call_visitor(BaseUnitTest* _u) :
			unit(_u)
	{
	}

	virtual ~ast_call_visitor()
	{
	}

	virtual bool traverse(class ast_node *node) { return unit->accept(depth, node, AST_NODE); }
	virtual bool traverse(class ast_expression *node) { return unit->accept(depth, node, AST_EXPRESSION); }
	virtual bool traverse(class ast_expression_bin *node) { return unit->accept(depth, node, AST_EXPRESSION_BIN); }
	virtual bool traverse(class ast_function_expression *node) { return unit->accept(depth, node, AST_FUNCTION_EXPRESSION); }
	virtual bool traverse(class ast_array_specifier *node) { return unit->accept(depth, node, AST_ARRAY_SPECIFIER); }
	virtual bool traverse(class ast_aggregate_initializer *node) { return unit->accept(depth, node, AST_AGGREGATE_INITIALIZER); }
	virtual bool traverse(class ast_declaration *node) { return unit->accept(depth, node, AST_DECLARATION); }
	virtual bool traverse(class ast_struct_specifier *node) { return unit->accept(depth, node, AST_STRUCT_SPECIFIER); }
	virtual bool traverse(class ast_type_specifier *node) { return unit->accept(depth, node, AST_TYPE_SPECIFIER); }
	virtual bool traverse(class ast_fully_specified_type *node) { return unit->accept(depth, node, AST_FULLY_SPECIFIED_TYPE); }
	virtual bool traverse(class ast_parameter_declarator *node) { return unit->accept(depth, node, AST_PARAMETER_DECLARATOR); }
	virtual bool traverse(class ast_function *node) { return unit->accept(depth, node, AST_FUNCTION); }
	virtual bool traverse(class ast_case_label *node) { return unit->accept(depth, node, AST_CASE_LABEL); }
	virtual bool traverse(class ast_case_statement *node) { return unit->accept(depth, node, AST_CASE_STATEMENT); }
	virtual bool traverse(class ast_switch_body *node) { return unit->accept(depth, node, AST_SWITCH_BODY); }
	virtual bool traverse(class ast_selection_statement *node) { return unit->accept(depth, node, AST_SELECTION_STATEMENT); }
	virtual bool traverse(class ast_switch_statement *node) { return unit->accept(depth, node, AST_SWITCH_STATEMENT); }
	virtual bool traverse(class ast_iteration_statement *node) { return unit->accept(depth, node, AST_ITERATION_STATEMENT); }
	virtual bool traverse(class ast_jump_statement *node) { return unit->accept(depth, node, AST_JUMP_STATEMENT); }
	virtual bool traverse(class ast_function_definition *node) { return unit->accept(depth, node, AST_FUNCTION_DEFINITION); }
	virtual bool traverse(class ast_interface_block *node) { return unit->accept(depth, node, AST_INTERFACE_BLOCK); }
	virtual bool traverse(class ast_gs_input_layout *node) { return unit->accept(depth, node, AST_GS_INPUT_LAYOUT); }

	BaseUnitTest* unit;
};


#endif /* AST_CALL_VISITOR_H_ */
