/*
 * ast_visitor.h
 *
 *  Created on: 05.02.2014
 */

#ifndef AST_leave_VISITOR_H_
#define AST_leave_VISITOR_H_

class exec_list;

class ast_traverse_visitor {
public:
	ast_traverse_visitor() :
			depth(0), flags(0)
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

	virtual bool enter(class ast_node *) { return false; }
	virtual bool enter(class ast_expression *) { return true; }
	virtual bool enter(class ast_expression_bin *) { return true; }
	virtual bool enter(class ast_function_expression *) { return true; }
	virtual bool enter(class ast_array_specifier *) { return true; }
	virtual bool enter(class ast_aggregate_initializer *) { return true; }
	virtual bool enter(class ast_compound_statement *) { return true; }
	virtual bool enter(class ast_declaration *) { return true; }
	virtual bool enter(class ast_struct_specifier *) { return true; }
	virtual bool enter(class ast_type_specifier *) { return true; }
	virtual bool enter(class ast_fully_specified_type *) { return true; }
	virtual bool enter(class ast_declarator_list *) { return true; }
	virtual bool enter(class ast_parameter_declarator *) { return true; }
	virtual bool enter(class ast_function *) { return true; }
	virtual bool enter(class ast_expression_statement *) { return true; }
	virtual bool enter(class ast_case_label *) { return true; }
	virtual bool enter(class ast_case_statement *) { return true; }
	virtual bool enter(class ast_case_statement_list *) { return true; }
	virtual bool enter(class ast_switch_body *) { return true; }
	virtual bool enter(class ast_selection_statement *) { return true; }
	virtual bool enter(class ast_switch_statement *) { return true; }
	virtual bool enter(class ast_iteration_statement *) { return true; }
	virtual bool enter(class ast_jump_statement *) { return true; }
	virtual bool enter(class ast_function_definition *) { return true; }
	virtual bool enter(class ast_interface_block *) { return true; }
	virtual bool enter(class ast_gs_input_layout *) { return true; }

	virtual void leave(class ast_node *) {}
	virtual void leave(class ast_expression *) {}
	virtual void leave(class ast_expression_bin *) {}
	virtual void leave(class ast_function_expression *) {}
	virtual void leave(class ast_array_specifier *) {}
	virtual void leave(class ast_aggregate_initializer *) {}
	virtual void leave(class ast_compound_statement *) {}
	virtual void leave(class ast_declaration *) {}
	virtual void leave(class ast_struct_specifier *) {}
	virtual void leave(class ast_type_specifier *) {}
	virtual void leave(class ast_fully_specified_type *) {}
	virtual void leave(class ast_declarator_list *) {}
	virtual void leave(class ast_parameter_declarator *) {}
	virtual void leave(class ast_function *) {}
	virtual void leave(class ast_expression_statement *) {}
	virtual void leave(class ast_case_label *) {}
	virtual void leave(class ast_case_statement *) {}
	virtual void leave(class ast_case_statement_list *) {}
	virtual void leave(class ast_switch_body *) {}
	virtual void leave(class ast_selection_statement *) {}
	virtual void leave(class ast_switch_statement *) {}
	virtual void leave(class ast_iteration_statement *) {}
	virtual void leave(class ast_jump_statement *) {}
	virtual void leave(class ast_function_definition *) {}
	virtual void leave(class ast_interface_block *) {}
	virtual void leave(class ast_gs_input_layout *) {}

	int depth;
	int flags;
};

#endif /* AST_leave_VISITOR_H_ */
