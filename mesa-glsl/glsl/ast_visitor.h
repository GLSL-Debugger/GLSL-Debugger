/*
 * ast_visitor.h
 *
 *  Created on: 05.02.2014
 */

#ifndef AST_TRAVERSE_VISITOR_H_
#define AST_TRAVERSE_VISITOR_H_

enum ast_traverse_flags {
	traverse_previsit = 1,
	traverse_postvisit = 2,
	traverse_debugvisit = 4
};

class exec_list;

class ast_traverse_visitor {
public:
	ast_traverse_visitor() :
			depth(0), flags(traverse_previsit), skipInternal(true)
	{
	}

	virtual ~ast_traverse_visitor()
	{
	}

	virtual void visit(exec_list *);
	virtual void visit(class ast_node *);
	virtual void visit(class ast_expression *);
	virtual void visit(class ast_expression_bin *);
	virtual void visit(class ast_function_expression *);
	virtual void visit(class ast_array_specifier *);
	virtual void visit(class ast_aggregate_initializer *);
	virtual void visit(class ast_compound_statement *);
	virtual void visit(class ast_declaration *);
	virtual void visit(class ast_struct_specifier *);
	virtual void visit(class ast_type_specifier *);
	virtual void visit(class ast_fully_specified_type *);
	virtual void visit(class ast_declarator_list *);
	virtual void visit(class ast_parameter_declarator *);
	virtual void visit(class ast_function *);
	virtual void visit(class ast_expression_statement *);
	virtual void visit(class ast_case_label *);
	virtual void visit(class ast_case_label_list *);
	virtual void visit(class ast_case_statement *);
	virtual void visit(class ast_case_statement_list *);
	virtual void visit(class ast_switch_body *);
	virtual void visit(class ast_selection_statement *);
	virtual void visit(class ast_switch_statement *);
	virtual void visit(class ast_iteration_statement *);
	virtual void visit(class ast_jump_statement *);
	virtual void visit(class ast_function_definition *);
	virtual void visit(class ast_interface_block *);
	virtual void visit(class ast_gs_input_layout *);

	virtual bool traverse(class ast_node *) { return false; }
	virtual bool traverse(class ast_expression *) { return true; }
	virtual bool traverse(class ast_expression_bin *) { return true; }
	virtual bool traverse(class ast_function_expression *) { return true; }
	virtual bool traverse(class ast_array_specifier *) { return true; }
	virtual bool traverse(class ast_aggregate_initializer *) { return true; }
	virtual bool traverse(class ast_declaration *) { return true; }
	virtual bool traverse(class ast_struct_specifier *) { return true; }
	virtual bool traverse(class ast_type_specifier *) { return true; }
	virtual bool traverse(class ast_fully_specified_type *) { return true; }
	virtual bool traverse(class ast_parameter_declarator *) { return true; }
	virtual bool traverse(class ast_function *) { return true; }
	virtual bool traverse(class ast_case_label *) { return true; }
	virtual bool traverse(class ast_case_statement *) { return true; }
	virtual bool traverse(class ast_switch_body *) { return true; }
	virtual bool traverse(class ast_selection_statement *) { return true; }
	virtual bool traverse(class ast_switch_statement *) { return true; }
	virtual bool traverse(class ast_iteration_statement *) { return true; }
	virtual bool traverse(class ast_jump_statement *) { return true; }
	virtual bool traverse(class ast_function_definition *) { return true; }
	virtual bool traverse(class ast_interface_block *) { return true; }
	virtual bool traverse(class ast_gs_input_layout *) { return true; }


	int depth;
	ast_traverse_flags flags;
	bool skipInternal;
};

#endif /* AST_TRAVERSE_VISITOR_H_ */
